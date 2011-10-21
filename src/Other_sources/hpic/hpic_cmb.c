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
 * This code deals with CMB specific operations                              *
 *****************************************************************************/

#include <hpic.h>

/* projection */

int hpic_cmb_proj_QU(hpic_proj * proj, hpic * Qmap, hpic * Umap,
                      size_t pnside, double maxmag, 
                      hpic_vec_index * headx,
                      hpic_vec_index * heady, hpic_vec_index * tailx,
                      hpic_vec_index * taily)
{

  size_t i;
  hpic *theta;
  hpic *phi;
  double q, u;
  double P, alpha;
  int err;

  if (!proj) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
  if (!Qmap) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "Q map pointer is NULL");
  }
  if (!Umap) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "U map pointer is NULL");
  }
  if (hpic_nside_get(Qmap) != hpic_nside_get(Umap)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "Q and U nsides must be equal");
  }
  if (hpic_order_get(Qmap) != hpic_order_get(Umap)) {
    HPIC_ERROR(HPIC_ERR_ORDER, "Q and U must have same order");
  }
  if (pnside > hpic_nside_get(Qmap)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "projected nside must be <= Q/U nside");
  }
  err = hpic_nsidecheck(pnside);
  if (err) {
    HPIC_ERROR(err, "projected nside is invalid");
  }
  if (!headx || !heady || !tailx || !taily) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "x and y head/tail vectors must be allocated (any size)");
  }

  theta =
    hpic_alloc(hpic_nside_get(Qmap), hpic_order_get(Qmap),
               hpic_coord_get(Qmap), hpic_mem_get(Qmap));
  phi =
    hpic_alloc(hpic_nside_get(Qmap), hpic_order_get(Qmap),
               hpic_coord_get(Qmap), hpic_mem_get(Qmap));

  for (i = 0; i < hpic_npix_get(Qmap); i++) {
    q = hpic_get(Qmap, i);
    u = hpic_get(Umap, i);
    if (!hpic_is_dnull(q) && !hpic_is_dnull(u)) {
      P = sqrt(q * q + u * u);
      alpha = 0.5 * atan(u / q);
      hpic_set(theta, i, P * cos(alpha));
      hpic_set(phi, i, P * sin(alpha));
    }
  }

  hpic_proj_vecfield(proj, theta, phi, pnside, maxmag, headx, heady, tailx,
                     taily);

  hpic_free(theta);
  hpic_free(phi);

  return 0;
}

/* cmb specific Map FITS reading / writing */

/* write a single, full-sky map */
int hpic_cmb_write_full(char *filename, hpic_float * data, char *comment,
                        char *creator, hpic_keys * keys)
{
  int ret = 0;
  char extname[HPIC_STRNL];
  hpic_fltarr *maps;

  hpic_keys_sadd(keys, "OBJECT", "FULLSKY", "Sky Coverage, FULLSKY or PARTIAL");
  strcpy(extname, "Full-Sky Map");
  maps = hpic_fltarr_alloc(1);
  hpic_fltarr_set(maps, 0, data);

  ret = hpic_fits_full_write(filename, creator, extname, comment, maps, keys);

  hpic_fltarr_free(maps);
  return ret;
}

/* write TQU full-sky maps */
int hpic_cmb_write_fullTQU(char *filename, hpic_float * tdata,
                           hpic_float * qdata, hpic_float * udata,
                           char *comment, char *creator, hpic_keys * keys)
{
  int ret = 0;
  char extname[HPIC_STRNL];
  hpic_fltarr *maps;

  strcpy(extname, "Full-Sky Maps");
  hpic_keys_sadd(keys, "OBJECT", "FULLSKY", "Sky Coverage, FULLSKY or PARTIAL");
  maps = hpic_fltarr_alloc(3);
  hpic_fltarr_set(maps, 0, tdata);
  hpic_fltarr_set(maps, 1, qdata);
  hpic_fltarr_set(maps, 2, udata);

  ret = hpic_fits_full_write(filename, creator, extname, comment, maps, keys);

  hpic_fltarr_free(maps);
  return ret;
}

/* write a single, cut-sky map */
int hpic_cmb_write_cut(char *filename, hpic_int * pix, hpic_float * data,
                       hpic_int * hits, hpic_float * errs, char *comment,
                       char *creator, hpic_keys * keys)
{
  int ret = 0;
  char extname[HPIC_STRNL];
  hpic_fltarr *maps;

  maps = hpic_fltarr_alloc(1);
  hpic_fltarr_set(maps, 0, data);

  strcpy(extname, "Cut-Sky Map");
  hpic_keys_sadd(keys, "OBJECT", "PARTIAL", "Sky Coverage, FULLSKY or PARTIAL");
  ret =
    hpic_fits_cut_write(filename, creator, extname, comment, pix, hits, errs,
                        maps, keys);

  hpic_fltarr_free(maps);
  return ret;
}

