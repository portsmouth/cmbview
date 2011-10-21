/*****************************************************************************
 *                                                                           *
 * This is the header file for the HPIC implementation of the healpix        *
 * pixelization of the sphere.                                               *
 *                                                                           * 
 * Copyright 2003-2005 Theodore Kisner <kisner@physics.ucsb.edu>             *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   * 
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * General Public License for more details.                                  *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                   *
 *                                                                           *
 * A Note on types:  all integer variables describing the size or indices    *
 * of data structures are defined as type "size_t" (usually equivalent to    *
 * unsigned long).  All integer variables taking on parameter values of      *
 * predefined constants are of type "int".                                   *
 *                                                                           *
 *****************************************************************************/

#ifndef HPIC_H
#  define HPIC_H
#  ifdef __cplusplus
extern "C" {
#  endif

#  include <string.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <fitsio.h>
#  include <time.h>
#  include <math.h>

/*****************************************************************************
 * hpic constants                                                            *
 *****************************************************************************/

/* general */

#  ifdef HPIC_PI
#    undef HPIC_PI
#  endif                        /* define pi... */
#  define HPIC_PI M_PI

#  ifdef HPIC_INVPI
#    undef HPIC_INVPI
#  endif                        /* define 1/pi... */
#  define HPIC_INVPI M_1_PI

#  ifdef HPIC_PISQ
#    undef HPIC_PISQ
#  endif                        /* define pi squared... */
#  define HPIC_PISQ 9.86960440108936

#  ifdef HPIC_HALFPI
#    undef HPIC_HALFPI
#  endif                        /* define half pi... */
#  define HPIC_HALFPI M_PI_2

#  ifdef HPIC_NSIDE_MAX
#    undef HPIC_NSIDE_MAX
#  endif                        /* max nside value */
#  define HPIC_NSIDE_MAX 8192

#  ifdef HPIC_STRNL
#    undef HPIC_STRNL
#  endif                        /* max string length */
#  define HPIC_STRNL 200

#  ifdef HPIC_MIN_EXP
#    undef HPIC_MIN_EXP
#  endif                        /* smallest IEEE double exponent */
#  define HPIC_MIN_EXP -1022

#  ifdef HPIC_MAX_EXP
#    undef HPIC_MAX_EXP
#  endif                        /* smallest IEEE double exponent */
#  define HPIC_MAX_EXP 1023

/* map parameters */

#  ifdef HPIC_RING
#    undef HPIC_RING
#  endif                        /* ordering = ring */
#  define HPIC_RING 0

#  ifdef HPIC_NEST
#    undef HPIC_NEST
#  endif                        /* ordering = nested */
#  define HPIC_NEST 1

#  ifdef HPIC_COORD_C
#    undef HPIC_COORD_C
#  endif                        /* coordsys = celestial/equatorial */
#  define HPIC_COORD_C 0

#  ifdef HPIC_COORD_G
#    undef HPIC_COORD_G
#  endif                        /* coordsys = galactic */
#  define HPIC_COORD_G 1

#  ifdef HPIC_COORD_E
#    undef HPIC_COORD_E
#  endif                        /* coordsys = ecliptic */
#  define HPIC_COORD_E 2

#  ifdef HPIC_COORD_O
#    undef HPIC_COORD_O
#  endif                        /* coordsys = other */
#  define HPIC_COORD_O 3

#  ifdef HPIC_STND
#    undef HPIC_STND
#  endif                        /* always store map data in a standard C array */
#  define HPIC_STND 0

#  ifdef HPIC_TREE
#    undef HPIC_TREE
#  endif                        /* always store map data in a tree */
#  define HPIC_TREE 1

#  ifdef HPIC_AUTO
#    undef HPIC_AUTO
#  endif                        /* automatically switch memory structure based on map size */
#  define HPIC_AUTO 2

/* vector parameters */

#  ifdef HPIC_VECBUF
#    undef HPIC_VECBUF
#  endif                        /* size of the extra space realloc'ed */
#  define HPIC_VECBUF 10

/* projection parameters */

#  ifdef HPIC_PROJ_CAR
#    undef HPIC_PROJ_CAR
#  endif                        /* projection = cartesian */
#  define HPIC_PROJ_CAR 0

#  ifdef HPIC_PROJ_SIN
#    undef HPIC_PROJ_SIN
#  endif                        /* projection = sinusoidal */
#  define HPIC_PROJ_SIN 1

/* math parameters */

#  ifdef HPIC_INTERSECT
#    undef HPIC_INTERSECT
#  endif                        /* take the intersection */
#  define HPIC_INTERSECT 0

#  ifdef HPIC_UNION
#    undef HPIC_UNION
#  endif                        /* take the union */
#  define HPIC_UNION 1

/* FITS parameters */

#  ifdef HPIC_FITS_FULL
#    undef HPIC_FITS_FULL
#  endif                        /* file contains full-sphere map(s) */
#  define HPIC_FITS_FULL 0

#  ifdef HPIC_FITS_CUT
#    undef HPIC_FITS_CUT
#  endif                        /* file contains cut-sphere map(s) */
#  define HPIC_FITS_CUT 1

#  ifdef HPIC_FITS_CHUNK
#    undef HPIC_FITS_CHUNK
#  endif                        /* file contains chunks of full-sphere map(s) */
#  define HPIC_FITS_CHUNK 2

#  ifdef HPIC_FITS_BIN
#    undef HPIC_FITS_BIN
#  endif                        /* file contains a binary extension */
#  define HPIC_FITS_BIN 0

#  ifdef HPIC_FITS_ASCII
#    undef HPIC_FITS_ASCII
#  endif                        /* file contains an ascii extension */
#  define HPIC_FITS_ASCII 1

#  ifdef HPIC_FITS_VEC
#    undef HPIC_FITS_VEC
#  endif                        /* file contains only float vectors */
#  define HPIC_FITS_VEC 0

#  ifdef HPIC_FITS_VEC_INDX
#    undef HPIC_FITS_VEC_INDX
#  endif                        /* file contains an index column plus float vectors */
#  define HPIC_FITS_VEC_INDX 1

#  ifdef HPIC_FITS_MAXCOL
#    undef HPIC_FITS_MAXCOL
#  endif                        /* maximum number of table columns supported */
#  define HPIC_FITS_MAXCOL 50

#  ifdef HPIC_NULL
#    undef HPIC_NULL
#  endif                        /* null value for float maps */
#  define HPIC_NULL (-1.6375e30)

#  ifdef HPIC_EPSILON
#    undef HPIC_EPSILON
#  endif                        /* range around HPIC_NULL that is considered equal to NULL */
#  define HPIC_EPSILON (0.0001e30)

#  ifdef HPIC_INT_NULL
#    undef HPIC_INT_NULL
#  endif                        /* null value for int maps */
#  define HPIC_INT_NULL (-2147483646)

/* ERROR Codes */

#  ifdef HPIC_ERR_NONE
#    undef HPIC_ERR_NONE
#  endif                        /* no error */
#  define HPIC_ERR_NONE 0

#  ifdef HPIC_ERR_ALLOC
#    undef HPIC_ERR_ALLOC
#  endif                        /* allocation error */
#  define HPIC_ERR_ALLOC 1

#  ifdef HPIC_ERR_FREE
#    undef HPIC_ERR_FREE
#  endif                        /* freeing error */
#  define HPIC_ERR_FREE 2

#  ifdef HPIC_ERR_NSIDE
#    undef HPIC_ERR_NSIDE
#  endif                        /* illegal nside value */
#  define HPIC_ERR_NSIDE 3

#  ifdef HPIC_ERR_ORDER
#    undef HPIC_ERR_ORDER
#  endif                        /* illegal ordering value */
#  define HPIC_ERR_ORDER 4

#  ifdef HPIC_ERR_COORD
#    undef HPIC_ERR_COORD
#  endif                        /* illegal coordinate value */
#  define HPIC_ERR_COORD 5

#  ifdef HPIC_ERR_RANGE
#    undef HPIC_ERR_RANGE
#  endif                        /* value is out of range */
#  define HPIC_ERR_RANGE 6

#  ifdef HPIC_ERR_ACCESS
#    undef HPIC_ERR_ACCESS
#  endif                        /* data is not accessible */
#  define HPIC_ERR_ACCESS 7

#  ifdef HPIC_ERR_PROJ
#    undef HPIC_ERR_PROJ
#  endif                        /* projection error */
#  define HPIC_ERR_PROJ 8

#  ifdef HPIC_ERR_FITS
#    undef HPIC_ERR_FITS
#  endif                        /* FITS error */
#  define HPIC_ERR_FITS 9

/*****************************************************************************
 * Global variables and library initialization                               *
 *****************************************************************************/

static size_t hpic_ctab[0x100];
static size_t hpic_utab[0x100];

static const size_t hpic_jrll[] = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};
static const size_t hpic_jpll[] = {1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};

