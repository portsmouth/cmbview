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
 * This code deals with nested tree structures used for high resolution      *
 * partial-sphere maps.  This code is only used internally and has           *
 * no public interface declared in the main header file.  Because of this,   *
 * some parameter checks (such as checking for valid nside values) have      *
 * been left out.                                                            *
 *****************************************************************************/

#ifndef HPIC_TREE_H
#  define HPIC_TREE_H

hpic_node *hpic_node_add();
int hpic_node_del(hpic_node * node);
void hpic_leaf_clear(hpic_node * root, size_t nside);
int hpic_leaf_set(hpic_node * root, size_t pix, size_t shift, size_t * nnodes, double val);
void hpic_leaf_float_set(hpic_node * root, size_t pix, size_t shift, size_t * nnodes, float val);
void hpic_leaf_int_set(hpic_node * root, size_t pix, size_t shift, size_t * nnodes, int val);
double hpic_leaf_get(hpic_node * root, size_t pix, size_t shift);
float hpic_leaf_float_get(hpic_node * root, size_t pix, size_t shift);
int hpic_leaf_int_get(hpic_node * root, size_t pix, size_t shift);
hpic_tree *hpic_tree_alloc(size_t nside);
void hpic_tree_free(hpic_tree * tree);
void hpic_tree_clear(hpic_tree * tree);
void hpic_tree_set(hpic_tree * tree, size_t pix, double val);
void hpic_tree_float_set(hpic_tree * tree, size_t pix, float val);
void hpic_tree_int_set(hpic_tree * tree, size_t pix, int val);
double hpic_tree_get(hpic_tree * tree, size_t pix);
float hpic_tree_float_get(hpic_tree * tree, size_t pix);
int hpic_tree_int_get(hpic_tree * tree, size_t pix);
size_t hpic_tree_thresh(size_t nside, size_t elemsize);
void hpic_tree_fromvec(hpic_tree * tree, double *data);
void hpic_tree_tovec(hpic_tree * tree, double *data);



/* node operations */

hpic_node *hpic_node_add()
{
  hpic_node *newnode;
  newnode = (hpic_node *) calloc(1, sizeof(hpic_node));
  if (!newnode) {
    fprintf(stderr, "hpic_node_add:  Cannot allocate node!\n");
    return NULL;
  }
  newnode->child00 = NULL;
  newnode->child01 = NULL;
  newnode->child10 = NULL;
  newnode->child11 = NULL;
  return newnode;
}

int hpic_node_del(hpic_node * node)
{
  if ((node->child00) || (node->child01) || (node->child10) ||
      (node->child11)) {
    fprintf(stderr,
            "hpic_node_del:  not all children are NULL, cannot delete node\n");
    return HPIC_ERR_FREE;
  } else {
    free(node);
  }
  return 0;
}

/* leaf operations */

void hpic_leaf_clear(hpic_node * root, size_t nside)
{
  if ((nside == 1) || (!root)) {
    return;
  }
  if (nside == 2) {
    if (root->child00) {
      free(root->child00);
      root->child00 = NULL;
    }
    if (root->child01) {
      free(root->child01);
      root->child01 = NULL;
    }
    if (root->child10) {
      free(root->child10);
      root->child10 = NULL;
    }
    if (root->child11) {
      free(root->child11);
      root->child11 = NULL;
    }
    hpic_node_del(root);
  } else {
    if (root->child00) {
      hpic_leaf_clear((hpic_node *) (root->child00), (size_t) (nside / 2));
      root->child00 = NULL;
    }
    if (root->child01) {
      hpic_leaf_clear((hpic_node *) (root->child01), (size_t) (nside / 2));
      root->child01 = NULL;
    }
    if (root->child10) {
      hpic_leaf_clear((hpic_node *) (root->child10), (size_t) (nside / 2));
      root->child10 = NULL;
    }
    if (root->child11) {
      hpic_leaf_clear((hpic_node *) (root->child11), (size_t) (nside / 2));
      root->child11 = NULL;
    }
    hpic_node_del(root);
  }
  return;
}

