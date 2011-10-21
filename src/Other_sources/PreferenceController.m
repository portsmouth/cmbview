/*****************************************************************************
* Copyright 2005 Jamie Portsmouth <jamports@mac.com>                         *
*                                                                            *
* This file is part of CMBview, a program for viewing HEALPix-format         *
* CMB data on an OpenGL-rendered 3d sphere.                                  * 
*                                                                            *
* See the source files in the Classes folder for the full license preamble   *
*                                                                            *
*****************************************************************************/

#import "PreferenceController.h"

//general panel
NSString *CMBview_backgrndcolorkey = @"OpenGLviewBackgroundColor";
NSString *CMBview_fovykey = @"fovy"; 
NSString *CMBview_orthokey = @"ortho"; 
NSString *CMBview_mousesensitivitykey = @"mousesensitivity"; 
NSString *CMBview_presentationbackgrndcolorkey = @"presentationbackgrndcolor";
NSString *CMBview_presentationtextcolorkey = @"presentationtextcolor";
NSString *CMBview_exportimagesizekey = @"exportimagesize";
//textures panel
NSString *CMBview_texnumkey = @"Ntexture";
NSString *CMBview_texinterpolatekey = @"texinterpolate";
//lighting panel
NSString *CMBview_ambientlightkey = @"ambientlightColor";
NSString *CMBview_diffuselightkey = @"diffuselightColor";
NSString *CMBview_specularlightkey = @"specularlightColor";
NSString *CMBview_ambientmatkey = @"ambientmatColor";
NSString *CMBview_diffusematkey = @"diffusematColor";
NSString *CMBview_specularmatkey = @"specularmatColor";
NSString *CMBview_lightingenabledkey = @"lightingenabledFlag"; 
NSString *CMBview_ambientenabledkey = @"ambientenabledFlag"; 
NSString *CMBview_diffuseenabledkey = @"diffuseenabledFlag"; 
NSString *CMBview_specularenabledkey = @"specularenabledFlag"; 
NSString *CMBview_shininesskey = @"shininess"; 
NSString *CMBview_lightradiuskey = @"lightradius"; 
NSString *CMBview_lightthetakey = @"lighttheta"; 
NSString *CMBview_lightphikey = @"lightphi"; 
//colormap panel
NSString *CMBview_colormapchoicekey = @"colormapchoice";
NSString *CMBview_colormapreversekey = @"colormapreverse";
//stokes panel
NSString *CMBview_stokesgridkey = @"stokesgrid"; 
NSString *CMBview_stokespropskey = @"stokesprops"; 
//grid panel
NSString *CMBview_gridspacekey = @"gridspace"; 
NSString *CMBview_gridpropskey = @"gridprops"; 


@implementation PreferenceController

- (id)init
{
	self = [super initWithWindowNibName:@"Preferences"];
	return self;							  
}

