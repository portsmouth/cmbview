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
* Dynamic memory allocation routines.                                        *
*                                                                            *    
*****************************************************************************/

#include "memory.h"

void memerror(char errortxt[])
{
	fprintf(stderr,"memory error: %s\n",errortxt);
	exit(EXIT_FAILURE);
}

float *vector(long nlow, long nhigh)
{
	float *v;
	v=(float *)malloc( (size_t) (nhigh-nlow+1)*sizeof(float) );
	if (!v) memerror("failure in vector()");
	return v-nlow;
}

int *ivector(long nlow, long nhigh)
{
	int *v;
	v=(int *)malloc( (size_t) (nhigh-nlow+1)*sizeof(int) );
	if (!v) memerror("failure in ivector()");
	return v-nlow;
}

unsigned char *cvector(long nlow, long nhigh)
{
	unsigned char *v;
	v=(unsigned char *)malloc( (size_t)(nhigh-nlow+1)*sizeof(unsigned char) );
	if (!v) memerror("failure in cvector()");
	return v-nlow;
}

unsigned long *lvector(long nlow, long nhigh)
{
	unsigned long *v;
	
	v=(unsigned long *)malloc( (size_t)(nhigh-nlow+1)*sizeof(long) );
	if (!v) memerror("failure in lvector()");
	return v-nlow;
}

double *dvector(long nlow, long nhigh)
{
	double *v;
	v=(double *)malloc( (size_t)(nhigh-nlow+1)*sizeof(double) );
	if (!v) memerror("failure in dvector()");
	return v-nlow;
}

float **matrix(long ilr, long ihr, long ilc, long ihc)
{
	long i, Nr=ihr-ilr+1,Nc=ihc-ilc+1;
	float **m;
	
	m=(float **) malloc( (size_t)(Nr)*sizeof(float*) );
	if (!m) memerror("failure A in matrix()");
	m -= ilr;
	m[ilr]=(float *) malloc( (size_t)(Nr*Nc)*sizeof(float) );
	if (!m[ilr]) memerror("failure B in matrix()");
	m[ilr] -= ilc;
	
	for(i=ilr+1;i<=ihr;i++) m[i]=m[i-1]+Nc;
	return m;
}

double **dmatrix(long ilr, long ihr, long ilc, long ihc)
{
	long i, Nr=ihr-ilr+1,Nc=ihc-ilc+1;
	double **m;
	
	m=(double **) malloc( (size_t)(Nr)*sizeof(double*) );
	if (!m) memerror("failure A in dmatrix()");
	m -= ilr;
	m[ilr]=(double *) malloc( (size_t)(Nr*Nc)*sizeof(double) );
	if (!m[ilr]) memerror("failure B in dmatrix()");
	m[ilr] -= ilc;
	
	for(i=ilr+1;i<=ihr;i++) m[i]=m[i-1]+Nc;
	return m;
}

int **imatrix(long ilr, long ihr, long ilc, long ihc)
{
	long i, Nr=ihr-ilr+1,Nc=ihc-ilc+1;
	int **m;
	
	m=(int **) malloc( (size_t)(Nr)*sizeof(int*) );
	if (!m) memerror("failure A in imatrix()");
	m -= ilr;
	m[ilr]=(int *) malloc( (size_t)(Nr*Nc)*sizeof(int) );
	if (!m[ilr]) memerror("failure B in imatrix()");
	m[ilr] -= ilc;
	
	for(i=ilr+1;i<=ihr;i++) m[i]=m[i-1]+Nc;
	return m;
}

