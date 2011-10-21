/*****************************************************************************
* Copyright 2005 Jamie Portsmouth <jamports@mac.com>                         *
*                                                                            *
* This file is part of CMBview, a program for viewing HEALPix-format         *
* CMB data on an OpenGL-rendered 3d sphere.                                  * 
*                                                                            *
* See the source files in the Classes folder for the full license preamble   *
*                                                                            *
*****************************************************************************/

#import "CMBview.h"

@class OpenGLview;
@class ColormapView;

//preference keys
extern NSString *CMBview_texnumkey;
extern NSString *CMBview_texinterpolatekey;
extern NSString *CMBview_backgrndcolorkey;
extern NSString *CMBview_fovykey;
extern NSString *CMBview_orthokey; 
extern NSString *CMBview_mousesensitivitykey;
extern NSString *CMBview_presentationbackgrndcolorkey;
extern NSString *CMBview_presentationtextcolorkey;
extern NSString *CMBview_exportimagesizekey;
extern NSString *CMBview_ambientlightkey;
extern NSString *CMBview_diffuselightkey;
extern NSString *CMBview_specularlightkey;
extern NSString *CMBview_ambientmatkey;
extern NSString *CMBview_diffusematkey;
extern NSString *CMBview_specularmatkey;
extern NSString *CMBview_lightingenabledkey;
extern NSString *CMBview_ambientenabledkey;
extern NSString *CMBview_diffuseenabledkey;
extern NSString *CMBview_specularenabledkey;
extern NSString *CMBview_shininesskey;
extern NSString *CMBview_lightradiuskey;
extern NSString *CMBview_lightthetakey;
extern NSString *CMBview_lightphikey;
extern NSString *CMBview_colormapchoicekey;
extern NSString *CMBview_colormapreversekey;
extern NSString *CMBview_stokesgridkey;
extern NSString *CMBview_stokespropskey; 
extern NSString *CMBview_gridspacekey;
extern NSString *CMBview_gridpropskey;

@interface PreferenceController : NSWindowController 
{
	//general panel
	IBOutlet NSColorWell *colorWell_background;
	IBOutlet NSSlider *fovy_slider;
	IBOutlet NSTextField *fovyslider_text;
	IBOutlet NSButton *orthoEnable;	
	IBOutlet NSSlider *mousesensitivity_slider;
	IBOutlet NSTextField *mousesensitivityslider_text;
	IBOutlet NSSlider *exportimagesize;
	IBOutlet NSTextField *exportimagesize_text;
	IBOutlet NSColorWell *presentationbackgrndColorWell;
	IBOutlet NSColorWell *presentationtextColorWell;

	//textures panel
	IBOutlet NSTextField *num_texels;
	IBOutlet NSTextField *texture_level;
	IBOutlet NSStepper *texture_index;
	IBOutlet NSMatrix *texelinterpolation_matrix;
	
	//lighting panel
	IBOutlet NSColorWell *colorWell_ambientlight;
	IBOutlet NSColorWell *colorWell_diffuselight;
	IBOutlet NSColorWell *colorWell_specularlight;
	IBOutlet NSColorWell *colorWell_ambientmat;
	IBOutlet NSColorWell *colorWell_diffusemat;
	IBOutlet NSColorWell *colorWell_specularmat;
	IBOutlet NSButton *lightingEnable;	
	IBOutlet NSButton *ambientEnable;	
	IBOutlet NSButton *diffuseEnable;	
	IBOutlet NSButton *specularEnable;	
	IBOutlet NSSlider *lightphi, *lighttheta;
	IBOutlet NSTextField *lightphi_text;
	IBOutlet NSTextField *lighttheta_text;
    IBOutlet NSSlider *lightradius;
	IBOutlet NSTextField *lightradius_text;
	IBOutlet NSSlider *shininess;
	IBOutlet NSTextField *lightshininess_text;
	
	//colormap panel
	IBOutlet ColormapView *myColormapView;
	IBOutlet NSMatrix *colormap_matrix;
	IBOutlet NSButton *colormapReverse;	
	
