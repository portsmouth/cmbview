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
 * This is legacy code which will be removed eventually.                     *
 *                                                                           *
 *****************************************************************************/

#include <hpic.h>
#include <hpic_config.h>
#ifdef HAVE_LIBPTHREAD
#  include <pthread.h>
#endif

#define HPIC_LEG if(!hpic_donelegacy)hpic_legacynotice()

void hpic_legacynotice();

void hpic_legacynotice() {
# ifdef HAVE_LIBPTHREAD  
  static pthread_mutex_t leglock = PTHREAD_MUTEX_INITIALIZER;        
  pthread_mutex_lock(&leglock);
# endif  
  fprintf(stderr, "HPIC Legacy Function NOTICE:  \n");
  fprintf(stderr, "     This program uses a deprecated function name.  All\n");
  fprintf(stderr, "     functions now start with hpic_* to avoid namespace\n");
  fprintf(stderr, "     pollution.  Please refer to the NEWS file for\n");
  fprintf(stderr, "     more information on automatically changing your\n");
  fprintf(stderr, "     code to use the new function names.\n");
  hpic_donelegacy = 1;
# ifdef HAVE_LIBPTHREAD  
  pthread_mutex_unlock(&leglock);
# endif
  return;
}

int is_dnull(double val) {
  HPIC_LEG;
  return hpic_is_dnull(val);
}

int is_fnull(float val) {
  HPIC_LEG;
  return hpic_is_fnull(val);
}

int is_inull(int val) {
  HPIC_LEG;
  return hpic_is_inull(val);
}

int nsidecheck(size_t nside) {
  HPIC_LEG;
  return hpic_nsidecheck(nside);
}

size_t nside2npix(size_t nside) {
  HPIC_LEG;
  return hpic_nside2npix(nside);
}

size_t npix2nside(size_t npix) {
  HPIC_LEG;
  return hpic_npix2nside(npix);
}

int nest2ring(size_t nside, size_t pnest, size_t * pring) {
  HPIC_LEG;
  return hpic_nest2ring(nside, pnest, pring);
}

int ring2nest(size_t nside, size_t pring, size_t * pnest) {
  HPIC_LEG;
  return hpic_ring2nest(nside, pring, pnest);
}

int pix2ang_ring(size_t nside, size_t pix, double *theta, double *phi) {
  HPIC_LEG;
  return hpic_pix2ang_ring(nside, pix, theta, phi);
}

int pix2ang_nest(size_t nside, size_t pix, double *theta, double *phi) {
  HPIC_LEG;
  return hpic_pix2ang_nest(nside, pix, theta, phi);
}

int ang2pix_ring(size_t nside, double theta, double phi, size_t * pix) {
  HPIC_LEG;
  return hpic_ang2pix_ring(nside, theta, phi, pix);
}

int ang2pix_nest(size_t nside, double theta, double phi, size_t * pix) {
  HPIC_LEG;
  return hpic_ang2pix_nest(nside, theta, phi, pix);
}

int vec2ang(double xcomp, double ycomp, double zcomp, double *theta, double *phi) {
  HPIC_LEG;
  return hpic_vec2ang(xcomp, ycomp, zcomp, theta, phi);
}

int ang2vec(double theta, double phi, double *xcomp, double *ycomp, double *zcomp) {
  HPIC_LEG;
  return hpic_ang2vec(theta, phi, xcomp, ycomp, zcomp);
}

int pix2vec_ring(size_t nside, size_t pix, double *xcomp, double *ycomp, double *zcomp) {
  HPIC_LEG;
  return hpic_pix2vec_ring(nside, pix, xcomp, ycomp, zcomp);
}

int pix2vec_nest(size_t nside, size_t pix, double *xcomp, double *ycomp, double *zcomp) {
  HPIC_LEG;
  return hpic_pix2vec_nest(nside, pix, xcomp, ycomp, zcomp);
}

int vec2pix_ring(size_t nside, double xcomp, double ycomp, double zcomp, size_t * pix) {
  HPIC_LEG;
  return hpic_vec2pix_ring(nside, xcomp, ycomp, zcomp, pix);
}

