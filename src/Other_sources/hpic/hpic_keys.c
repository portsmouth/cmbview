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
 * This code deals with the FITS keys structure                              *
 *****************************************************************************/

#include <hpic.h>

/* alloc/free */

hpic_keys *hpic_keys_alloc();

hpic_keys *hpic_keys_alloc()
{
  hpic_keys *keys;
  keys = (hpic_keys *) calloc(1, sizeof(hpic_keys));
  if (!keys) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate keys", NULL);
  }
  keys->nskeys = 0;
  keys->nikeys = 0;
  keys->nfkeys = 0;
  keys->skeynames = NULL;
  keys->skeyvals = NULL;
  keys->skeycoms = NULL;
  keys->ikeynames = NULL;
  keys->ikeyvals = NULL;
  keys->ikeycoms = NULL;
  keys->fkeynames = NULL;
  keys->fkeyvals = NULL;
  keys->fkeycoms = NULL;
  return keys;
}

int hpic_keys_free(hpic_keys * keys)
{
  size_t i;
  if (keys) {
    for (i = 0; i < (keys->nskeys); i++) {
      free(keys->skeynames[i]);
      free(keys->skeyvals[i]);
      free(keys->skeycoms[i]);
    }
    if (keys->nskeys != 0) {
      free(keys->skeynames);
      free(keys->skeyvals);
      free(keys->skeycoms);
    }
    for (i = 0; i < (keys->nikeys); i++) {
      free(keys->ikeynames[i]);
      free(keys->ikeycoms[i]);
    }
    if (keys->nikeys != 0) {
      free(keys->ikeynames);
      free(keys->ikeyvals);
      free(keys->ikeycoms);
    }
    for (i = 0; i < (keys->nfkeys); i++) {
      free(keys->fkeynames[i]);
      free(keys->fkeycoms[i]);
    }
    if (keys->nfkeys != 0) {
      free(keys->fkeynames);
      free(keys->fkeyvals);
      free(keys->fkeycoms);
    }
    free(keys);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "keys not allocated, so not freeing");
  }
}

int hpic_keys_clear(hpic_keys * keys)
{
  size_t i;
  if (keys) {
    for (i = 0; i < (keys->nskeys); i++) {
      free(keys->skeynames[i]);
      free(keys->skeyvals[i]);
      free(keys->skeycoms[i]);
    }
    if (keys->nskeys != 0) {
      free(keys->skeynames);
      free(keys->skeyvals);
      free(keys->skeycoms);
    }
    for (i = 0; i < (keys->nikeys); i++) {
      free(keys->ikeynames[i]);
      free(keys->ikeycoms[i]);
    }
    if (keys->nikeys != 0) {
      free(keys->ikeynames);
      free(keys->ikeyvals);
      free(keys->ikeycoms);
    }
    for (i = 0; i < (keys->nfkeys); i++) {
      free(keys->fkeynames[i]);
      free(keys->fkeycoms[i]);
    }
    if (keys->nfkeys != 0) {
      free(keys->fkeynames);
      free(keys->fkeyvals);
      free(keys->fkeycoms);
    }
    keys->nskeys = 0;
    keys->nikeys = 0;
    keys->nfkeys = 0;
    keys->skeynames = NULL;
    keys->skeyvals = NULL;
    keys->skeycoms = NULL;
    keys->ikeynames = NULL;
    keys->ikeyvals = NULL;
    keys->ikeycoms = NULL;
    keys->fkeynames = NULL;
    keys->fkeyvals = NULL;
    keys->fkeycoms = NULL;
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "keys not allocated, so not freeing");
  }
}

/* add keys */

