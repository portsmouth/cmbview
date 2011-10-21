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
* This code handles the drawing of the histograms in the small OpenGL view,  *
* and mouse control in that view.                                            *
*                                                                            *
*****************************************************************************/

#import "LittleOpenGLview.h"

@implementation LittleOpenGLview

/* setup OpenGL view for the histograms and GUI */
- (id)initWithFrame:(NSRect)frameRect
{
	NSOpenGLPixelFormatAttribute MyAttributes [] = 
	{	
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFADepthSize, 16,
		NSOpenGLPFASampleBuffers, 1, 
		NSOpenGLPFASamples, 6,
		NSOpenGLPFANoRecovery, 0, 
	};
	NSOpenGLPixelFormat* pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:MyAttributes];

	if ((self = [super initWithFrame:frameRect pixelFormat:pixFmt])==nil) 
	{
		printf("ERROR: could not initialize little openGL context");
		exit(EXIT_FAILURE);
	}
	[pixFmt autorelease];	
	oglContext_littleview = [self openGLContext];
	[oglContext_littleview makeCurrentContext];
	
	return self;
}

/* prepare display */
- (void)prepareOpenGL
{				
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE_ARB);
	glHint (GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);		
	//glEnable(GL_DEPTH_TEST);			

	NSRect rect=[self bounds];	
	littleview_width = rect.size.width;
	littleview_height = rect.size.height;		
	glViewport(0,0, rect.size.width, rect.size.height);	
		
	mouse_flag = 0;
	widget_flag = 0;

	//widget shape parameters
	widgetradius = 0.05f;
	xoffset = 0.01f;

	[self reinitializeHistograms];
}

/**********************************************************************/
/*                       handle histogram data                        */
/**********************************************************************/

- (void)reinitializeHistograms
{
	//default no-map widget positions
	current_maxwidget = 1.0f;
	current_minwidget = 0.0f;
		
	const widgets_coords widgets_coords_init = { 1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0 };
	widgets_interactive = widgets_coords_init;
	widgets_render = widgets_coords_init;
	widgets_presentation = widgets_coords_init;
	
	//set initial colorrange to full range
	float maxc = -1.0f + widgetradius + 2.0f*(1.0f-widgetradius);
	float minc = -1.0f + widgetradius;
	
	const colorrange_coords colorrange_coords_init = {maxc,minc,maxc,minc,maxc,minc,maxc,minc};
	colorrange_interactive = colorrange_coords_init;
	colorrange_render = colorrange_coords_init;
	colorrange_presentation = colorrange_coords_init;
	
	[self clearHistogram:[myCMBdata histogram_interactive]];
	[self clearHistogram:[myCMBdata histogram_render]];
	[self clearHistogram:[myCMBdata histogram_presentation]];
}

- (void)clearHistogram:(struct Histogram *)histp;
{
	int bin;
	for(bin=0;bin<Nbin;bin++)
	{
		histp->normThist[bin] = 0.0f; histp->normThistlog[bin] = 0.0f;
		histp->normQhist[bin] = 0.0f; histp->normQhistlog[bin] = 0.0f;
		histp->normUhist[bin] = 0.0f; histp->normUhistlog[bin] = 0.0f;
		histp->normPhist[bin] = 0.0f; histp->normPhistlog[bin] = 0.0f;
	}	
}

- (void)getCurrentMapMaxima: (float*)max
						   : (float*)min
{
	maptype = [myAppController maptype];
	
	switch (maptype) 
	{
		case 1: //T map
			*max = currentmapmaxima->maxT;
			*min = currentmapmaxima->minT;
			break;
			
		case 2: //Q map
			*max = currentmapmaxima->maxQ;
			*min = currentmapmaxima->minQ;
			break;

		case 3: //U map
			*max = currentmapmaxima->maxU;
			*min = currentmapmaxima->minU;
			break;

		case 4: //P map
			*max = currentmapmaxima->maxP;
			*min = currentmapmaxima->minP;
			break;
			
	}	
	
}

- (void)getCurrentRange: (float*)max
					   : (float*)min
{
	float colorrange_minval, colorrange_maxval;
	[self getCurrentMapMaxima: &colorrange_maxval: &colorrange_minval];
	
	*max = colorrange_minval + current_maxwidget * (colorrange_maxval-colorrange_minval);
	*min = colorrange_minval + current_minwidget * (colorrange_maxval-colorrange_minval);
}

