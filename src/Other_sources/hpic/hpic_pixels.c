/*****************************************************************************
 * These are pixel tools that originated in the code for Healpix_cxx.        *
 * See http://sourceforge.net/projects/healpix/ for the original code        *
 * (version 2.00 was used for the code below)                                *
 *                                                                           *
 * Healpix 2.00 is copyright 2005 by E. Hivon, M. Reinecke, W. O'Mullane,    *
 * H.K. Eriksen, K.M. Gorski, A.J. Banday                                    *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   * 
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Modified by Theodore Kisner beginning 2005-09-19                          *
 *****************************************************************************/

#include <hpic.h>
#include <hpic_config.h>
#ifdef HAVE_LIBPTHREAD
#  include <pthread.h>
#endif

#define HPIC_CHK if(!hpic_doneinit)hpic_init()

void hpic_init();

void hpic_init() {
  size_t m;
# ifdef HAVE_LIBPTHREAD  
  static pthread_mutex_t tablock = PTHREAD_MUTEX_INITIALIZER;        
  pthread_mutex_lock(&tablock);
# endif  
  for (m = 0; m < 0x100; m++) {
    hpic_ctab[m] = (m&0x1) | ((m&0x2) << 7) | ((m&0x4) >> 1) | ((m&0x8) << 6) | ((m&0x10) >> 2) | ((m&0x20) << 5) | ((m&0x40) >> 3) | ((m&0x80) << 4);
    hpic_utab[m] = (m&0x1) | ((m&0x2) << 1) | ((m&0x4) << 2) | ((m&0x8) << 3) | ((m&0x10) << 4) | ((m&0x20) << 5) | ((m&0x40) << 6) | ((m&0x80) << 7);
  }
  hpic_doneinit = 1;
# ifdef HAVE_LIBPTHREAD  
  pthread_mutex_unlock(&tablock);
# endif
  return;
}

/* low level functions that only advanced users should be          */
/* calling directly.  minimal error checking to increase speed.    */

# if SIZEOF_SIZE_T == 8
/* Definitions for 64bit pixel values.  Note that  */
/* HPIC_NSIDE_MAX must be <= 2^30 in order to have */
/* 12*nside^2 be < 2^64.                           */

int hpic_xy2pix(size_t x, size_t y, size_t *pix) {
  HPIC_CHK;
  (*pix) = hpic_utab[x&0xff] | (hpic_utab[(x>>8)&0xff]<<16) | (hpic_utab[(x>>16)&0xff]<<32) | (hpic_utab[(x>>24)&0xff]<<48) | (hpic_utab[y&0xff]<<1) | (hpic_utab[(y>>8)&0xff]<<17) | (hpic_utab[(y>>16)&0xff]<<33) | (hpic_utab[(y>>24)&0xff]<<48);
  return HPIC_ERR_NONE;
}

int hpic_x2pix(size_t x, size_t *pix) {
  HPIC_CHK;
  (*pix) = hpic_utab[x&0xff] | (hpic_utab[x>>8]<<16) | (hpic_utab[(x>>16)&0xff]<<32) | (hpic_utab[(x>>24)&0xff]<<48);
  return HPIC_ERR_NONE;
}

int hpic_y2pix(size_t y, size_t *pix) {
  HPIC_CHK;
  (*pix) = (hpic_utab[y&0xff]<<1) | (hpic_utab[y>>8]<<17) | (hpic_utab[(y>>16)&0xff]<<33) | (hpic_utab[(y>>24)&0xff]<<48);
  return HPIC_ERR_NONE;
}

int hpic_pix2xy(size_t pix, size_t *x, size_t *y) {
  size_t raw;
  HPIC_CHK;
  raw = (pix&0x5555) | ((pix&0x55550000)>>15) | ((pix&0x555500000000)>>16) | ((pix&0x5555000000000000)>>31);
  (*x) = hpic_ctab[raw&0xff] | (hpic_ctab[(raw>>8)&0xff]<<4) | (hpic_ctab[(raw>>16)&0xff]<<16) | (hpic_ctab[(raw>>24)&0xff]<<20);
  raw = ((pix&0xaaaa)>>1) | ((pix&0xaaaa0000)>>16) | ((pix&0xaaaa00000000)>>17) | ((pix&0xaaaa000000000000)>>32);
  (*y) = hpic_ctab[raw&0xff] | (hpic_ctab[(raw>>8)&0xff]<<4) | (hpic_ctab[(raw>>16)&0xff]<<16) | (hpic_ctab[(raw>>24)&0xff]<<20);
  return HPIC_ERR_NONE;
}

