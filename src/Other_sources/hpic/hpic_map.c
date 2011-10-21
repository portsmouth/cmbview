/*****************************************************************************
 * Copyright 2003-2005 Theodore Kisner <kisner@physics.ucsb.edu>             *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   * 
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Please see the notice at the top of the hpic.h header file for         *
 * additional copyright and warranty exclusion information.                  *
 *                                                                           *
 * This code deals with basic map operations                                 *
 *****************************************************************************/

#include "hpic.h"
#include "hpic_tree.h"

/* alloc/free */

hpic *hpic_alloc(size_t nside, int order, int coord, int mem)
{
  size_t i;
  hpic *map;
  int err;
  
  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR_VAL(err,"nside value not allowed",NULL);
  }
  if ((order != HPIC_RING) && (order != HPIC_NEST)) {
    HPIC_ERROR_VAL(HPIC_ERR_ORDER,"order must be HPIC_RING or HPIC_NEST",NULL);
  }
  map = (hpic *) calloc(1, sizeof(hpic));
  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map",NULL);
  }
  map->nside = nside;
  map->npix = 12 * nside * nside;
  map->order = order;
  map->coord = coord;
  map->name = (char *)calloc(FLEN_VALUE, sizeof(char));
  if (!(map->name)) {
    free(map);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->name",NULL);
  }
  map->units = (char *)calloc(FLEN_VALUE, sizeof(char));
  if (!(map->units)) {
    free(map->name);
    free(map);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->units",NULL);
  }
  
  /*temporary redirect of HPIC_TREE option (which is not implemented) */
  if (mem == HPIC_TREE) {
    mem = HPIC_AUTO;
  }

  map->mem = mem;
  if (((mem == HPIC_TREE)) && (nside != 1)) {
    map->tree = hpic_tree_alloc(nside);
    map->curstate = HPIC_TREE;
  } else {
    map->data = (double *)calloc(map->npix, sizeof(double));
    if (!(map->data)) {
      free(map->name);
      free(map->units);
      free(map);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->data",NULL);
    }
    for (i = 0; i < (map->npix); i++) {
      map->data[i] = HPIC_NULL;
    }
    map->curstate = HPIC_STND;
  }
  
  return map;
}

int hpic_free(hpic * map)
{
  if (map) {
    if (map->curstate == HPIC_TREE) {
      hpic_tree_free(map->tree);
    } else {
      free(map->data);
    }
    free(map->name);
    free(map->units);
    free(map);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "map not allocated, so not freeing");
  }
}

hpic_float *hpic_float_alloc(size_t nside, int order, int coord, int mem)
{
  size_t i;
  hpic_float *map;
  int err;
  
  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR_VAL(err,"nside value not allowed",NULL);
  }
  if ((order != HPIC_RING) && (order != HPIC_NEST)) {
    HPIC_ERROR_VAL(HPIC_ERR_ORDER,"order must be HPIC_RING or HPIC_NEST",NULL);
  }
  map = (hpic_float *) calloc(1, sizeof(hpic_float));
  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map",NULL);
  }
  map->nside = nside;
  map->npix = 12 * nside * nside;
  map->order = order;
  map->coord = coord;
  map->name = (char *)calloc(FLEN_VALUE, sizeof(char));
  if (!(map->name)) {
    free(map);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->name",NULL);
  }
  map->units = (char *)calloc(FLEN_VALUE, sizeof(char));
  if (!(map->units)) {
    free(map->name);
    free(map);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->units",NULL);
  }
  
  /*temporary redirect of HPIC_TREE option (which is not implemented) */
  if (mem == HPIC_TREE) {
    mem = HPIC_AUTO;
  }

  map->mem = mem;
  if (((mem == HPIC_TREE)) && (nside != 1)) {
    map->tree = hpic_tree_alloc(nside);
    map->curstate = HPIC_TREE;
  } else {
    map->data = (float *)calloc(map->npix, sizeof(float));
    if (!(map->data)) {
      free(map->name);
      free(map->units);
      free(map);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->data",NULL);
    }
    for (i = 0; i < (map->npix); i++) {
      map->data[i] = HPIC_NULL;
    }
    map->curstate = HPIC_STND;
  }
  
  return map;
}

