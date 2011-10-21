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
* This code handles the drawing of the sphere in the main OpenGL view,       *
* and mouse control in that view.                                            *
*                                                                            *
*****************************************************************************/

#include <ApplicationServices/ApplicationServices.h>
#import "OpenGLview.h"
#import "PreferenceController.h"

@implementation OpenGLview

/**********************************************************************/
/*        initialize openGL context and set-up notifications          */
/**********************************************************************/

- (id)initWithFrame:(NSRect)frameRect
{
	NSOpenGLPixelFormatAttribute MyAttributes [] =
	{	
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFAColorSize, 32,
		NSOpenGLPFADepthSize, 32,
		NSOpenGLPFASampleBuffers, 1, 
		NSOpenGLPFASamples, 6,
		NSOpenGLPFANoRecovery, 0, 
	};	
	
	NSOpenGLPixelFormat* pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:MyAttributes];
	
	if ((self = [super initWithFrame:frameRect pixelFormat:pixFmt])==nil)
	{
		NSLog(@"ERROR: could not initialize main openGL context");
		exit(EXIT_FAILURE);
	}	
	[pixFmt autorelease];
	oglContext_mainview = [self openGLContext];
	[oglContext_mainview makeCurrentContext];
	
	long swapInterval;
	swapInterval = 1;
	[[self openGLContext] setValues: &swapInterval		
					   forParameter: NSOpenGLCPSwapInterval ];
	
	[oglContext_mainview makeCurrentContext];
	
	//set up notifications of preference changes
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc addObserver:self
		   selector:@selector(handle_backgrndcolorChanged:)
			   name:@"CMBview_backgrndcolorChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_ambientlightChanged:)
			   name:@"CMBview_ambientlightChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_ambientmatChanged:)
			   name:@"CMBview_ambientmatChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_diffuselightChanged:)
			   name:@"CMBview_diffuselightChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_diffusematChanged:)
			   name:@"CMBview_diffusematChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_specularlightChanged:)
			   name:@"CMBview_specularlightChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_specularmatChanged:)
			   name:@"CMBview_specularmatChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_rotatelightthetaChanged:)
			   name:@"CMBview_rotatelightthetaChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_rotatelightphiChanged:)
			   name:@"CMBview_rotatelightphiChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_lightradiusChanged:)
			   name:@"CMBview_lightradiusChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_shininessChanged:)
			   name:@"CMBview_shininessChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_lightingenableChanged:)
			   name:@"CMBview_lightingenableChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_ambientenableChanged:)
			   name:@"CMBview_ambientenableChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_diffuseenableChanged:)
			   name:@"CMBview_diffuseenableChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_specularenableChanged:)
			   name:@"CMBview_specularenableChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_gridSpacingChanged:)
			   name:@"CMBview_gridSpacingChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_stokesPropsChanged:)
			   name:@"CMBview_stokesPropsChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_texelinterpolationChanged:)
			   name:@"CMBview_texelinterpolationChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_gridPropsChanged:)
			   name:@"CMBview_gridPropsChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_fovyChanged:)
			   name:@"CMBview_fovyChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_orthoChanged:)
			   name:@"CMBview_orthoChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_mousesensitivityChanged:)
			   name:@"CMBview_mousesensitivityChanged"
			 object:nil];
	[nc addObserver:self
		   selector:@selector(handle_presentationbackgrndcolorChanged:)
			   name:@"CMBview_presentationbackgrndcolorChanged"
			 object:nil];	
	[nc addObserver:self
		   selector:@selector(handle_presentationtextcolorChanged:)
			   name:@"CMBview_presentationtextcolorChanged"
			 object:nil];
							
	return self;
}

/**********************************************************************/
/*                         prepare display                            */
/**********************************************************************/

- (void)prepareOpenGL
{					
	// Configure the view
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE_ARB);
	glHint (GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	
	glLineWidth(1.0);
	glDisable(GL_LINE_STIPPLE);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glGenTextures(6,face_texs);
	glGenTextures((GLsizei)1,&render_tex);
	
	//generate the sphere mesh vertices and the corresponding
	//texture coords on the circumscribed cube
	initcoordsystem();
	genvertexcoords();
			
	//pointers to arrays used in drawing routines
	v1_display = vector(0,2);
	v2_display = vector(0,2);
	v3_display = vector(0,2);
	t1_display = vector(0,1);
	t2_display = vector(0,1);
	t3_display = vector(0,1);
	norm_display = vector(0,2);
	
	//initialize various flags and variables
	texelInterpolationFlag = [[NSUserDefaults standardUserDefaults] 
                                    integerForKey:CMBview_texinterpolatekey];		

	//mouse mode
	mouse_sensitivity = [[NSUserDefaults standardUserDefaults] 
                                    floatForKey:CMBview_mousesensitivitykey];
	draw_mousepoint_flag =0;
	
	//set light properties from user preferences or defaults
	[self initialize_lighting];
	
	//initialize observer position
	float zoom_init = 2.0;
	[self setViewZoom:zoom_init];
	[myAppController set_zoom_slider:zoom_init];		
	viewTheta = 45.0;
	viewPhi = 135.0;	

	//initialize grid properties
	GRID_NUM = 4*[[NSUserDefaults standardUserDefaults] integerForKey:CMBview_gridspacekey];

	NSColor *colorRGB;
	NSData *MAasData = [[NSUserDefaults standardUserDefaults]
                                objectForKey:CMBview_gridpropskey];
	NSMutableArray *myMutableArray = [NSKeyedUnarchiver 
											unarchiveObjectWithData:MAasData];
	enum gridproperties sp; 
	sp = grid_enable;
	
	NSNumber *gridenableValue = [myMutableArray objectAtIndex:sp];
	axis_flag = [gridenableValue boolValue];
	
	sp = grid_color;
	NSColor *color = [NSKeyedUnarchiver unarchiveObjectWithData:[myMutableArray objectAtIndex:sp]];
	colorRGB = [[color colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];
	gridRGBA[0] = [colorRGB redComponent];
	gridRGBA[1] = [colorRGB greenComponent];
	gridRGBA[2] = [colorRGB blueComponent];
	[colorRGB release];		
	
	sp = grid_opacity;
	NSNumber *gridopacityValue = [myMutableArray objectAtIndex:sp];
	gridRGBA[3] = [gridopacityValue floatValue];
	
	sp = grid_thickness;
	NSNumber *gridthicknessValue = [myMutableArray objectAtIndex:sp];
	grid_line_thickness = [gridthicknessValue floatValue];
	
	sp = gridincaps;
	NSNumber *gridincapsValue = [myMutableArray objectAtIndex:sp];
	gridincapsFlag = [gridincapsValue boolValue];
		
	//initialize stokes vector properties
	StokesLength = 0.7;	
	
	NSData *MAasData2 = [[NSUserDefaults standardUserDefaults]
                                objectForKey:CMBview_stokespropskey];
	NSMutableArray *myMutableArray2 = [NSKeyedUnarchiver 
											unarchiveObjectWithData:MAasData2];
	enum stokesproperties sp2; 
	sp2 = stokes_color;
	NSColor *color2 = [NSKeyedUnarchiver unarchiveObjectWithData:[myMutableArray2 objectAtIndex:sp2]];
	colorRGB = [[color2 colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];
	stokesRGBA[0] = [colorRGB redComponent];
	stokesRGBA[1] = [colorRGB greenComponent];
	stokesRGBA[2] = [colorRGB blueComponent];
	[colorRGB release];		
	
	sp2 = stokes_opacity;
	NSNumber *stokesopacityValue = [myMutableArray2 objectAtIndex:sp2];
	stokesRGBA[3] = [stokesopacityValue floatValue];
	
	sp2 = stokes_length;
	NSNumber *stokeslengthValue = [myMutableArray2 objectAtIndex:sp2];
	max_stokes_arcmin = [stokeslengthValue floatValue];		
	
	sp2 = stokes_thickness;
	NSNumber *stokesthicknessValue = [myMutableArray2 objectAtIndex:sp2];
	stokes_line_thickness = [stokesthicknessValue floatValue];
	
	sp2 = stokes_convention;
	NSNumber *stokesconventionValue = [myMutableArray2 objectAtIndex:sp2];
	stokesConventionFlag = [stokesconventionValue intValue];		
	
	//initialize background color
	NSData *colorAsData;
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_backgrndcolorkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red = [colorRGB redComponent]*255;
	int green = [colorRGB greenComponent]*255;
	int blue = [colorRGB blueComponent]*255;	
	[self setBackgroundColor:(float)red green:(float)green blue:(float)blue];
	[colorRGB release];

	//initialize presentation mode background and text colors
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_presentationbackgrndcolorkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	present_bgrd_RGBA[0] = [colorRGB redComponent];
	present_bgrd_RGBA[1] = [colorRGB greenComponent];
	present_bgrd_RGBA[2] = [colorRGB blueComponent];
	[colorRGB release];
	
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_presentationtextcolorkey];		
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	present_text_RGBA[0] = [colorRGB redComponent];
	present_text_RGBA[1] = [colorRGB greenComponent];
	present_text_RGBA[2] = [colorRGB blueComponent];
	[colorRGB release];	
	
	//Clear the background and prepare OpenGL projection
	glClearColor(backgroundRGB[0],backgroundRGB[1],backgroundRGB[2],0.0);
	glClear(GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT);		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	NSRect rect=[self bounds];
	obs.view_width = rect.size.width;
	obs.view_height = rect.size.height;
	obs.aspect_ratio = obs.view_width/obs.view_height;

	//field of view from user defaults
	fovy = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_fovykey];
	
	//orthographic mode?
	orthoEnabledFlag = [[NSUserDefaults standardUserDefaults] boolForKey:CMBview_orthokey];	
	
	//small distance between axis lines and sphere in radii
	obs.axisspace = 2.0e-3;
	
	//furthest allowed distance of observer from sphere center, in radii
	maxZoom = 200.0;

	//smallest allowed distance of observer from sphere surface, in radii
	minApproach = 0.01;
	
	//(these determine the following view frustum dimensions)	
	obs.near = minApproach-obs.axisspace;
	obs.far = (2.0*maxZoom+1.0)*radius;
	obs.frustum_height = 2.0*obs.near*tan(PI*fovy/360.0);
	obs.frustum_width = obs.aspect_ratio*obs.frustum_height;	
	gluPerspective(fovy,obs.aspect_ratio,obs.near,obs.far);	
	glViewport(0,0, rect.size.width, rect.size.height);	
		
}

