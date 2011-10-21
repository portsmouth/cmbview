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
 * This code deals with map conversion operations                            *
 *****************************************************************************/

#include <hpic.h>

/* map type conversions */

hpic *hpic_float2double(hpic_float * map)
{
  size_t i;
  hpic *dmap = NULL;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  dmap =
    hpic_alloc(hpic_float_nside_get(map), hpic_float_order_get(map),
               hpic_float_coord_get(map), map->mem);
  if (!dmap) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate double map", NULL);
  }
  hpic_name_set(dmap, hpic_float_name_get(map));
  hpic_units_set(dmap, hpic_float_units_get(map));
  for (i = 0; i < (map->npix); i++) {
    if (hpic_is_fnull(hpic_float_get(map, i))) {
      hpic_set(dmap, i, HPIC_NULL);
    } else {
      hpic_set(dmap, i, (double)hpic_float_get(map, i));
    }
  }
  return dmap;
}

hpic *hpic_int2double(hpic_int * map)
{
  size_t i;
  hpic *dmap = NULL;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  dmap =
    hpic_alloc(hpic_int_nside_get(map), hpic_int_order_get(map),
               hpic_int_coord_get(map), map->mem);
  if (!dmap) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate double map", NULL);
  }
  hpic_name_set(dmap, hpic_int_name_get(map));
  hpic_units_set(dmap, hpic_int_units_get(map));
  for (i = 0; i < (map->npix); i++) {
    if (hpic_is_inull(hpic_int_get(map, i))) {
      hpic_set(dmap, i, HPIC_NULL);
    } else {
      hpic_set(dmap, i, (double)hpic_int_get(map, i));
    }
  }
  return dmap;
}

hpic_float *hpic_double2float(hpic * map)
{
  size_t i;
  hpic_float *fmap = NULL;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  fmap =
    hpic_float_alloc(hpic_nside_get(map), hpic_order_get(map),
                     hpic_coord_get(map), map->mem);
  if (!fmap) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate float map", NULL);
  }
  hpic_float_name_set(fmap, hpic_name_get(map));
  hpic_float_units_set(fmap, hpic_units_get(map));
  for (i = 0; i < (map->npix); i++) {
    if (hpic_is_dnull(hpic_get(map, i))) {
      hpic_float_set(fmap, i, HPIC_NULL);
    } else {
      hpic_float_set(fmap, i, (float)hpic_get(map, i));
    }
  }
  return fmap;
}

hpic_float *hpic_int2float(hpic_int * map)
{
  size_t i;
  hpic_float *fmap = NULL;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  fmap =
    hpic_float_alloc(hpic_int_nside_get(map), hpic_int_order_get(map),
                     hpic_int_coord_get(map), map->mem);
  if (!fmap) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate float map", NULL);
  }
  hpic_float_name_set(fmap, hpic_int_name_get(map));
  hpic_float_units_set(fmap, hpic_int_units_get(map));
  for (i = 0; i < (map->npix); i++) {
    if (hpic_is_inull(hpic_int_get(map, i))) {
      hpic_float_set(fmap, i, HPIC_NULL);
    } else {
      hpic_float_set(fmap, i, (float)hpic_int_get(map, i));
    }
  }
  return fmap;
}

hpic_int *hpic_double2int(hpic * map)
{
  size_t i;
  hpic_int *imap = NULL;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  imap =
    hpic_int_alloc(hpic_nside_get(map), hpic_order_get(map),
                   hpic_coord_get(map), map->mem);
  if (!imap) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate int map", NULL);
  }
  hpic_int_name_set(imap, hpic_name_get(map));
  hpic_int_units_set(imap, hpic_units_get(map));
  for (i = 0; i < (map->npix); i++) {
    if (hpic_is_dnull(hpic_get(map, i))) {
      hpic_int_set(imap, i, HPIC_INT_NULL);
    } else {
      hpic_int_set(imap, i, (int)hpic_get(map, i));
    }
  }
  return imap;
}