int hpic_float_free(hpic_float * map)
{
  if (map) {
    if (map->curstate == HPIC_TREE) {
      hpic_tree_free(map->tree);
    } else {
      free(map->data);
    }
    free(map->name);
    free(map->units);
    free(map);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "map not allocated, so not freeing");
  }
}

hpic_int *hpic_int_alloc(size_t nside, int order, int coord, int mem)
{
  size_t i;
  hpic_int *map;
  int err;

  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR_VAL(err,"nside value not allowed",NULL);
  }
  if ((order != HPIC_RING) && (order != HPIC_NEST)) {
    HPIC_ERROR_VAL(HPIC_ERR_ORDER,"order must be HPIC_RING or HPIC_NEST",NULL);
  }
  map = (hpic_int *) calloc(1, sizeof(hpic_int));
  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map",NULL);
  }
  map->nside = nside;
  map->npix = 12 * nside * nside;
  map->order = order;
  map->coord = coord;
  map->name = (char *)calloc(FLEN_VALUE, sizeof(char));
  if (!(map->name)) {
    free(map);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->name",NULL);
  }
  map->units = (char *)calloc(FLEN_VALUE, sizeof(char));
  if (!(map->units)) {
    free(map->name);
    free(map);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->units",NULL);
  }
  
  /*temporary redirect of HPIC_TREE option (which is not implemented) */
  if (mem == HPIC_TREE) {
    mem = HPIC_AUTO;
  }

  map->mem = mem;
  if (((mem == HPIC_TREE)) && (nside != 1)) {
    map->tree = hpic_tree_alloc(nside);
    map->curstate = HPIC_TREE;
  } else {
    map->data = (int *)calloc(map->npix, sizeof(int));
    if (!(map->data)) {
      free(map->name);
      free(map->units);
      free(map);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate map->data",NULL);
    }
    for (i = 0; i < (map->npix); i++) {
      map->data[i] = HPIC_INT_NULL;
    }
    map->curstate = HPIC_STND;
  }
    
  return map;
}

int hpic_int_free(hpic_int * map)
{
  if (map) {
    if (map->curstate == HPIC_TREE) {
      hpic_tree_free(map->tree);
    } else {
      free(map->data);
    }
    free(map->name);
    free(map->units);
    free(map);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "map not allocated, so not freeing");
  }
}

/* memory remapping */