- (void)initialize_lighting
{	
	NSData *colorAsData;
	NSColor *colorRGB;
	int red, green, blue;
	
	blackRGBA[0] = 0.0f; blackRGBA[1] = 0.0f;
	blackRGBA[2] = 0.0f; blackRGBA[3] = 1.0f;	
	whiteRGBA[0] = 1.0f; whiteRGBA[1] = 1.0f;
	whiteRGBA[2] = 1.0f; whiteRGBA[3] = 1.0f;
	
	//initialize lighting with user preferences or defaults
	lightRadius = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_lightradiuskey];
	lightTheta = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_lightthetakey];
	lightPhi = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_lightphikey];	
	shininess = [[NSUserDefaults standardUserDefaults] floatForKey:CMBview_shininesskey];	
	
	lightingEnabledFlag = [[NSUserDefaults standardUserDefaults] 
                                   boolForKey:CMBview_lightingenabledkey];
	ambientEnabledFlag = [[NSUserDefaults standardUserDefaults] 
                                   boolForKey:CMBview_ambientenabledkey];
	diffuseEnabledFlag = [[NSUserDefaults standardUserDefaults] 
                                   boolForKey:CMBview_diffuseenabledkey];
	specularEnabledFlag = [[NSUserDefaults standardUserDefaults] 
                                   boolForKey:CMBview_specularenabledkey];

	//ambient color
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_ambientlightkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;	
	[self setAmbientLight:(float)red:(float)green:(float)blue];
	[colorRGB release];
	
	//ambient mat
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_ambientmatkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;	
	[self setAmbientMat:(float)red:(float)green:(float)blue];
	[colorRGB release];
	
	//diffuse color
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_diffuselightkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;	
	[self setDiffuseLight:(float)red:(float)green:(float)blue];
	[colorRGB release];
	
	//diffuse mat
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_diffusematkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;	
	[self setDiffuseMat:(float)red:(float)green:(float)blue];
	[colorRGB release];
	
	//specular color
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_specularlightkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;	
	[self setSpecularLight:(float)red:(float)green:(float)blue];
	[colorRGB release];
	
	//specular mat
	colorAsData = [[NSUserDefaults standardUserDefaults] 
						    objectForKey:CMBview_specularmatkey];
	colorRGB = [[[NSKeyedUnarchiver unarchiveObjectWithData:colorAsData]
						    colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;	
	[self setSpecularMat:(float)red:(float)green:(float)blue];
	[colorRGB release];
		
	glEnable(GL_LIGHT0);	
}

- (void)makeThisViewCurrentContext
{
	[oglContext_mainview makeCurrentContext];
}
- (void)updateContext
{
	[oglContext_mainview update];
}

- (NSOpenGLContext*)getContext
{
	return oglContext_mainview;
}

- (void)reshape
{
	[oglContext_mainview makeCurrentContext];
	obs.view_width = [self bounds].size.width;
	obs.view_height = [self bounds].size.height;
	obs.aspect_ratio = obs.view_width/obs.view_height;
	glViewport(0,0, obs.view_width, obs.view_height);
}


/**********************************************************************/
/*                         drawing routines                           */
/**********************************************************************/

/* draw main openGL view */
- (void)drawRect:(NSRect)r
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	[oglContext_mainview makeCurrentContext];	
	
	int render = [myAppController render_mode];		
	switch (render) 
	{
		
		case 1: //interactive mode:
			
			glClearColor(backgroundRGB[0],backgroundRGB[1],backgroundRGB[2],1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
			
			[self setup_lighting];
			[self setup_observer:fovy:orthoEnabledFlag];
			[self draw_interactivemode];
			[self drawAxes:fovy:orthoEnabledFlag];
			if ([myAppController Stokes_flag]) 
				[self drawStokes:fovy:orthoEnabledFlag];
			[self draw_mousepoint:fovy];
			break;
			
		case 2: //render mode:
			
			glClearColor(backgroundRGB[0],backgroundRGB[1],backgroundRGB[2],1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			[self setup_lighting];
			[self setup_observer:fovy_render:orthoEnabledFlag_render];
			[self draw_rendermode];	
			[self drawAxes:fovy_render:orthoEnabledFlag_render];
			if ([myAppController Stokes_flag]) 
				[self drawStokes:fovy_render:orthoEnabledFlag_render];
			[self draw_mousepoint:fovy_render];
			break;
	
		case 3:	//presentation mode:
			
			glClearColor(present_bgrd_RGBA[0],present_bgrd_RGBA[1],present_bgrd_RGBA[2],1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			[self setup_lighting];
			[self setup_observer:fovy_render:orthoEnabledFlag_render];
			[self draw_presentationmode];
			[self draw_mousepoint:fovy_render];
			break;
			
	}	
	
	glDisable(GL_DEPTH_TEST);				
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	//dark grey border
	float colorvec[3] = {0.2,0.2,0.2};
	[self drawBorderWithPixnum:3 color:colorvec];	
	
	[oglContext_mainview flushBuffer];	
	
	glPopAttrib();
}	

- (void)setup_lighting
{	
	float lightTheta_rad, lightPhi_rad;		
	
	lightTheta_rad = PI*(lightTheta/180.0);
	lightPhi_rad = 2.0*PI*(lightPhi/360.0);
	light_position[0] = sin(lightTheta_rad)*cos(lightPhi_rad);
	light_position[1] = cos(lightTheta_rad);
	light_position[2] = sin(lightTheta_rad)*sin(lightPhi_rad);	
	light_position[3] = 1.0;
	int a;
	for (a=0;a<3;a++)
	{
		light_position[a] *= lightRadius;
	}

	if (lightingEnabledFlag) 
	{
		glEnable(GL_LIGHTING);
		glColor3f(1.0,1.0,1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
		if (ambientEnabledFlag)
		{
			glLightfv(GL_LIGHT0, GL_AMBIENT, ambientlightRGBA);
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambientmatRGBA);
		}
		else
		{
			glLightfv(GL_LIGHT0, GL_AMBIENT, blackRGBA);
			glMaterialfv(GL_FRONT, GL_AMBIENT, blackRGBA);
		}
		
		if (diffuseEnabledFlag) 
		{
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuselightRGBA);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusematRGBA);
		}
		else 
		{
			glLightfv(GL_LIGHT0, GL_DIFFUSE, blackRGBA);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blackRGBA);
		}
		
		if (specularEnabledFlag) 
		{
			glLightfv(GL_LIGHT0, GL_SPECULAR, specularlightRGBA);
			glMaterialfv(GL_FRONT, GL_SPECULAR, specularmatRGBA);
			glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);	
		}
		else 
		{
			glLightfv(GL_LIGHT0, GL_SPECULAR, blackRGBA);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blackRGBA);
		}
	}
	else
	{
		glDisable(GL_LIGHTING);
		glColor3f(1.0,1.0,1.0);
	}
}

