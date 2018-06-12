/*****************************************************************************
 * clock.c: Ouptut modules synchronisation clock
 *****************************************************************************
 * Copyright (C) 2018 VLC authors and VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <assert.h>
#include "clock.h"
#include "clock_internal.h"

struct vlc_clock_main_t
{
    vlc_mutex_t lock;
    vlc_cond_t cond;

    vlc_clock_t * master;

    /* FIXME do we need to keep the slaves?*/
    vlc_clock_t ** slaves;
    int nslaves;

    /**
     * Linear function
     * system = pts * coeff / rate + offset
     */
    clock_point_t last;
    average_t coeff_avg; /* Moving average to smooth out the instant coeff */
    float coeff;
    float rate;
    vlc_tick_t offset;
    vlc_tick_t delay;
    vlc_tick_t reset_date;

    vlc_tick_t pause_date;

    clock_point_t wait_sync_ref; /* When the master */
    vlc_tick_t dejitter; /* Delay used to absorb the clock jitter */
    bool abort;
};

struct vlc_clock_t
{
    vlc_tick_t (*update)(vlc_clock_t * clock, vlc_tick_t system_now, vlc_tick_t pts,
                      float rate);
    void (*reset)(vlc_clock_t * clock);
    void (*pause)(vlc_clock_t * clock, vlc_tick_t system_now, bool paused);
    vlc_tick_t (*set_delay)(vlc_clock_t * clock, vlc_tick_t delay);
    void (*set_dejitter)(vlc_clock_t * clock, vlc_tick_t delay, int cr_avg);
    vlc_tick_t (*to_system_locked)(vlc_clock_t * clock, vlc_tick_t system_now,
                                vlc_tick_t pts);

    vlc_clock_main_t * owner;
    vlc_tick_t delay;
};

static vlc_tick_t main_system_to_stream(vlc_clock_main_t * main_clock,
                                       vlc_tick_t system)
{
    if (unlikely(main_clock->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (vlc_tick_t) ((system - main_clock->offset) * main_clock->rate / main_clock->coeff);
}

static vlc_tick_t main_stream_to_system(vlc_clock_main_t * main_clock,
                                       vlc_tick_t pts)
{
    if (unlikely(main_clock->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (vlc_tick_t) (pts * main_clock->coeff / main_clock->rate + main_clock->offset);
}

static void vlc_clock_main_reset(vlc_clock_main_t * main_clock)
{
    AvgReset(&main_clock->coeff_avg);
    main_clock->coeff = 1.0f;
    main_clock->rate = 1.0f;
    main_clock->offset = VLC_TS_INVALID;

    main_clock->wait_sync_ref =
        main_clock->last = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);
    vlc_cond_broadcast(&main_clock->cond);
}

static vlc_tick_t vlc_clock_master_update(vlc_clock_t * clock, vlc_tick_t system_now,
                                       vlc_tick_t pts, float rate)
{
    vlc_clock_main_t * main_clock = clock->owner;

    vlc_mutex_lock(&main_clock->lock);
    assert(main_clock->pause_date == VLC_TS_INVALID);

    if (main_clock->reset_date != VLC_TS_INVALID)
    {
        if (system_now < main_clock->reset_date)
        {
            vlc_mutex_unlock(&main_clock->lock);
            return 0;
        }
        main_clock->reset_date = VLC_TS_INVALID;
        vlc_clock_main_reset(main_clock);
    }

    if (unlikely(pts == VLC_TS_INVALID || system_now == VLC_TS_INVALID))
    {
        vlc_mutex_unlock(&main_clock->lock);
        return VLC_TS_INVALID;
    }

    if (main_clock->offset != VLC_TS_INVALID && pts != main_clock->last.stream)
    {
        /* We have a reference so we can update coeff */
        float instant_coeff =
            ((float) (system_now - main_clock->last.system))/(pts - main_clock->last.stream);
        if (rate == main_clock->rate)
        {
          instant_coeff *= rate;
          AvgUpdate(&main_clock->coeff_avg, instant_coeff);
          main_clock->coeff = AvgGet(&main_clock->coeff_avg);
        }
    }
    else
        main_clock->wait_sync_ref =
            clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);

    main_clock->offset = system_now - pts * main_clock->coeff / rate;

    if (pts != VLC_TS_INVALID && system_now != VLC_TS_INVALID)
        main_clock->last = clock_point_Create(pts, system_now);

    main_clock->rate = rate;
    vlc_cond_broadcast(&main_clock->cond);
    vlc_mutex_unlock(&main_clock->lock);
    return 0;
}

static void vlc_clock_master_reset(vlc_clock_t * clock)
{
    vlc_clock_main_t * main_clock = clock->owner;

    vlc_mutex_lock(&main_clock->lock);
    vlc_clock_main_reset(main_clock);
    vlc_mutex_unlock(&main_clock->lock);
}

static void vlc_clock_master_pause(vlc_clock_t * clock, vlc_tick_t now, bool paused)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    assert(paused == (main_clock->pause_date == VLC_TS_INVALID));

    if (paused)
        main_clock->pause_date = now;
    else
    {
        /**
         * Only apply a delay if the clock has a reference point to avoid
         * messing up the timings if the stream was paused then seeked
         */
        if (main_clock->offset != VLC_TS_INVALID ||
            (main_clock->wait_sync_ref.stream != VLC_TS_INVALID ||
             main_clock->wait_sync_ref.system != VLC_TS_INVALID))
        {
            const vlc_tick_t delay = now - main_clock->pause_date;
            main_clock->last.system += delay;
            main_clock->offset += delay;
        }
        main_clock->pause_date = VLC_TS_INVALID;
        vlc_cond_broadcast(&main_clock->cond);
    }
    vlc_mutex_unlock(&main_clock->lock);
}

static vlc_tick_t vlc_clock_master_set_delay(vlc_clock_t * clock, vlc_tick_t delay)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);

    vlc_tick_t delta = delay - main_clock->delay;
    main_clock->delay = delay;
    if (delta > 0)
    {
        if (main_clock->reset_date == VLC_TS_INVALID)
            main_clock->reset_date = vlc_tick_now() + delta;
        else
            main_clock->reset_date += delta;
    }
    else
        delta = 0;

    vlc_cond_broadcast(&main_clock->cond);
    vlc_mutex_unlock(&main_clock->lock);
    return delta;
}