int hpic_mem_get(hpic * map)
{
  if (map) {
    return map->mem;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_mem_set(hpic * map, int mem)
{
  /*
     if(map){
     map->mem = mem;

     return;
     }else{
     fprintf(stderr,"hpic_mem_set:  map pointer is NULL\n");
     exit(1);
     }
   */
}

int hpic_float_mem_get(hpic_float * map)
{
  if (map) {
    return map->mem;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_float_mem_set(hpic_float * map, int mem)
{
  /*
     if(map){
     map->mem = mem;
     return;
     }else{
     fprintf(stderr,"hpic_float_mem_set:  map pointer is NULL\n");
     exit(1);
     }
   */
}

int hpic_int_mem_get(hpic_int * map)
{
  if (map) {
    return map->mem;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_int_mem_set(hpic_int * map, int mem)
{
  /*
     if(map){
     map->mem = mem;
     return;
     }else{
     fprintf(stderr,"hpic_int_mem_set:  map pointer is NULL\n");
     exit(1);
     }
   */
}

/* parameter access */

int hpic_name_set(hpic * map, const char *name)
{
  if (map) {
    strncpy(map->name, name, FLEN_VALUE);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_units_set(hpic * map, const char *units)
{
  if (map) {
    strncpy(map->units, units, FLEN_VALUE);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

char *hpic_name_get(hpic * map)
{
  if (map) {
    return map->name;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", '\0');
  }
}

char *hpic_units_get(hpic * map)
{
  if (map) {
    return map->units;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", '\0');
  }
}

inline size_t hpic_nside_get(hpic * map)
{
  if (map) {
    return map->nside;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline size_t hpic_npix_get(hpic * map)
{
  if (map) {
    return map->npix;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline int hpic_order_get(hpic * map)
{
  if (map) {
    return map->order;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline int hpic_coord_get(hpic * map)
{
  if (map) {
    return map->coord;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

int hpic_float_name_set(hpic_float * map, const char *name)
{
  if (map) {
    strncpy(map->name, name, FLEN_VALUE);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_float_units_set(hpic_float * map, const char *units)
{
  if (map) {
    strncpy(map->units, units, FLEN_VALUE);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

char *hpic_float_name_get(hpic_float * map)
{
  if (map) {
    return map->name;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", '\0');
  }
}

char *hpic_float_units_get(hpic_float * map)
{
  if (map) {
    return map->units;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", '\0');
  }
}

inline size_t hpic_float_nside_get(hpic_float * map)
{
  if (map) {
    return map->nside;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline size_t hpic_float_npix_get(hpic_float * map)
{
  if (map) {
    return map->npix;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline int hpic_float_order_get(hpic_float * map)
{
  if (map) {
    return map->order;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline int hpic_float_coord_get(hpic_float * map)
{
  if (map) {
    return map->coord;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

int hpic_int_name_set(hpic_int * map, const char *name)
{
  if (map) {
    strncpy(map->name, name, FLEN_VALUE);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_int_units_set(hpic_int * map, const char *units)
{
  if (map) {
    strncpy(map->units, units, FLEN_VALUE);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

char *hpic_int_name_get(hpic_int * map)
{
  if (map) {
    return map->name;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", '\0');
  }
}

inline char *hpic_int_units_get(hpic_int * map)
{
  if (map) {
    return map->units;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", '\0');
  }
}

inline size_t hpic_int_nside_get(hpic_int * map)
{
  if (map) {
    return map->nside;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline size_t hpic_int_npix_get(hpic_int * map)
{
  if (map) {
    return map->npix;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline int hpic_int_order_get(hpic_int * map)
{
  if (map) {
    return map->order;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

inline int hpic_int_coord_get(hpic_int * map)
{
  if (map) {
    return map->coord;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", 0);
  }
}

/* data access */

int hpic_set(hpic * map, size_t pix, double val)
{
  if (map) {
    if ((pix < (map->npix)) && (pix >= 0)) {
      map->data[pix] = val;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "pixel value out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

inline double hpic_get(hpic * map, size_t pix)
{
  if (map) {
    if ((pix < (map->npix)) && (pix >= 0)) {
      return map->data[pix];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "pixel value out of range", HPIC_NULL);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", HPIC_NULL);
  }
}

int hpic_float_set(hpic_float * map, size_t pix, float val)
{
  if (map) {
    if ((pix < (map->npix)) && (pix >= 0)) {
      map->data[pix] = val;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "pixel value out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

inline float hpic_float_get(hpic_float * map, size_t pix)
{
  if (map) {
    if ((pix < (map->npix)) && (pix >= 0)) {
      return map->data[pix];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "pixel value out of range", HPIC_NULL);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", HPIC_NULL);
  }
}

int hpic_int_set(hpic_int * map, size_t pix, int val)
{
  if (map) {
    if ((pix < (map->npix)) && (pix >= 0)) {
      map->data[pix] = val;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "pixel value out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

inline int hpic_int_get(hpic_int * map, size_t pix)
{
  if (map) {
    if ((pix < (map->npix)) && (pix >= 0)) {
      return map->data[pix];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "pixel value out of range", HPIC_INT_NULL);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", HPIC_INT_NULL);
  }
}

int hpic_setall(hpic * map, double val)
{
  size_t i;
  if (map) {
    for (i = 0; i < (map->npix); i++) {
      hpic_set(map, i, val);
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_float_setall(hpic_float * map, float val)
{
  size_t i;
  if (map) {
    for (i = 0; i < (map->npix); i++) {
      hpic_float_set(map, i, val);
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_int_setall(hpic_int * map, int val)
{
  size_t i;
  if (map) {
    for (i = 0; i < (map->npix); i++) {
      hpic_int_set(map, i, val);
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

hpic *hpic_copy(hpic * map)
{
  hpic *copy;
  size_t i;
  int err;
  if (map) {
    copy = hpic_alloc(map->nside, map->order, map->coord, map->mem);
    if (!copy) {
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate copy", NULL);
    }
    err = hpic_name_set(copy, hpic_name_get(map));
    if (err) {
      HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "cannot set copy name", NULL);
    }
    err = hpic_units_set(copy, hpic_units_get(map));
    if (err) {
      HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "cannot set copy units", NULL);
    }
    for (i = 0; i < (map->npix); i++) {
      hpic_set(copy, i, hpic_get(map, i));
    }
    return copy;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", NULL);
  }
}

hpic_float *hpic_float_copy(hpic_float * map)
{
  hpic_float *copy;
  size_t i;
  int err;
  if (map) {
    copy = hpic_float_alloc(map->nside, map->order, map->coord, map->mem);
    if (!copy) {
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate copy", NULL);
    }
    err = hpic_float_name_set(copy, hpic_float_name_get(map));
    if (err) {
      HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "cannot set copy name", NULL);
    }
    err = hpic_float_units_set(copy, hpic_float_units_get(map));
    if (err) {
      HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "cannot set copy units", NULL);
    }
    for (i = 0; i < (map->npix); i++) {
      hpic_float_set(copy, i, hpic_float_get(map, i));
    }
    return copy;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", NULL);
  }
}

hpic_int *hpic_int_copy(hpic_int * map)
{
  hpic_int *copy;
  size_t i;
  int err;
  if (map) {
    copy = hpic_int_alloc(map->nside, map->order, map->coord, map->mem);
    if (!copy) {
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate copy", NULL);
    }
    err = hpic_int_name_set(copy, hpic_int_name_get(map));
    if (err) {
      HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "cannot set copy name", NULL);
    }
    err = hpic_int_units_set(copy, hpic_int_units_get(map));
    if (err) {
      HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "cannot set copy units", NULL);
    }
    for (i = 0; i < (map->npix); i++) {
      hpic_int_set(copy, i, hpic_int_get(map, i));
    }
    return copy;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "map pointer is NULL", NULL);
  }
}

/* info printing */

int hpic_info_fprintf(FILE * fp, hpic * map)
{
  if (map) {
    fprintf(fp, "  ----------------------------------------\n");
    fprintf(fp, "  | Map = \"%s\" Units = \"%s\"\n", map->name, map->units);
    fprintf(fp, "  |   nside = %lu npix = %lu\n", map->nside, map->npix);
    fprintf(fp, "  |   order = %d coord = %d\n", map->order, map->coord);
    fprintf(fp, "  |   data = %g ... %g\n", map->data[0],
            map->data[(map->npix) - 1]);
    fprintf(fp, "  ----------------------------------------\n");
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_info_printf(hpic * map)
{
  return hpic_info_fprintf(stdout, map);
}

int hpic_float_info_fprintf(FILE * fp, hpic_float * map)
{
  if (map) {
    fprintf(fp, "  ----------------------------------------\n");
    fprintf(fp, "  | Map = \"%s\" Units = \"%s\"\n", map->name, map->units);
    fprintf(fp, "  |   nside = %lu npix = %lu\n", map->nside, map->npix);
    fprintf(fp, "  |   order = %d coord = %d\n", map->order, map->coord);
    fprintf(fp, "  |   data = %g ... %g\n", map->data[0],
            map->data[(map->npix) - 1]);
    fprintf(fp, "  ----------------------------------------\n");
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_float_info_printf(hpic_float * map)
{
  return hpic_float_info_fprintf(stdout, map);
}

int hpic_int_info_fprintf(FILE * fp, hpic_int * map)
{
  if (map) {
    fprintf(fp, "  ----------------------------------------\n");
    fprintf(fp, "  | Map = \"%s\" Units = \"%s\"\n", map->name, map->units);
    fprintf(fp, "  |   nside = %lu npix = %lu\n", map->nside, map->npix);
    fprintf(fp, "  |   order = %d coord = %d\n", map->order, map->coord);
    fprintf(fp, "  |   data = %d ... %d\n", map->data[0],
            map->data[(map->npix) - 1]);
    fprintf(fp, "  ----------------------------------------\n");
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
}

int hpic_int_info_printf(hpic_int * map)
{
  return hpic_int_info_fprintf(stdout, map);
}

int hpic_comp(hpic * map1, hpic * map2)
{
  int result = 0;
  size_t i;
  double temp1;
  double temp2;
  
  if (!map1) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map1 pointer is NULL");
  }
  if (!map2) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map2 pointer is NULL");
  }

  if (strncmp(hpic_name_get(map1), hpic_name_get(map2), FLEN_VALUE) != 0) {
    result = 1;
  }
  if (strncmp(hpic_units_get(map1), hpic_units_get(map2), FLEN_VALUE) != 0) {
    result = 1;
  }
  if (hpic_order_get(map1) != hpic_order_get(map2)) {
    result = 1;
  }
  if (hpic_coord_get(map1) != hpic_coord_get(map2)) {
    result = 1;
  }
  if (hpic_nside_get(map1) != hpic_nside_get(map2)) {
    result = 2;
  }
  if (result < 2) {
    for (i = 0; i < hpic_npix_get(map1); i++) {
      temp1 = hpic_get(map1, i);
      temp2 = hpic_get(map2, i);
      if (hpic_is_dnull(temp1) || hpic_is_dnull(temp2)) {
        if (!(hpic_is_dnull(temp1) && hpic_is_dnull(temp2))) {
          result = 2;
        }
      } else {
        if (temp1 != temp2) {
          result = 2;
        }
      }
    }
  }
  return result;
}

int hpic_float_comp(hpic_float * map1, hpic_float * map2)
{
  int result = 0;
  size_t i;
  float temp1;
  float temp2;

  if (!map1) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map1 pointer is NULL");
  }
  if (!map2) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map2 pointer is NULL");
  }
  
  if (strncmp
      (hpic_float_name_get(map1), hpic_float_name_get(map2),
       FLEN_VALUE) != 0) {
    result = 1;
  }
  if (strncmp
      (hpic_float_units_get(map1), hpic_float_units_get(map2),
       FLEN_VALUE) != 0) {
    result = 1;
  }
  if (hpic_float_order_get(map1) != hpic_float_order_get(map2)) {
    result = 1;
  }
  if (hpic_float_coord_get(map1) != hpic_float_coord_get(map2)) {
    result = 1;
  }
  if (hpic_float_nside_get(map1) != hpic_float_nside_get(map2)) {
    result = 2;
  }
  if (result < 2) {
    for (i = 0; i < hpic_float_npix_get(map1); i++) {
      temp1 = hpic_float_get(map1, i);
      temp2 = hpic_float_get(map2, i);
      if (hpic_is_fnull(temp1) || hpic_is_fnull(temp2)) {
        if (!(hpic_is_fnull(temp1) && hpic_is_fnull(temp2))) {
          result = 2;
        }
      } else {
        if (temp1 != temp2) {
          result = 2;
        }
      }
    }
  }
  return result;
}

int hpic_int_comp(hpic_int * map1, hpic_int * map2)
{
  int result = 0;
  size_t i;
  int temp1;
  int temp2;

  if (!map1) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map1 pointer is NULL");
  }
  if (!map2) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map2 pointer is NULL");
  }
  
  if (strncmp(hpic_int_name_get(map1), hpic_int_name_get(map2), FLEN_VALUE) !=
      0) {
    result = 1;
  }
  if (strncmp(hpic_int_units_get(map1), hpic_int_units_get(map2), FLEN_VALUE)
      != 0) {
    result = 1;
  }
  if (hpic_int_order_get(map1) != hpic_int_order_get(map2)) {
    result = 1;
  }
  if (hpic_int_coord_get(map1) != hpic_int_coord_get(map2)) {
    result = 1;
  }
  if (hpic_int_nside_get(map1) != hpic_int_nside_get(map2)) {
    result = 2;
  }
  if (result < 2) {
    for (i = 0; i < hpic_int_npix_get(map1); i++) {
      temp1 = hpic_int_get(map1, i);
      temp2 = hpic_int_get(map2, i);
      if (hpic_is_inull(temp1) || hpic_is_inull(temp2)) {
        if (!(hpic_is_inull(temp1) && hpic_is_inull(temp2))) {
          result = 2;
        }
      } else {
        if (temp1 != temp2) {
          result = 2;
        }
      }
    }
  }
  return result;
}