int vec2pix_nest(size_t nside, double xcomp, double ycomp, double zcomp, size_t * pix) {
  HPIC_LEG;
  return hpic_vec2pix_nest(nside, xcomp, ycomp, zcomp, pix);
}

int degrade_nest(size_t oldnside, size_t oldpix, size_t newnside, size_t * newpix) {
  HPIC_LEG;
  return hpic_degrade_nest(oldnside, oldpix, newnside, newpix);
}

int degrade_ring(size_t oldnside, size_t oldpix, size_t newnside, size_t * newpix) {
  HPIC_LEG;
  return hpic_degrade_ring(oldnside, oldpix, newnside, newpix);
}

int proj_car(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double theta, double phi, double *x, double *y) {
  HPIC_LEG;
  return hpic_proj_car(mintheta, maxtheta, minphi, maxphi, xmax, ymax, theta, phi, x, y);
}

int proj_sin(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double theta, double phi, double *x, double *y) {
  HPIC_LEG;
  return hpic_proj_sin(mintheta, maxtheta, minphi, maxphi, xmax, ymax, theta, phi, x, y);
}

int proj_rev_car(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double x, double y, double *theta, double *phi) {
  HPIC_LEG;
  return hpic_proj_rev_car(mintheta, maxtheta, minphi, maxphi, xmax, ymax, x, y, theta, phi);
}

int proj_rev_sin(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double x, double y, double *theta, double *phi) {
  HPIC_LEG;
  return hpic_proj_rev_sin(mintheta, maxtheta, minphi, maxphi, xmax, ymax, x, y, theta, phi);
}
  
/* compatibility pixel tools */

void compat_ang2pix_nest(const long nside, double theta, double phi,
                         long *ipix)
{
  size_t pix;
  hpic_ang2pix_nest((size_t) nside, theta, phi, &pix);
  (*ipix) = (long)pix;
  return;
}
void compat_ang2pix_ring(const long nside, double theta, double phi,
                         long *ipix)
{
  size_t pix;
  hpic_ang2pix_ring((size_t) nside, theta, phi, &pix);
  (*ipix) = (long)pix;
  return;
}
void compat_pix2ang_nest(long nside, long ipix, double *theta, double *phi)
{
  hpic_pix2ang_nest((size_t) nside, (size_t) ipix, theta, phi);
  return;
}
void compat_pix2ang_ring(long nside, long ipix, double *theta, double *phi)
{
  hpic_pix2ang_nest((size_t) nside, (size_t) ipix, theta, phi);
  return;
}
void compat_nest2ring(long nside, long ipnest, long *ipring)
{
  size_t pring;
  hpic_nest2ring((size_t) nside, (size_t) ipnest, &pring);
  (*ipring) = (long)pring;
  return;
}
void compat_ring2nest(long nside, long ipring, long *ipnest)
{
  size_t pnest;
  hpic_ring2nest((size_t) nside, (size_t) ipring, &pnest);
  (*ipnest) = (long)pnest;
  return;
}
long compat_nside2npix(const long nside)
{
  size_t npix;
  long ret;
  npix = hpic_nside2npix((size_t) nside);
  ret = (long)npix;
  return ret;
}

/* compatibility FITS functions */

long compat_get_fits_size(char *filename, long *nside, char *ordering)
{
  size_t snside;
  int order;
  int coord;
  int type;
  size_t nmaps;
  long npix;

  if (hpic_fits_map_test(filename, &snside, &order, &coord, &type, &nmaps)) {
    (*nside) = (long)snside;

    if (order == HPIC_RING) {
      strcpy(ordering, "RING");
    } else {
      strcpy(ordering, "NESTED");
    }

    npix = (long)hpic_nside2npix(snside);
  } else {
    fprintf(stderr,
            "compat_get_fits_size:  file %s is not a healpix format file!\n",
            filename);
    npix = 0;
  }
  return npix;
}