//observer coord system
- (void)setup_observer:(float)fovy_current:(BOOL)orthoFlag
{
	int a;
	float viewTheta_rad,viewPhi_rad,zoom,zoomfactor;	
	
	viewTheta_rad = PI*(viewTheta/180.0);
	viewPhi_rad = 2.0*PI*(viewPhi/360.0);
	obs.view_direction[0] = -cos(viewPhi_rad)*sin(viewTheta_rad);
	obs.view_direction[1] = -sin(viewPhi_rad)*sin(viewTheta_rad);
	obs.view_direction[2] = -cos(viewTheta_rad);
	
	obs.localy[0] = -cos(viewTheta_rad)*cos(viewPhi_rad);
	obs.localy[1] = -cos(viewTheta_rad)*sin(viewPhi_rad);
	obs.localy[2] = sin(viewTheta_rad);	
	obs.localx[0] = -sin(viewPhi_rad);
	obs.localx[1] = cos(viewPhi_rad);
	obs.localx[2] = 0.0;
		
	zoom = ([self viewZoom]/5.0);
	
	if (!orthoFlag)
	{
		maxZoom = 3.0/(float)tan((double)0.5*fovy_current*PI/180.0);
		zoomfactor = 2.0*(maxZoom-1.0-minApproach)/(zoom+1.0)+2.0-maxZoom+2.0*minApproach;
		obs.eyedistance_perspective = radius*zoomfactor;
		for (a=0;a<3;a++) 
		{
			obs.view_point[a] = obs.eyedistance_perspective*(-obs.view_direction[a]);
		}	
	}
	else 
	{
		obs.eyedistance_ortho = radius*(1.0+minApproach+5.0*(1.0-zoom));
		for (a=0;a<3;a++) 
		{
			obs.view_point[a] = obs.eyedistance_ortho*(-obs.view_direction[a]);
		}	
	}
								
}

- (void)draw_interactivemode
{
	int a,i,j,vind;
	float tex_coords1[2],tex_coords2[2],tex_coords3[2];	
	float d1[3],d2[3],norm[3],vert1[3],vert2[3],vert3[3];
	
	glEnable(GL_DEPTH_TEST);				
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (!orthoEnabledFlag) 
	{
		gluPerspective(fovy,obs.aspect_ratio,obs.near,obs.far);		
	}
	else
	{
		obs.frustum_height = (obs.eyedistance_ortho-radius);
		obs.frustum_width = obs.aspect_ratio * obs.frustum_height;
		glOrtho(-0.5*obs.frustum_width,0.5*obs.frustum_width,
				-0.5*obs.frustum_height,0.5*obs.frustum_height,
				-200.0*radius,200.0*radius);
	}
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(obs.view_point[0],obs.view_point[1],obs.view_point[2],
			  0.0,0.0,0.0,obs.localy[0],obs.localy[1],obs.localy[2]);
		
	glEnable(GL_TEXTURE_2D);	
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0,1.0);
	
	float priorities[6],facenormal[3];
	int face;
	for (face=0;face<6;face++) 
	{
		for (i=0;i<3;i++)
		{
			facenormal[i] = cubecoords.local_z[face][i];
		}	
		priorities[face] = -scalarprod(facenormal,obs.view_direction);
		if (priorities[face]<0.0) priorities[face] = 0.0;		
	}
	glPrioritizeTextures(6, face_texs,priorities);
	
	link tlist[6];	
		
	for (face=0;face<6;face++) 
	{
		glBindTexture(GL_TEXTURE_2D,face_texs[face]);		
		if (texelInterpolationFlag)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		}
		else 
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
		}			
		
		glBegin(GL_TRIANGLES);		
		
		//point to first node of list 
		tlist[face] = triangles_on_face[face]->next;		
		
		//advance through the list of all the triangles on this face
		while (tlist[face] != NULL)
		{
			v1_display = &(tlist[face]->vertex1[0]);
			v2_display = &(tlist[face]->vertex2[0]);
			v3_display = &(tlist[face]->vertex3[0]);
			t1_display = &(tlist[face]->texcoords1[0]);
			t2_display = &(tlist[face]->texcoords2[0]);
			t3_display = &(tlist[face]->texcoords3[0]);	
			norm_display = &(tlist[face]->normal[0]);			
			
			if (scalarprod(norm_display,obs.view_direction)<0.0)
			{				
				drawtriangle_projected(v1_display, v2_display, v3_display,
									   t1_display, t2_display, t3_display);
			}
			tlist[face] = tlist[face]->next;	
		}			
		glEnd();														
	}
}

- (void)setFovy_render:(float)fr
{
	fovy_render = fr;
}
- (float)fovy_render
{
	return fovy_render;
}
- (void)setOrthoEnabledFlag_render:(BOOL)flag
{
	orthoEnabledFlag_render = flag;
}