static int hpic_doneinit = 0;
void hpic_init();

static int hpic_donelegacy = 0;
void hpic_legacynotice();

/* error handling */

typedef void hpic_error_handler_t (int errcode, const char *file, 
                                   int line, const char *msg);
static hpic_error_handler_t * hpic_error_handler = NULL;

/*****************************************************************************
 * hpic base types                                                           *
 *****************************************************************************/
  
/* tree structures */

/* The children are void pointers so that they can EITHER point to other     */
/* nodes OR point to a pixel value (if they are a leaf node).  Since the     */
/* trees are only descended, there is no need for a link to the parent node. */

  typedef struct {              /* hpic tree node */
    void *child00;
    void *child01;
    void *child10;
    void *child11;
  } hpic_node;

  typedef struct {              /* hpic tree */
    size_t nside;
    size_t shiftroot;
    size_t nnodes;
    hpic_node *root00;
    hpic_node *root01;
    hpic_node *root02;
    hpic_node *root03;
    hpic_node *root04;
    hpic_node *root05;
    hpic_node *root06;
    hpic_node *root07;
    hpic_node *root08;
    hpic_node *root09;
    hpic_node *root10;
    hpic_node *root11;
  } hpic_tree;

/* map structures */

  typedef struct {              /* hpic map */
    char *name;
    char *units;
    size_t nside;
    size_t npix;
    int order;
    int coord;
    int mem;
    int curstate;
    double *data;
    hpic_tree *tree;
    size_t nnodes;
    size_t transhigh;
    size_t translow;
  } hpic;

  typedef struct {              /* hpic float map */
    char *name;
    char *units;
    size_t nside;
    size_t npix;
    int order;
    int coord;
    int mem;
    int curstate;
    float *data;
    hpic_tree *tree;
    size_t nnodes;
    size_t transhigh;
    size_t translow;
  } hpic_float;

  typedef struct {              /* hpic int map */
    char *name;
    char *units;
    size_t nside;
    size_t npix;
    int order;
    int coord;
    int mem;
    int curstate;
    int *data;
    hpic_tree *tree;
    size_t nnodes;
    size_t transhigh;
    size_t translow;
  } hpic_int;

  typedef struct {              /* array of float maps */
    size_t nmaps;
    hpic_float **maps;
  } hpic_fltarr;