/* read user preferences to set up pref panel */
- (void)windowDidLoad
{
	NSColor *color;
	
	//general panel
	color = [self backgrndcolor];
	[colorWell_background setColor:color];
	
	color = [self presentationbackgrndColor];
	[presentationbackgrndColorWell setColor:color];
	
	color = [self presentationtextColor];
	[presentationtextColorWell setColor:color];
	
	float fovyValue = [self fovy];
	[fovy_slider setFloatValue:fovyValue];
	NSString *fovy_text = [[NSString alloc] initWithFormat:@"%3.2f",fovyValue];
	[fovyslider_text setStringValue:fovy_text];
	[fovy_text release];	
	
	BOOL orthostate = [self orthoEnable];		
	[orthoEnable setState:orthostate];
	if (orthostate) {
		[fovy_slider setEnabled:NO];
	}
	else {
		[fovy_slider setEnabled:YES];
	}
	
	int exportimagesizeValue = [self exportimagesize];
	[exportimagesize setIntValue:exportimagesizeValue];
	NSString *export_text = [[NSString alloc] initWithFormat:@"%dx%d",exportimagesizeValue,
		exportimagesizeValue];
	[exportimagesize_text setStringValue:export_text];
	[export_text release];	
			
	//grid panel
	int gridnum = [self GridSpacing];
	[grid_index setIntValue:gridnum];	
	int GRID_NUM = 4*gridnum;
	float GRID_SPACE = 360.0/(float)GRID_NUM;
	NSString *grid_text;
	if (gridnum==1) {	
		grid_text = [[NSString alloc] initWithFormat:
			@"%d line, %5.3f degrees",GRID_NUM/2-1,GRID_SPACE];
	}
	else {
		grid_text = [[NSString alloc] initWithFormat:
			@"%d lines, %5.3f degrees",GRID_NUM/2-1,GRID_SPACE];
	}
	[num_grid setStringValue:grid_text];
	[grid_text release];	
	
	NSMutableArray *myMutableArray = [self gridProps];
	enum gridproperties sp; 
	
	sp = grid_enable;
	NSNumber *gridenableValue = [myMutableArray objectAtIndex:sp];
	[gridEnable setState:[gridenableValue boolValue]];
	
	sp = grid_opacity;
	NSNumber *gridopacityValue = [myMutableArray objectAtIndex:sp];
	[gridopacity setFloatValue:[gridopacityValue floatValue]];
	NSString *grid_text2 = [[NSString alloc] initWithFormat:@"%3.2f",
		[gridopacityValue floatValue]];
	[gridopacity_text setStringValue:grid_text2];
	[grid_text2 release];	
		
	sp = grid_thickness;
	NSNumber *gridthicknessValue = [myMutableArray objectAtIndex:sp];
	[gridthickness setFloatValue:[gridthicknessValue floatValue]];
	
	sp = grid_color;
	color = [NSKeyedUnarchiver unarchiveObjectWithData:[myMutableArray objectAtIndex:sp]];
	[gridColorWell setColor:color];
	
	sp = gridincaps;
	NSNumber *gridincapsValue = [myMutableArray objectAtIndex:sp];
	[gridincapsEnable setState:[gridincapsValue boolValue]];

	//textures panel
	int texnum = [self TextureLevel];
	[texture_index setIntValue:texnum];

	int Ntexture = (int)256*pow(2,texnum);
	NSString *texture_level_text = [[NSString alloc] initWithFormat:@"%d",Ntexture];
	[texture_level setStringValue:texture_level_text];
	[texture_level_text release];
	
	NSString *num_texels_text = [[NSString alloc] initWithFormat:@"%d",6*(int)SQR((float)Ntexture/(1.0+flange))];
	[num_texels setStringValue:num_texels_text];
	[num_texels_text release];
	
	int texelinterpolation = [self TexelInterpolation];
	[texelinterpolation_matrix selectCellWithTag:texelinterpolation];
		
	//colormap panel
	[colormapReverse setState:[self colormapReverse]];

	//lighting panel		
	color = [self ambientlightcolor];
	[colorWell_ambientlight setColor:color];
	color = [self ambientmatcolor];
	[colorWell_ambientmat setColor:color];
	
	color = [self diffuselightcolor];
	[colorWell_diffuselight setColor:color];
	color = [self diffusematcolor];
	[colorWell_diffusemat setColor:color];
	
	color = [self specularlightcolor];
	[colorWell_specularlight setColor:color];
	color = [self specularmatcolor];
	[colorWell_specularmat setColor:color];
		
	float lightTheta = [self lightTheta];
	NSString *light_text1 = [[NSString alloc] initWithFormat:@"%3.2f",lightTheta];
	[lighttheta_text setStringValue:light_text1];
	[light_text1 release];
	
	float lightPhi = [self lightPhi];
	NSString *light_text2 = [[NSString alloc] initWithFormat:@"%3.2f",lightPhi];
	[lightphi_text setStringValue:light_text2];
	[light_text2 release];
		
	float lightRadius = [self lightRadius];
	NSString *radius_text = [[NSString alloc] initWithFormat:@"%3.2f",lightRadius];
	[lightradius_text setStringValue:radius_text];
	[radius_text release];
	
	float shininessVal = [self shininess];
	NSString *shine_text = [[NSString alloc] initWithFormat:@"%3.2f",shininessVal];
	[lightshininess_text setStringValue:shine_text];
	[shine_text release];
	
	[lighttheta setFloatValue:lightTheta];
	[lightphi setFloatValue:lightPhi];
	[lightradius setFloatValue:lightRadius];
	[shininess setFloatValue:shininessVal];
	
	[lightingEnable setState:[self lightingEnable]];
	[ambientEnable setState:[self ambientEnable]];
	[diffuseEnable setState:[self diffuseEnable]];
	[specularEnable setState:[self specularEnable]];
	
	//stokes panel
	int stokesValue = [self stokesNum];
	[stokesnum setFloatValue:(float)stokesValue];
	NSString *stokes_text = [[NSString alloc] initWithFormat:@"%d",stokesValue];
	[stokesnum_text setStringValue:stokes_text];
	[stokes_text release];
	
	NSMutableArray *myMutableArray2 = [self stokesProps];
	enum stokesproperties sp2; 
		
	sp2 = stokes_opacity;
	NSNumber *stokesopacityValue = [myMutableArray2 objectAtIndex:sp2];
	[stokesopacity setFloatValue:[stokesopacityValue floatValue]];
	NSString *stokes_text2 = [[NSString alloc] initWithFormat:@"%3.2f",
									[stokesopacityValue floatValue]];
	[stokesopacity_text setStringValue:stokes_text2];
	[stokes_text2 release];	
	
	sp2 = stokes_length;
	NSNumber *stokeslengthValue = [myMutableArray2 objectAtIndex:sp2];
	[stokeslength setFloatValue:[stokeslengthValue floatValue]];
	NSString *stokes_text3 = [[NSString alloc] initWithFormat:@"%3.2f",
		                            [stokeslengthValue floatValue]];
	[stokeslength_text setStringValue:stokes_text3];
	[stokes_text3 release];	
	
	sp2 = stokes_thickness;
	NSNumber *stokesthicknessValue = [myMutableArray2 objectAtIndex:sp2];
	[stokesthickness setFloatValue:[stokesthicknessValue floatValue]];
	
	sp2 = stokes_color;
	color = [NSKeyedUnarchiver unarchiveObjectWithData:[myMutableArray2 objectAtIndex:sp2]];
	[stokesColorWell setColor:color];
	
	sp2 = stokes_convention;
	NSNumber *stokesconventionValue = [myMutableArray2 objectAtIndex:sp2];	
	[stokesconvention_matrix selectCellWithTag:[stokesconventionValue intValue]];

}

