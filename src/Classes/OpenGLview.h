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
#import "CMBdata.h"
#import "LittleOpenGLview.h"
#import "AppController.h"
#import "PreferenceController.h"

@class CMBdata;
@class AppController;
@class LittleOpenGLview;
@class PreferenceController;

@interface OpenGLview : NSOpenGLView
{
	NSOpenGLContext *oglContext_mainview;
	IBOutlet CMBdata *myCMBdata;
	IBOutlet AppController *myAppController;
	
	//viewing state
	float *v1_display, *v2_display, *v3_display;
	float *t1_display, *t2_display, *t3_display;
	float *norm_display;
	float viewTheta, viewPhi, oldviewTheta, oldviewPhi;
	float viewZoom, maxZoom, minApproach, backgroundRGB[3];
	int texelInterpolationFlag;
	float fovy, fovy_render;
	BOOL orthoEnabledFlag, orthoEnabledFlag_render;
	
	//lighting state
	GLfloat ambientlightRGBA[4], ambientmatRGBA[4];
	GLfloat diffuselightRGBA[4], diffusematRGBA[4];
	GLfloat specularlightRGBA[4], specularmatRGBA[4];
	GLfloat blackRGBA[4], whiteRGBA[4];
	float lightTheta, lightPhi, lightRadius;
	GLfloat light_position[4], shininess;
	BOOL lightingEnabledFlag, ambientEnabledFlag;
	BOOL diffuseEnabledFlag, specularEnabledFlag;
	
	//stokes vector state
	GLfloat stokesRGBA[4];
	float StokesLength, max_stokes_arcmin, stokes_line_thickness;
	int stokesConventionFlag;
	
	//presentation mode state
	GLfloat present_bgrd_RGBA[4], present_text_RGBA[4];
	BOOL renderwith_colormap_reverse;
 	
	//axis grid state
	BOOL axis_flag, gridincapsFlag;
	int GRID_NUM;
	GLfloat gridRGBA[4];
	float grid_line_thickness;
	
	//mouse state
	float down_mouse_theta, down_mouse_phi,down_rightmouse_theta;
	float down_rightmouse_phi, current_mouse_theta, current_mouse_phi;
	float last_current_mouse_theta, last_current_mouse_phi;
	float mouse_sensitivity;
	int draw_mousepoint_flag;
	NSPoint downPoint, currentPoint, lastPoint;
}

- (void)initialize_lighting;
- (void)setup_observer:(float)fovy_current:(BOOL)orthoFlag;
- (void)setup_lighting;
- (void)makeThisViewCurrentContext;
- (void)updateContext;
- (NSOpenGLContext*)getContext;

- (void)drawRect:(NSRect)r;
- (void)drawBorderWithPixnum:(int)bpix
					   color:(float*)colorvec;
- (void)draw_interactivemode;
- (void)draw_rendermode;
- (void)draw_presentationmode;
- (void)drawAxes:(float)fovy_current:(BOOL)orthoFlag;
- (void)drawStokes:(float)fovy_current:(BOOL)orthoFlag;
- (void)draw_mousepoint:(float)fovy_current;

- (float)viewTheta;
- (float)viewPhi;
- (float)viewZoom;
- (float)StokesLength;
- (int)stokesConventionFlag;
- (void)setViewTheta:(float)theta;
- (void)setViewPhi:(float)phi;
- (void)setViewZoom:(float)zoom;
- (void)setBackgroundColor:(float)red green:(float)green blue:(float)blue;
- (void)setAmbientLight:(float)red:(float)green:(float)blue;
- (void)setAmbientMat:(float)red:(float)green:(float)blue;
- (void)setDiffuseLight:(float)red:(float)green:(float)blue;
- (void)setDiffuseMat:(float)red:(float)green:(float)blue;
- (void)setSpecularLight:(float)red:(float)green:(float)blue;
- (void)setSpecularMat:(float)red:(float)green:(float)blue;
- (void)setStokesLength:(float)Stokes_length;
- (void)setFovy_render:(float)fr;
- (void)setRenderwith_colormap_reverse:(BOOL)reverseFlag;
- (float)fovy_render;
- (BOOL)orthoEnabledFlag;
- (BOOL)orthoEnabledFlag_render;
- (void)setOrthoEnabledFlag_render:(BOOL)flag;

- (void)handle_backgrndcolorChanged:(NSNotification *)note;
- (void)handle_ambientlightChanged:(NSNotification *)note;
- (void)handle_ambientmatChanged:(NSNotification *)note;
- (void)handle_diffuselightChanged:(NSNotification *)note;
- (void)handle_diffusematChanged:(NSNotification *)note;
- (void)handle_specularlightChanged:(NSNotification *)note;
- (void)handle_specularmatChanged:(NSNotification *)note;
- (void)handle_rotatelightthetaChanged:(NSNotification *)note; 
- (void)handle_rotatelightphiChanged:(NSNotification *)note;
- (void)handle_lightradiusChanged:(NSNotification *)note;
- (void)handle_shininessChanged:(NSNotification *)note;
- (void)handle_lightingenableChanged:(NSNotification *)note;
- (void)handle_ambientenableChanged:(NSNotification *)note;
- (void)handle_diffuseenableChanged:(NSNotification *)note;
- (void)handle_specularenableChanged:(NSNotification *)note;
- (void)handle_gridSpacingChanged:(NSNotification *)note;
- (void)handle_stokesPropsChanged:(NSNotification *)note;
- (void)handle_texelinterpolationChanged:(NSNotification *)note;
- (void)handle_gridPropsChanged:(NSNotification *)note;
- (void)handle_orthoChanged:(NSNotification *)note;
- (void)handle_mousesensitivityChanged:(NSNotification *)note;

@end
