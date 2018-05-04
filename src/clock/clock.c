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
#include "clock.h"
#include "clock_internal.h"

struct vlc_clock_master_t
{
    vlc_mutex_t lock;
    vlc_cond_t cond;

    vlc_clock_t * master;

    vlc_clock_t ** slaves;
    size_t nslaves;

    /* Linear function
     * system = pts * coeff / rate + offset
     */
    double coeff; /* TODO use a moving average */
    double rate;
    mtime_t offset;

    mtime_t pause_date;

    mtime_t jitter; /* TODO */
    bool abort;
};

struct vlc_clock_t
{
    mtime_t (*update)(vlc_clock_t * clock, mtime_t timestamp,
                      mtime_t system_now, double rate);
    void (*reset)(vlc_clock_t * clock);
    void (*pause)(vlc_clock_t * clock, bool paused, mtime_t now);
    int (*wait)(vlc_clock_t * clock, mtime_t pts);
    void (*set_jitter)(vlc_clock_t * clock, mtime_t delay, int cr_avg);

    vlc_clock_master_t * owner;
    double rate;
    clock_point_t last;
};

static mtime_t master_system_to_stream(vlc_clock_master_t * master,
                                       mtime_t system)
{
    if (unlikely(master->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (mtime_t) ((system - master->offset) * master->rate / master->coeff);
}

static mtime_t master_stream_to_system(vlc_clock_master_t * master,
                                       mtime_t pts)
{
    if (unlikely(master->offset == VLC_TS_INVALID))
        return VLC_TS_INVALID;
    return (mtime_t) (pts * master->coeff / master->rate + master->offset);
}


static mtime_t vlc_clock_master_update(vlc_clock_t * clock, mtime_t timestamp,
                                       mtime_t system_now, double rate)
{
    vlc_clock_master_t * master = clock->owner;

    vlc_mutex_lock(&master->lock);
    if (rate != master->rate)
    {
        master->rate = rate;
        vlc_cond_broadcast(&master->cond);
    }
    if (unlikely(timestamp == VLC_TS_INVALID || system_now == VLC_TS_INVALID))
    {
        vlc_mutex_unlock(&master->lock);
        return VLC_TS_INVALID;
    }

    if (master->offset != VLC_TS_INVALID)
    {
        /* We have a reference so we can update coeff */
        /* TODO*/
    }
    master->offset = system_now - timestamp * master->coeff / rate;

    vlc_mutex_unlock(&master->lock);
    return 0;
}

static void vlc_clock_master_reset(vlc_clock_t * clock)
{
    vlc_clock_master_t * master = clock->owner;

    vlc_mutex_lock(&master->lock);
    master->coeff = 1.0;
    /* TODO Reset coeff avg */
    master->offset = VLC_TS_INVALID;
    master->rate = 1.0;
    vlc_cond_broadcast(&master->cond);
    vlc_mutex_unlock(&master->lock);
}

static void vlc_clock_master_pause(vlc_clock_t * clock, bool paused, mtime_t now)
{
    vlc_clock_master_t * master = clock->owner;
    vlc_mutex_lock(&master->lock);

    if (paused)
        master->pause_date = now;
    else
        master->offset += now - master->pause_date;

    vlc_mutex_unlock(&master->lock);
}

static int vlc_clock_get_rate(vlc_clock_t * clock)
{
    double rate;
    vlc_clock_master_t * master = clock->owner;
    vlc_mutex_lock(&master->lock);

    rate = master->rate;

    vlc_mutex_unlock(&master->lock);

    return rate;
}

static int vlc_clock_master_wait(vlc_clock_t * clock, mtime_t pts)
{
    return 0; /* FIXME unused? */
}

static mtime_t vlc_clock_to_system(vlc_clock_t * clock, mtime_t timestamp)
{
    vlc_clock_master_t * master = clock->owner;
    mtime_t system;
    vlc_mutex_lock(&master->lock);
    system = master_stream_to_system(master, timestamp);
    vlc_mutex_unlock(&master->lock);
    return system;
}

static mtime_t vlc_clock_to_stream(vlc_clock_t * clock, mtime_t system)
{
    vlc_clock_master_t * master = clock->owner;
    mtime_t pts;

    vlc_mutex_lock(&master->lock);
    pts = master_system_to_stream(master, system);
    vlc_mutex_unlock(&master->lock);
    return pts;
}

static void vlc_clock_master_set_jitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{
    vlc_clock_master_t * master = clock->owner;

    vlc_mutex_lock(&master->lock);
    master->jitter = delay;
    vlc_mutex_unlock(&master->lock);
}

static mtime_t vlc_clock_get_jitter(vlc_clock_t * clock)
{
    vlc_clock_master_t * master = clock->owner;
    mtime_t jitter;

    vlc_mutex_lock(&master->lock);
    jitter = master->jitter;
    vlc_mutex_unlock(&master->lock);
    return jitter;
}

static mtime_t vlc_clock_slave_update(vlc_clock_t * clock, mtime_t timestamp,
                                      mtime_t system_now, double rate)
{
    mtime_t computed = vlc_clock_to_system(clock, timestamp);
    if (computed == VLC_TS_INVALID)
        computed =
            (timestamp - clock->last.i_stream) / rate + clock->last.i_system;
    return (computed - system_now);
}

static void vlc_clock_slave_reset(vlc_clock_t * clock)
{}

static void vlc_clock_slave_pause(vlc_clock_t * clock, bool paused, mtime_t now)
{}

static int vlc_clock_slave_wait(vlc_clock_t * clock, mtime_t pts)
{
    vlc_clock_master_t * master = clock->owner;
    vlc_mutex_lock(&master->lock);
    while (!master->abort)
    {
        mtime_t deadline = master_stream_to_system(master, pts);
        /* Fallback to clock monotonic when there is no master */
        if (unlikely(deadline == VLC_TS_INVALID))
        {
            if (unlikely(clock->last.i_stream == VLC_TS_INVALID ||
                         clock->last.i_system == VLC_TS_INVALID))
                return 0;

            deadline =
                (pts - clock->last.i_stream) / clock->rate + clock->last.i_system;
        }
        if (vlc_cond_timedwait(&master->cond, &master->lock, deadline))
            return 0;
    }
    return 1;
}

static void vlc_clock_slave_set_jitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{}

vlc_clock_master_t * vlc_clock_master_New()
{
    vlc_clock_master_t * master = malloc(sizeof(vlc_clock_master_t));

    if (master == NULL)
        return NULL;

    vlc_mutex_init(&master->lock);
    vlc_cond_init(&master->cond);
    master->master = NULL;

    TAB_INIT(master->slaves, master->nslaves);

    master->rate = 1.0;
    master->coeff = 1.0;
    master->offset = VLC_TS_INVALID;

    master->pause_date = VLC_TS_INVALID;
    master->jitter = VLC_TS_INVALID;
    master->abort = false;

    return master;
}

void vlc_clock_master_Abort(vlc_clock_master_t * master)
{
    size_t i;
    vlc_mutex_lock(&master->lock);
    master->abort = true;
    vlc_cond_broadcast(&master->cond);

    vlc_mutex_unlock(&master->lock);
}

void vlc_clock_master_Delete(vlc_clock_master_t * master)
{
    while (master->nslaves != 0)
        vlc_clock_Delete(master->slaves[0]);

    if (master->master != NULL)
        vlc_clock_Delete(master->master);

    TAB_CLEAN(master->nslaves, master->slaves);
    vlc_mutex_destroy(&master->lock);
    free(master);
}

mtime_t vlc_clock_Update(vlc_clock_t * clock, mtime_t timestamp,
                         mtime_t system_now, double rate)
{
    clock->rate = rate;
    if (timestamp != VLC_TS_INVALID && system_now != VLC_TS_INVALID)
        clock->last = clock_point_Create(timestamp, system_now);
    return clock->update(clock, timestamp, system_now, rate);
}

void vlc_clock_Reset(vlc_clock_t * clock)
{
    clock->last = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);
    clock->reset(clock);
}

void vlc_clock_ChangePause(vlc_clock_t * clock, bool paused,
                              mtime_t system_now)
{
    clock->pause(clock, paused, system_now);
}

int vlc_clock_GetRate(vlc_clock_t * clock)
{
    return vlc_clock_get_rate(clock);
}

int vlc_clock_Wait(vlc_clock_t * clock, mtime_t pts)
{
    return clock->wait(clock, pts);
}


mtime_t vlc_clock_ConvertToSystem(vlc_clock_t * clock, mtime_t timestamp)
{
    return vlc_clock_to_system(clock, timestamp);
}

mtime_t vlc_clock_ConvertToStream(vlc_clock_t * clock, mtime_t system)
{
    return vlc_clock_to_stream(clock, system);
}

void vlc_clock_SetJitter(vlc_clock_t * clock, mtime_t delay, int cr_avg)
{
    clock->set_jitter(clock, delay, cr_avg);
}

mtime_t vlc_clock_GetJitter(vlc_clock_t * clock)
{
    return vlc_clock_get_jitter(clock);
}

static void vlc_clock_set_master_cbk(vlc_clock_t * clk)
{

    clk->update = vlc_clock_master_update;
    clk->reset = vlc_clock_master_reset;
    clk->pause = vlc_clock_master_pause;
    clk->wait = vlc_clock_master_wait;
    clk->set_jitter = vlc_clock_master_set_jitter;
}

static void vlc_clock_set_slave_cbk(vlc_clock_t * clk)
{
    clk->update = vlc_clock_slave_update;
    clk->reset = vlc_clock_slave_reset;
    clk->pause = vlc_clock_slave_pause;
    clk->wait = vlc_clock_slave_wait;
    clk->set_jitter = vlc_clock_slave_set_jitter;
}

vlc_clock_t * vlc_clock_NewMaster(vlc_clock_master_t * master)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = master;

    vlc_mutex_lock(&master->lock);
    if (master->master != NULL)
    {
        vlc_clock_master_reset(master->master);
        vlc_clock_set_slave_cbk(master->master);
        TAB_APPEND(master->nslaves, master->slaves, master->master);
    }
    vlc_clock_set_master_cbk(clk);
    master->master = clk;
    vlc_mutex_unlock(&master->lock);

    return clk;
}

