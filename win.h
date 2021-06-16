#ifndef _GFX_WIN_HEADER_
#define _GFX_WIN_HEADER_
/*

    https://github.com/mjansson/window_lib/blob/master/window/window_osx.m
    https://github.com/yue/yue/blob/master/nativeui/mac/window_mac.mm

*/

typedef SDL_Window win_t;
// struct gfx_win_t {
//         void* window;
// }

#if !defined(DEFAULT_WIN_TITLE)
    #define DEFAULT_WIN_TITLE ""
#endif

// win flags
// NOTE: 24+ bits should be safe w/ SDL flags
#define WIN_FLOAT       SDL_WINDOW_ALWAYS_ON_TOP //  (X11 only, >= SDL 2.0.5)
#define WIN_BORDERLESS  SDL_WINDOW_BORDERLESS
#define WIN_RESIZEABLE  SDL_WINDOW_RESIZABLE
#define WIN_FULLSCREEN  SDL_WINDOW_FULLSCREEN
#define WIN_MAXIMIZED   SDL_WINDOW_MAXIMIZED
#define WIN_MINIMIZED   SDL_WINDOW_MINIMIZED
#define WIN_HIDE        SDL_WINDOW_HIDDEN
#define WIN_SHOW        SDL_WINDOW_SHOWN
// SDL_WINDOW_FULLSCREEN: fullscreen window
// SDL_WINDOW_FULLSCREEN_DESKTOP: fullscreen window at desktop resolution
#define WIN_TRANSPARENT 1 << 24

#define WINPOS_CENTER   SDL_WINDOWPOS_CENTERED
#define WINPOS_NULL     SDL_WINDOWPOS_UNDEFINED

win_t*  win_create(int flags);
win_t*  win_from_handle(void* hnd);
void*   win_get_handle(win_t* win);
void    win_destroy(win_t* win);
void    win_did_render(win_t* win);
void    win_set_size(void* win, int w, int h);
void    win_set_pos(void* win, int x, int y);
bool    win_is_always_on_top(win_t* win, bool val);
void    win_set_transparent(win_t* win, bool val);
#define win_set_title(win, title) SDL_SetWindowTitle((win_t*)(win), title)
#define win_get_size SDL_GetWindowSize
// void SDL_GetWindowSize(SDL_Window * window, int *w, int *h);
#define win_get_fbsize SDL_GL_GetDrawableSize
#define win_show(win)  SDL_ShowWindow(win)
#define win_hide(win)  SDL_HideWindow(win)
#define win_raise(win) SDL_RaiseWindow(win)

// int SDL_SetWindowHitTest(SDL_Window * window, SDL_HitTest callback, void *callback_data);
// https://wiki.libsdl.org/SDL_SetWindowHitTest

#define win_grab(win) SDL_SetWindowGrab(win, true)
#define win_ungrab(win) SDL_SetWindowGrab(win, false)
    // When input is grabbed the mouse is confined to the window.
    // If the caller enables a grab while another window is currently grabbed, the other window loses its grab in favor of the caller's window.
// SDL_GetWindowGrab(win)
// win_t* SDL_GetGrabbedWindow(void)
    // Returns the window if input is grabbed or NULL otherwise.

// SDL_Window * SDL_GetWindowFromID(Uint32 id);
// Uint32 SDL_GetWindowID(SDL_Window * window); // => `id` or NULL; call SDL_GetError() for more information.

// void* SDL_SetWindowData(SDL_Window * window, const char *name, void *userdata);

// int SDL_SetWindowInputFocus(SDL_Window * window);
    // You almost certainly want SDL_RaiseWindow() instead of this function. Use this with caution, as you might give focus to a window that is completely obscured by other windows.


void*   nwin_get_layer(void* hnd);
void*   nview_get_layer(void* hnd);


// ============================================================
// ============================================================
#ifdef __GFX_IMPLEMENTATION__


#ifdef GFX_WASM
    #include <emscripten/html5.h>
    // https://emscripten.org/docs/api_reference/html5.h.html#ui
    // https://github.com/emscripten-core/emscripten/blob/master/tests/test_html5_fullscreen.c
#endif

