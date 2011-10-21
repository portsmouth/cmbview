/*****************************************************************************
* Copyright 2005 Jamie Portsmouth <jamports@mac.com>                         *
*                                                                            *
* This file is part of CMBview, a program for viewing HEALPix-format         *
* CMB data on an OpenGL-rendered 3d sphere.                                  *  
*                                                                            *
* CMBview is free software; you can redistribute it and/or modify            *
* it under the terms of the GNU General Public License as published by       *
* the Free Software Foundation; either version 2 of the License, or          *
* (at your option) any later version.                                        *
*                                                                            *
* CMBview is distributed in the hope that it will be useful,                 * 
* but WITHOUT ANY WARRANTY; without even the implied warranty of             * 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License          * 
* along with CMBview; if not, write to the Free Software                     *
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
*                                                                            *
*****************************************************************************/

#import "CMBview.h"

void memerror(char errortxt[]);

//1d arrays
float *vector(long nlow, long nhigh);
int *ivector(long nlow, long nhigh);
unsigned char *cvector(long nlow, long nhigh);
unsigned long *lvector(long nlow, long nhigh);
double *dvector(long nlow, long nhigh);
void free_vector(float *v, long nlow, long nhigh);
void free_ivector(int *v, long nlow, long nhigh);
void free_cvector(unsigned char *v, long nlow, long nhigh);
void free_lvector(unsigned long *v, long nlow, long nhigh);
void free_dvector(double *v, long nlow, long nhigh);

//2d arrays
float **matrix(long ilr, long ihr, long ilc, long ihc);
double **dmatrix(long ilr, long ihr, long ilc, long ihc);
int **imatrix(long ilr, long ihr, long ilc, long ihc);
void free_matrix(float **m, long ilr, long ihr, long ilc, long ihc);
void free_dmatrix(double **m, long ilr, long ihr, long ilc, long ihc);
void free_imatrix(int **m, long ilr, long ihr, long ilc, long ihc);

//3d arrays
float ***f3matrix(long ilr, long ihr, long ilc, long ihc, long ild, long ihd);
GLubyte ***glu3matrix(long ilr, long ihr, long ilc, long ihc, long ild, long ihd);
void free_f3matrix(float ***t, long ilr, long ihr, long ilc, long ihc, long ild, long ihd);
void free_glu3matrix(GLubyte ***t, long ilr, long ihr, long ilc, long ihc, long ild, long ihd);