/*****************************************************************************
 * hpic vector types                                                         *
 *****************************************************************************/

  typedef struct {              /* hpic vector */
    size_t n;
    size_t size;
    double *data;
  } hpic_vec;

  typedef struct {              /* hpic float vector */
    size_t n;
    size_t size;
    float *data;
  } hpic_vec_float;

  typedef struct {              /* hpic int vector */
    size_t n;
    size_t size;
    int *data;
  } hpic_vec_int;

  typedef struct {              /* hpic index vector */
    size_t n;
    size_t size;
    size_t *data;
  } hpic_vec_index;

  typedef struct {              /* array of float vectors */
    size_t nvecs;
    hpic_vec_float **vecs;
  } hpic_vec_fltarr;

/*****************************************************************************
 * hpic projection type                                                      *
 *****************************************************************************/

  typedef struct {              /* hpic map projection */
    int type;
    size_t nx;
    size_t ny;
    double mintheta;
    double maxtheta;
    double minphi;
    double maxphi;
    double *data;               /*flat packed array of data */
  } hpic_proj;

/*****************************************************************************
 * hpic FITS key structure                                                   *
 *****************************************************************************/

  typedef struct {              /* hpic FITS keys */
    size_t nskeys;
    size_t nikeys;
    size_t nfkeys;
    char **skeynames;
    char **skeyvals;
    char **skeycoms;
    char **ikeynames;
    int *ikeyvals;
    char **ikeycoms;
    char **fkeynames;
    float *fkeyvals;
    char **fkeycoms;
  } hpic_keys;

/*****************************************************************************
 * function declarations                                                     *
 *****************************************************************************/

/* error handling */
  
  void hpic_error(int errcode, const char *file, int line, const char *msg);
  void hpic_error_default(int errcode, const char *file, int line, const char *msg);
  hpic_error_handler_t* hpic_set_error_handler(hpic_error_handler_t *newhandler);
  
  /* macros for calling the error handler and returning */

  #define HPIC_ERROR(errcode, msg) \
        do { \
        hpic_error (errcode, __FILE__, __LINE__, msg); \
        return errcode; \
        } while (0)

  #define HPIC_ERROR_VAL(errcode, msg, value) \
        do { \
        hpic_error (errcode, __FILE__, __LINE__, msg); \
        return value; \
        } while (0)

  #define HPIC_ERROR_VOID(errcode, msg) \
        do { \
        hpic_error (errcode, __FILE__, __LINE__, msg); \
        return; \
        } while (0)

/* general */

  int hpic_is_dnull(double val);
  int hpic_is_fnull(float val);
  int hpic_is_inull(int val);
  char **hpic_strarr_alloc(size_t nstring);
  int hpic_strarr_free(char **array, size_t nstring);
  
  /* LEGACY - these will eventually be removed in favor  */
  /* of the hpic_* versions.  This will reduce namespace */
  /* pollution and allow the user to mix functions from  */
  /* hpic and the various other healpix libraries.       */ 
  int is_dnull(double val);
  int is_fnull(float val);
  int is_inull(int val);

