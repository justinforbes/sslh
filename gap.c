/*
   gap.c: gap, a simple, dynamically-growing array
   of pointers that never shrinks

# Copyright (C) 2021  Yves Rutschle
# 
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later
# version.
# 
# This program is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more
# details.
# 
# The full text for the General Public License is here:
# http://www.gnu.org/licenses/gpl.html

*/


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "sslh-conf.h"
#include "gap.h"


/* Allocate one page-worth of elements */
static int gap_len_alloc(int elem_size)
{
    return getpagesize() / elem_size;
}

/* Creates a new gap at least `len` big, all pointers are initialised at NULL */
gap_array* gap_init(int len)
{
    gap_array* gap = malloc(sizeof(*gap));
    if (!gap) return NULL;
    memset(gap, 0, sizeof(*gap));

    int elem_size = sizeof(gap->array[0]);
    gap->len = gap_len_alloc(elem_size);
    if (gap->len < len) gap->len = len;
    gap->array = malloc(gap->len * elem_size);
    if (!gap->array) {
        free(gap);
        return NULL;
    }

    for (int i = 0; i < gap->len; i++)
        gap->array[i] = NULL;

    return gap;
}

void gap_set_hardlimit(gap_array* gap, int index)
{
    gap->hardlimit = index;
}

int gap_extend(gap_array* gap)
{
    int elem_size = sizeof(gap->array[0]);
    int new_length = gap->len + gap_len_alloc(elem_size);
    void** new = realloc(gap->array, new_length * elem_size);
    if (!new) return -1;

    gap->array = new;

    for (int i = gap->len; i < new_length; i++) {
        gap->array[i] = NULL;
    }

    gap->len = new_length;

    return 0;
}

void gap_destroy(gap_array* gap) 
{
    free(gap->array);
    free(gap);
}


/* In gap, find element pointing to ptr, then shift the rest of the array that
 * is considered len elements long.
 * A poor man's list, if you will. Currently only used to remove probing
 * connections, so it only copies a few pointers at most.
 * Returns:
 * -1 if ptr was not found 
 * -2 if `len` is inconsistent with gap->len (i.e. trying to shift beyond the
 * end of the array)
 * */
int gap_remove_ptr(gap_array* gap, void* ptr, int len)
{
    int start, i;

    for (i = 0; i < len; i++)
        if (gap->array[i] == ptr)
            break;

    if (i < len)
        start = i;
    else
        return -1;

    if (gap->len < len)
        return -2;

    for (i = start; i < len - 1; i++) {
        gap->array[i] = gap->array[i+1];
    }

    return 0;
}

