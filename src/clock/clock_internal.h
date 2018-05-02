/*****************************************************************************
 * clock_internal.h: Clock internal functions
 *****************************************************************************
 * Copyright (C) 2018 VLC authors and VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Laurent Aimar < fenrir _AT_ videolan _DOT_ org >
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
#ifndef LIBVLC_CLOCK_INTERNAL_H
#define LIBVLC_CLOCK_INTERNAL_H

#include <vlc_common.h>

/*****************************************************************************
 * Structures
 *****************************************************************************/

/**
 * This structure holds long term moving average
 */
typedef struct
{
    float value; /* The average value */
    int count; /* The number of sample evaluated */
    int range; /* The maximum range of sample on which we calculate the average*/
} average_t;

void AvgInit(average_t *, int range);
void AvgClean(average_t *);

void AvgReset(average_t *);

/*  calculates (previous_average * (range - 1) + new_value)/range */
void AvgUpdate(average_t *, mtime_t value);

float AvgGet(average_t *);
void AvgRescale(average_t *, int range);

/* */
typedef struct
{
    mtime_t stream;
    mtime_t system;
} clock_point_t;

static inline clock_point_t clock_point_Create( mtime_t stream, mtime_t system )
{
    clock_point_t p = { .stream = stream, .system = system };
    return p;
}

#endif /* LIBVLC_CLOCK_INTERNAL_H */
