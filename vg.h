#ifndef GFX_VG_H
#define GFX_VG_H


#if defined(GFX_METAL)
    #define NANOVG_MTL_IMPLEMENTATION
#elif GFX_GL_MAJOR == 2
    #define NANOVG_GL2_IMPLEMENTATION
#elif GFX_GL_MAJOR == 3
    #define NANOVG_GL3_IMPLEMENTATION
#elif GFX_GLES_MAJOR == 2
    #define NANOVG_GLES2_IMPLEMENTATION
#elif GFX_GLES_MAJOR == 3
    #define NANOVG_GLES3_IMPLEMENTATION
#endif
#include "nanovg/src/nanovg.h"

#ifdef NANOVG_MTL_IMPLEMENTATION
    #include "nanovg_mtl/src/nanovg_mtl.m"
#endif

typedef NVGcolor            color_t;

#define VG_VSYNC SDL_RENDERER_PRESENTVSYNC
#define VG_ACCEL SDL_RENDERER_ACCELERATED
#define VG_TRANSPARENT (1 << 20)
#define VG_TARGET_TEXTURE SDL_RENDERER_TARGETTEXTURE

vg_t*   win_create_vg(win_t* win, int flags);
int     vg_flags(const bool antialias, const bool stencil_strokes, const int number_of_buffers);
void    vg_clear(vg_t* ctx, const color_t clear);
void    vg_viewport(vg_t* ctx, const int x, const int y, const int w, const int h);
void    vg_read_pixels(vg_t* ctx,   // nvgReadPixels
            int img,
            int x, int y, int w, int h,
            void* data);

void    gl_ctx_set_transparent(void* context, bool val);


void    vg_img_unpremultalpha(unsigned char* image, const int width, const int height);
        #define nvgUnpremultiplyImageAlpha vg_img_unpremultalpha

void    vg_draw_shadow(vg_t* ctx,
            const float x, const float y, const float width, const float height,
            const float radius, float feather,
            const NVGcolor inner_color, const NVGcolor outer_color);

#if GFX_GL || GFX_GLES
    #if GFX_GL_MAJOR == 3
        #define NANOVG_GL3 1
        #define vg_create(NIL, flags) nvgCreateGL3(flags)
        #define vg_destroy(context) nvgDeleteGL3(context)
        #define vg_img_from_texture(ctx, textureId, w,h, flags) nvglCreateImageFromHandleGL3(ctx, textureId, w,h, flags)
    #elif GFX_GL_MAJOR == 2
        #define NANOVG_GL2 1
        #define vg_create(NIL, flags) nvgCreateGL2(flags)
        #define vg_destroy(context) nvgDeleteGL2(context)
        #define vg_img_from_texture(ctx, textureId, w,h, flags) nvglCreateImageFromHandleGL2(ctx, textureId, w,h, flags)
    #elif GFX_GLES_MAJOR == 3
        #define NANOVG_GLES3 1
        #define vg_create(NIL, flags) nvgCreateGLES3(flags)
        #define vg_destroy(context) nvgDeleteGLES3(context)
    #elif GFX_GLES_MAJOR == 2
        #define NANOVG_GLES2 1
        #define vg_create(NIL, flags) nvgCreateGLES2(flags)
        #define vg_destroy(context) nvgDeleteGLES2(context)
    #endif
    #define vg_create_default(NIL) (vg_create(NULL, vg_flags(false, true, 0)))
    #define vg_fb_bind(fb) nvgluBindFramebuffer(fb)
    #define vg_fb_create(ctx, w, h, flags) nvgluCreateFramebuffer(ctx, w, h, flags)
    #define vg_fb_delete(fb) nvgDeleteFramebuffer(fb)
#elif defined(GFX_METAL)
    #define vg_create(layer, flags) nvgCreateMTL((__bridge void*)(layer), (flags))
    #define vg_create_default(layer) vg_create((layer), (vg_flags(true, false, 3)))
    #define vg_destroy(context) nvgDeleteMTL(context)
    #define vg_fb_bind(fb) mnvgBindFramebuffer(fb)
    #define vg_fb_create(ctx, w, h, flags) mnvgCreateFramebuffer(ctx, w, h, flags)
    #define vg_fb_delete(fb) mnvgDeleteFramebuffer(fb)
#endif




// ============================================================
#ifdef __GFX_IMPLEMENTATION__
// ============================================================



#if GFX_GL || GFX_GLES
    #include "nanovg/src/nanovg_gl.h"
    #include "nanovg/src/nanovg_gl_utils.h"
#elif defined(GFX_METAL)
    #include "nanovg_mtl/src/nanovg_mtl.h"
#endif
#include "nanovg/src/nanovg.c"