hpic_int *hpic_float2int(hpic_float * map)
{
  size_t i;
  hpic_int *imap = NULL;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  imap =
    hpic_int_alloc(hpic_float_nside_get(map), hpic_float_order_get(map),
                   hpic_float_coord_get(map), map->mem);
  if (!imap) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate int map", NULL);
  }
  hpic_int_name_set(imap, hpic_float_name_get(map));
  hpic_int_units_set(imap, hpic_float_units_get(map));
  for (i = 0; i < (map->npix); i++) {
    if (hpic_is_fnull(hpic_float_get(map, i))) {
      hpic_int_set(imap, i, HPIC_INT_NULL);
    } else {
      hpic_int_set(imap, i, (int)hpic_float_get(map, i));
    }
  }
  return imap;
}

/* map order conversions */

int hpic_conv_nest(hpic * map)
{
  return hpic_conv_nestcopy(map);
}

int hpic_conv_nestcopy(hpic * map)
{
  hpic *copy = NULL;
  size_t i;
  size_t new;

  if (!map) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "input map is NULL");
  }
  if ((hpic_order_get(map) == HPIC_NEST) || (hpic_nside_get(map) == 1)) {
    return 0;
  }

  copy =
    hpic_alloc(hpic_nside_get(map), HPIC_NEST, hpic_coord_get(map), map->mem);
  if (!copy) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate temp map");
  }

  for (i = 0; i < hpic_npix_get(map); i++) {
    hpic_ring2nest(hpic_nside_get(map), i, &new);
    hpic_set(copy, new, hpic_get(map, i));
  }
  for (i = 0; i < hpic_npix_get(map); i++) {
    hpic_set(map, i, hpic_get(copy, i));
  }
  map->order = HPIC_NEST;

  hpic_free(copy);
  return 0;
}

int hpic_conv_float_nest(hpic_float * map)
{
  return hpic_conv_float_nestcopy(map);
}

int hpic_conv_float_nestcopy(hpic_float * map)
{
  hpic_float *copy = NULL;
  size_t i;
  size_t new;

  if (!map) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "input map is NULL");
  }
  if ((hpic_float_order_get(map) == HPIC_NEST) ||
      (hpic_float_nside_get(map) == 1)) {
    return 0;
  }

  copy =
    hpic_float_alloc(hpic_float_nside_get(map), HPIC_NEST,
                     hpic_float_coord_get(map), map->mem);
  if (!copy) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate temp map");
  }

  for (i = 0; i < hpic_float_npix_get(map); i++) {
    hpic_ring2nest(hpic_float_nside_get(map), i, &new);
    hpic_float_set(copy, new, hpic_float_get(map, i));
  }
  for (i = 0; i < hpic_float_npix_get(map); i++) {
    hpic_float_set(map, i, hpic_float_get(copy, i));
  }
  map->order = HPIC_NEST;

  hpic_float_free(copy);
  return 0;
}

int hpic_conv_int_nest(hpic_int * map)
{
  return hpic_conv_int_nestcopy(map);
}

int hpic_conv_int_nestcopy(hpic_int * map)
{
  hpic_int *copy = NULL;
  size_t i;
  size_t new;

  if (!map) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "input map is NULL");
  }
  if ((hpic_int_order_get(map) == HPIC_NEST) ||
      (hpic_int_nside_get(map) == 1)) {
    return 0;
  }

  copy =
    hpic_int_alloc(hpic_int_nside_get(map), HPIC_NEST,
                   hpic_int_coord_get(map), map->mem);
  if (!copy) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate temp map");
  }

  for (i = 0; i < hpic_int_npix_get(map); i++) {
    hpic_ring2nest(hpic_int_nside_get(map), i, &new);
    hpic_int_set(copy, new, hpic_int_get(map, i));
  }
  for (i = 0; i < hpic_int_npix_get(map); i++) {
    hpic_int_set(map, i, hpic_int_get(copy, i));
  }
  map->order = HPIC_NEST;

  hpic_int_free(copy);
  return 0;
}