/* if a new map has just been selected, setup pointers to histogram data for that map */
- (void)changeHistogram
{
	float colorrange_minval, colorrange_maxval;
	render = [myAppController render_mode];
					
	switch (render) 
	{
		case 1:
			current_widget = &widgets_interactive;
			currentmapmaxima = [myCMBdata mapmaxima_interactive];
			current_colorrange = [myAppController colorrange_interactive];
			current_colorrangecoords = &colorrange_interactive;
			current_hist = [myCMBdata histogram_interactive];
			break;
		
		case 2:
			current_widget = &widgets_render;
			currentmapmaxima = [myCMBdata mapmaxima_render];
			current_colorrange = [myAppController colorrange_render];
			current_colorrangecoords = &colorrange_render;
			current_hist = [myCMBdata histogram_render];
			break;
		
		case 3:
			current_widget = &widgets_presentation;
			currentmapmaxima = [myCMBdata mapmaxima_presentation];
			current_colorrange = [myAppController colorrange_presentation];
			current_colorrangecoords = &colorrange_presentation;
			current_hist = [myCMBdata histogram_presentation];
			break;
	}		
		
	//default no-map widget positions
	current_maxwidget = 1.0f;
	current_minwidget = 0.0f;
	
	maptype = [myAppController maptype];
	
	switch (maptype) 
	{
		case 1: //T map
			current_maxwidget = current_widget->maxT;
			current_minwidget = current_widget->minT;
			colorrange_maxval = currentmapmaxima->maxT;
			colorrange_minval = currentmapmaxima->minT;
			break;
		
		case 2: //Q map
			current_maxwidget = current_widget->maxQ;
			current_minwidget = current_widget->minQ;
			colorrange_maxval = currentmapmaxima->maxQ;
			colorrange_minval = currentmapmaxima->minQ;
			break;
		
		case 3: //U map
			current_maxwidget = current_widget->maxU;
			current_minwidget = current_widget->minU;
			colorrange_maxval = currentmapmaxima->maxU;
			colorrange_minval = currentmapmaxima->minU;
			break;
		
		case 4: //P map
			current_maxwidget = current_widget->maxP;
			current_minwidget = current_widget->minP;
			colorrange_maxval = currentmapmaxima->maxP;
			colorrange_minval = currentmapmaxima->minP;
			break;
	}	
	
	float newmax, newmin;
	newmax = colorrange_minval + current_maxwidget * (colorrange_maxval-colorrange_minval);
	newmin = colorrange_minval + current_minwidget * (colorrange_maxval-colorrange_minval);
	
	NSString *thewidgettext_max;
	thewidgettext_max = [[NSString alloc] initWithFormat:@"%8e",newmax];
	[myAppController setwidgetmax:thewidgettext_max];
	[thewidgettext_max release];
	
	NSString *thewidgettext_min;
	thewidgettext_min = [[NSString alloc] initWithFormat:@"%8e",newmin];
	[myAppController setwidgetmin:thewidgettext_min];
	[thewidgettext_min release];
		
	switch (maptype) 
	{
		case 1: //T map
			current_colorrange->maxT = newmax;
			current_colorrange->minT = newmin;	
			break;
		
		case 2: //Q map
			current_colorrange->maxQ = newmax;
			current_colorrange->minQ = newmin;				
			break;
		
		case 3: //U map
			current_colorrange->maxU = newmax;
			current_colorrange->minU = newmin;				
			break;
		
		case 4: //P map
			current_colorrange->maxP = newmax;
			current_colorrange->minP = newmin;			
			break;
	}
}

