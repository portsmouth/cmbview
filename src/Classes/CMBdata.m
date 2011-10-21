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
* This code processes the map data and generates textures and histograms.    *
*                                                                            *
*****************************************************************************/

#import "CMBdata.h"

@implementation CMBdata

- (id)init
{
	//initialize data pointers
	Tface = Qface = Uface = Pface = NULL;
	renderdata = renderdata_export = NULL;
	rendermask = rendermask_export = NULL;
	stokes_ptrs.Stokes_headless = NULL;
	stokes_ptrs.Stokes_headless_mag = NULL;
	stokes_ptrs.Stokes_mask = NULL;
	stokes_ptrs.Stokes_theta_proj = stokes_ptrs.Stokes_phi_proj = NULL;

	[super init];
	if (self) 
	{
		return self;
	}
	else 
	{
		printf("ERROR: failed to init CMBdata");
		exit(EXIT_FAILURE);
	}	
}

- (void)dealloc 
{
	if (Tface) free_f3matrix(Tface,0,5,0,Ntexture-1,0,Ntexture-1);
	if (Qface) free_f3matrix(Qface,0,5,0,Ntexture-1,0,Ntexture-1);
	if (Uface) free_f3matrix(Uface,0,5,0,Ntexture-1,0,Ntexture-1);
	if (Pface) free_f3matrix(Pface,0,5,0,Ntexture-1,0,Ntexture-1);
	[super dealloc];
}

/**********************************************************************/
/*             generate interactive (cubemap) textures                */
/**********************************************************************/

//methods to allocate and deallocate texture memory for cubemaps

- (void)alloc_Ttexture
{
	Tface = f3matrix(0,5,0,Ntexture-1,0,Ntexture-1);
}

- (void)alloc_Qtexture
{
	Qface = f3matrix(0,5,0,Ntexture-1,0,Ntexture-1);
}

- (void)alloc_Utexture
{
	Uface = f3matrix(0,5,0,Ntexture-1,0,Ntexture-1);
}

- (void)alloc_Ptexture
{
	Pface = f3matrix(0,5,0,Ntexture-1,0,Ntexture-1);
}

- (void)dealloc_Ttexture
{
	free_f3matrix(Tface,0,5,0,Ntexture-1,0,Ntexture-1);
	Tface = NULL;
}

- (void)dealloc_Qtexture
{
	free_f3matrix(Qface,0,5,0,Ntexture-1,0,Ntexture-1);
	Qface = NULL;
}

- (void)dealloc_Utexture
{
	free_f3matrix(Uface,0,5,0,Ntexture-1,0,Ntexture-1);
	Uface = NULL;
}

- (void)dealloc_Ptexture
{
	free_f3matrix(Pface,0,5,0,Ntexture-1,0,Ntexture-1);
	Pface = NULL;
}

/* 
   scan through texels on texture cube, projecting back to sphere. 
   Generate texture map data arrays.
*/	

- (void)genTextures_interactive
{		
	//dealloc any interactive textures resident in memory
	if (Tface) 
		[self dealloc_Ttexture];
	if (Qface) 
		[self dealloc_Qtexture];
	if (Uface)
		[self dealloc_Utexture];
	if (Pface)
		[self dealloc_Ptexture];		
	
	//find a new texture level setting from user prefs
	Ntexture = (int)256 * pow( 2, [[NSUserDefaults standardUserDefaults] integerForKey:CMBview_texnumkey] );
	
	//allocate new memory
	if ([myAppController polarisation]==0) 
	{
		[self alloc_Ttexture];	
	}
	else
	{
		[self alloc_Ttexture];	
		[self alloc_Qtexture];	
		[self alloc_Utexture];	
		[self alloc_Ptexture];	
	}
	
	[myAppController setProgressText:@"generating cube-maps..."];			
	
	if ([myAppController polarisation]==0) 
	{
		//TO DO: put this computation (and others similarly) in a separate thread
		[self scancube_T];
	}
	else 
	{
		[self scancube_TQU];
	}

	[self updateTexs_interactive];
	[self makeHistograms_interactive];
}

- (void)updateTexs_interactive
{
	//free up existing texture resources
	glDeleteTextures((GLsizei)6,face_texs);
	
	//create new texture names
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures((GLsizei)6,face_texs);
	
	GLubyte ***texels;
	texels = glu3matrix(0,Ntexture-1,0,Ntexture-1,0,2);
	
	[myOpenGLview makeThisViewCurrentContext];
	colorrange *c = [myAppController colorrange_interactive];
	float currentmax,currentmin;
	
	int map_type;
	map_type = [myAppController maptype];

	switch (map_type) 
	{		
		case 1:			
			currentmax = c->maxT;
			currentmin = c->minT;
			break;
		
		case 2:			
			currentmax = c->maxQ;
			currentmin = c->minQ;			
			break;
		
		case 3:		
			currentmax = c->maxU;
			currentmin = c->minU;
			break;
		
		case 4:			
			currentmax = c->maxP;
			currentmin = c->minP;
			break;
	}
	
	[myAppController setProgressText:@"binding cube-maps to sphere ..."];
	int face, a,b, step;
	float scalar,R,G,B,hue,texelval,color_x[3];
	double progress;
			
	BOOL reverse = [[NSUserDefaults standardUserDefaults] boolForKey:CMBview_colormapreversekey];
	
	for (face=0; face<6; face++)
	{
		for (a=0; a<Ntexture; a++)
		{
			step = (a%(Ntexture/16)==0 || (a==Ntexture-1 && face==5));
			if (step)
			{
				progress = (double)face+((double)a/Ntexture)/6.0;				
				[myAppController setProgressIndicator:progress];
			}	
			
			for (b=0; b<Ntexture; b++) 
			{	
				/* read scalar from CMBdata */
				switch (map_type)
				{		
					case 1:			
						texelval = Tface[face][a][b];
						break;
					
					case 2:		
						texelval = Qface[face][a][b];
						break;
					
					case 3:			
						texelval = Uface[face][a][b];
						break;
					
					case 4:			
						texelval = Pface[face][a][b];
						break;
				}				
				if (currentmax != currentmin)
				{
					scalar = (texelval-currentmin) / (currentmax-currentmin);
				}
				else 
				{
					scalar = 0.5;
				}
				
				/* clamp colors to acceptable range */
				if (scalar>1.0) 
				{
					scalar = 1.0;
				}
				else if (scalar<0.0)
				{
					scalar = 0.0;
				}			
				
				switch ([myAppController colormap_flag])
				{
					case 0 :
						/* greyscale map */		
						R = scalar; G = scalar; B = scalar;
						break;				
					
					case 1 :	
						/* color map */
						if (reverse) scalar = 1.0-scalar;
						colorpath(current_colormap_ptr->color_N,
								  current_colormap_ptr->color_xi,
								  current_colormap_ptr->color_ci,
								  scalar, color_x);	
						HSVtoRGB(&R,&G,&B,color_x[0],color_x[1],color_x[2]);	
				}
				
				texels[a][b][0] = (GLubyte) 255.0*R;
				texels[a][b][1] = (GLubyte) 255.0*G;
				texels[a][b][2] = (GLubyte) 255.0*B;
			}
		}
		
		//bind the texture data for this face to a texture object
		//TO DO: If there is enough memory, why not store each of T,Q,U,P
		//in separate textures which are retained on switching between the
		//maps? Then clicking between T,Q,U,P would be instantaneous after
		//the first time.
		glBindTexture(GL_TEXTURE_2D,face_texs[face]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Ntexture, Ntexture, 0, GL_RGB,GL_UNSIGNED_BYTE, **texels);
	}
		
	free_glu3matrix(texels,0,Ntexture-1,0,Ntexture-1,0,2);
	[myAppController setProgressText:@""];
	[myAppController setProgressIndicator:0.0];
	[myOpenGLview setNeedsDisplay:YES];
}

