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
 * This code deals with vector operations                                    *
 *****************************************************************************/

#include <hpic.h>

/* alloc/free */

hpic_vec *hpic_vec_alloc(size_t n)
{
  hpic_vec *vec;
  vec = (hpic_vec *) calloc(1, sizeof(hpic_vec));
  if (!vec) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector", NULL);
  }
  vec->n = n;
  if (n != 0) {
    vec->size = n + HPIC_VECBUF;
    vec->data = (double *)calloc(vec->size, sizeof(double));
    if (!(vec->data)) {
      free(vec);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector->data", NULL);
    }
  } else {
    vec->size = 0;
    vec->data = NULL;
  }
  return vec;
}

int hpic_vec_free(hpic_vec * vec)
{
  if (vec) {
    if (vec->data) {
      free(vec->data);
    }
    free(vec);
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "vector not allocated, not freeing");
  }
  return 0;
}

hpic_vec_float *hpic_vec_float_alloc(size_t n)
{
  hpic_vec_float *vec;
  vec = (hpic_vec_float *) calloc(1, sizeof(hpic_vec_float));
  if (!vec) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector", NULL);
  }
  vec->n = n;
  if (n != 0) {
    vec->size = n + HPIC_VECBUF;
    vec->data = (float *)calloc(vec->size, sizeof(float));
    if (!(vec->data)) {
      free(vec);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector->data", NULL);
    }
  } else {
    vec->size = 0;
    vec->data = NULL;
  }
  return vec;
}

int hpic_vec_float_free(hpic_vec_float * vec)
{
  if (vec) {
    if (vec->data) {
      free(vec->data);
    }
    free(vec);
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "vector not allocated, not freeing");
  }
  return 0;
}

hpic_vec_int *hpic_vec_int_alloc(size_t n)
{
  hpic_vec_int *vec;
  vec = (hpic_vec_int *) calloc(1, sizeof(hpic_vec_int));
  if (!vec) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector", NULL);
  }
  vec->n = n;
  if (n != 0) {
    vec->size = n + HPIC_VECBUF;
    vec->data = (int *)calloc(vec->size, sizeof(int));
    if (!(vec->data)) {
      free(vec);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector->data", NULL);
    }
  } else {
    vec->size = 0;
    vec->data = NULL;
  }
  return vec;
}

int hpic_vec_int_free(hpic_vec_int * vec)
{
  if (vec) {
    if (vec->data) {
      free(vec->data);
    }
    free(vec);
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "vector not allocated, not freeing");
  }
  return 0;
}

hpic_vec_index *hpic_vec_index_alloc(size_t n)
{
  hpic_vec_index *vec;
  vec = (hpic_vec_index *) calloc(1, sizeof(hpic_vec_index));
  if (!vec) {
    HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector", NULL);
  }
  vec->n = n;
  if (n != 0) {
    vec->size = n + HPIC_VECBUF;
    vec->data = (size_t *) calloc(vec->size, sizeof(size_t));
    if (!(vec->data)) {
      free(vec);
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector->data", NULL);
    }
  } else {
    vec->size = 0;
    vec->data = NULL;
  }
  return vec;
}

int hpic_vec_index_free(hpic_vec_index * vec)
{
  if (vec) {
    if (vec->data) {
      free(vec->data);
    }
    free(vec);
  } else {
    HPIC_ERROR(HPIC_ERR_FREE, "vector not allocated, not freeing");
  }
  return 0;
}

/* data access */

inline size_t hpic_vec_n_get(hpic_vec * vec)
{
  if (vec) {
    return vec->n;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0);
  }
}

inline size_t hpic_vec_float_n_get(hpic_vec_float * vec)
{
  if (vec) {
    return vec->n;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0);
  }
}

inline size_t hpic_vec_int_n_get(hpic_vec_int * vec)
{
  if (vec) {
    return vec->n;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0);
  }
}

inline size_t hpic_vec_index_n_get(hpic_vec_index * vec)
{
  if (vec) {
    return vec->n;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0);
  }
}

inline int hpic_vec_set(hpic_vec * vec, size_t elem, double val)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      vec->data[elem] = val;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "elem is out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }
}

inline double hpic_vec_get(hpic_vec * vec, size_t elem)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      return vec->data[elem];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "elem is out of range", 0.0);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0.0);
  }  
}

inline int hpic_vec_float_set(hpic_vec_float * vec, size_t elem, float val)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      vec->data[elem] = val;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "elem is out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }
}

inline float hpic_vec_float_get(hpic_vec_float * vec, size_t elem)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      return vec->data[elem];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "elem is out of range", 0.0);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0.0);
  }  
}

inline int hpic_vec_int_set(hpic_vec_int * vec, size_t elem, int val)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      vec->data[elem] = val;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "elem is out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }
}

inline int hpic_vec_int_get(hpic_vec_int * vec, size_t elem)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      return vec->data[elem];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "elem is out of range", 0);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0);
  }  
}

inline int hpic_vec_index_set(hpic_vec_index * vec, size_t elem, size_t val)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      vec->data[elem] = val;
      return 0;
    } else {
      HPIC_ERROR(HPIC_ERR_RANGE, "elem is out of range");
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }  
}

