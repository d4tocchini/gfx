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



#if GFX_GL || GFX_GLES
    #include "nanovg/src/nanovg_gl.h"
    #include "nanovg/src/nanovg_gl_utils.h"
#elif defined(GFX_METAL)
    #include "nanovg_metal/src/nanovg_mtl.m"
#endif
#include "nanovg/src/nanovg.c"


int _vg_grow_commands_if_needed(vg_t* ctx, int nvals) {
    if (ctx->ncommands+nvals > ctx->ccommands) {
      float* commands;
      int ccommands = ctx->ncommands+nvals + ctx->ccommands/2;
      commands = (float*)realloc(ctx->commands, sizeof(float)*ccommands);
      if (commands == NULL) return 1;
      ctx->commands = commands;
      ctx->ccommands = ccommands;
    }
    return 0;
}


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
            /* NOTE: no longer a mac issue, properly handled by SDL anyway
                see: https://github.com/floooh/sokol/pull/488
            #ifdef GFX_MAC
                // // https://github.com/godotengine/godot/commit/0ce44336866582ffa132fb440163066ad66b18c1
                // // https://github.com/godotengine/godot/tree/master/platform/osx
                // // https://community.khronos.org/t/mac-vbl/53947/4
                // // The best of both worlds : “vsync if framerate above display refresh, no vsync if below” have a look here :
                // // http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=196733#Post196733
                // // That is what most modern games seem to do, both for PC and consoles.
                // // ...
                // // Seriously, don’t do that. Use CoreVideo or CGLFlushBuffer to block until the appropriate moment.
                // // CGLCPSwapInterval broke in OSX 10.14 and it seems Apple is not interested in fixing
                // // it as OpenGL is now deprecated and Metal solves this differently.
                // // Following SDLs example we're working around this using DisplayLink
                // // When vsync is enabled we set a flag "waiting_for_vsync" to true.
                // // This flag is set to false when DisplayLink informs us our display is about to refresh.
                // CGLContextObj ctx = CGLGetCurrentContext();
                //     if (ctx) {
                //             GLint swapInterval = p_enable ? 1 : 0;
                //             CGLSetParameter(ctx, kCGLCPSwapInterval, &swapInterval);
                // ///TODO Maybe pause/unpause display link?
                GLint vblSync = 1;
                [(__bridge NSOpenGLContext*)context setValues:&vblSync forParameter:NSOpenGLCPSwapInterval];
            #endif
            */
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


//
// pathdata
//
// #include <stdbool.h>
//
#define VG_PATHDATA_HEADER_LEN 2
/*
    https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/d
    " : 34
    \ \n \t : 32,10,9
    ,       : 44
    - . / 0-9 : 45,46,47,48-57
        A-a : 65-97
*/
#define VG_D_NVALS_SHIFT        12
#define VG_D_NVALS_MASK         0b1111000000000000
#define VG_CLOSEPATH_NVALS      (1 << VG_D_NVALS_SHIFT)
#define VG_MOVETO_NVALS         (3 << VG_D_NVALS_SHIFT)
#define VG_LINETO_NVALS         (3 << VG_D_NVALS_SHIFT)
#define VG_BEZIERTO_NVALS       (7 << VG_D_NVALS_SHIFT)
//
#define VG_D_TYPE_SHIFT         16
#define _VG_D_MOVETO             ((NVG_MOVETO   << VG_D_TYPE_SHIFT) | VG_MOVETO_NVALS)
#define _VG_D_LINETO             ((NVG_LINETO   << VG_D_TYPE_SHIFT) | VG_LINETO_NVALS)
#define _VG_D_BEZIERTO           ((NVG_BEZIERTO << VG_D_TYPE_SHIFT) | VG_BEZIERTO_NVALS)
//
// TODO: #define VG_ARCTO_ARGC
//
// #define VG_D_HORIZONTAL_SHIFT   4
// #define VG_D_VERTICAL_SHIFT     5
// #define VG_D_SMOOTH_SHIFT       6
// #define VG_D_RELATIVE_SHIFT     7
#define VG_D_HORIZONTAL         0b000000010000
#define VG_D_VERTICAL           0b000000100000
#define VG_D_RELATIVE           0b000001000000
#define VG_D_CUBIC              0b000010000000
#define VG_D_QUADRATIC          0b000100000000
#define VG_D_SMOOTH             0b001000000000
//
#define VG_D_ARGC_MASK          0b0000000000001111
#define VG_D_MOVETO             (_VG_D_MOVETO   | 2)
#define VG_D_LINETO             (_VG_D_LINETO   | 2)
#define VG_D_HLINETO            (_VG_D_LINETO   | 1 | VG_D_HORIZONTAL)
#define VG_D_VLINETO            (_VG_D_LINETO   | 1 | VG_D_VERTICAL)
#define VG_D_BEZIERTO           (_VG_D_BEZIERTO | 6 | VG_D_CUBIC)
#define VG_D_SBEZIERTO          (_VG_D_BEZIERTO | 4 | VG_D_CUBIC | VG_D_SMOOTH)
#define VG_D_QUADTO             (_VG_D_BEZIERTO | 4 | VG_D_QUADRATIC)
#define VG_D_SQUADTO            (_VG_D_BEZIERTO | 2 | VG_D_QUADRATIC | VG_D_SMOOTH)
#define VG_D_CLOSE              ((NVG_CLOSE << VG_D_TYPE_SHIFT) | (1 << VG_D_NVALS_SHIFT))
#define VG_D_WINDING            ((NVG_WINDING << VG_D_TYPE_SHIFT) | (1 << VG_D_NVALS_SHIFT))
//
#define _VG_D_LOG(...)
// #define _VG_D_LOG(...)           printf(__VA_ARGS__)
#define _VG_D_LOG_COMMAND(ch)    printf("%c\n", ch + 32*(!!(comflag & VG_D_RELATIVE)))
#define _VG_D_ERROR(...) { \
        fprintf(stderr, __VA_ARGS__); \
        return 1; \
    }