static float vlc_clock_get_rate(vlc_clock_t * clock)
{
    float rate;
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);

    rate = main_clock->rate;

    vlc_mutex_unlock(&main_clock->lock);

    return rate;
}

static vlc_tick_t vlc_clock_to_stream(vlc_clock_t * clock, vlc_tick_t system)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_tick_t pts;

    vlc_mutex_lock(&main_clock->lock);
    pts = main_system_to_stream(main_clock, system);
    vlc_mutex_unlock(&main_clock->lock);
    return pts;
}

static void vlc_clock_master_set_dejitter(vlc_clock_t * clock, vlc_tick_t delay, int cr_avg)
{
    VLC_UNUSED(cr_avg);
    vlc_clock_main_t * main_clock = clock->owner;

    vlc_mutex_lock(&main_clock->lock);
    main_clock->dejitter = delay;
    vlc_mutex_unlock(&main_clock->lock);
}

static vlc_tick_t vlc_clock_get_dejitter(vlc_clock_t * clock)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_tick_t dejitter;

    vlc_mutex_lock(&main_clock->lock);
    dejitter = main_clock->dejitter;
    vlc_mutex_unlock(&main_clock->lock);
    return dejitter;
}

static vlc_tick_t vlc_clock_main_to_system_locked(vlc_clock_main_t * main_clock,
                                               vlc_tick_t now, vlc_tick_t pts)
{
    vlc_tick_t system = main_stream_to_system(main_clock, pts);
    if (system == VLC_TS_INVALID)
    {
        /**
         * We don't have a master sync point, let's fallback to a monotonic
         * ref point
         */
        if (main_clock->wait_sync_ref.stream == VLC_TS_INVALID ||
            main_clock->wait_sync_ref.system == VLC_TS_INVALID)
        {
            main_clock->wait_sync_ref =
                clock_point_Create(pts, now + main_clock->dejitter);
        }
        system = (pts - main_clock->wait_sync_ref.stream) / main_clock->rate;
        system += main_clock->wait_sync_ref.system;
    }
    return system;
}

static vlc_tick_t vlc_clock_slave_to_system_locked(vlc_clock_t * clock, vlc_tick_t now,
                                                vlc_tick_t pts)
{
    vlc_clock_main_t * main_clock = clock->owner;
    if (main_clock->pause_date != VLC_TS_INVALID)
        return INT64_MAX;

    return vlc_clock_main_to_system_locked(main_clock, now, pts)
         + clock->delay - main_clock->delay;
}

static vlc_tick_t vlc_clock_master_to_system_locked(vlc_clock_t * clock, vlc_tick_t now,
                                                 vlc_tick_t pts)
{
    vlc_clock_main_t * main_clock = clock->owner;
    assert(main_clock->pause_date == VLC_TS_INVALID);
    return vlc_clock_main_to_system_locked(main_clock, now, pts);
}

