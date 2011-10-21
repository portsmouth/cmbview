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
 * This code deals with vector arrays.                                       *
 *****************************************************************************/

#include <hpic.h>

/* alloc/free */

hpic_vec_fltarr *hpic_vec_fltarr_alloc(size_t nvecs)
{
  size_t i;
  hpic_vec_fltarr *array;

  if (nvecs == 0) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate array of zero size", NULL);
  }
  array = (hpic_vec_fltarr *) calloc(1, sizeof(hpic_vec_fltarr));
  if (!array) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate array", NULL);
  }
  array->nvecs = nvecs;

  array->vecs = (hpic_vec_float **) calloc(nvecs, sizeof(hpic_vec_float *));
  if (!(array->vecs)) {
    free(array);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate array->vecs", NULL);
  }
  for (i = 0; i < nvecs; i++) {
    array->vecs[i] = NULL;
  }
  return array;
}

int hpic_vec_fltarr_free(hpic_vec_fltarr * array)
{
  if (array) {
    free(array->vecs);
    free(array);
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "array not allocated, not freeing");
  }
  return 0;
}

/* parameter access */

inline size_t hpic_vec_fltarr_n_get(hpic_vec_fltarr * array)
{
  if (array) {
    return array->nvecs;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "array pointer is NULL", 0);
  }
}

/* data access */

inline int hpic_vec_fltarr_set(hpic_vec_fltarr * array, size_t elem,
                         hpic_vec_float * vec)
{
  if (array) {
    if (elem < (array->nvecs)) {
      array->vecs[elem] = vec;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "array element out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "array pointer is NULL");
  }
}

inline hpic_vec_float *hpic_vec_fltarr_get(hpic_vec_fltarr * array, size_t elem)
{
  if (array) {
    if (elem < (array->nvecs)) {
      return array->vecs[elem];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "array element out of range", NULL);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "array pointer is NULL", NULL);
  }
}

