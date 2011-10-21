/*****************************************************************************
 * Copyright 2003-2005 Theodore Kisner <kisner@physics.ucsb.edu>             *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   * 
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Please see the notice at the top of the hpic.h header file for            *
 * additional copyright and warranty exclusion information.                  *
 *                                                                           *
 * This code deals with map arrays.                                          *
 *****************************************************************************/

#include <hpic.h>

/* alloc/free */

hpic_fltarr *hpic_fltarr_alloc(size_t nmaps)
{
  size_t i;
  hpic_fltarr *array;

  if (nmaps == 0) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate array of zero size", NULL);
  }
  array = (hpic_fltarr *) calloc(1, sizeof(hpic_fltarr));
  if (!array) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate array", NULL);
  }
  array->nmaps = nmaps;

  array->maps = (hpic_float **) calloc(nmaps, sizeof(hpic_float *));
  if (!(array->maps)) {
    free(array);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate array->maps", NULL);
  }
  for (i = 0; i < nmaps; i++) {
    array->maps[i] = NULL;
  }
  return array;
}

int hpic_fltarr_free(hpic_fltarr * array)
{
  if (array) {
    free(array->maps);
    free(array);
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "array not allocated, not freeing");
  }
  return 0;
}

/* parameter access */

size_t hpic_fltarr_n_get(hpic_fltarr * array)
{
  if (array) {
    return array->nmaps;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "array pointer is NULL", 0);
  }
}

/* data access */

int hpic_fltarr_set(hpic_fltarr * array, size_t elem, hpic_float * map)
{
  if (array) {
    if (elem < (array->nmaps)) {
      array->maps[elem] = map;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "array element out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "array pointer is NULL");
  }
}

hpic_float *hpic_fltarr_get(hpic_fltarr * array, size_t elem)
{
  if (array) {
    if (elem < (array->nmaps)) {
      return array->maps[elem];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "array element out of range", NULL);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "array pointer is NULL", NULL);
  }
}