- (void)changeColorRange 
{
	/* if user has requested to update color range, set histogram
	   upper and lower color bounds to current widget viewport y-coords */
	
	float wx,wy_max,wy_min;

	wy_max = -1.0f + widgetradius + 2.0f*(1.0f-widgetradius) * current_maxwidget;
	wy_min = -1.0f + widgetradius + 2.0f*(1.0f-widgetradius) * current_minwidget;
	
	//find current mode
	render = [myAppController render_mode];
	
	switch (render) 
	{		
			case 1:			
				current_colorrangecoords = &colorrange_interactive;
				break;
			
			case 2:
				current_colorrangecoords = &colorrange_render;
				break;
			
			case 3:
				current_colorrangecoords = &colorrange_presentation;
				break;
	}			
	
	switch ([myAppController maptype]) 
	{
		case 1: //T map
			current_colorrangecoords->minT = wy_min;
			current_colorrangecoords->maxT = wy_max;
			break;
		
		case 2: //Q map
			current_colorrangecoords->minQ = wy_min;
			current_colorrangecoords->maxQ = wy_max;
			break;
		
		case 3: //U map
			current_colorrangecoords->minU = wy_min;
			current_colorrangecoords->maxU = wy_max;
			break;
		
		case 4: //P map
			current_colorrangecoords->minP = wy_min;
			current_colorrangecoords->maxP = wy_max;
			break;
	}
}


// set the widget positions by text entry in the histogram range fields
- (BOOL)setMaxLimit:(float)max
{
	if (!maptype) return FALSE;
	
	float mapmax, mapmin;
	[self getCurrentMapMaxima: &mapmax: &mapmin];
	
	if (mapmax == mapmin) return FALSE;
	float new_maxwidget = (max - mapmin) / (mapmax - mapmin);
	
	//do not allow values outside current map range
	if (new_maxwidget>1.0f || new_maxwidget<0.0f) return FALSE;

	//do not allow the max widget to move below the current min widget
	if (new_maxwidget < current_minwidget) return FALSE;
	
	//move the max widget
	current_maxwidget = new_maxwidget;
	widget_flag = 1;
	
	[self release_widget];
	[self changeColorRange];
	
	[self setNeedsDisplay:TRUE];
	return TRUE;
}

- (BOOL)setMinLimit:(float)min
{
	if (!maptype) return FALSE;
	
	float mapmax, mapmin;
	[self getCurrentMapMaxima: &mapmax: &mapmin];
	
	if (mapmax == mapmin) return FALSE;
	float new_minwidget = (min - mapmin) / (mapmax - mapmin);
	
	//do not allow values outside current map range
	if (new_minwidget>1.0f || new_minwidget<0.0f) return FALSE;
	
	//do not allow the min widget to move above the current max widget
	if (new_minwidget > current_maxwidget) return FALSE;
	
	//move the min widget
	current_minwidget = new_minwidget;
	widget_flag = 2;
	
	[self release_widget];
	[self changeColorRange];
	
	[self setNeedsDisplay:TRUE];
	return TRUE;
	
}


/**********************************************************************/
/*                         widget behaviour                           */
/**********************************************************************/

/* determine if max or min widget was clicked */
- (void)click_widget:(float)wx
					:(float)wy_min
					:(float)wy_max
{
	float xclick = -1.0f+2.0f*mousedown_initpos[0];
	float yclick = -1.0f+2.0f*mousedown_initpos[1];
	
	//select widget closest along vertical
	int clicked_max = (yclick > 0.5*(wy_max + wy_min));
	int clicked_min = (yclick < 0.5*(wy_max + wy_min));		
	
	//max widget
	if (clicked_max) 
	{
		widget_flag = 1;	
	}
	
	//min widget	
	else if (clicked_min) 
	{
		widget_flag = 2;	
	}
	
	else 
	{
		widget_flag = 0;				
	}
}

