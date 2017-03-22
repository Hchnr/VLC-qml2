/*****************************************************************************
 * thread.c : nacl thread functions and futexes
 *****************************************************************************
 * Copyright (C) 2017 VLC authors and VideoLAN
 *
 * Authors: Dennis Hamester <dhamester@jusst.de>
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
#include <vlc_atomic.h>

#include "libvlc.h"
#include <errno.h>
#include <time.h>
#include <assert.h>

#include <sys/types.h>
#include <pthread.h>

unsigned long vlc_thread_id(void)
{
    return (unsigned long)(void *)pthread_self();
}

typedef struct {
    void *next;
    void *addr;
    int ref_count;
    pthread_cond_t cv;
} futex_t;

static futex_t *futex_list = NULL;
static pthread_mutex_t futex_list_mtx = PTHREAD_MUTEX_INITIALIZER;

static futex_t* get_futex(void *addr, bool create) {
    futex_t *empty = NULL;
    futex_t *prev = NULL;
    futex_t *futex = futex_list;
    while (futex && (futex->addr != addr)) {
        if (!futex->addr)
            empty = futex;

        prev = futex;
        futex = futex->next;
    }

    if (!futex && create) {
        if(empty) {
            empty->addr = addr;
            futex = empty;
        }
        else {
            futex_t **last;
            if (prev)
                last = (futex_t **)&prev->next;
            else
                last = &futex_list;

            *last = futex = malloc(sizeof(futex_t));
            if (!futex)
                abort();
            futex->next = NULL;
            futex->addr = addr;
            futex->ref_count = 0;
            pthread_cond_init(&futex->cv, NULL);
        }
    }

    if (futex)
        ++futex->ref_count;

    return futex;
}

static void release_futex(futex_t *futex) {
    --futex->ref_count;
    if (!futex->ref_count)
        futex->addr = NULL;
}

void vlc_addr_signal(void *addr)
{
    if(pthread_mutex_lock(&futex_list_mtx))
        return;

    futex_t *futex = get_futex(addr, false);
    if (futex) {
        pthread_cond_signal(&futex->cv);
        release_futex(futex);
    }

    pthread_mutex_unlock(&futex_list_mtx);
}

void vlc_addr_broadcast(void *addr)
{
    if(pthread_mutex_lock(&futex_list_mtx))
        return;

    futex_t *futex = get_futex(addr, false);
    if (futex) {
        pthread_cond_broadcast(&futex->cv);
        release_futex(futex);
    }

    pthread_mutex_unlock(&futex_list_mtx);
}

void vlc_addr_wait(void *addr, unsigned val)
{
    volatile unsigned *ptr = (volatile unsigned *)addr;
    if (*ptr != val)
        return;

    if(pthread_mutex_lock(&futex_list_mtx))
        return;

    futex_t *futex = get_futex(addr, true);
    while (*ptr == val) {
        if (pthread_cond_wait(&futex->cv, &futex_list_mtx))
            break;
    }

    release_futex(futex);
    pthread_mutex_unlock(&futex_list_mtx);
}

bool vlc_addr_timedwait(void *addr, unsigned val, mtime_t delay)
{
    volatile unsigned *ptr = (volatile unsigned *)addr;
    if (*ptr != val)
        return true;

    if(pthread_mutex_lock(&futex_list_mtx))
        return true;

    bool timeout = false;
    futex_t *futex = get_futex(addr, true);

    mtime_t end = mdate() + delay;
    lldiv_t d = lldiv(end, CLOCK_FREQ);
    struct timespec ts = { d.quot, d.rem * (1000000000 / CLOCK_FREQ) };

    while (*ptr == val) {
        if(pthread_cond_timedwait(&futex->cv, &futex_list_mtx, &ts))
            break;
    }

    release_futex(futex);
    pthread_mutex_unlock(&futex_list_mtx);
    return !timeout;
}
