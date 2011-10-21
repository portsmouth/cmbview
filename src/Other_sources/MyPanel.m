/*****************************************************************************
* Copyright 2005 Jamie Portsmouth <jamports@mac.com>                         *
*                                                                            *
* This file is part of CMBview, a program for viewing HEALPix-format         *
* CMB data on an OpenGL-rendered 3d sphere.                                  * 
*                                                                            *
* See the source files in the Classes folder for the full license preamble   *
*                                                                            *
*****************************************************************************/

#import "MyPanel.h"

@implementation MyPanel

- (id)initWithContentRect:(NSRect)contentRect 
				styleMask:(unsigned int)aStyle 
				  backing:(NSBackingStoreType)bufferingType 
					defer:(BOOL)flag
{
	self = [super initWithContentRect:contentRect
							styleMask:NSTitledWindowMask
									 |NSClosableWindowMask 
		                             |NSBackingStoreBuffered
		                             |NSTexturedBackgroundWindowMask
							  backing:NSBackingStoreBuffered 
								defer:NO];
	//[self setBackgroundColor: [NSColor whiteColor]];
	//[self setAlphaValue:1.0];
	//[self setOpaque:YES];
	return self;
}

@end