//on dragging mouse after clicking on a widget
- (void)drag_widget
{
	//first, get map maxima so we can compute value to display in text box
	float colorrange_minval,colorrange_maxval;
	if (maptype) 
	{
		[self getCurrentMapMaxima: &colorrange_maxval: &colorrange_minval];
	}
	
	//move the widget, making sure to do the Right Thing (TM) if the mouse 
	//moves out of range or the user tries to drag the two widgets across one another.	
		
	float dydrag = (mousedown_currentpos[1]-mousedown_initpos[1]) / (1.0f-widgetradius);		

	//max widget is dragged
	if (widget_flag==1) 
	{			
		current_maxwidget = clicked_maxwidget + dydrag;
		if (current_maxwidget>1.0f) current_maxwidget = 1.0f;		
		if (current_maxwidget<current_minwidget) current_maxwidget = current_minwidget;
		
		//if there is a map, print the data
		if (maptype)
		{				
			float newmax = colorrange_minval + current_maxwidget * (colorrange_maxval-colorrange_minval);
			NSString *thewidgettext;
			thewidgettext = [[NSString alloc] initWithFormat:@"%8e",newmax];
			[myAppController setwidgetmax:thewidgettext];
			[thewidgettext release];			
		}
	}		
	
	//min widget is dragged
	if (widget_flag==2)
	{
		current_minwidget = clicked_minwidget + dydrag;
		if (current_minwidget<0.0f) current_minwidget = 0.0f;		
		if (current_minwidget>current_maxwidget) current_minwidget = current_maxwidget;
		
		//if there is a map, print the data
		if (maptype) 
		{				
			float newmin = colorrange_minval + current_minwidget * (colorrange_maxval-colorrange_minval);
			NSString *thewidgettext;
			thewidgettext = [[NSString alloc] initWithFormat:@"%8e",newmin];
			[myAppController setwidgetmin:thewidgettext];
			[thewidgettext release];
		}
	}			
}


/* if a widget was moved, update the color range information
   in the AppController, and store the max and min values shown */
- (void)release_widget
{					
	float colorrange_minval,colorrange_maxval;

	switch (maptype)
	{
		case 1: //T map
			colorrange_maxval = currentmapmaxima->maxT;
			colorrange_minval = currentmapmaxima->minT;				
			current_widget->maxT = current_maxwidget;
			current_widget->minT = current_minwidget;
			break;
	
		case 2: //Q map
			colorrange_maxval = currentmapmaxima->maxQ;
			colorrange_minval = currentmapmaxima->minQ;				
			current_widget->maxQ = current_maxwidget;
			current_widget->minQ = current_minwidget;
			break;
		
		case 3: //U map
			colorrange_maxval = currentmapmaxima->maxU;
			colorrange_minval = currentmapmaxima->minU;				
			current_widget->maxU = current_maxwidget;
			current_widget->minU = current_minwidget;
			break;
	
		case 4: //P map
			colorrange_maxval = currentmapmaxima->maxP;
			colorrange_minval = currentmapmaxima->minP;				
			current_widget->maxP = current_maxwidget;
			current_widget->minP = current_minwidget;
			break;
	}
	
	float newmax, newmin;	
	
	//maximum widget was moved		
	if (widget_flag==1)
	{
		newmax = colorrange_minval + current_maxwidget * (colorrange_maxval-colorrange_minval);
		NSString *thewidgettext;
		thewidgettext = [[NSString alloc] initWithFormat:@"%8e",newmax];
		[myAppController setwidgetmax:thewidgettext];
		[thewidgettext release];
		
		switch (maptype) 
		{
			case 1: //T map
				current_colorrange->maxT = newmax;
				break;
			case 2: //Q map
				current_colorrange->maxQ = newmax;
				break;
			case 3: //U map
				current_colorrange->maxU = newmax;
				break;
			case 4: //P map
				current_colorrange->maxP = newmax;
				break;
		}
	}			
	
	//minimum widget was moved
	if (widget_flag==2) 
	{			
		newmin = colorrange_minval + current_minwidget * (colorrange_maxval-colorrange_minval);
		NSString *thewidgettext;
		thewidgettext = [[NSString alloc] initWithFormat:@"%8e",newmin];
		[myAppController setwidgetmin:thewidgettext];
		[thewidgettext release];
	
		switch (maptype)
		{
			case 1: //T map
				current_colorrange->minT = newmin;
				break;
			case 2: //Q map
				current_colorrange->minQ = newmin;
				break;
			case 3: //U map
				current_colorrange->minU = newmin;
				break;
			case 4: //P map
				current_colorrange->minP = newmin;
				break;
		}
	}									
}


/**********************************************************************/
/*                        draw histogram view                         */
/**********************************************************************/