# else
/* Definitions for 32bit pixel values.  Note that  */
/* HPIC_NSIDE_MAX must be <= 2^14 in order to have */
/* 12*nside^2 be < 2^32.                           */

int hpic_xy2pix(size_t x, size_t y, size_t *pix) {
  HPIC_CHK;
  (*pix) = hpic_utab[x&0xff] | (hpic_utab[x>>8]<<16) | (hpic_utab[y&0xff]<<1) | (hpic_utab[y>>8]<<17);
  return HPIC_ERR_NONE;
}

int hpic_x2pix(size_t x, size_t *pix) {
  HPIC_CHK;
  (*pix) = hpic_utab[x&0xff] | (hpic_utab[x>>8]<<16);
  return HPIC_ERR_NONE;
}

int hpic_y2pix(size_t y, size_t *pix) {
  HPIC_CHK;
  (*pix) = (hpic_utab[y&0xff]<<1) | (hpic_utab[y>>8]<<17);
  return HPIC_ERR_NONE;
}

int hpic_pix2xy(size_t pix, size_t *x, size_t *y) {
  size_t raw;
  HPIC_CHK;
  raw = (pix&0x5555) | ((pix&0x55550000)>>15);
  (*x) = hpic_ctab[raw&0xff] | (hpic_ctab[raw>>8]<<4);
  raw = ((pix&0xaaaa)>>1) | ((pix&0xaaaa0000)>>16);
  (*y) = hpic_ctab[raw&0xff] | (hpic_ctab[raw>>8]<<4);
  return HPIC_ERR_NONE;
}

# endif

int hpic_nest2xyf (size_t nside, size_t pix, size_t *x, size_t *y, size_t *face_num) {
  int err;
  size_t factor = hpic_nside2factor(nside);
  (*face_num) = pix >> (2*factor);
  err = hpic_pix2xy(pix & (nside*nside-1), x, y);
  return err;
}

int hpic_xyf2nest (size_t nside, size_t x, size_t y, size_t face_num, size_t *pix) {
  int err;
  size_t factor = hpic_nside2factor(nside);
  err = hpic_xy2pix(x, y, pix);
  (*pix) += (face_num<<(2*factor));
  return err;
}

int hpic_ring2xyf (size_t nside, size_t pix, size_t *x, size_t *y, size_t *face_num) {
  long iring, iphi, kshift, nr;
  long nl2 = 2 * nside;
  size_t ncap = 2 * (nside*nside - nside);
  long tmp, ip, ire, irm, ifm, ifp, irt, ipt;
  long factor = (long int)hpic_nside2factor(nside);
  long face;

  if (pix < ncap) {
    iring = (long)(0.5*(1.0+sqrt((double)(1+2*pix))));
    iphi  = (long)(pix+1) - 2*iring*(iring-1);
    kshift = 0;
    nr = iring;
    face = 0;
    tmp = iphi-1;
    if (tmp >= (2*iring)) {
      face = 2;
      tmp -= 2*iring;
    }
    if (tmp >= iring) {
      face++;
    }
  } else if (pix < (12*nside*nside - ncap)) {
    ip = (long)(pix - ncap);
    iring = (ip>>(factor+2)) + (long)nside;
    iphi = (ip&(4*(long)nside-1)) + 1;
    kshift = (iring+(long)nside)&1;
    nr = (long)nside;
    ire = iring - (long)nside + 1;
    irm = nl2+2-ire;
    ifm = (iphi - ire/2 + (long)nside - 1) >> factor;
    ifp = (iphi - irm/2 + (long)nside - 1) >> factor;
    if (ifp == ifm) /* faces 4 to 7 */
      face = (ifp==4) ? 4 : ifp+4;
    else if (ifp < ifm) /* (half-)faces 0 to 3 */
      face = ifp;
    else /* (half-)faces 8 to 11 */
      face = ifm + 8;
  } else {
    ip = (long)(12*nside*nside - pix);
    iring = (long)(0.5*(1.0+sqrt((double)(2*ip-1))));
    iphi  = 4*iring + 1 - (ip - 2*iring*(iring-1));
    kshift = 0;
    nr = iring;
    iring = 2*nl2-iring;
    face = 8;
    tmp = iphi-1;
    if (tmp >= (2*nr)) {
      face = 10;
      tmp -= 2*nr;
    }
    if (tmp >= nr) face++;
  }

  irt = iring - (long)(hpic_jrll[face]*nside) + 1;
  ipt = 2*iphi - (long)hpic_jpll[face]*nr - kshift -1;
  if (ipt >= nl2) ipt -= 8*nside;

  (*face_num) = face;
  (*x) = (size_t)((ipt-irt) >> 1);
  (*y) = (size_t)((-(ipt+irt)) >> 1);
  
  return HPIC_ERR_NONE;
}