win_t*
win_create(int flags)
{
    flags |= SDL_WINDOW_ALLOW_HIGHDPI
        #if GFX_GL || GFX_GLES
        | SDL_WINDOW_OPENGL
        #elif GFX_METAL
        | SDL_WINDOW_METAL
        #endif
        // SDL_WINDOW_VULKAN
        | (WIN_HIDE * ((flags & WIN_SHOW) == 0));

    win_t* win;
    int win_w;
    int win_h;
    // #if GFX_WASM
    //     double _w,_h;
    //     emscripten_get_canvas_element_size("#canvas", &_w, &_h);
    //     win_w = (int)_w;
    //     win_h = (int)_h;
    // #else
        win_w = 512;
        win_h = 768;
    // #endif

    win = SDL_CreateWindow(
        DEFAULT_WIN_TITLE,
        WINPOS_CENTER, WINPOS_CENTER,
        win_w, win_h, flags);

    #if GFX_GL || GFX_GLES
        if (!win) {
            SDL_Log("fallback");
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
            win = SDL_CreateWindow(
                DEFAULT_WIN_TITLE,
                WINPOS_CENTER, WINPOS_CENTER,
                win_w, win_h, flags);
        }
    #endif
    if (!win)
        return NULL; // TODO:

    if (flags & WIN_TRANSPARENT)
        win_set_transparent(win, true);

    // SDL_AddEventWatch(_win_resize_watcher, win);

    return win;
}

//
// TODO: Handle resize
// see https://stackoverflow.com/questions/44507711/how-to-correctly-refresh-sdl2-window-after-resizing
//
// static int _win_resize_watcher(void* data, SDL_Event* event)
// {
//     // event watches are invoked by the thread emitting the event itself (which may be a system/OS thread). In this case, the event pump is blocked on the resizing action which doesn't return on the resize is finished. In the meantime, size change events are being queued, but they are being queued behind the current resizing operation. Adding an event watch to listen to these events effectively handles them on a different thread, giving you an opportunity to repaint. Careful with thread safety though!
//     if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
//         win_t* win = SDL_GetWindowFromID(event->window.windowID);
//         if (win == (win_t*)data) {
//             printf("resizing.....\n");
//         }
//     }
//     return 0;
// }

win_t*
win_from_handle(void* hnd)
{

}
/*

mac

ARC
    https://developpaper.com/objective-c-memory-management-2-from-mrc-to-arc/
    https://clang.llvm.org/docs/AutomaticReferenceCounting.html

    variable modifiers provided by ARC are as follows:
        __strong
        __weak
        __unsafe_unretaied
        __autoreleasing
    Methods in the alloc, copy, init, mutableCopy, and new families are implicitly marked __attribute__((ns_returns_retained)).

count the number of all subviews on a view https://developpaper.com/algorithm-count-the-number-of-all-subviews-on-a-view/

tranlsate obj-c to c
    `clang -rewrite-objc main.m`

fork to SDL_CreateWindowFrom pass NSView under mac osx
    https://github.com/flowercodec/sdl2/commit/616e6c429a037ef50ab9d0e454771a643f52e3fb

SDL2 Raising a window without giving it focus
    https://stackoverflow.com/questions/25126617/sdl2-raising-a-window-without-giving-it-focus

void*
win_get_contentview(window_t* window, unsigned int tag)
{
	// FOUNDATION_UNUSED(tag);
	return (__bridge void*)((window && window->nswindow) ?
	                        [(__bridge NSWindow*)window->nswindow contentView] : 0);
}

void
win_add_child(win_t* Window* child)
{
  [window_ addChildWindow:child->GetNative() ordered:NSWindowAbove];
}

void
win_remove_child(Window* child)
{
  [window_ removeChildWindow:child->GetNative()];
}
*/
void*
win_get_handle(win_t* win)
{
    /*
    NSView vs. NSView?
            https://developer.apple.com/swift/blog/?id=25
    When to use __bridge...
            https://stackoverflow.com/questions/20842310/arc-bridge-versus-bridge-retained-using-contextinfo-test-case
            https://medium.com/@hadhi631/when-to-use-bridge-bridge-transfer-cfbridgingrelease-and-bridge-retained-cfbridgingretain-4b3d2fc932df
    */
    SDL_SysWMinfo winfo;
    SDL_VERSION(&winfo.version); // TODO:
    if (SDL_GetWindowWMInfo(win, &winfo)) {
        #ifdef GFX_MAC
            return (__bridge void*)(winfo.info.cocoa.window);
            // printf("NSWindow %" PRIi64 "\n", nswin);
        #else
            // TODO:
            puts("win_get_handle WIP...");
            exit(1);
        #endif
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't get window information: %s", SDL_GetError());
    }
    return NULL;
}