static vlc_tick_t vlc_clock_slave_update(vlc_clock_t * clock, vlc_tick_t system_now,
                                      vlc_tick_t pts, float rate)
{
    VLC_UNUSED(rate);
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    if (!main_clock->master && main_clock->rate != rate)
        main_clock->rate = rate;

    vlc_tick_t computed = clock->to_system_locked(clock, system_now, pts);
    vlc_mutex_unlock(&main_clock->lock);
    return (computed != INT64_MAX)?(computed - system_now):VLC_TS_INVALID;
}

static void vlc_clock_slave_reset(vlc_clock_t * clock)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    main_clock->wait_sync_ref = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);
    vlc_mutex_unlock(&main_clock->lock);
}

static void vlc_clock_slave_pause(vlc_clock_t * clock, vlc_tick_t now, bool paused)
{
    VLC_UNUSED(clock);
    VLC_UNUSED(paused);
    VLC_UNUSED(now);
}

static vlc_tick_t vlc_clock_slave_set_delay(vlc_clock_t * clock, vlc_tick_t delay)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);

    clock->delay = delay;

    vlc_cond_broadcast(&main_clock->cond);
    vlc_mutex_unlock(&main_clock->lock);
    return 0;
}

int vlc_clock_Wait(vlc_clock_t * clock, vlc_tick_t pts, vlc_tick_t max_duration)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    vlc_tick_t now = vlc_tick_now();
    vlc_tick_t max_deadline = max_duration > 0 ? now + max_duration : INT64_MAX;
    while (!main_clock->abort)
    {
        vlc_tick_t deadline;
        if (main_clock->pause_date != VLC_TS_INVALID)
            deadline = INT64_MAX;
        else
            deadline = clock->to_system_locked(clock, now, pts);
        deadline = __MIN(deadline, max_deadline);

        if (vlc_cond_timedwait(&main_clock->cond, &main_clock->lock, deadline))
        {
            vlc_mutex_unlock(&main_clock->lock);
            return 0;
        }
    }
    vlc_mutex_unlock(&main_clock->lock);
    return 1;
}

static void vlc_clock_slave_set_dejitter(vlc_clock_t * clock, vlc_tick_t delay, int cr_avg)
{
    VLC_UNUSED(clock);
    VLC_UNUSED(delay);
    VLC_UNUSED(cr_avg);
}


vlc_clock_main_t * vlc_clock_main_New(void)
{
    vlc_clock_main_t * main_clock = malloc(sizeof(vlc_clock_main_t));

    if (main_clock == NULL)
        return NULL;

    vlc_mutex_init(&main_clock->lock);
    vlc_cond_init(&main_clock->cond);
    main_clock->master = NULL;

    TAB_INIT(main_clock->nslaves, main_clock->slaves);
    main_clock->coeff = 1.0f;
    main_clock->rate = 1.0f;
    main_clock->offset = VLC_TS_INVALID;
    main_clock->reset_date = VLC_TS_INVALID;
    main_clock->delay = 0;

    main_clock->wait_sync_ref =
        main_clock->last = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);

    main_clock->pause_date = VLC_TS_INVALID;
    main_clock->dejitter = 200000;
    main_clock->abort = false;

    AvgInit(&main_clock->coeff_avg, 10);

    return main_clock;
}

void vlc_clock_main_Abort(vlc_clock_main_t * main_clock)
{
    vlc_mutex_lock(&main_clock->lock);
    main_clock->abort = true;
    vlc_cond_broadcast(&main_clock->cond);

    vlc_mutex_unlock(&main_clock->lock);
}

void vlc_clock_main_Reset(vlc_clock_main_t * main_clock)
{
    vlc_mutex_lock(&main_clock->lock);
    vlc_clock_main_reset(main_clock);
    vlc_mutex_unlock(&main_clock->lock);
}

void vlc_clock_main_Delete(vlc_clock_main_t * main_clock)
{
    while (main_clock->nslaves != 0)
        vlc_clock_Delete(main_clock->slaves[0]);

    if (main_clock->master != NULL)
        vlc_clock_Delete(main_clock->master);

    TAB_CLEAN(main_clock->nslaves, main_clock->slaves);
    vlc_mutex_destroy(&main_clock->lock);
    vlc_cond_destroy(&main_clock->cond);
    free(main_clock);
}

vlc_tick_t vlc_clock_Update(vlc_clock_t * clock, vlc_tick_t system_now, vlc_tick_t pts,
                         float rate)
{
    return clock->update(clock, system_now, pts, rate);
}

void vlc_clock_Reset(vlc_clock_t * clock)
{
    clock->reset(clock);
}