- (void)scancube_T
{
	int a,b,nside,ordering,face;
	float T, Tmax,Tmin;
	double theta_proj,phi_proj;
	size_t pixnum;
	
	nside = [myAppController map_nside];
	ordering = [myAppController pixelordering];
	
	for (face=0;face<6;face++)
	{		
		for(a=0;a<Ntexture;a++)
		{
			//make progress indicator advance 16 times per face
			if (a%(Ntexture/16)==0 || (a==Ntexture-1 && face==5)) 
			{
				[myAppController setProgressIndicator: ((double)face + ((double)a/Ntexture)) / 6.0];
			}
				
			for(b=0;b<Ntexture;b++)
			{
				cubetexel_to_sphere( Ntexture, a, b, face, &theta_proj, &phi_proj );				
				
				if (ordering==0) 
				{
					heal_ang2pix_ring(nside,theta_proj,phi_proj,&pixnum);					
				}
				else
				{
					heal_ang2pix_nest(nside,theta_proj,phi_proj,&pixnum);					
				}	
				
				T = hpic_float_get(hpic_Tmap,pixnum);
				Tface[face][a][b] = T;
				//update maxima and minima
				if (face==0 && (a==0 && b==0))
				{
					Tmin = T; Tmax = T;
				}
				else
				{
					if (T<Tmin) Tmin=T;
					if (T>Tmax) Tmax=T;
				}
			}
			
		}		
	}
	
	mapmaxima_interactive.maxT = Tmax;
	mapmaxima_interactive.minT = Tmin;
	colorrange c = {Tmax,Tmin,0.0,0.0,0.0,0.0};
	[myAppController setColorrange_interactive:c];
}

- (void)scancube_TQU
{
	int a,b,nside,ordering,face;
	float T,Q,U,P;
	float Tmax,Tmin,Qmax,Qmin,Umax,Umin,Pmax,Pmin;
	double theta_proj,phi_proj;
	size_t pixnum;
				
	nside = [myAppController map_nside];
	ordering = [myAppController pixelordering];
				
	for (face=0;face<6;face++) 
	{				
		for(a=0;a<Ntexture;a++) 
		{
			
			//make progress indicator advance 16 times per face
			if (a%(Ntexture/16)==0 || (a==Ntexture-1 && face==5)) 
			{
				[myAppController setProgressIndicator: ((double)face + ((double)(a-1)/Ntexture)) / 6.0 ];
			}
				
			for(b=0;b<Ntexture;b++) 
			{					
				cubetexel_to_sphere( Ntexture, a, b, face, &theta_proj, &phi_proj );								
				
				if (ordering==0) 
				{
					heal_ang2pix_ring(nside,theta_proj,phi_proj,&pixnum);					
				}
				else 
				{
					heal_ang2pix_nest(nside,theta_proj,phi_proj,&pixnum);					
				}
				
				T = hpic_float_get(hpic_Tmap,pixnum);
				Q = hpic_float_get(hpic_Qmap,pixnum);
				U = hpic_float_get(hpic_Umap,pixnum);					
				P = (float)sqrt((double)Q*Q+U*U);
				
				Tface[face][a][b] = T;
				Qface[face][a][b] = Q;
				Uface[face][a][b] = U;					
				Pface[face][a][b] = P;					
				
				//update maxima and minima
				if (face==0 && (a==0 && b==0)) 
				{
					Tmin = T; Tmax = T;
					Qmin = Q; Qmax = Q;
					Umin = U; Umax = U;
					Pmin = P; Pmax = P;
				} 
				else
				{							
					if (T<Tmin) Tmin=T; if (T>Tmax) Tmax=T;															
					if (Q<Qmin) Qmin=Q; if (Q>Qmax) Qmax=Q;
					if (U<Umin) Umin=U; if (U>Umax) Umax=U;
					if (P<Pmin) Pmin=P; if (P>Pmax) Pmax=P;
				}
				
			}
		}
	}	
	
	mapmaxima_interactive.maxT = Tmax; mapmaxima_interactive.minT = Tmin;
	mapmaxima_interactive.maxQ = Qmax; mapmaxima_interactive.minQ = Qmin;
	mapmaxima_interactive.maxU = Umax; mapmaxima_interactive.minU = Umin;
	mapmaxima_interactive.maxP = Pmax; mapmaxima_interactive.minP = Pmin;
	
	colorrange c = {Tmax,Tmin,Qmax,Qmin,Umax,Umin,Pmax,Pmin};
	[myAppController setColorrange_interactive:c];
}


/**********************************************************************/
/*                     generate Stokes vectors                        */
/**********************************************************************/

- (void)dealloc_stokesdata
{
	if (stokes_ptrs.Stokes_headless) 
	{	
		free_f3matrix(stokes_ptrs.Stokes_headless,0,NStokes,0,NStokes,0,2);
		stokes_ptrs.Stokes_headless = NULL;
	}
	
	if (stokes_ptrs.Stokes_mask)
	{
		free_imatrix(stokes_ptrs.Stokes_mask,0,NStokes,0,NStokes);
		stokes_ptrs.Stokes_mask = NULL;
	}
	
	if (stokes_ptrs.Stokes_headless_mag)
	{
		free_matrix(stokes_ptrs.Stokes_headless_mag,0,NStokes,0,NStokes);
		stokes_ptrs.Stokes_headless_mag = NULL;
	}
	
	if (stokes_ptrs.Stokes_theta_proj)
	{
		free_dmatrix(stokes_ptrs.Stokes_theta_proj,0,NStokes,0,NStokes);
		stokes_ptrs.Stokes_theta_proj = NULL;
	}
	
	if (stokes_ptrs.Stokes_phi_proj)
	{
		free_dmatrix(stokes_ptrs.Stokes_phi_proj,0,NStokes,0,NStokes);
		stokes_ptrs.Stokes_phi_proj = NULL;
	}		
}

//accessor method for OpenGLview to get allocated Stokes dimensions
- (int)NStokes
{
	return NStokes;
}

