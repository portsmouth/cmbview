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
 * This code deals with FITS I/O of maps                                     *
 *****************************************************************************/

#include <hpic.h>

/* error reporting */

void fitserr(int ret, char *message)
{
  if (ret) {
    fits_report_error(stderr, ret);
    HPIC_ERROR_VOID(HPIC_ERR_FITS, message);
  }
  return;
}

/* file tests */

int hpic_fits_map_test(char *filename, size_t * nside, int *order, int *coord, int *type, size_t * nmaps)
{

  int i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int obs;
  int ttype;
  int inside;
  long nrows;
  long pcount;
  int tfields;
  int grain;
  char pixtype[HPIC_STRNL];
  char coordstr[HPIC_STRNL];
  char orderstr[HPIC_STRNL];
  char indxstr[HPIC_STRNL];
  char comment[HPIC_STRNL];
  char extname[HPIC_STRNL];
  float nullval = HPIC_NULL;
  int nnull;
  float testval;
  long keynpix;
  long keyfirst;
  long keylast;
  int ischunk = 0;
  int lastcol;

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    return 0;
  }

  /* make sure extension is a binary table */
  if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (ttype != BINARY_TBL) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* determine the number of maps */
  if (fits_read_btblhdr
      (fp, HPIC_FITS_MAXCOL, &nrows, &tfields, NULL, NULL, NULL, extname, &pcount, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* make sure this is a HEALPIX file */
  if (fits_read_key(fp, TSTRING, "PIXTYPE", pixtype, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(pixtype, "HEALPIX", HPIC_STRNL) != 0) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* check required keywords (NSIDE, ORDERING, COORDSYS) */
  if (fits_read_key(fp, TINT, "NSIDE", &inside, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  (*nside) = (size_t) inside;
  if (fits_read_key(fp, TSTRING, "ORDERING", orderstr, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(orderstr, "RING", HPIC_STRNL) == 0) {
    (*order) = HPIC_RING;
  } else if (strncmp(orderstr, "NESTED", HPIC_STRNL) == 0) {
    (*order) = HPIC_NEST;
  } else {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (fits_read_key(fp, TSTRING, "COORDSYS", coordstr, comment, &ret)) {
    ret = 0;
    (*coord) = HPIC_COORD_C;
  } else {
    if (strncmp(coordstr, "C", HPIC_STRNL) == 0) {
      (*coord) = HPIC_COORD_C;
    } else if (strncmp(coordstr, "G", HPIC_STRNL) == 0) {
      (*coord) = HPIC_COORD_G;
    } else if (strncmp(coordstr, "E", HPIC_STRNL) == 0) {
      (*coord) = HPIC_COORD_E;
    } else {
      (*coord) = HPIC_COORD_O;
    }
  }

  /* check for INDXSCHM and GRAIN.  if not found, assume implicit */
  
  strcpy(indxstr,"");
  if (fits_read_key(fp, TSTRING, "OBJECT", indxstr, comment, &ret)) {
    ret = 0;
    if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
      ret = 0;
      (*type) = HPIC_FITS_FULL;
    } else {
      if (strncmp(indxstr, "EXPLICIT", HPIC_STRNL) == 0) {
        (*type) = HPIC_FITS_CUT;
      } else {
        (*type) = HPIC_FITS_FULL;
      }
    }
    if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
      ret = 0;
      grain = 0;
    }
    if (((grain == 0) && ((*type) == HPIC_FITS_CUT)) ||
          ((grain != 0) && ((*type) == HPIC_FITS_FULL))) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {   
    if (strncmp(indxstr, "PARTIAL", HPIC_STRNL) == 0) {
      (*type) = HPIC_FITS_CUT;
    } else {
      if (strncmp(indxstr, "FULLSKY", HPIC_STRNL) == 0) {
        (*type) = HPIC_FITS_FULL;
      } else {
        if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
          ret = 0;
          (*type) = HPIC_FITS_FULL;
        } else {
          if (strncmp(indxstr, "EXPLICIT", HPIC_STRNL) == 0) {
            (*type) = HPIC_FITS_CUT;
          } else {
            (*type) = HPIC_FITS_FULL;
          }
        }
        if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
          ret = 0;
          grain = 0;
        }
        if (((grain == 0) && ((*type) == HPIC_FITS_CUT)) ||
              ((grain != 0) && ((*type) == HPIC_FITS_FULL))) {
          ret = 0;
          fits_close_file(fp, &ret);
          return 0;
        } 
      } 
    }
  }
        
  /* check for chunk file and truncation */
  
  if ((*type) == HPIC_FITS_FULL) {
    (*nmaps) = tfields;
    if ((nrows != (long)(12*inside*inside))&&(1024*nrows != (long)(12*inside*inside))) {
      /*is this a chunk file?*/
      if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
        /*must at least have FIRSTPIX key*/
        ret = 0;
        ischunk = 0;
      } else {
        if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
          ret = 0;
          /*might be using LASTPIX instead*/
          if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
            ret = 0;
            ischunk = 0;
          } else {
            keynpix = keynpix - keyfirst + 1;
            if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
              ischunk = 0;
            } else {
              ischunk = 1;
            }
          }
        } else {
          if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
            ischunk = 0;
          } else {
            ischunk = 1;
          }
        }
      }
    } else {
      /*header doesn't matter, since we have entire map*/
      if (nrows == (long)(12*inside*inside)) {
        lastcol = 1;
      } else {
        lastcol = 1024;
      }
    }
    if (ischunk) {
      if (nrows == keynpix) {
        lastcol = 1;
      } else {
        lastcol = keynpix % 1024;
        if (lastcol == 0) {
          lastcol = 1024;
        }
      }
    }
    if (fits_read_col(fp, TFLOAT, 1, nrows, lastcol, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    (*nmaps) = tfields - 3;
    if (fits_read_col(fp, TFLOAT, 2, nrows, 1, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    } 
  }
    
  fits_close_file(fp, &ret);
  return 1;
}

int hpic_fits_map_info(char *filename, size_t * nside, int *order,
                       int *coord, int *type, size_t * nmaps, char *creator, char *extname, char **names, char **units, hpic_keys *keys)
{
  int i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int obs;
  int ttype;
  int inside;
  long nrows;
  long pcount;
  int tfields;
  int grain;
  char pixtype[HPIC_STRNL];
  char coordstr[HPIC_STRNL];
  char orderstr[HPIC_STRNL];
  char indxstr[HPIC_STRNL];
  char comment[HPIC_STRNL];
  float nullval = HPIC_NULL;
  int nnull;
  float testval;
  long keynpix;
  long keyfirst;
  long keylast;
  int ischunk = 0;
  int lastcol;

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    return 0;
  }
  
  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* make sure extension is a binary table */
  if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (ttype != BINARY_TBL) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* determine the number of maps */
  if (fits_read_btblhdr
      (fp, HPIC_FITS_MAXCOL, &nrows, &tfields, names, NULL, units, extname, &pcount, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  
  /* make sure this is a HEALPIX file */
  if (fits_read_key(fp, TSTRING, "PIXTYPE", pixtype, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(pixtype, "HEALPIX", HPIC_STRNL) != 0) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* check required keywords (NSIDE, ORDERING, COORDSYS) */
  if (fits_read_key(fp, TINT, "NSIDE", &inside, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  (*nside) = (size_t) inside;
  if (fits_read_key(fp, TSTRING, "ORDERING", orderstr, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(orderstr, "RING", HPIC_STRNL) == 0) {
    (*order) = HPIC_RING;
  } else if (strncmp(orderstr, "NESTED", HPIC_STRNL) == 0) {
    (*order) = HPIC_NEST;
  } else {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (fits_read_key(fp, TSTRING, "COORDSYS", coordstr, comment, &ret)) {
    ret = 0;
    (*coord) = HPIC_COORD_C;
  } else {
    if (strncmp(coordstr, "C", HPIC_STRNL) == 0) {
      (*coord) = HPIC_COORD_C;
    } else if (strncmp(coordstr, "G", HPIC_STRNL) == 0) {
      (*coord) = HPIC_COORD_G;
    } else if (strncmp(coordstr, "E", HPIC_STRNL) == 0) {
      (*coord) = HPIC_COORD_E;
    } else {
      (*coord) = HPIC_COORD_O;
    }
  }

  /* check for INDXSCHM and GRAIN.  if not found, assume implicit */
  
  strcpy(indxstr,"");
  if (fits_read_key(fp, TSTRING, "OBJECT", indxstr, comment, &ret)) {
    ret = 0;
    if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
      ret = 0;
      (*type) = HPIC_FITS_FULL;
    } else {
      if (strncmp(indxstr, "EXPLICIT", HPIC_STRNL) == 0) {
        (*type) = HPIC_FITS_CUT;
      } else {
        (*type) = HPIC_FITS_FULL;
      }
    }
    if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
      ret = 0;
      grain = 0;
    }
    if (((grain == 0) && ((*type) == HPIC_FITS_CUT)) ||
      ((grain != 0) && ((*type) == HPIC_FITS_FULL))) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {   
    if (strncmp(indxstr, "PARTIAL", HPIC_STRNL) == 0) {
      (*type) = HPIC_FITS_CUT;
    } else {
      if (strncmp(indxstr, "FULLSKY", HPIC_STRNL) == 0) {
        (*type) = HPIC_FITS_FULL;
      } else {
        if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
          ret = 0;
          (*type) = HPIC_FITS_FULL;
        } else {
          if (strncmp(indxstr, "EXPLICIT", HPIC_STRNL) == 0) {
            (*type) = HPIC_FITS_CUT;
          } else {
            (*type) = HPIC_FITS_FULL;
          }
        }
        if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
          ret = 0;
          grain = 0;
        }
        if (((grain == 0) && ((*type) == HPIC_FITS_CUT)) ||
           ((grain != 0) && ((*type) == HPIC_FITS_FULL))) {
          ret = 0;
          fits_close_file(fp, &ret);
          return 0;
        } 
      } 
    }
  }
  
  /* read extension keys */
  
  hpic_keys_read(keys, fp, &ret);
  
  /* check for chunk file and truncation */
  
  if ((*type) == HPIC_FITS_FULL) {
    (*nmaps) = tfields;
    if ((nrows != (long)(12*inside*inside))&&(1024*nrows != (long)(12*inside*inside))) {
      /*is this a chunk file?*/
      if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
        /*must at least have FIRSTPIX key*/
        ret = 0;
        ischunk = 0;
      } else {
        if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
          ret = 0;
          /*might be using LASTPIX instead*/
          if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
            ret = 0;
            ischunk = 0;
          } else {
            keynpix = keynpix - keyfirst + 1;
            if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
              ischunk = 0;
            } else {
              ischunk = 1;
            }
          }
        } else {
          if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
            ischunk = 0;
          } else {
            ischunk = 1;
          }
        }
      }
    } else {
      /*header doesn't matter, since we have entire map*/
      if (nrows == (long)(12*inside*inside)) {
        lastcol = 1;
      } else {
        lastcol = 1024;
      }
    }
    if (ischunk) {
      if (nrows == keynpix) {
        lastcol = 1;
      } else {
        lastcol = keynpix % 1024;
        if (lastcol == 0) {
          lastcol = 1024;
        }
      }
    }
    if (fits_read_col(fp, TFLOAT, 1, nrows, lastcol, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    (*nmaps) = tfields - 3;
    if (fits_read_col(fp, TFLOAT, 2, nrows, 1, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    } 
  }
  
  fits_close_file(fp, &ret);
  return 1;
}

/* FITS map writing */

int hpic_fits_full_write(char *filename, char *creator, char *extname,
                         char *comment, hpic_fltarr * maps, hpic_keys * keys)
{

  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int bitpix = SHORT_IMG;
  int nax = 0;
  long axes[] = { 0, 0 };
  int total;
  int type;
  long rows;
  long frow = 1;
  long fsamp = 1;
  int nside;
  long npix;
  int order;
  int coord;
  int grain = 0;
  char **colnames;
  char **coltypes;
  char **colunits;
  int wide = 0;
  hpic_vec_float *datavec;
  size_t nmaps = hpic_fltarr_n_get(maps);
  hpic_float *tempmap;
  char xkey[HPIC_STRNL];

  if (nmaps == 0) {
    HPIC_ERROR(HPIC_ERR_FITS, "must specify more than zero maps!");
  }
  for (i = 0; i < nmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    if (!tempmap) {
      HPIC_ERROR(HPIC_ERR_ACCESS, "input map is not allocated");
    }
  }
  colnames = hpic_strarr_alloc(nmaps);
  coltypes = hpic_strarr_alloc(nmaps);
  colunits = hpic_strarr_alloc(nmaps);

  tempmap = hpic_fltarr_get(maps, 0);
  nside = (int)hpic_float_nside_get(tempmap);
  order = hpic_float_order_get(tempmap);
  coord = hpic_float_coord_get(tempmap);
  npix = (long)hpic_float_npix_get(tempmap);
  for (i = 1; i < nmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    if (nside != (int)hpic_float_nside_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside");
    }
    if (order != hpic_float_order_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering");
    }
    if (coord != hpic_float_coord_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system");
    }
  }

  if (nside > 8) {
    wide = 1;
    rows = (long)(npix / 1024);
  } else {
    wide = 0;
    rows = npix;
  }

  /* setup column parameters */

  for (i = 0; i < nmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    strncpy(colnames[i], hpic_float_name_get(tempmap), HPIC_STRNL);
    strncpy(colunits[i], hpic_float_units_get(tempmap), HPIC_STRNL);
    if (wide) {
      strncpy(coltypes[i], "1024E", HPIC_STRNL);
    } else {
      strncpy(coltypes[i], "1E", HPIC_STRNL);
    }
  }

  /* create file */
  if (fits_create_file(&fp, filename, &ret)) {
    fitserr(ret, "hpic_fits_full_write:  creating file");
  }

  /* create empty primary image */
  if (fits_create_img(fp, bitpix, nax, axes, &ret)) {
    fitserr(ret, "hpic_fits_full_write:  creating primary image");
  }

  /* write header */
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing header comment 1");
  }
  if (fits_write_comment(fp, comment, &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing header comment 2");
  }
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing header comment 3");
  }
  if (fits_write_date(fp, &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing date");
  }
  if (fits_write_key
      (fp, TSTRING, "CREATOR", creator, "Software creating the map", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing creator");
  }

  /* write extension */
  if (fits_create_tbl
      (fp, BINARY_TBL, rows, (int)nmaps, colnames, coltypes, colunits,
       extname, &ret)) {
    fitserr(ret, "hpic_fits_full_write:  creating binary table");
  }
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_full_write:  moving to extension");
  }

  /* write mandatory keywords */
  if (fits_write_comment(fp, "     ", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 1");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 2");
  }
  if (fits_write_comment
      (fp, "         Pixelization Specific Keywords    ", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 3");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 4");
  }
  if (fits_write_key
      (fp, TSTRING, "PIXTYPE", "HEALPIX", "HEALPIX Pixelization", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing pixtype keyword");
  }
  if (order == HPIC_RING) {
    if (fits_write_key
        (fp, TSTRING, "ORDERING", "RING",
         "Pixel ordering scheme, either RING or NESTED", &ret)) {
      fitserr(ret, "hpic_fits_full_write:  writing ordering = ring");
    }
  } else {
    if (fits_write_key
        (fp, TSTRING, "ORDERING", "NESTED",
         "Pixel ordering scheme, either RING or NESTED", &ret)) {
      fitserr(ret, "hpic_fits_full_write:  writing ordering = nested");
    }
  }
  if (fits_write_key
      (fp, TINT, "NSIDE", &nside, "Resolution parameter for HEALPIX", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing nside");
  }
  switch (coord) {
  case HPIC_COORD_G:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "G", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_full_write:  writing coord = G");
    }
    break;
  case HPIC_COORD_C:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "C", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_full_write:  writing coord = C");
    }
    break;
  case HPIC_COORD_E:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "E", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_full_write:  writing coord = E");
    }
    break;
  default:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "O", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_full_write:  writing coord = O");
    }
    break;
  }
  if (fits_write_comment
      (fp,
       "    G = Galactic, E = ecliptic, C = celestial = equatorial, O = other",
       &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 5");
  }
  if (fits_write_comment(fp, "      ", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 6");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 7");
  }
  if (fits_write_comment(fp, "            Data Specific Keywords    ", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext comment 8");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing ext commment 9");
  }
  if (fits_write_key
      (fp, TSTRING, "INDXSCHM", "IMPLICIT", "Indexing : IMPLICIT or EXPLICIT",
       &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing indxschm");
  }
  if (fits_write_key
      (fp, TINT, "GRAIN", &grain, "Grain of pixel indexing", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing grain");
  }
  if (fits_write_comment
      (fp,
       "GRAIN=0 : no indexing of pixel data                         (IMPLICIT)",
       &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing comment 11");
  }
  if (fits_write_comment
      (fp,
       "GRAIN=1 : 1 pixel index -> 1 pixel data                     (EXPLICIT)",
       &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing comment 12");
  }
  if (fits_write_comment
      (fp,
       "GRAIN>1 : 1 pixel index -> data of GRAIN consecutive pixels (EXPLICIT)",
       &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing comment 13");
  }
  if (fits_write_comment(fp, "      ", &ret)) {
    fitserr(ret, "hpic_fits_full_write:  writing comment 14");
  }

  /* write optional keywords */

  /*delete extraneous keys that may cause file
  detection errors */
  strcpy(xkey, "NPIX");
  hpic_keys_del(keys, xkey);
  strcpy(xkey, "FIRSTPIX");
  hpic_keys_del(keys, xkey);
  strcpy(xkey, "LASTPIX");
  hpic_keys_del(keys, xkey);
  
  hpic_keys_write(keys, fp, &ret);

  /* write the data and clean up */

  datavec = hpic_vec_float_alloc((size_t) npix);
  for (i = 0; i < nmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    for (j = 0; j < (size_t) npix; j++) {
      hpic_vec_float_set(datavec, j, hpic_float_get(tempmap, j));
    }
    if (fits_write_col
        (fp, TFLOAT, (int)(i + 1), frow, fsamp, npix, datavec->data, &ret)) {
      fitserr(ret, "hpic_fits_full_write:  writing column");
    }
  }
  
  hpic_vec_float_free(datavec);
  hpic_strarr_free(colnames, nmaps);
  hpic_strarr_free(coltypes, nmaps);
  hpic_strarr_free(colunits, nmaps);
  
  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_full_write:  closing file");
  }

  return ret;
}

int hpic_fits_cut_write(char *filename, char *creator, char *extname,
                        char *comment, hpic_int * pixels, hpic_int * hits,
                        hpic_float * errs, hpic_fltarr * maps,
                        hpic_keys * keys)
{

  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int bitpix = SHORT_IMG;
  int nax = 0;
  long axes[] = { 0, 0 };
  int total;
  int type;
  long rows;
  long frow = 1;
  long fsamp = 1;
  int nside;
  long npix;
  int order;
  int coord;
  int grain = 1;
  char **colnames;
  char **coltypes;
  char **colunits;
  hpic_vec_float *datavec;
  hpic_vec_int *pixvec;
  hpic_vec_int *hitvec;
  hpic_vec_float *errvec;
  size_t nmaps = hpic_fltarr_n_get(maps);
  hpic_float *tempmap;
  char xkey[HPIC_STRNL];

  if (nmaps == 0) {
    HPIC_ERROR(HPIC_ERR_FITS, "must specify more than zero maps!");
  }
  for (i = 0; i < nmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    if (!tempmap) {
      HPIC_ERROR(HPIC_ERR_ACCESS, "input map is not allocated");
    }
  }
  if (!pixels) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "pixel map is not allocated");
  }
  if (!hits) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "hits map is not allocated");
  }
  if (!errs) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "error map is not allocated");
  }
  colnames = hpic_strarr_alloc(nmaps + 3);
  coltypes = hpic_strarr_alloc(nmaps + 3);
  colunits = hpic_strarr_alloc(nmaps + 3);

  nside = (int)hpic_int_nside_get(pixels);
  order = hpic_int_order_get(pixels);
  coord = hpic_int_coord_get(pixels);
  npix = (long)hpic_int_npix_get(pixels);
  for (i = 0; i < nmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    if (nside != (int)hpic_float_nside_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside");
    }
    if (order != hpic_float_order_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering");
    }
    if (coord != hpic_float_coord_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system");
    }
  }
  if (nside != (int)hpic_float_nside_get(errs)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside");
  }
  if (order != hpic_float_order_get(errs)) {
    HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering");
  }
  if (coord != hpic_float_coord_get(errs)) {
    HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system");
  }
  if (nside != (int)hpic_int_nside_get(hits)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside");
  }
  if (order != hpic_int_order_get(hits)) {
    HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering");
  }
  if (coord != hpic_int_coord_get(hits)) {
    HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system");
  }

  /* find number of rows */

  rows = 0;
  for (i = 0; i < hpic_int_npix_get(pixels); i++) {
    if (!hpic_is_inull(hpic_int_get(pixels, i))) {
      rows++;
    }
  }

  /* setup column parameters */

  strncpy(colnames[0], hpic_int_name_get(pixels), HPIC_STRNL);
  strncpy(colunits[0], hpic_int_units_get(pixels), HPIC_STRNL);
  strncpy(coltypes[0], "1J", HPIC_STRNL);
  for (i = 0; i < nmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    strncpy(colnames[i + 1], hpic_float_name_get(tempmap), HPIC_STRNL);
    strncpy(colunits[i + 1], hpic_float_units_get(tempmap), HPIC_STRNL);
    strncpy(coltypes[i + 1], "1E", HPIC_STRNL);
  }
  strncpy(colnames[nmaps + 1], hpic_int_name_get(hits), HPIC_STRNL);
  strncpy(colunits[nmaps + 1], hpic_int_units_get(hits), HPIC_STRNL);
  strncpy(coltypes[nmaps + 1], "1J", HPIC_STRNL);
  strncpy(colnames[nmaps + 2], hpic_float_name_get(errs), HPIC_STRNL);
  strncpy(colunits[nmaps + 2], hpic_float_units_get(errs), HPIC_STRNL);
  strncpy(coltypes[nmaps + 2], "1E", HPIC_STRNL);

  /* create file */
  if (fits_create_file(&fp, filename, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  creating file");
  }

  /* create empty primary image */
  if (fits_create_img(fp, bitpix, nax, axes, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  creating primary image");
  }

  /* write header */
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing header comment 1");
  }
  if (fits_write_comment(fp, comment, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing header comment 2");
  }
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing header comment 3");
  }
  if (fits_write_date(fp, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing date");
  }
  if (fits_write_key
      (fp, TSTRING, "CREATOR", creator, "Software creating the map", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing creator");
  }

  /* write extension */
  if (fits_create_tbl
      (fp, BINARY_TBL, rows, (int)(nmaps + 3), colnames, coltypes, colunits,
       extname, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  creating binary table");
  }
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  moving to extension");
  }

  /* write mandatory keywords */
  if (fits_write_comment(fp, "     ", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 1");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 2");
  }
  if (fits_write_comment
      (fp, "         Pixelization Specific Keywords    ", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 3");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 4");
  }
  if (fits_write_key
      (fp, TSTRING, "PIXTYPE", "HEALPIX", "HEALPIX Pixelization", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing pixtype keyword");
  }
  if (order == HPIC_RING) {
    if (fits_write_key
        (fp, TSTRING, "ORDERING", "RING",
         "Pixel ordering scheme, either RING or NESTED", &ret)) {
      fitserr(ret, "hpic_fits_cut_write:  writing ordering = ring");
    }
  } else {
    if (fits_write_key
        (fp, TSTRING, "ORDERING", "NESTED",
         "Pixel ordering scheme, either RING or NESTED", &ret)) {
      fitserr(ret, "hpic_fits_cut_write:  writing ordering = nested");
    }
  }
  if (fits_write_key
      (fp, TINT, "NSIDE", &nside, "Resolution parameter for HEALPIX", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing nside");
  }
  switch (coord) {
  case HPIC_COORD_G:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "G", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_cut_write:  writing coord = G");
    }
    break;
  case HPIC_COORD_C:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "C", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_cut_write:  writing coord = C");
    }
    break;
  case HPIC_COORD_E:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "E", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_cut_write:  writing coord = E");
    }
    break;
  default:
    if (fits_write_key
        (fp, TSTRING, "COORDSYS", "O", "Pixelization coordinate system",
         &ret)) {
      fitserr(ret, "hpic_fits_cut_write:  writing coord = O");
    }
    break;
  }
  if (fits_write_comment
      (fp,
       "    G = Galactic, E = ecliptic, C = celestial = equatorial, O = other",
       &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 5");
  }
  if (fits_write_comment(fp, "      ", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 6");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 7");
  }
  if (fits_write_comment(fp, "            Data Specific Keywords    ", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext comment 8");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing ext commment 9");
  }
  if (fits_write_key
      (fp, TSTRING, "INDXSCHM", "EXPLICIT", "Indexing : IMPLICIT or EXPLICIT",
       &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing indxschm");
  }
  if (fits_write_key
      (fp, TINT, "GRAIN", &grain, "Grain of pixel indexing", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing grain");
  }
  if (fits_write_comment
      (fp,
       "GRAIN=0 : no indexing of pixel data                         (IMPLICIT)",
       &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing comment 11");
  }
  if (fits_write_comment
      (fp,
       "GRAIN=1 : 1 pixel index -> 1 pixel data                     (EXPLICIT)",
       &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing comment 12");
  }
  if (fits_write_comment
      (fp,
       "GRAIN>1 : 1 pixel index -> data of GRAIN consecutive pixels (EXPLICIT)",
       &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing comment 13");
  }
  if (fits_write_comment(fp, "      ", &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing comment 14");
  }

  /* write optional keywords */

  /*delete extraneous keys that may cause file
  detection errors */
  strcpy(xkey, "NPIX");
  hpic_keys_del(keys, xkey);
  strcpy(xkey, "FIRSTPIX");
  hpic_keys_del(keys, xkey);
  strcpy(xkey, "LASTPIX");
  hpic_keys_del(keys, xkey);
      
  hpic_keys_write(keys, fp, &ret);

  /* write the data and clean up */
  pixvec = hpic_vec_int_alloc((size_t) rows);
  datavec = hpic_vec_float_alloc((size_t) rows);
  hitvec = hpic_vec_int_alloc((size_t) rows);
  errvec = hpic_vec_float_alloc((size_t) rows);
  j = 0;
  for (i = 0; i < (size_t) npix; i++) {
    if (!hpic_is_inull(hpic_int_get(pixels, i))) {
      hpic_vec_int_set(pixvec, j, hpic_int_get(pixels, i));
      hpic_vec_int_set(hitvec, j, hpic_int_get(hits, i));
      hpic_vec_float_set(errvec, j, hpic_float_get(errs, i));
      j++;
    }
  }
  if (fits_write_col(fp, TINT, 1, frow, fsamp, rows, pixvec->data, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing column");
  }
  for (i = 0; i < nmaps; i++) {
    k = 0;
    tempmap = hpic_fltarr_get(maps, i);
    for (j = 0; j < (size_t) npix; j++) {
      if (!hpic_is_inull(hpic_int_get(pixels, j))) {
        hpic_vec_float_set(datavec, k, hpic_float_get(tempmap, j));
        k++;
      }
    }
    if (fits_write_col
        (fp, TFLOAT, (int)(i + 2), frow, fsamp, rows, datavec->data, &ret)) {
      fitserr(ret, "hpic_fits_cut_write:  writing column");
    }
  }
  if (fits_write_col
      (fp, TINT, (int)(nmaps + 2), frow, fsamp, rows, hitvec->data, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing column");
  }
  if (fits_write_col
      (fp, TFLOAT, (int)(nmaps + 3), frow, fsamp, rows, errvec->data, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  writing column");
  }
  
  hpic_vec_float_free(datavec);
  hpic_vec_float_free(errvec);
  hpic_vec_int_free(pixvec);
  hpic_vec_int_free(hitvec);
  hpic_strarr_free(colnames, nmaps + 3);
  hpic_strarr_free(coltypes, nmaps + 3);
  hpic_strarr_free(colunits, nmaps + 3);
  
  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_cut_write:  closing file");
  }

  return ret;
}

/* FITS map reading */

int hpic_fits_full_read(char *filename, char *creator, char *extname,
                        hpic_fltarr * maps, hpic_keys * keys)
{

  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int type;
  int order;
  int coord;
  size_t nside;
  int ttype;
  size_t tmaps;
  char name[HPIC_STRNL];
  char units[HPIC_STRNL];
  char comment[HPIC_STRNL];
  float nullval;
  int nnull;
  int tfields;
  long nrows;
  long nelem;
  long pcount = 0;
  char **colnames;
  char **coltypes;
  char **colunits;
  hpic_vec_float *datavec;
  size_t nmaps = hpic_fltarr_n_get(maps);
  hpic_float *tempmap;
  int keynpix;
  int keyfirst;
  int ischunk;

  /* test file to make sure its a full-sphere type */
  if (!hpic_fits_map_test(filename, &nside, &order, &coord, &ttype, &tmaps)) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a healpix format file!");
  } else {
    if (ttype != HPIC_FITS_FULL) {
      HPIC_ERROR(HPIC_ERR_FITS, "file is not a full-sphere format file");
    }
  }
  if (nmaps < tmaps) {
    HPIC_ERROR(HPIC_ERR_FITS, "map array does not have enough room for data in file");
  }
  for (i = 0; i < tmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    if (nside != (size_t)hpic_float_nside_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside as the FITS file");
    }
    if (order != hpic_float_order_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering as the FITS file");
    }
    if (coord != hpic_float_coord_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system as the FITS file");
    }
  }

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    fitserr(ret, "hpic_fits_full_read:  opening file");
  }

  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* read primary image keywords */
  hpic_keys_clear(keys);
  hpic_keys_read(keys, fp, &ret);

  /* read extension info */
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_full_read:  moving to extension");
  }
  colnames = hpic_strarr_alloc(tmaps);
  coltypes = hpic_strarr_alloc(tmaps);
  colunits = hpic_strarr_alloc(tmaps);

  if (fits_read_btblhdr
      (fp, (int)tmaps, &nrows, &tfields, colnames, coltypes, colunits,
       extname, &pcount, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  
  /* read extension keywords */

  hpic_keys_read(keys, fp, &ret);
  
  /* is this a chunk? */
  
  if ((nrows != (long)(12*nside*nside))&&(1024*nrows != (long)(12*nside*nside))) {
    /*this must be a chunk file*/
    if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
      /*must at least have FIRSTPIX key*/
      fits_close_file(fp, &ret);
      return 0;
    } else {
      if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
        ret = 0;
        /*might be using LASTPIX instead*/
        if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
          fits_close_file(fp, &ret);
          return 0;
        } else {
          keynpix = keynpix - keyfirst + 1;
          ischunk = 1;
        }
      } else {
        ischunk = 1;
      }
    }
  } else {
    ischunk = 0;
  }
  
  /* read the data */

  nullval = HPIC_NULL;
  nnull = 0;
  if (ischunk) {
    datavec = hpic_vec_float_alloc((size_t)keynpix);
    nelem = (long)keynpix;
  } else {
    datavec = hpic_vec_float_alloc(12*nside*nside);
    nelem = (long)(12*nside*nside);
  }
  for (i = 0; i < tmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    hpic_float_name_set(tempmap, colnames[i]);
    hpic_float_units_set(tempmap, colunits[i]);
    if (fits_read_col
        (fp, TFLOAT, (int)(i + 1), 1, 1, nelem, &nullval, datavec->data,
         &nnull, &ret)) {
      fitserr(ret, "hpic_fits_full_read:  reading data");
    }
    hpic_float_setall(tempmap, HPIC_NULL);
    if (ischunk) {
      for (j = 0; j < (size_t)nelem; j++) {
        hpic_float_set(tempmap, j+(size_t)keyfirst, hpic_vec_float_get(datavec, j));
      }
    } else {
      for (j = 0; j < (size_t)nelem; j++) {
        hpic_float_set(tempmap, j, hpic_vec_float_get(datavec, j));
      }
    }
  }
  
  hpic_vec_float_free(datavec);
  hpic_strarr_free(colnames, tmaps);
  hpic_strarr_free(coltypes, tmaps);
  hpic_strarr_free(colunits, tmaps);

  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_full_read:  closing file");
  }

  return ret;
}

int hpic_fits_cut_read(char *filename, char *creator, char *extname,
                       hpic_int * pixels, hpic_int * hits, hpic_float * errs,
                       hpic_fltarr * maps, hpic_keys * keys)
{

  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int type;
  int order;
  int coord;
  size_t nside;
  int ttype;
  size_t tmaps;
  char name[HPIC_STRNL];
  char units[HPIC_STRNL];
  float nullval;
  int nnull;
  int tfields;
  long nrows;
  long pcount = 0;
  char **colnames;
  char **coltypes;
  char **colunits;
  char comment[HPIC_STRNL];
  hpic_vec_int *pixvec;
  hpic_vec_float *datavec;
  hpic_vec_int *hitsvec;
  hpic_vec_float *errsvec;
  size_t nmaps = hpic_fltarr_n_get(maps);
  hpic_float *tempmap;

  /* test file to make sure its a cut-sphere type */
  if (!hpic_fits_map_test(filename, &nside, &order, &coord, &ttype, &tmaps)) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a healpix format file!");
  } else {
    if (ttype != HPIC_FITS_CUT) {
      HPIC_ERROR(HPIC_ERR_FITS, "file is not a cut-sphere format file");
    }
  }
  if (nmaps < tmaps) {
    HPIC_ERROR(HPIC_ERR_FITS, "map array does not have enough room for data in file");
  }
  for (i = 0; i < tmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    if (nside != (size_t)hpic_float_nside_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside as the FITS file");
    }
    if (order != hpic_float_order_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering as the FITS file");
    }
    if (coord != hpic_float_coord_get(tempmap)) {
      HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system as the FITS file");
    }
  }
  if (nside != (size_t)hpic_float_nside_get(errs)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside");
  }
  if (order != hpic_float_order_get(errs)) {
    HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering");
  }
  if (coord != hpic_float_coord_get(errs)) {
    HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system");
  }
  if (nside != (size_t)hpic_int_nside_get(hits)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside");
  }
  if (order != hpic_int_order_get(hits)) {
    HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering");
  }
  if (coord != hpic_int_coord_get(hits)) {
    HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system");
  }
  if (nside != (size_t)hpic_int_nside_get(pixels)) {
    HPIC_ERROR(HPIC_ERR_NSIDE, "all maps must have the same nside");
  }
  if (order != hpic_int_order_get(pixels)) {
    HPIC_ERROR(HPIC_ERR_ORDER, "all maps must have the same ordering");
  }
  if (coord != hpic_int_coord_get(pixels)) {
    HPIC_ERROR(HPIC_ERR_COORD, "all maps must have the same coordinate system");
  }
  
  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    fitserr(ret, "hpic_fits_cut_read:  opening file");
  }

  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* read primary image keywords */
  hpic_keys_clear(keys);
  hpic_keys_read(keys, fp, &ret);

  /* read extension info */
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_cut_read:  moving to extension");
  }
  colnames = hpic_strarr_alloc(tmaps + 3);
  coltypes = hpic_strarr_alloc(tmaps + 3);
  colunits = hpic_strarr_alloc(tmaps + 3);

  if (fits_read_btblhdr
      (fp, (int)(tmaps + 3), &nrows, &tfields, colnames, coltypes, colunits,
       extname, &pcount, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* read extension keywords */
  hpic_keys_read(keys, fp, &ret);

  /* read the data */

  nullval = HPIC_NULL;
  nnull = 0;
  datavec = hpic_vec_float_alloc((size_t) nrows);
  errsvec = hpic_vec_float_alloc((size_t) nrows);
  pixvec = hpic_vec_int_alloc((size_t) nrows);
  hitsvec = hpic_vec_int_alloc((size_t) nrows);

  hpic_int_name_set(pixels, colnames[0]);
  hpic_int_units_set(pixels, colunits[0]);
  if (fits_read_col
      (fp, TINT, 1, 1, 1, nrows, &nullval, pixvec->data, &nnull, &ret)) {
    fitserr(ret, "hpic_fits_cut_read:  reading pixels");
  }
  hpic_int_setall(pixels, HPIC_INT_NULL);
  for (j = 0; j < (size_t) (nrows); j++) {
    hpic_int_set(pixels, (size_t) hpic_vec_int_get(pixvec, j),
                 hpic_vec_int_get(pixvec, j));
  }

  for (i = 0; i < tmaps; i++) {
    tempmap = hpic_fltarr_get(maps, i);
    hpic_float_setall(tempmap, HPIC_NULL);
    hpic_float_name_set(tempmap, colnames[i + 1]);
    hpic_float_units_set(tempmap, colunits[i + 1]);
    if (fits_read_col
        (fp, TFLOAT, (int)(i + 2), 1, 1, nrows, &nullval, datavec->data,
         &nnull, &ret)) {
      fitserr(ret, "hpic_fits_cut_read:  reading data");
    }
    for (j = 0; j < (size_t) (nrows); j++) {
      hpic_float_set(tempmap, (size_t) hpic_vec_int_get(pixvec, j),
                     hpic_vec_float_get(datavec, j));
    }
  }

  hpic_int_name_set(hits, colnames[tmaps + 1]);
  hpic_int_units_set(hits, colunits[tmaps + 1]);
  if (fits_read_col
      (fp, TINT, (int)(tmaps + 2), 1, 1, nrows, &nullval, hitsvec->data,
       &nnull, &ret)) {
    fitserr(ret, "hpic_fits_cut_read:  reading hits");
  }
  hpic_int_setall(hits, HPIC_INT_NULL);
  for (j = 0; j < (size_t) (nrows); j++) {
    hpic_int_set(hits, (size_t) hpic_vec_int_get(pixvec, j),
                 hpic_vec_int_get(hitsvec, j));
  }
  hpic_float_name_set(errs, colnames[tmaps + 2]);
  hpic_float_units_set(errs, colunits[tmaps + 2]);
  if (fits_read_col
      (fp, TFLOAT, (int)(tmaps + 3), 1, 1, nrows, &nullval, errsvec->data,
       &nnull, &ret)) {
    fitserr(ret, "hpic_fits_cut_read:  reading errs");
  }
  hpic_float_setall(errs, HPIC_NULL);
  for (j = 0; j < (size_t) (nrows); j++) {
    hpic_float_set(errs, (size_t) hpic_vec_int_get(pixvec, j),
                   hpic_vec_float_get(errsvec, j));
  }

  hpic_vec_float_free(datavec);
  hpic_vec_float_free(errsvec);
  hpic_vec_int_free(pixvec);
  hpic_vec_int_free(hitsvec);
  hpic_strarr_free(colnames, tmaps + 3);
  hpic_strarr_free(coltypes, tmaps + 3);
  hpic_strarr_free(colunits, tmaps + 3);
  
  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_cut_read:  closing file");
  }

  return ret;
}

/* read one map from a full or cut-sky file */
hpic_float *hpic_fits_read_one(char *filename, size_t mapnum, char *creator, hpic_keys * keys)
{
  size_t i, j, k;
  fitsfile *fp;
  int ret = 0;
  size_t nside;
  int order;
  int coord;
  int type;
  int ttype;
  long nrows;
  long pcount;
  int tfields;
  size_t nmaps;
  size_t absnum;
  size_t ncol;
  hpic_float *data;
  hpic_vec_float *datavec;
  hpic_vec_int *pixvec;
  char comment[HPIC_STRNL];
  char extname[HPIC_STRNL];
  char **colnames;
  char **coltypes;
  char **colunits;
  float nullval;
  int nnull;
  int keynpix;
  int keyfirst;
  int ischunk;
  long nelem;

  if (!hpic_fits_map_test(filename, &nside, &order, &coord, &type, &nmaps)) {
    HPIC_ERROR_VAL(HPIC_ERR_FITS, "file is not a healpix format file!", NULL);
  }
  if (mapnum >= nmaps) {
    HPIC_ERROR_VAL(HPIC_ERR_RANGE, "requested map number is out of range", NULL);
  }
  if (type == HPIC_FITS_FULL) {
    absnum = mapnum;
    ncol = nmaps;
  } else {
    absnum = mapnum + 1;
    ncol = nmaps + 3;
  }

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    fitserr(ret, "hpic_fits_read_one:  opening file");
  }

  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* read primary image keys */
  hpic_keys_clear(keys);
  hpic_keys_read(keys, fp, &ret);

  /* read extension info */
  if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
    fitserr(ret, "hpic_fits_read_one:  moving to extension");
  }
  colnames = hpic_strarr_alloc(ncol);
  coltypes = hpic_strarr_alloc(ncol);
  colunits = hpic_strarr_alloc(ncol);

  if (fits_read_btblhdr
      (fp, (int)ncol, &nrows, &tfields, colnames, coltypes, colunits, extname,
       &pcount, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* read extension keys */
  hpic_keys_read(keys, fp, &ret);

  /* read data */

  data = hpic_float_alloc(nside, order, coord, HPIC_AUTO);
  if (!data) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate output map", NULL);
  }
  hpic_float_name_set(data, colnames[absnum]);
  hpic_float_units_set(data, colunits[absnum]);

  if (type == HPIC_FITS_CUT) {
    datavec = hpic_vec_float_alloc((size_t) nrows);
    pixvec = hpic_vec_int_alloc((size_t) nrows);
    if (fits_read_col
        (fp, TINT, 1, 1, 1, nrows, &nullval, pixvec->data, &nnull, &ret)) {
      fitserr(ret, "hpic_fits_read_one:  reading pixels");
    }
    if (fits_read_col
        (fp, TFLOAT, (int)(absnum + 1), 1, 1, nrows, &nullval, datavec->data,
         &nnull, &ret)) {
      fitserr(ret, "hpic_fits_read_one:  reading data");
    }
    for (j = 0; j < (size_t) (nrows); j++) {
      hpic_float_set(data, (size_t) hpic_vec_int_get(pixvec, j),
                     hpic_vec_float_get(datavec, j));
    }
    hpic_vec_int_free(pixvec);
  } else {
    /* is this a chunk? */
    if ((nrows != (long)(12*nside*nside))&&(1024*nrows != (long)(12*nside*nside))) {
      /*this must be a chunk file*/
      if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
        /*must at least have FIRSTPIX key*/
        fits_close_file(fp, &ret);
        return 0;
      } else {
        if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
          ret = 0;
          /*might be using LASTPIX instead*/
          if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
            fits_close_file(fp, &ret);
            return 0;
          } else {
            keynpix = keynpix - keyfirst + 1;
            ischunk = 1;
          }
        } else {
          ischunk = 1;
        }
      }
    } else {
      ischunk = 0;
    }
    if (ischunk) {
      datavec = hpic_vec_float_alloc((size_t)keynpix);
      nelem = (long)keynpix;
    } else {
      datavec = hpic_vec_float_alloc(12*nside*nside);
      nelem = (long)(12*nside*nside);
    }
    if (fits_read_col(fp, TFLOAT, (int)(absnum + 1), 1, 1, nelem, &nullval, datavec->data, &nnull, &ret)) {
      fitserr(ret, "hpic_fits_read_one:  reading data");
    }
    if (ischunk) {
      for (j = 0; j < (size_t)nelem; j++) {
        hpic_float_set(data, j+(size_t)keyfirst, hpic_vec_float_get(datavec, j));
      }
    } else {
      for (j = 0; j < (size_t)nelem; j++) {
        hpic_float_set(data, j, hpic_vec_float_get(datavec, j));
      }
    }
  }

  hpic_vec_float_free(datavec);
  hpic_strarr_free(colnames, ncol);
  hpic_strarr_free(coltypes, ncol);
  hpic_strarr_free(colunits, ncol);

  fits_close_file(fp, &ret);
  return data;
}