inline size_t hpic_vec_index_get(hpic_vec_index * vec, size_t elem)
{
  if (vec) {
    if ((elem < (vec->n)) && (elem >= 0)) {
      return vec->data[elem];
    } else {
      HPIC_ERROR_VAL(HPIC_ERR_RANGE, "elem is out of range", 0);
    }
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", 0);
  }  
}

/* operations */

inline int hpic_vec_setall(hpic_vec * vec, double val)
{
  size_t i;
  if (vec) {
    for (i = 0; i < (vec->n); i++) {
      vec->data[i] = val;
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }
  return 0;
}

inline int hpic_vec_float_setall(hpic_vec_float * vec, float val)
{
  size_t i;
  if (vec) {
    for (i = 0; i < (vec->n); i++) {
      vec->data[i] = val;
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }
  return 0;
}

inline int hpic_vec_int_setall(hpic_vec_int * vec, int val)
{
  size_t i;
  if (vec) {
    for (i = 0; i < (vec->n); i++) {
      vec->data[i] = val;
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }
  return 0;
}

inline int hpic_vec_index_setall(hpic_vec_index * vec, size_t val)
{
  size_t i;
  if (vec) {
    for (i = 0; i < (vec->n); i++) {
      vec->data[i] = val;
    }
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "vec pointer is NULL");
  }
  return 0;
}

inline hpic_vec *hpic_vec_copy(hpic_vec * vec)
{
  hpic_vec *copy;
  size_t i;
  if (vec) {
    copy = hpic_vec_alloc(vec->n);
    if (!copy) {
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector copy", NULL);
    }
    for (i = 0; i < (vec->n); i++) {
      hpic_vec_set(copy, i, hpic_vec_get(vec, i));
    }
    return copy;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", NULL);
  }
}

inline hpic_vec_float *hpic_vec_float_copy(hpic_vec_float * vec)
{
  hpic_vec_float *copy;
  size_t i;
  if (vec) {
    copy = hpic_vec_float_alloc(vec->n);
    if (!copy) {
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector copy", NULL);
    }
    for (i = 0; i < (vec->n); i++) {
      hpic_vec_float_set(copy, i, hpic_vec_float_get(vec, i));
    }
    return copy;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", NULL);
  }
}

inline hpic_vec_int *hpic_vec_int_copy(hpic_vec_int * vec)
{
  hpic_vec_int *copy;
  size_t i;
  if (vec) {
    copy = hpic_vec_int_alloc(vec->n);
    if (!copy) {
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector copy", NULL);
    }
    for (i = 0; i < (vec->n); i++) {
      hpic_vec_int_set(copy, i, hpic_vec_int_get(vec, i));
    }
    return copy;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", NULL);
  }
}

inline hpic_vec_index *hpic_vec_index_copy(hpic_vec_index * vec)
{
  hpic_vec_index *copy;
  size_t i;
  if (vec) {
    copy = hpic_vec_index_alloc(vec->n);
    if (!copy) {
      HPIC_ERROR_VAL(HPIC_ERR_ALLOC, "cannot allocate vector copy", NULL);
    }
    for (i = 0; i < (vec->n); i++) {
      hpic_vec_index_set(copy, i, hpic_vec_index_get(vec, i));
    }
    return copy;
  } else {
    HPIC_ERROR_VAL(HPIC_ERR_ACCESS, "vec pointer is NULL", NULL);
  }
}

/* resize and append - realloc the vector memory in chunks so */
/* that we don't have to realloc on every resize.             */

int hpic_vec_resize(hpic_vec * vec, size_t newn)
{
  double *newmem;
  size_t n = vec->n;
  size_t newsize;
  size_t i;
  if (vec) {
    if (newn == vec->n) {
      return 0;
    }
    if (newn == 0) {
      if (vec->data) {
        free(vec->data);
      }
      vec->data = NULL;
      vec->n = 0;
      vec->size = 0;
      return 0;
    }
    if ((newn > vec->size) || (vec->size > newn + 2 * HPIC_VECBUF)) {
      newsize = newn + HPIC_VECBUF;
      newmem =
        (double *)realloc((void *)(vec->data), newsize * sizeof(double));
      if (newmem == NULL) {
        HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc vector data");
      } else {
        (vec->data) = newmem;
        (vec->n) = newn;
        (vec->size) = newsize;
        if (newn > n) {
          for (i = n; i < newn; i++) {
            hpic_vec_set(vec, i, 0.0);
          }
        }
      }
    } else {
      vec->n = newn;
      if (newn > n) {
        for (i = n; i < newn; i++) {
          hpic_vec_set(vec, i, 0.0);
        }
      }
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot resize NULL vector");
  }
}

inline int hpic_vec_append(hpic_vec * vec, double val)
{
  size_t n;
  int err;
  if (vec) {
    n = hpic_vec_n_get(vec);
    err = hpic_vec_resize(vec, n + 1);
    if (err) {
      HPIC_ERROR(err, "cannot resize vector for appending");
    }
    hpic_vec_set(vec, n, val);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot append to NULL vector");
  }
}

int hpic_vec_float_resize(hpic_vec_float * vec, size_t newn)
{
  float *newmem;
  size_t n = vec->n;
  size_t newsize;
  size_t i;
  if (vec) {
    if (newn == vec->n) {
      return 0;
    }
    if (newn == 0) {
      if (vec->data) {
        free(vec->data);
      }
      vec->data = NULL;
      vec->n = 0;
      vec->size = 0;
      return 0;
    }
    if ((newn > vec->size) || (vec->size > newn + 2 * HPIC_VECBUF)) {
      newsize = newn + HPIC_VECBUF;
      newmem = (float *)realloc((void *)(vec->data), newsize * sizeof(float));
      if (newmem == NULL) {
        HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc vector data");
      } else {
        (vec->data) = newmem;
        (vec->n) = newn;
        (vec->size) = newsize;
        if (newn > n) {
          for (i = n; i < newn; i++) {
            hpic_vec_float_set(vec, i, 0.0);
          }
        }
      }
    } else {
      vec->n = newn;
      if (newn > n) {
        for (i = n; i < newn; i++) {
          hpic_vec_float_set(vec, i, 0.0);
        }
      }
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot resize NULL vector");
  }
}

inline int hpic_vec_float_append(hpic_vec_float * vec, float val)
{
  size_t n;
  int err;
  if (vec) {
    n = hpic_vec_float_n_get(vec);
    err = hpic_vec_float_resize(vec, n + 1);
    if (err) {
      HPIC_ERROR(err, "cannot resize vector for appending");
    }
    hpic_vec_float_set(vec, n, val);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot append to NULL vector");
  }
}

int hpic_vec_int_resize(hpic_vec_int * vec, size_t newn)
{
  int *newmem;
  size_t n = vec->n;
  size_t newsize;
  size_t i;
  if (vec) {
    if (newn == vec->n) {
      return 0;
    }
    if (newn == 0) {
      if (vec->data) {
        free(vec->data);
      }
      vec->data = NULL;
      vec->n = 0;
      vec->size = 0;
      return 0;
    }
    if ((newn > vec->size) || (vec->size > newn + 2 * HPIC_VECBUF)) {
      newsize = newn + HPIC_VECBUF;
      newmem = (int *)realloc((void *)(vec->data), newsize * sizeof(int));
      if (newmem == NULL) {
        HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc vector data");
      } else {
        (vec->data) = newmem;
        (vec->n) = newn;
        (vec->size) = newsize;
        if (newn > n) {
          for (i = n; i < newn; i++) {
            hpic_vec_int_set(vec, i, 0);
          }
        }
      }
    } else {
      vec->n = newn;
      if (newn > n) {
        for (i = n; i < newn; i++) {
          hpic_vec_int_set(vec, i, 0);
        }
      }
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot resize NULL vector");
  }
}

inline int hpic_vec_int_append(hpic_vec_int * vec, int val)
{
  size_t n;
  int err;
  if (vec) {
    n = hpic_vec_int_n_get(vec);
    err = hpic_vec_int_resize(vec, n + 1);
    if (err) {
      HPIC_ERROR(err, "cannot resize vector for appending");
    }
    hpic_vec_int_set(vec, n, val);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot append to NULL vector");
  }
}

int hpic_vec_index_resize(hpic_vec_index * vec, size_t newn)
{
  size_t *newmem;
  size_t n = vec->n;
  size_t newsize;
  size_t i;
  if (vec) {
    if (newn == vec->n) {
      return 0;
    }
    if (newn == 0) {
      if (vec->data) {
        free(vec->data);
      }
      vec->data = NULL;
      vec->n = 0;
      vec->size = 0;
      return 0;
    }
    if ((newn > vec->size) || (vec->size > newn + 2 * HPIC_VECBUF)) {
      newsize = newn + HPIC_VECBUF;
      newmem =
        (size_t *) realloc((void *)(vec->data), newsize * sizeof(size_t));
      if (newmem == NULL) {
        HPIC_ERROR(HPIC_ERR_ALLOC, "cannot realloc vector data");
      } else {
        (vec->data) = newmem;
        (vec->n) = newn;
        (vec->size) = newsize;
        if (newn > n) {
          for (i = n; i < newn; i++) {
            hpic_vec_index_set(vec, i, 0);
          }
        }
      }
    } else {
      vec->n = newn;
      if (newn > n) {
        for (i = n; i < newn; i++) {
          hpic_vec_index_set(vec, i, 0);
        }
      }
    }
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot resize NULL vector");
  }
}

inline int hpic_vec_index_append(hpic_vec_index * vec, size_t val)
{
  size_t n;
  int err;
  if (vec) {
    n = hpic_vec_index_n_get(vec);
    err = hpic_vec_index_resize(vec, n + 1);
    if (err) {
      HPIC_ERROR(err, "cannot resize vector for appending");
    }
    hpic_vec_index_set(vec, n, val);
    return 0;
  } else {
    HPIC_ERROR(HPIC_ERR_ACCESS, "cannot append to NULL vector");
  }
}