- (void)genStokes
{
	//free previously allocated Stokes-vector memory
	[self dealloc_stokesdata];

	//number of Stokes vectors per viewport edge from user prefs
	NStokes = [[NSUserDefaults standardUserDefaults] integerForKey:CMBview_stokesgridkey];
		
	stokes_ptrs.Stokes_headless     = f3matrix(0,NStokes,0,NStokes,0,2);
	stokes_ptrs.Stokes_mask         = imatrix(0,NStokes,0,NStokes);
	stokes_ptrs.Stokes_headless_mag = matrix(0,NStokes,0,NStokes);			
	stokes_ptrs.Stokes_theta_proj   = dmatrix(0,NStokes,0,NStokes);
	stokes_ptrs.Stokes_phi_proj     = dmatrix(0,NStokes,0,NStokes);
	
	float **Qproj, **Uproj, **Pproj;
	Qproj = matrix(0,NStokes,0,NStokes);
	Uproj = matrix(0,NStokes,0,NStokes);
	Pproj = matrix(0,NStokes,0,NStokes);
	
	int a,b;
	double scalar_llp, ray[3],lprime[3],wx,wy,raylength,J;
	double sphere_theta,sphere_phi;
	float T,Q,U,P;
	
	[myAppController setProgressText:@"generating Stokes vectors..."];			
	
	size_t pixnum;
	int nside = [myAppController map_nside];
	int ordering = [myAppController pixelordering];
	int Npix = [myAppController Npixels];
	
	//project viewport grid onto sphere
	
	int render = [myAppController render_mode];	
	float fovy_current;
	BOOL ortho_current;
	
	if (render == 1)
	{
		fovy_current = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_fovykey];
		ortho_current = [myOpenGLview orthoEnabledFlag];
	}
	else if (render == 2 || render == 3)
	{
		fovy_current = [myOpenGLview fovy_render];
		ortho_current = [myOpenGLview orthoEnabledFlag_render];
	}	
			
	BOOL intersect_flag;
	
	if (ortho_current)
	{
		obs.frustum_height = (obs.eyedistance_ortho-radius);
	}
	else
	{
		obs.frustum_height = 2.0*obs.near*(float)tan((double)PI*fovy_current/360.0);
	}
	obs.frustum_width = obs.aspect_ratio*obs.frustum_height;
					
	float Pmax;
	int firstpoint = 1;
	double current_x, current_y;
	double orthox0[3];

	//TO DO: an option to distribute vectors evenly over sphere
	//for nicer appearance when viewing full sphere would be nice.
	//maximum number density could be restricted to prevent performance
	//and memory issues, i.e. must use uniform grid when at high zoom
	
	for (a=0;a<NStokes;a++)
	{	
		for (b=0;b<NStokes;b++)
		{	
			current_x = ((double)a+0.5)/(double)NStokes;
			current_y = ((double)b+0.5)/(double)NStokes;
			wx = 0.5*obs.frustum_width*(2.0*current_x-1.0);
			wy = 0.5*obs.frustum_height*(2.0*current_y-1.0);
			
			intersect_flag = NO;
			
			//project through perspective view frustum
			if (!ortho_current) 
			{				
				lprime[0] = obs.near*obs.view_direction[0] 
							+ wx*obs.localx[0] + wy*obs.localy[0];
				lprime[1] = obs.near*obs.view_direction[1] 
					        + wx*obs.localx[1] + wy*obs.localy[1];
				lprime[2] = obs.near*obs.view_direction[2] 
					        + wx*obs.localx[2] + wy*obs.localy[2];
				normalize_double(lprime);
				
				scalar_llp = obs.view_direction[0]*lprime[0]
					         + obs.view_direction[1]*lprime[1]
					         + obs.view_direction[2]*lprime[2];
				
				J = SQR(scalar_llp)-(1.0-SQR(radius/obs.eyedistance_perspective));
				if (J>=0.0) 
				{
					intersect_flag = YES;
					stokes_ptrs.Stokes_mask[a][b] = 1;
					raylength = obs.eyedistance_perspective*(scalar_llp-sqrt(J));		
					ray[0] = obs.view_point[0] + raylength*lprime[0];
					ray[1] = obs.view_point[1] + raylength*lprime[1];
					ray[2] = obs.view_point[2] + raylength*lprime[2];				
					normalize_double(ray);
				}
			}
			//project through orthographic view cuboid
			else 
			{					
				J = SQR(radius)-SQR(wx)-SQR(wy);
				if (J>=0.0)
				{					
					intersect_flag = YES;
					stokes_ptrs.Stokes_mask[a][b] = 1;
					orthox0[0] = obs.view_point[0]  + wx*obs.localx[0] + wy*obs.localy[0];
					orthox0[1] = obs.view_point[1]  + wx*obs.localx[1] + wy*obs.localy[1];
					orthox0[2] = obs.view_point[2]  + wx*obs.localx[2] + wy*obs.localy[2];
					raylength = obs.eyedistance_ortho-sqrt(J);
					
					ray[0] = orthox0[0] + raylength*obs.view_direction[0];
					ray[1] = orthox0[1] + raylength*obs.view_direction[1];
					ray[2] = orthox0[2] + raylength*obs.view_direction[2];				
					normalize_double(ray);
				}									
			}
			
			if (intersect_flag) 
			{								
				sphere_theta = acos(ray[2]);
				sphere_phi = atan2(ray[1],ray[0]);
				
				if (ordering==0)
				{
					heal_ang2pix_ring(nside,sphere_theta,sphere_phi,&pixnum);				
				}
				else
				{
					heal_ang2pix_nest(nside,sphere_theta,sphere_phi,&pixnum);				
				}	
				
				stokes_ptrs.Stokes_theta_proj[a][b] = sphere_theta;
				stokes_ptrs.Stokes_phi_proj[a][b] = sphere_phi;
				
				if (firstpoint)
				{	
					Q = hpic_float_get(hpic_Qmap,pixnum);
					U = hpic_float_get(hpic_Umap,pixnum);
					P = (float)sqrt((double)Q*Q+U*U);					
					Pmax = P;
					Qproj[a][b] = Q;
					Uproj[a][b] = U;
					Pproj[a][b] = P;
					firstpoint = 0;					
				}
				else 
				{
					Q = hpic_float_get(hpic_Qmap,pixnum);
					U = hpic_float_get(hpic_Umap,pixnum);
					P = (float)sqrt((double)Q*Q+U*U);
					if (P>Pmax) Pmax=P;
					Qproj[a][b] = Q;
					Uproj[a][b] = U;
					Pproj[a][b] = P;
				}						
			}
			else 
			{
				stokes_ptrs.Stokes_mask[a][b] = 0;
			}
		}
	}
	
	//construct Stokes vectors at projected grid points
	int ConventionFlag = [myOpenGLview stokesConventionFlag];
	int c;
	float thetas, phis, Stokes_angle, Stokes_xvec[3], Stokes_yvec[3];
	
	for (a=0;a<NStokes;a++)
	{	
		for (b=0;b<NStokes;b++)
		{
			if (stokes_ptrs.Stokes_mask[a][b]) 
			{				
				thetas = stokes_ptrs.Stokes_theta_proj[a][b];
				phis = stokes_ptrs.Stokes_phi_proj[a][b];							
			
				Stokes_xvec[0] = cos(phis)*cos(thetas);
				Stokes_xvec[1] = sin(phis)*cos(thetas);
				Stokes_xvec[2] = -sin(thetas);		
				
				if (!ConventionFlag)
				{
					//HEALPix convention								
					Stokes_yvec[0] = -sin(phis);
					Stokes_yvec[1] = cos(phis);
					Stokes_yvec[2] = 0.0;			
				}
				else 
				{
					//IAU convention								
					Stokes_yvec[0] = sin(phis);
					Stokes_yvec[1] = -cos(phis);
					Stokes_yvec[2] = 0.0;
				}
				
				Stokes_angle = 0.5*(float)atan2((double)Uproj[a][b],(double)Qproj[a][b]);
				
				for(c=0;c<3;c++)
				{
					stokes_ptrs.Stokes_headless[a][b][c] = cos(Stokes_angle)*Stokes_xvec[c] + sin(Stokes_angle)*Stokes_yvec[c];
				}
				
				stokes_ptrs.Stokes_headless_mag[a][b] = Pproj[a][b]/Pmax;							
			}
		}
	}
	
	[myAppController setProgressText:@""];
	[myAppController setProgressIndicator:0.0];
	
	free_matrix(Qproj,0,NStokes,0,NStokes);
	free_matrix(Uproj,0,NStokes,0,NStokes);
	free_matrix(Pproj,0,NStokes,0,NStokes);	
}

/**********************************************************************/
/*             generate rendered/ray-traced textures                  */
/**********************************************************************/