int vg_pathdata(vg_t* ctx, vg_pathdata_t buf)
{
    float* tf = _vg_get_transform(ctx);
    float* argv;
    int   argc = 0;
    int   argi = 0;

    int comflag = 0;
    float x = 0.0;
    float y = 0.0;
    float cx = 0.0;
    float cy = 0.0;

    int comflag0 = 0;
    float x0 = 0.0;
    float y0 = 0.0;
    float cx0 = 0.0;
    float cy0 = 0.0;
    int len = (int)buf[0] ;
    int commands_len = (int)buf[1];
    // if (len < 0)
    //   _VG_D_ERROR("vg_pathdata invalid header[0] value: len\n")
    // _VG_D_LOG("vg_pathdata len=%i commands_len=%i\n",len,commands_len);
    int i = VG_PATHDATA_HEADER_LEN;
    int idx = 0;
    if (_vg_grow_commands_if_needed(ctx, len))
        _VG_D_ERROR("vg_pathdata _vg_grow_commands_if_needed failed to grow\n")

    float* commands = ctx->commands + ctx->ncommands;
    ctx->ncommands += len - VG_PATHDATA_HEADER_LEN;

    while (i < len) {
        comflag0 = comflag;
        x0 = x;
        y0 = y;
        cx0 = cx;
        cy0 = cy;

        argv = buf + i;
        comflag = (int)argv[0];
        argc = (comflag & VG_D_ARGC_MASK);

        int nvals = (comflag & VG_D_NVALS_MASK) >> VG_D_NVALS_SHIFT;
        float* vals = commands + idx;
        vals[0] = (float)(comflag >> VG_D_TYPE_SHIFT);
        _VG_D_LOG("  comflag=%i, i=%i, nvals=%i, argc=%i\n",comflag,i,nvals,argc);

        i += 1 + argc;
        idx += nvals;
        cx = 0.0;
        cy = 0.0;

        // CLOSE || WINDING
        if (nvals < 3)
            continue;

        bool not_horiz = 1 ^ (bool)(VG_D_HORIZONTAL & comflag);
        bool not_verti = 1 ^ (bool)(VG_D_VERTICAL & comflag);
        bool is_rel = (bool)(VG_D_RELATIVE & comflag);
        float x0_rel = x0 * is_rel;
        float y0_rel = y0 * is_rel;
        x = x0_rel + argv[argc - not_horiz] * not_verti;
        y = y0_rel + argv[argc] * not_horiz;

        vals[nvals-2] = x;
        vals[nvals-1] = y;

        _VG_D_LOG("\tx%f,y%f\n",x,y);

        // MOVETO || LINETO
        if (nvals == 3) {
            _VG_TRANSFORM_PT_AT(vals,1,tf)
            continue;
        }

        // BEZIERTO
        if (comflag & VG_D_CUBIC) {
            switch ((bool)(comflag & VG_D_SMOOTH)) {
                case false: {
                    vals[1] = x0_rel + argv[1];
                    vals[2] = y0_rel + argv[2];
                    vals[3] = x0_rel + argv[3];
                    vals[4] = y0_rel + argv[4];
                    _VG_D_LOG("\tcubic: c1%f,%f  c2%f,%f\n",argv[1],argv[2], argv[3],argv[4]);
                }   break;
                case true: {
                    bool prev_is_type = (bool)(comflag0 & VG_D_CUBIC);
                    // start control point is reflection of previous (if cubic)
                    vals[1] = x0 + (x0 - cx0) * prev_is_type;
                    vals[2] = y0 + (y0 - cy0) * prev_is_type;
                    // end control point
                    vals[3] = x0_rel + argv[1];
                    vals[4] = y0_rel + argv[2];
                }   break;
            }
            cx = vals[3];
            cy = vals[4];
            _VG_D_LOG("\tcubic: c1%f,%f  c2%f,%f\n",vals[1],vals[2], vals[3],vals[4]);

        }
        else if (comflag & VG_D_QUADRATIC) {
            switch ((bool)(comflag & VG_D_SMOOTH)) {
                case false: {
                    cx = x0_rel + argv[1];
                    cy = y0_rel + argv[2];
                }   break;
                case true: {
                    bool prev_is_type = (bool)(comflag0 & VG_D_QUADRATIC);
                    // control point is reflection of previous (if quadratic)
                    cx = x0 + (x0 - cx0) * prev_is_type;
                    cy = y0 + (y0 - cy0) * prev_is_type;
                }   break;
            }
            vals[1] = x0 + 2.0f/3.0f*(cx - x0);
            vals[2] = y0 + 2.0f/3.0f*(cy - y0);
            vals[3] = x + 2.0f/3.0f*(cx - x);
            vals[4] = y + 2.0f/3.0f*(cy - y);
        }
        _VG_TRANSFORM_PT_AT(vals,1, tf)
        _VG_TRANSFORM_PT_AT(vals,3, tf)
        _VG_TRANSFORM_PT_AT(vals,5, tf)
    }
    _VG_D_LOG("done looping...\n");
    ctx->commandx = x;
    ctx->commandy = y;
    // memcpy(&ctx->commands[ctx->ncommands], vals, nvals*sizeof(float));
    return 0;
}

