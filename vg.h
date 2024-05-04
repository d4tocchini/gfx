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
    #include "nanovg_metal/src/nanovg_mtl.h"
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
    // TODO: VG_AA
    #define VG_AA 1
    #define VG_STENCIL_STROKES 1
    #define vg_create_default(NIL) (vg_create(NULL, vg_flags(VG_AA, VG_STENCIL_STROKES, 0)))
    #define vg_fb_bind(fb) nvgluBindFramebuffer(fb)
    #define vg_fb_create(ctx, w, h, flags) nvgluCreateFramebuffer(ctx, w, h, flags)
    #define vg_fb_delete(fb) nvgDeleteFramebuffer(fb)
#elif defined(GFX_METAL)
    #define VG_AA 1
    #define VG_STENCIL_STROKES 0
    #define vg_create(layer, flags) nvgCreateMTL((__bridge void*)(layer), (flags))
    #define vg_create_default(layer) vg_create((layer), (vg_flags(VG_AA, VG_STENCIL_STROKES, 3)))
    #define vg_destroy(context) nvgDeleteMTL(context)
    #define vg_fb_bind(fb) mnvgBindFramebuffer(fb)
    #define vg_fb_create(ctx, w, h, flags) mnvgCreateFramebuffer(ctx, w, h, flags)
    #define vg_fb_delete(fb) mnvgDeleteFramebuffer(fb)
#endif


#define _VG_TRANSFORM_PT_AT(vals, idx, tf) \
    vals[idx]   = vals[idx]*tf[0] + vals[idx+1]*tf[2] + tf[4]; \
    vals[idx+1] = vals[idx]*tf[1] + vals[idx+1]*tf[3] + tf[5];

//
// pathdata
//
typedef float* vg_pathdata_t;
int      vg_pathdata(vg_t* ctx, vg_pathdata_t buf);
#define _vg_pathdata_parse(commandstring, out) \
         vg_pathdata_parse_buf(commandstring, strlen(commandstring), out)
#define  vg_pathdata_parse(commandstring, out) \
        _vg_pathdata_parse((commandstring),(out))
int      vg_pathdata_parse_buf(const char* commandstring, int len, vg_pathdata_t* out);


// ============================================================
#ifdef __GFX_IMPLEMENTATION__
// ============================================================
    #include "./src/vg.c"
#endif


#endif