- (void)dealloc_renderdata
{
	if (renderdata)
	{
		free_matrix(renderdata,0,Ntex_render-1,0,Ntex_render-1);
		renderdata = NULL;
	}
	if (rendermask) 
	{
		free_imatrix(rendermask,0,Ntex_render-1,0,Ntex_render-1);
		rendermask = NULL;
	}		
}

//generate pixel accurate texture data
- (void)genTextures_render
{	
	[self dealloc_renderdata];
	
	int viewport_width, viewport_height;	
	NSRect rect = [myOpenGLview bounds];
	viewport_width = rect.size.width;
	viewport_height = rect.size.height;
	
	//determine minimum (power of 2) size of texture required
	Ntex_render = FMAX(viewport_width,viewport_height);
	int n = 0;
	do 
	{
		n++;
	} while (pow(2,n)<Ntex_render);
	Ntex_render = pow(2,n);
			
	renderdata = matrix(0,Ntex_render-1,0,Ntex_render-1);
	rendermask = imatrix(0,Ntex_render-1,0,Ntex_render-1);
			
	int map_type;
	map_type = [myAppController maptype];
	
	//if map has been loaded..
	if ([myAppController maptype]!=0)
	{		
		size_t pixnum;
		int nside = [myAppController map_nside];
		int ordering = [myAppController pixelordering];
		int Npix = [myAppController Npixels];
					
		[myAppController setProgressText:@"generating rendered texture map..."];			
		
		float fovy = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_fovykey];
		
		//OpenGLview needs to store this fovy value to correctly draw render mode
		//texture even if user changes fovy in prefs while viewing render mode
		[myOpenGLview setFovy_render:fovy];		
		
		//similarly for orthoEnabledFlag
		BOOL ortho_current = [myOpenGLview orthoEnabledFlag];
		[myOpenGLview setOrthoEnabledFlag_render:ortho_current];
				
		[myOpenGLview setup_observer:fovy:ortho_current];
		if (ortho_current) 
		{
			obs.frustum_height = (obs.eyedistance_ortho-radius);
		}
		else 
		{
			obs.frustum_height = 2.0*obs.near*(float)tan((double)PI*fovy/360.0);
		}
		obs.frustum_width = obs.aspect_ratio*obs.frustum_height;

		int a,b;
		double current_x, current_y,scalar_llp,ray[3],lprime[3],wx,wy,raylength,J;
		double orthox0[3],dp,sphere_theta,sphere_phi;
		float T,Q,U,P,Tmax,Tmin,Qmax,Qmin,Umax,Umin,Pmax,Pmin;
		int firstpoint = 1;
		BOOL intersect_flag;
		
		for (a=0;a<Ntex_render;a++) 
		{	
			if (a%(Ntex_render/64)==0 || (a==Ntex_render-1))
			{
				[myAppController setProgressIndicator:((double)a/Ntex_render)];
			}
			for (b=0;b<Ntex_render;b++) 
			{							
				//ray trace along ray passing through center of texel
				current_x = ((double)a+0.5)/(double)Ntex_render;
				current_y = ((double)b+0.5)/(double)Ntex_render;
				
				//TO DO: alternatively, ray trace through each texel corner and,
				//if at least one corner lies inside the sphere, set the pixel to the 
				//average value of the corners. This may solve the issue where a 
				//halo is sometimes seen around the edge of the sphere in render mode, 
				//due to pixels in the polygonal model of the underlying lit sphere
				//not matching well with the pixels obtained by raytracing. 
				//The corner method would include pixels whose center lies outside
				//the sphere but have one corner inside.
				
				//compute where the intersection with sphere occurs, in world coords.
				wx = 0.5f * obs.frustum_width  * (2.0f*current_x - 1.0f);
				wy = 0.5f * obs.frustum_height * (2.0f*current_y - 1.0f);
				
				intersect_flag = NO;
				
				//project through perspective view frustum
				if (!ortho_current) 
				{					
					//lprime is vector directed from observer through selected point on sphere
					lprime[0] = obs.near*obs.view_direction[0] + wx*obs.localx[0] + wy*obs.localy[0];
					lprime[1] = obs.near*obs.view_direction[1] + wx*obs.localx[1] + wy*obs.localy[1];
					lprime[2] = obs.near*obs.view_direction[2] + wx*obs.localx[2] + wy*obs.localy[2];				
					normalize_double(lprime);
							
					scalar_llp = obs.view_direction[0]*lprime[0]
								+ obs.view_direction[1]*lprime[1]
						        + obs.view_direction[2]*lprime[2];
					
					J = SQR(scalar_llp)-(1.0-SQR(radius/obs.eyedistance_perspective));				
					if (J>=0.0)
					{					
						intersect_flag = YES;
						raylength = obs.eyedistance_perspective*(scalar_llp-sqrt(J));		
						ray[0] = obs.view_point[0] + raylength*lprime[0];
						ray[1] = obs.view_point[1] + raylength*lprime[1];
						ray[2] = obs.view_point[2] + raylength*lprime[2];				
						normalize_double(ray);																		
					}					
				}
				//project through orthographic view cuboid
				else 
				{					
					J = SQR(radius)-SQR(wx)-SQR(wy);
					if (J>=0.0)
					{					
						intersect_flag = YES;
						
						//orthox0[] is position of point on cuboid face where ray starts
						orthox0[0] = obs.view_point[0] + wx*obs.localx[0] + wy*obs.localy[0];
						orthox0[1] = obs.view_point[1] + wx*obs.localx[1] + wy*obs.localy[1];
						orthox0[2] = obs.view_point[2] + wx*obs.localx[2] + wy*obs.localy[2];
						raylength = obs.eyedistance_ortho-sqrt(J);
						
						ray[0] = orthox0[0] + raylength*obs.view_direction[0];
						ray[1] = orthox0[1] + raylength*obs.view_direction[1];
						ray[2] = orthox0[2] + raylength*obs.view_direction[2];				
						normalize_double(ray);
					}									
				}
				
				if (intersect_flag)
				{
					sphere_theta = acos((double)ray[2]);
					sphere_phi = atan2((double)ray[1],(double)ray[0]);	
					
					//..find Healpix pixel corresponding to these angles
					if (ordering==0)
					{
						heal_ang2pix_ring(nside,sphere_theta,sphere_phi,&pixnum);				
					}
					else 
					{
						heal_ang2pix_nest(nside,sphere_theta,sphere_phi,&pixnum);				
					}		
					
					if (firstpoint) 
					{	
						//if this is the first sphere data point,
						//start recording the latest maximum and minimum
						firstpoint = 0;
						switch (map_type)
						{		
							case 1:							
								T = hpic_float_get(hpic_Tmap,pixnum);
								renderdata[a][b] = T;
								Tmin = T; Tmax = T;
								break;
							
							case 2:			
								Q = hpic_float_get(hpic_Qmap,pixnum);
								renderdata[a][b] = Q;
								Qmin = Q; Qmax = Q;
								break;
							
							case 3:		
								U = hpic_float_get(hpic_Umap,pixnum);
								renderdata[a][b] = U;
								Umin = U; Umax = U;
								break;
							
							case 4:		
								Q = hpic_float_get(hpic_Qmap,pixnum);
								U = hpic_float_get(hpic_Umap,pixnum);
								P = (float)sqrt((double)Q*Q+U*U);
								renderdata[a][b] = P;
								Pmin = P; Pmax = P;
								break;
						}
					}
					else 
					{
						switch (map_type) 
						{		
							case 1:							
								T = hpic_float_get(hpic_Tmap,pixnum);
								renderdata[a][b] = T;
								if (T<Tmin) Tmin=T;
								if (T>Tmax) Tmax=T;
								break;
							case 2:			
								Q = hpic_float_get(hpic_Qmap,pixnum);
								renderdata[a][b] = Q;
								if (Q<Qmin) Qmin=Q;
								if (Q>Qmax) Qmax=Q;
								break;
							case 3:		
								U = hpic_float_get(hpic_Umap,pixnum);
								renderdata[a][b] = U;
								if (U<Umin) Umin=U;
								if (U>Umax) Umax=U;											
								break;
							case 4:		
								Q = hpic_float_get(hpic_Qmap,pixnum);
								U = hpic_float_get(hpic_Umap,pixnum);
								P = (float)sqrt((double)Q*Q+U*U);
								renderdata[a][b] = P;
								if (P<Pmin) Pmin=P;
								if (P>Pmax) Pmax=P;
								break;										
						}							
					}						
					rendermask[a][b] = 1;
				}
				else
				{
					rendermask[a][b] = 0;
				}
				
			}
		}			
		switch (map_type)
		{		
			case 1:	
				mapmaxima_render.maxT = Tmax; mapmaxima_presentation.maxT = Tmax;
				mapmaxima_render.minT = Tmin; mapmaxima_presentation.minT = Tmin;				
				break;
			
			case 2:
				mapmaxima_render.maxQ = Qmax; mapmaxima_presentation.maxQ = Qmax;
				mapmaxima_render.minQ = Qmin; mapmaxima_presentation.minQ = Qmin;					
				break;
			
			case 3:
				mapmaxima_render.maxU = Umax; mapmaxima_presentation.maxU = Umax;
				mapmaxima_render.minU = Umin; mapmaxima_presentation.minU = Umin;					
				break;
			
			case 4:
				mapmaxima_render.maxP = Pmax; mapmaxima_presentation.maxP = Pmax;
				mapmaxima_render.minP = Pmin; mapmaxima_presentation.minP = Pmin;	
				break;
		}
		
		colorrange c = {Tmax,Tmin,Qmax,Qmin,Umax,Umin,Pmax,Pmin};
		[myAppController setColorrange_render:c];
		[myAppController setColorrange_presentation:c];		
		[self makeHistograms_render];
		[self updateTexs_render];	
	}	
}