int compat_read_healpix_map(char *infile, long *nside, char *coordsys,
                            char *ordering, float *map)
{
  size_t i;
  hpic_float *hpic_signal = NULL;
  char creator[HPIC_STRNL];
  hpic_keys *keys = NULL;

  keys = hpic_keys_alloc();
  hpic_signal = hpic_fits_read_one(infile, 0, creator, keys);

  (*nside) = (long)hpic_float_nside_get(hpic_signal);
  if (hpic_float_order_get(hpic_signal) == HPIC_RING) {
    strcpy(ordering, "RING");
  } else {
    strcpy(ordering, "NESTED");
  }

  switch (hpic_float_coord_get(hpic_signal)) {
  case HPIC_COORD_G:
    strcpy(coordsys, "G");
    break;
  case HPIC_COORD_C:
    strcpy(coordsys, "C");
    break;
  case HPIC_COORD_E:
    strcpy(coordsys, "E");
    break;
  default:
    strcpy(coordsys, "O");
    break;
  }

  for (i = 0; i < hpic_float_npix_get(hpic_signal); i++) {
    map[i] = hpic_float_get(hpic_signal, i);
  }

  hpic_keys_free(keys);
  hpic_float_free(hpic_signal);
  return 0;
}

int compat_write_healpix_map(float *hpic_signal, long nside, char *filename,
                             char nest, char *coordsys)
{
  size_t i;
  int order;
  int coord;
  hpic_float *map = NULL;
  hpic_keys *keys = NULL;

  if (nest) {
    order = HPIC_NEST;
  } else {
    order = HPIC_RING;
  }
  if (strncmp(coordsys, "C", HPIC_STRNL) == 0) {
    coord = HPIC_COORD_C;
  } else if (strncmp(coordsys, "G", HPIC_STRNL) == 0) {
    coord = HPIC_COORD_G;
  } else if (strncmp(coordsys, "E", HPIC_STRNL) == 0) {
    coord = HPIC_COORD_E;
  } else {
    coord = HPIC_COORD_O;
  }

  keys = hpic_keys_alloc();
  map = hpic_float_alloc((size_t) nside, order, coord, HPIC_AUTO);
  for (i = 0; i < hpic_float_npix_get(map); i++) {
    hpic_float_set(map, i, hpic_signal[i]);
  }

  hpic_cmb_write_full(filename, map,
                      "write_healpix_map Compatability Function",
                      "compat_write_healpix_map", keys);

  hpic_keys_free(keys);
  hpic_float_free(map);

  return 0;
}

void compat_read_fits_cut4(char *filename, int *pixel, float *hpic_signal,
                           int *n_obs, float *serror, char *channel,
                           char *coordsys, int *nside, int *obs_npix,
                           char *ordering, char *target, int *shot,
                           char *units)
{

  size_t i, j;
  size_t snside;
  int order;
  int coord;
  int type;
  size_t nmaps;
  char creator[HPIC_STRNL];
  char keycom[HPIC_STRNL];
  hpic_float *data;
  hpic_int *pix;
  hpic_int *hits;
  hpic_float *err;
  hpic_keys *keys;

  if (hpic_fits_map_test(filename, &snside, &order, &coord, &type, &nmaps)) {
    if ((type != HPIC_FITS_CUT) || (nmaps != 1)) {
      fprintf(stderr,
              "compat_read_fits_cut4:  file %s is not a single map, cut-sky format file!\n",
              filename);
      exit(1);
    }

    (*nside) = (int)snside;

    if (order == HPIC_RING) {
      strcpy(ordering, "RING");
    } else {
      strcpy(ordering, "NESTED");
    }
    switch (coord) {
    case HPIC_COORD_G:
      strcpy(coordsys, "G");
      break;
    case HPIC_COORD_C:
      strcpy(coordsys, "C");
      break;
    case HPIC_COORD_E:
      strcpy(coordsys, "E");
      break;
    default:
      strcpy(coordsys, "O");
      break;
    }
    keys = hpic_keys_alloc();
    pix = hpic_int_alloc(snside, order, coord, HPIC_AUTO);
    hits = hpic_int_alloc(snside, order, coord, HPIC_AUTO);
    data = hpic_float_alloc(snside, order, coord, HPIC_AUTO);
    err = hpic_float_alloc(snside, order, coord, HPIC_AUTO);

    hpic_cmb_read_cut(filename, pix, data, hits, err, creator, keys);

    j = 0;
    for (i = 0; i < snside; i++) {
      if (!hpic_is_inull(hpic_int_get(pix, i))) {
        pixel[j] = hpic_int_get(pix, i);
        hpic_signal[j] = hpic_float_get(data, i);
        n_obs[j] = hpic_int_get(hits, i);
        serror[j] = hpic_float_get(err, i);
        j++;
      }
    }

    (*obs_npix) = (int)j;

    strcpy(units, hpic_float_units_get(data));

    hpic_keys_sfind(keys, "CHANNEL", channel, keycom);
    hpic_keys_sfind(keys, "TARGET", target, keycom);
    hpic_keys_ifind(keys, "SHOT", shot, keycom);

    hpic_keys_free(keys);
    hpic_int_free(pix);
    hpic_int_free(hits);
    hpic_float_free(data);
    hpic_float_free(err);

  } else {
    fprintf(stderr,
            "compat_read_fits_cut4:  file %s is not a healpix format file!\n",
            filename);
    exit(1);
  }

  return;
}