int hpic_keys_sadd(hpic_keys * keys, char *keyname, char *keyval, char *keycom)
{
  if (keys) {
    (keys->skeynames) =
      (char **)realloc(keys->skeynames,
                       ((keys->nskeys) + 1) * sizeof(char *));
    if (!(keys->skeynames)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc skeynames");
    }
    (keys->skeynames[keys->nskeys]) =
      (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!(keys->skeynames[keys->nskeys])) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocated appended skeyname");
    }
    (keys->skeyvals) =
      (char **)realloc(keys->skeyvals, ((keys->nskeys) + 1) * sizeof(char *));
    if (!(keys->skeyvals)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc skeyvals");
    }
    (keys->skeyvals[keys->nskeys]) = (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!(keys->skeyvals[keys->nskeys])) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocated appended skeyval");
    }
    (keys->skeycoms) =
      (char **)realloc(keys->skeycoms, ((keys->nskeys) + 1) * sizeof(char *));
    if (!(keys->skeycoms)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc skeycoms");
    }
    (keys->skeycoms[keys->nskeys]) = (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!(keys->skeycoms[keys->nskeys])) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocated appended skeycom");
    }
    strncpy(keys->skeynames[keys->nskeys], keyname, HPIC_STRNL);
    strncpy(keys->skeyvals[keys->nskeys], keyval, HPIC_STRNL);
    strncpy(keys->skeycoms[keys->nskeys], keycom, HPIC_STRNL);
    (keys->nskeys)++;
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }
}

int hpic_keys_iadd(hpic_keys * keys, char *keyname, int keyval, char *keycom)
{
  if (keys) {
    (keys->ikeynames) =
      (char **)realloc(keys->ikeynames,
                       ((keys->nikeys) + 1) * sizeof(char *));
    if (!(keys->ikeynames)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc ikeynames");
    }
    (keys->ikeynames[keys->nikeys]) =
      (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!(keys->ikeynames[keys->nikeys])) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocated appended ikeyname");
    }
    (keys->ikeyvals) =
      (int *)realloc(keys->ikeyvals, ((keys->nikeys) + 1) * sizeof(int));
    if (!(keys->ikeyvals)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc ikeyvals");
    }
    (keys->ikeycoms) =
      (char **)realloc(keys->ikeycoms, ((keys->nikeys) + 1) * sizeof(char *));
    if (!(keys->ikeycoms)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc ikeycoms");
    }
    (keys->ikeycoms[keys->nikeys]) = (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!(keys->ikeycoms[keys->nikeys])) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocated appended ikeycom");
    }
    strncpy(keys->ikeynames[keys->nikeys], keyname, HPIC_STRNL);
    keys->ikeyvals[keys->nikeys] = keyval;
    strncpy(keys->ikeycoms[keys->nikeys], keycom, HPIC_STRNL);
    (keys->nikeys)++;
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }
}

int hpic_keys_fadd(hpic_keys * keys, char *keyname, float keyval, char *keycom)
{
  if (keys) {
    (keys->fkeynames) =
        (char **)realloc(keys->fkeynames,
    ((keys->nfkeys) + 1) * sizeof(char *));
    if (!(keys->fkeynames)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc fkeynames");
    }
    (keys->fkeynames[keys->nfkeys]) =
        (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!(keys->fkeynames[keys->nfkeys])) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocated appended fkeyname");
    }
    (keys->fkeyvals) =
        (float *)realloc(keys->fkeyvals, ((keys->nfkeys) + 1) * sizeof(float));
    if (!(keys->fkeyvals)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc fkeyvals");
    }
    (keys->fkeycoms) =
        (char **)realloc(keys->fkeycoms, ((keys->nfkeys) + 1) * sizeof(char *));
    if (!(keys->fkeycoms)) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc fkeycoms");
    }
    (keys->fkeycoms[keys->nfkeys]) = (char *)calloc(HPIC_STRNL, sizeof(char));
    if (!(keys->fkeycoms[keys->nfkeys])) {
      HPIC_ERROR(HPIC_ERR_ALLOC, "cannot allocated appended fkeycom");
    }
    strncpy(keys->fkeynames[keys->nfkeys], keyname, HPIC_STRNL);
    keys->fkeyvals[keys->nfkeys] = keyval;
    strncpy(keys->fkeycoms[keys->nfkeys], keycom, HPIC_STRNL);
    (keys->nfkeys)++;
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }  
}