void
win_destroy(win_t* win)
{
    #if defined(GFX_METAL)
        // TODO: update to metalview api with next SDL2 release
        SDL_Renderer* r = SDL_GetRenderer(win);
        if (r)
            SDL_DestroyRenderer(r);
    #endif
    // SDL_Metal_DestroyView(metalview);
    SDL_DestroyWindow(win);
}

void
win_did_render(win_t* win)
{
    #if GFX_WASM
        // TODO: is swap window needed on iOS / ES in general, seems slow w/ WASM...
    #elif GFX_GL || GFX_GLES
        SDL_GL_SwapWindow(win);
    #endif
}

void
win_set_size(void* win, int w, int h)
{
    SDL_SetWindowSize((win_t*)win, w, h);
}

void
win_max_size(void* win)
{
    #if GFX_WASM
        double w = 0;
        double h = 0;
        emscripten_get_element_css_size("#html",&w, &h);
        // printf("win_max_size w=%i, h=%i",(int)w,(int)h);
        win_set_size(win, (int)w, (int)h);
    #else
        SDL_MaximizeWindow((win_t*)win);
    #endif
}

void
win_set_pos(void* win, int x, int y)
{
    SDL_SetWindowPosition((win_t*)win, x, y);
}

void
win_measure()
{
}

bool
win_is_always_on_top(win_t* win, bool val)
{
    void* hnd = win_get_handle(win);
    if (!hnd)
        puts("ERROR: win_set_transparent !hnd");

    #ifdef GFX_MAC
        NSWindow* nswin = (__bridge NSWindow*)hnd;
        return [nswin level] == NSFloatingWindowLevel;
    #else
        puts("win_is_always_on_top WIP...");
        return false;
    #endif
}

void
win_set_transparent(win_t* win, bool val)
{
    void* hnd = win_get_handle(win);
    if (!hnd)
        puts("ERROR: win_set_transparent !hnd");

    #ifdef GFX_MAC
        NSWindow* nswin = (__bridge NSWindow*)hnd;
        if (val) {
            [nswin setOpaque:NO];
            [nswin setHasShadow:NO];
            [nswin setBackgroundColor:[NSColor clearColor]];
            // [nswin setBackgroundColor:[NSColor colorWithCalibratedWhite:1.0 alpha:0.0]];  //Tells the window to use a transparent colour.
        }
        else {
            // TODO: not always white win bg
            [nswin setBackgroundColor:[NSColor colorWithCalibratedWhite:1 alpha:1]];
            [nswin setOpaque:YES];
            [nswin setHasShadow:YES];
        }
    #else
        puts("win_set_transparent WIP...");
        exit(1);
    #endif
}

void*
nwin_get_layer(void* hnd)
{
    #ifdef GFX_MAC
        NSWindow* nwin = (__bridge NSWindow*)hnd;
        #ifdef GFX_METAL
            return (__bridge void*)[nwin.contentView.layer.sublayers firstObject];
        #else
            return (__bridge void*)nwin.contentView.layer;
        #endif
    #else
        // TODO:?
    #endif
}

void*
nview_get_layer(void* hnd)
{
    #ifdef GFX_MAC
        NSView* nview = (__bridge NSView*)hnd;
        #ifdef GFX_METAL
            return (__bridge void*)[nview.layer.sublayers firstObject];
        #else
            return (__bridge void*)nview.layer;
        #endif
    #else
        // TODO:?
    #endif
}

// void nwin_enable_alpha(void* window){}

// void nsgl_context_enable_alpha(void* context){}


// https://wiki.libsdl.org/SDL_WindowFlags?highlight=%28%5CbCategoryEnum%5Cb%29%7C%28SDLEnumTemplate%29
// On Apple's OS X you must set the NSHighResolutionCapable Info.plist property to YES, otherwise you will not receive a High DPI OpenGL canvas.

// // Indicates inset distances for rectangles.
// struct EdgeInsets {
//   float top;
//   float left;
//   float bottom;
//   float right;
// };

// // Indicates a specific location.
// struct Point {
//   float x;
//   float y;
// };

// // Indicates a specific size.
// struct Size {
//   float width;
//   float height;
// };


#endif

#endif