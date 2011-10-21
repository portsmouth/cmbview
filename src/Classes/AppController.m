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
* This code handles all the GUI stuff and also FITS file IO using            *
* Theodore Kisner's hpic library.                                            *
*                                                                            *
*****************************************************************************/

#import "AppController.h"


/**********************************************************************/
/*                      hpic error handling                           */
/**********************************************************************/

//this is a pointer to my (IB-instantiated) instance of AppController (set in init)
//which is used to call an Obj-C instance method from the following hpic error handler C function 
AppController *anAppController;

void my_hpic_error_handler (int errcode, const char *file, 
							int line, const char *msg)
{	
	//It's not currently really necessary to make a method call here,
	//but in future it might be desirable to do things with the GUI
	//right in the error handler, which is only possible from inside a method.
	[anAppController GUI_error_handler:errcode];			
	return;
}


@implementation AppController

- (void)GUI_error_handler:(int)errcode
{
	switch (errcode)
	{
		case 0:
			return;
			break;
		case HPIC_ERR_ALLOC:
			strcpy(hpic_errorstr, "Memory allocation error");
			break;
		case HPIC_ERR_FREE:
			strcpy(hpic_errorstr, "Memory freeing error");
			break;
		case HPIC_ERR_NSIDE:
			strcpy(hpic_errorstr, "Illegal NSIDE value");
			break;
		case HPIC_ERR_ORDER:
			strcpy(hpic_errorstr, "Illegal ORDER value");
			break;
		case HPIC_ERR_COORD:
			strcpy(hpic_errorstr, "Illegal COORDINATE value");
			break;
		case HPIC_ERR_RANGE:
			strcpy(hpic_errorstr, "Value is out of range");
			break;
		case HPIC_ERR_ACCESS:
			strcpy(hpic_errorstr, "Memory is not accessible");
			break;
		case HPIC_ERR_PROJ:
			strcpy(hpic_errorstr, "Projection error");
			break;
		case HPIC_ERR_FITS:
			strcpy(hpic_errorstr, "FITS error");
			break;
		default:
			strcpy(hpic_errorstr, "Unknown error code");
			break;
	}	
	HPIC_ERROR_FLAG = TRUE;
	return;
}

/**********************************************************************/
/*        initialize and define "factory default" preferences         */
/**********************************************************************/

- (id)init
{		
	if ((self = [super init])==nil) 
	{
		NSLog(@"ERROR: could not initialize AppController");
		exit(EXIT_FAILURE);
	}
	
	//no map yet
	[self setMaptype:0];
	
	//set up preset colormaps
	define_colormaps();
	
	//set colormap to color (rather than grey) initially
	[self SetColormap_flag:1];
	
	//make sure color buttons are correspondingly in correct state
	[color_matrix_HSV setState:NSOnState];
	[color_matrix_grey setState:NSOffState];
	
	//disable editing of histogram range text fields initially
	[widget_max setEditable:NO];
	[widget_min setEditable:NO];
	
	//disable mode selection until a map is loaded
	id theCell_interactive = [render_mode_matrix cellWithTag:1];
	id theCell_render = [render_mode_matrix cellWithTag:2];
	id theCell_presentation = [render_mode_matrix cellWithTag:3];				
	[theCell_interactive setEnabled:NO];
	[theCell_render setEnabled:NO];
	[theCell_presentation setEnabled:NO];
	[render_mode_matrix selectCellWithTag:1];

	linlog_flag = 0;
	render_mode = 1;
	
	hpic_Tmap = NULL;
	hpic_Qmap = NULL;
	hpic_Umap = NULL;
	hpic_Nmap = NULL;
	hpic_maps = NULL;
	
	HPIC_ERROR_FLAG = FALSE;
	hpic_set_error_handler(&my_hpic_error_handler);
	anAppController = self;
	
	return self;
}