/* delete keys */

int hpic_keys_del(hpic_keys * keys, char *keyname)
{
  size_t i, j;
  if (keys) {
    i = 0;
    while (i < (keys->nskeys)) {
      if (strncmp(keys->skeynames[i], keyname, HPIC_STRNL) == 0) {
        for (j = i; j < (keys->nskeys) - 1; j++) {
          strncpy(keys->skeynames[j], keys->skeynames[j + 1], HPIC_STRNL);
          strncpy(keys->skeyvals[j], keys->skeyvals[j + 1], HPIC_STRNL);
          strncpy(keys->skeycoms[j], keys->skeycoms[j + 1], HPIC_STRNL);
        }
        free(keys->skeynames[(keys->nskeys) - 1]);
        free(keys->skeyvals[(keys->nskeys) - 1]);
        free(keys->skeycoms[(keys->nskeys) - 1]);
        (keys->nskeys)--;
        if (keys->nskeys > 0) {
          (keys->skeynames) =
            (char **)realloc(keys->skeynames, (keys->nskeys) * sizeof(char *));
          if (!(keys->skeynames)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc skeynames");
          }
          (keys->skeyvals) =
            (char **)realloc(keys->skeyvals, (keys->nskeys) * sizeof(char *));
          if (!(keys->skeyvals)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc skeyvals");
          }
          (keys->skeycoms) =
            (char **)realloc(keys->skeycoms, (keys->nskeys) * sizeof(char *));
          if (!(keys->skeycoms)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc skeycoms");
          }
        } else {
          free(keys->skeynames);
          free(keys->skeyvals);
          free(keys->skeycoms);
        }
      } else {
        i++;
      }
    }
    i = 0;
    while (i < (keys->nikeys)) {
      if (strncmp(keys->ikeynames[i], keyname, HPIC_STRNL) == 0) {
        for (j = i; j < (keys->nikeys) - 1; j++) {
          strncpy(keys->ikeynames[j], keys->ikeynames[j + 1], HPIC_STRNL);
          keys->ikeyvals[j] = keys->ikeyvals[j + 1];
          strncpy(keys->ikeycoms[j], keys->ikeycoms[j + 1], HPIC_STRNL);
        }
        free(keys->ikeynames[(keys->nikeys) - 1]);
        free(keys->ikeycoms[(keys->nikeys) - 1]);
        (keys->nikeys)--;
        if (keys->nikeys > 0) {
          (keys->ikeynames) =
            (char **)realloc(keys->ikeynames, (keys->nikeys) * sizeof(char *));
          if (!(keys->ikeynames)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc ikeynames");
          }
          (keys->ikeyvals) =
            (int *)realloc(keys->ikeyvals, (keys->nikeys) * sizeof(int));
          if (!(keys->ikeyvals)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc ikeyvals");
          }
          (keys->ikeycoms) =
            (char **)realloc(keys->ikeycoms, (keys->nikeys) * sizeof(char *));
          if (!(keys->ikeycoms)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc ikeycoms");
          }
        } else {
          free(keys->ikeynames);
          free(keys->ikeyvals);
          free(keys->ikeycoms);
        } 
      } else {
        i++;
      }
    }
    i = 0;
    while (i < (keys->nfkeys)) {
      if (strncmp(keys->fkeynames[i], keyname, HPIC_STRNL) == 0) {
        for (j = i; j < (keys->nfkeys) - 1; j++) {
          strncpy(keys->fkeynames[j], keys->fkeynames[j + 1], HPIC_STRNL);
          keys->fkeyvals[j] = keys->fkeyvals[j + 1];
          strncpy(keys->fkeycoms[j], keys->fkeycoms[j + 1], HPIC_STRNL);
        }
        free(keys->fkeynames[(keys->nfkeys) - 1]);
        free(keys->fkeycoms[(keys->nfkeys) - 1]);
        (keys->nfkeys)--;
        if (keys->nfkeys > 0) {
          (keys->fkeynames) =
            (char **)realloc(keys->fkeynames, (keys->nfkeys) * sizeof(char *));
          if (!(keys->fkeynames)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc fkeynames");
          }
          (keys->fkeyvals) =
            (float *)realloc(keys->fkeyvals, (keys->nfkeys) * sizeof(float));
          if (!(keys->fkeyvals)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc fkeyvals");
          }
          (keys->fkeycoms) =
            (char **)realloc(keys->fkeycoms, (keys->nfkeys) * sizeof(char *));
          if (!(keys->fkeycoms)) {
            HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc fkeycoms");
          }
        } else {
          free(keys->fkeynames);
          free(keys->fkeyvals);
          free(keys->fkeycoms);
        }
      } else {
        i++;
      }
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }
}

/* lookup keys */

int hpic_keys_total(hpic_keys * keys)
{
  int total = 0;
  if (keys) {
    total += (int)(keys->nskeys);
    total += (int)(keys->nikeys);
    total += (int)(keys->nfkeys);
    return total;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "keys pointer is NULL", 0);
  }
}