int hpic_xyf2ring (size_t nside, size_t x, size_t y, size_t face_num, size_t *pix) {
  long nl4 = 4*(long)nside;
  long jr = (long)hpic_jrll[face_num];
  long nr, kshift, n_before;
  long ncap = 2 * (long)(nside*nside - nside);
  long jp;
  
  jr = (jr*(long)nside) - (long)x - (long)y - 1;
  /*
  printf("  jr = %ld\n", jr);
  fflush(stdout);
  */
  if (jr < (long)nside) {
    nr = jr;
    n_before = 2 * nr * (nr-1);
    kshift = 0;
  } else if (jr > (long)(3*nside)) {
    nr = nl4 - jr;
    n_before = (long)(12*nside*nside) - 2*(nr+1)*nr;
    kshift = 0;
  } else {
    nr = (long)nside;
    /*
    printf("  nr = %ld\n", nr);
    fflush(stdout);
    */
    n_before = ncap + (jr - (long)nside)*nl4;
    /*
    printf("  n_before = %ld\n", n_before);
    fflush(stdout);
    */
    kshift = (jr - (long)nside)&1;
    /*
    printf("  kshift = %ld\n", kshift);
    fflush(stdout);
    */
  }

  jp = (long)((long)((long)hpic_jpll[face_num]*nr + (long)x - (long)y + 1 + kshift) / 2);
  /*
  printf("  jp = %ld\n", jp);
  fflush(stdout);
  */
  if (jp > nl4) {
    jp -= nl4;
  } else {
    if (jp < 1) jp += nl4;
  }
  /*
  printf("  prereturn = %ld\n", n_before+jp-1);
  fflush(stdout);
  */
  (*pix) = (size_t)(n_before + jp - 1);
  
  return HPIC_ERR_NONE;
}

/* slightly higher-level functions */

int hpic_nest2ring (size_t nside, size_t pnest, size_t *pring) {
  int err;
  size_t x, y, face_num;

  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR(err,"nside value is not valid");
  }
  if (pnest > (12 * nside * nside - 1)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"pixel value out of range");
  }
  err = hpic_nest2xyf(nside, pnest, &x, &y, &face_num);
  /*
  printf(" nest2xyf:  %lu  x = %lu  y = %lu  face = %lu\n", pnest, x, y, face_num);
  fflush(stdout);
  */
  if (err) {
    return err;
  }
  err = hpic_xyf2ring(nside, x, y, face_num, pring);
  /*
  printf(" xyf2ring:  x = %lu  y = %lu  face = %lu  %lu\n", x, y, face_num, (*pring));
  */
  fflush(stdout);
  return err;
}

int hpic_ring2nest(size_t nside, size_t pring, size_t *pnest) {
  int err;
  size_t x, y, face_num;
  err = hpic_nsidecheck(nside);
  if (err) {
    HPIC_ERROR(err,"nside value is not valid");
  }
  if (pring > (12 * nside * nside - 1)) {
    HPIC_ERROR(HPIC_ERR_RANGE,"pixel value out of range");
  }
  err = hpic_ring2xyf(nside, pring, &x, &y, &face_num);
  /*
  printf("ring2xyf:  %lu  x = %lu  y = %lu  face = %lu\n", pring, x, y, face_num);
  */
  fflush(stdout);
  if (err) {
    return err;
  }
  err = hpic_xyf2nest(nside, x, y, face_num, pnest);
  /*
  printf(" xyf2nest:  x = %lu  y = %lu  face = %lu  %lu\n", x, y, face_num, (*pnest));
  */
  fflush(stdout);
  return err;
}

