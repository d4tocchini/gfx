


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

#if defined(GFX_IOS)
#endif

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

    _gfxlib_t _gfxlib = { 0 };

#if defined(GFX_MAC)
    #import <Cocoa/Cocoa.h>
    #import <Foundation/Foundation.h>
#endif


#if GFX_GL && __APPLE__
    #if GFX_GL >= 3
        // ls "$(xcrun --sdk macosx --show-sdk-path)/System/Library/Frameworks/OpenGL.framework/Headers"
        #include <OpenGL/gl3.h>
        // #include <OpenGL/gl3ext.h>
    #else
        #define GL_GLEXT_LEGACY
        #define long ptrdiff_t
        #include <OpenGL/gl.h>
        #undef long
        #undef GL_VERSION_1_2
        #undef GL_VERSION_1_3
        #undef GL_VERSION_1_4
        #undef GL_VERSION_1_5
        #undef GL_VERSION_2_0
    #endif
#elif GFX_GL >= 31
    #include <gl/glcorearb.h>
    // #ifdef __APPLE__
    //     #define GL_ARB_shader_objects // OSX collsion with GLhandleARB in gltypes.h
    // #endif
#elif GFX_GL
    #if defined(GFX_LINUX) || defined(GFX_BSD)
        #define GL_PROTOTYPES
        #define GL_GLEXT_LEGACY
        #include <GL/gl.h>
        #undef GL_PROTOTYPES
    #else
        #include <GL/gl.h>
    #endif
#elif GFX_GLES >= 3
    #ifdef GFX_IOS
        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>
    #elif GFX_WASM
        #include <GLES3/gl3platform.h>
        #include <GLES3/gl3.h>
        #include <GLES3/gl2ext.h>
    #else
        #include <GLES3/gl3platform.h>
        #include <GLES3/gl3.h>
        #include <GLES3/gl3ext.h>
    #endif
#elif GFX_GLES == 2
    #ifdef GFX_IOS
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #else
        #include <GLES2/gl2platform.h>
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
    #endif
#endif

#if defined(GFX_WASM)
    #include <emscripten.h>
#endif



#ifdef _WIN32
    #include <SDL.h>
    // #include <SDL_image.h>
    #include <SDL_opengl.h>
    #include <SDL_syswm.h>
    // #include <SDL_ttf.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_syswm.h>
    #if defined(GFX_GL)
        // #include <SDL2/SDL_image.h>
        #include <SDL2/SDL_opengl.h>
        // #include <SDL2/SDL_ttf.h>
    #elif defined(GFX_METAL)
        #import <Metal/Metal.h>
        #import <QuartzCore/CAMetalLayer.h>
    #endif
#endif

// #else /* _MSC_VER */
//     /* OpenGL ES2 headers for Visual Studio */
//     #include "SDL_opengles2_khrplatform.h"
//     #include "SDL_opengles2_gl2platform.h"
//     #include "SDL_opengles2_gl2.h"
//     #include "SDL_opengles2_gl2ext.h"
// #endif /* _MSC_VER */

void _gfx_init_time(void);

#include "./win.h"
#include "./vg.h"

int gfx_init()
{
    if (_gfxlib.initialized)
        return 0;
    memset(&_gfxlib, 0, sizeof(_gfxlib));
    _gfxlib.initialized = 1;
    _gfx_init_time();

    #ifdef _WIN32
        HINSTANCE lib = LoadLibrary("user32.dll");
        int (*SetProcessDPIAware)() = (void*) GetProcAddress(lib, "SetProcessDPIAware");
        SetProcessDPIAware();
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        // SDL_INIT_EVERYTHING
        // TODO: SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        exit(1);
    }
    SDL_EnableScreenSaver();
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    atexit(SDL_Quit);

    // https://wiki.libsdl.org/CategoryHints#Hints

    // TODO:
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1"); // By default no mouse click events are sent when clicking to focus.

    // ??? for Metal too?
    // if (SDL_GL_SetSwapInterval(-1) == -1)
    //         SDL_GL_SetSwapInterval(1);

    #ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
        SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    #endif

    // https://wiki.libsdl.org/SDL_GL_SetAttribute
    // SDL_HINT_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN

    #if defined(GFX_METAL)
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    #elif GFX_GL
        // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GFX_GL_MAJOR );
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GFX_GL_MINOR );
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); //set to 1 to require hardware acceleration, set to 0 to force software rendering; defaults to allow either

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
        //

        // SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1) // OpenGL context sharing; defaults to 0
    #elif GFX_GLES
        // https://wiki.libsdl.org/SDL_GL_SetAttribute
        // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GFX_GLES_MAJOR );
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GFX_GLES_MINOR );
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
        // SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); //set to 1 to require hardware acceleration, set to 0 to force software rendering; defaults to allow either
        // SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1) // OpenGL context sharing; defaults to 0
    // #elif defined(GFX_WASM)
        // SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT // https://wiki.libsdl.org/SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT?highlight=%28%5CbCategoryDefine%5Cb%29%7C%28CategoryHints%29
    #endif
    return 0;
}