int hpic_keys_sfind(hpic_keys * keys, char *keyname, char *keyval, char *keycom)
{
  size_t i, j;
  if (keys) {
    for (i = 0; i < (keys->nskeys); i++) {
      if (strncmp(keys->skeynames[i], keyname, HPIC_STRNL) == 0) {
        strncpy(keyval, keys->skeyvals[i], HPIC_STRNL);
        strncpy(keycom, keys->skeycoms[i], HPIC_STRNL);
        return 0;
      }
    }
    strncpy(keyval, "", HPIC_STRNL);
    strncpy(keycom, "", HPIC_STRNL);
    return 1;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }
}

int hpic_keys_ifind(hpic_keys * keys, char *keyname, int *keyval, char *keycom)
{
  size_t i, j;
  if (keys) {
    for (i = 0; i < (keys->nikeys); i++) {
      if (strncmp(keys->ikeynames[i], keyname, HPIC_STRNL) == 0) {
        (*keyval) = keys->ikeyvals[i];
        strncpy(keycom, keys->ikeycoms[i], HPIC_STRNL);
        return 0;
      }
    }
    (*keyval) = 0;
    strncpy(keycom, "", HPIC_STRNL);
    return 1;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }
}

int hpic_keys_ffind(hpic_keys * keys, char *keyname, float *keyval, char *keycom)
{
  size_t i, j;
  if (keys) {
    for (i = 0; i < (keys->nfkeys); i++) {
      if (strncmp(keys->fkeynames[i], keyname, HPIC_STRNL) == 0) {
        (*keyval) = keys->fkeyvals[i];
        strncpy(keycom, keys->fkeycoms[i], HPIC_STRNL);
        return 0;
      }
    }
    (*keyval) = 0.0;
    strncpy(keycom, "", HPIC_STRNL);
    return 1;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }
}

/* read keys */