int hpic_conv_ring(hpic * map)
{
  return hpic_conv_ringcopy(map);
}

int hpic_conv_ringcopy(hpic * map)
{
  hpic *copy = NULL;
  size_t i;
  size_t new;

  if (!map) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "input map is NULL");
  }
  if ((hpic_order_get(map) == HPIC_RING) || (hpic_nside_get(map) == 1)) {
    return 0;
  }

  copy =
    hpic_alloc(hpic_nside_get(map), HPIC_RING, hpic_coord_get(map), map->mem);
  if (!copy) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate temp map");
  }

  for (i = 0; i < hpic_npix_get(map); i++) {
    hpic_nest2ring(hpic_nside_get(map), i, &new);
    hpic_set(copy, new, hpic_get(map, i));
  }
  for (i = 0; i < hpic_npix_get(map); i++) {
    hpic_set(map, i, hpic_get(copy, i));
  }
  map->order = HPIC_RING;

  hpic_free(copy);
  return 0;
}

int hpic_conv_float_ring(hpic_float * map)
{
  return hpic_conv_float_ringcopy(map);
}

int hpic_conv_float_ringcopy(hpic_float * map)
{
  hpic_float *copy = NULL;
  size_t i;
  size_t new;

  if (!map) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "input map is NULL");
  }
  if ((hpic_float_order_get(map) == HPIC_RING) ||
      (hpic_float_nside_get(map) == 1)) {
    return 0;
  }

  copy =
    hpic_float_alloc(hpic_float_nside_get(map), HPIC_RING,
                     hpic_float_coord_get(map), map->mem);
  if (!copy) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate temp map");
  }

  for (i = 0; i < hpic_float_npix_get(map); i++) {
    hpic_nest2ring(hpic_float_nside_get(map), i, &new);
    hpic_float_set(copy, new, hpic_float_get(map, i));
  }
  for (i = 0; i < hpic_float_npix_get(map); i++) {
    hpic_float_set(map, i, hpic_float_get(copy, i));
  }
  map->order = HPIC_RING;

  hpic_float_free(copy);
  return 0;
}

int hpic_conv_int_ring(hpic_int * map)
{
  return hpic_conv_int_ringcopy(map);
}

int hpic_conv_int_ringcopy(hpic_int * map)
{
  hpic_int *copy = NULL;
  size_t i;
  size_t new;

  if (!map) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "input map is NULL");
  }
  if ((hpic_int_order_get(map) == HPIC_RING) ||
      (hpic_int_nside_get(map) == 1)) {
    return 0;
  }

  copy =
    hpic_int_alloc(hpic_int_nside_get(map), HPIC_RING,
                   hpic_int_coord_get(map), map->mem);
  if (!copy) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate temp map");
  }

  for (i = 0; i < hpic_int_npix_get(map); i++) {
    hpic_nest2ring(hpic_int_nside_get(map), i, &new);
    hpic_int_set(copy, new, hpic_int_get(map, i));
  }
  for (i = 0; i < hpic_int_npix_get(map); i++) {
    hpic_int_set(map, i, hpic_int_get(copy, i));
  }
  map->order = HPIC_RING;

  hpic_int_free(copy);
  return 0;
}

/* degrade and prograde */