// void sdl_render_prepare(SDL_Renderer* renderer)
// {
//     SDL_SetRenderTarget(renderer, NULL);
//     SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
// }

// void sdl_render_tex_fill(SDL_Renderer* renderer, SDL_Texture* texture, int r, int g, int b, int a)
// {
//     SDL_SetRenderTarget(renderer, texture);
//     SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
//     SDL_SetRenderDrawColor(renderer, r, g, b, a);
//     SDL_RenderFillRect(renderer, NULL);
// }

void gfx_check_err()
{
    const char* err = SDL_GetError();
    if (err && *err)
        printf("SDL ERROR: %s\n", err);
}

void gfx_did_render()
{
    // #       ifdef GFX_METAL
    // SDL_Delay(GFX_RENDER_DELAY);
    // #       endif
}

// bool color_eq(const color_t c1, const color_t c2)
// {
//     return c1.r == c2.r
//         && c1.g == c2.g
//         && c1.b == c2.b
//         && c1.a == c2.a;
// }

void gl_ctx_set_transparent(void* context, bool val)
    // http://0x1c3.blogspot.com/2017/09/sdl2-osx-opengl-transparent-background.html
{
    #if GFX_GL && defined(GFX_MAC)
        GLint opacity = val^1;
        NSOpenGLContext* nscontext = (__bridge NSOpenGLContext*)context;
        [nscontext setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];
        // }
    #endif

}


// ----------------------------------------------------------------------
//      Time

#if defined(GFX_MAC)

    #include <mach/mach_time.h>

    void _gfx_init_timer(void)
    {
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        _gfxlib.timerfreq = (info.denom * 1e9) / info.numer;
    }

    uint64_t _gfx_get_timerval(void)
    {
        return mach_absolute_time();
    }

#else

    #include <sys/time.h>
    #include <time.h>

    void _gfx_init_timer(void)
    {
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
            _gfxlib.timerfreq = 1000000000;
        else
            _gfxlib.timerfreq = 1000000;
    }

    uint64_t _gfx_get_timerval(void)
    {
        if (_gfxlib.timerfreq == 1000000000) {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return (uint64_t) ts.tv_sec * (uint64_t) 1000000000 + (uint64_t) ts.tv_nsec;
        }
        else {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return (uint64_t) tv.tv_sec * (uint64_t) 1000000 + (uint64_t) tv.tv_usec;
        }
    }

#endif

void _gfx_init_time(void)
{
    _gfxlib.timer_offset = _gfx_get_timerval();
    _gfx_init_timer();
}

double gfx_get_time(void)
{
    // _GLFW_REQUIRE_INIT_OR_RETURN(0.0);
    return (double) (_gfx_get_timerval() - _gfxlib.timer_offset) / _gfxlib.timerfreq;
}

void gfx_set_time(double time)
{
    // _GLFW_REQUIRE_INIT();
    // if (time != time || time < 0.0 || time > 18446744073.0)
    // {
    //     _glfwInputError(GLFW_INVALID_VALUE, "Invalid time %f", time);
    //     return;
    // }
    _gfxlib.timer_offset = _gfx_get_timerval() - (uint64_t) (time * _gfxlib.timerfreq);
}

uint64_t gfx_get_timerval(void)
{
    // _GLFW_REQUIRE_INIT_OR_RETURN(0);
    return _gfx_get_timerval();
}

uint64_t gfx_get_timerfreq(void)
{
    // _GLFW_REQUIRE_INIT_OR_RETURN(0);
    return _gfxlib.timerfreq;
}


// The `Widget` class represents a graphical element that can be displayed on
// the screen.
// class Widget {
//  public:
//   enum class Alignment {
//     kLeft,
//     kCenter,
//     kRight,
//     kTop,
//     kMiddle,
//     kBottom,
//   };
//   enum class Unit {
//     kPercent,
//     kPoint,
//   };

//   // The constants to alter the default way to calculate width and height of
//   // the widgets.
//   enum class BoxSizing {
//     // The width and height are measureed in consideration of only the content,
//     // but not the padding. This is the default value.
//     kContentBox,
//     // The width and height are measured in consideratino of the padding.
//     kBorderBox,
//   };

// Widget::Widget(const bool caches_rendering)

// float Widget::GetHeight() const {
//   float parent_height = parent_ == nullptr ? 0 : parent_->GetHeight();
//   if (parent_ != nullptr && box_sizing_ == BoxSizing::kBorderBox) {
//     parent_height -= (parent_->top_padding() + parent_->bottom_padding());
//   }
//   return CalculatePoints(height_unit_, height_value_, parent_height);
// }

// float Widget::GetMeasuredAlpha() {
//   float measure_alpha = alpha_;
//   Widget* parent = parent_;
//   while (parent != nullptr) {
//     measure_alpha *= parent->alpha();
//     parent = parent->parent();
//   }
//   return measure_alpha;
// }