- (void)drawRect:(NSRect)r
{	
	//find current mode
	render = [myAppController render_mode];
	maptype = [myAppController maptype];
	
	//TO DO: an option to zoom histogram range to current selection,
	//(and a button to zoom back out to full range). 
	
	//compute widget positions in view coords
	float wx,wy_max,wy_min;
	wx = 1.0f-widgetradius;			
	wy_max = -1.0f + widgetradius + 2.0f*(1.0f-widgetradius)*current_maxwidget;
	wy_min = -1.0f + widgetradius + 2.0f*(1.0f-widgetradius)*current_minwidget;
	
	//mouse control of widgets
	switch (mouse_flag) 
	{
		/* mouse clicked on the view */
		case 1: 
			[self click_widget:wx:wy_min:wy_max];
			//record the widget positions on the first click
			clicked_maxwidget = current_maxwidget;
			clicked_minwidget = current_minwidget;
			break;
			
		/* mouse dragged */	
		case 2:				
			if (widget_flag) [self drag_widget];		
			break;
			
		/* mouse released */	
		case 3:			
			if (maptype)
			{		
				[self release_widget];
				[self changeColorRange];
			}
			mouse_flag = 0;	
			widget_flag = 0;
			break;
	}
	
	//Make sure we're drawing in the right context	
	[oglContext_littleview makeCurrentContext];
				
	glClearColor(0.07f,0.1f,0.07f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//can I get the histogram border in cornflower blue? 
	float borderColor[3] = { 0.525f, 0.678f, 0.812f };
	
	[self drawBorderWithPixnum:8 color:borderColor];

	[self draw_widgets:   wx: wy_min: wy_max];	
	[self draw_histogram: wx: wy_min: wy_max];
	
	[oglContext_littleview flushBuffer];
}

/* draw a border around the viewport edge, bpix pixels wide */
- (void)drawBorderWithPixnum:(int)bpix
					   color:(float*)colorvec
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0,1.0,-1.0,1.0);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	float dpix_y = (float)bpix/littleview_height;
	float dpix_x = (float)bpix/littleview_width;
	
	glDisable(GL_LIGHTING);
	glColor4f(colorvec[0], colorvec[1], colorvec[2], 1.0);
	
	glBegin(GL_QUADS);			
	
	glVertex2f(-1.0,-1.0); 
	glVertex2f( 1.0,-1.0);		
	glVertex2f( 1.0,-1.0+dpix_y);
	glVertex2f(-1.0,-1.0+dpix_y);
	
	glVertex2f(-1.0,1.0); 
	glVertex2f( 1.0,1.0);		
	glVertex2f( 1.0,1.0-dpix_y);
	glVertex2f(-1.0,1.0-dpix_y);
	
	glVertex2f(-1.0,-1.0); 
	glVertex2f(-1.0, 1.0);		
	glVertex2f(-1.0+dpix_x, 1.0);
	glVertex2f(-1.0+dpix_x,-1.0);
	
	glVertex2f(1.0,-1.0); 
	glVertex2f(1.0, 1.0);		
	glVertex2f(1.0-dpix_x, 1.0);
	glVertex2f(1.0-dpix_x,-1.0);
	
	glEnd();	
}