hpic *hpic_conv_xgrade(hpic * map, size_t newnside)
{
  hpic *newmap = NULL;
  hpic_int *hits = NULL;
  int order;
  size_t i;
  size_t temp;
  int err;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  err = hpic_nsidecheck(newnside);
  if (err) {
    HPIC_ERROR_VAL(err, "illegal new nside", NULL);
  }

  if (newnside == hpic_nside_get(map)) {
    newmap = hpic_copy(map);
    if (!newmap) {
      HPIC_ERROR_VAL(err, "cannot allocate new map", NULL);
    }
    return newmap;
  }

  order = hpic_order_get(map);
  newmap = hpic_alloc(newnside, order, hpic_coord_get(map), map->mem);
  if (!newmap) {
    HPIC_ERROR_VAL(err, "cannot allocate new map", NULL);
  }
  hpic_name_set(newmap, hpic_name_get(map));
  hpic_units_set(newmap, hpic_units_get(map));

  if (newnside > hpic_nside_get(map)) { /*prograde */
    for (i = 0; i < (hpic_npix_get(newmap)); i++) {
      if (order == HPIC_NEST) {
        hpic_degrade_nest(newnside, i, hpic_nside_get(map), &temp);
      } else {
        hpic_degrade_ring(newnside, i, hpic_nside_get(map), &temp);
      }
      hpic_set(newmap, i, hpic_get(map, temp));
    }
  } else {                      /*degrade */
    hits = hpic_int_alloc(newnside, order, hpic_coord_get(map), map->mem);
    for (i = 0; i < (hpic_npix_get(map)); i++) {
      if (!hpic_is_dnull(hpic_get(map, i))) {
        if (order == HPIC_NEST) {
          hpic_degrade_nest(hpic_nside_get(map), i, newnside, &temp);
        } else {
          hpic_degrade_ring(hpic_nside_get(map), i, newnside, &temp);
        }
        if (!hpic_is_dnull(hpic_get(newmap, temp))) {
          hpic_set(newmap, temp, hpic_get(newmap, temp) + hpic_get(map, i));
          hpic_int_set(hits, temp, hpic_int_get(hits, temp) + 1);
        } else {
          hpic_set(newmap, temp, hpic_get(map, i));
          hpic_int_set(hits, temp, 1);
        }
      }
    }
    for (i = 0; i < (hpic_npix_get(newmap)); i++) {
      if (!hpic_is_dnull(hpic_get(newmap, i))) {
        hpic_set(newmap, i,
                 hpic_get(newmap, i) / ((double)hpic_int_get(hits, i)));
      }
    }
    hpic_int_free(hits);
  }
  return newmap;
}

hpic_float *hpic_conv_float_xgrade(hpic_float * map, size_t newnside)
{
  hpic_float *newmap = NULL;
  hpic_int *hits = NULL;
  int order;
  size_t i;
  size_t temp;
  int err;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  err = hpic_nsidecheck(newnside);
  if (err) {
    HPIC_ERROR_VAL(err, "illegal new nside", NULL);
  }

  if (newnside == hpic_float_nside_get(map)) {
    newmap = hpic_float_copy(map);
    if (!newmap) {
      HPIC_ERROR_VAL(err, "cannot allocate new map", NULL);
    }
    return newmap;
  }

  order = hpic_float_order_get(map);
  newmap =
    hpic_float_alloc(newnside, order, hpic_float_coord_get(map), map->mem);
  if (!newmap) {
    HPIC_ERROR_VAL(err, "cannot allocate new map", NULL);
  }
  hpic_float_name_set(newmap, hpic_float_name_get(map));
  hpic_float_units_set(newmap, hpic_float_units_get(map));

  if (newnside > hpic_float_nside_get(map)) {   /*prograde */
    for (i = 0; i < (hpic_float_npix_get(newmap)); i++) {
      if (order == HPIC_NEST) {
        hpic_degrade_nest(newnside, i, hpic_float_nside_get(map), &temp);
      } else {
        hpic_degrade_ring(newnside, i, hpic_float_nside_get(map), &temp);
      }
      hpic_float_set(newmap, i, hpic_float_get(map, temp));
    }
  } else {                      /*degrade */
    hits =
      hpic_int_alloc(newnside, order, hpic_float_coord_get(map), map->mem);
    for (i = 0; i < (hpic_float_npix_get(map)); i++) {
      if (!hpic_is_fnull(hpic_float_get(map, i))) {
        if (order == HPIC_NEST) {
          hpic_degrade_nest(hpic_float_nside_get(map), i, newnside, &temp);
        } else {
          hpic_degrade_ring(hpic_float_nside_get(map), i, newnside, &temp);
        }
        if (!hpic_is_fnull(hpic_float_get(newmap, temp))) {
          hpic_float_set(newmap, temp,
                         hpic_float_get(newmap, temp) + hpic_float_get(map,
                                                                       i));
          hpic_int_set(hits, temp, hpic_int_get(hits, temp) + 1);
        } else {
          hpic_float_set(newmap, temp, hpic_float_get(map, i));
          hpic_int_set(hits, temp, 1);
        }
      }
    }
    for (i = 0; i < (hpic_float_npix_get(newmap)); i++) {
      if (!hpic_is_fnull(hpic_float_get(newmap, i))) {
        hpic_float_set(newmap, i,
                       hpic_float_get(newmap,
                                      i) / ((float)hpic_int_get(hits, i)));
      }
    }
    hpic_int_free(hits);
  }
  return newmap;
}

