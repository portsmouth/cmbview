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
 * This code deals with map projection                                       *
 *****************************************************************************/

#include <hpic.h>

/* alloc / free */

hpic_proj *hpic_proj_alloc(size_t nx, size_t ny)
{
  hpic_proj *proj;
  proj = (hpic_proj *) calloc(1, sizeof(hpic_proj));
  if (!proj) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate projection", NULL);
  }
  proj->nx = nx;
  proj->ny = ny;
  proj->type = HPIC_PROJ_CAR;
  proj->mintheta = 0.0;
  proj->maxtheta = 0.0;
  proj->minphi = 0.0;
  proj->maxphi = 0.0;
  proj->data = (double *)calloc(nx * ny, sizeof(double));
  if (!proj->data) {
    free(proj);
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate projection->data", NULL);
  }
  hpic_proj_setall(proj, HPIC_NULL);
  return proj;
}

int hpic_proj_free(hpic_proj * proj)
{
  if (proj) {
    free(proj->data);
    free(proj);
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "projection not allocated, not freeing");
  }
  return 0;
}

/* parameter access */

int hpic_proj_type_get(hpic_proj * proj)
{
  if (proj) {
    return proj->type;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
}

int hpic_proj_type_set(hpic_proj * proj, int type)
{
  if (proj) {
    proj->type = type;
    hpic_proj_setall(proj, HPIC_NULL);
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
  return 0;
}

int hpic_proj_range_get(hpic_proj * proj, double *mintheta, double *maxtheta,
                         double *minphi, double *maxphi)
{
  if (proj) {
    (*mintheta) = proj->mintheta;
    (*maxtheta) = proj->maxtheta;
    (*minphi) = proj->minphi;
    (*maxphi) = proj->maxphi;
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
}

int hpic_proj_range_set(hpic_proj * proj, double mintheta, double maxtheta,
                         double minphi, double maxphi)
{
  if (proj) {
    if ((mintheta < 0.0) || (maxtheta > HPIC_PI) || (minphi < 0.0) ||
        (maxphi > 2 * HPIC_PI)) {
      HPIC_ERROR(HPIC_ERR_RANGE, "parameters not within acceptable limits");
    } else {
      proj->mintheta = mintheta;
      proj->maxtheta = maxtheta;
      proj->minphi = minphi;
      proj->maxphi = maxphi;
      hpic_proj_setall(proj, HPIC_NULL);
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
}

/* data access */

int hpic_proj_set(hpic_proj * proj, size_t xelem, size_t yelem, double val)
{
  if (proj) {
    if ((xelem >= 0) && (xelem < (proj->nx)) && (yelem >= 0) && (yelem < (proj->ny))) {
      proj->data[(size_t) (xelem * (proj->ny) + yelem)] = val;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "xelem and/or yelem out of range");
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
}

double hpic_proj_get(hpic_proj * proj, size_t xelem, size_t yelem)
{
  if (proj) {
    if ((xelem >= 0) && (xelem < (proj->nx)) && (yelem >= 0) && (yelem < (proj->ny))) {
      return proj->data[(size_t) (xelem * (proj->ny) + yelem)];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "xelem and/or yelem out of range", HPIC_NULL);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "projection pointer is NULL", HPIC_NULL);
  }
}

int hpic_proj_setall(hpic_proj * proj, double val)
{
  size_t i;
  if (proj) {
    for (i = 0; i < ((proj->nx) * (proj->ny)); i++) {
      proj->data[i] = val;
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
  return 0;
}

/* point vector projection */

int hpic_proj_points(hpic_proj * proj, hpic_vec * theta, hpic_vec * phi,
                      hpic_vec_index * x, hpic_vec_index * y)
{
  size_t i;
  size_t n;
  double tempx, tempy;

  n = hpic_vec_n_get(theta);
  if ((hpic_vec_n_get(phi) != n)) {
    HPIC_ERROR(HPIC_ERR_RANGE, "theta and phi vectors must have the same length");
  }
  if (!x || !y) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "x and y vector must be allocated (to any size)");
  }
  hpic_vec_index_resize(x, 0);
  hpic_vec_index_resize(y, 0);

  if (proj->type == HPIC_PROJ_CAR) {
    for (i = 0; i < n; i++) {
      hpic_proj_car(proj->mintheta, proj->maxtheta, proj->minphi, proj->maxphi,
               proj->nx, proj->ny, hpic_vec_get(theta, i), hpic_vec_get(phi,
                                                                        i),
               &tempx, &tempy);
      if ((!hpic_is_dnull(tempx)) && (!hpic_is_dnull(tempy))) {
        hpic_vec_index_append(x, (size_t) floor(tempx));
        hpic_vec_index_append(y, (size_t) floor(tempy));
      }
    }
  } else if (proj->type == HPIC_PROJ_SIN) {
    for (i = 0; i < n; i++) {
      hpic_proj_sin(proj->mintheta, proj->maxtheta, proj->minphi, proj->maxphi,
               proj->nx, proj->ny, hpic_vec_get(theta, i), hpic_vec_get(phi,
                                                                        i),
               &tempx, &tempy);
      if ((!hpic_is_dnull(tempx)) && (!hpic_is_dnull(tempy))) {
        hpic_vec_index_append(x, (size_t) floor(tempx));
        hpic_vec_index_append(y, (size_t) floor(tempy));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_PROJ, "unrecognized projection type");
  }

  return 0;
}

/* pixel vector projection */

int hpic_proj_pixels(hpic_proj * proj, size_t nside, int order,
                      hpic_vec_index * pixels, hpic_vec_index * x,
                      hpic_vec_index * y)
{
  size_t i, j, k;
  double theta;
  double phi;
  hpic_int *map;
  size_t pix;
  int err;

  if (!proj) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
  if (!pixels) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "pixels pointer is NULL");
  }
  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR(err, "nside value invalid");
  }
  if (!x || !y) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "x and y vector must be allocated (to any size)");
  }
  hpic_vec_index_resize(x, 0);
  hpic_vec_index_resize(y, 0);

  map = hpic_int_alloc(nside, order, HPIC_COORD_C, HPIC_AUTO);
  if (!map) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate pixmap");
  }

  for (i = 0; i < hpic_vec_index_n_get(pixels); i++) {
    hpic_int_set(map, hpic_vec_index_get(pixels, i), 1);
  }

  if (proj->type == HPIC_PROJ_CAR) {
    for (i = 0; i < (proj->nx); i++) {
      for (j = 0; j < (proj->ny); j++) {
        hpic_proj_rev_car(proj->mintheta, proj->maxtheta, proj->minphi,
                     proj->maxphi, proj->nx, proj->ny, (double)i + 0.5,
                     (double)j + 0.5, &theta, &phi);
        if (order == HPIC_RING) {
          hpic_ang2pix_ring(nside, theta, phi, &pix);
        } else {
          hpic_ang2pix_nest(nside, theta, phi, &pix);
        }
        if (!hpic_is_inull(hpic_int_get(map, pix))) {        /*add pixel to list */
          hpic_vec_index_append(x, i);
          hpic_vec_index_append(y, j);
        }
      }
    }
  } else if (proj->type == HPIC_PROJ_SIN) {
    for (i = 0; i < (proj->nx); i++) {
      for (j = 0; j < (proj->ny); j++) {
        hpic_proj_rev_sin(proj->mintheta, proj->maxtheta, proj->minphi,
                     proj->maxphi, proj->nx, proj->ny, (double)i + 0.5,
                     (double)j + 0.5, &theta, &phi);
        if (order == HPIC_RING) {
          hpic_ang2pix_ring(nside, theta, phi, &pix);
        } else {
          hpic_ang2pix_nest(nside, theta, phi, &pix);
        }
        if (!hpic_is_inull(hpic_int_get(map, pix))) {        /*add pixel to list */
          hpic_vec_index_append(x, i);
          hpic_vec_index_append(y, j);
        }
      }
    }
  } else {
    hpic_int_free(map);
    HPIC_ERROR(HPIC_ERR_PROJ, "unrecognized projection type");
  }
  hpic_int_free(map);

  return 0;
}

