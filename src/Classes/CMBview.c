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
* This file contains miscellaneous functions and defines global variables    *
*                                                                            *
*****************************************************************************/

#import "CMBview.h"
#import "memory.h"

/**********************************************************************/
/*                         global variables                           */
/**********************************************************************/

//global pointers to hpic map data
hpic_fltarr* hpic_maps; 			
hpic_float *hpic_Tmap, *hpic_Qmap, *hpic_Umap, *hpic_Nmap;
//these are needed for cut-sky case
hpic_float *cuterrs;
hpic_int *cutpix, *cuthits;
//hpic error
char hpic_errorstr[HPIC_STRNL];
_Bool HPIC_ERROR_FLAG;

//vertex data and textures
GLuint face_texs[6], render_tex;
link triangles_on_face[6];

//global pointers to Stokes vector data
Stokes_ptrs stokes_ptrs;

//observer parameters
Observer obs;

//local coord system for each face of texture cube
Cubecoords cubecoords;

//colormaps
colormap mycolormaps[8];
colormap *current_colormap_ptr;



/**********************************************************************/
/*                          math routines                             */
/**********************************************************************/

inline float scalarprod(float v1[3], float v2[3])
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

inline void normalize(float v[3])
{
	float d = (float)sqrtf( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	if (d==0.0f) 
	{
		printf("ERROR: attempted to normalize a null vector\n");
		exit(EXIT_FAILURE);
	}
	
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;		
}

inline void normalize_double(double v[3])
{
	double d = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	if (d==0.0)
	{
		printf("ERROR: attempted to normalize a null vector\n");
		exit(EXIT_FAILURE);
	}
	
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;		
}

inline void normcrossprod(float v1[3], float v2[3], float out[3])
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
	normalize(out);
}


/**********************************************************************/
/*                          draw routines                             */
/**********************************************************************/

inline void drawtriangle_projected(float *v1, float *v2, float *v3,
								   float tex1[2], float tex2[2], float tex3[2])
{	
	glNormal3fv(v1);		
	glTexCoord2f(tex1[0],tex1[1]);
	glVertex3fv(v1);
	
	glNormal3fv(v2);		
	glTexCoord2f(tex2[0],tex2[1]);
	glVertex3fv(v2);
	
	glNormal3fv(v3);		
	glTexCoord2f(tex3[0],tex3[1]);
	glVertex3fv(v3);	
}

inline void drawtriangle_projected_notex(float *v1, float *v2, float *v3)
{
	glNormal3fv(v1);		
	glVertex3fv(v1);
	
	glNormal3fv(v2);		
	glVertex3fv(v2);
	
	glNormal3fv(v3);		
	glVertex3fv(v3);
}

/* draw a great circle arc from (gc_theta1,gc_phi1) to (gc_theta2,gc_phi2) with Narc equally spaced vertices */
inline void greatcircle(double gc_theta1, double gc_phi1, double gc_theta2, double gc_phi2, int Narc)
{
	int i,n_gc;
	double gc_vec1[3],gc_vec2[3],gc_vec[3];
	double gc_rad,gc_lambda,gc_arc,gc_mu,gc_norm;
	
	gc_rad = (double)radius*(1.0+0.5*obs.axisspace);
	
	gc_vec1[0] = sin(gc_theta1)*cos(gc_phi1);
	gc_vec1[1] = sin(gc_theta1)*sin(gc_phi1);
	gc_vec1[2] = cos(gc_theta1);
	
	gc_vec2[0] = sin(gc_theta2)*cos(gc_phi2);
	gc_vec2[1] = sin(gc_theta2)*sin(gc_phi2);
	gc_vec2[2] = cos(gc_theta2);
	
	gc_mu = 0.0;
	for (i=0;i<3;i++)
	{
		gc_mu += gc_vec1[i]*gc_vec2[i];
	}
	
	for (n_gc=0;n_gc<Narc;n_gc++)
	{
		gc_arc = (double)n_gc*fabs(acos(gc_mu))/((double)Narc-1);
		gc_lambda = fabs(cos(gc_arc));
		for (i=0;i<3;i++) 
		{			
			gc_vec[i] = (gc_lambda-gc_mu) * gc_vec1[i] - (gc_lambda-1.0) * gc_vec2[i];				
		}
		
		gc_norm = 0.0;
		for (i=0;i<3;i++) 
		{			
			gc_norm += gc_vec[i]*gc_vec[i];
		}
		gc_norm = sqrt(gc_norm);
		
		for (i=0;i<3;i++)
		{			
			gc_vec[i] *= gc_rad/gc_norm;
		}
		glVertex3d(gc_vec[0],gc_vec[1],gc_vec[2]);
	}	
}

