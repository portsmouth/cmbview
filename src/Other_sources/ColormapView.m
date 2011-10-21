/*****************************************************************************
* Copyright 2005 Jamie Portsmouth <jamports@mac.com>                         *
*                                                                            *
* This file is part of CMBview, a program for viewing HEALPix-format         *
* CMB data on an OpenGL-rendered 3d sphere.                                  * 
*                                                                            *
* See the source files in the Classes folder for the full license preamble   *
*                                                                            *
*****************************************************************************/

#import "ColormapView.h"

@implementation ColormapView

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil) {
	}

	reverse = [[NSUserDefaults standardUserDefaults] 
                                   boolForKey:CMBview_colormapreversekey];
	
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc addObserver:self
		   selector:@selector(handle_colormapReverseChanged:)
			   name:@"CMBview_colormapReverseChanged"
			 object:nil];
		
	return self;
}

- (void)handle_colormapReverseChanged:(NSNotification *)note
{
	PreferenceController *sender = [note object];
	reverse = [sender colormapReverse];
	[self setNeedsDisplay:YES];	
}

- (void)drawRect:(NSRect)r	
{
	float color_x[3];
	
	NSColor *quadcolor;
	NSRect quad;
	NSRect viewrect = [self bounds];
	
	int Nquad = 512;
	quad.size.width = 1.1*viewrect.size.width/(float)Nquad;
	quad.size.height = viewrect.size.height;
	
	int n;
	float scalar,R,G,B;
		
	for (n=0;n<=Nquad;n++) {
		scalar = (float)n/Nquad;	
		colorpath(current_colormap_ptr->color_N,
				  current_colormap_ptr->color_xi,
				  current_colormap_ptr->color_ci,
				  scalar, color_x);		
		if (reverse) scalar = 1.0-scalar;

		HSVtoRGB(&R,&G,&B,color_x[0],color_x[1],color_x[2]);	
		quadcolor = [NSColor colorWithCalibratedRed:R
											  green:G
											   blue:B
											  alpha:1.0f];
		quad.origin.x = scalar*viewrect.size.width;
		quad.origin.y = 0.0;
		[quadcolor set];
		[NSBezierPath fillRect:quad];		
	}	
}

- (void)dealloc
{
	NSNotificationCenter *nc;
	nc = [NSNotificationCenter defaultCenter];
	[nc removeObserver:self];
	[super dealloc];
}


@end