- (IBAction)restoreDefaults:(id)sender
{
	//warning panel
	NSString *msg;
	msg = [[NSString alloc] initWithFormat:
		@"This will erase all of your current preferences. OK?"];
	int choice = NSRunAlertPanel(@"Warning",msg,@"Go ahead",@"Cancel",nil);
	
	if (choice == NSAlertDefaultReturn) {
		NSUserDefaults *defaults;		
		defaults = [NSUserDefaults standardUserDefaults];
		NSNotificationCenter *nc;
		nc = [NSNotificationCenter defaultCenter];
		
		[defaults removeObjectForKey:CMBview_texnumkey];
		[defaults removeObjectForKey:CMBview_texinterpolatekey];
		[defaults removeObjectForKey:CMBview_backgrndcolorkey];
		[defaults removeObjectForKey:CMBview_fovykey];
		[defaults removeObjectForKey:CMBview_orthokey ];
		[defaults removeObjectForKey:CMBview_mousesensitivitykey];
		[defaults removeObjectForKey:CMBview_presentationbackgrndcolorkey];
		[defaults removeObjectForKey:CMBview_presentationtextcolorkey];
		[defaults removeObjectForKey:CMBview_exportimagesizekey];
		[defaults removeObjectForKey:CMBview_ambientlightkey];
		[defaults removeObjectForKey:CMBview_diffuselightkey];
		[defaults removeObjectForKey:CMBview_specularlightkey];
		[defaults removeObjectForKey:CMBview_ambientmatkey];
		[defaults removeObjectForKey:CMBview_diffusematkey];
		[defaults removeObjectForKey:CMBview_specularmatkey];
		[defaults removeObjectForKey:CMBview_lightingenabledkey];
		[defaults removeObjectForKey:CMBview_ambientenabledkey];
		[defaults removeObjectForKey:CMBview_diffuseenabledkey];
		[defaults removeObjectForKey:CMBview_specularenabledkey];
		[defaults removeObjectForKey:CMBview_shininesskey];
		[defaults removeObjectForKey:CMBview_lightradiuskey];
		[defaults removeObjectForKey:CMBview_lightthetakey];
		[defaults removeObjectForKey:CMBview_lightphikey];
		[defaults removeObjectForKey:CMBview_colormapchoicekey];
		[defaults removeObjectForKey:CMBview_colormapreversekey];
		[defaults removeObjectForKey:CMBview_stokesgridkey];
		[defaults removeObjectForKey:CMBview_stokespropskey];
		[defaults removeObjectForKey:CMBview_gridspacekey];
		[defaults removeObjectForKey:CMBview_gridpropskey];

		[nc postNotificationName:@"CMBview_texelinterpolationChanged" object:self];	
		[nc postNotificationName:@"CMBview_backgrndcolorChanged" object:self];
		[nc postNotificationName:@"CMBview_fovyChanged" object:self];
		[nc postNotificationName:@"CMBview_orthoChanged" object:self];	
		[nc postNotificationName:@"CMBview_mousesensitivityChanged" object:self];	
		[nc postNotificationName:@"CMBview_presentationbackgrndcolorChanged" object:self];	
		[nc postNotificationName:@"CMBview_presentationtextcolorChanged" object:self];
		[nc postNotificationName:@"CMBview_colormapReverseChanged" object:self];	
		[nc postNotificationName:@"CMBview_stokesPropsChanged" object:self];
		[nc postNotificationName:@"CMBview_gridSpacingChanged" object:self];
		[nc postNotificationName:@"CMBview_gridPropsChanged" object:self];		
		[nc postNotificationName:@"CMBview_lightingenableChanged" object:self];	
		[nc postNotificationName:@"CMBview_ambientenableChanged" object:self];
		[nc postNotificationName:@"CMBview_diffuseenableChanged" object:self];
		[nc postNotificationName:@"CMBview_specularenableChanged" object:self];
		[nc postNotificationName:@"CMBview_rotatelightthetaChanged" object:self];
		[nc postNotificationName:@"CMBview_rotatelightphiChanged" object:self];
		[nc postNotificationName:@"CMBview_shininessChanged" object:self];
		[nc postNotificationName:@"CMBview_lightradiusChanged" object:self];
		[nc postNotificationName:@"CMBview_ambientlightChanged" object:self];
		[nc postNotificationName:@"CMBview_diffuselightChanged" object:self];
		[nc postNotificationName:@"CMBview_specularlightChanged" object:self];
		[nc postNotificationName:@"CMBview_ambientmatChanged" object:self];
		[nc postNotificationName:@"CMBview_diffusematChanged" object:self];
		[nc postNotificationName:@"CMBview_specularmatChanged" object:self];
						
		[self windowDidLoad];
	}	
}

