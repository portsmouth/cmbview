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
 * This code deals with basic map math                                       *
 *****************************************************************************/

#include <hpic.h>

/* scaling and offsets */

int hpic_scale(hpic * map, double val)
{
  size_t i;
  if (map) {
    for (i = 0; i < hpic_npix_get(map); i++) {
      if (!hpic_is_dnull(hpic_get(map, i))) {
        hpic_set(map, i, val * hpic_get(map, i));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
  return 0;
}

int hpic_float_scale(hpic_float * map, double val)
{
  size_t i;
  if (map) {
    for (i = 0; i < hpic_float_npix_get(map); i++) {
      if (!hpic_is_fnull(hpic_float_get(map, i))) {
        hpic_float_set(map, i, (float)(val * (double)hpic_float_get(map, i)));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
  return 0;
}

int hpic_int_scale(hpic_int * map, double val)
{
  size_t i;
  if (map) {
    for (i = 0; i < hpic_int_npix_get(map); i++) {
      if (!hpic_is_inull(hpic_int_get(map, i))) {
        hpic_int_set(map, i, (int)(val * (double)hpic_int_get(map, i)));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
  return 0;
}

int hpic_offset(hpic * map, double val)
{
  size_t i;
  if (map) {
    for (i = 0; i < hpic_npix_get(map); i++) {
      if (!hpic_is_dnull(hpic_get(map, i))) {
        hpic_set(map, i, val + hpic_get(map, i));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
  return 0;
}

int hpic_float_offset(hpic_float * map, float val)
{
  size_t i;
  if (map) {
    for (i = 0; i < hpic_float_npix_get(map); i++) {
      if (!hpic_is_fnull(hpic_float_get(map, i))) {
        hpic_float_set(map, i, val + hpic_float_get(map, i));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
  return 0;
}

int hpic_int_offset(hpic_int * map, int val)
{
  size_t i;
  if (map) {
    for (i = 0; i < hpic_int_npix_get(map); i++) {
      if (!hpic_is_inull(hpic_int_get(map, i))) {
        hpic_int_set(map, i, val + hpic_int_get(map, i));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }
  return 0;
}

/* basic math */

int hpic_add(hpic * first, hpic * second, int mode)
{
  size_t i;
  hpic *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_nside_get(first) != hpic_nside_get(second)) ||
      (hpic_order_get(first) != hpic_order_get(second))) {
    sameres = hpic_conv_xgrade(second, hpic_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_order_get(first) == HPIC_RING) {
      err = hpic_conv_ringcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_nestcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if ((hpic_is_dnull(hpic_get(first, i))) || (hpic_is_dnull(hpic_get(sameres, i)))) {
        hpic_set(first, i, HPIC_NULL);
      } else {
        hpic_set(first, i, hpic_get(first, i) + hpic_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if (hpic_is_dnull(hpic_get(first, i))) {
        hpic_set(first, i, hpic_get(sameres, i));
      } else if (!hpic_is_dnull(hpic_get(sameres, i))) {
        hpic_set(first, i, hpic_get(first, i) + hpic_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_free(sameres);
  }

  return 0;
}

int hpic_float_add(hpic_float * first, hpic_float * second, int mode)
{
  size_t i;
  hpic_float *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_float_nside_get(first) != hpic_float_nside_get(second)) ||
      (hpic_float_order_get(first) != hpic_float_order_get(second))) {
    sameres = hpic_conv_float_xgrade(second, hpic_float_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_float_order_get(first) == HPIC_RING) {
      err = hpic_conv_float_ringcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_float_nestcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if ((hpic_is_fnull(hpic_float_get(first, i))) ||
          (hpic_is_fnull(hpic_float_get(sameres, i)))) {
        hpic_float_set(first, i, HPIC_NULL);
      } else {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) + hpic_float_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if (hpic_is_fnull(hpic_float_get(first, i))) {
        hpic_float_set(first, i, hpic_float_get(sameres, i));
      } else if (!hpic_is_fnull(hpic_float_get(sameres, i))) {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) + hpic_float_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_float_free(sameres);
  }

  return 0;
}

int hpic_int_add(hpic_int * first, hpic_int * second, int mode)
{
  size_t i;
  hpic_int *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_int_nside_get(first) != hpic_int_nside_get(second)) ||
      (hpic_int_order_get(first) != hpic_int_order_get(second))) {
    sameres = hpic_conv_int_xgrade(second, hpic_int_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_int_order_get(first) == HPIC_RING) {
      err = hpic_conv_int_ringcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_int_nestcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if ((hpic_is_inull(hpic_int_get(first, i))) ||
          (hpic_is_inull(hpic_int_get(sameres, i)))) {
        hpic_int_set(first, i, HPIC_INT_NULL);
      } else {
        hpic_int_set(first, i,
                     hpic_int_get(first, i) + hpic_int_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if (hpic_is_inull(hpic_int_get(first, i))) {
        hpic_int_set(first, i, hpic_int_get(sameres, i));
      } else if (!hpic_is_inull(hpic_int_get(sameres, i))) {
        hpic_int_set(first, i,
                     hpic_int_get(first, i) + hpic_int_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_int_free(sameres);
  }

  return 0;
}

int hpic_subtract(hpic * first, hpic * second, int mode)
{
  size_t i;
  hpic *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_nside_get(first) != hpic_nside_get(second)) ||
      (hpic_order_get(first) != hpic_order_get(second))) {
    sameres = hpic_conv_xgrade(second, hpic_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_order_get(first) == HPIC_RING) {
      err = hpic_conv_ringcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_nestcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if ((hpic_is_dnull(hpic_get(first, i))) || (hpic_is_dnull(hpic_get(sameres, i)))) {
        hpic_set(first, i, HPIC_NULL);
      } else {
        hpic_set(first, i, hpic_get(first, i) - hpic_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if (hpic_is_dnull(hpic_get(first, i))) {
        hpic_set(first, i, -hpic_get(sameres, i));
      } else if (!hpic_is_dnull(hpic_get(sameres, i))) {
        hpic_set(first, i, hpic_get(first, i) - hpic_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_free(sameres);
  }

  return 0;
}

int hpic_float_subtract(hpic_float * first, hpic_float * second, int mode)
{
  size_t i;
  hpic_float *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_float_nside_get(first) != hpic_float_nside_get(second)) ||
      (hpic_float_order_get(first) != hpic_float_order_get(second))) {
    sameres = hpic_conv_float_xgrade(second, hpic_float_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_float_order_get(first) == HPIC_RING) {
      err = hpic_conv_float_ringcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_float_nestcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if ((hpic_is_fnull(hpic_float_get(first, i))) ||
          (hpic_is_fnull(hpic_float_get(sameres, i)))) {
        hpic_float_set(first, i, HPIC_NULL);
      } else {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) - hpic_float_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if (hpic_is_fnull(hpic_float_get(first, i))) {
        hpic_float_set(first, i, -hpic_float_get(sameres, i));
      } else if (!hpic_is_fnull(hpic_float_get(sameres, i))) {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) - hpic_float_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_float_free(sameres);
  }

  return 0;
}

int hpic_int_subtract(hpic_int * first, hpic_int * second, int mode)
{
  size_t i;
  hpic_int *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_int_nside_get(first) != hpic_int_nside_get(second)) ||
      (hpic_int_order_get(first) != hpic_int_order_get(second))) {
    sameres = hpic_conv_int_xgrade(second, hpic_int_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_int_order_get(first) == HPIC_RING) {
      err = hpic_conv_int_ringcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_int_nestcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if ((hpic_is_inull(hpic_int_get(first, i))) ||
          (hpic_is_inull(hpic_int_get(sameres, i)))) {
        hpic_int_set(first, i, HPIC_INT_NULL);
      } else {
        hpic_int_set(first, i,
                     hpic_int_get(first, i) - hpic_int_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if (hpic_is_inull(hpic_int_get(first, i))) {
        hpic_int_set(first, i, -hpic_int_get(sameres, i));
      } else if (!hpic_is_inull(hpic_int_get(sameres, i))) {
        hpic_int_set(first, i,
                     hpic_int_get(first, i) - hpic_int_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_int_free(sameres);
  }

  return 0;
}

int hpic_multiply(hpic * first, hpic * second, int mode)
{
  size_t i;
  hpic *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_nside_get(first) != hpic_nside_get(second)) ||
      (hpic_order_get(first) != hpic_order_get(second))) {
    sameres = hpic_conv_xgrade(second, hpic_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_order_get(first) == HPIC_RING) {
      err = hpic_conv_ringcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_nestcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if ((hpic_is_dnull(hpic_get(first, i))) || (hpic_is_dnull(hpic_get(sameres, i)))) {
        hpic_set(first, i, HPIC_NULL);
      } else {
        hpic_set(first, i, hpic_get(first, i) * hpic_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if (hpic_is_dnull(hpic_get(first, i))) {
        hpic_set(first, i, hpic_get(sameres, i));
      } else if (!hpic_is_dnull(hpic_get(sameres, i))) {
        hpic_set(first, i, hpic_get(first, i) * hpic_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_free(sameres);
  }

  return 0;
}

int hpic_float_multiply(hpic_float * first, hpic_float * second, int mode)
{
  size_t i;
  hpic_float *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_float_nside_get(first) != hpic_float_nside_get(second)) ||
      (hpic_float_order_get(first) != hpic_float_order_get(second))) {
    sameres = hpic_conv_float_xgrade(second, hpic_float_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_float_order_get(first) == HPIC_RING) {
      err = hpic_conv_float_ringcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_float_nestcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if ((hpic_is_fnull(hpic_float_get(first, i))) ||
          (hpic_is_fnull(hpic_float_get(sameres, i)))) {
        hpic_float_set(first, i, HPIC_NULL);
      } else {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) * hpic_float_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if (hpic_is_fnull(hpic_float_get(first, i))) {
        hpic_float_set(first, i, hpic_float_get(sameres, i));
      } else if (!hpic_is_fnull(hpic_float_get(sameres, i))) {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) * hpic_float_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_float_free(sameres);
  }

  return 0;
}
 
int hpic_int_multiply(hpic_int * first, hpic_int * second, int mode)
{
  size_t i;
  hpic_int *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_int_nside_get(first) != hpic_int_nside_get(second)) ||
      (hpic_int_order_get(first) != hpic_int_order_get(second))) {
    sameres = hpic_conv_int_xgrade(second, hpic_int_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_int_order_get(first) == HPIC_RING) {
      err = hpic_conv_int_ringcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_int_nestcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if ((hpic_is_inull(hpic_int_get(first, i))) ||
          (hpic_is_inull(hpic_int_get(sameres, i)))) {
        hpic_int_set(first, i, HPIC_INT_NULL);
      } else {
        hpic_int_set(first, i,
                     hpic_int_get(first, i) * hpic_int_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if (hpic_is_inull(hpic_int_get(first, i))) {
        hpic_int_set(first, i, hpic_int_get(sameres, i));
      } else if (!hpic_is_inull(hpic_int_get(sameres, i))) {
        hpic_int_set(first, i,
                     hpic_int_get(first, i) * hpic_int_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_int_free(sameres);
  }

  return 0;
}

int hpic_divide(hpic * first, hpic * second, int mode)
{
  size_t i;
  hpic *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_nside_get(first) != hpic_nside_get(second)) ||
      (hpic_order_get(first) != hpic_order_get(second))) {
    sameres = hpic_conv_xgrade(second, hpic_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_order_get(first) == HPIC_RING) {
      err = hpic_conv_ringcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_nestcopy(sameres);
      if (err) {
        hpic_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if ((hpic_is_dnull(hpic_get(first, i))) || (hpic_is_dnull(hpic_get(sameres, i)))) {
        hpic_set(first, i, HPIC_NULL);
      } else {
        hpic_set(first, i, hpic_get(first, i) / hpic_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_npix_get(first); i++) {
      if (hpic_is_dnull(hpic_get(first, i))) {
        hpic_set(first, i, 1.0 / hpic_get(sameres, i));
      } else if (!hpic_is_dnull(hpic_get(sameres, i))) {
        hpic_set(first, i, hpic_get(first, i) / hpic_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_free(sameres);
  }

  return 0;
}

int hpic_float_divide(hpic_float * first, hpic_float * second, int mode)
{
  size_t i;
  hpic_float *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_float_nside_get(first) != hpic_float_nside_get(second)) ||
      (hpic_float_order_get(first) != hpic_float_order_get(second))) {
    sameres = hpic_conv_float_xgrade(second, hpic_float_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_float_order_get(first) == HPIC_RING) {
      err = hpic_conv_float_ringcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_float_nestcopy(sameres);
      if (err) {
        hpic_float_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if ((hpic_is_fnull(hpic_float_get(first, i))) ||
          (hpic_is_fnull(hpic_float_get(sameres, i)))) {
        hpic_float_set(first, i, HPIC_NULL);
      } else {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) / hpic_float_get(sameres, i));
      }
    }
  } else {
    for (i = 0; i < hpic_float_npix_get(first); i++) {
      if (hpic_is_fnull(hpic_float_get(first, i))) {
        hpic_float_set(first, i, 1.0 / hpic_float_get(sameres, i));
      } else if (!hpic_is_fnull(hpic_float_get(sameres, i))) {
        hpic_float_set(first, i,
                       hpic_float_get(first, i) / hpic_float_get(sameres, i));
      }
    }
  }
  if (sameres != second) {
    hpic_float_free(sameres);
  }

  return 0;
}

int hpic_int_divide(hpic_int * first, hpic_int * second, int mode)
{
  size_t i;
  hpic_int *sameres;
  int err;

  if (!first) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "first map pointer is NULL");
  }
  if (!second) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "second map pointer is NULL");
  }
  if ((hpic_int_nside_get(first) != hpic_int_nside_get(second)) ||
      (hpic_int_order_get(first) != hpic_int_order_get(second))) {
    sameres = hpic_conv_int_xgrade(second, hpic_int_nside_get(first));
    if (!sameres) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot degrade second map");
    }
    if (hpic_int_order_get(first) == HPIC_RING) {
      err = hpic_conv_int_ringcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    } else {
      err = hpic_conv_int_nestcopy(sameres);
      if (err) {
        hpic_int_free(sameres);
        HPIC_ERROR(err, "cannot convert ordering");
      }
    }
  } else {
    sameres = second;
  }

  if (mode == HPIC_INTERSECT) {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if ((hpic_is_inull(hpic_int_get(first, i))) ||
          (hpic_is_inull(hpic_int_get(sameres, i)))) {
        hpic_int_set(first, i, HPIC_INT_NULL);
      } else {
        hpic_int_set(first, i,
                     (int)(hpic_int_get(first, i) /
                           hpic_int_get(sameres, i)));
      }
    }
  } else {
    for (i = 0; i < hpic_int_npix_get(first); i++) {
      if (hpic_is_inull(hpic_int_get(first, i))) {
        hpic_int_set(first, i, (int)(1 / hpic_int_get(sameres, i)));
      } else if (!hpic_is_inull(hpic_int_get(sameres, i))) {
        hpic_int_set(first, i,
                     (int)(hpic_int_get(first, i) /
                           hpic_int_get(sameres, i)));
      }
    }
  }
  if (sameres != second) {
    hpic_int_free(sameres);
  }

  return 0;
}