+ (void)initialize {
	
	//Create a dictionary for user preferences
	NSMutableDictionary *defaultValues = [NSMutableDictionary dictionary];
	
	NSData *colorAsData;
				
	//general 
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:[NSColor blackColor]];
	[defaultValues setObject:colorAsData forKey:CMBview_backgrndcolorkey];
	
	BOOL orthoenabledFlag = YES;
	[defaultValues setObject:[NSNumber numberWithBool:orthoenabledFlag]
					  forKey:CMBview_orthokey];
	
	float fovy_init = 45.0;
	[defaultValues setObject:[NSNumber numberWithFloat:fovy_init]
					  forKey:CMBview_fovykey];	
	
	int exportimagesize_init = 1024;
	[defaultValues setObject:[NSNumber numberWithInt:exportimagesize_init]
					  forKey:CMBview_exportimagesizekey];	
	
	float mousesensitivity_init = 0.2;
	[defaultValues setObject:[NSNumber numberWithFloat:mousesensitivity_init]
					  forKey:CMBview_mousesensitivitykey];
	
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:[NSColor whiteColor]];
	[defaultValues setObject:colorAsData forKey:CMBview_presentationbackgrndcolorkey];
	
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:[NSColor blackColor]];
	[defaultValues setObject:colorAsData forKey:CMBview_presentationtextcolorkey];
	
	//texture 
	int texnum_init = 1;
	[defaultValues setObject:[NSNumber numberWithInt:texnum_init]
					  forKey:CMBview_texnumkey];
	
	int texelinterpolation_init = 0;	
	[defaultValues setObject:[NSNumber numberWithInt:texelinterpolation_init]
					  forKey:CMBview_texinterpolatekey];
	
	//colormaps 
	current_colormap_ptr = &mycolormaps[hsv];
	
	BOOL reversecolormapFlag = NO;
	[defaultValues setObject:[NSNumber numberWithBool:reversecolormapFlag]
					  forKey:CMBview_colormapreversekey];
	
	//lighting 	
	float R,G,B,A;

	R = 1.0; G = 1.0; B = 1.0; A = 1.0;
	NSColor *ambientlight = [NSColor colorWithCalibratedRed:R
													 green:G
													  blue:B
													 alpha:A];	
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:ambientlight];
	[defaultValues setObject:colorAsData 
					  forKey:CMBview_ambientlightkey];

	R = 0.2; G = 0.2; B = 0.2; A = 1.0;
	NSColor *ambientmat = [NSColor colorWithCalibratedRed:R
													 green:G
													  blue:B
													 alpha:A];
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:ambientmat];
	[defaultValues setObject:colorAsData 
					  forKey:CMBview_ambientmatkey];

	R = 1.0; G = 1.0; B = 1.0; A = 1.0;
	NSColor *diffuselight = [NSColor colorWithCalibratedRed:R
													  green:G
													   blue:B
													  alpha:A];	
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:diffuselight];
	[defaultValues setObject:colorAsData 
					  forKey:CMBview_diffuselightkey];
	
	R = 0.5; G = 0.5; B = 0.5; A = 1.0;
	NSColor *diffusemat = [NSColor colorWithCalibratedRed:R
													green:G
													 blue:B
													alpha:A];
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:diffusemat];
	[defaultValues setObject:colorAsData 
					  forKey:CMBview_diffusematkey];

	R = 1.0; G = 1.0; B = 1.0; A = 1.0;
	NSColor *specularlight = [NSColor colorWithCalibratedRed:R
													  green:G
													   blue:B
													  alpha:A];	
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:specularlight];
	[defaultValues setObject:colorAsData 
					  forKey:CMBview_specularlightkey];
	
	R = 0.1; G = 0.1; B = 0.1; A = 1.0;
	NSColor *specularmat = [NSColor colorWithCalibratedRed:R
													green:G
													 blue:B
													alpha:A];
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:specularmat];
	[defaultValues setObject:colorAsData 
					  forKey:CMBview_specularmatkey];
	
	float lighttheta_init = 0.0;					
	float lightphi_init = 0.0;	
	[defaultValues setObject:[NSNumber numberWithFloat:lighttheta_init]
					  forKey:CMBview_lightthetakey];
	[defaultValues setObject:[NSNumber numberWithFloat:lightphi_init]
					  forKey:CMBview_lightphikey];
	float lightradius_init = 2.0;	
	[defaultValues setObject:[NSNumber numberWithFloat:lightradius_init]
					  forKey:CMBview_lightradiuskey];
	float shininess_init = 50.0;		
	[defaultValues setObject:[NSNumber numberWithFloat:shininess_init]
					  forKey:CMBview_shininesskey];
	
	BOOL lightingenabledFlag = YES;
	BOOL ambientenabledFlag = YES;
	BOOL diffuseenabledFlag = YES;
	BOOL specularenabledFlag = YES;
	
	[defaultValues setObject:[NSNumber numberWithBool:lightingenabledFlag]
					  forKey:CMBview_lightingenabledkey];
	[defaultValues setObject:[NSNumber numberWithBool:ambientenabledFlag]
					  forKey:CMBview_ambientenabledkey];
	[defaultValues setObject:[NSNumber numberWithBool:diffuseenabledFlag]
					  forKey:CMBview_diffuseenabledkey];
	[defaultValues setObject:[NSNumber numberWithBool:specularenabledFlag]
					  forKey:CMBview_specularenabledkey];
	
	//stokes
	int stokesgrid_init = 100;
	[defaultValues setObject:[NSNumber numberWithInt:stokesgrid_init]
					  forKey:CMBview_stokesgridkey];

	float stokesopacity_init = 1.0;
	R = 1.0; G = 1.0; B = 1.0; A = 1.0;
	NSColor *stokescolor = [NSColor colorWithCalibratedRed:R
													 green:G
													  blue:B
													 alpha:A];	
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:stokescolor];
	float stokesthickness_init = 1.0;
	float stokeslength_init = 60.0;	
	int stokesconvention_init = 0;
	
	NSMutableArray *myMutableArray_stokes = [[NSMutableArray alloc] init];		
	[myMutableArray_stokes addObject:colorAsData];
	[myMutableArray_stokes addObject:[NSNumber numberWithFloat:stokesopacity_init]];
	[myMutableArray_stokes addObject:[NSNumber numberWithFloat:stokesthickness_init]];
	[myMutableArray_stokes addObject:[NSNumber numberWithFloat:stokeslength_init]];
	[myMutableArray_stokes addObject:[NSNumber numberWithInt:stokesconvention_init]];

	NSData *MAasData_stokes = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray_stokes];		
	[defaultValues setObject:MAasData_stokes
				 forKey:CMBview_stokespropskey];	
	
	//grid
	int gridnum_init = 1;
	[defaultValues setObject:[NSNumber numberWithInt:gridnum_init]
					  forKey:CMBview_gridspacekey];

	R = 1.0; G = 1.0; B = 1.0; A = 1.0;
	NSColor *gridcolor = [NSColor colorWithCalibratedRed:R
												   green:G
													blue:B
												   alpha:A];	
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:gridcolor];
	BOOL gridenable_init = YES;
	float gridopacity_init = 0.7;
	float gridthickness_init = 1.0;
	BOOL gridincaps_init = YES;
	NSMutableArray *myMutableArray_grid = [[NSMutableArray alloc] init];
	
	[myMutableArray_grid addObject:colorAsData];
	[myMutableArray_grid addObject:[NSNumber numberWithBool:gridenable_init]];
	[myMutableArray_grid addObject:[NSNumber numberWithFloat:gridopacity_init]];
	[myMutableArray_grid addObject:[NSNumber numberWithFloat:gridthickness_init]];
	[myMutableArray_grid addObject:[NSNumber numberWithBool:gridincaps_init]];
	
	NSData *MAasData_grid = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray_grid];		
	[defaultValues setObject:MAasData_grid
					  forKey:CMBview_gridpropskey];	

	
	//register defaults
	[[NSUserDefaults standardUserDefaults] registerDefaults: defaultValues];

}