/* general panel */
- (IBAction)changeFovy:(id)sender
{
	float fovyValue = [fovy_slider floatValue];	
	NSString *fovy_text = [[NSString alloc] initWithFormat:@"%3.2f",fovyValue];
	[fovyslider_text setStringValue:fovy_text];
	[fovy_text release];	
		
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithFloat:fovyValue]
				 forKey:CMBview_fovykey];	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_fovyChanged" object:self];
}

- (float)fovy
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults floatForKey:CMBview_fovykey];
}

- (BOOL)orthoEnable
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults boolForKey:CMBview_orthokey];
}

- (IBAction)changeOrthoEnable:(id)sender
{
	BOOL orthostate = [sender state];
	
	[[NSUserDefaults standardUserDefaults] setBool:orthostate
											forKey:CMBview_orthokey];
	if (orthostate) {
		[fovy_slider setEnabled:NO];
	}
	else {
		[fovy_slider setEnabled:YES];
	}
								
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_orthoChanged" object:self];	
}

- (float)MouseSensitivity
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults floatForKey:CMBview_mousesensitivitykey];
}

- (IBAction)changeMouseSensitivity:(id)sender
{
	float mousesensitivityValue = [mousesensitivity_slider floatValue];	
	NSString *ms_text = [[NSString alloc] initWithFormat:@"%3.2f",mousesensitivityValue];
	[mousesensitivityslider_text setStringValue:ms_text];
	[ms_text release];	
	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithFloat:mousesensitivityValue]
				 forKey:CMBview_mousesensitivitykey];

	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_mousesensitivityChanged" object:self];	
}