/* pixel tools */

  int hpic_nsidecheck(size_t nside);
  size_t hpic_nside2npix(size_t nside);
  size_t hpic_npix2nside(size_t npix);
  size_t hpic_nside2factor(size_t nside);
  size_t hpic_factor2nside(size_t factor);
  
  int hpic_xy2pix(size_t x, size_t y, size_t *pix);
  int hpic_x2pix(size_t x, size_t *pix);
  int hpic_y2pix(size_t y, size_t *pix);
  int hpic_pix2xy(size_t pix, size_t *x, size_t *y);
  
  int hpic_nest2xyf (size_t nside, size_t pix, size_t *x, 
                     size_t *y, size_t *face_num);
  int hpic_xyf2nest (size_t nside, size_t x, size_t y, 
                     size_t face_num, size_t *pix);
  int hpic_ring2xyf (size_t nside, size_t pix, size_t *x, 
                     size_t *y, size_t *face_num);
  int hpic_xyf2ring (size_t nside, size_t x, size_t y, 
                     size_t face_num, size_t *pix);
  
  int hpic_nest2ring (size_t nside, size_t pnest, size_t *pring);
  int hpic_ring2nest(size_t nside, size_t pring, size_t *pnest);
  int hpic_ang2pix_ring(size_t nside, double theta, double phi, 
                        size_t *pix);
  int hpic_ang2pix_nest(size_t nside, double theta, double phi, 
                        size_t *pix);
  int hpic_pix2ang_ring(size_t nside, size_t pix, double *theta, 
                        double *phi);
  int hpic_pix2ang_nest(size_t nside, size_t pix, double *theta, 
                        double *phi);
    
  int hpic_vec2ang(double xcomp, double ycomp, double zcomp, 
                   double *theta, double *phi);
  int hpic_ang2vec(double theta, double phi, double *xcomp, 
                   double *ycomp, double *zcomp);
  int hpic_pix2vec_ring(size_t nside, size_t pix, double *xcomp, 
                        double *ycomp, double *zcomp);
  int hpic_pix2vec_nest(size_t nside, size_t pix, double *xcomp, 
                        double *ycomp, double *zcomp);
  int hpic_vec2pix_ring(size_t nside, double xcomp, double ycomp, 
                        double zcomp, size_t * pix);
  int hpic_vec2pix_nest(size_t nside, double xcomp, double ycomp, 
                        double zcomp, size_t * pix);

  int hpic_degrade_nest(size_t oldnside, size_t oldpix, size_t newnside,
                        size_t * newpix);
  int hpic_degrade_ring(size_t oldnside, size_t oldpix, size_t newnside,
                        size_t * newpix);
  int hpic_neighbors(size_t nside, int ordering, size_t pixel, hpic_vec_index *parray);
  
  /* LEGACY - these will eventually be removed in favor  */
  /* of the hpic_* versions.  This will reduce namespace */
  /* pollution and allow the user to mix functions from  */
  /* hpic and the various other healpix libraries.       */ 
  int nsidecheck(size_t nside);
  size_t nside2npix(size_t nside);
  size_t npix2nside(size_t npix);
  int nest2ring(size_t nside, size_t pnest, size_t * pring);
  int ring2nest(size_t nside, size_t pring, size_t * pnest);
  int pix2ang_ring(size_t nside, size_t pix, double *theta, double *phi);
  int pix2ang_nest(size_t nside, size_t pix, double *theta, double *phi);
  int ang2pix_ring(size_t nside, double theta, double phi, size_t * pix);
  int ang2pix_nest(size_t nside, double theta, double phi, size_t * pix);
  int vec2ang(double xcomp, double ycomp, double zcomp, double *theta,
              double *phi);
  int ang2vec(double theta, double phi, double *xcomp, double *ycomp,
              double *zcomp);
  int pix2vec_ring(size_t nside, size_t pix, double *xcomp, 
                   double *ycomp, double *zcomp);
  int pix2vec_nest(size_t nside, size_t pix, double *xcomp, 
                   double *ycomp, double *zcomp);
  int vec2pix_ring(size_t nside, double xcomp, double ycomp, 
                   double zcomp, size_t * pix);
  int vec2pix_nest(size_t nside, double xcomp, double ycomp, 
                   double zcomp, size_t * pix);
  int degrade_nest(size_t oldnside, size_t oldpix, size_t newnside,
                   size_t * newpix);
  int degrade_ring(size_t oldnside, size_t oldpix, size_t newnside,
                   size_t * newpix);
  