int hpic_ang2pix_ring(size_t nside, double theta, double phi, size_t *pix) {
  double z = cos(theta);
  double za = fabs(z);
  double tt;
  double intpart;
  long longpart;
  double temp1, temp2;
  long jp, jm;
  long ip, ir, kshift;
  size_t ncap = 2 * (HPIC_NSIDE_MAX*HPIC_NSIDE_MAX - HPIC_NSIDE_MAX);
  double tp, tmp;
  size_t pixmax;
  int err;
  
  while (phi < 0.0) {
    phi += 2.0 * HPIC_PI;
  }
  intpart = floor(phi / (2.0*HPIC_PI));
  phi -= intpart;
  tt = 2.0 * phi / HPIC_PI;
  /*
  printf(" ang2pix:  z = %e  za = %e  tt = %e\n", z, za, tt);
  fflush(stdout);
  */
  if (za <= (2.0/3.0)) {
    temp1 = (double)HPIC_NSIDE_MAX * (0.5+tt);
    temp2 = (double)HPIC_NSIDE_MAX * z * 0.75;
    /*
    printf(" ang2pix:  temp1 = %e temp2 = %e\n", temp1, temp2);
    fflush(stdout);
    */
    jp = (long)(temp1 - temp2); /* index of  ascending edge line */
    jm = (long)(temp1 + temp2); /* index of descending edge line */
    /*
    printf(" ang2pix:  jp = %ld jm = %ld\n", jp, jm);
    fflush(stdout);
    */
    /* ring number counted from z=2/3 */
    ir = HPIC_NSIDE_MAX + 1 + jp - jm; /* in {1,2n+1} */
    kshift = 1 - (ir&1); /* kshift=1 if ir even, 0 otherwise */

    ip = (long)((long)(jp + jm - HPIC_NSIDE_MAX + kshift + 1) / 2); /* in {0,4n-1} */
    ip = ip % (long)(4*HPIC_NSIDE_MAX);

    pixmax = ncap + (size_t)((ir-1) * 4 * HPIC_NSIDE_MAX + ip);
    /*
    printf(" ang2pix:  ip = %ld ir = %ld pixmax = %lu\n", ip, ir, pixmax);
    fflush(stdout);
    */
  } else {
    tp = tt - floor(tt);
    tmp = (double)(HPIC_NSIDE_MAX) * sqrt(3.0 * (1.0 - za));
    /*
    printf(" ang2pix:  dbl(max) = %e sqrt() = %e tmp = %e\n", (double)HPIC_NSIDE_MAX, sqrt(3.0 * (1.0-za)), tmp);
    printf(" ang2pix:  tp = %e tt = %e floor(tt) = %e\n", tp, tt, floor(tt));
    fflush(stdout);
    */
    jp = (long)(tp * tmp); /* increasing edge line index */
    jm = (long)((1.0-tp)*tmp); /* decreasing edge line index */
    /*
    printf(" ang2pix:  jp = %ld jm = %ld\n", jp, jm);
    fflush(stdout);
    */
    ir = jp + jm + 1; /* ring number counted from the closest pole */
    ip = (long)(tt * (double)ir); /* in {0,4*ir-1} */
    longpart = (long)(ip / (4*ir));
    ip -= longpart;
    /*
    printf(" ang2pix:  (tt*ir) = %e longpart = %ld ip = %ld\n", tt*(double)ir, (long)(ip / (4*ir)), ip);
    fflush(stdout);
    */
 
    if (z > 0.0) {
      pixmax = (size_t)(2 * ir * (ir-1) + ip);
    } else {
      pixmax = (size_t)((long)(12*HPIC_NSIDE_MAX*HPIC_NSIDE_MAX) - 2*ir*(ir+1) + ip);
    }
    /*
    printf(" ang2pix:  ip = %ld ir = %ld pixmax = %lu\n", ip, ir, pixmax);
    fflush(stdout);
    */
  }
  err = hpic_degrade_ring(HPIC_NSIDE_MAX, pixmax, nside, pix);
  return err;
}