/* write TQU cut-sky maps */
int hpic_cmb_write_cutTQU(char *filename, hpic_int * pix, hpic_float * tdata,
                          hpic_float * qdata, hpic_float * udata,
                          hpic_int * hits, hpic_float * errs, char *comment,
                          char *creator, hpic_keys * keys)
{
  int ret = 0;
  char extname[HPIC_STRNL];
  hpic_fltarr *maps;

  maps = hpic_fltarr_alloc(3);
  hpic_fltarr_set(maps, 0, tdata);
  hpic_fltarr_set(maps, 1, qdata);
  hpic_fltarr_set(maps, 2, udata);

  strcpy(extname, "Cut-Sky Maps");
  hpic_keys_sadd(keys, "OBJECT", "PARTIAL", "Sky Coverage, FULLSKY or PARTIAL");
  ret =
    hpic_fits_cut_write(filename, creator, extname, comment, pix, hits, errs,
                        maps, keys);

  hpic_fltarr_free(maps);
  return ret;
}

/* read a TQU full-sky file */
int hpic_cmb_read_fullTQU(char *filename, hpic_float * tdata,
                          hpic_float * qdata, hpic_float * udata,
                          char *creator, hpic_keys * keys)
{
  int ret = 0;
  size_t nside;
  int order;
  int coord;
  int type;
  size_t nmaps;
  char extname[HPIC_STRNL];
  hpic_fltarr *maps;

  if (!hpic_fits_map_test(filename, &nside, &order, &coord, &type, &nmaps)) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a healpix format file!");
  }
  if (type != HPIC_FITS_FULL) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a full-sky format file");
  }
  if (nmaps != 3) {
    HPIC_ERROR(HPIC_ERR_FITS, "number of maps is != 3, try using hpic_fits_read_one");
  }

  maps = hpic_fltarr_alloc(3);
  hpic_fltarr_set(maps, 0, tdata);
  hpic_fltarr_set(maps, 1, qdata);
  hpic_fltarr_set(maps, 2, udata);

  ret = hpic_fits_full_read(filename, creator, extname, maps, keys);

  hpic_fltarr_free(maps);
  return ret;
}

/* read a cut-sky file */
int hpic_cmb_read_cut(char *filename, hpic_int * pix, hpic_float * data,
                      hpic_int * hits, hpic_float * errs, char *creator,
                      hpic_keys * keys)
{
  int ret = 0;
  size_t nside;
  int order;
  int coord;
  int type;
  size_t nmaps;
  char extname[HPIC_STRNL];
  hpic_fltarr *maps;

  if (!hpic_fits_map_test(filename, &nside, &order, &coord, &type, &nmaps)) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a healpix format file!");
  }
  if (type != HPIC_FITS_CUT) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a cut-sky format file");
  }
  if (nmaps != 1) {
    HPIC_ERROR(HPIC_ERR_FITS, "number of maps is != 1, try using hpic_cmb_read_cutTQU");
  }

  maps = hpic_fltarr_alloc(1);
  hpic_fltarr_set(maps, 0, data);

  ret =
    hpic_fits_cut_read(filename, creator, extname, pix, hits, errs, maps,
                       keys);

  hpic_fltarr_free(maps);
  return ret;
}

/* read a cut-sky TQU file */
int hpic_cmb_read_cutTQU(char *filename, hpic_int * pix, hpic_float * tdata,
                         hpic_float * qdata, hpic_float * udata,
                         hpic_int * hits, hpic_float * errs, char *creator,
                         hpic_keys * keys)
{
  int ret = 0;
  size_t nside;
  int order;
  int coord;
  int type;
  size_t nmaps;
  char extname[HPIC_STRNL];
  hpic_fltarr *maps;

  if (!hpic_fits_map_test(filename, &nside, &order, &coord, &type, &nmaps)) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a healpix format file!");
  }
  if (type != HPIC_FITS_CUT) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a cut-sky format file");
  }
  if (nmaps != 3) {
    HPIC_ERROR(HPIC_ERR_FITS, "number of maps is != 3, try using hpic_fits_read_one");
  }

  maps = hpic_fltarr_alloc(3);
  hpic_fltarr_set(maps, 0, tdata);
  hpic_fltarr_set(maps, 1, qdata);
  hpic_fltarr_set(maps, 2, udata);

  ret =
    hpic_fits_cut_read(filename, creator, extname, pix, hits, errs, maps,
                       keys);

  hpic_fltarr_free(maps);
  return ret;
}

/* cmb specific Vector FITS reading / writing */