// struct NVGLUframebuffer {
// 	NVGcontext* ctx;
// 	GLuint fbo;
// 	GLuint rbo;
// 	GLuint texture;
// 	int image;
// };
#if GFX_GL || GFX_GLES
    typedef NVGLUframebuffer    vg_fb_t;
#elif defined(GFX_METAL)
    typedef MNVGframebuffer     vg_fb_t;
#endif



// void vg_did_render(vg_t* vg)
// {
// }

int vg_flags(const bool antialias, const bool stencil_strokes, const int number_of_buffers)
// nvgContextFlags
{
    int flags = 0;
    if (antialias)
        flags |= NVG_ANTIALIAS;
    if (stencil_strokes)
        flags |= NVG_STENCIL_STROKES;
    #if defined(GFX_METAL)
        if (number_of_buffers >= 3)
            flags |= NVG_TRIPLE_BUFFER;
        else if (number_of_buffers == 2)
            flags |= NVG_DOUBLE_BUFFER;
    #endif
    return flags; //| NVG_DEBUG;
}

vg_t* win_create_vg(win_t* win, int flags)
{
    vg_t* vg = NULL;

    #if defined(GFX_METAL)

        // SDL_MetalView metalview = SDL_Metal_CreateView(win);
        // CAMetalLayer* layer = (__bridge CAMetalLayer*)SDL_Metal_GetLayer(metalview);

        SDL_Renderer* renderer = SDL_CreateRenderer(
            win, -1,
            flags
            // SDL_RENDERER_PRESENTVSYNC // mtllayer.displaySyncEnabled
            // |SDL_RENDERER_ACCELERATED // ???
        );
        const CAMetalLayer* layer = (__bridge CAMetalLayer*)SDL_RenderGetMetalLayer(renderer);

        vg = vg_create_default(layer);

        layer.displaySyncEnabled = (bool)(flags & VG_VSYNC);

        if (flags & VG_TRANSPARENT) {
            layer.opaque = NO;
            layer.backgroundColor = [NSColor clearColor].CGColor;
            // (
            // SDL_Surface *screen;
            // screen = SDL_GetWindowSurface(win);
            // SDL_SetSurfaceAlphaMod(screen, 0);
            // SDL_SetSurfaceBlendMode(screen,SDL_BLENDMODE_BLEND);

            // int width = 720;
            // int height = 720;
            // SDL_Texture *rtex = SDL_CreateTexture(renderer,
            //         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
            // SDL_SetTextureBlendMode(rtex, SDL_BLENDMODE_BLEND);

            // SDL_Texture *gtex = SDL_CreateTexture(renderer,
            //         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
            // SDL_SetTextureBlendMode(gtex, SDL_BLENDMODE_BLEND);

            // SDL_Texture *btex = SDL_CreateTexture(renderer,
            //         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
            // SDL_SetTextureBlendMode(btex, SDL_BLENDMODE_BLEND);

            // sdl_render_tex_fill(renderer, rtex, 255, 0, 0, 0);
            // sdl_render_tex_fill(renderer, gtex, 0, 255, 0, 20);
            // sdl_render_tex_fill(renderer, btex, 255, 0, 255, 50);

            // sdl_render_prepare(renderer);

            // SDL_Rect rect;
            // rect.w = width;
            // rect.h = height;
            // // SDL_RenderClear(renderer);
            // SDL_RenderFillRect(renderer, NULL);
            // rect.x = 50;
            // rect.y = 50;
            // SDL_RenderCopy(renderer, rtex, NULL, &rect);
            // rect.x = 75;
            // rect.y = 70;
            // SDL_RenderCopy(renderer, gtex, NULL, &rect);
            // rect.x = 75;
            // rect.y = 30;
            // SDL_RenderCopy(renderer, btex, NULL, &rect);

            // SDL_RenderPresent(renderer);

            /* Clear the entire screen to our selected color. */
            // SDL_RenderClear(renderer);
            // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
            // SDL_RenderFillRect(renderer, NULL);

            /* Up until now everything was drawn behind the scenes.
                                    This will show the new, red contents of the window. */
            // SDL_RenderPresent(renderer);
        }
        //  for (CALayer* layer in self.layer.sublayers) {

    #else
        SDL_GLContext context = SDL_GL_CreateContext(win);
        if (context == NULL) {
            printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
            exit(1);
        }

        SDL_GL_MakeCurrent(win, context);

        if (flags & VG_VSYNC) {
            #ifdef GFX_MAC
                // CGLCPSwapInterval broke in OSX 10.14 and it seems Apple is not interested in fixing
                // it as OpenGL is now deprecated and Metal solves this differently.
                // Following SDLs example we're working around this using DisplayLink
                // When vsync is enabled we set a flag "waiting_for_vsync" to true.
                // This flag is set to false when DisplayLink informs us our display is about to refresh.
                /*	CGLContextObj ctx = CGLGetCurrentContext();
                                if (ctx) {
                                        GLint swapInterval = p_enable ? 1 : 0;
                                        CGLSetParameter(ctx, kCGLCPSwapInterval, &swapInterval);
                                }*/
                ///TODO Maybe pause/unpause display link?

                GLint vblSync = 1;
                [(__bridge NSOpenGLContext*)context setValues:&vblSync forParameter:NSOpenGLCPSwapInterval];

                // }
            #endif


            if (SDL_GL_SetSwapInterval(-1) == -1)
                if (SDL_GL_SetSwapInterval(1) == -1)
                    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
        } else
            SDL_GL_SetSwapInterval(0);

        if (flags & VG_TRANSPARENT) {
            glClearColor(0.f, 0.f, 0.f, 0.f);
            gl_ctx_set_transparent(context, true);
        }
        vg = vg_create_default();
    #endif
    if (vg == NULL) {
        printf("win_create_vg could not be created! SDL Error: %s\n", SDL_GetError());
    }
    return vg;
}