hpic_int *hpic_conv_int_xgrade(hpic_int * map, size_t newnside)
{
  hpic_int *newmap = NULL;
  hpic_int *hits = NULL;
  int order;
  size_t i;
  size_t temp;
  int err;

  if (!map) {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "input map is NULL", NULL);
  }
  err = hpic_nsidecheck(newnside);
  if (err) {
    HPIC_ERROR_VAL(err, "illegal new nside", NULL);
  }

  if (newnside == hpic_int_nside_get(map)) {
    newmap = hpic_int_copy(map);
    if (!newmap) {
      HPIC_ERROR_VAL(err, "cannot allocate new map", NULL);
    }
    return newmap;
  }

  order = hpic_int_order_get(map);
  newmap = hpic_int_alloc(newnside, order, hpic_int_coord_get(map), map->mem);
  if (!newmap) {
    HPIC_ERROR_VAL(err, "cannot allocate new map", NULL);
  }
  hpic_int_name_set(newmap, hpic_int_name_get(map));
  hpic_int_units_set(newmap, hpic_int_units_get(map));

  if (newnside > hpic_int_nside_get(map)) {     /*prograde */
    for (i = 0; i < (hpic_int_npix_get(newmap)); i++) {
      if (order == HPIC_NEST) {
        hpic_degrade_nest(newnside, i, hpic_int_nside_get(map), &temp);
      } else {
        hpic_degrade_ring(newnside, i, hpic_int_nside_get(map), &temp);
      }
      hpic_int_set(newmap, i, hpic_int_get(map, temp));
    }
  } else {                      /*degrade */
    hits = hpic_int_alloc(newnside, order, hpic_int_coord_get(map), map->mem);
    for (i = 0; i < (hpic_int_npix_get(map)); i++) {
      if (!hpic_is_inull(hpic_int_get(map, i))) {
        if (order == HPIC_NEST) {
          hpic_degrade_nest(hpic_int_nside_get(map), i, newnside, &temp);
        } else {
          hpic_degrade_ring(hpic_int_nside_get(map), i, newnside, &temp);
        }
        if (!hpic_is_inull(hpic_int_get(newmap, temp))) {
          hpic_int_set(newmap, temp,
                       hpic_int_get(newmap, temp) + hpic_int_get(map, i));
          hpic_int_set(hits, temp, hpic_int_get(hits, temp) + 1);
        } else {
          hpic_int_set(newmap, temp, hpic_int_get(map, i));
          hpic_int_set(hits, temp, 1);
        }
      }
    }
    for (i = 0; i < (hpic_int_npix_get(newmap)); i++) {
      if (!hpic_is_inull(hpic_int_get(newmap, i))) {
        hpic_int_set(newmap, i,
                     (int)(hpic_int_get(newmap, i) / hpic_int_get(hits, i)));
      }
    }
    hpic_int_free(hits);
  }
  return newmap;
}