- (void)updateTexs_render
{
	int a,b;
	float currentmax,currentmin;
	float scalar,R,G,B;
	
	//must make sure main view is the current context, since
	//textures belong to the current context.
	[myOpenGLview makeThisViewCurrentContext];
	
	//create a new texture name
	glDeleteTextures((GLsizei)1,&render_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures((GLsizei)1,&render_tex);
	
	GLubyte ***rendertexture;
	rendertexture = glu3matrix(0,Ntex_render-1,0,Ntex_render-1,0,2);
	
	//find the current color range
	int map_type = [myAppController maptype];	
	int render = [myAppController render_mode];
	colorrange *c;
	if (render==2) {
		c = [myAppController colorrange_render];
	}
	else c = [myAppController colorrange_presentation];	
	switch (map_type) {		
		case 1:		
			currentmax = c->maxT;
			currentmin = c->minT;
			break;
		case 2:			
			currentmax = c->maxQ;
			currentmin = c->minQ;
			break;
		case 3:			
			currentmax = c->maxU;
			currentmin = c->minU;
			break;
		case 4:			
			currentmax = c->maxP;
			currentmin = c->minP;
			break;
	}
	
	[myAppController setProgressText:@"binding rendered texture map to sphere ..."];
	
	float color_x[3];
	BOOL reverse = [[NSUserDefaults standardUserDefaults] 
                                   boolForKey:CMBview_colormapreversekey];
	[myOpenGLview setRenderwith_colormap_reverse:reverse];
	
	for (a=0;a<Ntex_render;a++)
	{	
		if (a%(Ntex_render/64)==0 || (a==Ntex_render-1)) [myAppController setProgressIndicator: ((double)a/Ntex_render)];
		for (b=0;b<Ntex_render;b++) {	
			
			if (rendermask[a][b]) 
			{		
				if (currentmax != currentmin)
				{
					scalar = (renderdata[a][b]-currentmin) / (currentmax-currentmin);	
				}
				else
				{
					scalar = 0.5;
				}
				
				/* clamp colors to acceptable range */
				if (scalar>1.0) 
				{ 
					scalar = 1.0;
				}
				if (scalar<0.0) 
				{
					scalar = 0.0;
				}					
				
				switch ([myAppController colormap_flag]) 
				{					
					case 0 :
						/* greyscale map */		
						R = scalar; G = scalar; B = scalar;
						break;						
				
					case 1 :	
						/* color map */		
						if (reverse) scalar = 1.0-scalar;
						colorpath(current_colormap_ptr->color_N,
								  current_colormap_ptr->color_xi,
								  current_colormap_ptr->color_ci,
								  scalar, color_x);	
						HSVtoRGB(&R,&G,&B,color_x[0],color_x[1],color_x[2]);	
				}
				
				rendertexture[a][b][0] = (GLubyte) 255.0*R;
				rendertexture[a][b][1] = (GLubyte) 255.0*G;
				rendertexture[a][b][2] = (GLubyte) 255.0*B;																				
			}
			
			else
			{
				//background color
				rendertexture[a][b][0] = (GLubyte) 255;
				rendertexture[a][b][1] = (GLubyte) 255;
				rendertexture[a][b][2] = (GLubyte) 255;
			}			
		}
	}

	//bind the texture data to a texture object
	glBindTexture(GL_TEXTURE_2D,render_tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,Ntex_render,Ntex_render,0,
				 GL_RGB,GL_UNSIGNED_BYTE,**rendertexture);

	free_glu3matrix(rendertexture,0,Ntex_render-1,0,Ntex_render-1,0,2);	

	[myAppController setProgressText:@""];
	[myAppController setProgressIndicator:0.0];
	[myOpenGLview setNeedsDisplay:YES];
}

//generate pixel accurate texture data, raw for export
- (void)genTextures_render_forexport
{	
	int map_type = [myAppController maptype];	

	//if map has been loaded..
	if ([myAppController maptype]!=0)
	{				
	
		NSUserDefaults *defaults;		
		defaults = [NSUserDefaults standardUserDefaults];
		int Ntex_render_export = [defaults integerForKey:CMBview_exportimagesizekey];
						
		renderdata_export = matrix(0,Ntex_render_export-1,0,Ntex_render_export-1);
		rendermask_export = imatrix(0,Ntex_render_export-1,0,Ntex_render_export-1);	
		
		size_t pixnum;
		int nside = [myAppController map_nside];
		int ordering = [myAppController pixelordering];
		int Npix = [myAppController Npixels];		
		int a,b;
		double scalar_llp, ray[3],lprime[3],wx,wy,raylength,J;
		double sphere_theta,sphere_phi,current_x, current_y;
		double orthox0[3],dp;
		float T,Q,U,P;

		[myAppController setProgressText:@"generating rendered texture map for export..."];					
		
		float fovy = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_fovykey];		
		BOOL ortho_current = [myOpenGLview orthoEnabledFlag];
		[myOpenGLview setup_observer:fovy:ortho_current];		

		BOOL intersect_flag;
		if (ortho_current) 
		{
			obs.frustum_height = (obs.eyedistance_ortho-radius);
		}
		else 
		{
			obs.frustum_height = 2.0*obs.near*(float)tan((double)PI*fovy/360.0);
		}
		obs.frustum_width = obs.aspect_ratio*obs.frustum_height;
										
		for (a=0;a<Ntex_render_export;a++) 
		{	
			if (a%(Ntex_render_export/64)==0 || (a==Ntex_render_export-1)) 
			{
				[myAppController setProgressIndicator: ((double)a/Ntex_render_export)];
			}
			for (b=0;b<Ntex_render_export;b++)
			{					
				current_x = ((double)a+0.5)/(double)Ntex_render_export;
				current_y = ((double)b+0.5)/(double)Ntex_render_export;
				
				wx = 0.5*obs.frustum_width*(2.0*current_x-1.0);
				wy = 0.5*obs.frustum_height*(2.0*current_y-1.0);
				
				intersect_flag = NO;
				
				//project through perspective view frustum
				if (!ortho_current)
				{		
					lprime[0] = obs.near*obs.view_direction[0] 
					            + wx*obs.localx[0] + wy*obs.localy[0];
					lprime[1] = obs.near*obs.view_direction[1] 
								+ wx*obs.localx[1] + wy*obs.localy[1];
					lprime[2] = obs.near*obs.view_direction[2] 
						        + wx*obs.localx[2] + wy*obs.localy[2];				
					normalize_double(lprime);
					
					scalar_llp = obs.view_direction[0]*lprime[0]
						       + obs.view_direction[1]*lprime[1]
						       + obs.view_direction[2]*lprime[2];
					
					J = SQR(scalar_llp)-(1.0-SQR(radius/obs.eyedistance_perspective));				
					if (J>=0.0)
					{	
						intersect_flag = YES;
						raylength = obs.eyedistance_perspective*(scalar_llp-sqrt(J));		
						ray[0] = obs.view_point[0] + raylength*lprime[0];
						ray[1] = obs.view_point[1] + raylength*lprime[1];
						ray[2] = obs.view_point[2] + raylength*lprime[2];				
						normalize_double(ray);
					}
				}
				
				//project through orthographic view cuboid
				else 
				{					
					J = SQR(radius)-SQR(wx)-SQR(wy);
					if (J>=0.0)
					{					
						intersect_flag = YES;
						orthox0[0] = obs.view_point[0] 
								     + wx*obs.localx[0] + wy*obs.localy[0];
						orthox0[1] = obs.view_point[1] 
							         + wx*obs.localx[1] + wy*obs.localy[1];
						orthox0[2] = obs.view_point[2]
							         + wx*obs.localx[2] + wy*obs.localy[2];
						raylength = obs.eyedistance_ortho-sqrt(J);
					
						ray[0] = orthox0[0] + raylength*obs.view_direction[0];
						ray[1] = orthox0[1] + raylength*obs.view_direction[1];
						ray[2] = orthox0[2] + raylength*obs.view_direction[2];				
						normalize_double(ray);
					}									
				}
				
				if (intersect_flag)
				{					
					sphere_theta = acos((double)ray[2]);
					sphere_phi = atan2((double)ray[1],(double)ray[0]);
					
					if (ordering==0) 
					{
						heal_ang2pix_ring(nside,sphere_theta,sphere_phi,&pixnum);				
					}
					else
					{
						heal_ang2pix_nest(nside,sphere_theta,sphere_phi,&pixnum);				
					}		
					
					switch (map_type) 
					{		
						case 1:							
							T = hpic_float_get(hpic_Tmap,pixnum);
							renderdata_export[a][b] = T;							
							break;
						
						case 2:			
							Q = hpic_float_get(hpic_Qmap,pixnum);
							renderdata_export[a][b] = Q;						
							break;
						
						case 3:		
							U = hpic_float_get(hpic_Umap,pixnum);
							renderdata_export[a][b] = U;							
							break;
						
						case 4:		
							Q = hpic_float_get(hpic_Qmap,pixnum);
							U = hpic_float_get(hpic_Umap,pixnum);
							P = (float)sqrt((double)Q*Q+U*U);
							renderdata_export[a][b] = P;							
							break;
					}
					rendermask_export[a][b] = 1;
				}
				
				else
				{
					rendermask_export[a][b] = 0;
				}				
			}
		}
		
		float currentmax,currentmin;
		float scalar,R,G,B;		
		GLubyte ***rendertexture_export;
		rendertexture_export = glu3matrix(0,Ntex_render_export-1,0,Ntex_render_export-1,0,2);		
		
		//find the current color range in render mode 
		colorrange *c;
		c = [myAppController colorrange_render];
		
		switch (map_type) 
		{		
			case 1:		
				currentmax = c->maxT;
				currentmin = c->minT;
				break;
			
			case 2:			
				currentmax = c->maxQ;
				currentmin = c->minQ;
				break;
			
			case 3:			
				currentmax = c->maxU;
				currentmin = c->minU;
				break;
			
			case 4:			
				currentmax = c->maxP;
				currentmin = c->minP;
				break;
		}	
		
		[myAppController setProgressText:@"creating rendered texture map for export ..."];		
		
		float color_x[3];
		BOOL reverse = [[NSUserDefaults standardUserDefaults] boolForKey:CMBview_colormapreversekey];
		int color_N_current;	
		
		for (a=0;a<Ntex_render_export;a++) 
		{	
			if (a%(Ntex_render_export/64)==0 || (a==Ntex_render_export-1)) 
			{
				[myAppController setProgressIndicator: ((double)a/Ntex_render_export)];
			}
				
			for (b=0;b<Ntex_render_export;b++) 
			{					
				if (rendermask_export[a][b]) 
				{				
					scalar = (renderdata_export[a][b]-currentmin)/(currentmax-currentmin);	
					
					/* clamp colors to acceptable range */
					if (scalar>1.0) 
					{ 
						scalar = 1.0;
					}
					else if (scalar<0.0) {
						scalar = 0.0;
					}					
					
					switch ([myAppController colormap_flag])
					{					
						case 0 :
							/* greyscale map */		
							R = scalar; G = scalar; B = scalar;
							break;							
						
						case 1 :	
							/* color map */		
							if (reverse) scalar = 1.0-scalar;
							colorpath(current_colormap_ptr->color_N,
									  current_colormap_ptr->color_xi,
									  current_colormap_ptr->color_ci,
									  scalar, color_x);	
							HSVtoRGB(&R,&G,&B,color_x[0],color_x[1],color_x[2]);	
					}
				
					rendertexture_export[a][b][0] = (GLubyte) 255.0*R;
					rendertexture_export[a][b][1] = (GLubyte) 255.0*G;
					rendertexture_export[a][b][2] = (GLubyte) 255.0*B;																				
				}
				
				else 
				{
					//background color
					rendertexture_export[a][b][0] = (GLubyte) 255;
					rendertexture_export[a][b][1] = (GLubyte) 255;
					rendertexture_export[a][b][2] = (GLubyte) 255;
				}			
			}
		}		
		
		free_matrix(renderdata_export,0,Ntex_render_export-1,0,Ntex_render_export-1);
		free_imatrix(rendermask_export,0,Ntex_render_export-1,0,Ntex_render_export-1);
							
		NSBitmapImageRep *thebitmap;
		thebitmap = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&(**rendertexture_export)
															pixelsWide:Ntex_render_export 
															pixelsHigh:Ntex_render_export 
														 bitsPerSample:8
													   samplesPerPixel:3 
															  hasAlpha:NO
															  isPlanar:NO 
														colorSpaceName:@"NSDeviceRGBColorSpace" 
														   bytesPerRow:3*Ntex_render_export 
														  bitsPerPixel:24];
		
		NSData* myTIFF = [thebitmap TIFFRepresentation];
		[thebitmap release];
		[myAppController setTIFF:myTIFF];
													
		free_glu3matrix(rendertexture_export,0,Ntex_render_export-1,0,Ntex_render_export-1,0,2);	
		[myAppController setProgressText:@""];
		[myAppController setProgressIndicator:0.0];				
	}
}