/**********************************************************************/
/*                          GUI operations                            */
/**********************************************************************/

- (IBAction)showPreferencePanel:(id)sender
{
	if (!preferenceController) 
	{
		preferenceController = [[PreferenceController alloc] init];
	}
	[preferenceController showWindow:self];
}

- (IBAction)showAboutPanel:(id)sender
{
	if (!cmbviewAboutPanel) 
	{
		cmbviewAboutPanel = [[CMBviewAboutPanel alloc] init];
	}
	[cmbviewAboutPanel showWindow:self];
}

- (IBAction)choose_render_mode:(id)sender
{
	if ([self maptype]!=0) 
	{
	
		id theCell = [sender selectedCell];
		int theTag = [theCell tag];
		
		id theCell_interactive = [sender cellWithTag:1];
		id theCell_render = [sender cellWithTag:2];
		id theCell_presentation = [sender cellWithTag:3];
		
		//render mode
		if (theTag==2) 
		{
			render_mode = 2;
			
			[myCMBdata genTextures_render];
			[myOpenGLview display];
			[myLittleOpenGLview setNeedsDisplay:YES];
			
			[self disable_zoom_slider];
			
			[theCell_render setEnabled:NO];
			[theCell_interactive setEnabled:YES];
			[theCell_presentation setEnabled:YES];
			
			if (polarisation==1) [genStokesbutton setEnabled:(BOOL)YES];
		}
		
		//presentation mode
		if (theTag==3) 
		{
			render_mode = 3;			
			
			//TO DO: if the last mode was render mode, then we needn't
			// regenerate texture since viewpoint hasn't changed.
			// (and similarly if we change from presentation mode to render mode).
			
			[myCMBdata genTextures_render];
			[myOpenGLview display];
			[myOpenGLview display];
			[myLittleOpenGLview setNeedsDisplay:YES];
			
			[self disable_zoom_slider];
			
			[theCell_render setEnabled:YES];
			[theCell_interactive setEnabled:YES];
			[theCell_presentation setEnabled:NO];
			
			[genStokesbutton setEnabled:NO];
		}
		
		//interactive mode
		else if (theTag==1) 
		{
			render_mode = 1;
			
			[myCMBdata dealloc_renderdata];
			[myLittleOpenGLview changeHistogram];
			[myLittleOpenGLview setNeedsDisplay:YES];
			[myOpenGLview setNeedsDisplay:YES];
			
			[self enable_zoom_slider];
		
			[theCell_render setEnabled:YES];
			[theCell_interactive setEnabled:NO];
			[theCell_presentation setEnabled:YES];
			
			if (polarisation==1) [genStokesbutton setEnabled:(BOOL)YES];
		}
	}
}

- (IBAction)updateColors:(id)sender
{
	[myLittleOpenGLview changeColorRange];
	[myLittleOpenGLview changeHistogram];

	if (render_mode==1)
	{
		[myCMBdata updateTexs_interactive];
	}
	else
	{
		[myCMBdata updateTexs_render];
	}
	[myOpenGLview setNeedsDisplay:YES];
	[myLittleOpenGLview setNeedsDisplay:YES];
}

