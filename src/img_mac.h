
/* This function assumes that it's called from within an autorelease pool */
extern NSImage * Cocoa_CreateImage(SDL_Surface * surface);

void
Cocoa_SetWindowIcon(_THIS, SDL_Window * window, SDL_Surface * icon)
{ @autoreleasepool
{
    NSImage *nsimage = Cocoa_CreateImage(icon);

    if (nsimage) {
        [NSApp setApplicationIconImage:nsimage];
    }
}}




// NSImage resize

+ (NSImage *)resizedImage:(NSImage *)sourceImage toPixelDimensions:(NSSize)newSize
{
    if (! sourceImage.isValid) return nil;

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
              initWithBitmapDataPlanes:NULL
                            pixelsWide:newSize.width
                            pixelsHigh:newSize.height
                         bitsPerSample:8
                       samplesPerPixel:4
                              hasAlpha:YES
                              isPlanar:NO
                        colorSpaceName:NSCalibratedRGBColorSpace
                           bytesPerRow:0
                          bitsPerPixel:0];
    rep.size = newSize;

    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithBitmapImageRep:rep]];
    [sourceImage drawInRect:NSMakeRect(0, 0, newSize.width, newSize.height) fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0];
    [NSGraphicsContext restoreGraphicsState];

    NSImage *newImage = [[NSImage alloc] initWithSize:newSize];
    [newImage addRepresentation:rep];
    return newImage;
}