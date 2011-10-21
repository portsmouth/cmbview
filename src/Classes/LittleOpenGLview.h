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
#import "OpenGLview.h"
#import "AppController.h"
#import "CMBdata.h"

@class AppController;
@class CMBdata;
@class OpenGLview;

@interface LittleOpenGLview : NSOpenGLView
{
    IBOutlet AppController *myAppController;
    IBOutlet CMBdata *myCMBdata;
	NSOpenGLContext *oglContext_littleview;
	
	//view rendering
	float littleview_width, littleview_height;
	
	//mouse state
	int mouse_flag;
	float mousedown_initpos[2], mousedown_currentpos[2];
	
	//widget state
	int widget_flag;	
	float widgetradius, xoffset;	
	float current_maxwidget, current_minwidget;
	float clicked_maxwidget, clicked_minwidget;
	
	//current mode
	int render, maptype;		
	
	//pointer to current position of widgets
	widgets_coords *current_widget;
	
	//pointer to current colorrange in view coords
	colorrange_coords *current_colorrangecoords;
	
	//pointer to current map maxima
	mapmaxima *currentmapmaxima;
	
	//pointer to current color range limits in map dimensions
	colorrange *current_colorrange;
	
	//histogram data
	histogram *current_hist;			
	
	//stored widget and colorrange data
	widgets_coords widgets_interactive;
	widgets_coords widgets_render;
	widgets_coords widgets_presentation;
	
	colorrange_coords colorrange_interactive;
	colorrange_coords colorrange_render;
	colorrange_coords colorrange_presentation;
}

- (void)changeHistogram;
- (void)changeColorRange;
- (void)reinitializeHistograms;
- (void)clearHistogram:(struct Histogram *)histp;
- (void)drag_widget;
- (void)release_widget;

- (void)getCurrentMapMaxima: (float*)max
						   : (float*)min;
- (void)getCurrentRange: (float*)max
					   : (float*)min;

- (BOOL)setMaxLimit:(float)max;
- (BOOL)setMinLimit:(float)min;

- (void)click_widget:(float)wx
					:(float)wy_min
					:(float)wy_max;

- (void)drawBorderWithPixnum:(int)bpix
					   color:(float*)colorvec;

- (void)draw_widgets:(float)wx
					:(float)wy_min
					:(float)wy_max;

- (void)draw_histogram:(float)wx
					  :(float)wy_min
					  :(float)wy_max;


@end
