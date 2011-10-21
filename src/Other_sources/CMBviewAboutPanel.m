/*****************************************************************************
* Copyright 2005 Jamie Portsmouth <jamports@mac.com>                         *
*                                                                            *
* This file is part of CMBview, a program for viewing HEALPix-format         *
* CMB data on an OpenGL-rendered 3d sphere.                                  * 
*                                                                            *
* See the source files in the Classes folder for the full license preamble   *
*                                                                            *
*****************************************************************************/

#import "CMBviewAboutPanel.h"

@implementation CMBviewAboutPanel

- (id)init
{
	self = [super initWithWindowNibName:@"AboutPanel"];		
	return self;
}

- (void)windowDidLoad
{
	//load about panel image
	NSString *path;
	NSImage *image;
	path = [NSString stringWithFormat:@"%@/%s",
		[[NSBundle mainBundle ] resourcePath],"aboutpanel9.png"];
	
	image = [[NSImage alloc] initWithContentsOfFile:path];	
	[aboutView setImage:image];
}

@end