- (void)draw_rendermode
{
	int i,j,vind;
	float d1[3],d2[3],norm[3],vert1[3],vert2[3],vert3[3];
			
	//draw untextured lit sphere, no blending
	glEnable(GL_DEPTH_TEST);				
	glDisable(GL_TEXTURE_2D);	
	glDisable(GL_BLEND);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (!orthoEnabledFlag_render) 
	{
		gluPerspective(fovy_render,obs.aspect_ratio,obs.near,obs.far);		
	}
	else 
	{
		obs.frustum_height = (obs.eyedistance_ortho-radius);
		obs.frustum_width = obs.aspect_ratio * obs.frustum_height;
		glOrtho(-0.5*obs.frustum_width,0.5*obs.frustum_width,
				-0.5*obs.frustum_height,0.5*obs.frustum_height,
				-200.0*radius,200.0*radius);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(obs.view_point[0],obs.view_point[1],obs.view_point[2],
			  0.0,0.0,0.0,obs.localy[0],obs.localy[1],obs.localy[2]);
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0,1.0);
	
	link tlist[6];
	int face;
	for (face=0;face<6;face++) 
	{
		glBegin(GL_TRIANGLES);		
		tlist[face] = triangles_on_face[face];		
		while (tlist[face]->next != NULL)
		{
			tlist[face] = tlist[face]->next;	
			v1_display = &(tlist[face]->vertex1[0]);
			v2_display = &(tlist[face]->vertex2[0]);
			v3_display = &(tlist[face]->vertex3[0]);
			norm_display = &(tlist[face]->normal[0]);			
			
			if (scalarprod(norm_display,obs.view_direction)<0.0)
			{				
				drawtriangle_projected_notex(v1_display, v2_display, v3_display);									   
			}			
		}			
		glEnd();														
	}
	
	//draw a textured quad to exactly cover the viewport.
	//blend texture with previously drawn sphere to 
	//simulate a lit textured sphere in GL_MODULATE mode
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	glEnable(GL_BLEND);	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);		
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);		
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0,1.0,-1.0,1.0,-1.0,1.0);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D,render_tex);
	
	if (texelInterpolationFlag)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}
	else 
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	}
	
	glBegin(GL_QUADS);				
	glNormal3f(0.0,0.0,1.0);				
	glTexCoord2f(0.0,0.0);
	
	glVertex3f(-1.0,-1.0,0.0);		
	glTexCoord2f(0.0,1.0);
	glVertex3f(1.0,-1.0,0.0);		
	
	glTexCoord2f(1.0,1.0);
	glVertex3f(1.0,1.0,0.0);		
	glTexCoord2f(1.0,0.0);
	glVertex3f(-1.0,1.0,0.0);		
	glEnd();
				
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);							
}

//sets colormap reverse flag to correctly display colorbar 
- (void)setRenderwith_colormap_reverse:(BOOL)reverseFlag
{
	renderwith_colormap_reverse = reverseFlag;
}

- (void)draw_presentationmode
{
	int i,j,vind;
	float d1[3],d2[3],norm[3],vert1[3],vert2[3],vert3[3];
	
	NSRect rect=[self bounds];
					
	//draw untextured lit sphere, no blending
	glEnable(GL_DEPTH_TEST);				
	glDisable(GL_TEXTURE_2D);	
	glDisable(GL_BLEND);
	
	glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );
	
	float boundarywidth = 0.1;
	glViewport(boundarywidth*rect.size.width, 
			   1.5*boundarywidth*rect.size.height, 
			   (1.0-2.0*boundarywidth)*rect.size.width, 
			   (1.0-2.0*boundarywidth)*rect.size.height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (!orthoEnabledFlag_render) 
	{
		gluPerspective(fovy_render,obs.aspect_ratio,obs.near,obs.far);		
	}
	else
	{
		obs.frustum_height = (obs.eyedistance_ortho-radius);
		obs.frustum_width = obs.aspect_ratio*obs.frustum_height;
		glOrtho(-0.5*obs.frustum_width,0.5*obs.frustum_width,
				-0.5*obs.frustum_height,0.5*obs.frustum_height,
				-200.0*radius,200.0*radius);
	}
			
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(obs.view_point[0],obs.view_point[1],obs.view_point[2],
			  0.0,0.0,0.0,obs.localy[0],obs.localy[1],obs.localy[2]);
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0,1.0);
	
	link tlist[6];
	int face;
	for (face=0;face<6;face++) 
	{
		glBegin(GL_TRIANGLES);		
		tlist[face] = triangles_on_face[face];		
		
		while (tlist[face]->next != NULL) 
		{
			tlist[face] = tlist[face]->next;	
			v1_display = &(tlist[face]->vertex1[0]);
			v2_display = &(tlist[face]->vertex2[0]);
			v3_display = &(tlist[face]->vertex3[0]);
			norm_display = &(tlist[face]->normal[0]);			
			
			if (scalarprod(norm_display,obs.view_direction)<0.0) 
			{				
				drawtriangle_projected_notex(v1_display, v2_display, v3_display);									   
			}			
		}			
		glEnd();														
	}
	
	//draw a textured quad to exactly cover the viewport.
	//blend texture with previously drawn sphere to 
	//simulate a lit textured sphere in GL_MODULATE mode

	glDisable(GL_DEPTH_TEST);				
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	glEnable(GL_BLEND);	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);		
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);		

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0,1.0,-1.0,1.0,-1.0,1.0);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D,render_tex);
	
	if (texelInterpolationFlag)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	}
		
	glBegin(GL_QUADS);				
	glNormal3f(0.0,0.0,1.0); 
	glTexCoord2f(0.0,0.0);
	glVertex3f(-1.0,-1.0,0.0);		
	glTexCoord2f(0.0,1.0);
	glVertex3f(1.0,-1.0,0.0);		
	glTexCoord2f(1.0,1.0);
	glVertex3f(1.0,1.0,0.0);		
	glTexCoord2f(1.0,0.0);
	glVertex3f(-1.0,1.0,0.0);		
	glEnd();
	
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);				
	[self drawAxes:fovy_render:orthoEnabledFlag_render];
	if ([myAppController Stokes_flag]) [self drawStokes:fovy_render:orthoEnabledFlag_render];
	glDisable(GL_DEPTH_TEST);				

	[self drawBorderWithPixnum:4 color:present_text_RGBA];
	
	glPopAttrib();

	//draw colorbar
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0,1.0,-1.0,1.0);		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
		
	glViewport(0,0, rect.size.width, rect.size.height);
	glColor4f(present_text_RGBA[0],present_text_RGBA[1],present_text_RGBA[2],1.0);

	float barheight = 0.02;
	float barmargin = 0.0;	
	float by2 = -1.0+2.0*boundarywidth-2.0*barmargin;
	float by1 = by2 - 2.0*barheight;

	glBegin(GL_LINE_LOOP);
	glVertex2f(-1.0+2.0*boundarywidth,by2);
	glVertex2f(1.0-2.0*boundarywidth,by2);
	glVertex2f(1.0-2.0*boundarywidth,by1);
	glVertex2f(-1.0+2.0*boundarywidth,by1);
	glEnd();
	
	float barwidth = 2.0-4.0*boundarywidth;
	
	int Nquad = 128;
	int n;
	float scalar,R,G,B,color_x[3],bx1,bx2;
	float quadwidth = barwidth/(float)Nquad;
	for (n=0;n<Nquad;n++)
	{
		scalar = (float)n/Nquad;
		colorpath(current_colormap_ptr->color_N,
				  current_colormap_ptr->color_xi,
				  current_colormap_ptr->color_ci,
				  scalar, color_x);		
		if (renderwith_colormap_reverse) scalar = 1.0-scalar-1.0/(float)Nquad;
		
		HSVtoRGB(&R,&G,&B,color_x[0],color_x[1],color_x[2]);			
		bx1 = scalar*barwidth + (-1.0+2.0*boundarywidth);
		bx2 = bx1 + quadwidth;
		glBegin(GL_QUADS);
		glColor4f(R,G,B,1.0);
		glVertex2f(bx1,by1);
		glVertex2f(bx2,by1);
		glVertex2f(bx2,by2);
		glVertex2f(bx1,by2);
		glEnd();
	}	

	//draw colorbar text
	float barmin, barmax;
	colorrange *current_colorrange = [myAppController colorrange_presentation];
	if ([myAppController maptype]==1)
	{
		//T map
		barmin=current_colorrange->minT;
		barmax=current_colorrange->maxT;
	}			
	if ([myAppController maptype]==2)
	{
		//Q map
		barmin=current_colorrange->minQ;
		barmax=current_colorrange->maxQ;
	}
	if ([myAppController maptype]==3)
	{
		//U map
		barmin=current_colorrange->minU;
		barmax=current_colorrange->maxU;
	}
	if ([myAppController maptype]==4) 
	{
		//P map
		barmin=current_colorrange->minP;
		barmax=current_colorrange->maxP;
	}					
	
	NSString *bartext_min, *bartext_max;
	bartext_min = [[NSString alloc] initWithFormat:@"%5.4e",barmin];
	bartext_max = [[NSString alloc] initWithFormat:@"%5.4e",barmax];
	
	glColor4f(present_text_RGBA[0],present_text_RGBA[1],present_text_RGBA[2],1.0);

	glRasterPos2f(-0.8,-0.9); 		
	for( i=0; i < (int)[bartext_min length]; i++) 
	{
		glutBitmapCharacter( GLUT_BITMAP_8_BY_13, [bartext_min characterAtIndex:i] );
	}				
	glRasterPos2f(0.55,-0.9); 		
	for( i=0; i < (int)[bartext_max length]; i++) 
	{
		glutBitmapCharacter( GLUT_BITMAP_8_BY_13, [bartext_max characterAtIndex:i] );
	}
	[bartext_min release];
	[bartext_max release];
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

