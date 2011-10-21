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
*                                                                            *
* This is the main header file for CMBview.                                  * 
*                                                                            *
*****************************************************************************/

#import <stdio.h>
#import <stdlib.h>
#import <stddef.h>
#import <stdbool.h>
#import <math.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <GLUT/glut.h>
#import "hpic.h"

/* useful defs */

#define PI M_PI
static float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
static float maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
				   (maxarg1) : (maxarg2))

/* hard-coded */

#define radius 1.0

//number of polygons in sphere mesh
#define Ntheta 64
#define Nphi (2*Ntheta)

//fraction by which each cube face overhangs 
#define flange 0.15

//number of bins for histogram view
#define Nbin 256

/* typedefs and global variable externs */

//vertices and texture coord data
typedef struct meshtriangle *link;
typedef struct meshtriangle
{
	float vertex1[3], vertex2[3], vertex3[3]; 
	float texcoords1[2], texcoords2[2], texcoords3[2];
	float normal[3];
	link next;	
} meshtriangle;

extern link triangles_on_face[6];
extern GLuint face_texs[6], render_tex;

//Stokes vector data
typedef struct
{
	float ***Stokes_headless, **Stokes_headless_mag;
	double **Stokes_theta_proj, **Stokes_phi_proj;
	int **Stokes_mask;	
} Stokes_ptrs;
extern Stokes_ptrs stokes_ptrs;
enum stokesproperties {stokes_color,stokes_opacity,stokes_thickness,
	                   stokes_length,stokes_convention};

//local coord system for each face of texture cube
typedef struct 
{
	float local_x[6][3];
	float local_y[6][3];
	float local_z[6][3];
} Cubecoords;
extern Cubecoords cubecoords;

//colormap variables
typedef struct 
{
	int color_N;
	float *color_xi;
	float **color_ci;
} colormap;
extern colormap mycolormaps[8];
extern colormap *current_colormap_ptr;
enum colormaps {hsv, jet, hot, cool, copper, neg, bone, winter};

//grid properties
enum gridproperties {grid_color,grid_enable,grid_opacity,grid_thickness,gridincaps};

//global pointers to hpic data
extern hpic_fltarr* hpic_maps; 	
extern hpic_float *hpic_Tmap, *hpic_Qmap, *hpic_Umap, *hpic_Nmap;
extern hpic_float *cuterrs;
extern hpic_int *cutpix, *cuthits;
extern char hpic_errorstr[HPIC_STRNL];
extern _Bool HPIC_ERROR_FLAG;

//observer parameters
typedef struct
{
	float aspect_ratio,near,far,frustum_width,frustum_height;
	float view_width,view_height,axisspace;
	float eyedistance, eyedistance_perspective, eyedistance_ortho;
	float view_direction[3],view_point[3],localy[3],localx[3];
} Observer;
extern Observer obs;

//structures to organize histogram data and view:

//map maxima
typedef struct
{
	float maxT; float minT;
	float maxQ; float minQ;
	float maxU; float minU;
	float maxP; float minP;
} mapmaxima;

//view coordinates of the widgets for each of the maps
typedef struct
{
	float maxT; float minT;
	float maxQ; float minQ;
	float maxU; float minU;
	float maxP; float minP;
} widgets_coords;

//view coordinates of the colorrange limits for each of the maps
typedef struct
{
	float maxT; float minT;
	float maxQ; float minQ;
	float maxU; float minU;
	float maxP; float minP;
} colorrange_coords;

//colorrange limits in map dimensions
typedef struct colorrange 
{
	float maxT; float minT;
	float maxQ; float minQ;
	float maxU; float minU;
	float maxP; float minP;
} colorrange;

//histogram data
typedef struct Histogram 
{
	float Thist[Nbin]; float normThist[Nbin]; float normThistlog[Nbin];
	float Qhist[Nbin]; float normQhist[Nbin]; float normQhistlog[Nbin];
	float Uhist[Nbin]; float normUhist[Nbin]; float normUhistlog[Nbin];
	float Phist[Nbin]; float normPhist[Nbin]; float normPhistlog[Nbin];	
} histogram;


/* function declarations */

//math routines
float scalarprod(float v1[3], float v2[3]);
void normalize(float v[3]);
void normalize_double(double v[3]);
void normcrossprod(float v1[3], float v2[3], float out[3]);

//draw routines
void drawtriangle_projected(float *v1, float *v2, float *v3,
							float tex1[2], float tex2[2], float tex3[2]);
void drawtriangle_projected_notex(float *v1, float *v2, float *v3);
void greatcircle(double gc_theta1,double gc_phi1,
				 double gc_theta2,double gc_phi2,int Narc);
void latitude(double lat_theta,double lat_phi1,double lat_phi2,int Narc);

//mesh geometry and projection
void initcoordsystem(void);
void genvertexcoords(void);
void addtriangle(link *tlink,float *v1, float *v2, float *v3,
				             float *t1, float *t2, float *t3);
void projecttocube(float *onsphere, int *facenum, float *tex);
void projecttoface(float *onsphere, int whichface, float *tex);
void cubetexel_to_sphere(int Ntexture, int a, int b, int face, double *theta_proj, double *phi_proj);

//color computations
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
void colorpath(int n, float *xi, float **ci, float x, float *c);
void findbin(float xx[], unsigned long n, float x, unsigned long *j);
void define_colormaps(void);
void colortable(int c, int samples, int channels, 
				float map_scalars[samples], float map_values[samples][channels],
				float **cm_scalars, float ***cm_values);