int hpic_ang2pix_nest(size_t nside, double theta, double phi, size_t *pix) {
  double z = cos(theta);
  double za = fabs(z);
  double tt;
  double intpart;
  double temp1, temp2;
  long jp, jm;
  long face, x, y;
  long ifp, ifm, ntt;
  size_t sipf;
  size_t maxfactor;
  size_t factor;
  int err;
  double tp, tmp;
  size_t pixmax;
  
  while (phi < 0.0) {
    phi += 2.0 * HPIC_PI;
  }
  intpart = floor(phi / (2.0*HPIC_PI));
  phi -= intpart;
  tt = 2.0 * phi / HPIC_PI;
  
  maxfactor = hpic_nside2factor(HPIC_NSIDE_MAX);
  factor = hpic_nside2factor(nside);
  /*
  printf(" ang2pix:  z = %e  za = %e  tt = %e  maxfactor = %lu  factor = %lu\n", z, za, tt, maxfactor, factor);
  fflush(stdout);
  */
  if (za <= (2.0/3.0)) {
    temp1 = (double)HPIC_NSIDE_MAX * (0.5+tt);
    temp2 = (double)HPIC_NSIDE_MAX * z * 0.75;
    jp = (long)(temp1-temp2); /* index of  ascending edge line */
    jm = (long)(temp1+temp2); /* index of descending edge line */
    /*
    printf(" ang2pix:  jp = %ld jm = %ld\n", jp, jm);
    fflush(stdout);
    */
    ifp = jp >> maxfactor;  /* in {0,4} */
    ifm = jm >> maxfactor;
    /*
    printf(" ang2pix:  ifp = %ld ifm = %ld\n", ifp, ifm);
    fflush(stdout);
    */
    if (ifp == ifm) {          /* faces 4 to 7 */
      face = (ifp==4) ? (long)4 : ifp+4;
    } else if (ifp < ifm) {    /* (half-)faces 0 to 3 */
        face = ifp;
    } else {                   /* (half-)faces 8 to 11 */
        face = ifm + 8;
    }
    x = jm & (long)(HPIC_NSIDE_MAX-1);
    y = (long)HPIC_NSIDE_MAX - (jp & (long)(HPIC_NSIDE_MAX-1)) - 1;
    /*
    printf(" ang2pix:  x = %ld y = %ld face = %ld\n", x, y, face);
    fflush(stdout);
    */
  } else {
    ntt = (long)tt;
    tp = tt - (double)ntt;
    tmp = (double)HPIC_NSIDE_MAX * sqrt(3.0*(1.0-za));
    /*
    printf(" ang2pix:  ntt = %ld tp = %e sqrt() = %e tmp = %e\n", ntt, tp, sqrt(3.0 * (1.0-za)), tmp);
    fflush(stdout);
    */
    jp = (long)(tp*tmp); /* increasing edge line index */
    jm = (long)((1.0-tp)*tmp); /* decreasing edge line index */
    /*
    printf(" ang2pix:  jp = %ld jm = %ld\n", jp, jm);
    fflush(stdout);
    */
    if (jp >= (long)HPIC_NSIDE_MAX) {
      jp = (long)HPIC_NSIDE_MAX-1; /* for points too close to the boundary */
    }
    if (jm >= (long)HPIC_NSIDE_MAX) {
      jm = (long)HPIC_NSIDE_MAX-1;
    }
    /*
    printf(" ang2pix:  jp = %ld jm = %ld\n", jp, jm);
    fflush(stdout);
    */
    if (z >= 0) {
      face = ntt;  /* in {0,3} */
      x = HPIC_NSIDE_MAX - jm - 1;
      y = HPIC_NSIDE_MAX - jp - 1;
    } else {
      face = ntt + 8; /* in {8,11} */
      x = jp;
      y = jm;
    }
    /*
    printf(" ang2pix:  face = %ld x = %ld y = %ld\n", face, x, y);
    fflush(stdout);
    */
  }

  err = hpic_xy2pix((size_t)x, (size_t)y, &sipf);
  if (err) {
    return err;
  }

  pixmax = sipf + (size_t)(face << (2*maxfactor));
  
  err = hpic_degrade_nest(HPIC_NSIDE_MAX, pixmax, nside, pix);
  /*
  printf(" ang2pix:  pixmax = %lu pix = %lu\n", pixmax, (*pix));
  fflush(stdout);
  */
  return err;
}