- (int)exportimagesize
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults integerForKey:CMBview_exportimagesizekey];
	
}

- (IBAction)changeExportimagesize:(id)sender
{
	int exportimagesizeValue = (int)[exportimagesize floatValue];	
	NSString *export_text = [[NSString alloc] initWithFormat:@"%dx%d",exportimagesizeValue,
		exportimagesizeValue];
	[exportimagesize_text setStringValue:export_text];
	[export_text release];	
	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithInt:exportimagesizeValue]
				 forKey:CMBview_exportimagesizekey];	
}

- (NSColor *)presentationbackgrndColor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_presentationbackgrndcolorkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changePresentationbackgrndColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_presentationbackgrndcolorkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_presentationbackgrndcolorChanged" object:self];	
}

- (NSColor *)presentationtextColor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_presentationtextcolorkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changePresentationtextColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_presentationtextcolorkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_presentationtextcolorChanged" object:self];
}


- (NSColor *)backgrndcolor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_backgrndcolorkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changeBackgroundColor:(id)sender
{		
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_backgrndcolorkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_backgrndcolorChanged" object:self];
}


//grid panel

- (IBAction)changeGridEnable:(id)sender
{
	BOOL gridenableValue = [sender state];
	
	NSMutableArray *myMutableArray = [self gridProps];
	
	enum gridproperties sp; sp = grid_enable;
	[myMutableArray replaceObjectAtIndex:sp
							  withObject:[NSNumber numberWithBool:gridenableValue]];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_gridpropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_gridPropsChanged" object:self];
}

- (IBAction)changeGridOpacity:(id)sender
{
	float gridopacityValue = [gridopacity floatValue];	
	NSString *grid_text = [[NSString alloc] initWithFormat:@"%3.2f",gridopacityValue];
	[gridopacity_text setStringValue:grid_text];
	[grid_text release];	
	
	NSMutableArray *myMutableArray = [self gridProps];

	enum gridproperties sp; sp = grid_opacity;
	[myMutableArray replaceObjectAtIndex:sp
							  withObject:[NSNumber numberWithFloat:gridopacityValue]];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_gridpropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_gridPropsChanged" object:self];
}

- (IBAction)changeGridColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	
	NSMutableArray *myMutableArray = [self gridProps];				
	enum gridproperties sp; sp = grid_color;
	[myMutableArray replaceObjectAtIndex:sp
							  withObject:colorAsData];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_gridpropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_gridPropsChanged" object:self];	
}