/* projection tools */

  int hpic_proj_car(double mintheta, double maxtheta, double minphi,
                    double maxphi, double xmax, double ymax, 
                    double theta, double phi, double *x, double *y);
  int hpic_proj_sin(double mintheta, double maxtheta, double minphi,
                    double maxphi, double xmax, double ymax, 
                    double theta, double phi, double *x, double *y);
  int hpic_proj_rev_car(double mintheta, double maxtheta, double minphi,
                        double maxphi, double xmax, double ymax, 
                        double x, double y, double *theta, double *phi);
  int hpic_proj_rev_sin(double mintheta, double maxtheta, double minphi,
                        double maxphi, double xmax, double ymax, 
                        double x, double y, double *theta, double *phi);
  
  /* LEGACY - these will eventually be removed in favor  */
  /* of the hpic_* versions.  This will reduce namespace */
  /* pollution and allow the user to mix functions from  */
  /* hpic and the various other healpix libraries.       */
  int proj_car(double mintheta, double maxtheta, double minphi,
               double maxphi, double xmax, double ymax, double theta,
               double phi, double *x, double *y);
  int proj_sin(double mintheta, double maxtheta, double minphi,
               double maxphi, double xmax, double ymax, double theta,
               double phi, double *x, double *y);
  int proj_rev_car(double mintheta, double maxtheta, double minphi,
                   double maxphi, double xmax, double ymax, double x,
                   double y, double *theta, double *phi);
  int proj_rev_sin(double mintheta, double maxtheta, double minphi,
                   double maxphi, double xmax, double ymax, double x,
                   double y, double *theta, double *phi);

/* location tools */

  double hpic_loc_dist(size_t nside, int order, size_t pix1, size_t pix2);

/* map operations */

  hpic *hpic_alloc(size_t nside, int order, int coord, int mem);
  int hpic_free(hpic * map);
  hpic_float *hpic_float_alloc(size_t nside, int order, int coord, int mem);
  int hpic_float_free(hpic_float * map);
  hpic_int *hpic_int_alloc(size_t nside, int order, int coord, int mem);
  int hpic_int_free(hpic_int * map);

  int hpic_name_set(hpic * map, const char *name);
  int hpic_units_set(hpic * map, const char *units);
  char *hpic_name_get(hpic * map);
  char *hpic_units_get(hpic * map);
  size_t hpic_nside_get(hpic * map);
  size_t hpic_npix_get(hpic * map);
  int hpic_order_get(hpic * map);
  int hpic_coord_get(hpic * map);
  int hpic_float_name_set(hpic_float * map, const char *name);
  int hpic_float_units_set(hpic_float * map, const char *units);
  char *hpic_float_name_get(hpic_float * map);
  char *hpic_float_units_get(hpic_float * map);
  size_t hpic_float_nside_get(hpic_float * map);
  size_t hpic_float_npix_get(hpic_float * map);
  int hpic_float_order_get(hpic_float * map);
  int hpic_float_coord_get(hpic_float * map);
  int hpic_int_name_set(hpic_int * map, const char *name);
  int hpic_int_units_set(hpic_int * map, const char *units);
  char *hpic_int_name_get(hpic_int * map);
  char *hpic_int_units_get(hpic_int * map);
  size_t hpic_int_nside_get(hpic_int * map);
  size_t hpic_int_npix_get(hpic_int * map);
  int hpic_int_order_get(hpic_int * map);
  int hpic_int_coord_get(hpic_int * map);

  int hpic_set(hpic * map, size_t pix, double val);
  double hpic_get(hpic * map, size_t pix);
  int hpic_float_set(hpic_float * map, size_t pix, float val);
  float hpic_float_get(hpic_float * map, size_t pix);
  int hpic_int_set(hpic_int * map, size_t pix, int val);
  int hpic_int_get(hpic_int * map, size_t pix);
  int hpic_setall(hpic * map, double val);
  int hpic_float_setall(hpic_float * map, float val);
  int hpic_int_setall(hpic_int * map, int val);

  hpic *hpic_copy(hpic * map);
  hpic_float *hpic_float_copy(hpic_float * map);
  hpic_int *hpic_int_copy(hpic_int * map);

  int hpic_mem_set(hpic * map, int mem);
  int hpic_float_mem_set(hpic_float * map, int mem);
  int hpic_int_mem_set(hpic_int * map, int mem);
  int hpic_mem_get(hpic * map);
  int hpic_float_mem_get(hpic_float * map);
  int hpic_int_mem_get(hpic_int * map);

  int hpic_comp(hpic * map1, hpic * map2);
  int hpic_float_comp(hpic_float * map1, hpic_float * map2);
  int hpic_int_comp(hpic_int * map1, hpic_int * map2);

  int hpic_info_fprintf(FILE * fp, hpic * map);
  int hpic_float_info_fprintf(FILE * fp, hpic_float * map);
  int hpic_int_info_fprintf(FILE * fp, hpic_int * map);
  int hpic_info_printf(hpic * map);
  int hpic_float_info_printf(hpic_float * map);
  int hpic_int_info_printf(hpic_int * map);

/* map arrays */

  hpic_fltarr *hpic_fltarr_alloc(size_t nmaps);
  int hpic_fltarr_free(hpic_fltarr * array);
  size_t hpic_fltarr_n_get(hpic_fltarr * array);
  int hpic_fltarr_set(hpic_fltarr * array, size_t elem, hpic_float * map);
  hpic_float *hpic_fltarr_get(hpic_fltarr * array, size_t elem);