int hpic_pix2ang_ring(size_t nside, size_t pix, double *theta, double *phi){
  size_t ncap = 2 * (nside*nside - nside);
  long iring, iphi;
  long ip, nl2;
  double fodd;
  
  if (pix < ncap) {
    iring = (long)(0.5*(1.0+sqrt(1.0+2.0*(double)pix)));
    iphi  = (long)(pix+1) - 2 * iring * (iring-1);
    /*
    printf(" pix2ang:  iring = %ld iphi = %ld\n", iring, iphi);
    fflush(stdout);
    */
    (*theta) = acos(1.0 - (double)(iring*iring) / (double)(3*nside*nside));
    (*phi) = ((double)iphi - 0.5) * HPIC_PI / (2.0 * (double)iring);
    /*
    printf(" pix2ang:  theta = %e phi = %e\n", (*theta), (*phi));
    fflush(stdout);
    */
  } else if (pix < (12*nside*nside-ncap)) {
    ip  = (long)pix - (long)ncap;
    iring = (long)(ip / (4*(long)nside)) + (long)nside; /* counted from North pole */
    iphi  = ip % (4*(long)nside) + 1; /* 1 if iring+nside is odd, 1/2 otherwise */
    fodd = ((iring+(long)nside)&1) ? 1.0 : 0.5;
    nl2 = 2 * (long)nside;
    
    (*theta) = acos((double)(nl2-iring) * 2.0 / (double)(3*nside));
    (*phi) = ((double)iphi - fodd) * HPIC_PI / (double)nl2;
  } else {
    ip = (long)(12*nside*nside - pix);
    iring = (long)(0.5*(1.0+sqrt((double)(2*ip-1))));
    iphi = 4*iring + 1 - (ip - 2*iring*(iring-1));

    (*theta) = acos(-1.0 + (double)(iring*iring) / (double)(3*nside*nside));
    (*phi) = ((double)iphi - 0.5) * HPIC_PI / (double)(2*iring);
  }
  return HPIC_ERR_NONE;
}

int hpic_pix2ang_nest(size_t nside, size_t pix, double *theta, double *phi){
  size_t factor = hpic_nside2factor(nside);
  long nl4 = (long)(nside*4);
  long face = (long)pix >> (2*factor);
  long ipf = (long)pix & (long)(nside*nside-1);
  long x, y;
  size_t sx, sy;
  long jr, nr, kshift, jp;
  double z;
  int err;
  
  err = hpic_pix2xy((size_t)ipf, &sx, &sy);
  if (err) {
    return err;
  }
  x = (long)sx;
  y = (long)sy;

  jr = (long)(hpic_jrll[face]<<factor) - x - y - 1;
  /*
  printf(" pix2ang:  x = %ld y = %ld jr = %ld\n", x, y, jr);
  fflush(stdout);
  */
  if (jr < (long)nside) {
    nr = jr;
    z = 1.0 - (double)(nr * nr) / (double)(3*nside*nside);
    kshift = 0;
  } else if (jr > (long)(3*nside)) {
    nr = nl4 - jr;
    z = (double)(nr*nr) / (double)(3*nside*nside) - 1.0;
    kshift = 0;
  } else {
    nr = (long)nside;
    z = (double)((long)(2*nside)-jr) * 2.0 / (double)(3*nside);
    kshift = (jr-(long)nside)&1;
  }

  jp = (long)((long)((long)hpic_jpll[face]*nr + x - y + 1 + kshift) / 2);
  if (jp > nl4) jp -= nl4;
  if (jp < 1) jp += nl4;

  (*theta) = acos(z);
  (*phi) = ((double)jp - (double)(kshift+1)*0.5) * HPIC_HALFPI / (double)nr;
  
  return HPIC_ERR_NONE;
}