int hpic_keys_read(hpic_keys * keys, fitsfile * fp, int *ret)
{
  size_t i, j;
  int nread = 0;
  int rec = 0;
  size_t nexc = 21;
  char **exclist;
  char **inclist;
  char card[HPIC_STRNL];
  char keyval[HPIC_STRNL];
  char strval[HPIC_STRNL];
  char keycom[HPIC_STRNL];
  char keytype;
  char keyname[HPIC_STRNL];
  int keylen;

  exclist = hpic_strarr_alloc(nexc);
  inclist = hpic_strarr_alloc(1);

  /*exclude required keywords */
  strcpy(exclist[0], "XTENSION");
  strcpy(exclist[1], "BITPIX");
  strcpy(exclist[2], "NAXIS*");
  strcpy(exclist[3], "PCOUNT");
  strcpy(exclist[4], "GCOUNT");
  strcpy(exclist[5], "TFIELDS");
  strcpy(exclist[6], "TTYPE*");
  strcpy(exclist[7], "TFORM*");
  strcpy(exclist[8], "TUNIT*");
  strcpy(exclist[9], "EXTNAME");
  strcpy(exclist[10], "PIXTYPE");
  strcpy(exclist[11], "ORDERING");
  strcpy(exclist[12], "NSIDE");
  strcpy(exclist[13], "COORDSYS");
  strcpy(exclist[14], "INDXSCHM");
  strcpy(exclist[15], "GRAIN");
  strcpy(exclist[16], "COMMENT");
  strcpy(exclist[17], "TBCOL*");
  strcpy(exclist[18], "SIMPLE");
  strcpy(exclist[19], "EXTEND");
  strcpy(exclist[19], "CREATOR");
  strcpy(inclist[0], "*");

  (*ret) = 0;
  if (fits_read_record(fp, rec, card, ret)) {
    fitserr((*ret), "hpic_keys_read:  rewinding key pointer");
  }
  while (!fits_find_nextkey(fp, inclist, 1, exclist, (int)nexc, card, ret)) {
    fits_get_keyname(card, keyname, &keylen, ret);
    fits_parse_value(card, keyval, keycom, ret);
    fits_get_keytype(keyval, &keytype, ret);
    switch (keytype) {
    case 'I':
      hpic_keys_iadd(keys, keyname, atoi(keyval), keycom);
      break;
    case 'F':
      hpic_keys_fadd(keys, keyname, (float)atof(keyval), keycom);
      break;
    default:
      hpic_keys_sadd(keys, keyname, keyval, keycom);
      break;
    }
    nread++;
  }
  (*ret = 0);

  hpic_strarr_free(exclist, nexc);
  hpic_strarr_free(inclist, 1);

  return nread;
}

/* write keys */

int hpic_keys_write(hpic_keys * keys, fitsfile * fp, int *ret)
{
  size_t i, j;
  int nwrit = 0;

  for (i = 0; i < (keys->nskeys); i++) {
    if (fits_write_key
        (fp, TSTRING, keys->skeynames[i], keys->skeyvals[i],
         keys->skeycoms[i], ret)) {
      fitserr((*ret), "hpic_keys_write:  writing string key");
    }
    nwrit++;
  }
  for (i = 0; i < (keys->nikeys); i++) {
    if (fits_write_key
        (fp, TINT, keys->ikeynames[i], &(keys->ikeyvals[i]),
         keys->ikeycoms[i], ret)) {
      fitserr((*ret), "hpic_keys_write:  writing integer key");
    }
    nwrit++;
  }
  for (i = 0; i < (keys->nfkeys); i++) {
    if (fits_write_key
        (fp, TFLOAT, keys->fkeynames[i], &(keys->fkeyvals[i]),
         keys->fkeycoms[i], ret)) {
      fitserr((*ret), "hpic_keys_write:  writing float key");
    }
    nwrit++;
  }
  return nwrit;
}

/* info printing */

int hpic_keys_fprintf(FILE * fp, hpic_keys * keys)
{
  size_t i;
  if (keys) {
    fprintf(fp, "# %d Optional Keys\n", hpic_keys_total(keys));
    for (i = 0; i < (keys->nskeys); i++) {
      fprintf(fp, "# %20s = %20s  / %25s\n", keys->skeynames[i],
              keys->skeyvals[i], keys->skeycoms[i]);
    }
    for (i = 0; i < (keys->nikeys); i++) {
      fprintf(fp, "# %20s = %20d  / %25s\n", keys->ikeynames[i],
              keys->ikeyvals[i], keys->ikeycoms[i]);
    }
    for (i = 0; i < (keys->nfkeys); i++) {
      fprintf(fp, "# %20s = %20e  / %25s\n", keys->fkeynames[i],
              keys->fkeyvals[i], keys->fkeycoms[i]);
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "keys pointer is NULL");
  }
}

int hpic_keys_printf(hpic_keys * keys)
{
  return hpic_keys_fprintf(stdout, keys);
}
