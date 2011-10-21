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
#import "memory.h"
#import "OpenGLview.h"
#import "LittleOpenGLview.h"
#import "AppController.h"

@class OpenGLview;
@class LittleOpenGLview;
@class AppController;
 
extern AppController *anAppController;

@interface CMBdata : NSObject
{
	IBOutlet OpenGLview *myOpenGLview;
	IBOutlet LittleOpenGLview *myLittleOpenGLview;
	IBOutlet AppController *myAppController;
	
	mapmaxima mapmaxima_interactive;
	mapmaxima mapmaxima_render;
	mapmaxima mapmaxima_presentation;
	
	histogram histogram_interactive;
	histogram histogram_render;
	histogram histogram_presentation;
	
	int Ngc_arc, Ntexture, Ntex_render, NStokes;
	
	//pointers to texture data
	float ***Tface, ***Qface, ***Uface, ***Pface;
	float **renderdata, **renderdata_export;
	int **rendermask, **rendermask_export;
} 

- (id)init;

- (void)alloc_Ttexture;
- (void)alloc_Qtexture;
- (void)alloc_Utexture;
- (void)alloc_Ptexture;
- (void)dealloc_Ttexture;
- (void)dealloc_Qtexture;
- (void)dealloc_Utexture;
- (void)dealloc_Ptexture;
- (void)dealloc_renderdata;
- (void)dealloc_stokesdata;

- (void)genTextures_interactive;
- (void)updateTexs_interactive;
- (void)scancube_T;
- (void)scancube_TQU;
- (void)genTextures_render;
- (void)updateTexs_render;
- (void)genTextures_render_forexport;
- (void)framebuffer_forexport;
- (void)genStokes;

- (void)makeHistograms_interactive;
- (void)makeHistograms_render;

- (mapmaxima *)mapmaxima_interactive;
- (mapmaxima *)mapmaxima_render;
- (mapmaxima *)mapmaxima_presentation;
- (histogram *)histogram_interactive;
- (histogram *)histogram_render;
- (histogram *)histogram_presentation;

- (int)NStokes;

@end