// void Widget::RenderOnDemand(NVGcontext* context) {
//   if (caches_rendering_ && default_framebuffer_ != nullptr) {
//     nvgBeginPath(context);
//     nvgRect(context, 0, 0, GetWidth(), GetHeight());
//     nvgFillPaint(context, default_framebuffer_paint_);
//     nvgFill(context);
//   } else {
//     ExecuteRenderFunction(context);
//   }
// }

// void Widget::ExecuteRenderFunction(NVGcontext* context) {
//   // Fills the background color.
//   if (!caches_rendering_ && is_opaque_ && background_color_.a > 0) {
//     nvgBeginPath(context);
//     nvgRect(context, 0, 0, GetWidth(), GetHeight());
//     nvgFillColor(context, background_color_);
//     nvgFill(context);
//   }

//   nvgTranslate(context, rendering_offset_.x, rendering_offset_.y);
//   if (render_function_ == NULL) {
//     Render(context);
//   } else {
//     nvgScale(context, rendering_scale_, rendering_scale_);
//     render_function_(this, context);
//   }
// }

// WidgetView::WidgetView(const int context_flags)
//     : context_(nullptr), context_flags_(context_flags), is_ready_(false),
//       preparing_for_rendering_(false), requests_redraw_(false),
//       root_widget_(new Widget) {
//   root_widget_->set_widget_view(this);
// }

// void WidgetView::Render() {
//   Render(root_widget_, nullptr);
// }

// void WidgetView::Render(Widget* widget, NVGframebuffer* framebuffer) {
//   const float kWidth = widget->GetWidth();
//   const float kHeight = widget->GetHeight();
//   if (kWidth == 0 || kHeight == 0) {
//     return;
//   }

//   preparing_for_rendering_ = true;
//   NVGcontext* context = this->context();
//   visible_widgets_.clear();

//   // Determines widgets to render in order and filters invisible onces.
//   requests_redraw_ = true;
//   int count = 0;
//   const float kScreenScaleFactor = \
//       Device::GetScreenScaleFactor() * widget->GetMeasuredScale();
//   while (requests_redraw_) {
//     if (++count == 1000) {
// # ifdef DEBUG
//       printf("!! WidgetView::Render: Too many redraws.\n");
// # endif
//       break;
//     }
//     requests_redraw_ = false;
//     if (widget == root_widget_) {
//       nvgBeginFrame(context, kWidth , kHeight, kScreenScaleFactor);
//       WidgetViewWillRender(widget);
//       nvgCancelFrame(context);
//     }
//   }
//   preparing_for_rendering_ = false;
//   std::vector<WidgetItem*> widget_list;
//   PopulateWidgetList(0, widget->GetMeasuredScale(), &widget_list, widget,
//                      nullptr);

//   // Renders offscreen stuff here so it won't interfere the onscreen rendering.
//   if (framebuffer != nullptr)
//     nvgBindFramebuffer(NULL);
//   for (WidgetItem* item : widget_list) {
//     item->widget->RenderFramebuffer(context);
//     item->widget->RenderDefaultFramebuffer(context);
//   }
//   if (framebuffer != nullptr)
//     nvgBindFramebuffer(framebuffer);

//   // Clears the render buffer.
//   bool clears_color = !BackgroundIsOpaque();
// # ifdef MOUI_METAL
//   if (!clears_color) {
//     MNVGTarget target = mnvgTarget();
//     if (target == MNVG_MACOS || target == MNVG_SIMULATOR) {
//       clears_color = (widget != root_widget_ && !widget->is_opaque());
//     }
//   }
// # endif  // MOUI_METAL
//   if (clears_color) {
//     moui::nvgClearColor(context,
//                         kWidth * kScreenScaleFactor,
//                         kHeight * kScreenScaleFactor,
//                         nvgRGBAf(0, 0, 0, 0));
//   }

// # ifdef MOUI_GL
//   glViewport(0, 0, kWidth * kScreenScaleFactor, kHeight * kScreenScaleFactor);
// # endif  // MOUI_GL

//   // Renders visible widgets on screen.
//   nvgBeginFrame(context, kWidth , kHeight, kScreenScaleFactor);
//   WidgetItemStack rendering_stack;
//   for (WidgetItem* item : widget_list) {
//     PopAndFinalizeWidgetItems(item->level, &rendering_stack);
//     rendering_stack.push(item);
//     nvgSave(context);
//     nvgGlobalAlpha(context, item->alpha);
//     nvgTranslate(context, item->origin.x, item->origin.y);
//     nvgScale(context, item->widget->scale(), item->widget->scale());
//     nvgIntersectScissor(context, 0, 0, item->width, item->height);
//     item->widget->WidgetWillRender(context);
//     nvgSave(context);
//     item->widget->RenderOnDemand(context);
//     nvgRestore(context);
//   }
//   PopAndFinalizeWidgetItems(0, &rendering_stack);
//   nvgEndFrame(context);

//   // Notifies all attached widgets that the rendering process is done.
//   WidgetViewDidRender(widget);
//   is_ready_ = true;
// }


#endif

#endif