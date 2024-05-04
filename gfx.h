


#ifndef GFX_H_H
#define GFX_H_H

/*
TODO:
    * IOS

* Cross platform headers
    * https://github.com/bkaradzic/bgfx/blob/6b790dc96b4f8cb829e7aa65c66ce231dcc3bf05/src/renderer_gl.h
    * Visual Studio https://github.com/emscripten-ports/SDL2/blob/master/include/SDL_opengles2.h
* EGL support (Android primarily, maybe WebGL1 or Linux or Win?)
    * https://emscripten.org/docs/porting/multimedia_and_graphics/EGL-Support-in-Emscripten.html
    * https://bugzilla.libsdl.org/show_bug.cgi?id=2865
* MacOS
    * API https://git.covolunablu.org/TheExperienceArchitects/Marte/marte-engine/blob/aa9ec210b9b878f7cfe093ba82715b0a247129b3/platform/osx/os_osx.mm#L2377
    * to find headers...
        ls "$(xcrun --sdk macosx --show-sdk-path)/System/Library/Frameworks/OpenGL.framework/Headers"
*/

#define __STDC_FORMAT_MACROS
        //printf("blabla %" PRIi64 " blabla\n", var);
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef GFX_SDL
    #define GFX_SDL
#endif

#if defined(GFX_WEBGL1) || GFX_WEBGL == 1
    #define GFX_WASM 1
    #define GFX_GLES2
#elif defined(GFX_WEBGL2) || GFX_WEBGL == 2
    #define GFX_WASM 1
    #define GFX_GLES3
// #elif defined(GFX_IOS)
// #elif defined(GFX_ANDROID)
//     #define GFX_GLES3
#endif

// For a long time, MacOS X supported OpenGL only in version 3.2 and below.
// MacOS X 10.9, "Mavericks", added support for OpenGL 3.3, 4.0 and 4.1.

// #elif defined(__APPLE__)
//     #include "TargetConditionals.h"
//     #if defined(TARGET_IPHONE_SIMULATOR) && (TARGET_IPHONE_SIMULATOR != 0)
//         #define GFX_IOS_SIMULATOR 1
//     #elif defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE != 0)
//         #define GFX_IOS 1
//     #elif defined(TARGET_OS_WATCH) && (TARGET_OS_WATCH != 0)
//         #define GFX_APPLE_WATCH 1
//     #elif defined(TARGET_OS_MAC) && (TARGET_OS_MAC != 0)
//         #define GFX_MACOS 1
//     #else
//         #error Unknown Apple platform
//     #endif
// #endif

#define __GFX_IMPLEMENTATION__
#if defined(GFX_GL2) || GFX_GL == 2
    #define GFX_GL 2
    #define GFX_GL_MAJOR 2
    #define GFX_GL_MINOR 0
#elif defined(GFX_GL3) || GFX_GL == 3
    #define GFX_GL 3
    #define GFX_GL_MAJOR 3
    #define GFX_GL_MINOR 0
#elif defined(GFX_GL31) || GFX_GL == 31
    #define GFX_GL 31
    #define GFX_GL_MAJOR 3
    #define GFX_GL_MINOR 1
#elif defined(GFX_GL32) || GFX_GL == 32
    #define GFX_GL 32
    #define GFX_GL_MAJOR 3
    #define GFX_GL_MINOR 2
#elif defined(GFX_GL33) || GFX_GL == 33
    #define GFX_GL 33
    #define GFX_GL_MAJOR 3
    #define GFX_GL_MINOR 3
#elif defined(GFX_GL4) || GFX_GL == 4
    #define GFX_GL 4
    #define GFX_GL_MAJOR 4
    #define GFX_GL_MINOR 0
#elif defined(GFX_GL41) || GFX_GL == 41
    #define GFX_GL 41
    #define GFX_GL_MAJOR 4
    #define GFX_GL_MINOR 1
#elif defined(GFX_GL42) || GFX_GL == 42
    #define GFX_GL 42
    #define GFX_GL_MAJOR 4
    #define GFX_GL_MINOR 2
#elif defined(GFX_GL43) || GFX_GL == 43
    #define GFX_GL 43
    #define GFX_GL_MAJOR 4
    #define GFX_GL_MINOR 3
#elif defined(GFX_GLES2) || GFX_GLES == 2
    #define GFX_GLES 2
    #define GFX_GLES_MAJOR 2
    #define GFX_GLES_MINOR 0
#elif defined(GFX_GLES3) || GFX_GLES == 3
    #define GFX_GLES 3
    #define GFX_GLES_MAJOR 3
    #define GFX_GLES_MINOR 0
#elif defined(GFX_GLES31) || GFX_GLES == 31
    #define GFX_GLES 31
    #define GFX_GLES_MAJOR 3
    #define GFX_GLES_MINOR 1
#elif defined(GFX_METAL)
    #undef GFX_GL
#else
    #undef __GFX_IMPLEMENTATION__
#endif


// GFX_DEBUG

// Library global data
typedef struct _gfxlib_t _gfxlib_t;
struct _gfxlib_t            // _GLFWlibrary
{
    int     initialized;
    uint64_t timerfreq;
    uint64_t timer_offset;
};

#define GFX_RENDER_DELAY 8
int     gfx_init();
void    gfx_did_render();
void    gl_ctx_set_transparent(void* context, bool val);

//      Time
double  gfx_get_time(void);
void    gfx_set_time(double time);
uint64_t  gfx_get_timerval(void);
uint64_t  gfx_get_timerfreq(void);


// # define gfx_set_hint(st_name, st_val) SDL_SetHint(st_name, st_val)

#define max(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b;      \
    })

#define min(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b;      \
    })

// ============================================================
#ifndef __GFX_IMPLEMENTATION__

    // Global state shared between compilation units of GLFW
    extern _gfxlib_t _gfxlib;

// ============================================================
#else
    #include "gfx/src/gfx.c" // D4 TODO?
#endif

#endif