- (void)draw_widgets:(float)wx
					:(float)wy_min
					:(float)wy_max
{		
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	int n,m;
	
	float ovl;
	float widg_x = widgetradius;
	float widg_y = widgetradius;
	int novl = 4;

	//max widget
	/*
	for (n=0;n<novl;n++) 
	{
		ovl = 1.05*((float)n)/((float)novl-1);
		glColor3f(1.0f,1.0f-ovl,1.0f-ovl);
		
		glBegin(GL_TRIANGLES);					
		glVertex3f( wx-ovl*widg_x, wy_max, 0.5f );		
		glVertex3f( wx+ovl*widg_x, wy_max, 0.5f );		
		glVertex3f( wx, wy_max+ovl*widg_y, 0.5f );				
		glEnd();
	}
	 */
	
	int mstrip = 32;
	float dw = (wy_max-wy_min)/(float)mstrip;
	float ivl;
	
	//strip between widgets
	/*
	for (n=0;n<novl;n++)
	{		
		ovl = 1.05*((float)n)/((float)novl-1);
		for (m=0;m<mstrip;m++)
		{
			ivl = (float)m/(mstrip-1);
			glColor3f(1.0f - ovl*ivl, 1.0f-ovl, 1.0f+ovl*(ivl-1.0f));
			
			glBegin(GL_QUADS);
			glVertex3f( wx-ovl*widg_x, wy_max-(float)m*dw, 0.5f );
			glVertex3f( wx+ovl*widg_x, wy_max-(float)m*dw, 0.5f );		
			glVertex3f( wx+ovl*widg_x, wy_max-(float)(m+1)*dw, 0.5f );		
			glVertex3f( wx-ovl*widg_x, wy_max-(float)(m+1)*dw, 0.5f );		
			glEnd();
		}
	}
	*/
		
	//min widget
	/*
	for (n=0;n<novl;n++)
	{
		ovl = 1.05*((float)n)/((float)novl-1);
		glColor3f(1.0f-ovl,1.0f-ovl,1.0);
		
		glBegin(GL_TRIANGLES);					
		glVertex3f( wx-ovl*widg_x, wy_min, 0.5f );		
		glVertex3f( wx+ovl*widg_x, wy_min, 0.5f );		
		glVertex3f( wx, wy_min-ovl*widg_y, 0.5f );				
		glEnd();
	}
	*/
	
	//draw dashed lines sticking out of widgets
	int widgetdashed = 17; //number of dashes
	glLineWidth(1.0f);
	
	float midpnt = 0.5f * (wy_max + wy_min);
	
	float dypix = 0.25f / (littleview_height - 2.0f*widgetradius);
	
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	for (n=0;n<widgetdashed;n++)
	{
		glVertex3f( xoffset + wx-0.5f*widgetradius-(float)n*2.0f*wx/(float)widgetdashed, wy_max, 0.1f );
	}
	glEnd();				
	glBegin(GL_LINES);
	for (n=0;n<widgetdashed;n++) 
	{
		glVertex3f( xoffset + wx-0.5*widgetradius-(float)n*2.0*wx/(float)widgetdashed, wy_min, 0.1f );
	}
	glEnd();
	
	glColor3f(1.0f,1.0f,0.0f);	
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	for (n=0;n<widgetdashed;n++)
	{
		glVertex3f( xoffset + wx-0.5*widgetradius-(float)n*2.0*wx/(float)widgetdashed, midpnt+dypix, 0.1f );
		glVertex3f( xoffset + wx-0.5*widgetradius-(float)n*2.0*wx/(float)widgetdashed, midpnt-dypix, 0.1f );
	}
	glEnd();
		
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINES);
	for (n=1;n<widgetdashed;n++) 
	{
		glVertex3f( xoffset + wx-0.5f*widgetradius-(float)n*2.0f*wx/(float)widgetdashed, wy_max, 0.1f );
	}
	glEnd();					
	glBegin(GL_LINES);
	for (n=1;n<widgetdashed;n++)
	{
		glVertex3f( xoffset + wx-0.5f*widgetradius-(float)n*2.0f*wx/(float)widgetdashed, wy_min, 0.1f );
	}
	glEnd();
	
	glPopAttrib();
}