int hpic_neighbors(size_t nside, int ordering, size_t pixel, hpic_vec_index *parray) {
  
  int err;
  size_t i;
  size_t ptemp;
  static const int xoffset[] = { -1, 1, 0, 0,-1,-1, 1, 1 };
  static const int yoffset[] = {  0, 0,-1, 1,-1, 1, 1,-1 };
  static const int facearray[][12] =
  { {  8, 9,10,11,-1,-1,-1,-1,10,11, 8, 9 },   /* S */
  {  5, 6, 7, 4, 8, 9,10,11, 9,10,11, 8 },   /* SE */
  { -1,-1,-1,-1, 5, 6, 7, 4,-1,-1,-1,-1 },   /* E */
  {  4, 5, 6, 7,11, 8, 9,10,11, 8, 9,10 },   /* SW */
  {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11 },   /* center */
  {  1, 2, 3, 0, 0, 1, 2, 3, 5, 6, 7, 4 },   /* NE */
  { -1,-1,-1,-1, 7, 4, 5, 6,-1,-1,-1,-1 },   /* W */
  {  3, 0, 1, 2, 3, 0, 1, 2, 4, 5, 6, 7 },   /* NW */
  {  2, 3, 0, 1,-1,-1,-1,-1, 0, 1, 2, 3 } }; /* N */
  static const int swaparray[][12] =
  { {  0,0,0,0,0,0,0,0,3,3,3,3 },   /* S */
  {  0,0,0,0,0,0,0,0,6,6,6,6 },   /* SE */
  {  0,0,0,0,0,0,0,0,0,0,0,0 },   /* E */
  {  0,0,0,0,0,0,0,0,5,5,5,5 },   /* SW */
  {  0,0,0,0,0,0,0,0,0,0,0,0 },   /* center */
  {  5,5,5,5,0,0,0,0,0,0,0,0 },   /* NE */
  {  0,0,0,0,0,0,0,0,0,0,0,0 },   /* W */
  {  6,6,6,6,0,0,0,0,0,0,0,0 },   /* NW */
  {  3,3,3,3,0,0,0,0,0,0,0,0 } }; /* N */
  int x, y, ix, iy, f;
  size_t stx, sty;
  size_t face;
  const size_t nsm1 = nside - 1;
  size_t nbnum;
  int tmp;
  
  hpic_vec_index_resize(parray, 0);
  
  if (ordering == HPIC_RING) {
    err = hpic_ring2xyf(nside, pixel, &stx, &sty, &face);
  } else {    
    err = hpic_nest2xyf(nside, pixel, &stx, &sty, &face);
  }
  if (err) {
    return err;
  }
  ix = (int)stx;
  iy = (int)sty;
    
  if ((ix > 0)&&(ix < (int)nsm1)&&(iy > 0)&&(iy < (int)nsm1)) {
    if (ordering == HPIC_RING) {
      for (i = 0; i < 8; i++) {
        err = hpic_xyf2ring(nside, (size_t)(ix + xoffset[i]), (size_t)(iy+yoffset[i]), face, &ptemp);
        err = hpic_vec_index_append(parray, ptemp);
      }
    } else {
      for (i = 0; i < 8; i++) {
        err = hpic_xyf2nest(nside, (size_t)(ix + xoffset[i]), (size_t)(iy+yoffset[i]), face, &ptemp);
        err = hpic_vec_index_append(parray, ptemp);
      }
    }
  } else {
    for (i = 0; i < 8; i++) {
      x = ix + xoffset[i];
      y = iy + yoffset[i];
      nbnum = 4;
      if (x < 0) { 
        x += (int)nside;
        nbnum -= 1; 
      } else if (x >= (int)nside) { 
        x -= (int)nside;
        nbnum += 1; 
      }
      if (y < 0) {
        y += (int)nside;
        nbnum -= 3; 
      } else if (y >= (int)nside) {
        y -= (int)nside;
        nbnum += 3; 
      }

      f = facearray[nbnum][face];
      if (f >= 0) {
        if (swaparray[nbnum][face]&1) {
          x = (int)nside - x - 1;
        }
        if (swaparray[nbnum][face]&2) {
          y = (int)nside - y - 1;
        }
        if (swaparray[nbnum][face]&4) {
          tmp = x;
          x = y;
          y = tmp;
        }
        if (ordering == HPIC_RING) {
          err = hpic_xyf2ring(nside, (size_t)x, (size_t)y, (size_t)f, &ptemp);
        } else {
          err = hpic_xyf2nest(nside, (size_t)x, (size_t)y, (size_t)f, &ptemp);
        }
        hpic_vec_index_append(parray, ptemp);
      }
    }
  }
  return HPIC_ERR_NONE;
}