	//stokes panel 
	IBOutlet NSSlider *stokesnum;
	IBOutlet NSColorWell *stokesColorWell;
	IBOutlet NSSlider *stokesopacity;
	IBOutlet NSSlider *stokesthickness;
	IBOutlet NSSlider *stokeslength;
	IBOutlet NSTextField *stokesnum_text;
	IBOutlet NSTextField *stokesopacity_text;
	IBOutlet NSTextField *stokeslength_text;
	IBOutlet NSMatrix *stokesconvention_matrix;
	
	//grid panel
	IBOutlet NSStepper *grid_index;
	IBOutlet NSTextField *num_grid;
	IBOutlet NSColorWell *gridColorWell;
	IBOutlet NSSlider *gridopacity;
	IBOutlet NSSlider *gridthickness;
	IBOutlet NSTextField *gridopacity_text;
	IBOutlet NSButton *gridincapsEnable;
	IBOutlet NSButton *gridEnable;	
}

- (IBAction)restoreDefaults:(id)sender;

//general panel
- (IBAction)changeBackgroundColor:(id)sender;
- (NSColor *)backgrndcolor;
- (IBAction)changeFovy:(id)sender;
- (float)fovy;
- (BOOL)orthoEnable;
- (IBAction)changeOrthoEnable:(id)sender;
- (IBAction)changeMouseSensitivity:(id)sender;
- (float)MouseSensitivity;
- (IBAction)changeExportimagesize:(id)sender;
- (int)exportimagesize;
- (NSColor *)presentationbackgrndColor;
- (IBAction)changePresentationbackgrndColor:(id)sender;
- (NSColor *)presentationtextColor;
- (IBAction)changePresentationtextColor:(id)sender;

//grid panel
- (int)GridSpacing;
- (NSMutableArray *)gridProps;
- (IBAction)changeGridEnable:(id)sender;
- (IBAction)changeGridSpacing:(id)sender;
- (IBAction)changeGridOpacity:(id)sender;
- (IBAction)changeGridColor:(id)sender;
- (IBAction)changeGridThickness:(id)sender;
- (IBAction)changeGridInCaps:(id)sender;

//textures panel
- (int)TextureLevel;
- (IBAction)changeTextureLevel:(id)sender;
- (int)TexelInterpolation;
- (IBAction)changeTexelInterpolation:(id)sender;

//lighting panel
- (BOOL)lightingEnable;
- (IBAction)changeLightingEnable:(id)sender;
- (BOOL)ambientEnable;
- (IBAction)changeAmbientEnable:(id)sender;
- (BOOL)diffuseEnable;
- (IBAction)changeDiffuseEnable:(id)sender;
- (BOOL)specularEnable;
- (IBAction)changeSpecularEnable:(id)sender;
- (NSColor *)ambientlightcolor;
- (NSColor *)ambientmatcolor;
- (IBAction)changeAmbientLightColor:(id)sender;
- (IBAction)changeAmbientMatColor:(id)sender;
- (NSColor *)diffuselightcolor;
- (NSColor *)diffusematcolor;
- (IBAction)changeDiffuseLightColor:(id)sender;
- (IBAction)changeDiffuseMatColor:(id)sender;
- (NSColor *)specularlightcolor;
- (NSColor *)specularmatcolor;
- (IBAction)changeSpecularLightColor:(id)sender;
- (IBAction)changeSpecularMatColor:(id)sender;
- (float)shininess;
- (IBAction)changeShininess:(id)sender;
- (float)lightTheta;
- (float)lightPhi;
- (float)lightRadius;
- (IBAction)rotate_lightphi:(id)sender;
- (IBAction)rotate_lighttheta:(id)sender;
- (IBAction)changeLightRadius:(id)sender;

//colormap panel
- (BOOL)colormapReverse;
- (IBAction)changeColormapReverse:(id)sender;
- (IBAction)colormap_matrix:(id)sender;

//stokes panel
- (int)stokesNum;
- (NSMutableArray *)stokesProps;
- (IBAction)changeStokesNum:(id)sender;
- (IBAction)changeStokesOpacity:(id)sender;
- (IBAction)changeStokesColor:(id)sender;
- (IBAction)changeStokesThickness:(id)sender;
- (IBAction)changeStokesLength:(id)sender;
- (IBAction)changeStokesConvention:(id)sender;

@end