/* draw an arc of constant latitude lat_theta approximated as a series of Narc
   vertices equally spaced in phi in interval (lat_phi1, lat_phi2). */
inline void latitude(double lat_theta, double lat_phi1, double lat_phi2, int Narc)
{
	double gc_rad,lat_z,lat_plane,phiarc,dphiarc;
	int n_lat;
	
	gc_rad = radius*(1.0+0.5*obs.axisspace);
	lat_plane = gc_rad*sin(lat_theta);	
	lat_z = gc_rad*cos(lat_theta);	
	
	dphiarc = (lat_phi2-lat_phi1) / ((double)Narc-1);
	for (n_lat=0;n_lat<Narc;n_lat++)
	{
		phiarc = lat_phi1 + dphiarc*(double)n_lat;
		glVertex3d(lat_plane*cos(phiarc),lat_plane*sin(phiarc),lat_z);
	}		
}

/**********************************************************************/
/*                   mesh geometry and projection                     */
/**********************************************************************/

/* define vectors which orient local coord system for each cube face */
void initcoordsystem(void) 
{		
	const float xvecs[6][3] = {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0},
	                           {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, { 0.0,-1.0, 0.0}};
	
	const float yvecs[6][3] = {{0.0, 1.0, 0.0}, {0.0,-1.0, 0.0}, { 0.0, 0.0, 1.0},
						       {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, { 0.0, 0.0, 1.0}};
	
	const float zvecs[6][3] = {{0.0, 0.0, 1.0}, {0.0, 0.0,-1.0}, { 0.0, 1.0, 0.0},
	                           {0.0,-1.0, 0.0}, {1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}};
	
	int i,j;
	for (i=0;i<6;i++)
	{
		for (j=0;j<3;j++)
		{		
			cubecoords.local_x[i][j] = xvecs[i][j];
			cubecoords.local_y[i][j] = yvecs[i][j];
			cubecoords.local_z[i][j] = zvecs[i][j];	
		}
	}	
}

/* generates the sphere mesh vertices and the corresponding
   texture coordinates on the circumscribed cube */
void genvertexcoords(void) 
{
	int i,j,facenum;
	float vert1[3], vert2[3], vert3[3];
	float tex1[3], tex2[3], tex3[3];
	float theta1,theta2,phi1,phi2,dtheta,dphi;
	
	dtheta = PI/((float)Ntheta+1);
	dphi = 2.0*PI/((float)Nphi);
	
	link tlist[6];
	
	//set up pointers for linked lists we are about to populate
	for (facenum=0;facenum<6;facenum++)
	{
		//these nodes point to the head of the linked lists for each face;
		//they merely provide an entry into the linked list and contain no data:
		triangles_on_face[facenum] = malloc(sizeof *triangles_on_face[facenum]);
		
		//set the current pointer to point initially to the heads: 
		tlist[facenum] = triangles_on_face[facenum];		
	}
	
	/* top cap */	
	for (j=0; j<Nphi; j++)
	{
		vert1[0] = 0.0f;
		vert1[1] = 0.0f;
		vert1[2] = (float)radius;
		
		vert2[0] = (float)radius*sin(dtheta)*cos(j*dphi);
		vert2[1] = (float)radius*sin(dtheta)*sin(j*dphi);
		vert2[2] = (float)radius*cos(dtheta);
		
		vert3[0] = (float)radius*sin(dtheta)*cos((j+1)*dphi);
		vert3[1] = (float)radius*sin(dtheta)*sin((j+1)*dphi);
		vert3[2] = (float)radius*cos(dtheta);
		
		projecttocube(vert1,&facenum,tex1);
		projecttoface(vert2,facenum,tex2);
		projecttoface(vert3,facenum,tex3);	
		addtriangle(&tlist[facenum],vert1,vert2,vert3,tex1,tex2,tex3);		
	}
	
	/* bottom cap */
	for (j=0; j<Nphi; j++) 
	{		
		vert1[0] = 0.0f;
		vert1[1] = 0.0f;
		vert1[2] = -(float)radius;
		
		vert2[0] = (float)radius*sin(PI-dtheta)*cos(j*dphi);
		vert2[1] = (float)radius*sin(PI-dtheta)*sin(j*dphi);
		vert2[2] = (float)radius*cos(PI-dtheta);
		
		vert3[0] = (float)radius*sin(PI-dtheta)*cos((j+1)*dphi);
		vert3[1] = (float)radius*sin(PI-dtheta)*sin((j+1)*dphi);
		vert3[2] = (float)radius*cos(PI-dtheta);
		
		projecttocube(vert1,&facenum,tex1);
		projecttoface(vert2,facenum,tex2);		
		projecttoface(vert3,facenum,tex3);		
		addtriangle(&tlist[facenum],vert1,vert2,vert3,tex1,tex2,tex3);		
	}
	
	/* bulk of sphere */
	for (i=1; i<Ntheta+1; i++) 
	{
		for (j=0; j<Nphi; j++)
		{
			
			theta1 = (float)i*dtheta;
			phi1 = (float)j*dphi;
			theta2 = (float)(i+1)*dtheta;
			phi2 = (float)(j+1)*dphi;
			
			/* triangle 1 */
			vert1[0] = (float)radius*sin(theta1)*cos(phi1);
			vert1[1] = (float)radius*sin(theta1)*sin(phi1);
			vert1[2] = (float)radius*cos(theta1);
			
			vert2[0] = (float)radius*sin(theta2)*cos(phi1);
			vert2[1] = (float)radius*sin(theta2)*sin(phi1);
			vert2[2] = (float)radius*cos(theta2);
			
			vert3[0] = (float)radius*sin(theta2)*cos(phi2);
			vert3[1] = (float)radius*sin(theta2)*sin(phi2);
			vert3[2] = (float)radius*cos(theta2);
			
			projecttocube(vert1,&facenum,tex1);
			projecttoface(vert2,facenum,tex2);
			projecttoface(vert3,facenum,tex3);					
			addtriangle(&tlist[facenum],vert1,vert2,vert3,tex1,tex2,tex3);		
			
			/* triangle 2 */
			vert1[0] = (float)radius*sin(theta1)*cos(phi1);
			vert1[1] = (float)radius*sin(theta1)*sin(phi1);
			vert1[2] = (float)radius*cos(theta1);
			
			vert2[0] = (float)radius*sin(theta2)*cos(phi2);
			vert2[1] = (float)radius*sin(theta2)*sin(phi2);
			vert2[2] = (float)radius*cos(theta2);
			
			vert3[0] = (float)radius*sin(theta1)*cos(phi2);
			vert3[1] = (float)radius*sin(theta1)*sin(phi2);
			vert3[2] = (float)radius*cos(theta1);
			
			projecttocube(vert1,&facenum,tex1);
			projecttoface(vert2,facenum,tex2);
			projecttoface(vert3,facenum,tex3);								
			addtriangle(&tlist[facenum],vert1,vert2,vert3,tex1,tex2,tex3);		
			
		}
	}
}

void addtriangle(link *tlink, 
				 float *v1, float *v2, float *v3,
				 float *t1, float *t2, float *t3)
{
	int a;	
	*tlink = ((*tlink)->next = malloc(sizeof **tlink));
	for (a=0;a<3;a++) 
	{
		(*tlink)->vertex1[a] = v1[a];
		(*tlink)->vertex2[a] = v2[a];
		(*tlink)->vertex3[a] = v3[a];
	}
	for (a=0;a<2;a++) 
	{
		(*tlink)->texcoords1[a] = t1[a];
		(*tlink)->texcoords2[a] = t2[a];
		(*tlink)->texcoords3[a] = t3[a];
	}
	
	float d1[3],d2[3],norm[3];
	for (a=0;a<3;a++) 
	{
		d1[a] = v2[a]-v1[a];
		d2[a] = v3[a]-v2[a];			
	}
	normcrossprod(d1,d2,norm);
	for (a=0;a<3;a++)
	{
		(*tlink)->normal[a] = norm[a];
	}
	
	(*tlink)->next = NULL;
}

/* project from sphere to texture cube. First argument is (unit) vector 
   location on sphere. Returns the index of the face on which projected point lies, 
   and the texture coordinates of the intersection on the face.
   Note the cube faces "overhang" to avoid having to deal with triangles which
   project onto a cube edge or corner. */
void projecttocube(float *onsphere, int *facenum, float *tex)
{
	int face, nearest_face,i;
	float mu, d_intersect, faceside, dmin;
	float facenormal[3], xlocal[3], ylocal[3], intersection[3][6];
	float xintersect, yintersect, nearest_intersection[3];
	
	//dimensions of the (overhanging) cube faces
	faceside = 2.0f*(1.0f+flange);
	int firsttime = 1;

	//step through faces
	for (face=0;face<6;face++)
	{	
		for (i=0;i<3;i++) 
		{
			facenormal[i] = cubecoords.local_z[face][i];
		}	
	
		//compute scalar product of face normal and unit location vector.
		//if this is less than or equal to zero, we can rule out this face index.			
		mu = scalarprod(onsphere,facenormal);
		if (mu>0.0f) 
		{
			d_intersect = 0.0f;
			for (i=0;i<3;i++) 
			{
				intersection[i][face] = onsphere[i]/mu;				
				d_intersect += intersection[i][face]*intersection[i][face];
			}
			if (firsttime || d_intersect<dmin)
			{
				dmin = d_intersect;
				nearest_face = face;						
				firsttime = 0;
			}
		}
	}		
	*facenum = nearest_face;

	for (i=0;i<3;i++) 
	{
		nearest_intersection[i] = intersection[i][nearest_face];
		xlocal[i] = cubecoords.local_x[nearest_face][i];
		ylocal[i] = cubecoords.local_y[nearest_face][i];
	}	
	xintersect = scalarprod(nearest_intersection,xlocal);
	yintersect = scalarprod(nearest_intersection,ylocal);	
	
	//corresponding (u,v) texture coords
	tex[0] = xintersect/faceside+0.5f;
	tex[1] = yintersect/faceside+0.5f;
	if ((tex[0]<0.0f || tex[0]>1.0f) || (tex[1]<0.0f || tex[1]>1.0f)) 
	{
		printf("ERROR: texture coords. out of range in projecttocube (u,v): %f %f\n",tex[0],tex[1]);
		exit(EXIT_FAILURE);
	}	
}

/* project from sphere to a specified face of the texture cube. 
   This function projects vertices of triangles which have already been assigned 
   to a texture cube face (since the first vertex was projected to that face).
   First argument is (unit) vector location on sphere. Second argument is the 
   desired face. Returns the texture coordinates of the intersection on the face */
void projecttoface(float *onsphere, int whichface, float *tex)
{
	int face,nearest_face,i;
	float mu,d_intersect,faceside,dmin;
	float facenormal[3],xlocal[3],ylocal[3],intersection[3][6];
	float xintersect,yintersect,nearest_intersection[3];
	
	//dimensions of the (overhanging) cube faces
	faceside = 2.0f*(1.0f+flange);
	for (i=0;i<3;i++) 
	{
		facenormal[i] = cubecoords.local_z[whichface][i];
	}	
	
	//compute scalar product of face normal and unit location vector.
	mu = scalarprod(onsphere,facenormal);
	if (mu>0.0f)
	{
		for (i=0;i<3;i++)
		{
			nearest_intersection[i] = onsphere[i]/mu;
		}
	}
	else
	{
		printf("something seriously wrong with vertices. See projecttoface\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=0;i<3;i++) 
	{
		xlocal[i] = cubecoords.local_x[whichface][i];
		ylocal[i] = cubecoords.local_y[whichface][i];
	}	
	xintersect = scalarprod(nearest_intersection,xlocal);
	yintersect = scalarprod(nearest_intersection,ylocal);
	
	//corresponding (u,v) texture coords
	tex[0] = xintersect/faceside+0.5f;
	tex[1] = yintersect/faceside+0.5f;
	if ((tex[0]<0.0f || tex[0]>1.0f) || (tex[1]<0.0f || tex[1]>1.0f)) 
	{
		printf("texture coords. out of range in projecttoface (u,v): %f %f\n",tex[0],tex[1]);
		exit(EXIT_FAILURE);
	}	
}

/* projects texel centers on the circumscribed cube onto the sphere */
inline void cubetexel_to_sphere(int Ntexture, int a, int b, int face, double *theta_proj, double *phi_proj)
{
	int c;
	float dl_tex,cubepos[3];		
	
	//actual size of texels on face
	dl_tex = 2.0f*(1.0f+flange)/((float)Ntexture);
	
	for(c=0;c<3;c++) 
	{
		// location of texel center on cube
		cubepos[c] = cubecoords.local_z[face][c];
		cubepos[c] += cubecoords.local_x[face][c] * ( dl_tex*(0.5f+(float)b) - (1.0f+flange) );
		cubepos[c] += cubecoords.local_y[face][c] * ( dl_tex*(0.5f+(float)a) - (1.0f+flange) );										
	}
	
	// project this texel center radially onto sphere
	normalize(cubepos);
	*theta_proj = acos((double)cubepos[2]);
	*phi_proj = atan2((double)cubepos[1],(double)cubepos[0]);
}


/**********************************************************************/
/*                       color computations                           */
/**********************************************************************/

/* convert from HSV color values to RGB values (all in range [0,1]) */
inline void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if (s==0.0f) 
	{
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	if (h==1.0f)
	{
		i = 5;
		h = 6.0f;  
	}
	else
	{
		h *= 6.0f;  
		i = (int)floor((double)h);
	}
	f = h - (float)i;                      
	p = v * (1.0f - s);
	q = v * (1.0f - s*f);
	t = v * (1.0f - s*(1.0f-f));
	switch(i)
	{
		case 0:  *r = v; *g = t; *b = p; break;
		case 1:  *r = q; *g = v; *b = p; break;
		case 2:  *r = p; *g = v; *b = t; break;
		case 3:  *r = p; *g = q; *b = v; break;
		case 4:  *r = t; *g = p; *b = v; break;
		default: *r = v; *g = p; *b = q; break;
	}
}

inline void colortable(int c, int samples, int channels, float map_scalars[samples], 
					   float map_values[samples][channels], float **cm_scalars, float ***cm_values)
{
	int i,j;	
	for (i=0;i<samples;i++) 
	{
		cm_scalars[c][i] = map_scalars[i];
		for (j=0;j<channels;j++)
		{
			cm_values[c][i][j] = map_values[i][j];
		}
	}
}

/* discover where x lies in an ordered table tbl[] */
inline void findbin(float tbl[], unsigned long n, float x, unsigned long *j)
{
	unsigned long ju,jm,jl;
	int step;
	
	jl=0;
	ju=n+1;
	step = (tbl[n] >= tbl[1]);
	while (ju-jl > 1) 
	{
		jm=(ju+jl) >> 1;
		if ((x >= tbl[jm]) == step)
			jl=jm;
		else
			ju=jm;
	}
	if (x == tbl[1]) *j=1;
	else if(x == tbl[n]) *j=n-1;
	else *j=jl;
}


/* linearly interpolate between endpoints in HSV color space */
inline void colorpath(int n, float *xi, float **ci, float x, float *c)
{
	//find out in which interval x lies
	unsigned long j;
	findbin(xi-1, n, x, &j); 
	j -= 1;
	
	//find interpolated HSV value
	int a;
	float step = (xi[j+1] - xi[j]);
	float histep = (xi[j+1]-x)/step;
	float lostep = (x-xi[j])/step;
	for (a=0;a<3;a++) 
	{
		c[a] =  histep*ci[j][a] + lostep*ci[j+1][a];
	}
}

/* Colormaps are defined by specifying HSV triples at points (samples) 
   on the interval [0,1], and linearly interpolating HSV between these points. 
   In each colormap structure, 1-d array color_xi contains the sample positions, 
   and 2-d array color_ci contains the HSV triples at each sample. */
void define_colormaps(void)
{	
	int i,j; 	
	enum colormaps cm;
	
	const int MAX_NUMBER_OF_COLORMAPS = 8;
	const int MAX_COLORMAP_SAMPLES = 10;
	
	float **CM_SCALARS = matrix(0, MAX_NUMBER_OF_COLORMAPS, 0, MAX_COLORMAP_SAMPLES);
	float ***CM_VALUES = f3matrix(0, MAX_NUMBER_OF_COLORMAPS, 0, MAX_COLORMAP_SAMPLES, 0, 3);
	
	for (cm = hsv; cm<=winter; cm++) 
	{	
		for (i=0;i<MAX_COLORMAP_SAMPLES;i++) 
		{
			CM_SCALARS[cm][i] = 0.0f;
			for (j=0;j<3;j++)
			{
				CM_VALUES[cm][i][j] = 0.0f;
			}
		}
	}
	
	//hsv colormap
	cm = hsv;
	mycolormaps[cm].color_N = 2;		
	float HSV_SCALARS[2] = {0.0f, 1.0f};
	float HSV_VALUES[2][3] = {{0.0f, 1.0f, 1.0f}, {0.936f, 1.0f, 1.0f}};		
	colortable(cm, mycolormaps[cm].color_N, 3 , HSV_SCALARS ,HSV_VALUES, CM_SCALARS, CM_VALUES);
	
	//jet colormap
	cm = jet;
	mycolormaps[cm].color_N = 6;	
	float JET_SCALARS[6] = {0.0f, 0.3f, 0.36f, 0.55f, 0.75f, 1.0f};
	float JET_VALUES[6][3] = 
	{{ 0.68f,   1.0f,  0.7f },
	 { 0.5f,    1.0f,  1.0f },
	 { 0.456f,  0.55f, 0.95f},
	 { 0.1666f, 0.6f,  1.0f },
	 { 0.0444f, 1.0f,  1.0f },
	 { 0.0f,    1.0f,  0.52f}};			
	colortable(cm, mycolormaps[cm].color_N, 3, JET_SCALARS, JET_VALUES, CM_SCALARS, CM_VALUES);
	
	//hot colormap
	cm = hot;
	mycolormaps[cm].color_N = 4;	
	float HOT_SCALARS[4] = {0.0f, 0.3333f, 0.6666f, 1.0f};
	float HOT_VALUES[4][3] = 
	{{ 0.0f,    0.0f, 0.0f },
	 { 0.0f,    1.0f, 1.0f },
	 { 0.1666f, 1.0f, 1.0f },
	 { 0.1666f, 0.0f, 1.0f }};			
	colortable(cm,mycolormaps[cm].color_N, 3, HOT_SCALARS, HOT_VALUES, CM_SCALARS, CM_VALUES);
	
	//cool colormap
	cm = cool;
	mycolormaps[cm].color_N = 3;	
	float COOL_SCALARS[3] = {0.0f, 0.5f, 1.0f};
	float COOL_VALUES[3][3] = 
	{{ 0.5f,    1.0f,  1.0f },
	 { 0.636f,  0.55f, 1.0f },
	 { 0.8333f, 1.0f,  1.0f }};
	colortable(cm, mycolormaps[cm].color_N, 3, COOL_SCALARS, COOL_VALUES, CM_SCALARS, CM_VALUES);
	
	//copper colormap
	cm = copper;
	mycolormaps[cm].color_N = 3;	
	float COPPER_SCALARS[3] = {0.0f, 0.5f, 1.0f};
	float COPPER_VALUES[3][3] = 
	{{ 0.0f,     0.67f, 0.05f },
	 { 0.06888f, 0.62f, 0.68f },
	 { 0.09444f, 0.52f, 1.0f  }};
	colortable(cm, mycolormaps[cm].color_N, 3, COPPER_SCALARS, COPPER_VALUES, CM_SCALARS, CM_VALUES);
	
	//negative grey colormap
	cm = neg;
	mycolormaps[cm].color_N = 2;	
	float NEG_SCALARS[2] = {0.0f, 1.0f};
	float NEG_VALUES[2][3] = 
	{{ 0.0f, 0.0f, 1.0f },
	 { 0.0f, 0.0f, 0.0f }};
	colortable(cm, mycolormaps[cm].color_N, 3, NEG_SCALARS, NEG_VALUES, CM_SCALARS, CM_VALUES);
	
	//bone colormap
	cm = bone;
	mycolormaps[cm].color_N = 3;	
	float BONE_SCALARS[3] = {0.0f, 0.5f, 1.0f};
	float BONE_VALUES[3][3] = 
	{{ 0.6666f, 1.0f,  0.03f },
	 { 0.625f,  0.24f, 0.55f },
	 { 0.0f,    0.0f,  1.0f  }};
	colortable(cm, mycolormaps[cm].color_N, 3, BONE_SCALARS, BONE_VALUES, CM_SCALARS, CM_VALUES);
	
	//winter colormap
	cm = winter;
	mycolormaps[cm].color_N = 5;	
	float WINTER_SCALARS[5] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
	float WINTER_VALUES[5][3] = 
	{{ 0.6666f, 1.0f, 0.0f },
	 { 0.6666f, 1.0f, 0.5f },
	 { 0.5444f, 1.0f, 0.7f },
	 { 0.4194f, 1.0f, 1.0f },
	 { 0.4194f, 0.0f, 1.0f }};
	colortable(cm, mycolormaps[cm].color_N, 3, WINTER_SCALARS, WINTER_VALUES, CM_SCALARS, CM_VALUES);
	
	for (cm = hsv; cm<=winter; cm++)
	{	
		mycolormaps[cm].color_xi = vector(0,mycolormaps[cm].color_N-1);
		mycolormaps[cm].color_ci = matrix(0,mycolormaps[cm].color_N-1,0,2);
		
		for (i=0;i<mycolormaps[cm].color_N;i++) 
		{
			mycolormaps[cm].color_xi[i] = CM_SCALARS[cm][i];
			for (j=0;j<3;j++)
			{
				mycolormaps[cm].color_ci[i][j] = CM_VALUES[cm][i][j];
			}
		}
	}
	
	free_matrix(CM_SCALARS, 0, MAX_NUMBER_OF_COLORMAPS, 0, MAX_COLORMAP_SAMPLES);
	free_f3matrix(CM_VALUES, 0, MAX_NUMBER_OF_COLORMAPS, 0, MAX_COLORMAP_SAMPLES, 0, 3);
}

