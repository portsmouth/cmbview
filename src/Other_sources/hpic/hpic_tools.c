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
 * This code deals with low level tools.                                     *
 *****************************************************************************/

#include <hpic.h>

/* NULL checking */

int hpic_is_dnull(double val)
{
  if ((val > HPIC_NULL - HPIC_EPSILON) && (val < HPIC_NULL + HPIC_EPSILON)) {
    return 1;
  } else {
    return 0;
  }
}

int hpic_is_fnull(float val)
{
  if ((val > HPIC_NULL - HPIC_EPSILON) && (val < HPIC_NULL + HPIC_EPSILON)) {
    return 1;
  } else {
    return 0;
  }
}

int hpic_is_inull(int val)
{
  if (val == HPIC_INT_NULL) {
    return 1;
  } else {
    return 0;
  }
}

/* simple tools */

char **hpic_strarr_alloc(size_t nstring)
{
  size_t i,j;
  char **array = NULL;
  if (nstring == 0) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate strarr of size zero",NULL);
  }
  array = (char **)calloc(nstring, sizeof(char *));
  if (!array) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate array",NULL);
  }
  for (i = 0; i < nstring; i++) {
    array[i] = NULL;
    array[i] = (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!array[i]) {
      for (j = 0; j < i; j++) {
        free(array[j]);
      }
      free(array);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC,"cannot allocate strarr element",NULL);
    }
  }
  return array;
}

int hpic_strarr_free(char **array, size_t nstring)
{
  size_t i;
  if ((array == NULL)||(nstring == 0)) {
    HPIC_ERROR(HPIC_ERR_FREE,"cannot free non-allocated array");
  }
  for (i = 0; i < nstring; i++) {
    free(array[i]);
  }
  free(array);
  return 0;
}

int hpic_nsidecheck(size_t nside)
{
  if (nside > HPIC_NSIDE_MAX) {
    return 1;
  }
  if (nside == 0) {
    return 1;
  }
  if (((~nside)&(nside-1)) != (nside-1)) {  
    return 1;
  }
  return 0;
}

size_t hpic_npix2nside(size_t npix)
{
  double fpix;
  int err;
  fpix = ((double)npix) / 12.0;
  fpix = sqrt(fpix);
  err = hpic_nsidecheck((size_t) fpix);
  if (err) {
    HPIC_ERROR_VAL(err,"npix value is not valid",0);
  }
  return (size_t) fpix;
}

size_t hpic_nside2npix(size_t nside)
{
  int err;
  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR_VAL(err,"nside value is not valid",0);
  }
  return 12 * nside * nside;
}

size_t hpic_nside2factor(size_t nside)
{
  int err;
  size_t factor = 0;
  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR_VAL(err,"nside value is not valid",0);
  }
  while (nside != (1ul<<factor)) {
    factor++;
  }
  return factor;
}

size_t hpic_factor2nside(size_t factor)
{
  size_t nside = 1 << factor;
  if (nside > HPIC_NSIDE_MAX) {
    HPIC_ERROR_VAL(HPIC_ERR_NSIDE,"factor is not valid",0);
  }
  return nside;
}

/* unit sphere vectors */

int hpic_vec2ang(double xcomp, double ycomp, double zcomp, double *theta,
             double *phi)
{
  double TOL = HPIC_PI/(1000.0*4.0*HPIC_NSIDE_MAX);
  (*theta) = atan2(sqrt(xcomp*xcomp+ycomp*ycomp), zcomp);
  (*phi) = 0.0;
  if (fabs(*theta) > TOL) {
    (*phi) = atan2(ycomp, xcomp);
    if ((*phi) < 0.0) {
      (*phi) = (*phi) + 2*HPIC_PI;
    }
  }
  return 0;
}

int hpic_ang2vec(double theta, double phi, double *xcomp, double *ycomp,
             double *zcomp)
{
  (*xcomp) = sin(theta) * cos(phi);
  (*ycomp) = sin(theta) * sin(phi);
  (*zcomp) = cos(theta);
  return 0;
}