- (IBAction)changeGridThickness:(id)sender
{
	float gridthicknessValue = [gridthickness floatValue];	
		
	NSMutableArray *myMutableArray = [self gridProps];
	
	enum gridproperties sp; sp = grid_thickness;
	[myMutableArray replaceObjectAtIndex:sp
							  withObject:[NSNumber numberWithFloat:gridthicknessValue]];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_gridpropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_gridPropsChanged" object:self];	
}

- (IBAction)changeGridInCaps:(id)sender
{
	BOOL gridincapsValue = [sender state];
	
	NSMutableArray *myMutableArray = [self gridProps];
	
	enum gridproperties sp; sp = gridincaps;
	[myMutableArray replaceObjectAtIndex:sp
							  withObject:[NSNumber numberWithBool:gridincapsValue]];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_gridpropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_gridPropsChanged" object:self];		
}

- (NSMutableArray *)gridProps
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	NSData *MAasData = [defaults objectForKey:CMBview_gridpropskey];
	NSMutableArray *myMutableArray = [NSKeyedUnarchiver 
											unarchiveObjectWithData:MAasData];	
	return myMutableArray;
}

- (int)GridSpacing
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults integerForKey:CMBview_gridspacekey];	
}
- (IBAction)changeGridSpacing:(id)sender
{		
	int gridnum = [grid_index intValue];	
	int GRID_NUM = 4*gridnum;
	float GRID_SPACE = 360.0/(float)GRID_NUM;
	NSString *grid_text;
	if (gridnum==1) {	
		grid_text = [[NSString alloc] initWithFormat:
			@"%d line, %5.3f degrees",GRID_NUM/2-1,GRID_SPACE];
	}
	else {
		grid_text = [[NSString alloc] initWithFormat:
			@"%d lines, %5.3f degrees",GRID_NUM/2-1,GRID_SPACE];
	}
	[num_grid setStringValue:grid_text];
	[grid_text release];	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithInt:gridnum]
				 forKey:CMBview_gridspacekey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_gridSpacingChanged" object:self];
}


/* stokes panel */
- (int)stokesNum
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults integerForKey:CMBview_stokesgridkey];
}
- (IBAction)changeStokesNum:(id)sender
{
	int stokesValue = (int)[stokesnum floatValue];	
	NSString *stokes_text = [[NSString alloc] initWithFormat:@"%d",stokesValue];
	[stokesnum_text setStringValue:stokes_text];
	[stokes_text release];	
				
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithInt:stokesValue]
				 forKey:CMBview_stokesgridkey];	
}

- (NSMutableArray *)stokesProps
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	NSData *MAasData = [defaults objectForKey:CMBview_stokespropskey];
	NSMutableArray *myMutableArray = [NSKeyedUnarchiver 
											unarchiveObjectWithData:MAasData];
	
	return myMutableArray;
}
- (IBAction)changeStokesOpacity:(id)sender
{
	float stokesopacityValue = [stokesopacity floatValue];	
	NSString *stokes_text = [[NSString alloc] initWithFormat:@"%3.2f",stokesopacityValue];
	[stokesopacity_text setStringValue:stokes_text];
	[stokes_text release];	
	
	NSMutableArray *myMutableArray = [self stokesProps];				
	enum stokesproperties sp; sp = stokes_opacity;
	[myMutableArray replaceObjectAtIndex:sp
							  withObject:[NSNumber numberWithFloat:stokesopacityValue]];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
					  forKey:CMBview_stokespropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_stokesPropsChanged" object:self];
}

- (IBAction)changeStokesThickness:(id)sender
{
	float stokesthicknessValue = [stokesthickness floatValue];	
	
	NSMutableArray *myMutableArray = [self stokesProps];				
	enum stokesproperties sp; sp = stokes_thickness;
	[myMutableArray replaceObjectAtIndex:sp 
							  withObject:[NSNumber numberWithFloat:stokesthicknessValue]];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_stokespropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_stokesPropsChanged" object:self];
}