/**********************************************************************/
/*   accessor methods for structure containing  current map maxima.   */
/**********************************************************************/

- (mapmaxima *)mapmaxima_interactive
{
	return &mapmaxima_interactive;
}
- (mapmaxima *)mapmaxima_render
{
	return &mapmaxima_render;
}
- (mapmaxima *)mapmaxima_presentation
{
	return &mapmaxima_presentation;
}


/**********************************************************************/
/*      generate histogram data for LittleOpenGLview to display       */
/**********************************************************************/

- (void)makeHistograms_render
{	
	int a,b,face;
	float T,Q,U,P;
	int bin,hist_max;
	float hlog_max,hlog_min;

	[myAppController setProgressText:@"making render mode histograms..."];
	
	histogram *current_hist;
	mapmaxima *current_maxima;
	int render = [myAppController render_mode];
	if (render==2)
	{
		current_hist = &histogram_render;	
		current_maxima = &mapmaxima_render;
	}
	else
	{
		current_hist = &histogram_presentation;	
		current_maxima = &mapmaxima_presentation;
	}
	
	//initialize histogram
	int hist[Nbin];
	float normhist[Nbin],normhistlog[Nbin];
	for(bin=0;bin<Nbin;bin++)
	{
		hist[bin] = 0;
	}
	
	float V,Vmax,Vmin;		
	int map_type = [myAppController maptype];
	switch (map_type)
	{		
		case 1: Vmax = current_maxima->maxT; Vmin = current_maxima->minT; break;
		case 2: Vmax = current_maxima->maxQ; Vmin = current_maxima->minQ; break;
		case 3: Vmax = current_maxima->maxU; Vmin = current_maxima->minU; break;
		case 4: Vmax = current_maxima->maxP; Vmin = current_maxima->minP; break;
	}
			
	//construct histograms
	for (a=0;a<Ntex_render;a++) 
	{	
		for (b=0;b<Ntex_render;b++)
		{
			if (rendermask[a][b]) 
			{				
				V = renderdata[a][b];
				if (Vmax!=Vmin) 
				{
					bin = floor( (float) (Nbin-1) * (V-Vmin)/(Vmax-Vmin) );
				}
				else bin = 0;
				hist[bin]++;
			}
		}
	}
	
	//normalize histograms to max. count value
	hist_max=0;
	for(bin=0;bin<Nbin;bin++)
	{
		if (hist[bin]>hist_max) hist_max=hist[bin];		
	}
	
	for(bin=0;bin<Nbin;bin++)
	{
		if (hist_max==0) normhist[bin]=0.0;
		else 
		{
			normhist[bin] = (float)hist[bin]/hist_max;
		}
	}
	
	for(bin=0;bin<Nbin;bin++)
	{
		if (bin==0)
		{
			hlog_max = log((float)hist[bin]);
			hlog_min = log((float)hist[bin]);
		}
		else
		{
			if (hist[bin]!=0 && log((float)hist[bin])>hlog_max) hlog_max=log(hist[bin]);		
			if (hist[bin]!=0 && log((float)hist[bin])<hlog_min) hlog_min=log(hist[bin]);		
		}
	}
	
	float loghist_floor = 0.02;
	for(bin=0;bin<Nbin;bin++)
	{
		if (hist[bin]==0 || (hlog_max==hlog_min)) 
		{
			normhistlog[bin] =  0.0;
		}
		else 
		{
			normhistlog[bin] = loghist_floor + (log((float)hist[bin])-hlog_min)/(hlog_max-hlog_min) * (1.0-loghist_floor);
		}				
	}
	
	for(bin=0;bin<Nbin;bin++) 
	{		
		switch (map_type) 
		{		
			case 1: 
				current_hist->Thist[bin] = hist[bin];
				current_hist->normThist[bin] = normhist[bin];
				current_hist->normThistlog[bin] = normhistlog[bin];
				break;
			
			case 2: 
				current_hist->Qhist[bin] = hist[bin];
				current_hist->normQhist[bin] = normhist[bin];
				current_hist->normQhistlog[bin] = normhistlog[bin];
				break;
			
			case 3: 
				current_hist->Uhist[bin] = hist[bin];
				current_hist->normUhist[bin] = normhist[bin];
				current_hist->normUhistlog[bin] = normhistlog[bin];
				break;
		
			case 4: 
				current_hist->Phist[bin] = hist[bin];
				current_hist->normPhist[bin] = normhist[bin];
				current_hist->normPhistlog[bin] = normhistlog[bin];
				break;
		}		
	}
	
	[myLittleOpenGLview changeHistogram];
}