- (void)drawStokes:(float)fovy_current:(BOOL)orthoFlag
{
	glShadeModel(GL_FLAT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (!orthoFlag)
	{
		gluPerspective(fovy_current,obs.aspect_ratio,obs.near,obs.far);		
	}
	else 
	{
		obs.frustum_height = (obs.eyedistance_ortho-radius);
		obs.frustum_width = obs.aspect_ratio * obs.frustum_height;
		glOrtho(-0.5*obs.frustum_width,0.5*obs.frustum_width,
				-0.5*obs.frustum_height,0.5*obs.frustum_height,
				-200.0*radius,200.0*radius);
	}	
			
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	gluLookAt(obs.view_point[0],obs.view_point[1],obs.view_point[2],
			  0.0,0.0,0.0,obs.localy[0],obs.localy[1],obs.localy[2]);	
	
	glLineWidth((GLfloat)stokes_line_thickness);
	
	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(stokesRGBA[0], stokesRGBA[1], stokesRGBA[2], stokesRGBA[3]);		
	
	int a,b,c;
	float spherevec[3], vert1[3], vert2[3], stokelength;
	double thetas, phis;
	float max_length = 0.5*radius*(max_stokes_arcmin/60.0)*PI/180.0;
			
	int nStokes = [myCMBdata NStokes];
		
	for (a=0;a<nStokes;a++)
	{	
		for (b=0;b<nStokes;b++) 
		{
			if (stokes_ptrs.Stokes_mask[a][b]) 
			{
				thetas = stokes_ptrs.Stokes_theta_proj[a][b];
				phis = stokes_ptrs.Stokes_phi_proj[a][b];		
				
				//TO DO: these might as well be pre-computed
				spherevec[0] = (1.0+obs.axisspace) * radius * (float)sin(thetas)*cos(phis);
				spherevec[1] = (1.0+obs.axisspace) * radius * (float)sin(thetas)*sin(phis);
				spherevec[2] = (1.0+obs.axisspace) * radius * (float)cos(thetas);
				
				//StokesLength contains current user selected de-magnification factor, in [0,1]
				stokelength = max_length*StokesLength*stokes_ptrs.Stokes_headless_mag[a][b];										
				for (c=0; c<3; c++)
				{
					vert1[c] = (spherevec[c] + stokelength*stokes_ptrs.Stokes_headless[a][b][c]);
					vert2[c] = (spherevec[c] - stokelength*stokes_ptrs.Stokes_headless[a][b][c]);
				}
				
				glBegin(GL_LINES);
				glVertex3f(vert1[0],vert1[1],vert1[2]);
				glVertex3f(vert2[0],vert2[1],vert2[2]);
				glEnd();						
			}							
		}
	}
	glLineWidth((GLfloat)1.0);
	glShadeModel(GL_SMOOTH);
}

//TO DO: it would be much more efficient to compute the grid vertices once
//rather than having to call a lot of trig functions every frame.
- (void)drawAxes:(float)fovy_current:(BOOL)orthoFlag
{			
	//if axis button is toggled on, draw axes, which are GRID_NUM lines of longitude and GRID_NUM/2 - 1 lines of latitude	
	double GRID_SPACE = 360.0/(double)GRID_NUM;
	
	int gc_i, gc_j, Ngc_arc;
	double gc_vec1[3], gc_vec2[3], gc_vec[3], gc_last[3], gc_lambda;
	double gc_arc, gc_mu, gc_norm, gc_rad, gc_theta1, gc_phi1, gc_theta2;
    double gc_phi2, gc_theta, gc_theta3, gc_phi;
	
	if (axis_flag) 
	{
		//glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glLineWidth((GLfloat)grid_line_thickness);
		
		glEnable(GL_BLEND);	
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(gridRGBA[0],gridRGBA[1],gridRGBA[2],gridRGBA[3]);		
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (!orthoFlag)
		{
			gluPerspective(fovy_current,obs.aspect_ratio,obs.near,obs.far);		
		}
		else 
		{
			obs.frustum_height = (obs.eyedistance_ortho-radius);
			obs.frustum_width = obs.aspect_ratio*obs.frustum_height;
			glOrtho(-0.5*obs.frustum_width,0.5*obs.frustum_width,
					-0.5*obs.frustum_height,0.5*obs.frustum_height,
					-200.0*radius,200.0*radius);
		}	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();	
		gluLookAt(obs.view_point[0],obs.view_point[1],obs.view_point[2],
				  0.0,0.0,0.0,obs.localy[0],obs.localy[1],obs.localy[2]);	
		
		//longitude lines are great circles from pole to pole
		if (gridincapsFlag) 
		{
			gc_theta1 = 0.0;
			gc_theta3 = M_PI;}
		else 
		{
			gc_theta1 = GRID_SPACE/180.0*M_PI;
			gc_theta3 = M_PI-gc_theta1;
		}
		gc_theta2 = 0.5*M_PI;			
		Ngc_arc = 32; //numbers of line segments per quarter circle		
		
		//prime meridian is plotted with a stippled line
		gc_phi1 = 0.0;
		glLineStipple(1,0x3F07);
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINE_STRIP);
		greatcircle(gc_theta1,gc_phi1,gc_theta2,gc_phi1,Ngc_arc);
		greatcircle(gc_theta2,gc_phi1,gc_theta3,gc_phi1,Ngc_arc);
		glEnd();				
		glDisable(GL_LINE_STIPPLE);
		for (gc_i=1;gc_i<GRID_NUM;gc_i++) 
		{
			glBegin(GL_LINE_STRIP);
			gc_phi1 = (double)gc_i/GRID_NUM * 2.0*M_PI;
			greatcircle(gc_theta1,gc_phi1,gc_theta2,gc_phi1,Ngc_arc);
			greatcircle(gc_theta2,gc_phi1,gc_theta3,gc_phi1,Ngc_arc);
			glEnd();				
		}
		
		//latitude lines are small circles of constant theta
		for (gc_i=1;gc_i<GRID_NUM/4;gc_i++)
		{	
			glBegin(GL_LINE_STRIP);
			gc_theta1 = (double)gc_i*GRID_SPACE/180.0*M_PI;
			latitude(gc_theta1,0.0,M_PI,2*Ngc_arc);
			latitude(gc_theta1,M_PI,2.0*M_PI,2*Ngc_arc);
			glEnd();				
		}
		
		//draw equator stippled
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINE_STRIP);
		gc_theta1 = (double)0.5*M_PI;
		latitude(gc_theta1,0.0,M_PI,2*Ngc_arc);
		latitude(gc_theta1,M_PI,2.0*M_PI,2*Ngc_arc);
		glEnd();				
		
		glDisable(GL_LINE_STIPPLE);
		for (gc_i=GRID_NUM/4+1;gc_i<GRID_NUM/2;gc_i++)
		{	
			glBegin(GL_LINE_STRIP);
			gc_theta1 = (double)gc_i*GRID_SPACE/180.0*M_PI;
			latitude(gc_theta1,0.0,M_PI,2*Ngc_arc);
			latitude(gc_theta1,M_PI,2.0*M_PI,2*Ngc_arc);
			glEnd();				
		}
	}
}

