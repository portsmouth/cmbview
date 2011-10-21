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
 * This code deals with low level pixel projection                           *
 *****************************************************************************/

#include <hpic.h>


int proj_stereo(double mintheta, double maxtheta, double minphi, double maxphi,
				double xmax, double ymax, double theta, double phi, double *x,
				double *y);


/* single pixel forward projections */

int hpic_proj_car(double mintheta, double maxtheta, double minphi, double maxphi,
              double xmax, double ymax, double theta, double phi, double *x,
              double *y)
{

  if ((mintheta < 0.0) || (mintheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum theta must be in the range [0..PI]");
  }
  if ((maxtheta < 0.0) || (maxtheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be in the range [0..PI]");
  }
  if ((minphi < 0.0) || (minphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum phi must be in the range [0..2*PI]");
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi must be in the range [0..2*PI]");
  }
  if (maxtheta <= mintheta) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be greater than minimum theta");
  }
  if (maxphi == minphi) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi cannot equal minimum phi");
  }
  if ((theta < 0.0) || (theta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"theta must be in the range [0..PI]");
  }
  if ((phi < 0.0) || (phi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"phi must be in the range [0..2*PI]");
  }

  if ((theta > maxtheta) || (theta < mintheta)) {
    (*x) = HPIC_NULL;
    (*y) = HPIC_NULL;
    return 0;
  }
  if (maxphi > minphi) {
    if ((phi > maxphi) || (phi < minphi)) {
      (*x) = HPIC_NULL;
      (*y) = HPIC_NULL;
      return 0;
    }
  } else {
    if ((phi > maxphi) && (phi < minphi)) {
      (*x) = HPIC_NULL;
      (*y) = HPIC_NULL;
      return 0;
    }
  }

  (*y) = ((maxtheta - theta) / (maxtheta - mintheta)) * ymax;

  if (maxphi > minphi) {
    (*x) = ((phi - minphi) / (maxphi - minphi)) * xmax;
  } else {
    if (phi <= maxphi) {
      (*x) =
        ((phi + (2.0 * HPIC_PI - minphi)) / (maxphi +
                                             (2.0 * HPIC_PI -
                                              minphi))) * xmax;
    } else {
      (*x) = ((phi - minphi) / (maxphi + (2.0 * HPIC_PI - minphi))) * xmax;
    }
  }
  return 0;
}

int hpic_proj_sin(double mintheta, double maxtheta, double minphi, double maxphi,
              double xmax, double ymax, double theta, double phi, double *x,
              double *y)
{

  double centphi;
  double centtheta;
  double halfx;
  double tempx;

  if ((mintheta < 0.0) || (mintheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum theta must be in the range [0..PI]");
  }
  if ((maxtheta < 0.0) || (maxtheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be in the range [0..PI]");
  }
  if ((minphi < 0.0) || (minphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum phi must be in the range [0..2*PI]");
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi must be in the range [0..2*PI]");
  }
  if (maxtheta <= mintheta) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be greater than minimum theta");
  }
  if (maxphi == minphi) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi cannot equal minimum phi");
  }
  if ((theta < 0.0) || (theta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"theta must be in the range [0..PI]");
  }
  if ((phi < 0.0) || (phi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"phi must be in the range [0..2*PI]");
  }

  (*y) = ((maxtheta - theta) / (maxtheta - mintheta)) * ymax;
  if (((*y) > ymax) || ((*y) < 0.0)) {
    (*x) = HPIC_NULL;
    (*y) = HPIC_NULL;
    return 0;
  }

  /* find half width of viewing area */

  centtheta = (maxtheta + mintheta) / 2.0;

  if (maxphi > minphi) {
    centphi = (maxphi + minphi) / 2.0;
    halfx = ((maxphi - minphi) / 2.0) * cos(-(centtheta - HPIC_PI / 2.0));
  } else {
    centphi = minphi + (minphi + (2.0 * HPIC_PI - maxphi)) / 2.0;
    if (minphi >= 2.0 * HPIC_PI) {
      centphi -= 2.0 * HPIC_PI;
    }
    halfx =
      ((minphi + (2.0 * HPIC_PI - maxphi)) / 2.0) * cos(-(centtheta -
                                                          HPIC_PI / 2.0));
  }

  /* find x offset from center */

  if (maxphi > minphi) {
    tempx = phi - centphi;
  } else {
    if (centphi > maxphi) {
      if (phi <= centphi) {
        tempx = phi - centphi;
      } else {
        if (phi < maxphi) {
          tempx = phi + (2.0 * HPIC_PI - centphi);
        } else {
          tempx = phi - centphi;
        }
      }
    } else {
      if (phi < centphi) {
        if (phi > minphi) {
          tempx = -(centphi + 2.0 * HPIC_PI - phi);
        } else {
          tempx = phi - centphi;
        }
      } else {
        tempx = phi - centphi;
      }
    }
  }
  tempx *= cos(-(theta - HPIC_PI / 2.0));

  if ((tempx > halfx) || (tempx < -halfx)) {
    (*x) = HPIC_NULL;
    (*y) = HPIC_NULL;
    return 0;
  }

  (*x) = (xmax / 2.0) * ((tempx / halfx) + 1.0);

  return 0;
}

int proj_stereo(double mintheta, double maxtheta, double minphi, double maxphi,
             double xmax, double ymax, double theta, double phi, double *x,
             double *y)
{

  if ((mintheta < 0.0) || (mintheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum theta must be in the range [0..PI]");
  }
  if ((maxtheta < 0.0) || (maxtheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be in the range [0..PI]");
  }
  if ((minphi < 0.0) || (minphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum phi must be in the range [0..2*PI]");
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi must be in the range [0..2*PI]");
  }
  if (maxtheta <= mintheta) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be greater than minimum theta");
  }
  if (maxphi == minphi) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi cannot equal minimum phi");
  }
  if ((theta < 0.0) || (theta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"theta must be in the range [0..PI]");
  }
  if ((phi < 0.0) || (phi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"phi must be in the range [0..2*PI]");
  }

  if ((theta > maxtheta) || (theta < mintheta)) {
    (*x) = HPIC_NULL;
    (*y) = HPIC_NULL;
    return 0;
  }
  if (maxphi > minphi) {
    if ((phi > maxphi) || (phi < minphi)) {
      (*x) = HPIC_NULL;
      (*y) = HPIC_NULL;
      return 0;
    }
  } else {
    if ((phi > maxphi) && (phi < minphi)) {
      (*x) = HPIC_NULL;
      (*y) = HPIC_NULL;
      return 0;
    }
  }

  (*y) = ((maxtheta - theta) / (maxtheta - mintheta)) * ymax;

  if (maxphi > minphi) {
    (*x) = ((phi - minphi) / (maxphi - minphi)) * xmax;
  } else {
    if (phi <= maxphi) {
      (*x) =
          ((phi + (2.0 * HPIC_PI - minphi)) / (maxphi +
          (2.0 * HPIC_PI -
          minphi))) * xmax;
    } else {
      (*x) = ((phi - minphi) / (maxphi + (2.0 * HPIC_PI - minphi))) * xmax;
    }
  }
  return 0;
}

/* single pixel reverse projections */

int hpic_proj_rev_car(double mintheta, double maxtheta, double minphi,
                  double maxphi, double xmax, double ymax, double x, double y,
                  double *theta, double *phi)
{

  if ((mintheta < 0.0) || (mintheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum theta must be in the range [0..PI]");
  }
  if ((maxtheta < 0.0) || (maxtheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be in the range [0..PI]");
  }
  if ((minphi < 0.0) || (minphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum phi must be in the range [0..2*PI]");
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi must be in the range [0..2*PI]");
  }
  if (maxtheta <= mintheta) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be greater than minimum theta");
  }
  if (maxphi == minphi) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi cannot equal minimum phi");
  }
  
  if ((y > ymax) || (y < 0.0) || (x > xmax) || (x < 0.0)) {
    (*theta) = HPIC_NULL;
    (*phi) = HPIC_NULL;
    return 0;
  }

  (*theta) = maxtheta - (y / ymax) * (maxtheta - mintheta);

  if (maxphi > minphi) {
    (*phi) = minphi + (x / xmax) * (maxphi - minphi);
  } else {
    (*phi) = minphi + (x / xmax) * (maxphi + (2.0 * HPIC_PI - minphi));
    if ((*phi) >= 2.0 * HPIC_PI) {
      (*phi) -= 2.0 * HPIC_PI;
    }
  }
  return 0;
}

int hpic_proj_rev_sin(double mintheta, double maxtheta, double minphi,
                  double maxphi, double xmax, double ymax, double x, double y,
                  double *theta, double *phi)
{

  double centphi;
  double centtheta;
  double halfx;
  double ph, th;

  if ((mintheta < 0.0) || (mintheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum theta must be in the range [0..PI]");
  }
  if ((maxtheta < 0.0) || (maxtheta > HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be in the range [0..PI]");
  }
  if ((minphi < 0.0) || (minphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"minimum phi must be in the range [0..2*PI]");
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HPIC_PI)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi must be in the range [0..2*PI]");
  }
  if (maxtheta <= mintheta) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum theta must be greater than minimum theta");
  }
  if (maxphi == minphi) {
    HPIC_ERROR(HPIC_ERR_RANGE,"maximum phi cannot equal minimum phi");
  }
  
  if ((y > ymax) || (y < 0.0) || (x > xmax) || (x < 0.0)) {
    (*theta) = HPIC_NULL;
    (*phi) = HPIC_NULL;
    return 0;
  }

  th = maxtheta - (y / ymax) * (maxtheta - mintheta);

  if ((th == 0.0) || (th == HPIC_PI)) { /*at a pole */
    if (x != (xmax / 2.0)) {
      (*theta) = HPIC_NULL;
      (*phi) = HPIC_NULL;
      return 0;
    } else {
      (*theta) = th;
      if (maxphi > minphi) {
        (*phi) = (maxphi + minphi) / 2.0;
      } else {
        ph = minphi + (maxphi + (2.0 * HPIC_PI - minphi)) / 2.0;
        if (ph >= 2.0 * HPIC_PI) {
          ph -= 2.0 * HPIC_PI;
        }
        (*phi) = ph;
      }
      return 0;
    }
  }

  /* find half width of viewing area */

  centtheta = (maxtheta + mintheta) / 2.0;

  if (maxphi > minphi) {
    centphi = (maxphi + minphi) / 2.0;
    halfx = ((maxphi - minphi) / 2.0) * cos(-(centtheta - HPIC_PI / 2.0));
  } else {
    centphi = minphi + (maxphi + (2.0 * HPIC_PI - minphi)) / 2.0;
    if (centphi >= 2.0 * HPIC_PI) {
      centphi -= 2.0 * HPIC_PI;
    }
    halfx =
      ((maxphi + (2.0 * HPIC_PI - minphi)) / 2.0) * cos(-(centtheta -
                                                          HPIC_PI / 2.0));
  }

  /* find phi offset from center */

  ph = ((2.0 * x / xmax - 1.0) * halfx) / (cos(-(th - HPIC_PI / 2.0)));

  if ((ph > HPIC_PI) || (ph < -HPIC_PI)) {
    (*theta) = HPIC_NULL;
    (*phi) = HPIC_NULL;
    return 0;
  }

  ph = ph + centphi;
  while (ph >= 2.0 * HPIC_PI) {
    ph -= 2.0 * HPIC_PI;
  }
  while (ph < 0.0) {
    ph += 2.0 * HPIC_PI;
  }

  (*theta) = th;
  (*phi) = ph;

  return 0;
}