vlc_clock_t * vlc_clock_NewSlave(vlc_clock_master_t * master)
{
    vlc_clock_t * clk = malloc(sizeof(vlc_clock_t));
    if (clk == NULL)
        return NULL;

    clk->owner = master;
    clk->last = clock_point_Create(VLC_TS_INVALID, VLC_TS_INVALID);

    vlc_mutex_lock(&master->lock);
    clk->rate = master->rate;
    vlc_clock_set_slave_cbk(clk);
    TAB_APPEND(master->nslaves, master->slaves, clk);
    vlc_mutex_unlock(&master->lock);

    return clk;
}

void vlc_clock_SetMaster(vlc_clock_master_t * master, vlc_clock_t * clk)
{
    vlc_mutex_lock(&master->lock);
    TAB_REMOVE(master->nslaves, master->slaves, clk);
    if (master->master != NULL)
    {
        vlc_clock_master_reset(master->master);
        vlc_clock_set_slave_cbk(master->master);
        TAB_APPEND(master->nslaves, master->slaves, master->master);
    }
    vlc_clock_set_master_cbk(clk);
    master->master = clk;
    vlc_mutex_unlock(&master->lock);
}


void vlc_clock_Delete(vlc_clock_t * clock)
{
    vlc_clock_master_t * master = clock->owner;
    vlc_mutex_lock(&master->lock);
    if (clock == master->master)
    {
        vlc_clock_master_reset(clock);
        master->master = NULL;
    }
    else
        TAB_REMOVE(master->nslaves, master->slaves, clock);
    vlc_mutex_unlock(&master->lock);
    free(clock);
}