/* on right mouse click on sphere, draw a tiny sphere to indicate the clicked location */
- (void)draw_mousepoint:(float)fovy_current
{
	int npixels = 1; //number of pixels for mouse point
	float mouseverts[3], smallsphererad;		
	
	glColor4f(1.0,1.0,1.0,1.0);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
		
	if (draw_mousepoint_flag)
	{
		if (!orthoEnabledFlag) 
		{
			gluPerspective(fovy_current,obs.aspect_ratio,obs.near,obs.far);
			smallsphererad = (float)npixels/obs.view_height*(PI/180.0) * fovy_current * (obs.eyedistance_perspective-radius);
		}
		else
		{
			obs.frustum_height = (obs.eyedistance_ortho-radius);
			obs.frustum_width = obs.aspect_ratio * obs.frustum_height;
			glOrtho(-0.5*obs.frustum_width,0.5*obs.frustum_width,
					-0.5*obs.frustum_height,0.5*obs.frustum_height,
					-200.0*radius,200.0*radius);
			smallsphererad = (float)npixels/obs.view_height*obs.frustum_height;
		}	
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();	
		gluLookAt(obs.view_point[0],obs.view_point[1],obs.view_point[2],
				  0.0,0.0,0.0,obs.localy[0],obs.localy[1],obs.localy[2]);
		
		mouseverts[0] = (radius+smallsphererad)*cos(down_rightmouse_phi)*sin(down_rightmouse_theta);
		mouseverts[1] = (radius+smallsphererad)*sin(down_rightmouse_phi)*sin(down_rightmouse_theta);
		mouseverts[2] = (radius+smallsphererad)*cos(down_rightmouse_theta);
	
		glPushMatrix();	
		glTranslatef(mouseverts[0],mouseverts[1],mouseverts[2]);		
		glutSolidSphere(smallsphererad,10.0,10.0);
		glPopMatrix();
	}	
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
	
	float dpix_y = (float)bpix/obs.view_height;
	float dpix_x = (float)bpix/obs.view_width;
	
	glDisable(GL_LIGHTING);
	glColor4f(colorvec[0],colorvec[1],colorvec[2],1.0);
	
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



/**********************************************************************/
/*                         mouse events                               */
/**********************************************************************/

- (BOOL)acceptsFirstResponder
{	
	return YES;	
}

- (void)mouseDown:(NSEvent *)event
{
	if ([event modifierFlags] & NSControlKeyMask) //emulate right mouse button if ctrl-key depressed
	{
		currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
		lastPoint = currentPoint;		
		[self rightMouseDown:event];
	}
	else
	{		
		draw_mousepoint_flag = 0;
		currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
		lastPoint = currentPoint;		
	}
}

- (void)mouseDragged:(NSEvent *)event
{		
	if ( [event modifierFlags] & NSControlKeyMask ) //don't rotate if ctrl-key depressed
	{		
		currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
		lastPoint = currentPoint;				
	}
		
	else if( [myAppController render_mode]==1 )      //or if not in interactive mode
	{
		currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
		draw_mousepoint_flag = 0;
		
		viewTheta += mouse_sensitivity * (currentPoint.y-lastPoint.y);	
		viewPhi   -= mouse_sensitivity * (currentPoint.x-lastPoint.x);
		
		if (viewPhi<0.0)
		{
			viewPhi += 360.0;
		}
		if (viewPhi>360.0) 
		{
			viewPhi -= 360.0;
		}
		if (viewTheta<0.0)
		{
			viewTheta = 0.0;
		}
		if (viewTheta>180.0) 
		{
			viewTheta = 180.0;
		}			
		lastPoint = currentPoint;
		[self setNeedsDisplay:YES];	
	}
}

- (void)mouseUp:(NSEvent *)event
{		
	if ([myAppController render_mode]==1) 
	{
		currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
		[self setNeedsDisplay:YES];	
	}
}

- (void)rightMouseDown:(NSEvent *)event
{
	int a,b,k,l,d,ray_flag;
	double Nray,lray,delta_ray,scalar_llp,fovy_current;
	double ray[3],lprime[3],wx,wy,raylength,J;
	double orthox0[3],dp;
	BOOL ortho_current;
		
	int render = [myAppController render_mode];	

	if (render == 1) 
	{
		[self setup_observer:fovy:orthoEnabledFlag];
		fovy_current = fovy;
		if (orthoEnabledFlag)
		{
			obs.frustum_height = (obs.eyedistance_ortho - radius);
			ortho_current = YES;
		}
		else 
		{
			obs.frustum_height = 2.0*obs.near*(float)tan((double)PI*fovy_current/360.0);
			ortho_current = NO;
		}		
	}
	else if (render == 2)
	{
		[self setup_observer:fovy_render:orthoEnabledFlag_render];
		fovy_current = fovy_render;
		if (orthoEnabledFlag_render)
		{
			obs.frustum_height = (obs.eyedistance_ortho-radius);
			ortho_current = YES;
		}
		else 
		{
			obs.frustum_height = 2.0*obs.near*(float)tan((double)PI*fovy_current/360.0);
			ortho_current = NO;
		}		
	}	
	obs.frustum_width = obs.aspect_ratio*obs.frustum_height;
	
	BOOL intersect_flag;
	
	if (render == 1 || render == 2) {	

		draw_mousepoint_flag = 1;
		currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
		lastPoint = currentPoint;
											
		//compute where the intersection with sphere occurs, in world coords.
		wx = 0.5* obs.frustum_width  * (2.0*currentPoint.x/obs.view_width  - 1.0);
		wy = 0.5* obs.frustum_height * (2.0*currentPoint.y/obs.view_height - 1.0);
		intersect_flag = NO;
				
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
			down_rightmouse_theta = (float)acos((double)ray[2]);
			down_rightmouse_phi = (float)atan2((double)ray[1],(double)ray[0]);
			
			//write angles in pixel info box
			NSString *Theta_pixelinfo_text;
			NSString *Phi_pixelinfo_text;
			Theta_pixelinfo_text = [[NSString alloc] initWithFormat:@"Th:% 6.4f",
				down_rightmouse_theta*180.0/M_PI];
			
			float phi_correct;
			phi_correct = down_rightmouse_phi*180.0/M_PI;
			if (phi_correct<0.0) phi_correct += 360.0;		
			Phi_pixelinfo_text = [[NSString alloc] initWithFormat:@"Ph:% 6.4f",
				phi_correct];
			
			[myAppController setPixinfoText_theta:Theta_pixelinfo_text];
			[myAppController setPixinfoText_phi:Phi_pixelinfo_text];
			[Theta_pixelinfo_text release];
			[Phi_pixelinfo_text release];
			
			//if map has been loaded..
			if ([myAppController maptype]!=0)
			{		
				//..find Healpix pixel corresponding to these angles
				//and write information to text fields in the pixel info box 					
				size_t pixnum;
				int ordering = [myAppController pixelordering];
				int nside = [myAppController map_nside];
				if (ordering==0) 
				{
					heal_ang2pix_ring(nside,down_rightmouse_theta,down_rightmouse_phi,&pixnum);					
				}
				else 
				{
					heal_ang2pix_nest(nside,down_rightmouse_theta,down_rightmouse_phi,&pixnum);					
				}		
				
				//Healpix n
				NSString *N_pixelinfo_text;
				N_pixelinfo_text = [[NSString alloc] initWithFormat:@"n: %-9d",pixnum];
				[myAppController setPixinfoText_N:N_pixelinfo_text];
				[N_pixelinfo_text release];
				
				//T
				float Tundermouse;		
				Tundermouse = hpic_float_get(hpic_Tmap,pixnum);		
				NSString *T_pixelinfo_text;
				T_pixelinfo_text = [[NSString alloc] initWithFormat:@"T:%+5.4e",Tundermouse];
				[myAppController setPixinfoText_T:T_pixelinfo_text];
				[T_pixelinfo_text release];
				
				if ([myAppController polarisation]==1) 
				{
					float Qundermouse, Uundermouse, Pundermouse;
					
					//Q
					Qundermouse = hpic_float_get(hpic_Qmap,pixnum);		
					NSString *Q_pixelinfo_text;
					Q_pixelinfo_text = [[NSString alloc] initWithFormat:@"Q:%+5.4e",Qundermouse];
					[myAppController setPixinfoText_Q:Q_pixelinfo_text];
					[Q_pixelinfo_text release];
					
					//U
					Uundermouse = hpic_float_get(hpic_Umap,pixnum);		
					NSString *U_pixelinfo_text;
					U_pixelinfo_text = [[NSString alloc] initWithFormat:@"U:%+5.4e",Uundermouse];
					[myAppController setPixinfoText_U:U_pixelinfo_text];
					[U_pixelinfo_text release];
					
					//P
					Pundermouse = sqrt(SQR(Uundermouse)+SQR(Qundermouse));		
					NSString *P_pixelinfo_text;
					P_pixelinfo_text = [[NSString alloc] initWithFormat:@"P:%+5.4e",Pundermouse];
					[myAppController setPixinfoText_P:P_pixelinfo_text];
					[P_pixelinfo_text release];
				}
				else if ([myAppController polarisation]==2)
				{
					float Qundermouse, Uundermouse, Pundermouse;
					//N
					Qundermouse = hpic_float_get(hpic_Qmap,pixnum);		
					NSString *Q_pixelinfo_text;
					Q_pixelinfo_text = [[NSString alloc] initWithFormat:@"N:%+5.4e",Qundermouse];
					[myAppController setPixinfoText_Q:Q_pixelinfo_text];
					[Q_pixelinfo_text release];
				}
			}			
		}
		[self setNeedsDisplay:YES];
	}
}