- (void)makeHistograms_interactive
{
	int a,b,face;
	float T,Q,U,P;
	int bin,Thist_max,Qhist_max,Uhist_max,Phist_max;
	float Thlog_max,Qhlog_max,Uhlog_max,Phlog_max;
	float Thlog_min,Qhlog_min,Uhlog_min,Phlog_min;
	
	[myAppController setProgressText:@"making interactive mode histograms..."];

	histogram *current_hist = &histogram_interactive;	
	
	//initialize histograms
	for(bin=0;bin<Nbin;bin++) 
	{
		current_hist->Thist[bin] = 0;
		if ([myAppController polarisation]==1 || [myAppController polarisation]==2) 
		{										
			current_hist->Qhist[bin] = 0;
			current_hist->Uhist[bin] = 0;
			current_hist->Phist[bin] = 0;
		}
	}
		
	float Tmax,Tmin,Qmax,Qmin,Umax,Umin,Pmax,Pmin;
	Tmax = mapmaxima_interactive.maxT; 	Tmin = mapmaxima_interactive.minT; 
	Qmax = mapmaxima_interactive.maxQ; 	Qmin = mapmaxima_interactive.minQ; 
	Umax = mapmaxima_interactive.maxU; 	Umin = mapmaxima_interactive.minU; 
	Pmax = mapmaxima_interactive.maxP; 	Pmin = mapmaxima_interactive.minP; 
		
	//construct histograms
	for (face=0;face<6;face++)
	{		
		for(a=0;a<Ntexture;a++) 
		{
			for(b=0;b<Ntexture;b++) 
			{
				//T histogram
				T = Tface[face][a][b];
				if (Tmax!=Tmin)
				{
					bin = floor((float)(Nbin-1)*(T-Tmin)/(Tmax-Tmin));
				}
				else 
				{
					bin = 0;
				}
				(current_hist->Thist[bin])++;
				
				if ([myAppController polarisation]==1 || [myAppController polarisation]==2) 
				{										
					//Q histogram
					Q = Qface[face][a][b];
					if (Qmax!=Qmin)
					{
						bin = floor((float)(Nbin-1)*(Q-Qmin)/(Qmax-Qmin));
					}
					else 
					{
						bin = 0;
					}
					(current_hist->Qhist[bin])++;
					
					//U histogram
					U = Uface[face][a][b];
					if (Umax!=Umin) {
						bin = floor((float)(Nbin-1)*(U-Umin)/(Umax-Umin));
					}
					else 
					{
						bin = 0;
					}
					(current_hist->Uhist[bin])++;
					
					//P histogram
					P = Pface[face][a][b];
					if (Pmax!=Pmin) {
						bin = floor((float)(Nbin-1)*(P-Pmin)/(Pmax-Pmin));
					}
					else
					{
						bin = 0;
					}
					(current_hist->Phist[bin])++;
				}
			}
		}
	}
	//normalize histograms to max. count value
	Thist_max=0;
	if ([myAppController polarisation]==1 || [myAppController polarisation]==2) 
	{											
		Qhist_max=0;
		Uhist_max=0;
		Phist_max=0;
	}
	
	for(bin=0;bin<Nbin;bin++)
	{
		if ((current_hist->Thist[bin])>Thist_max) Thist_max=(current_hist->Thist[bin]);		
		if ([myAppController polarisation]==1 || [myAppController polarisation]==2)
		{								
			if ((current_hist->Qhist[bin])>Qhist_max) Qhist_max=(current_hist->Qhist[bin]);		
			if ((current_hist->Uhist[bin])>Uhist_max) Uhist_max=(current_hist->Uhist[bin]);		
			if ((current_hist->Phist[bin])>Phist_max) Phist_max=(current_hist->Phist[bin]);
		}
	}

	for(bin=0;bin<Nbin;bin++)
	{
		float Th = current_hist->Thist[bin];
		float Qh = current_hist->Qhist[bin];
		float Uh = current_hist->Uhist[bin];
		float Ph = current_hist->Phist[bin];
	
		if (Thist_max==0) 
		{
			current_hist->normThist[bin]=0.0;	
		}
		else
		{
			(current_hist->normThist[bin]) = Th/(float)Thist_max;
		}

		if ([myAppController polarisation]==1 || [myAppController polarisation]==2)
		{								
			if (Qhist_max==0) 
			{
				current_hist->normQhist[bin]=0.0;	
			}
			else 
			{
				(current_hist->normQhist[bin]) = Qh/(float)Qhist_max;
			}
			
			if (Uhist_max==0) 
			{
				current_hist->normUhist[bin]=0.0;
			}
			else 
			{
				(current_hist->normUhist[bin]) = Uh/(float)Uhist_max;
			}
			
			if (Phist_max==0) 
			{
				(current_hist->normPhist[bin])=0.0;
			}
			else
			{
				(current_hist->normPhist[bin]) = Ph/(float)Phist_max;
			}
		}
	}
	
	for(bin=0;bin<Nbin;bin++)
	{
		
		float Th = current_hist->Thist[bin];
		float Qh = current_hist->Qhist[bin];
		float Uh = current_hist->Uhist[bin];
		float Ph = current_hist->Phist[bin];
		
		if (bin==0) 
		{
			Thlog_max = log(Th);
			Thlog_min = log(Th);
		}
		else
		{
			if (Th!=0 && log(Th)>Thlog_max) Thlog_max=log(Th);		
			if (Th!=0 && log(Th)<Thlog_min) Thlog_min=log(Th);		
		}
		
		if ([myAppController polarisation]==1 || [myAppController polarisation]==2)
		{								
			if (bin==0) 
			{
				Qhlog_max = log(Qh); Qhlog_min = log(Qh);
				Uhlog_max = log(Uh); Uhlog_min = log(Uh);
				Phlog_max = log(Ph); Phlog_min = log(Ph);
			}
			else 
			{		
				if (Qh!=0 && log(Qh)>Qhlog_max) Qhlog_max=log(Qh);		
				if (Qh!=0 && log(Qh)<Qhlog_min) Qhlog_min=log(Qh);	
				if (Uh!=0 && log(Uh)>Uhlog_max) Uhlog_max=log(Uh);		
				if (Uh!=0 && log(Uh)<Uhlog_min) Uhlog_min=log(Uh);	
				if (Ph!=0 && log(Ph)>Phlog_max) Phlog_max=log(Ph);		
				if (Ph!=0 && log(Ph)<Phlog_min) Phlog_min=log(Ph);									
			}	
		}
	}

	float loghist_floor = 0.02;
	for(bin=0;bin<Nbin;bin++) 
	{
		
		float Th = current_hist->Thist[bin];
		float Qh = current_hist->Qhist[bin];
		float Uh = current_hist->Uhist[bin];
		float Ph = current_hist->Phist[bin];
		
		if (Th==0 || (Thlog_max==Thlog_min)) 
		{
			(current_hist->normThistlog[bin]) =  0.0;
		}
		else      
		{
			(current_hist->normThistlog[bin]) = loghist_floor + 
			(log(Th)-Thlog_min)/(Thlog_max-Thlog_min)*(1.0-loghist_floor);
		}		
		
		if ([myAppController polarisation]==1 || [myAppController polarisation]==2)
		{								
			
			if (Qh==0 || (Qhlog_max==Qhlog_min)) 
			{
				(current_hist->normQhistlog[bin]) =  0.0;
			}
			else 
			{
				(current_hist->normQhistlog[bin]) = loghist_floor + 
				(log(Qh)-Qhlog_min)/(Qhlog_max-Qhlog_min)*(1.0-loghist_floor);
			}	
			
			if (Uh==0 || (Uhlog_max==Uhlog_min)) 
			{
				(current_hist->normUhistlog[bin]) =  0.0;
			}
			else
			{
				(current_hist->normUhistlog[bin]) = loghist_floor +
				(log(Uh)-Uhlog_min)/(Uhlog_max-Uhlog_min)*(1.0-loghist_floor);
			}	
			
			if (Ph==0 || (Phlog_max==Phlog_min)) 
			{
				(current_hist->normPhistlog[bin]) =  0.0;
			}
			else 
			{
				(current_hist->normPhistlog[bin]) = loghist_floor + 
				(log(Ph)-Phlog_min)/(Phlog_max-Phlog_min)*(1.0-loghist_floor);
			}											
		}
	}
	
	[myLittleOpenGLview changeColorRange];
	[myLittleOpenGLview changeHistogram];
	[myLittleOpenGLview display];	
}