int hpic_leaf_set(hpic_node * root, size_t pix, size_t shift, size_t * nnodes, double val)
{
  if (!root) {
    if (hpic_is_dnull(val)) {
      return 0;
    }
    root = hpic_node_add();
    (*nnodes)++;
  }
  if (shift == 0) {
    if ((pix & 3) == 0) {
      if (hpic_is_dnull(val)) {      /*delete this leaf, if it exists */
        if (root->child00) {
          free(root->child00);
          root->child00 = NULL;
        }
      } else {
        if (!(root->child00)) { /*create new leaf if necessary */
          root->child00 = calloc(1, sizeof(double));
          if (!(root->child00)) {
            fprintf(stderr, "hpic_leaf_set:  cannot allocate leaf 0\n");
            return HPIC_ERR_ALLOC;
          }
        }
        *((double *)(root->child00)) = val;
      }
    }
    if ((pix & 3) == 1) {
      if (hpic_is_dnull(val)) {      /*delete this leaf, if it exists */
        if (root->child01) {
          free(root->child01);
          root->child01 = NULL;
        }
      } else {
        if (!(root->child01)) { /*create new leaf if necessary */
          root->child01 = calloc(1, sizeof(double));
          if (!(root->child01)) {
            fprintf(stderr, "hpic_leaf_set:  cannot allocate leaf 1\n");
            return HPIC_ERR_ALLOC;
          }
        }
        *((double *)(root->child01)) = val;
      }
    }
    if ((pix & 3) == 2) {
      if (hpic_is_dnull(val)) {      /*delete this leaf, if it exists */
        if (root->child10) {
          free(root->child10);
          root->child10 = NULL;
        }
      } else {
        if (!(root->child10)) { /*create new leaf if necessary */
          root->child10 = calloc(1, sizeof(double));
          if (!(root->child10)) {
            fprintf(stderr, "hpic_leaf_set:  cannot allocate leaf 2\n");
            return HPIC_ERR_ALLOC;
          }
        }
        *((double *)(root->child10)) = val;
      }
    }
    if ((pix & 3) == 3) {
      if (hpic_is_dnull(val)) {      /*delete this leaf, if it exists */
        if (root->child11) {
          free(root->child11);
          root->child11 = NULL;
        }
      } else {
        if (!(root->child11)) { /*create new leaf if necessary */
          root->child11 = calloc(1, sizeof(double));
          if (!(root->child11)) {
            fprintf(stderr, "hpic_leaf_set:  cannot allocate leaf 3\n");
            exit(1);
          }
        }
        *((double *)(root->child11)) = val;
      }
    }
  } else {                      /*allocate child nodes as needed and call recursively */
    if (((pix >> shift) & 3) == 0) {
      if (!(root->child00)) {
        if (hpic_is_dnull(val)) {
          return;
        }
        root->child00 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_set((hpic_node *) (root->child00), pix, shift, nnodes, val);
      if (hpic_is_dnull(val)) {
        if ((!((hpic_node *) (root->child00))->child00) &&
            (!((hpic_node *) (root->child00))->child01) &&
            (!((hpic_node *) (root->child00))->child10) &&
            (!((hpic_node *) (root->child00))->child11)) {
          hpic_node_del((hpic_node *) root->child00);
          root->child00 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 1) {
      if (!(root->child01)) {
        if (hpic_is_dnull(val)) {
          return;
        }
        root->child01 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_set((hpic_node *) (root->child01), pix, shift, nnodes, val);
      if (hpic_is_dnull(val)) {
        if ((!((hpic_node *) (root->child01))->child00) &&
            (!((hpic_node *) (root->child01))->child01) &&
            (!((hpic_node *) (root->child01))->child10) &&
            (!((hpic_node *) (root->child01))->child11)) {
          hpic_node_del((hpic_node *) root->child01);
          root->child01 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 2) {
      if (!(root->child10)) {
        if (hpic_is_dnull(val)) {
          return;
        }
        root->child10 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_set((hpic_node *) (root->child10), pix, shift, nnodes, val);
      if (hpic_is_dnull(val)) {
        if ((!((hpic_node *) (root->child10))->child00) &&
            (!((hpic_node *) (root->child10))->child01) &&
            (!((hpic_node *) (root->child10))->child10) &&
            (!((hpic_node *) (root->child10))->child11)) {
          hpic_node_del((hpic_node *) root->child10);
          root->child10 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 3) {
      if (!(root->child11)) {
        if (hpic_is_dnull(val)) {
          return;
        }
        root->child11 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_set((hpic_node *) (root->child11), pix, shift, nnodes, val);
      if (hpic_is_dnull(val)) {
        if ((!((hpic_node *) (root->child11))->child00) &&
            (!((hpic_node *) (root->child11))->child01) &&
            (!((hpic_node *) (root->child11))->child10) &&
            (!((hpic_node *) (root->child11))->child11)) {
          hpic_node_del((hpic_node *) root->child11);
          root->child11 = NULL;
          (*nnodes)--;
        }
      }
    }
  }

  return 0;
}

void hpic_leaf_float_set(hpic_node * root, size_t pix, size_t shift, size_t * nnodes, float val)
{
  if (!root) {
    if (hpic_is_fnull(val)) {
      return;
    }
    root = hpic_node_add();
    (*nnodes)++;
  }
  if (shift == 0) {
    if ((pix & 3) == 0) {
      if (hpic_is_fnull(val)) {      /*delete this leaf, if it exists */
        if (root->child00) {
          free(root->child00);
          root->child00 = NULL;
        }
      } else {
        if (!(root->child00)) { /*create new leaf if necessary */
          root->child00 = calloc(1, sizeof(float));
          if (!(root->child00)) {
            fprintf(stderr, "hpic_leaf_float_set:  cannot allocate leaf 0\n");
            exit(1);
          }
        }
        *((float *)(root->child00)) = val;
      }
    }
    if ((pix & 3) == 1) {
      if (hpic_is_fnull(val)) {      /*delete this leaf, if it exists */
        if (root->child01) {
          free(root->child01);
          root->child01 = NULL;
        }
      } else {
        if (!(root->child01)) { /*create new leaf if necessary */
          root->child01 = calloc(1, sizeof(float));
          if (!(root->child01)) {
            fprintf(stderr, "hpic_leaf_float_set:  cannot allocate leaf 1\n");
            exit(1);
          }
        }
        *((float *)(root->child01)) = val;
      }
    }
    if ((pix & 3) == 2) {
      if (hpic_is_fnull(val)) {      /*delete this leaf, if it exists */
        if (root->child10) {
          free(root->child10);
          root->child10 = NULL;
        }
      } else {
        if (!(root->child10)) { /*create new leaf if necessary */
          root->child10 = calloc(1, sizeof(float));
          if (!(root->child10)) {
            fprintf(stderr, "hpic_leaf_float_set:  cannot allocate leaf 2\n");
            exit(1);
          }
        }
        *((float *)(root->child10)) = val;
      }
    }
    if ((pix & 3) == 3) {
      if (hpic_is_fnull(val)) {      /*delete this leaf, if it exists */
        if (root->child11) {
          free(root->child11);
          root->child11 = NULL;
        }
      } else {
        if (!(root->child11)) { /*create new leaf if necessary */
          root->child11 = calloc(1, sizeof(float));
          if (!(root->child11)) {
            fprintf(stderr, "hpic_leaf_float_set:  cannot allocate leaf 3\n");
            exit(1);
          }
        }
        *((float *)(root->child11)) = val;
      }
    }
  } else {                      /*allocate child nodes as needed and call recursively */
    if (((pix >> shift) & 3) == 0) {
      if (!(root->child00)) {
        if (hpic_is_fnull(val)) {
          return;
        }
        root->child00 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_float_set((hpic_node *) (root->child00), pix, shift, nnodes, val);
      if (hpic_is_fnull(val)) {
        if ((!((hpic_node *) (root->child00))->child00) &&
            (!((hpic_node *) (root->child00))->child01) &&
            (!((hpic_node *) (root->child00))->child10) &&
            (!((hpic_node *) (root->child00))->child11)) {
          hpic_node_del((hpic_node *) root->child00);
          root->child00 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 1) {
      if (!(root->child01)) {
        if (hpic_is_fnull(val)) {
          return;
        }
        root->child01 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_float_set((hpic_node *) (root->child01), pix, shift, nnodes, val);
      if (hpic_is_fnull(val)) {
        if ((!((hpic_node *) (root->child01))->child00) &&
            (!((hpic_node *) (root->child01))->child01) &&
            (!((hpic_node *) (root->child01))->child10) &&
            (!((hpic_node *) (root->child01))->child11)) {
          hpic_node_del((hpic_node *) root->child01);
          root->child01 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 2) {
      if (!(root->child10)) {
        if (hpic_is_fnull(val)) {
          return;
        }
        root->child10 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_float_set((hpic_node *) (root->child10), pix, shift, nnodes, val);
      if (hpic_is_fnull(val)) {
        if ((!((hpic_node *) (root->child10))->child00) &&
            (!((hpic_node *) (root->child10))->child01) &&
            (!((hpic_node *) (root->child10))->child10) &&
            (!((hpic_node *) (root->child10))->child11)) {
          hpic_node_del((hpic_node *) root->child10);
          root->child10 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 3) {
      if (!(root->child11)) {
        if (hpic_is_fnull(val)) {
          return;
        }
        root->child11 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_float_set((hpic_node *) (root->child11), pix, shift, nnodes, val);
      if (hpic_is_fnull(val)) {
        if ((!((hpic_node *) (root->child11))->child00) &&
            (!((hpic_node *) (root->child11))->child01) &&
            (!((hpic_node *) (root->child11))->child10) &&
            (!((hpic_node *) (root->child11))->child11)) {
          hpic_node_del((hpic_node *) root->child11);
          root->child11 = NULL;
          (*nnodes)--;
        }
      }
    }
  }

  return;
}

void hpic_leaf_int_set(hpic_node * root, size_t pix, size_t shift, size_t * nnodes, int val)
{
  if (!root) {
    if (hpic_is_inull(val)) {
      return;
    }
    root = hpic_node_add();
    (*nnodes)++;
  }
  if (shift == 0) {
    if ((pix & 3) == 0) {
      if (hpic_is_inull(val)) {      /*delete this leaf, if it exists */
        if (root->child00) {
          free(root->child00);
          root->child00 = NULL;
        }
      } else {
        if (!(root->child00)) { /*create new leaf if necessary */
          root->child00 = calloc(1, sizeof(int));
          if (!(root->child00)) {
            fprintf(stderr, "hpic_leaf_int_set:  cannot allocate leaf 0\n");
            exit(1);
          }
        }
        *((int *)(root->child00)) = val;
      }
    }
    if ((pix & 3) == 1) {
      if (hpic_is_inull(val)) {      /*delete this leaf, if it exists */
        if (root->child01) {
          free(root->child01);
          root->child01 = NULL;
        }
      } else {
        if (!(root->child01)) { /*create new leaf if necessary */
          root->child01 = calloc(1, sizeof(int));
          if (!(root->child01)) {
            fprintf(stderr, "hpic_leaf_int_set:  cannot allocate leaf 1\n");
            exit(1);
          }
        }
        *((int *)(root->child01)) = val;
      }
    }
    if ((pix & 3) == 2) {
      if (hpic_is_inull(val)) {      /*delete this leaf, if it exists */
        if (root->child10) {
          free(root->child10);
          root->child10 = NULL;
        }
      } else {
        if (!(root->child10)) { /*create new leaf if necessary */
          root->child10 = calloc(1, sizeof(int));
          if (!(root->child10)) {
            fprintf(stderr, "hpic_leaf_int_set:  cannot allocate leaf 2\n");
            exit(1);
          }
        }
        *((int *)(root->child10)) = val;
      }
    }
    if ((pix & 3) == 3) {
      if (hpic_is_inull(val)) {      /*delete this leaf, if it exists */
        if (root->child11) {
          free(root->child11);
          root->child11 = NULL;
        }
      } else {
        if (!(root->child11)) { /*create new leaf if necessary */
          root->child11 = calloc(1, sizeof(int));
          if (!(root->child11)) {
            fprintf(stderr, "hpic_leaf_int_set:  cannot allocate leaf 3\n");
            exit(1);
          }
        }
        *((int *)(root->child11)) = val;
      }
    }
  } else {                      /*allocate child nodes as needed and call recursively */
    if (((pix >> shift) & 3) == 0) {
      if (!(root->child00)) {
        if (hpic_is_inull(val)) {
          return;
        }
        root->child00 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_int_set((hpic_node *) (root->child00), pix, shift, nnodes, val);
      if (hpic_is_inull(val)) {
        if ((!((hpic_node *) (root->child00))->child00) &&
            (!((hpic_node *) (root->child00))->child01) &&
            (!((hpic_node *) (root->child00))->child10) &&
            (!((hpic_node *) (root->child00))->child11)) {
          hpic_node_del((hpic_node *) root->child00);
          root->child00 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 1) {
      if (!(root->child01)) {
        if (hpic_is_inull(val)) {
          return;
        }
        root->child01 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_int_set((hpic_node *) (root->child01), pix, shift, nnodes, val);
      if (hpic_is_inull(val)) {
        if ((!((hpic_node *) (root->child01))->child00) &&
            (!((hpic_node *) (root->child01))->child01) &&
            (!((hpic_node *) (root->child01))->child10) &&
            (!((hpic_node *) (root->child01))->child11)) {
          hpic_node_del((hpic_node *) root->child01);
          root->child01 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 2) {
      if (!(root->child10)) {
        if (hpic_is_inull(val)) {
          return;
        }
        root->child10 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_int_set((hpic_node *) (root->child10), pix, shift, nnodes, val);
      if (hpic_is_inull(val)) {
        if ((!((hpic_node *) (root->child10))->child00) &&
            (!((hpic_node *) (root->child10))->child01) &&
            (!((hpic_node *) (root->child10))->child10) &&
            (!((hpic_node *) (root->child10))->child11)) {
          hpic_node_del((hpic_node *) root->child10);
          root->child10 = NULL;
          (*nnodes)--;
        }
      }
    }
    if (((pix >> shift) & 3) == 3) {
      if (!(root->child11)) {
        if (hpic_is_inull(val)) {
          return;
        }
        root->child11 = (void *)hpic_node_add();
        (*nnodes)++;
      }
      shift -= 2;
      hpic_leaf_int_set((hpic_node *) (root->child11), pix, shift, nnodes, val);
      if (hpic_is_inull(val)) {
        if ((!((hpic_node *) (root->child11))->child00) &&
            (!((hpic_node *) (root->child11))->child01) &&
            (!((hpic_node *) (root->child11))->child10) &&
            (!((hpic_node *) (root->child11))->child11)) {
          hpic_node_del((hpic_node *) root->child11);
          root->child11 = NULL;
          (*nnodes)--;
        }
      }
    }
  }

  return;
}

double hpic_leaf_get(hpic_node * root, size_t pix, size_t shift)
{
  if (!root) {
    return HPIC_NULL;
  }
  if (shift == 0) {
    if ((pix & 3) == 0) {
      if (root->child00) {      /*if leaf does not exist, return null */
        return (*((double *)(root->child00)));
      } else {
        return HPIC_NULL;
      }
    }
    if ((pix & 3) == 1) {
      if (root->child01) {      /*if leaf does not exist, return null */
        return (*((double *)(root->child01)));
      } else {
        return HPIC_NULL;
      }
    }
    if ((pix & 3) == 2) {
      if (root->child10) {      /*if leaf does not exist, return null */
        return (*((double *)(root->child10)));
      } else {
        return HPIC_NULL;
      }
    }
    if ((pix & 3) == 3) {
      if (root->child11) {      /*if leaf does not exist, return null */
        return (*((double *)(root->child11)));
      } else {
        return HPIC_NULL;
      }
    }
  } else {                      /*descend to next level, if allocated */
    if (((pix >> shift) & 3) == 0) {
      if (root->child00) {
        shift -= 2;
        return hpic_leaf_get((hpic_node *) (root->child00), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
    if (((pix >> shift) & 3) == 1) {
      if (root->child01) {
        shift -= 2;
        return hpic_leaf_get((hpic_node *) (root->child01), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
    if (((pix >> shift) & 3) == 2) {
      if (root->child10) {
        shift -= 2;
        return hpic_leaf_get((hpic_node *) (root->child10), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
    if (((pix >> shift) & 3) == 3) {
      if (root->child11) {
        shift -= 2;
        return hpic_leaf_get((hpic_node *) (root->child11), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
  }
}

float hpic_leaf_float_get(hpic_node * root, size_t pix, size_t shift)
{
  if (!root) {
    return HPIC_NULL;
  }
  if (shift == 0) {
    if ((pix & 3) == 0) {
      if (root->child00) {      /*if leaf does not exist, return null */
        return (*((float *)(root->child00)));
      } else {
        return HPIC_NULL;
      }
    }
    if ((pix & 3) == 1) {
      if (root->child01) {      /*if leaf does not exist, return null */
        return (*((float *)(root->child01)));
      } else {
        return HPIC_NULL;
      }
    }
    if ((pix & 3) == 2) {
      if (root->child10) {      /*if leaf does not exist, return null */
        return (*((float *)(root->child10)));
      } else {
        return HPIC_NULL;
      }
    }
    if ((pix & 3) == 3) {
      if (root->child11) {      /*if leaf does not exist, return null */
        return (*((float *)(root->child11)));
      } else {
        return HPIC_NULL;
      }
    }
  } else {                      /*descend to next level, if allocated */
    if (((pix >> shift) & 3) == 0) {
      if (root->child00) {
        shift -= 2;
        return hpic_leaf_float_get((hpic_node *) (root->child00), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
    if (((pix >> shift) & 3) == 1) {
      if (root->child01) {
        shift -= 2;
        return hpic_leaf_float_get((hpic_node *) (root->child01), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
    if (((pix >> shift) & 3) == 2) {
      if (root->child10) {
        shift -= 2;
        return hpic_leaf_float_get((hpic_node *) (root->child10), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
    if (((pix >> shift) & 3) == 3) {
      if (root->child11) {
        shift -= 2;
        return hpic_leaf_float_get((hpic_node *) (root->child11), pix, shift);
      } else {
        return HPIC_NULL;
      }
    }
  }
}

int hpic_leaf_int_get(hpic_node * root, size_t pix, size_t shift)
{
  if (!root) {
    return HPIC_INT_NULL;
  }
  if (shift == 0) {
    if ((pix & 3) == 0) {
      if (root->child00) {      /*if leaf does not exist, return null */
        return (*((int *)(root->child00)));
      } else {
        return HPIC_INT_NULL;
      }
    }
    if ((pix & 3) == 1) {
      if (root->child01) {      /*if leaf does not exist, return null */
        return (*((int *)(root->child01)));
      } else {
        return HPIC_INT_NULL;
      }
    }
    if ((pix & 3) == 2) {
      if (root->child10) {      /*if leaf does not exist, return null */
        return (*((int *)(root->child10)));
      } else {
        return HPIC_INT_NULL;
      }
    }
    if ((pix & 3) == 3) {
      if (root->child11) {      /*if leaf does not exist, return null */
        return (*((int *)(root->child11)));
      } else {
        return HPIC_INT_NULL;
      }
    }
  } else {                      /*descend to next level, if allocated */
    if (((pix >> shift) & 3) == 0) {
      if (root->child00) {
        shift -= 2;
        return hpic_leaf_int_get((hpic_node *) (root->child00), pix, shift);
      } else {
        return HPIC_INT_NULL;
      }
    }
    if (((pix >> shift) & 3) == 1) {
      if (root->child01) {
        shift -= 2;
        return hpic_leaf_int_get((hpic_node *) (root->child01), pix, shift);
      } else {
        return HPIC_INT_NULL;
      }
    }
    if (((pix >> shift) & 3) == 2) {
      if (root->child10) {
        shift -= 2;
        return hpic_leaf_int_get((hpic_node *) (root->child10), pix, shift);
      } else {
        return HPIC_INT_NULL;
      }
    }
    if (((pix >> shift) & 3) == 3) {
      if (root->child11) {
        shift -= 2;
        return hpic_leaf_int_get((hpic_node *) (root->child11), pix, shift);
      } else {
        return HPIC_INT_NULL;
      }
    }
  }
}

/* tree operations */

hpic_tree *hpic_tree_alloc(size_t nside)
{
  hpic_tree *tree;
  size_t temp;
  tree = (hpic_tree *) calloc(1, sizeof(hpic_tree));
  if (!tree) {
    fprintf(stderr, "hpic_tree_alloc:  Cannot allocate tree!\n");
    exit(1);
  }
  temp = nside;
  tree->shiftroot = 0;
  while (temp > 1) {
    tree->shiftroot += 2;
    temp = (size_t) (temp / 2);
  }
  tree->nnodes = 0;
  tree->shiftroot -= 2;
  tree->nside = nside;
  tree->root00 = NULL;
  tree->root01 = NULL;
  tree->root02 = NULL;
  tree->root03 = NULL;
  tree->root04 = NULL;
  tree->root05 = NULL;
  tree->root06 = NULL;
  tree->root07 = NULL;
  tree->root08 = NULL;
  tree->root09 = NULL;
  tree->root10 = NULL;
  tree->root11 = NULL;
  return tree;
}

void hpic_tree_free(hpic_tree * tree)
{
  if (tree) {
    hpic_leaf_clear(tree->root00, tree->nside);
    hpic_leaf_clear(tree->root01, tree->nside);
    hpic_leaf_clear(tree->root02, tree->nside);
    hpic_leaf_clear(tree->root03, tree->nside);
    hpic_leaf_clear(tree->root04, tree->nside);
    hpic_leaf_clear(tree->root05, tree->nside);
    hpic_leaf_clear(tree->root06, tree->nside);
    hpic_leaf_clear(tree->root07, tree->nside);
    hpic_leaf_clear(tree->root08, tree->nside);
    hpic_leaf_clear(tree->root09, tree->nside);
    hpic_leaf_clear(tree->root10, tree->nside);
    hpic_leaf_clear(tree->root11, tree->nside);
    free(tree);
  } else {
    fprintf(stderr, "hpic_tree_free:  tree not allocated, so not freeing\n");
  }
  return;
}
void hpic_tree_clear(hpic_tree * tree)
{
  if (tree) {
    hpic_leaf_clear(tree->root00, tree->nside);
    hpic_leaf_clear(tree->root01, tree->nside);
    hpic_leaf_clear(tree->root02, tree->nside);
    hpic_leaf_clear(tree->root03, tree->nside);
    hpic_leaf_clear(tree->root04, tree->nside);
    hpic_leaf_clear(tree->root05, tree->nside);
    hpic_leaf_clear(tree->root06, tree->nside);
    hpic_leaf_clear(tree->root07, tree->nside);
    hpic_leaf_clear(tree->root08, tree->nside);
    hpic_leaf_clear(tree->root09, tree->nside);
    hpic_leaf_clear(tree->root10, tree->nside);
    hpic_leaf_clear(tree->root11, tree->nside);
    tree->root00 = NULL;
    tree->root01 = NULL;
    tree->root02 = NULL;
    tree->root03 = NULL;
    tree->root04 = NULL;
    tree->root05 = NULL;
    tree->root06 = NULL;
    tree->root07 = NULL;
    tree->root08 = NULL;
    tree->root09 = NULL;
    tree->root10 = NULL;
    tree->root11 = NULL;
  } else {
    fprintf(stderr,
            "hpic_tree_clear:  tree not allocated, so not clearing\n");
  }
  return;
}

void hpic_tree_set(hpic_tree * tree, size_t pix, double val)
{
  size_t subpix;
  size_t root;
  if (tree) {
    root = (size_t) (pix / (tree->nside * tree->nside));
    subpix = (size_t) (pix % (tree->nside * tree->nside));
    switch (root) {
    case 0:
      hpic_leaf_set(tree->root00, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 1:
      hpic_leaf_set(tree->root01, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 2:
      hpic_leaf_set(tree->root02, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 3:
      hpic_leaf_set(tree->root03, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 4:
      hpic_leaf_set(tree->root04, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 5:
      hpic_leaf_set(tree->root05, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 6:
      hpic_leaf_set(tree->root06, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 7:
      hpic_leaf_set(tree->root07, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 8:
      hpic_leaf_set(tree->root08, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 9:
      hpic_leaf_set(tree->root09, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 10:
      hpic_leaf_set(tree->root10, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 11:
      hpic_leaf_set(tree->root11, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    default:
      fprintf(stderr, "hpic_tree_set:  pixel %lu is out of range\n", pix);
      break;
    }
  } else {
    fprintf(stderr,
            "hpic_tree_set:  tree not allocated, cannot set leaf value\n");
  }
  return;
}

void hpic_tree_float_set(hpic_tree * tree, size_t pix, float val)
{
  size_t subpix;
  size_t root;
  if (tree) {
    root = (size_t) (pix / (tree->nside * tree->nside));
    subpix = (size_t) (pix % (tree->nside * tree->nside));
    switch (root) {
    case 0:
      hpic_leaf_float_set(tree->root00, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 1:
      hpic_leaf_float_set(tree->root01, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 2:
      hpic_leaf_float_set(tree->root02, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 3:
      hpic_leaf_float_set(tree->root03, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 4:
      hpic_leaf_float_set(tree->root04, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 5:
      hpic_leaf_float_set(tree->root05, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 6:
      hpic_leaf_float_set(tree->root06, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 7:
      hpic_leaf_float_set(tree->root07, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 8:
      hpic_leaf_float_set(tree->root08, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 9:
      hpic_leaf_float_set(tree->root09, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 10:
      hpic_leaf_float_set(tree->root10, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 11:
      hpic_leaf_float_set(tree->root11, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    default:
      fprintf(stderr, "hpic_tree_float_set:  pixel %lu is out of range\n",
              pix);
      break;
    }
  } else {
    fprintf(stderr,
            "hpic_tree_float_set:  tree not allocated, cannot set leaf value\n");
  }
  return;
}

void hpic_tree_int_set(hpic_tree * tree, size_t pix, int val)
{
  size_t subpix;
  size_t root;
  if (tree) {
    root = (size_t) (pix / (tree->nside * tree->nside));
    subpix = (size_t) (pix % (tree->nside * tree->nside));
    switch (root) {
    case 0:
      hpic_leaf_int_set(tree->root00, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 1:
      hpic_leaf_int_set(tree->root01, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 2:
      hpic_leaf_int_set(tree->root02, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 3:
      hpic_leaf_int_set(tree->root03, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 4:
      hpic_leaf_int_set(tree->root04, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 5:
      hpic_leaf_int_set(tree->root05, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 6:
      hpic_leaf_int_set(tree->root06, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 7:
      hpic_leaf_int_set(tree->root07, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 8:
      hpic_leaf_int_set(tree->root08, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 9:
      hpic_leaf_int_set(tree->root09, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 10:
      hpic_leaf_int_set(tree->root10, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    case 11:
      hpic_leaf_int_set(tree->root11, subpix, tree->shiftroot, &(tree->nnodes), val);
      break;
    default:
      fprintf(stderr, "hpic_tree_int_set:  pixel %lu is out of range\n", pix);
      break;
    }
  } else {
    fprintf(stderr,
            "hpic_tree_int_set:  tree not allocated, cannot set leaf value\n");
  }
  return;
}

double hpic_tree_get(hpic_tree * tree, size_t pix)
{
  size_t subpix;
  size_t root;
  if (tree) {
    root = (size_t) (pix / (tree->nside * tree->nside));
    subpix = (size_t) (pix % (tree->nside * tree->nside));
    switch (root) {
    case 0:
      return hpic_leaf_get(tree->root00, subpix, tree->shiftroot);
      break;
    case 1:
      return hpic_leaf_get(tree->root01, subpix, tree->shiftroot);
      break;
    case 2:
      return hpic_leaf_get(tree->root02, subpix, tree->shiftroot);
      break;
    case 3:
      return hpic_leaf_get(tree->root03, subpix, tree->shiftroot);
      break;
    case 4:
      return hpic_leaf_get(tree->root04, subpix, tree->shiftroot);
      break;
    case 5:
      return hpic_leaf_get(tree->root05, subpix, tree->shiftroot);
      break;
    case 6:
      return hpic_leaf_get(tree->root06, subpix, tree->shiftroot);
      break;
    case 7:
      return hpic_leaf_get(tree->root07, subpix, tree->shiftroot);
      break;
    case 8:
      return hpic_leaf_get(tree->root08, subpix, tree->shiftroot);
      break;
    case 9:
      return hpic_leaf_get(tree->root09, subpix, tree->shiftroot);
      break;
    case 10:
      return hpic_leaf_get(tree->root10, subpix, tree->shiftroot);
      break;
    case 11:
      return hpic_leaf_get(tree->root11, subpix, tree->shiftroot);
      break;
    default:
      fprintf(stderr, "hpic_tree_get:  pixel %lu is out of range\n", pix);
      break;
    }
  } else {
    fprintf(stderr,
            "hpic_tree_get:  tree not allocated, cannot get leaf value\n");
  }
  return;
}

float hpic_tree_float_get(hpic_tree * tree, size_t pix)
{
  size_t subpix;
  size_t root;
  if (tree) {
    root = (size_t) (pix / (tree->nside * tree->nside));
    subpix = (size_t) (pix % (tree->nside * tree->nside));
    switch (root) {
    case 0:
      return hpic_leaf_float_get(tree->root00, subpix, tree->shiftroot);
      break;
    case 1:
      return hpic_leaf_float_get(tree->root01, subpix, tree->shiftroot);
      break;
    case 2:
      return hpic_leaf_float_get(tree->root02, subpix, tree->shiftroot);
      break;
    case 3:
      return hpic_leaf_float_get(tree->root03, subpix, tree->shiftroot);
      break;
    case 4:
      return hpic_leaf_float_get(tree->root04, subpix, tree->shiftroot);
      break;
    case 5:
      return hpic_leaf_float_get(tree->root05, subpix, tree->shiftroot);
      break;
    case 6:
      return hpic_leaf_float_get(tree->root06, subpix, tree->shiftroot);
      break;
    case 7:
      return hpic_leaf_float_get(tree->root07, subpix, tree->shiftroot);
      break;
    case 8:
      return hpic_leaf_float_get(tree->root08, subpix, tree->shiftroot);
      break;
    case 9:
      return hpic_leaf_float_get(tree->root09, subpix, tree->shiftroot);
      break;
    case 10:
      return hpic_leaf_float_get(tree->root10, subpix, tree->shiftroot);
      break;
    case 11:
      return hpic_leaf_float_get(tree->root11, subpix, tree->shiftroot);
      break;
    default:
      fprintf(stderr, "hpic_tree_float_get:  pixel %lu is out of range\n",
              pix);
      break;
    }
  } else {
    fprintf(stderr,
            "hpic_tree_float_get:  tree not allocated, cannot get leaf value\n");
  }
  return;
}

int hpic_tree_int_get(hpic_tree * tree, size_t pix)
{
  size_t subpix;
  size_t root;
  if (tree) {
    root = (size_t) (pix / (tree->nside * tree->nside));
    subpix = (size_t) (pix % (tree->nside * tree->nside));
    switch (root) {
    case 0:
      return hpic_leaf_int_get(tree->root00, subpix, tree->shiftroot);
      break;
    case 1:
      return hpic_leaf_int_get(tree->root01, subpix, tree->shiftroot);
      break;
    case 2:
      return hpic_leaf_int_get(tree->root02, subpix, tree->shiftroot);
      break;
    case 3:
      return hpic_leaf_int_get(tree->root03, subpix, tree->shiftroot);
      break;
    case 4:
      return hpic_leaf_int_get(tree->root04, subpix, tree->shiftroot);
      break;
    case 5:
      return hpic_leaf_int_get(tree->root05, subpix, tree->shiftroot);
      break;
    case 6:
      return hpic_leaf_int_get(tree->root06, subpix, tree->shiftroot);
      break;
    case 7:
      return hpic_leaf_int_get(tree->root07, subpix, tree->shiftroot);
      break;
    case 8:
      return hpic_leaf_int_get(tree->root08, subpix, tree->shiftroot);
      break;
    case 9:
      return hpic_leaf_int_get(tree->root09, subpix, tree->shiftroot);
      break;
    case 10:
      return hpic_leaf_int_get(tree->root10, subpix, tree->shiftroot);
      break;
    case 11:
      return hpic_leaf_int_get(tree->root11, subpix, tree->shiftroot);
      break;
    default:
      fprintf(stderr, "hpic_tree_int_get:  pixel %lu is out of range\n", pix);
      break;
    }
  } else {
    fprintf(stderr,
            "hpic_tree_int_get:  tree not allocated, cannot get leaf value\n");
  }
  return;
}

/* tree <--> vector conversion */

void hpic_tree_tovec(hpic_tree * tree, double *data)
{
  size_t i;
  for (i = 0; i < (12 * (tree->nside) * (tree->nside)); i++) {
    data[i] = hpic_tree_get(tree, i);
  }
  return;
}

void hpic_tree_fromvec(hpic_tree * tree, double *data)
{
  size_t i;
  for (i = 0; i < (12 * (tree->nside) * (tree->nside)); i++) {
    hpic_tree_set(tree, i, data[i]);
  }
  return;
}

/* tree threshold calculations */

size_t hpic_tree_thresh(size_t nside, size_t elemsize)
{
  size_t npix;
  size_t thresh;

  npix = hpic_nside2npix(nside);
  thresh = (size_t) (npix * elemsize / (sizeof(hpic_node)));
  return thresh;
}


#endif