- (IBAction)changeStokesLength:(id)sender
{
	float stokeslengthValue = [stokeslength floatValue];	
	NSString *stokes_text = [[NSString alloc] initWithFormat:@"%3.2f",stokeslengthValue];
	[stokeslength_text setStringValue:stokes_text];
	[stokes_text release];	
	
	NSMutableArray *myMutableArray = [self stokesProps];				
	enum stokesproperties sp; sp = stokes_length;
	[myMutableArray replaceObjectAtIndex:sp 
							  withObject:[NSNumber numberWithFloat:stokeslengthValue]];
	
	
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_stokespropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_stokesPropsChanged" object:self];
}

- (IBAction)changeStokesColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	
	NSMutableArray *myMutableArray = [self stokesProps];				
	enum stokesproperties sp; sp = stokes_color;
	[myMutableArray replaceObjectAtIndex:sp
							  withObject:colorAsData];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_stokespropskey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_stokesPropsChanged" object:self];
}


- (IBAction)changeStokesConvention:(id)sender
{
	id theCell = [sender selectedCell];
	int theTag = [theCell tag];
	
	NSLog(@"stokes convention button: %d",theTag);
	
	NSMutableArray *myMutableArray = [self stokesProps];				
	enum stokesproperties sp; sp = stokes_convention;
	[myMutableArray replaceObjectAtIndex:sp 
							  withObject:[NSNumber numberWithInt:theTag]];
	
	NSData *updatedMAasData = [NSKeyedArchiver archivedDataWithRootObject:myMutableArray];		
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:updatedMAasData
				 forKey:CMBview_stokespropskey];
				
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_stokesPropsChanged" object:self];	
}


/* colormap panel */
- (BOOL)colormapReverse
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults boolForKey:CMBview_colormapreversekey];
}

- (IBAction)changeColormapReverse:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[sender state]
											forKey:CMBview_colormapreversekey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_colormapReverseChanged" object:self];	
}

- (IBAction)colormap_matrix:(id)sender
{
	id theCell = [sender selectedCell];
	int theTag = [theCell tag];
	
	current_colormap_ptr = &mycolormaps[theTag];
	[myColormapView setNeedsDisplay:YES];	
}


/* textures panel */
- (int)TextureLevel
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults integerForKey:CMBview_texnumkey];	
}
- (IBAction)changeTextureLevel:(id)sender
{		
	int texnum = [texture_index intValue];
	NSString *texture_level_text = [[NSString alloc] initWithFormat:	
		@"%d",256*(int)pow(2,(float)texnum)];
	[texture_level setStringValue:texture_level_text];
	[texture_level_text release];			
	NSString *num_texels_text = [[NSString alloc] initWithFormat:
		@"%d",6*(int)SQR((float)256.0*pow(2,texnum)/(1.0+flange))];
	[num_texels setStringValue:num_texels_text];
	[num_texels_text release];	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithInt:texnum]
				 forKey:CMBview_texnumkey];
}

- (int)TexelInterpolation
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults integerForKey:CMBview_texinterpolatekey];	
}

- (IBAction)changeTexelInterpolation:(id)sender
{
	id theCell = [sender selectedCell];
	int theTag = [theCell tag];
	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithInt:theTag]
				 forKey:CMBview_texinterpolatekey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_texelinterpolationChanged" object:self];	
}

/* lighting panel */

- (BOOL)lightingEnable
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults boolForKey:CMBview_lightingenabledkey];
}
- (IBAction)changeLightingEnable:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[sender state]
											forKey:CMBview_lightingenabledkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_lightingenableChanged" object:self];	
}

- (BOOL)ambientEnable
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults boolForKey:CMBview_ambientenabledkey];
}
- (IBAction)changeAmbientEnable:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[sender state]
											forKey:CMBview_ambientenabledkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_ambientenableChanged" object:self];
}

- (BOOL)diffuseEnable
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults boolForKey:CMBview_diffuseenabledkey];
}
- (IBAction)changeDiffuseEnable:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[sender state]
											forKey:CMBview_diffuseenabledkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_diffuseenableChanged" object:self];
}

