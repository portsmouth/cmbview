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
 * This code deals with error handling                                       *
 *****************************************************************************/

#include <hpic.h>
#include <hpic_config.h>

#ifdef HAVE_LIBPTHREAD
#  include <pthread.h>
#endif

/* set default error handler */

hpic_error_handler_t* hpic_set_error_handler(hpic_error_handler_t *newhandler) {
  hpic_error_handler_t *oldhandler;
# ifdef HAVE_LIBPTHREAD  
  static pthread_mutex_t errlock = PTHREAD_MUTEX_INITIALIZER;        
  pthread_mutex_lock(&errlock);
# endif  
  oldhandler = hpic_error_handler;
  hpic_error_handler = newhandler;
# ifdef HAVE_LIBPTHREAD  
  pthread_mutex_unlock(&errlock);
# endif
  return oldhandler;
}

/* default error handler - print message and abort */

void hpic_error_default(int errcode, const char *file, int line, const char *msg) {
  char errorstr[HPIC_STRNL];
  
  switch (errcode) {
    case 0:
      return;
      break;
    case HPIC_ERR_ALLOC:
      strcpy(errorstr, "Memory allocation error");
      break;
    case HPIC_ERR_FREE:
      strcpy(errorstr, "Memory freeing error");
      break;
    case HPIC_ERR_NSIDE:
      strcpy(errorstr, "Illegal NSIDE value");
      break;
    case HPIC_ERR_ORDER:
      strcpy(errorstr, "Illegal ORDER value");
      break;
    case HPIC_ERR_COORD:
      strcpy(errorstr, "Illegal COORDINATE value");
      break;
    case HPIC_ERR_RANGE:
      strcpy(errorstr, "Value is out of range");
      break;
    case HPIC_ERR_ACCESS:
      strcpy(errorstr, "Memory is not accessible");
      break;
    case HPIC_ERR_PROJ:
      strcpy(errorstr, "Projection error");
      break;
    case HPIC_ERR_FITS:
      strcpy(errorstr, "FITS error");
      break;
    default:
      fprintf(stderr,"Unknown error code");
      return;
      break;
  }
  fprintf(stderr, "HPIC ERROR %d:  %s in file %s, line %d -- %s\n", errcode, errorstr, file, line, msg);
  return;
}

void hpic_error(int errcode, const char *file, int line, const char *msg) {
  if (errcode == 0) {
    return;
  }
  /* call user-defined error handler if it exists */
  if (hpic_error_handler) {
    (*hpic_error_handler) (errcode, file, line, msg);
    return;
  }
  hpic_error_default(errcode, file, line, msg);
  
  abort();
}
   