/* draw histogram for current map */
- (void)draw_histogram:(float)wx
					  :(float)wy_min
					  :(float)wy_max
{	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	float minedge, maxedge;
	
	//find the current colorbar coordinates
	switch (maptype)
	{
		case 1: //T map
			minedge = current_colorrangecoords->minT;
			maxedge = current_colorrangecoords->maxT;
			break;
		
		case 2: //Q map
			minedge = current_colorrangecoords->minQ;
			maxedge = current_colorrangecoords->maxQ;
			break;
		
		case 3: //U map
			minedge = current_colorrangecoords->minU;
			maxedge = current_colorrangecoords->maxU;
			break;
		
		case 4: //P map
			minedge = current_colorrangecoords->minP;
			maxedge = current_colorrangecoords->maxP;
			break;
	}
	
	float color_x[3];
	BOOL reverse = [[NSUserDefaults standardUserDefaults] boolForKey:CMBview_colormapreversekey];
	int bin, mtype, n;
	float colorval, width, height, dbin, binx, biny, R, G, B, viewport_x, viewport_y;

	int logflag = [myAppController linlog_flag];
	
	dbin = 1.0/(float)Nbin;	
	
	if (maptype)
	{
		for (bin=0;bin<Nbin;bin++)
		{
			biny = (float)bin*dbin;
			
			switch (maptype) 
			{
				case 1:
					if (logflag)
					{
						binx = current_hist->normThistlog[bin];	
					}
					else
						binx = current_hist->normThist[bin];	
					break;
				
				case 2:
					if (logflag) 
					{
						binx = current_hist->normQhistlog[bin];	
					}
					else
						binx = current_hist->normQhist[bin];	
					break;
				
				case 3:
					if (logflag) 
					{
						binx = current_hist->normUhistlog[bin];	
					}
					else
						binx = current_hist->normUhist[bin];	
					break;
				
				case 4:
					if (logflag)
					{
						binx = current_hist->normPhistlog[bin];	
					}
					else
						binx = current_hist->normPhist[bin];	
					break;
			}			
			
			viewport_x = -1.0f + 1.5f*widgetradius + 2.0f*binx * (1.0f-1.75f*widgetradius);
			viewport_y = -1.0f + widgetradius + 2.0f*biny * (1.0f-widgetradius);
			
			if (viewport_y<minedge)
			{
				colorval = 0.0f;
			}
			else if (viewport_y>maxedge) 
			{
				colorval = 1.0;
			}
			else 
			{
				if (maxedge!=minedge) 
				{
					colorval = (viewport_y-minedge) / (maxedge-minedge);
				}
				else colorval = 0.5f;
			}
			
			//draw histogram polygons
			if ([myAppController colormap_flag]==0) 
			{
				//greyscale
				glColor3f(colorval,colorval,colorval);
			}
			else 
			{
				//color
				if (reverse) colorval = 1.0-colorval;
				colorpath(current_colormap_ptr->color_N,
						  current_colormap_ptr->color_xi,
						  current_colormap_ptr->color_ci,
						  colorval, color_x);	
				HSVtoRGB(&R,&G,&B,color_x[0],color_x[1],color_x[2]);						
				glColor3f(R,G,B);
			}
			
			glBegin( GL_QUADS );	
			glVertex3f( xoffset + -1.0f+1.5f*widgetradius, viewport_y, 0.0f ) ;		
			glVertex3f( xoffset + viewport_x, viewport_y, 0.0f ) ;		
			glVertex3f( xoffset + viewport_x, viewport_y + 2.0f*dbin*(1.0f-widgetradius), 0.0f ) ;		
			glVertex3f( xoffset + -1.0f+1.5*widgetradius, viewport_y + 2.0f*dbin*(1.0f-widgetradius), 0.0f ) ;		
			glEnd();										
		}
	}
	
	//draw border of histogram
	glColor3f(0.3f,0.3f,0.3f);
	glLineWidth(1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex3f( xoffset + -1.0f+1.5f*widgetradius, -1.0f+widgetradius, -0.5f );
	glVertex3f( -1.0f+widgetradius+2.0f*(1.0f-1.5f*widgetradius), -1.0f+widgetradius, -0.5f );
	glVertex3f( -1.0f+widgetradius+2.0f*(1.0f-1.5f*widgetradius), 1.0f-widgetradius, -0.5f );
	glVertex3f( xoffset + -1.0f+1.5f*widgetradius, 1.0f-widgetradius, -0.5f );
	glEnd();			
		
	glPopAttrib();
}

/**********************************************************************/
/*                          mouse events                              */
/**********************************************************************/

- (void)mouseDown:(NSEvent *)event
{
	NSPoint currentPoint;
	currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
	mousedown_initpos[0] = currentPoint.x/littleview_width;
	mousedown_initpos[1] = currentPoint.y/littleview_height;	
	mouse_flag = 1;	
	
	[self setNeedsDisplay:YES];
}

- (void)mouseDragged:(NSEvent *)event
{
	NSPoint currentPoint;
	currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
	mousedown_currentpos[0] = currentPoint.x/littleview_width;
	mousedown_currentpos[1] = currentPoint.y/littleview_height;	
	mouse_flag = 2;
	
	[self setNeedsDisplay:YES];
}

- (void)mouseUp:(NSEvent *)event
{
	NSPoint currentPoint;
	currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
	mousedown_currentpos[0] = currentPoint.x/littleview_width;
	mousedown_currentpos[1] = currentPoint.y/littleview_height;
	mouse_flag = 3;
	
	[self setNeedsDisplay:YES];
}

@end