/* map projection */

int hpic_proj_map(hpic_proj * proj, hpic * map)
{
  size_t i, j;
  double theta;
  double phi;
  int order;
  size_t nside;
  size_t pix;

  if (!proj) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
  if (!map) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "map pointer is NULL");
  }

  /*find map parameters */
  order = hpic_order_get(map);
  nside = hpic_nside_get(map);

  /*find position of each projected pixel, then find the value in the map */
  if (proj->type == HPIC_PROJ_CAR) {
    for (i = 0; i < (proj->nx); i++) {
      for (j = 0; j < (proj->ny); j++) {
        hpic_proj_rev_car(proj->mintheta, proj->maxtheta, proj->minphi,
                     proj->maxphi, proj->nx, proj->ny, (double)i + 0.5,
                     (double)j + 0.5, &theta, &phi);
        if (order == HPIC_RING) {
          hpic_ang2pix_ring(nside, theta, phi, &pix);
        } else {
          hpic_ang2pix_nest(nside, theta, phi, &pix);
        }
        hpic_proj_set(proj, i, j, hpic_get(map, pix));
      }
    }
  } else if (proj->type == HPIC_PROJ_SIN) {
    for (i = 0; i < (proj->nx); i++) {
      for (j = 0; j < (proj->ny); j++) {
        hpic_proj_rev_sin(proj->mintheta, proj->maxtheta, proj->minphi,
                     proj->maxphi, proj->nx, proj->ny, (double)i + 0.5,
                     (double)j + 0.5, &theta, &phi);
        if (order == HPIC_RING) {
          hpic_ang2pix_ring(nside, theta, phi, &pix);
        } else {
          hpic_ang2pix_nest(nside, theta, phi, &pix);
        }
        hpic_proj_set(proj, i, j, hpic_get(map, pix));
      }
    }
  } else {
    HPIC_ERROR(HPIC_ERR_PROJ, "unrecognized projection type");
  }
  return 0;
}

