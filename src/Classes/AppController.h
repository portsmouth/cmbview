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
#import "CMBdata.h"
#import "PreferenceController.h"

extern void my_hpic_error_handler ( int errcode, const char *file, 
							        int line, const char *msg );

@class OpenGLview;
@class LittleOpenGLview;
@class CMBdata;
@class PreferenceController;
@class ColormapView;
@class CMBviewAboutPanel;


@interface AppController : NSObject
{
	PreferenceController *preferenceController;
	CMBviewAboutPanel *cmbviewAboutPanel;
    
	// IB outlets
	IBOutlet NSSlider *Zoom_slider;
	IBOutlet NSSlider *Stokesslider; 
	
	IBOutlet NSTextField *FITSfilename;
	IBOutlet NSTextField *Numofpixels;
	IBOutlet NSTextField *progress_status_text;
	IBOutlet NSTextField *widget_max, *widget_min;
	IBOutlet NSTextField *Tval_here, *Qval_here;
	IBOutlet NSTextField *Uval_here, *Pval_here;	
	IBOutlet NSTextField *Thetaval_here, *Phival_here;	
	IBOutlet NSTextField *Nval_here;	
	IBOutlet NSTextView *drawer_text;
	
	IBOutlet NSMatrix *color_matrix;
	IBOutlet NSMatrix *linlog_matrix;
	IBOutlet NSMatrix *render_mode_matrix;
	
	IBOutlet NSButton *Tmapbutton, *Qmapbutton;	
	IBOutlet NSButton *Umapbutton, *Pmapbutton;	
	IBOutlet NSButton *showStokesbutton;	
	IBOutlet NSButton *genStokesbutton;	
	IBOutlet NSButton *updatebutton;
	IBOutlet NSButtonCell *color_matrix_HSV, *color_matrix_grey;	
	
	IBOutlet NSProgressIndicator *progressView;
	IBOutlet OpenGLview *myOpenGLview;
	IBOutlet LittleOpenGLview *myLittleOpenGLview;
	IBOutlet CMBdata *myCMBdata;
	IBOutlet NSWindow *myWindow;
	
	// GUI strings
	char GUI_errorstr[HPIC_STRNL];
	NSString *progressStatus;
	NSString *pixnumString;
	NSString *widgetmaxstring, *widgetminstring;
	NSString *myFITSfile;
	NSString *Tvalhere_string,*Qvalhere_string;
	NSString *Uvalhere_string,*Pvalhere_string;
	NSString *Thetavalhere_string,*Phivalhere_string;
	NSString *Nvalhere_string;
	NSTimer *openfiletimer;
	
	// map property flags
	int maptype,running_flag;
	int pixel,pixelordering,polarisation;
	int map_nside,Npixels,pixmin,pixmax,dpix,Nsideo; 
	
	// application flags
	int colormap_flag,linlog_flag;
	int Stokes_flag,render_mode;
	colorrange colorrange_interactive;
	colorrange colorrange_render;
	colorrange colorrange_presentation;
	
	// application state
	float thetaValue,phiValue;
	float zoomValue;	
	
	// saved image data
	NSData* myTIFF;
}

- (void)setTIFF:(NSData *)someData;
- (void)readFromFile;
- (void)GUI_error_handler:(int)errcode;

// IB actions
- (IBAction)showPreferencePanel:(id)sender;
- (IBAction)showAboutPanel:(id)sender;
- (IBAction)zoom_level:(id)sender;
- (IBAction)color_matrix:(id)sender;
- (IBAction)linlog_matrix:(id)sender;
- (IBAction)choose_render_mode:(id)sender;
- (IBAction)Tmap_select:(id)sender;
- (IBAction)Qmap_select:(id)sender;
- (IBAction)Umap_select:(id)sender;
- (IBAction)Pmap_select:(id)sender;
- (IBAction)showStokes:(id)sender;
- (IBAction)genStokes:(id)sender;
- (IBAction)Stokeslength:(id)sender;
- (IBAction)updateColors:(id)sender;
- (IBAction)saveImage:(id)sender;
- (IBAction)renderAndSave:(id)sender;
- (IBAction)readFile:(id)sender;

- (IBAction)setHistMax:(id)sender;
- (IBAction)setHistMin:(id)sender;


// accessor methods
- (int)Npixels;
- (int)maptype;
- (int)pixelordering;
- (int)polarisation;
- (int)map_nside;

// flags
- (int)colormap_flag;
- (int)linlog_flag;
- (int)Stokes_flag;
- (int)render_mode;
- (int)SetColormap_flag:(int)flag;
- (void)setStokesflag:(int)flag;

// accessor methods
- (void)setNpixels:(int)pixnum;
- (void)setwidgetmax:(NSString *)text;
- (void)setwidgetmin:(NSString *)text;
- (void)setProgressText:(NSString *)text;
- (void)setPixelnumText:(NSString *)text;
- (void)setPolarisation:(int)pol;
- (void)setPixelordering:(int)order;
- (void)setMap_nside:(int)nside;
- (void)setMaptype:(int)map_type;
- (void)set_zoom_slider:(float)zoomval;
- (void)disable_zoom_slider;
- (void)enable_zoom_slider;

// pixel info box strings
- (void)setPixinfoText_T:(NSString *)text;
- (void)setPixinfoText_Q:(NSString *)text;
- (void)setPixinfoText_U:(NSString *)text;
- (void)setPixinfoText_P:(NSString *)text;
- (void)setPixinfoText_theta:(NSString *)text;
- (void)setPixinfoText_phi:(NSString *)text;
- (void)setPixinfoText_N:(NSString *)text;

// access information about current color mappings
- (void)setColorrange_interactive:(colorrange)c;
- (void)setColorrange_render:(colorrange)c;
- (void)setColorrange_presentation:(colorrange)c;
- (colorrange *)colorrange_interactive;
- (colorrange *)colorrange_render;
- (colorrange *)colorrange_presentation;
- (NSString *)widgetminstring;
- (NSString *)widgetmaxstring;
	
// delegate method
- (NSSize)windowWillResize:(NSWindow *)sender
					toSize:(NSSize)frameSize;
- (void)setProgressIndicator:(double)x;
- (void)stopProgressBar:(BOOL)stop;

@end