int compat_write_fits_cut4(int *pixel, float *hpic_signal, int *n_obs,
                           float *serror, char *channel, char *coordsys,
                           int nside, int obs_npix, char *ordering,
                           char *target, int shot, char *units,
                           char *filename, char *creator, char *release)
{

  size_t i, j;
  size_t snside = (size_t) nside;
  int order;
  int coord;
  hpic_float *data;
  hpic_int *pix;
  hpic_int *hits;
  hpic_float *err;
  hpic_keys *keys;

  if (strncmp(ordering, "RING", HPIC_STRNL) == 0) {
    order = HPIC_RING;
  } else {
    order = HPIC_NEST;
  }
  if (strncmp(coordsys, "C", HPIC_STRNL) == 0) {
    coord = HPIC_COORD_C;
  } else if (strncmp(coordsys, "G", HPIC_STRNL) == 0) {
    coord = HPIC_COORD_G;
  } else if (strncmp(coordsys, "E", HPIC_STRNL) == 0) {
    coord = HPIC_COORD_E;
  } else {
    coord = HPIC_COORD_O;
  }

  keys = hpic_keys_alloc();
  pix = hpic_int_alloc(snside, order, coord, HPIC_AUTO);
  hits = hpic_int_alloc(snside, order, coord, HPIC_AUTO);
  data = hpic_float_alloc(snside, order, coord, HPIC_AUTO);
  err = hpic_float_alloc(snside, order, coord, HPIC_AUTO);

  hpic_float_units_set(data, units);
  hpic_float_units_set(err, units);
  hpic_float_name_set(data, "SIGNAL");
  hpic_float_name_set(err, "SERROR");
  hpic_int_name_set(pix, "PIXEL");
  hpic_int_name_set(hits, "N_OBS");

  for (i = 0; i < (size_t) obs_npix; i++) {
    hpic_int_set(pix, (size_t) pixel[i], pixel[i]);
    hpic_int_set(hits, (size_t) pixel[i], n_obs[i]);
    hpic_float_set(data, (size_t) pixel[i], hpic_signal[i]);
    hpic_float_set(err, (size_t) pixel[i], serror[i]);
  }

  hpic_keys_sadd(keys, "RELEASE", release, "Data Release");
  hpic_keys_sadd(keys, "CHANNEL", channel, "Channel of Observation");
  hpic_keys_iadd(keys, "OBS_NPIX", obs_npix, "Number of Observed Pixels");
  hpic_keys_iadd(keys, "SHOT", shot, "Shot of this Target");
  hpic_keys_sadd(keys, "OBJECT", "PARTIAL", "Sky Coverage");

  hpic_cmb_write_cut(filename, pix, data, hits, err,
                     "write_fits_cut4 Compatibility Function", creator, keys);

  hpic_keys_free(keys);
  hpic_int_free(pix);
  hpic_int_free(hits);
  hpic_float_free(data);
  hpic_float_free(err);

  return 0;
}