- (void)scrollWheel:(NSEvent *)event
{
	int render = [myAppController render_mode];
	if (render == 1) 
	{
		float dscrollwheel = [event deltaY];
		viewZoom += 0.02*dscrollwheel;
		if (viewZoom>5.0) viewZoom=5.0;
		if (viewZoom<0.0) viewZoom=0.0;
		
		[myAppController set_zoom_slider:viewZoom];
		[self setViewZoom:viewZoom];
		[self setNeedsDisplay:YES];	
	}
}

/**********************************************************************/
/*                        accessor methods                            */
/**********************************************************************/

- (float)viewTheta
{
	return viewTheta;
}
- (float)viewPhi
{
	return viewPhi;
}
- (void)setViewTheta:(float)theta
{
	viewTheta = theta;
}
- (void)setViewPhi:(float)phi
{
	viewPhi = phi;
}
- (void)setViewZoom:(float)zoom
{
	viewZoom = zoom;
	draw_mousepoint_flag = 0;
}
- (float)viewZoom
{
	return viewZoom;
}

- (void)setStokesLength:(float)Stokes_length
{
	StokesLength = Stokes_length;
}
- (float)StokesLength
{
	return StokesLength;
}
- (int)stokesConventionFlag
{
	return stokesConventionFlag;
}

- (void)setBackgroundColor:(float)red green:(float)green blue:(float)blue
{
	backgroundRGB[0] = red/255.0;
	backgroundRGB[1] = green/255.0;
	backgroundRGB[2] = blue/255.0;
	[self setNeedsDisplay:YES];
}

- (void)setAmbientLight:(float)red:(float)green:(float)blue
{
	ambientlightRGBA[0] = (GLfloat)red/255.0;
	ambientlightRGBA[1] = (GLfloat)green/255.0;
	ambientlightRGBA[2] = (GLfloat)blue/255.0;
	ambientlightRGBA[3] = (GLfloat)0.0;
	[self setNeedsDisplay:YES];
}
- (void)setAmbientMat:(float)red:(float)green:(float)blue
{
	ambientmatRGBA[0] = (GLfloat)red/255.0;
	ambientmatRGBA[1] = (GLfloat)green/255.0;
	ambientmatRGBA[2] = (GLfloat)blue/255.0;
	ambientmatRGBA[3] = (GLfloat)0.0;
	[self setNeedsDisplay:YES];
}

- (void)setDiffuseLight:(float)red:(float)green:(float)blue
{
	diffuselightRGBA[0] = (GLfloat)red/255.0;
	diffuselightRGBA[1] = (GLfloat)green/255.0;
	diffuselightRGBA[2] = (GLfloat)blue/255.0;
	diffuselightRGBA[3] = (GLfloat)0.0;
	[self setNeedsDisplay:YES];
}
- (void)setDiffuseMat:(float)red:(float)green:(float)blue
{
	diffusematRGBA[0] = (GLfloat)red/255.0;
	diffusematRGBA[1] = (GLfloat)green/255.0;
	diffusematRGBA[2] = (GLfloat)blue/255.0;
	diffusematRGBA[3] = (GLfloat)0.0;
	[self setNeedsDisplay:YES];
}

- (void)setSpecularLight:(float)red:(float)green:(float)blue
{
	specularlightRGBA[0] = (GLfloat)red/255.0;
	specularlightRGBA[1] = (GLfloat)green/255.0;
	specularlightRGBA[2] = (GLfloat)blue/255.0;
	specularlightRGBA[3] = (GLfloat)0.0;
	[self setNeedsDisplay:YES];
}
- (void)setSpecularMat:(float)red:(float)green:(float)blue
{
	specularmatRGBA[0] = (GLfloat)red/255.0;
	specularmatRGBA[1] = (GLfloat)green/255.0;
	specularmatRGBA[2] = (GLfloat)blue/255.0;
	specularmatRGBA[3] = (GLfloat)0.0;
	[self setNeedsDisplay:YES];
}
		