void vg_viewport(vg_t* ctx, const int x, const int y, const int w, const int h)
{
    #if GFX_GL || GFX_GLES
        glViewport(x, y, w, h);
        // rasterizer can render pixels outside viewport, hence the scisscor
        // glScissor() & enabling GL_SCISSOR_TEST (disabled by default) restricts the clear.
        // glScissor(x - 2, y - 2, w + 2, h + 2);
        glScissor(x, y, w, h);
        glEnable(GL_SCISSOR_TEST);
    #elif defined(GFX_METAL)
        // TODO: ???
    #endif
}

void vg_clear(vg_t* ctx, const color_t clear)
{
    #if GFX_GL || GFX_GLES
        const float ka = clear.a;
        const float kr = clear.r * ka;
        const float kg = clear.g * ka;
        const float kb = clear.b * ka;
        glClearColor(kr, kg, kb, ka);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    #elif defined(GFX_METAL)
        mnvgClearWithColor(ctx, clear);
    #endif
}


void vg_draw_shadow(vg_t* ctx,
    const float x, const float y, const float width, const float height,
    const float radius, float feather,
    const NVGcolor inner_color, const NVGcolor outer_color)
{
    const NVGpaint kShadowPaint = nvgBoxGradient(ctx,
        x, y, width, height,
        radius, feather,
        inner_color, outer_color);
    nvgBeginPath(ctx);
    nvgRect(ctx,
        x - feather, y - feather,
        width + feather * 2, height + feather * 2);
    nvgFillPaint(ctx, kShadowPaint);
    nvgFill(ctx);
}

void vg_read_pixels(vg_t* ctx,   // nvgReadPixels
    int image,
    int x, int y, int width, int height,
    void* data)
{
    #if GFX_GL || GFX_GLES
        glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    #elif defined(GFX_METAL)
        mnvgReadPixels(ctx, image, x, y, width, height, data);
    #endif
}


void vg_img_unpremultalpha(unsigned char* image, const int width, const int height)
//  nvgUnpremultiplyImageAlpha
{
    const int kStride = width * 4;

    // Unpremultiply.
    for (int y = 0; y < height; y++) {
        unsigned char* row = &image[y * kStride];
        for (int x = 0; x < width; x++) {
            const int kRed = row[0],
                      kGreen = row[1],
                      kBlue = row[2],
                      kAlpha = row[3];
            if (kAlpha != 0) {
                row[0] = min(kRed * 255 / kAlpha, 255);
                row[1] = min(kGreen * 255 / kAlpha, 255);
                row[2] = min(kBlue * 255 / kAlpha, 255);
            }
            row += 4;
        }
    }

    // Defringe.
    for (int y = 0; y < height; y++) {
        unsigned char* row = &image[y * kStride];
        for (int x = 0; x < width; x++) {
            const int kAlpha = row[3];
            int red = 0, green = 0, blue = 0, n = 0;
            if (kAlpha == 0) {
                if (x - 1 > 0 && row[-1] != 0) {
                    red += row[-4];
                    green += row[-3];
                    blue += row[-2];
                    n++;
                }
                if (x + 1 < width && row[7] != 0) {
                    red += row[4];
                    green += row[5];
                    blue += row[6];
                    n++;
                }
                if (y - 1 > 0 && row[-kStride + 3] != 0) {
                    red += row[-kStride];
                    green += row[-kStride + 1];
                    blue += row[-kStride + 2];
                    n++;
                }
                if (y + 1 < height && row[kStride + 3] != 0) {
                    red += row[kStride];
                    green += row[kStride + 1];
                    blue += row[kStride + 2];
                    n++;
                }
                if (n > 0) {
                    row[0] = red / n;
                    row[1] = green / n;
                    row[2] = blue / n;
                }
            }
            row += 4;
        }
    }
}




#endif


#endif