/* vector operations */

  hpic_vec *hpic_vec_alloc(size_t n);
  int hpic_vec_free(hpic_vec * vec);
  hpic_vec_float *hpic_vec_float_alloc(size_t n);
  int hpic_vec_float_free(hpic_vec_float * vec);
  hpic_vec_int *hpic_vec_int_alloc(size_t n);
  int hpic_vec_int_free(hpic_vec_int * vec);
  hpic_vec_index *hpic_vec_index_alloc(size_t n);
  int hpic_vec_index_free(hpic_vec_index * vec);

  size_t hpic_vec_n_get(hpic_vec * vec);
  size_t hpic_vec_float_n_get(hpic_vec_float * vec);
  size_t hpic_vec_int_n_get(hpic_vec_int * vec);
  size_t hpic_vec_index_n_get(hpic_vec_index * vec);

  int hpic_vec_set(hpic_vec * vec, size_t elem, double val);
  double hpic_vec_get(hpic_vec * vec, size_t elem);
  int hpic_vec_float_set(hpic_vec_float * vec, size_t elem, float val);
  float hpic_vec_float_get(hpic_vec_float * vec, size_t elem);
  int hpic_vec_int_set(hpic_vec_int * vec, size_t elem, int val);
  int hpic_vec_int_get(hpic_vec_int * vec, size_t elem);
  int hpic_vec_index_set(hpic_vec_index * vec, size_t elem, size_t val);
  size_t hpic_vec_index_get(hpic_vec_index * vec, size_t elem);

  int hpic_vec_setall(hpic_vec * vec, double val);
  int hpic_vec_float_setall(hpic_vec_float * vec, float val);
  int hpic_vec_int_setall(hpic_vec_int * vec, int val);
  int hpic_vec_index_setall(hpic_vec_index * vec, size_t val);

  hpic_vec *hpic_vec_copy(hpic_vec * vec);
  hpic_vec_float *hpic_vec_float_copy(hpic_vec_float * vec);
  hpic_vec_int *hpic_vec_int_copy(hpic_vec_int * vec);
  hpic_vec_index *hpic_vec_index_copy(hpic_vec_index * vec);

  int hpic_vec_resize(hpic_vec * vec, size_t newn);
  int hpic_vec_append(hpic_vec * vec, double val);
  int hpic_vec_float_resize(hpic_vec_float * vec, size_t newn);
  int hpic_vec_float_append(hpic_vec_float * vec, float val);
  int hpic_vec_int_resize(hpic_vec_int * vec, size_t newn);
  int hpic_vec_int_append(hpic_vec_int * vec, int val);
  int hpic_vec_index_resize(hpic_vec_index * vec, size_t newn);
  int hpic_vec_index_append(hpic_vec_index * vec, size_t val);

/* vector arrays */

  hpic_vec_fltarr *hpic_vec_fltarr_alloc(size_t nvecs);
  int hpic_vec_fltarr_free(hpic_vec_fltarr * array);
  size_t hpic_vec_fltarr_n_get(hpic_vec_fltarr * array);
  int hpic_vec_fltarr_set(hpic_vec_fltarr * array, size_t elem,
                           hpic_vec_float * vec);
  hpic_vec_float *hpic_vec_fltarr_get(hpic_vec_fltarr * array, size_t elem);

/* map math */

  int hpic_scale(hpic * map, double val);
  int hpic_float_scale(hpic_float * map, double val);
  int hpic_int_scale(hpic_int * map, double val);

  int hpic_offset(hpic * map, double val);
  int hpic_float_offset(hpic_float * map, float val);
  int hpic_int_offset(hpic_int * map, int val);

  int hpic_add(hpic * first, hpic * second, int mode);
  int hpic_float_add(hpic_float * first, hpic_float * second, int mode);
  int hpic_int_add(hpic_int * first, hpic_int * second, int mode);

  int hpic_subtract(hpic * first, hpic * second, int mode);
  int hpic_float_subtract(hpic_float * first, hpic_float * second, int mode);
  int hpic_int_subtract(hpic_int * first, hpic_int * second, int mode);

  int hpic_multiply(hpic * first, hpic * second, int mode);
  int hpic_float_multiply(hpic_float * first, hpic_float * second, int mode);
  int hpic_int_multiply(hpic_int * first, hpic_int * second, int mode);

  int hpic_divide(hpic * first, hpic * second, int mode);
  int hpic_float_divide(hpic_float * first, hpic_float * second, int mode);
  int hpic_int_divide(hpic_int * first, hpic_int * second, int mode);