int vg_pathdata_parse_buf(const char* commandstring, int len, vg_pathdata_t* out)
{
    float argv[len]; // max number
    int argi = VG_PATHDATA_HEADER_LEN; // 0: float count, 1: command count
    int command_count = 0;

    #define _VG_D_PARSE_SUCCESS { \
        argv[0] = (float)argi; \
        argv[1] = (float)command_count; \
        _VG_D_LOG("\nfloat_count=%i, command_count=%i\n",argi,command_count); \
        *out = malloc(argi*sizeof(float)); \
        if (*out == NULL) _VG_D_ERROR("ERROR vg_pathdata cannot allocate for out buffer\n"); \
        _VG_D_LOG("bufflen=%i\n",argi*sizeof(float)); \
        memcpy(*out, argv, argi*sizeof(float)); \
        _VG_D_LOG("\nfloat_count=%i, command_count=%i\n",(int)((*out)[0]),(int)((*out)[1])); \
        return 0; \
    }

    int vals_offset = 0;
    int comflag = 0;
    int numc = 0;
    // int argc = 0;
    int i = 0;
    while (i < len) {
        char ch = commandstring[i];
        while (ch <=',') { // skip whitespace
            if (i == len)
                _VG_D_PARSE_SUCCESS
            ch = commandstring[++i];
        }
        if (ch <= '9') { // extract floats
            if (numc == (comflag & VG_D_ARGC_MASK)) { // implicit command repeat
                ++command_count;
                argv[argi++] = (float)comflag;
                numc = 0;
                _VG_D_LOG("\n");
            }
            if (comflag == 0)
                _VG_D_ERROR("ERROR vg_pathdata cannot start command with number\n");
            const char* end;
            float val = strtof(commandstring + i, &end);
            i += end - (commandstring + i);

            argv[argi++] = val;
            numc++;

            _VG_D_LOG("\t%f\n", argv[argi-1]);
            continue;
        }
        if (numc != (comflag & VG_D_ARGC_MASK))
            _VG_D_ERROR("ERROR vg_pathdata param count is %i, expects %i. ch='%c'\n",numc,(comflag & VG_D_ARGC_MASK), ch);
        numc = 0;
        comflag = 0;
        if (ch >= 'a') { // lowercase == relative
            comflag |= VG_D_RELATIVE;
            ch -= 32; // force uppercase
        }
        ++command_count;
        _VG_D_LOG_COMMAND(ch);
        switch (ch) {
            /*
            IDEAS:
                o,O circle
                e,E ellipse
                r,R rect
                rrectv?
            */
            case 'M': { // moveto (dx,dy)+
                comflag |= VG_D_MOVETO;
            }   break;
            case 'L': { // lineto (dx,dy)+
                comflag |= VG_D_LINETO;
            }   break;
            case 'H': { // hlineto (dx)+
                comflag |= VG_D_HLINETO;
            }   break;
            case 'V': { // vlineto (dy)+
                comflag |= VG_D_VLINETO;
            }   break;
            case 'C': { // bezierto (dx1,dy1, dx2,dy2, dx,dy)+
                comflag |= VG_D_BEZIERTO;
            }   break;
            case 'S': { // smooth bezierto (dx2,dy2, dx,dy)+
                comflag |= VG_D_SBEZIERTO;
            }   break;
            case 'Z': { // closepath ()
                comflag |= VG_D_CLOSE;
            }   break;
            case 'Q': { // quadto (dx1,dy1, dx,dy)+
                comflag |= VG_D_QUADTO;
            }   break;
            case 'T': { // smooth quadto (dx,dy)+
                comflag |= VG_D_SQUADTO;
            }   break;
            // case 'A': { // arcto (rx,ry, angle,arcflag,sweepflag, dx,dy)+
            //     // comflag |= 7;
            //     _VG_D_ERROR("arcto command WIP");
            // }
            default: _VG_D_ERROR("ERROR vg_pathdata unrecognized command : %c\n",ch);
        }
        i++;
        _VG_D_LOG("  comflag=%i, argi=%i\n", comflag, argi);
        argv[argi++] = (float)comflag;
    }
    _VG_D_PARSE_SUCCESS
}

#endif


#endif