int hpic_pix2vec_ring(size_t nside, size_t pix, double *xcomp, double *ycomp,
                  double *zcomp)
{
  double theta;
  double phi;
  int err;
  err = hpic_pix2ang_ring(nside, pix, &theta, &phi);
  if (err) {
    HPIC_ERROR(err,"pix2ang conversion");
  }
  err = hpic_ang2vec(theta, phi, xcomp, ycomp, zcomp);
  if (err) {
    HPIC_ERROR(err,"ang2vec conversion");
  }
  return 0;
}

int hpic_pix2vec_nest(size_t nside, size_t pix, double *xcomp, double *ycomp,
                  double *zcomp)
{
  double theta;
  double phi;
  int err;
  err = hpic_pix2ang_nest(nside, pix, &theta, &phi);
  if (err) {
    HPIC_ERROR(err,"pix2ang conversion");
  }
  err = hpic_ang2vec(theta, phi, xcomp, ycomp, zcomp);
  if (err) {
    HPIC_ERROR(err,"ang2vec conversion");
  }
  return 0;
}

int hpic_vec2pix_ring(size_t nside, double xcomp, double ycomp, double zcomp, size_t * pix)
{
  double theta;
  double phi;
  int err;
  err = hpic_vec2ang(xcomp, ycomp, zcomp, &theta, &phi);
  if (err) {
    HPIC_ERROR(err,"vec2ang conversion");
  }
  err = hpic_ang2pix_ring(nside, theta, phi, pix);
  if (err) {
    HPIC_ERROR(err,"ang2pix conversion");
  }
  return 0;
}

int hpic_vec2pix_nest(size_t nside, double xcomp, double ycomp, double zcomp, size_t * pix)
{
  double theta;
  double phi;
  int err;
  err = hpic_vec2ang(xcomp, ycomp, zcomp, &theta, &phi);
  if (err) {
    HPIC_ERROR(err,"vec2ang conversion");
  }
  err = hpic_ang2pix_nest(nside, theta, phi, pix);
  if (err) {
    HPIC_ERROR(err,"ang2pix conversion");
  }
  return 0;
}

int hpic_degrade_nest(size_t oldnside, size_t oldpix, size_t newnside,
                  size_t * newpix)
{
  size_t oldfactor;
  size_t newfactor;
  size_t face;
  size_t relpix;
  int err;

  err = hpic_nsidecheck(oldnside);
  if (err) {
    HPIC_ERROR(err,"old nside value is not valid");
  }
  err = hpic_nsidecheck(newnside);
  if (err) {
    HPIC_ERROR(err,"new nside value is not valid");
  }
  if (oldnside < newnside) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "new nside must be <= old nside");
  }
  oldfactor = hpic_nside2factor(oldnside);
  newfactor = hpic_nside2factor(newnside);
  face = oldpix >> 2*oldfactor;
  
  (*newpix) = ((oldpix & (oldnside*oldnside-1)) >> (2*(oldfactor-newfactor))) + (face << 2*newfactor);
  /*
  printf(" degrade:  oldpix = %lu face = %lu newpix = %lu\n", oldpix, face, (*newpix));
  fflush(stdout);
  */
  return 0;
}

int hpic_degrade_ring(size_t oldnside, size_t oldpix, size_t newnside,
                  size_t * newpix)
{
  size_t oldnest;
  size_t newnest;
  int err;

  err = hpic_ring2nest(oldnside, oldpix, &oldnest);
  if (err) {
    HPIC_ERROR(err,"ring2nest conversion");
  }
  err = hpic_degrade_nest(oldnside, oldnest, newnside, &newnest);
  if (err) {
    HPIC_ERROR(err,"degrade_nest operation");
  }
  err = hpic_nest2ring(newnside, newnest, newpix);
  if (err) {
    HPIC_ERROR(err,"nest2ring conversion");
  }

  return 0;
}