float ***f3matrix(long ilr, long ihr, long ilc, long ihc, long ild, long ihd)
{
	long i,j,Nr=ihr-ilr+1,Nc=ihc-ilc+1,Nd=ihd-ild+1;
	float ***t;
	
	t=(float ***) malloc( (size_t)Nr*sizeof(float**) );
	if (!t) memerror("failure A in f3matrix()");
	t -= ilr;
	t[ilr]=(float **) malloc( (size_t)Nr*Nc*sizeof(float*) );
	if (!t[ilr]) memerror("failure B in f3matrix()");
	t[ilr] -= ilc;
	t[ilr][ilc]=(float *) malloc( (size_t)Nr*Nc*Nd*sizeof(float) );
	if (!t[ilr][ilc]) memerror("failure C in f3matrix()");
	t[ilr][ilc] -= ild;
	for(j=ilc+1;j<=ihc;j++) t[ilr][j]=t[ilr][j-1]+Nd;
	for(i=ilr+1;i<=ihr;i++) {
		t[i]=t[i-1]+Nc;
		t[i][ilc]=t[i-1][ilc]+Nc*Nd;
		for(j=ilc+1;j<=ihc;j++) t[i][j]=t[i][j-1]+Nd;
	}
	return t;
}

GLubyte ***glu3matrix(long ilr, long ihr, long ilc, long ihc, long ild, long ihd)
{
	long i,j,Nr=ihr-ilr+1,Nc=ihc-ilc+1,Nd=ihd-ild+1;
	GLubyte ***t;

	t=(GLubyte ***) malloc( (size_t)Nr*sizeof(GLubyte**) );
	if (!t) memerror("failure A in glu3matrix()");
	t -= ilr;
	t[ilr]=(GLubyte **) malloc( (size_t)Nr*Nc*sizeof(GLubyte*) );
	if (!t[ilr]) memerror("failure B in glu3matrix()");
	t[ilr] -= ilc;
	t[ilr][ilc]=(GLubyte *) malloc( (size_t)Nr*Nc*Nd*sizeof(GLubyte) );
	if (!t[ilr][ilc]) memerror("failure C in glu3matrix()");
	t[ilr][ilc] -= ild;
	for(j=ilc+1;j<=ihc;j++) t[ilr][j]=t[ilr][j-1]+Nd;
	for(i=ilr+1;i<=ihr;i++) {
		t[i]=t[i-1]+Nc;
		t[i][ilc]=t[i-1][ilc]+Nc*Nd;
		for(j=ilc+1;j<=ihc;j++) t[i][j]=t[i][j-1]+Nd;
	}
	return t;
}

void free_vector(float *v, long nlow, long nhigh)
{
	free((char*) (v+nlow));
}
void free_ivector(int *v, long nlow, long nhigh)
{
	free((char*) (v+nlow));
}
void free_cvector(unsigned char *v, long nlow, long nhigh)
{
	free((char*) (v+nlow));
}
void free_lvector(unsigned long *v, long nlow, long nhigh)
{
	free((char*) (v+nlow));
}
void free_dvector(double *v, long nlow, long nhigh)
{
	free((char*) (v+nlow));
}
void free_matrix(float **m, long ilr, long ihr, long ilc, long ihc)
{
	free((char*) (m[ilr]+ilc));
	free((char*) (m+ilr));
}
void free_dmatrix(double **m, long ilr, long ihr, long ilc, long ihc)
{
	free((char*) (m[ilr]+ilc));
	free((char*) (m+ilr));
}
void free_imatrix(int **m, long ilr, long ihr, long ilc, long ihc)
{
	free((char*) (m[ilr]+ilc));
	free((char*) (m+ilr));
}
void free_f3matrix(float ***t, long ilr, long ihr, long ilc, long ihc,
				   long ild, long ihd)
{
	free((char*) (t[ilr][ilc]+ild));
	free((char*) (t[ilr]+ilc));
	free((char*) (t+ilr));
}
void free_glu3matrix(GLubyte ***t, long ilr, long ihr, long ilc, long ihc,
				   long ild, long ihd)
{
	free((char*) (t[ilr][ilc]+ild));
	free((char*) (t[ilr]+ilc));
	free((char*) (t+ilr));
}


