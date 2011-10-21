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
 * This code deals with pixel location sets                                  *
 *****************************************************************************/

#include <hpic.h>

/* angular distance between two pixels */

double hpic_loc_dist(size_t nside, int order, size_t pix1, size_t pix2)
{
  double x1, y1_local, z1;
  double x2, y2, z2;
  double alpha;
  int err;

  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR_VAL(err, "nside value is not valid", 0.0);
  }
  if (pix1 > 12 * nside * nside) {
    HPIC_ERROR_VAL(HPIC_ERR_RANGE, "pixel1 value is out of range", 0.0);
  }
  if (pix2 > 12 * nside * nside) {
    HPIC_ERROR_VAL(HPIC_ERR_RANGE, "pixel2 value is out of range", 0.0);
  }
  if (order == HPIC_NEST) {
    err = hpic_pix2vec_nest(nside, pix1, &x1, &y1_local, &z1);
    if (err) {
      HPIC_ERROR_VAL(err, "pix2vec pixel1", 0.0);
    }
    err = hpic_pix2vec_nest(nside, pix2, &x2, &y2, &z2);
    if (err) {
      HPIC_ERROR_VAL(err, "pix2vec pixel2", 0.0);
    }
  } else {
    err = hpic_pix2vec_ring(nside, pix1, &x1, &y1_local, &z1);
    if (err) {
      HPIC_ERROR_VAL(err, "pix2vec pixel1", 0.0);
    }
    err = hpic_pix2vec_ring(nside, pix2, &x2, &y2, &z2);
    if (err) {
      HPIC_ERROR_VAL(err, "pix2vec pixel2", 0.0);
    }
  }
  alpha = acos((x1 * x2) + (y1_local * y2) + (z1 * z2));

  return alpha;
}