void vlc_clock_ChangePause(vlc_clock_t * clock, vlc_tick_t system_now, bool paused)
{
    clock->pause(clock, system_now, paused);
}

vlc_tick_t vlc_clock_SetDelay(vlc_clock_t * clock, vlc_tick_t delay)
{
    return clock->set_delay(clock, delay);
}

float vlc_clock_GetRate(vlc_clock_t * clock)
{
    return vlc_clock_get_rate(clock);
}

vlc_tick_t vlc_clock_ConvertToSystem(vlc_clock_t * clock, vlc_tick_t system_now,
                                  vlc_tick_t pts)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    vlc_tick_t system = clock->to_system_locked(clock, system_now, pts);
    vlc_mutex_unlock(&main_clock->lock);
    return system;
}

void vlc_clock_ConvertArrayToSystem(vlc_clock_t * clock, vlc_tick_t system_now,
                                    vlc_tick_t *pts_array, size_t pts_count)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    for (size_t i = 0; i < pts_count; ++i)
        pts_array[i] = clock->to_system_locked(clock, system_now, pts_array[i]);
    vlc_mutex_unlock(&main_clock->lock);
}

vlc_tick_t vlc_clock_ConvertToStream(vlc_clock_t * clock, vlc_tick_t system)
{
    return vlc_clock_to_stream(clock, system);
}

void vlc_clock_SetDejitter(vlc_clock_t * clock, vlc_tick_t delay, int cr_avg)
{
    clock->set_dejitter(clock, delay, cr_avg);
}

vlc_tick_t vlc_clock_GetDejitter(vlc_clock_t * clock)
{
    return vlc_clock_get_dejitter(clock);
}

static void vlc_clock_set_master_cbk(vlc_clock_t * clk)
{

    clk->update = vlc_clock_master_update;
    clk->reset = vlc_clock_master_reset;
    clk->pause = vlc_clock_master_pause;
    clk->set_delay = vlc_clock_master_set_delay;
    clk->set_dejitter = vlc_clock_master_set_dejitter;
    clk->to_system_locked = vlc_clock_master_to_system_locked;
}

static void vlc_clock_set_slave_cbk(vlc_clock_t * clk)
{
    clk->update = vlc_clock_slave_update;
    clk->reset = vlc_clock_slave_reset;
    clk->pause = vlc_clock_slave_pause;
    clk->set_delay = vlc_clock_slave_set_delay;
    clk->set_dejitter = vlc_clock_slave_set_dejitter;
    clk->to_system_locked = vlc_clock_slave_to_system_locked;
}

vlc_clock_t * vlc_clock_NewMaster(vlc_clock_main_t * main_clock)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = main_clock;
    clk->delay = 0;

    vlc_mutex_lock(&main_clock->lock);
    if (main_clock->master != NULL)
    {
        vlc_clock_master_reset(main_clock->master);
        vlc_clock_set_slave_cbk(main_clock->master);
        TAB_APPEND(main_clock->nslaves, main_clock->slaves, main_clock->master);
    }
    vlc_clock_set_master_cbk(clk);
    main_clock->master = clk;
    vlc_mutex_unlock(&main_clock->lock);

    return clk;
}

vlc_clock_t * vlc_clock_NewSlave(vlc_clock_main_t * main_clock)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = main_clock;
    clk->delay = 0;

    vlc_mutex_lock(&main_clock->lock);
    vlc_clock_set_slave_cbk(clk);
    TAB_APPEND(main_clock->nslaves, main_clock->slaves, clk);
    vlc_mutex_unlock(&main_clock->lock);

    return clk;
}

void vlc_clock_SetMaster(vlc_clock_main_t * main_clock, vlc_clock_t * clk)
{
    vlc_mutex_lock(&main_clock->lock);
    TAB_REMOVE(main_clock->nslaves, main_clock->slaves, clk);
    if (main_clock->master != NULL)
    {
        vlc_clock_master_reset(main_clock->master);
        vlc_clock_set_slave_cbk(main_clock->master);
        TAB_APPEND(main_clock->nslaves, main_clock->slaves, main_clock->master);
    }
    vlc_clock_set_master_cbk(clk);
    main_clock->master = clk;
    vlc_mutex_unlock(&main_clock->lock);
}


void vlc_clock_Delete(vlc_clock_t * clock)
{
    vlc_clock_main_t * main_clock = clock->owner;
    vlc_mutex_lock(&main_clock->lock);
    if (clock == main_clock->master)
    {
        vlc_clock_main_reset(main_clock);
        main_clock->master = NULL;
    }
    else
        TAB_REMOVE(main_clock->nslaves, main_clock->slaves, clock);
    vlc_mutex_unlock(&main_clock->lock);
    free(clock);
}