/* projection operations */

  hpic_proj *hpic_proj_alloc(size_t nx, size_t ny);
  int hpic_proj_free(hpic_proj * proj);

  int hpic_proj_type_get(hpic_proj * proj);
  int hpic_proj_type_set(hpic_proj * proj, int type);

  int hpic_proj_range_get(hpic_proj * proj, double *mintheta,
                           double *maxtheta, double *minphi, double *maxphi);
  int hpic_proj_range_set(hpic_proj * proj, double mintheta, double maxtheta,
                           double minphi, double maxphi);

  int hpic_proj_set(hpic_proj * proj, size_t xelem, size_t yelem,
                     double val);
  double hpic_proj_get(hpic_proj * proj, size_t xelem, size_t yelem);

  int hpic_proj_setall(hpic_proj * proj, double val);

  int hpic_proj_info_fprintf(FILE * fp, hpic_proj * proj);
  int hpic_proj_info_printf(hpic_proj * proj);

  int hpic_proj_points(hpic_proj * proj, hpic_vec * theta, hpic_vec * phi,
                        hpic_vec_index * x, hpic_vec_index * y);

  int hpic_proj_pixels(hpic_proj * proj, size_t nside, int order,
                        hpic_vec_index * pixels, hpic_vec_index * x,
                        hpic_vec_index * y);

  int hpic_proj_map(hpic_proj * proj, hpic * map);

  int hpic_proj_vecfield(hpic_proj * proj, hpic * thetacomp, hpic * phicomp,
                          size_t pnside, double maxmag,
                          hpic_vec_index * headx, hpic_vec_index * heady,
                          hpic_vec_index * tailx, hpic_vec_index * taily);

/* conversion operations */

  hpic *hpic_float2double(hpic_float * map);
  hpic *hpic_int2double(hpic_int * map);
  hpic_float *hpic_double2float(hpic * map);
  hpic_float *hpic_int2float(hpic_int * map);
  hpic_int *hpic_double2int(hpic * map);
  hpic_int *hpic_float2int(hpic_float * map);

  int hpic_conv_nestcopy(hpic * map);
  int hpic_conv_float_nestcopy(hpic_float * map);
  int hpic_conv_int_nestcopy(hpic_int * map);

  int hpic_conv_ringcopy(hpic * map);
  int hpic_conv_float_ringcopy(hpic_float * map);
  int hpic_conv_int_ringcopy(hpic_int * map);

  int hpic_conv_nest(hpic * map);
  int hpic_conv_float_nest(hpic_float * map);
  int hpic_conv_int_nest(hpic_int * map);

  int hpic_conv_ring(hpic * map);
  int hpic_conv_float_ring(hpic_float * map);
  int hpic_conv_int_ring(hpic_int * map);

  hpic *hpic_conv_xgrade(hpic * map, size_t newnside);
  hpic_float *hpic_conv_float_xgrade(hpic_float * map, size_t newnside);
  hpic_int *hpic_conv_int_xgrade(hpic_int * map, size_t newnside);

/* transforms and filtering */


/* FITS keys operations */

  hpic_keys *hpic_keys_alloc();
  int hpic_keys_free(hpic_keys * keys);
  int hpic_keys_clear(hpic_keys * keys);
  int hpic_keys_sadd(hpic_keys * keys, char *keyname, 
                     char *keyval, char *keycom);
  int hpic_keys_iadd(hpic_keys * keys, char *keyname, 
                     int keyval, char *keycom);
  int hpic_keys_fadd(hpic_keys * keys, char *keyname, 
                     float keyval, char *keycom);
  int hpic_keys_del(hpic_keys * keys, char *keyname);
  int hpic_keys_total(hpic_keys * keys);
  int hpic_keys_sfind(hpic_keys * keys, char *keyname, 
                      char *keyval, char *keycom);
  int hpic_keys_ifind(hpic_keys * keys, char *keyname, 
                      int *keyval, char *keycom);
  int hpic_keys_ffind(hpic_keys * keys, char *keyname, 
                      float *keyval, char *keycom);
  int hpic_keys_read(hpic_keys * keys, fitsfile * fp, int *ret);
  int hpic_keys_write(hpic_keys * keys, fitsfile * fp, int *ret);
  int hpic_keys_fprintf(FILE * fp, hpic_keys * keys);
  int hpic_keys_printf(hpic_keys * keys);

/* Map FITS file operations */

  void fitserr(int ret, char *message);

  int hpic_fits_map_test(char *filename, size_t * nside, int *order,
                         int *coord, int *type, size_t * nmaps);
  int hpic_fits_map_info(char *filename, size_t * nside, int *order,
                         int *coord, int *type, size_t * nmaps, char *creator, char *extname, char **names, char **units, hpic_keys *keys);
  int hpic_fits_full_write(char *filename, char *creator, char *extname,
                           char *comment, hpic_fltarr * maps,
                           hpic_keys * keys);
  int hpic_fits_cut_write(char *filename, char *creator, char *extname,
                          char *comment, hpic_int * pixels, hpic_int * hits,
                          hpic_float * errs, hpic_fltarr * maps,
                          hpic_keys * keys);
  int hpic_fits_full_read(char *filename, char *creator, char *extname,
                          hpic_fltarr * maps, hpic_keys * keys);
  int hpic_fits_cut_read(char *filename, char *creator, char *extname,
                         hpic_int * pixels, hpic_int * hits,
                         hpic_float * errs, hpic_fltarr * maps,
                         hpic_keys * keys);
  hpic_float *hpic_fits_read_one(char *filename, size_t mapnum, 
                                 char *creator, hpic_keys * keys);

