/*****************************************************************************
* Copyright 2005 Jamie Portsmouth <jamports@mac.com>                         *
*                                                                            *
* This file is part of CMBview, a program for viewing HEALPix-format         *
* CMB data on an OpenGL-rendered 3d sphere.                                  * 
*                                                                            *
* See the source files in the Classes folder for the full license preamble   *
*                                                                            *
*****************************************************************************/

#import "AboutView.h"

@implementation AboutView

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil) {
		// Add initialization code here
	}
	return self;
}

- (void)setImage:(NSImage *)someimage
{
	[someimage retain];
	[aboutimage release];
	aboutimage = someimage;
}

- (void)drawRect:(NSRect)rect
{
	NSRect imageRect;
	NSRect  drawingRect;
	imageRect.origin = NSZeroPoint;
	imageRect.size = [aboutimage size];
	drawingRect = [self bounds];
	[aboutimage drawInRect:drawingRect
				  fromRect:imageRect
				 operation:NSCompositeSourceOver
				  fraction:1.0];
	
}

@end