//return pointers to structures containing current map histograms.
- (histogram *)histogram_interactive
{
	return &histogram_interactive;
}
- (histogram *)histogram_render
{
	return &histogram_render;
}
- (histogram *)histogram_presentation
{
	return &histogram_presentation;
}


/**********************************************************************/
/*                 export framebuffer to image data                   */
/**********************************************************************/

- (void)framebuffer_forexport
{
	[myOpenGLview makeThisViewCurrentContext];
	[myOpenGLview update];
	
	NSRect rect = [myOpenGLview bounds];
	int viewport_width  = (int) rect.size.width;
	int viewport_height = (int) rect.size.height;
	
	GLubyte *imageBuffer = (GLubyte*)malloc(3 * viewport_width * viewport_height * sizeof(GLubyte));

	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, (int)viewport_width, (int)viewport_height, GL_RGB, GL_UNSIGNED_BYTE, imageBuffer);
	
	NSBitmapImageRep *thebitmap;
	thebitmap = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes: &imageBuffer		
														pixelsWide: viewport_width
														pixelsHigh: viewport_height 
													 bitsPerSample: 8
												   samplesPerPixel: 3
														  hasAlpha: NO
														  isPlanar: NO 
													colorSpaceName: @"NSDeviceRGBColorSpace" 
													   bytesPerRow: (int)viewport_width*3
													  bitsPerPixel: 24];
	
	NSImage *image = [[NSImage alloc] init];
	[image addRepresentation:thebitmap];
	
	// Flip image vertically
	[image setFlipped:YES];
	[image lockFocusOnRepresentation:thebitmap];
	[image unlockFocus];
	
	NSData *myTIFF = [image TIFFRepresentation];
	
	// Free up temporaries
	[thebitmap release];
	[image release];
	free(imageBuffer);	
	
	[myAppController setTIFF:myTIFF];	
}


@end