/* Vector FITS file operations */

  int hpic_fits_vec_test(char *filename, size_t * nvecs, size_t * length,
                         int *filetype, int *tabtype);
  int hpic_fits_vec_info(char *filename, size_t * nvecs, size_t * length,
                         int *filetype, int *tabtype, char *creator, char *extname, char **names, char **units, hpic_keys * keys);
  int hpic_fits_vec_write(char *filename, int tabtype, char *creator,
                          char *extname, char *comment,
                          hpic_vec_fltarr * vecs, char **vecnames,
                          char **vecunits, hpic_keys * keys);
  int hpic_fits_vecindx_write(char *filename, char *creator, char *extname,
                              char *comment, hpic_vec_int * indx,
                              hpic_vec_fltarr * vecs, char **vecnames,
                              char **vecunits, hpic_keys * keys);
  int hpic_fits_vec_read(char *filename, char *creator, char *extname,
                         hpic_vec_fltarr * vecs, char **vecnames,
                         char **vecunits, hpic_keys * keys);
  int hpic_fits_vecindx_read(char *filename, char *creator, char *extname,
                             hpic_vec_int * indx, hpic_vec_fltarr * vecs,
                             char **vecnames, char **vecunits,
                             hpic_keys * keys);

/* CMB specific functions */

  int hpic_cmb_proj_QU(hpic_proj * proj, hpic * Qmap, hpic * Umap,
                        size_t pnside, double maxmag, hpic_vec_index * headx,
                        hpic_vec_index * heady, hpic_vec_index * tailx,
                        hpic_vec_index * taily);

  int hpic_cmb_write_full(char *filename, hpic_float * data, char *comment,
                          char *creator, hpic_keys * keys);
  int hpic_cmb_write_fullTQU(char *filename, hpic_float * tdata,
                             hpic_float * qdata, hpic_float * udata,
                             char *comment, char *creator, hpic_keys * keys);
  int hpic_cmb_write_cut(char *filename, hpic_int * pix, hpic_float * data,
                         hpic_int * hits, hpic_float * errs, char *comment,
                         char *creator, hpic_keys * keys);
  int hpic_cmb_write_cutTQU(char *filename, hpic_int * pix,
                            hpic_float * tdata, hpic_float * qdata,
                            hpic_float * udata, hpic_int * hits,
                            hpic_float * errs, char *comment, char *creator,
                            hpic_keys * keys);
  int hpic_cmb_read_fullTQU(char *filename, hpic_float * tdata,
                            hpic_float * qdata, hpic_float * udata,
                            char *creator, hpic_keys * keys);
  int hpic_cmb_read_cut(char *filename, hpic_int * pix, hpic_float * data,
                        hpic_int * hits, hpic_float * errs, char *creator,
                        hpic_keys * keys);
  int hpic_cmb_read_cutTQU(char *filename, hpic_int * pix, hpic_float * tdata,
                           hpic_float * qdata, hpic_float * udata,
                           hpic_int * hits, hpic_float * errs, char *creator,
                           hpic_keys * keys);

/* compatability wrappers */
  
  /* LEGACY - these will be removed at some point */

  void compat_ang2pix_nest(const long nside, double theta, double phi,
                           long *ipix);
  void compat_ang2pix_ring(const long nside, double theta, double phi,
                           long *ipix);
  void compat_pix2ang_nest(long nside, long ipix, double *theta, double *phi);
  void compat_pix2ang_ring(long nside, long ipix, double *theta, double *phi);
  void compat_nest2ring(long nside, long ipnest, long *ipring);
  void compat_ring2nest(long nside, long ipring, long *ipnest);
  long compat_nside2npix(const long nside);

  int compat_read_healpix_map(char *infile, long *nside, char *coordsys,
                              char *ordering, float *map);
  int compat_write_healpix_map(float *hpic_signal, long nside, char *filename,
                               char nest, char *coordsys);
  long compat_get_fits_size(char *filename, long *nside, char *ordering);
  void compat_read_fits_cut4(char *filename, int *pixel, float *hpic_signal,
                             int *n_obs, float *serror, char *channel,
                             char *coordsys, int *nside, int *obs_npix,
                             char *ordering, char *target, int *shot,
                             char *units);
  int compat_write_fits_cut4(int *pixel, float *hpic_signal, int *n_obs,
                             float *serror, char *channel, char *coordsys,
                             int nside, int obs_npix, char *ordering,
                             char *target, int shot, char *units,
                             char *filename, char *creator, char *release);


#  ifdef __cplusplus
}
#  endif
#endif