- (IBAction)linlog_matrix:(id)sender
{
	id theCell = [sender selectedCell];
	int theTag = [theCell tag];
	
	//lin scale
	if (theTag==0) 
	{
		if (linlog_flag!=0) 
		{
			linlog_flag = 0;
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
	}
	
	//log scale
  	if (theTag==1) {
		if (linlog_flag!=1) 
		{
			linlog_flag = 1;
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
	}
}

- (IBAction)color_matrix:(id)sender
{
	id theCell = [sender selectedCell];
	int theTag = [theCell tag];
	int update_flag = 0;		
	
	//HSV
	if (theTag==0 && colormap_flag != 1)
	{
		colormap_flag = 1;
		update_flag = 1;
	}
	
	//grey
	if (theTag==1 && colormap_flag != 0) 
	{
		colormap_flag = 0;
		update_flag = 1;
	}		
	
	if (update_flag && ([self maptype]!=0))
	{
		[myLittleOpenGLview changeColorRange];
		if (render_mode==1) 
		{
			[myCMBdata updateTexs_interactive];
			[myOpenGLview setNeedsDisplay:YES];
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
		else
		{
			[myCMBdata updateTexs_interactive];
			[myCMBdata genTextures_render];
			[myOpenGLview display];
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
	}
}

- (IBAction)Tmap_select:(id)sender
{
	int currentmap;
	currentmap = [self maptype];
	[self setMaptype:1];
	
	if (currentmap!=1)
	{
		[Tmapbutton setState:NSOnState];
		[Qmapbutton setState:NSOffState];
		[Umapbutton setState:NSOffState];
		[Pmapbutton setState:NSOffState];
		
		[myLittleOpenGLview changeHistogram];
		[myLittleOpenGLview setNeedsDisplay:YES];
		
		if (render_mode==1)
		{
			[myCMBdata updateTexs_interactive];
			[myOpenGLview setNeedsDisplay:YES];
		}
		else 
		{
			[myCMBdata updateTexs_interactive];
			[myCMBdata genTextures_render];
			[myOpenGLview display];
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
	}
}
- (IBAction)Qmap_select:(id)sender
{
	int currentmap;
	currentmap = [self maptype];
	[self setMaptype:2];
	
	if (currentmap!=2) 
	{
		[Tmapbutton setState:NSOffState];
		[Qmapbutton setState:NSOnState];
		[Umapbutton setState:NSOffState];
		[Pmapbutton setState:NSOffState];
		
		[myLittleOpenGLview changeHistogram];
		[myLittleOpenGLview setNeedsDisplay:YES];
		
		if (render_mode==1) 
		{
			[myCMBdata updateTexs_interactive];
			[myOpenGLview setNeedsDisplay:YES];
		}
		else
		{
			[myCMBdata updateTexs_interactive];
			[myCMBdata genTextures_render];
			[myOpenGLview display];
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
		[myOpenGLview setNeedsDisplay:YES];
	}

}
- (IBAction)Umap_select:(id)sender
{
	int currentmap;
	currentmap = [self maptype];
	[self setMaptype:3];
	
	if (currentmap!=3) 
	{
		[Tmapbutton setState:NSOffState];
		[Qmapbutton setState:NSOffState];
		[Umapbutton setState:NSOnState];
		[Pmapbutton setState:NSOffState];
		
		[myLittleOpenGLview changeHistogram];
		[myLittleOpenGLview setNeedsDisplay:YES];		
		
		if (render_mode==1)
		{
			[myCMBdata updateTexs_interactive];
			[myOpenGLview setNeedsDisplay:YES];
		}
		else
		{
			[myCMBdata updateTexs_interactive];
			[myCMBdata genTextures_render];
			[myOpenGLview display];
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
		[myOpenGLview setNeedsDisplay:YES];
	}
}
- (IBAction)Pmap_select:(id)sender
{
	int currentmap;
	currentmap = [self maptype];
	[self setMaptype:4];
	
	if (currentmap!=4)
	{
		[Tmapbutton setState:NSOffState];
		[Qmapbutton setState:NSOffState];
		[Umapbutton setState:NSOffState];
		[Pmapbutton setState:NSOnState];
		
		[myLittleOpenGLview changeHistogram];
		[myLittleOpenGLview setNeedsDisplay:YES];
		
		if (render_mode==1)
		{
			[myCMBdata updateTexs_interactive];
			[myOpenGLview setNeedsDisplay:YES];
		}
		else 
		{
			[myCMBdata updateTexs_interactive];
			[myCMBdata genTextures_render];
			[myOpenGLview display];
			[myLittleOpenGLview setNeedsDisplay:YES];
		}
		[myOpenGLview setNeedsDisplay:YES];
	}
}

- (IBAction)showStokes:(id)sender;
{
	NSLog(@"show or hide current Stokes vectors");
	int currentStokesFlag;
	currentStokesFlag = [self Stokes_flag];
	
	if (currentStokesFlag==0)
	{
		[showStokesbutton setState:NSOnState];
		[self setStokesflag:1];
	}
	
	if (currentStokesFlag==1)
	{
		[showStokesbutton setState:NSOffState];
		[self setStokesflag:0];
	}
	[myOpenGLview setNeedsDisplay:YES];
}

- (IBAction)genStokes:(id)sender;
{
	[myCMBdata genStokes];
	[self setStokesflag:1];
	[showStokesbutton setEnabled:(BOOL)YES];
	[myOpenGLview setNeedsDisplay:YES];
}

- (IBAction)Stokeslength:(id)sender
{
	float Stokes_length;
	Stokes_length = [Stokesslider floatValue];
	[myOpenGLview setStokesLength:Stokes_length];
	[myOpenGLview setNeedsDisplay:YES];
}

// text editing of the histogram range
- (IBAction)setHistMax:(id)sender
{
	NSString* maxString = [sender stringValue];

	if (![self maptype]) return;
	
	// attempt to move the max widget
	BOOL success = [myLittleOpenGLview setMaxLimit:[maxString floatValue]];
	
	// if widget could not be moved there, restore the previous text in the max field
	if (!success)
	{
		float max, min;
		[myLittleOpenGLview getCurrentRange: &max: &min];
		
		NSString *currentMaxString;
		currentMaxString = [[NSString alloc] initWithFormat:@"%8e", max];
		[self setwidgetmax:currentMaxString];
		[currentMaxString release];
	}
}

- (IBAction)setHistMin:(id)sender
{
	NSString* minString = [sender stringValue];
	
	if (![self maptype]) return;
			
	// attempt to move the min widget
	BOOL success = [myLittleOpenGLview setMinLimit:[minString floatValue]];
	
	// if widget could not be moved there, restore the previous text in the min field
	if (!success)
	{
		float max, min;
		[myLittleOpenGLview getCurrentRange: &max: &min];
		
		NSString *currentMinString;
		currentMinString = [[NSString alloc] initWithFormat:@"%8e", min];
		[self setwidgetmin:currentMinString];
		[currentMinString release];
	}	
}

- (NSSize)windowWillResize:(NSWindow *)sender
					toSize:(NSSize)frameSize
{
	struct _NSSize mySize;
	struct _NSRect myFrame;
	struct _NSSize myFramesize; 
	
	myFrame = [myWindow frame];
	myFramesize = myFrame.size;	
	
	//make sure window width and height change in the right proportion,
	//so sphere view is always perfectly square
	
	float extrawidth = frameSize.width - myFramesize.width;
	mySize.width = frameSize.width;
	mySize.height = myFramesize.height + extrawidth;
	return mySize;
}

- (BOOL)windowShouldZoom:(NSWindow *)sender toFrame:(NSRect)newFrame
{
	//prevent the zoom button from doing anything nasty to my window
	return NO;
}

//all these methods write the text in the pixel info window on the lower RHS
//(displayed on right mouse click)

- (void)setPixelnumText:(NSString *)text
{
	text = [text copy];
	[pixnumString release];
	pixnumString = text;
	[Numofpixels setStringValue:pixnumString];
	[Numofpixels display];
}

- (void)setPixinfoText_T:(NSString *)text
{
	text = [text copy];	
	[Tvalhere_string release];
	Tvalhere_string = text;
	[Tval_here setStringValue:Tvalhere_string];
	[Tval_here display];
}

- (void)setPixinfoText_Q:(NSString *)text
{
	text = [text copy];	
	[Qvalhere_string release];
	Qvalhere_string = text;
	[Qval_here setStringValue:Qvalhere_string];
	[Qval_here display];
}

- (void)setPixinfoText_U:(NSString *)text
{
	text = [text copy];	
	[Uvalhere_string release];
	Uvalhere_string = text;
	[Uval_here setStringValue:Uvalhere_string];
	[Uval_here display];
}

- (void)setPixinfoText_P:(NSString *)text
{
	text = [text copy];	
	[Pvalhere_string release];
	Pvalhere_string = text;
	[Pval_here setStringValue:Pvalhere_string];
	[Pval_here display];
}

- (void)setPixinfoText_theta:(NSString *)text
{
	text = [text copy];	
	[Thetavalhere_string release];
	Thetavalhere_string = text;
	[Thetaval_here setStringValue:Thetavalhere_string];
	[Thetaval_here display];
}

- (void)setPixinfoText_phi:(NSString *)text
{
	text = [text copy];	
	[Phivalhere_string release];
	Phivalhere_string = text;
	[Phival_here setStringValue:Phivalhere_string];
	[Phival_here display];
}

- (void)setPixinfoText_N:(NSString *)text
{
	text = [text copy];	
	[Nvalhere_string release];
	Nvalhere_string = text;
	[Nval_here setStringValue:Nvalhere_string];
	[Nval_here display];
}

- (void)setwidgetmax:(NSString *)text
{
	text = [text copy];
	[widgetmaxstring release];
	widgetmaxstring = text;
	[widget_max setStringValue:widgetmaxstring];
	[widget_max display];
}

- (void)setwidgetmin:(NSString *)text
{
	text = [text copy];
	[widgetminstring release];
	widgetminstring = text;
	[widget_min setStringValue:widgetminstring];
	[widget_min display];
}

- (NSString *)widgetminstring
{
	return widgetminstring;
}

- (NSString *)widgetmaxstring
{
	return widgetmaxstring;
}

//deal with progress indicator at bottom of window
- (void)setProgressText:(NSString *)text 
{
	text = [text copy];
	[progressStatus release];
	progressStatus = text;
	[progress_status_text setStringValue:progressStatus];
	[progress_status_text display];
}

- (void)setProgressIndicator:(double)x
{
	[progressView setDoubleValue:x];
	[progressView display];
}

- (void)stopProgressBar:(BOOL)stop
{
	if (stop==YES) 
	{
		[progressView setHidden:YES];
	}
	else if (stop==NO)
	{
		[progressView setHidden:NO];
		[progressView animate:self];
		[progressView display];
	}
}

//deal with zoom slider
- (void)set_zoom_slider:(float)zoomval
{
	[Zoom_slider setFloatValue:zoomval];
}

- (IBAction)zoom_level:(id)sender
{
	zoomValue = [Zoom_slider floatValue];
	[myOpenGLview setViewZoom:zoomValue];
	[myOpenGLview setNeedsDisplay:YES];
}

- (void)disable_zoom_slider
{
	[Zoom_slider setEnabled:NO];
}

- (void)enable_zoom_slider
{
	[Zoom_slider setEnabled:YES];
}


/**********************************************************************/
/*                            export images                           */
/**********************************************************************/

/* generate a TIFF image of arbitrary dimensions set in prefs, 
   containing unlit render-mode texture */
- (IBAction)renderAndSave:(id)sender
{			
	if ([self maptype]) 
	{
		
		NSUserDefaults *defaults;		
		defaults = [NSUserDefaults standardUserDefaults];
		int eisize = [defaults integerForKey:CMBview_exportimagesizekey];
		
		NSBeep();	
		NSString *msg;
		msg = [[NSString alloc] initWithFormat:
			@"This will generate a texture of dimensions %dx%d, which may take some time. Continue?", eisize, eisize];
		int choice = NSRunAlertPanel(@"Warning",msg,@"Generate",@"Cancel",nil);
		
		if (choice == NSAlertDefaultReturn)
		{
			
			[myCMBdata genTextures_render_forexport];
	
			NSSavePanel *sp = [NSSavePanel savePanel];
			[sp setRequiredFileType:@"tiff"];	
			
			int runResult = [sp runModalForDirectory:NSHomeDirectory() file:@""];		
			if (runResult == NSOKButton)
			{
				[myTIFF writeToFile:[sp filename] atomically:NO];
			}	
					
			//delete image buffer from memory
			[myTIFF release];
			myTIFF = nil;			
		}
	}
}	

/* generate a TIFF image containing current framebuffer (currently disabled) */
- (IBAction)saveImage:(id)sender
{	
	[myCMBdata framebuffer_forexport];
	
	NSSavePanel *sp = [NSSavePanel savePanel];
	[sp setRequiredFileType:@"tiff"];
	
	int runResult = [sp runModalForDirectory:NSHomeDirectory() file:@""];	
	if (runResult == NSOKButton)
	{
		[myTIFF writeToFile:[sp filename] atomically:NO];
	}		
	
	//delete image buffer from memory
	[myTIFF release];
	myTIFF = nil;	
}

- (void)setTIFF:(NSData *)someData
{
	[someData retain];
	[myTIFF release];
	myTIFF = someData;
}


/**********************************************************************/
/*                         FITS IO via hpic                           */
/**********************************************************************/

/* file panel for "open FITS file" menu item */
- (IBAction)readFile:(id)sender
{	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel beginSheetForDirectory:nil
							 file:nil
							types:nil
				   modalForWindow:[NSApp mainWindow]
					modalDelegate:self
				   didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:)
					  contextInfo:NULL];
	return;
}

- (void) openPanelDidEnd:(NSOpenPanel*)panel 
			  returnCode:(int)rc 
			 contextInfo:(void *)ctx
{
		if (rc == NSOKButton)
		{	
			myFITSfile = [[NSString alloc] initWithFormat:@"%@",[panel filename]];
			
			//this timer forces open panel to close immediately
			openfiletimer = [[NSTimer scheduledTimerWithTimeInterval: 0.2
															  target: self
															selector:@selector(readFromFile)
															userInfo:nil
															 repeats:NO] retain];
		}
}

- (void)readFromFile
{
	int FITSflag,pol,order,coord,type;
	char *inFITSfilename, creator[200], extname[200];
	NSString *pixelcount;
	hpic_keys *keys;
	size_t mapnum, npix, pixnum, nside, nmaps;
	
	[openfiletimer invalidate];
	[openfiletimer release];
		
	inFITSfilename = (char*)[myFITSfile UTF8String];

	[self stopProgressBar:NO];
	[self setProgressText:@"querying file..."];
	[progressView setDoubleValue:0.0];
	HPIC_ERROR_FLAG = FALSE;
				
	//call hpic function to check whether file is valid FITS, and extract params
	FITSflag = hpic_fits_map_test(inFITSfilename,&nside,&order,&coord,&type,&nmaps);
	
	if (FITSflag && !HPIC_ERROR_FLAG) 
	{
			
		[self setProgressText:@"this is a valid HEALPix FITS file, reading data..."];
		[self setMap_nside:nside];
		[self setPixelordering:order];
		
		if ([self maptype]!=0) 
		{
			//free T map
			if (hpic_Tmap != NULL) 
			{
				hpic_float_free(hpic_Tmap);
				hpic_Tmap = NULL;
			}
			
			//free Q map
			if (hpic_Qmap != NULL)
			{
				hpic_float_free(hpic_Qmap);
				hpic_Qmap = NULL;
			}
			
			//free U map
			if (hpic_Umap != NULL)
			{
				hpic_float_free(hpic_Umap);
				hpic_Umap = NULL;
			}
			
			//free N map
			if (hpic_Nmap != NULL) 
			{
				hpic_float_free(hpic_Nmap);
				hpic_Nmap = NULL;
			}
			
			//free map container structure
			if (hpic_maps != NULL)
			{
				hpic_fltarr_free(hpic_maps);
				hpic_maps = NULL;
			}
		}
				 
		keys = hpic_keys_alloc();
					
		if (nmaps==3) 
		{						
			hpic_Tmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Qmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Umap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_maps = hpic_fltarr_alloc(3);
			
			hpic_fltarr_set(hpic_maps,0,hpic_Tmap);
			hpic_fltarr_set(hpic_maps,1,hpic_Qmap);
			hpic_fltarr_set(hpic_maps,2,hpic_Umap);	
			
			if (type == HPIC_FITS_FULL) 
			{
				hpic_fits_full_read(inFITSfilename,creator,extname,hpic_maps,keys);								
			}
			else 
			{
				[self setProgressText:@"this is a cut sky map... OK"];
				cuterrs = hpic_float_alloc(nside,order,coord,HPIC_AUTO);
				cutpix = hpic_int_alloc(nside,order,coord,HPIC_AUTO);
				cuthits = hpic_int_alloc(nside,order,coord,HPIC_AUTO);				
				hpic_fits_cut_read(inFITSfilename,creator,extname,cutpix,cuthits,cuterrs,hpic_maps,keys);
			}	
			
			//TO DO: implement a drawer which displays FITS keys
			npix = hpic_float_npix_get(hpic_Tmap);																
			[self setNpixels:(int)npix];
			pol=1;
		}
		
		else if (nmaps==2) 
		{
			//TO DO: note that currently, even though there are only 2 maps
			//in the FITS file, I set up a garbage U map (thus wasting memory) 
			//to simplify the code in CMBdata. I should change this eventually.
			hpic_Tmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Qmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Umap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_maps = hpic_fltarr_alloc(3);
			
			hpic_fltarr_set(hpic_maps,0,hpic_Tmap);
			hpic_fltarr_set(hpic_maps,1,hpic_Qmap);
			hpic_fltarr_set(hpic_maps,2,hpic_Umap);			
			
			if (type == HPIC_FITS_FULL) 
			{
				hpic_fits_full_read(inFITSfilename,creator,extname,hpic_maps,keys);								
			}
			else 
			{
				[self setProgressText:@"this is a cut sky map... OK"];
				cuterrs = hpic_float_alloc(nside,order,coord,HPIC_AUTO);
				cutpix = hpic_int_alloc(nside,order,coord,HPIC_AUTO);
				cuthits = hpic_int_alloc(nside,order,coord,HPIC_AUTO);				
				hpic_fits_cut_read(inFITSfilename,creator,extname,cutpix,cuthits,cuterrs,hpic_maps,keys);
			}			
			
			npix = hpic_float_npix_get(hpic_Tmap);					
			[self setNpixels:(int)npix];
			pol=2;

		}
		
		else if (nmaps==1) 
		{
			hpic_Tmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Qmap = NULL;
			hpic_Umap = NULL;
			hpic_maps = hpic_fltarr_alloc(1);
			hpic_fltarr_set(hpic_maps,0,hpic_Tmap);			
			if (type == HPIC_FITS_FULL)
			{
				hpic_fits_full_read(inFITSfilename,creator,extname,hpic_maps,keys);								
			}
			else 
			{
				//currently the facility for reading and displaying cut sky maps is
				//not well tested, but at least the code will (well, should) not crash...
				
				[self setProgressText:@"this is a cut sky map... OK"];
				cuterrs = hpic_float_alloc(nside,order,coord,HPIC_AUTO);
				cutpix = hpic_int_alloc(nside,order,coord,HPIC_AUTO);
				cuthits = hpic_int_alloc(nside,order,coord,HPIC_AUTO);				
				hpic_fits_cut_read(inFITSfilename,creator,extname,cutpix,cuthits,cuterrs,hpic_maps,keys);
			}						
			npix = hpic_float_npix_get(hpic_Tmap);					
			[self setNpixels:(int)npix];
			pol=0;
		}		
		
		else if (nmaps==4)
		{
			//added to handle the 4-map (T,Q,U,N) files provided in the WMAP 4-year data release. 
			//Unfortunately cannot view N map without major changes to GUI, so for now I just 
			//set things up so the 4 maps are read, and the N map immediately freed.
			
			hpic_Tmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Qmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Umap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			hpic_Nmap = hpic_float_alloc(nside,order,coord,HPIC_STND);
			
			hpic_maps = hpic_fltarr_alloc(4);
			
			hpic_fltarr_set(hpic_maps,0,hpic_Tmap);
			hpic_fltarr_set(hpic_maps,1,hpic_Qmap);
			hpic_fltarr_set(hpic_maps,2,hpic_Umap);	
			hpic_fltarr_set(hpic_maps,3,hpic_Nmap);	

			if (type == HPIC_FITS_FULL)
			{
				hpic_fits_full_read(inFITSfilename,creator,extname,hpic_maps,keys);								
			}
			else 
			{
				[self setProgressText:@"this is a cut sky map... OK"];
				cuterrs = hpic_float_alloc(nside,order,coord,HPIC_AUTO);
				cutpix = hpic_int_alloc(nside,order,coord,HPIC_AUTO);
				cuthits = hpic_int_alloc(nside,order,coord,HPIC_AUTO);				
				hpic_fits_cut_read(inFITSfilename,creator,extname,cutpix,cuthits,cuterrs,hpic_maps,keys);
			}			
			npix = hpic_float_npix_get(hpic_Tmap);																
			[self setNpixels:(int)npix];
			pol=1;
			
			hpic_float_free(hpic_Nmap);
			hpic_Nmap = NULL;
			
		}
		
		else if (nmaps>4)
		{
			[self setProgressText:@"FITS file contains more than 4 maps, not currently supported"];
			FITSflag=0;
		}
		else 
		{
			[self setProgressText:@"found strange value for number of maps"];
			FITSflag=0;
		}
		
		[self setPolarisation:pol];

		hpic_keys_free(keys);
	}
	
	else 
	{
		[self setProgressText:@"Does not seem to be a valid HEALPix FITS file"];
	}
	
	if (FITSflag && !HPIC_ERROR_FLAG) 
	{	
		//OK, file is kosher and data has been read, now we can display stuff		
		[FITSfilename setStringValue:[myFITSfile lastPathComponent]];
		[FITSfilename display];
		
		if (pol==0) 
		{
			pixelcount = [[NSString alloc] initWithFormat:
				@"%d pixels (T only)",[self Npixels]];
		}
		
		else if (pol==1)
		{
			pixelcount = [[NSString alloc] initWithFormat:
				@"%d pixels (T,Q,U)",[self Npixels]];
		}
		
		else if (pol==2)
		{
			pixelcount = [[NSString alloc] initWithFormat:
				@"%d pixels (T,N only)",[self Npixels]];
		}
		
		[self setPixelnumText:pixelcount];
		
		//show T map first				
		[self setMaptype:1];
		[self stopProgressBar:NO];
		
		//make sure to reset all color range information on loading the new map
		[myLittleOpenGLview reinitializeHistograms];
		
		//enable text editing of the histogram range values
		[widget_max setEditable:YES];
		[widget_min setEditable:YES];
		
		//switch to interactive mode
		render_mode = 1;
		[myCMBdata genTextures_interactive];
		
		[self setProgressText:@""];
		[self setProgressIndicator:0.0];
		[self enable_zoom_slider];
		
		id theCell_interactive = [render_mode_matrix cellWithTag:1];
		id theCell_render = [render_mode_matrix cellWithTag:2];
		id theCell_presentation = [render_mode_matrix cellWithTag:3];				
		[theCell_render setEnabled:YES];
		[theCell_interactive setEnabled:NO];
		[theCell_presentation setEnabled:YES];
		[render_mode_matrix selectCellWithTag:1];
		
		//do not show Stokes vectors initially
		[self setStokesflag:0];
		
		//enable the GUI buttons according to the available data
		[Tmapbutton setEnabled:(BOOL)YES];
		[Tmapbutton setState:NSOnState];	
		
		[Qmapbutton setState:NSOffState];
		[Umapbutton setState:NSOffState];
		[Pmapbutton setState:NSOffState];
		
		[showStokesbutton setEnabled:(BOOL)NO];
		[showStokesbutton setState:NSOffState];
		
		[color_matrix setEnabled:(BOOL)YES];
		[linlog_matrix setEnabled:(BOOL)YES];
		[updatebutton setEnabled:(BOOL)YES];
		
		if (pol==0) //T only
		{
			[Qmapbutton setEnabled:(BOOL)NO];
			[Umapbutton setEnabled:(BOOL)NO];
			[Pmapbutton setEnabled:(BOOL)NO];
			[genStokesbutton setEnabled:(BOOL)NO];
			[Stokesslider setEnabled:(BOOL)NO];
			[self setStokesflag:0];			
		} 				
		else if (pol==1) //T, Q, U
		{
			[Qmapbutton setTitle:@"Q"];
			[Qmapbutton setEnabled:(BOOL)YES];
			[Umapbutton setEnabled:(BOOL)YES];
			[Pmapbutton setEnabled:(BOOL)YES];
			[genStokesbutton setEnabled:(BOOL)YES];
			[Stokesslider setEnabled:(BOOL)YES];					
		}
		else if (pol==2) //T, N
		{
			[Qmapbutton setTitle:@"N"];
			[Qmapbutton setEnabled:(BOOL)YES];
			[Umapbutton setEnabled:(BOOL)NO];
			[Pmapbutton setEnabled:(BOOL)NO];
			[genStokesbutton setEnabled:(BOOL)NO];
			[Stokesslider setEnabled:(BOOL)NO];
		}
		
		//reset pixel info box.
		
		//angles
		NSString *Theta_pixelinfo_text;
		NSString *Phi_pixelinfo_text;
		Theta_pixelinfo_text = [[NSString alloc] initWithFormat:@"Th:"];
		Phi_pixelinfo_text = [[NSString alloc] initWithFormat:@"Ph:"];
		[self setPixinfoText_theta:Theta_pixelinfo_text];
		[self setPixinfoText_phi:Phi_pixelinfo_text];				
		
		//N
		NSString *N_pixelinfo_text;
		N_pixelinfo_text = [[NSString alloc] initWithFormat:@"n:"];
		[self setPixinfoText_N:N_pixelinfo_text];
		[N_pixelinfo_text release];
		
		//T
		NSString *T_pixelinfo_text;
		T_pixelinfo_text = [[NSString alloc] initWithFormat:@"T:"];
		[self setPixinfoText_T:T_pixelinfo_text];
		[T_pixelinfo_text release];
		
		//Q
		NSString *Q_pixelinfo_text;
		if (pol==1 || pol==0) 
		{
			Q_pixelinfo_text = [[NSString alloc] initWithFormat:@"Q:"];
		}
		else if (pol==2) 
		{
			Q_pixelinfo_text = [[NSString alloc] initWithFormat:@"N:"];
		}
		[self setPixinfoText_Q:Q_pixelinfo_text];
		[Q_pixelinfo_text release];
		
		//U
		NSString *U_pixelinfo_text;
		U_pixelinfo_text = [[NSString alloc] initWithFormat:@"U:"];
		[self setPixinfoText_U:U_pixelinfo_text];
		[U_pixelinfo_text release];
		
		//P
		NSString *P_pixelinfo_text;
		P_pixelinfo_text = [[NSString alloc] initWithFormat:@"P:"];
		[self setPixinfoText_P:P_pixelinfo_text];
		[P_pixelinfo_text release];

	}
	
	[myOpenGLview setNeedsDisplay:YES];
	
	if (HPIC_ERROR_FLAG) 
	{
		NSString *guierrorstr=[[NSString alloc] initWithFormat:@"hpic error: %s",hpic_errorstr];		
		[self setProgressText:guierrorstr];
		[guierrorstr release];
	}
	
}


/**********************************************************************/
/*                        accessor methods                            */
/**********************************************************************/

- (int)maptype
{
	return maptype;
}

- (void)setMaptype:(int)map_type
{
	maptype = map_type;
}

- (int)Npixels
{
	return Npixels;
}

- (int)pixelordering
{
	return pixelordering;
}

- (int)polarisation
{
	return polarisation;
}

- (int)map_nside
{
	return map_nside;
}

- (void)setNpixels:(int)pixelnumber
{
	Npixels = (int)pixelnumber;
}

- (void)setPolarisation:(int)pol
{
	polarisation = pol;
}

- (void)setMap_nside:(int)nside
{
	map_nside = nside;
}

- (void)setPixelordering:(int)order
{
	pixelordering = order;
}

- (int)Stokes_flag
{
	return Stokes_flag;
}
- (void)setStokesflag:(int)flag
{
	Stokes_flag = flag;
}

- (int)colormap_flag
{
	return colormap_flag;
}

- (int)SetColormap_flag:(int)flag
{
	colormap_flag = flag;
}

- (int)linlog_flag
{
	return linlog_flag;
}

- (int)render_mode
{
	return render_mode;
}

- (void)setColorrange_interactive:(colorrange)c
{
	colorrange_interactive = c;	
}
- (void)setColorrange_render:(colorrange)c
{
	colorrange_render = c;	
}
- (void)setColorrange_presentation:(colorrange)c
{
	colorrange_presentation = c;	
}

- (colorrange *)colorrange_interactive
{
	return &colorrange_interactive;
}
- (colorrange *)colorrange_render
{
	return &colorrange_render;
}
- (colorrange *)colorrange_presentation
{
	return &colorrange_presentation;
}


- (void)dealloc
{
	if ([self maptype]!=0) 
	{
		if (hpic_Tmap != NULL) 
		{
			hpic_float_free(hpic_Tmap);
		}
		if (hpic_Qmap != NULL)
		{
			hpic_float_free(hpic_Qmap);
		}
		if (hpic_Umap != NULL)
		{
			hpic_float_free(hpic_Umap);
		}
		if (hpic_maps != NULL)
		{
			hpic_fltarr_free(hpic_maps);
		}
	}	
	
	[preferenceController release];
	[cmbviewAboutPanel release];
	[super dealloc];
}

@end