- (BOOL)specularEnable
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults boolForKey:CMBview_specularenabledkey];
}
- (IBAction)changeSpecularEnable:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setBool:[sender state]
											forKey:CMBview_specularenabledkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_specularenableChanged" object:self];
}


- (float)shininess
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults floatForKey:CMBview_shininesskey];
}

- (float)lightTheta
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults floatForKey:CMBview_lightthetakey];
}
- (float)lightPhi
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults floatForKey:CMBview_lightphikey];	
}
- (float)lightRadius
{
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	return [defaults floatForKey:CMBview_lightradiuskey];		
}

- (IBAction)rotate_lighttheta:(id)sender
{
	float thetaValue = [lighttheta floatValue];		
	NSString *light_text = [[NSString alloc] initWithFormat:@"%3.2f",thetaValue];
	[lighttheta_text setStringValue:light_text];
	[light_text release];	
				
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithFloat:thetaValue]
				 forKey:CMBview_lightthetakey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_rotatelightthetaChanged" object:self];
}
- (IBAction)rotate_lightphi:(id)sender
{
	float phiValue = [lightphi floatValue];	
	NSString *light_text = [[NSString alloc] initWithFormat:@"%3.2f",phiValue];
	[lightphi_text setStringValue:light_text];
	[light_text release];	
	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithFloat:phiValue]
				 forKey:CMBview_lightphikey];	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_rotatelightphiChanged" object:self];
}

- (IBAction)changeShininess:(id)sender
{
	float shininessValue = [shininess floatValue];	
	NSString *shine_text = [[NSString alloc] initWithFormat:@"%3.2f",shininessValue];
	[lightshininess_text setStringValue:shine_text];
	[shine_text release];	
	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithFloat:shininessValue]
				 forKey:CMBview_shininesskey];	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_shininessChanged" object:self];
}

- (IBAction)changeLightRadius:(id)sender
{
	float radiusValue = [lightradius floatValue];
	NSString *radius_text = [[NSString alloc] initWithFormat:@"%3.2f",radiusValue];
	[lightradius_text setStringValue:radius_text];
	[radius_text release];	
	
	NSUserDefaults *defaults;		
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSNumber numberWithFloat:radiusValue]
				 forKey:CMBview_lightradiuskey];	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_lightradiusChanged" object:self];
}

//light properties
- (NSColor *)ambientlightcolor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_ambientlightkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changeAmbientLightColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_ambientlightkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_ambientlightChanged" object:self];
}

- (NSColor *)diffuselightcolor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_diffuselightkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changeDiffuseLightColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_diffuselightkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_diffuselightChanged" object:self];
}

- (NSColor *)specularlightcolor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_specularlightkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changeSpecularLightColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_specularlightkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_specularlightChanged" object:self];
}


//material properties
- (NSColor *)ambientmatcolor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_ambientmatkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changeAmbientMatColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_ambientmatkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_ambientmatChanged" object:self];
}

- (NSColor *)diffusematcolor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_diffusematkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changeDiffuseMatColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_diffusematkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_diffusematChanged" object:self];
}

- (NSColor *)specularmatcolor
{
	NSUserDefaults *defaults;
	NSData *colorAsData;	
	defaults = [NSUserDefaults standardUserDefaults];
	colorAsData = [defaults objectForKey:CMBview_specularmatkey];
	return [NSKeyedUnarchiver unarchiveObjectWithData:colorAsData];				
}
- (IBAction)changeSpecularMatColor:(id)sender
{
	NSColor *color = [sender color];
	NSData *colorAsData;
	colorAsData = [NSKeyedArchiver archivedDataWithRootObject:color];
	NSUserDefaults *defaults;
	defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:colorAsData
				 forKey:CMBview_specularmatkey];
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc postNotificationName:@"CMBview_specularmatChanged" object:self];
}



@end