/**********************************************************************/
/*                     notification handlers                          */
/**********************************************************************/

- (void)handle_backgrndcolorChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender backgrndcolor];
	
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red, green, blue;
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;
	[self setBackgroundColor:(float)red green:(float)green blue:(float)blue];
	[colorRGB release];		
}

- (void)handle_ambientlightChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender ambientlightcolor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red, green, blue;
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;
	[self setAmbientLight:(float)red:(float)green:(float)blue];
	[colorRGB release];		
}
- (void)handle_ambientmatChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender ambientmatcolor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red, green, blue;
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;
	[self setAmbientMat:(float)red:(float)green:(float)blue];
	[colorRGB release];		
}

- (void)handle_diffuselightChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender diffuselightcolor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red, green, blue;
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;
	[self setDiffuseLight:(float)red:(float)green:(float)blue];
	[colorRGB release];		
}
- (void)handle_diffusematChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender diffusematcolor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red, green, blue;
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;
	[self setDiffuseMat:(float)red:(float)green:(float)blue];
	[colorRGB release];		
}

- (void)handle_specularlightChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender specularlightcolor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red, green, blue;
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;
	[self setSpecularLight:(float)red:(float)green:(float)blue];
	[colorRGB release];		
}
- (void)handle_specularmatChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender specularmatcolor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	int red, green, blue;
	red = [colorRGB redComponent]*255;
	green = [colorRGB greenComponent]*255;
	blue = [colorRGB blueComponent]*255;
	[self setSpecularMat:(float)red:(float)green:(float)blue];
	[colorRGB release];		
}


- (void)handle_rotatelightthetaChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	lightTheta = [sender lightTheta];
	[self setNeedsDisplay:YES];
}
- (void)handle_rotatelightphiChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	lightPhi = [sender lightPhi];
	[self setNeedsDisplay:YES];
}
- (void)handle_lightradiusChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	lightRadius = [sender lightRadius];
	[self setNeedsDisplay:YES];
}

- (void)handle_shininessChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	shininess = [sender shininess];
	[self setNeedsDisplay:YES];
}

- (void)handle_lightingenableChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	lightingEnabledFlag = [sender lightingEnable];
	[self setNeedsDisplay:YES];	
}
- (void)handle_ambientenableChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	ambientEnabledFlag = [sender ambientEnable];
	[self setNeedsDisplay:YES];	
}
- (void)handle_diffuseenableChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	diffuseEnabledFlag = [sender diffuseEnable];
	[self setNeedsDisplay:YES];	
}
- (void)handle_specularenableChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	specularEnabledFlag = [sender specularEnable];
	[self setNeedsDisplay:YES];	
}

- (void)handle_gridSpacingChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	GRID_NUM = 4*[sender GridSpacing];
	[self setNeedsDisplay:YES];		
}

- (void)handle_stokesPropsChanged:(NSNotification *)note
{
	enum stokesproperties sp; 
	PreferenceController *sender = [note object];
	NSMutableArray *myMutableArray = [sender stokesProps];

	sp = stokes_opacity;
	NSNumber *stokesopacityValue = [myMutableArray objectAtIndex:sp];
	stokesRGBA[3] = [stokesopacityValue floatValue];
	
	sp = stokes_length;
	NSNumber *stokeslengthValue = [myMutableArray objectAtIndex:sp];
	max_stokes_arcmin = [stokeslengthValue floatValue];
	
	sp = stokes_thickness;
	NSNumber *stokesthicknessValue = [myMutableArray objectAtIndex:sp];
	stokes_line_thickness = [stokesthicknessValue floatValue];
	
	sp = stokes_color;
	NSColor *color = [NSKeyedUnarchiver unarchiveObjectWithData:[myMutableArray objectAtIndex:sp]];
	NSColor *colorRGB = [[color colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	stokesRGBA[0] = [colorRGB redComponent];
	stokesRGBA[1] = [colorRGB greenComponent];
	stokesRGBA[2] = [colorRGB blueComponent];
	[colorRGB release];		
		
	sp = stokes_convention;
	NSNumber *stokesconventionValue = [myMutableArray objectAtIndex:sp];
	stokesConventionFlag = [stokesconventionValue intValue];
	
	[self setNeedsDisplay:YES];		
}

- (void)handle_gridPropsChanged:(NSNotification *)note
{	
	enum gridproperties sp; 
	PreferenceController *sender = [note object];
	NSMutableArray *myMutableArray = [sender gridProps];
	
	sp = grid_enable;
	NSNumber *gridenableValue = [myMutableArray objectAtIndex:sp];
	axis_flag = [gridenableValue boolValue];
	
	sp = grid_opacity;
	NSNumber *gridopacityValue = [myMutableArray objectAtIndex:sp];
	gridRGBA[3] = [gridopacityValue floatValue];
	
	sp = grid_thickness;
	NSNumber *gridthicknessValue = [myMutableArray objectAtIndex:sp];
	grid_line_thickness = [gridthicknessValue floatValue];

	sp = grid_color;
	NSColor *color = [NSKeyedUnarchiver unarchiveObjectWithData:[myMutableArray objectAtIndex:sp]];
	NSColor *colorRGB = [[color colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];	
	gridRGBA[0] = [colorRGB redComponent];
	gridRGBA[1] = [colorRGB greenComponent];
	gridRGBA[2] = [colorRGB blueComponent];
	[colorRGB release];		
	
	sp = gridincaps;
	NSNumber *gridincapsValue = [myMutableArray objectAtIndex:sp];
	gridincapsFlag = [gridincapsValue boolValue];
			
	[self setNeedsDisplay:YES];		
}

- (void)handle_texelinterpolationChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	texelInterpolationFlag = [sender TexelInterpolation];
	[self setNeedsDisplay:YES];	
}

- (void)handle_fovyChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	fovy = [sender fovy];
	[self setNeedsDisplay:YES];
}

- (void)handle_orthoChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	orthoEnabledFlag = [sender orthoEnable];
	[self setNeedsDisplay:YES];	
}
- (BOOL)orthoEnabledFlag
{
	return orthoEnabledFlag;
}
- (BOOL)orthoEnabledFlag_render
{
	return orthoEnabledFlag_render;
}

- (void)handle_mousesensitivityChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	mouse_sensitivity = [sender MouseSensitivity];
	[self setNeedsDisplay:YES];
}

- (void)handle_presentationbackgrndcolorChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender presentationbackgrndColor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];
	present_bgrd_RGBA[0] = [colorRGB redComponent];
	present_bgrd_RGBA[1] = [colorRGB greenComponent];
	present_bgrd_RGBA[2] = [colorRGB blueComponent];
	[colorRGB release];		
	[self setNeedsDisplay:YES];	
}

- (void)handle_presentationtextcolorChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	NSColor *newColor = [sender presentationtextColor];
	NSColor *colorRGB;
	colorRGB = [[newColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace] retain];
	present_text_RGBA[0] = [colorRGB redComponent];
	present_text_RGBA[1] = [colorRGB greenComponent];
	present_text_RGBA[2] = [colorRGB blueComponent];
	[colorRGB release];		
	[self setNeedsDisplay:YES];		
}

- (void)dealloc
{
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc removeObserver:self];
	[super dealloc];
}


@end