/* vector field projection */

int hpic_proj_vecfield(hpic_proj * proj, hpic * thetacomp, hpic * phicomp,
                        size_t pnside, double maxmag, hpic_vec_index * headx,
                        hpic_vec_index * heady, hpic_vec_index * tailx,
                        hpic_vec_index * taily)
{
  size_t i, j, k;
  double theta;
  double headtheta;
  double tailtheta;
  double thetapart;
  double phi;
  double headphi;
  double tailphi;
  double phipart;
  hpic *thetaproj;
  hpic *phiproj;
  double maxang;
  double thx, thy, ttx, tty;
  hpic_vec_index *nearest;
  int order;
  int err;

  if (!proj) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
  if (!thetacomp) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "thetacomp pointer is NULL");
  }
  if (!phicomp) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "phicomp pointer is NULL");
  }
  if (hpic_nside_get(phicomp) != hpic_nside_get(thetacomp)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "component nsides must be equal");
  }
  if (hpic_order_get(phicomp) != hpic_order_get(thetacomp)) {
    HPIC_ERROR(HPIC_ERR_ORDER, "components must have same order");
  }
  if (pnside > hpic_nside_get(thetacomp)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "projected nside must be <= component nside");
  }
  err = hpic_nsidecheck(pnside);
  if (err) {
    HPIC_ERROR(err, "projected nside is invalid");
  }
  if (!headx || !heady || !tailx || !taily) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "x and y head/tail vectors must be allocated (any size)");
  }
  hpic_vec_index_resize(headx, 0);
  hpic_vec_index_resize(tailx, 0);
  hpic_vec_index_resize(heady, 0);
  hpic_vec_index_resize(taily, 0);
  nearest = hpic_vec_index_alloc(0);

  thetaproj = hpic_conv_xgrade(thetacomp, pnside);
  if (!thetaproj) {
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate theta projection");
  }
  phiproj = hpic_conv_xgrade(phicomp, pnside);
  if (!phiproj) {
    hpic_free(thetaproj);
    HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocate phi projection");
  }
  order = hpic_order_get(thetaproj);

  if (proj->type == HPIC_PROJ_CAR) {
    for (i = 0; i < hpic_npix_get(thetaproj); i++) {
      thetapart = hpic_get(thetaproj, i);
      phipart = hpic_get(phiproj, i);
      if (!hpic_is_dnull(thetapart) && !hpic_is_dnull(phipart)) {
        if (order == HPIC_RING) {
          hpic_pix2ang_ring(pnside, i, &theta, &phi);
        } else {
          hpic_pix2ang_nest(pnside, i, &theta, &phi);
        }
        /*find max angle */
        hpic_neighbors(pnside, order, i, nearest);
        maxang = hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 0));
        maxang += hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 1));
        maxang += hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 2));
        maxang += hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 3));
        maxang /= 4.0;
        thetapart *= maxang / maxmag;
        phipart *= maxang / maxmag;
        /*find coordinates of head and tail */
        headtheta = theta + 0.5 * thetapart;
        tailtheta = theta - 0.5 * thetapart;
        headphi = phi + 0.5 * phipart;
        tailphi = phi - 0.5 * phipart;
        while (headtheta > HPIC_PI) {
          headtheta -= HPIC_PI;
        }
        while (headtheta < 0.0) {
          headtheta += HPIC_PI;
        }
        while (tailtheta > HPIC_PI) {
          tailtheta -= HPIC_PI;
        }
        while (tailtheta < 0.0) {
          tailtheta += HPIC_PI;
        }
        while (headphi > 2.0 * HPIC_PI) {
          headphi -= 2.0 * HPIC_PI;
        }
        while (headphi < 0.0) {
          headphi += 2.0 * HPIC_PI;
        }
        while (tailphi > 2.0 * HPIC_PI) {
          tailphi -= 2.0 * HPIC_PI;
        }
        while (tailphi < 0.0) {
          tailphi += 2.0 * HPIC_PI;
        }
        hpic_proj_car(proj->mintheta, proj->maxtheta, proj->minphi, proj->maxphi,
                 proj->nx, proj->ny, headtheta, headphi, &thx, &thy);
        hpic_proj_car(proj->mintheta, proj->maxtheta, proj->minphi, proj->maxphi,
                 proj->nx, proj->ny, tailtheta, tailphi, &ttx, &tty);
        if (!hpic_is_dnull(thx) && !hpic_is_dnull(thy) && !hpic_is_dnull(ttx) &&
            !hpic_is_dnull(tty)) {
          hpic_vec_index_append(headx, (size_t) thx);
          hpic_vec_index_append(heady, (size_t) thy);
          hpic_vec_index_append(tailx, (size_t) ttx);
          hpic_vec_index_append(taily, (size_t) tty);
        }
      }
    }
  } else if (proj->type == HPIC_PROJ_SIN) {
    for (i = 0; i < hpic_npix_get(thetaproj); i++) {
      thetapart = hpic_get(thetaproj, i);
      phipart = hpic_get(phiproj, i);
      if (!hpic_is_dnull(thetapart) && !hpic_is_dnull(phipart)) {
        if (order == HPIC_RING) {
          hpic_pix2ang_ring(pnside, i, &theta, &phi);
        } else {
          hpic_pix2ang_nest(pnside, i, &theta, &phi);
        }
        /*find max angle */
        hpic_neighbors(pnside, order, i, nearest);
        maxang = hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 0));
        maxang += hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 1));
        maxang += hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 2));
        maxang += hpic_loc_dist(pnside, order, i, hpic_vec_index_get(nearest, 3));
        maxang /= 4.0;
        /*find components of projected vector */
        thetapart *= maxang / maxmag;
        phipart *= maxang / maxmag;
        /*find coordinates of head and tail */
        headtheta = theta + 0.5 * thetapart;
        tailtheta = theta - 0.5 * thetapart;
        headphi = phi + 0.5 * phipart;
        tailphi = phi - 0.5 * phipart;
        while (headtheta > HPIC_PI) {
          headtheta -= HPIC_PI;
        }
        while (headtheta < 0.0) {
          headtheta += HPIC_PI;
        }
        while (tailtheta > HPIC_PI) {
          tailtheta -= HPIC_PI;
        }
        while (tailtheta < 0.0) {
          tailtheta += HPIC_PI;
        }
        while (headphi > 2.0 * HPIC_PI) {
          headphi -= 2.0 * HPIC_PI;
        }
        while (headphi < 0.0) {
          headphi += 2.0 * HPIC_PI;
        }
        while (tailphi > 2.0 * HPIC_PI) {
          tailphi -= 2.0 * HPIC_PI;
        }
        while (tailphi < 0.0) {
          tailphi += 2.0 * HPIC_PI;
        }
        hpic_proj_sin(proj->mintheta, proj->maxtheta, proj->minphi, proj->maxphi,
                 proj->nx, proj->ny, headtheta, headphi, &thx, &thy);
        hpic_proj_sin(proj->mintheta, proj->maxtheta, proj->minphi, proj->maxphi,
                 proj->nx, proj->ny, tailtheta, tailphi, &ttx, &tty);
        if (!hpic_is_dnull(thx) && !hpic_is_dnull(thy) && !hpic_is_dnull(ttx) &&
            !hpic_is_dnull(tty)) {
          hpic_vec_index_append(headx, (size_t) thx);
          hpic_vec_index_append(heady, (size_t) thy);
          hpic_vec_index_append(tailx, (size_t) ttx);
          hpic_vec_index_append(taily, (size_t) tty);
        }
      }
    }
  } else {
    hpic_free(thetaproj);
    hpic_free(phiproj);
    hpic_vec_index_free(nearest);
    HPIC_ERROR(HPIC_ERR_PROJ, "unrecognized projection type");
  }

  hpic_vec_index_free(nearest);
  hpic_free(thetaproj);
  hpic_free(phiproj);
  return 0;
}

/* info printing */

int hpic_proj_info_fprintf(FILE * fp, hpic_proj * proj)
{
  if (proj) {
    fprintf(fp, "  ----------------------------------------\n");
    fprintf(fp, "  | Projection type = %d\n", proj->type);
    fprintf(fp, "  |   size = %lu x %lu\n", proj->nx, proj->ny);
    fprintf(fp, "  |   theta = %g ... %g\n", proj->mintheta, proj->maxtheta);
    fprintf(fp, "  |   phi = %g ... %g\n", proj->minphi, proj->maxphi);
    fprintf(fp, "  |   data = %g ... %g\n", proj->data[0],
            proj->data[(proj->nx) * (proj->ny) - 1]);
    fprintf(fp, "  ----------------------------------------\n");
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "projection pointer is NULL");
  }
  return 0;
}

int hpic_proj_info_printf(hpic_proj * proj)
{
  return hpic_proj_info_fprintf(stdout, proj);
}

