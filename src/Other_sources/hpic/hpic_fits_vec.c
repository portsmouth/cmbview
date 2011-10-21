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
 * This code deals with FITS I/O of vectors                                  *
 *****************************************************************************/

#include <hpic.h>

/* file tests */

int hpic_fits_vec_test(char *filename, size_t * nvecs, size_t * length,
                       int *filetype, int *tabtype)
{

  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int ttype;
  long nrows;
  long rowlen;
  long pcount;
  char extname[HPIC_STRNL];
  int tfields;
  long repeat;
  long width;

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    return 0;
  }

  /* check extension type, number of columns */
  if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (ttype == ASCII_TBL) {
    (*tabtype) = HPIC_FITS_ASCII;
    if (fits_read_atblhdr
        (fp, HPIC_FITS_MAXCOL, &rowlen, &nrows, &tfields, NULL, NULL, NULL, NULL, extname,
         &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    (*tabtype) = HPIC_FITS_BIN;
    if (fits_read_btblhdr
        (fp, HPIC_FITS_MAXCOL, &nrows, &tfields, NULL, NULL, NULL, extname, &pcount,
         &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  }

  /* determine the types of vectors */

  if (fits_get_coltype(fp, 1, &ttype, &repeat, &width, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if ((ttype == TINT) || (ttype == TINT32BIT) || (ttype == TLONG) ||
      (ttype == TSHORT)) {
    (*filetype) = HPIC_FITS_VEC_INDX;
  } else if ((ttype == TFLOAT) || (ttype == TDOUBLE)) {
    (*filetype) = HPIC_FITS_VEC;
  } else {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  for (i = 1; i < (size_t) tfields; i++) {
    if (fits_get_coltype(fp, (int)(i + 1), &ttype, &repeat, &width, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
    if ((ttype != TFLOAT) && (ttype != TDOUBLE)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  }

  if ((*filetype) == HPIC_FITS_VEC_INDX) {
    (*nvecs) = (size_t) tfields - 1;
  } else {
    (*nvecs) = (size_t) tfields;
  }
  (*length) = (size_t) nrows;

  fits_close_file(fp, &ret);
  return 1;
}

int hpic_fits_vec_info(char *filename, size_t * nvecs, size_t * length,
                       int *filetype, int *tabtype, char *creator, char *extname, char **names, char **units, hpic_keys * keys)
{
  
  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int ttype;
  long nrows;
  long rowlen;
  long pcount;
  char comment[HPIC_STRNL];
  int tfields;
  long repeat;
  long width;

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    return 0;
  }
  
  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* check extension type, number of columns */
  if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (ttype == ASCII_TBL) {
    (*tabtype) = HPIC_FITS_ASCII;
    if (fits_read_atblhdr
      (fp, HPIC_FITS_MAXCOL, &rowlen, &nrows, &tfields, names, NULL, NULL, units, extname,
      &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    (*tabtype) = HPIC_FITS_BIN;
    if (fits_read_btblhdr
      (fp, HPIC_FITS_MAXCOL, &nrows, &tfields, names, NULL, units, extname, &pcount,
      &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  }

  /* determine the types of vectors */

  if (fits_get_coltype(fp, 1, &ttype, &repeat, &width, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if ((ttype == TINT) || (ttype == TINT32BIT) || (ttype == TLONG) ||
    (ttype == TSHORT)) {
    (*filetype) = HPIC_FITS_VEC_INDX;
  } else if ((ttype == TFLOAT) || (ttype == TDOUBLE)) {
    (*filetype) = HPIC_FITS_VEC;
  } else {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  for (i = 1; i < (size_t) tfields; i++) {
    if (fits_get_coltype(fp, (int)(i + 1), &ttype, &repeat, &width, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
    if ((ttype != TFLOAT) && (ttype != TDOUBLE)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  }

  /* read extension keys */
  hpic_keys_read(keys, fp, &ret);
      
  if ((*filetype) == HPIC_FITS_VEC_INDX) {
    (*nvecs) = (size_t) tfields - 1;
  } else {
    (*nvecs) = (size_t) tfields;
  }
  (*length) = (size_t) nrows;

  fits_close_file(fp, &ret);
  return 1;
}

/* FITS vector writing */

int hpic_fits_vec_write(char *filename, int tabtype, char *creator,
                        char *extname, char *comment, hpic_vec_fltarr * vecs,
                        char **vecnames, char **vecunits, hpic_keys * keys)
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
  size_t veclen;
  char **vectypes;
  size_t nvecs = hpic_vec_fltarr_n_get(vecs);
  hpic_vec_float *tempvec;

  if (nvecs == 0) {
    HPIC_ERROR(HPIC_ERR_FITS, "must specify more than zero vectors!");
  }
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (!tempvec) {
      HPIC_ERROR(HPIC_ERR_ACCESS, "input vector is not allocated");
    }
  }

  vectypes = hpic_strarr_alloc(nvecs);

  tempvec = hpic_vec_fltarr_get(vecs, 0);
  veclen = hpic_vec_float_n_get(tempvec);
  for (i = 1; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (veclen != hpic_vec_float_n_get(tempvec)) {
      HPIC_ERROR(HPIC_ERR_FITS, "all vectors must have the same length");
    }
  }

  /* setup column parameters */

  if (tabtype == HPIC_FITS_ASCII) {
    for (i = 0; i < nvecs; i++) {
      strncpy(vectypes[i], "E15", HPIC_STRNL);
    }
  } else {
    for (i = 0; i < nvecs; i++) {
      strncpy(vectypes[i], "1E", HPIC_STRNL);
    }
  }

  /* create file */
  if (fits_create_file(&fp, filename, &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  creating file");
  }

  /* create empty primary image */
  if (fits_create_img(fp, bitpix, nax, axes, &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  creating primary image");
  }

  /* write header */
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing header comment 1");
  }
  if (fits_write_comment(fp, comment, &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing header comment 2");
  }
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing header comment 3");
  }
  if (fits_write_date(fp, &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing date");
  }
  if (fits_write_key
      (fp, TSTRING, "CREATOR", creator, "Software creating the map", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing creator");
  }

  /* write extension */
  if (tabtype == HPIC_FITS_ASCII) {
    if (fits_create_tbl
        (fp, ASCII_TBL, (long)veclen, (int)nvecs, vecnames, vectypes,
         vecunits, extname, &ret)) {
      fitserr(ret, "hpic_fits_vec_write:  creating ASCII table");
    }
  } else {
    if (fits_create_tbl
        (fp, BINARY_TBL, (long)veclen, (int)nvecs, vecnames, vectypes,
         vecunits, extname, &ret)) {
      fitserr(ret, "hpic_fits_vec_write:  creating binary table");
    }
  }
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  moving to extension");
  }

  /* write mandatory keywords */
  if (fits_write_comment(fp, "     ", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing ext comment 1");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing ext comment 2");
  }
  if (fits_write_comment(fp, "            Data Specific Keywords    ", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing ext comment 3");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing ext comment 4");
  }
  if (fits_write_comment(fp, "      ", &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  writing comment 5");
  }

  /* write optional keywords */

  hpic_keys_write(keys, fp, &ret);

  /* write the data and clean up */
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (fits_write_col
        (fp, TFLOAT, (int)(i + 1), frow, fsamp, (long)veclen, tempvec->data,
         &ret)) {
      fitserr(ret, "hpic_fits_vec_write:  writing column");
    }
  }
  
  hpic_strarr_free(vectypes, nvecs);
  
  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_vec_write:  closing file");
  }

  return ret;
}

int hpic_fits_vecindx_write(char *filename, char *creator, char *extname,
                            char *comment, hpic_vec_int * indx,
                            hpic_vec_fltarr * vecs, char **vecnames,
                            char **vecunits, hpic_keys * keys)
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
  size_t veclen;
  char **vectypes;
  char **names;
  char **units;
  size_t nvecs = hpic_vec_fltarr_n_get(vecs);
  hpic_vec_float *tempvec;

  if (nvecs == 0) {
    HPIC_ERROR(HPIC_ERR_FITS, "must specify more than zero vectors!");
  }
  if (!indx) {
    HPIC_ERROR(HPIC_ERR_ACCESS, "index vector is not allocated");
  }
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (!tempvec) {
      HPIC_ERROR(HPIC_ERR_ACCESS, "input vector is not allocated");
    }
  }
  vectypes = hpic_strarr_alloc(nvecs + 1);
  names = hpic_strarr_alloc(nvecs + 1);
  units = hpic_strarr_alloc(nvecs + 1);

  veclen = hpic_vec_int_n_get(indx);
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (veclen != hpic_vec_float_n_get(tempvec)) {
      HPIC_ERROR(HPIC_ERR_FITS, "all vectors must have the same length");
    }
  }

  /* setup column parameters */

  strncpy(vectypes[0], "1J", HPIC_STRNL);
  strncpy(names[0], "INDEX", HPIC_STRNL);
  strncpy(units[0], "", HPIC_STRNL);
  for (i = 1; i < nvecs + 1; i++) {
    strncpy(vectypes[i], "1E", HPIC_STRNL);
    strncpy(names[i], vecnames[i - 1], HPIC_STRNL);
    strncpy(units[i], vecunits[i - 1], HPIC_STRNL);
  }

  /* create file */
  if (fits_create_file(&fp, filename, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  creating file");
  }

  /* create empty primary image */
  if (fits_create_img(fp, bitpix, nax, axes, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  creating primary image");
  }

  /* write header */
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing header comment 1");
  }
  if (fits_write_comment(fp, comment, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing header comment 2");
  }
  if (fits_write_comment(fp, "    ", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing header comment 3");
  }
  if (fits_write_date(fp, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing date");
  }
  if (fits_write_key
      (fp, TSTRING, "CREATOR", creator, "Software creating the map", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing creator");
  }

  /* write extension */
  if (fits_create_tbl
      (fp, BINARY_TBL, (long)veclen, (int)(nvecs + 1), names, vectypes, units,
       extname, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  creating binary table");
  }
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  moving to extension");
  }

  /* write mandatory keywords */
  if (fits_write_comment(fp, "     ", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing ext comment 1");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing ext comment 2");
  }
  if (fits_write_comment(fp, "            Data Specific Keywords    ", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing ext comment 3");
  }
  if (fits_write_comment
      (fp, "-----------------------------------------------", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing ext comment 4");
  }
  if (fits_write_comment(fp, "      ", &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing comment 5");
  }

  /* write optional keywords */

  hpic_keys_write(keys, fp, &ret);

  /* write the data and clean up */
  if (fits_write_col
      (fp, TINT, 1, frow, fsamp, (long)veclen, indx->data, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  writing index column");
  }
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (fits_write_col
        (fp, TFLOAT, (int)(i + 2), frow, fsamp, (long)veclen, tempvec->data,
         &ret)) {
      fitserr(ret, "hpic_fits_vecindx_write:  writing data column");
    }
  }
  
  hpic_strarr_free(vectypes, nvecs + 1);
  hpic_strarr_free(names, nvecs + 1);
  hpic_strarr_free(units, nvecs + 1);

  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_write:  closing file");
  }

  return ret;
}

/* FITS vector reading */

int hpic_fits_vec_read(char *filename, char *creator, char *extname,
                       hpic_vec_fltarr * vecs, char **vecnames,
                       char **vecunits, hpic_keys * keys)
{

  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int bitpix = SHORT_IMG;
  int nax = 0;
  long axes[] = { 0, 0 };
  int total;
  int type;
  int ttype;
  int filetype;
  int tabtype;
  long nrows;
  int tfields;
  long frow = 1;
  long fsamp = 1;
  size_t veclen;
  size_t tvecs;
  char **vectypes;
  long rowlen;
  long *tbcol;
  long pcount = 0;
  char comment[HPIC_STRNL];
  float nullval;
  int nnull;
  size_t nvecs = hpic_vec_fltarr_n_get(vecs);
  hpic_vec_float *tempvec;

  if (nvecs <= 0) {
    HPIC_ERROR(HPIC_ERR_FITS, "must specify more than zero vectors!");
  }
  if (!hpic_fits_vec_test(filename, &tvecs, &veclen, &filetype, &tabtype)) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a healpix vector file");
  }
  if (filetype != HPIC_FITS_VEC) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a float vector file - try hpic_fits_vecindx_read");
  }
  if (tvecs > nvecs) {
    HPIC_ERROR(HPIC_ERR_FITS, "vector array does not have enough room for data in file");
  }
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (veclen != hpic_vec_float_n_get(tempvec)) {
      HPIC_ERROR(HPIC_ERR_FITS, "all vectors must have the same length");
    }
  }
  vectypes = hpic_strarr_alloc(nvecs);
  if (tabtype == HPIC_FITS_ASCII) {
    tbcol = (long *)calloc(nvecs, sizeof(long));
  }

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    fitserr(ret, "hpic_fits_vec_read:  opening file");
  }

  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* read extension info */
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_vec_read:  moving to extension");
  }

  if (tabtype == HPIC_FITS_ASCII) {
    if (fits_read_atblhdr
        (fp, (int)nvecs, &rowlen, &nrows, &tfields, vecnames, tbcol, vectypes,
         vecunits, extname, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    if (fits_read_btblhdr
        (fp, (int)nvecs, &nrows, &tfields, vecnames, vectypes, vecunits,
         extname, &pcount, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  }

  /* read optional keywords */

  hpic_keys_clear(keys);
  hpic_keys_read(keys, fp, &ret);

  /* read the data */
  nullval = HPIC_NULL;
  nnull = 0;
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (fits_read_col
        (fp, TFLOAT, (int)(i + 1), 1, 1, (long)(veclen), &nullval,
         tempvec->data, &nnull, &ret)) {
      fitserr(ret, "hpic_fits_vec_read:  reading data");
    }
  }

  hpic_strarr_free(vectypes, nvecs);
  if (tabtype == HPIC_FITS_ASCII) {
    free(tbcol);
  }

  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_vec_read:  closing file");
  }
  return ret;
}

int hpic_fits_vecindx_read(char *filename, char *creator, char *extname,
                           hpic_vec_int * indx, hpic_vec_fltarr * vecs,
                           char **vecnames, char **vecunits, hpic_keys * keys)
{

  size_t i, j, k, m;
  fitsfile *fp;
  int ret = 0;
  int bitpix = SHORT_IMG;
  int nax = 0;
  long axes[] = { 0, 0 };
  int total;
  int type;
  int ttype;
  int filetype;
  int tabtype;
  long nrows;
  int tfields;
  long frow = 1;
  long fsamp = 1;
  size_t veclen;
  size_t tvecs;
  char **vectypes;
  long rowlen;
  long *tbcol;
  long pcount = 0;
  char comment[HPIC_STRNL];
  float nullval;
  int nnull;
  char **names;
  char **units;
  size_t nvecs = hpic_vec_fltarr_n_get(vecs);
  hpic_vec_float *tempvec;

  if (nvecs <= 0) {
    HPIC_ERROR(HPIC_ERR_FITS, "must specify more than zero vectors!");
  }
  if (!hpic_fits_vec_test(filename, &tvecs, &veclen, &filetype, &tabtype)) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not a healpix vector file");
  }
  if (filetype != HPIC_FITS_VEC_INDX) {
    HPIC_ERROR(HPIC_ERR_FITS, "file is not an index vector file - try hpic_fits_vec_read");
  }
  if (tvecs > nvecs) {
    HPIC_ERROR(HPIC_ERR_FITS, "vector array does not have enough room for data in file");
  }
  if (veclen != hpic_vec_int_n_get(indx)) {
    HPIC_ERROR(HPIC_ERR_FITS, "index vector does not have correct length");
  }
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (veclen != hpic_vec_float_n_get(tempvec)) {
      HPIC_ERROR(HPIC_ERR_FITS, "all vectors must have the same length");
    }
  }
  vectypes = hpic_strarr_alloc(nvecs + 1);
  names = hpic_strarr_alloc(nvecs + 1);
  units = hpic_strarr_alloc(nvecs + 1);
  if (tabtype == HPIC_FITS_ASCII) {
    tbcol = (long *)calloc(nvecs + 1, sizeof(long));
  }

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_read:  opening file");
  }

  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* read extension info */
  if (fits_movabs_hdu(fp, 2, &type, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_read:  moving to extension");
  }

  if (tabtype == HPIC_FITS_ASCII) {
    if (fits_read_atblhdr
        (fp, (int)(nvecs + 1), &rowlen, &nrows, &tfields, names, tbcol,
         vectypes, units, extname, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    if (fits_read_btblhdr
        (fp, (int)(nvecs + 1), &nrows, &tfields, names, vectypes, units,
         extname, &pcount, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  }
  for (i = 0; i < nvecs; i++) {
    strncpy(vecnames[i], names[i + 1], HPIC_STRNL);
    strncpy(vecunits[i], units[i + 1], HPIC_STRNL);
  }

  /* read optional keywords */
  hpic_keys_clear(keys);
  hpic_keys_read(keys, fp, &ret);

  /* read the data */
  nullval = HPIC_NULL;
  nnull = 0;
  if (fits_read_col
      (fp, TINT, 1, 1, 1, (long)(veclen), &nullval, indx->data, &nnull,
       &ret)) {
    fitserr(ret, "hpic_fits_vecindx_read:  reading index");
  }
  for (i = 0; i < nvecs; i++) {
    tempvec = hpic_vec_fltarr_get(vecs, i);
    if (fits_read_col
        (fp, TFLOAT, (int)(i + 2), 1, 1, (long)(veclen), &nullval,
         tempvec->data, &nnull, &ret)) {
      fitserr(ret, "hpic_fits_vecindx_read:  reading data");
    }
  }

  hpic_strarr_free(vectypes, nvecs + 1);
  hpic_strarr_free(names, nvecs + 1);
  hpic_strarr_free(units, nvecs + 1);
  if (tabtype == HPIC_FITS_ASCII) {
    free(tbcol);
  }
  
  if (fits_close_file(fp, &ret)) {
    fitserr(ret, "hpic_fits_vecindx_read:  closing file");
  }

  return ret;
}

