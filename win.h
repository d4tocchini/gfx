#ifndef _GFX_WIN_HEADER_
#define _GFX_WIN_HEADER_


typedef SDL_Window win_t;
// struct gfx_win_t {
//         void* window;
// }

#define DEFAULT_WIN_TITLE ""
#define WIN_FLOAT SDL_WINDOW_ALWAYS_ON_TOP
#define WIN_BORDERLESS SDL_WINDOW_BORDERLESS
#define WIN_RESIZEABLE SDL_WINDOW_RESIZABLE
#define WIN_FULLSCREEN SDL_WINDOW_FULLSCREEN
#define WIN_HIDE SDL_WINDOW_HIDDEN
#define WIN_SHOW SDL_WINDOW_SHOWN
#define WIN_TRANSPARENT 1 << 24
// NOTE: SHOULD BE SAFE W/ SDL WIN FLAGS

#define WINPOS_CENTER SDL_WINDOWPOS_CENTERED
#define WINPOS_NULL SDL_WINDOWPOS_UNDEFINED

void*   gfx_create_win(int flags);
void*   win_get_handle(win_t* win);
void    win_destroy(win_t* win);
void    win_did_render(win_t* win);
void    win_set_size(void* win, int w, int h);
void    win_set_pos(void* win, int x, int y);
bool    win_is_always_on_top(win_t* win, bool val);
void    win_set_transparent(win_t* win, bool val);
#define win_set_title(win, title) SDL_SetWindowTitle((SDL_Window*)(win), title)
#define win_get_size SDL_GetWindowSize
#define win_get_fbsize SDL_GL_GetDrawableSize
#define win_show(win) SDL_ShowWindow(win)
#define win_hide(win) SDL_HideWindow(win)
#define win_raise(win) SDL_RaiseWindow(win)

void*   nwin_get_layer(void* hnd);
void*   nview_get_layer(void* hnd);


// ============================================================
#ifdef __GFX_IMPLEMENTATION__
// ============================================================

#ifdef GFX_WASM
    #include <emscripten/html5.h>
    // https://emscripten.org/docs/api_reference/html5.h.html#ui
    // https://github.com/emscripten-core/emscripten/blob/master/tests/test_html5_fullscreen.c
#endif

void* gfx_create_win(int flags)
{
    flags |= SDL_WINDOW_ALLOW_HIGHDPI
        #if GFX_GL || GFX_GLES
        | SDL_WINDOW_OPENGL
        #endif
        | (WIN_HIDE * ((flags & WIN_SHOW) == 0));

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

    SDL_Window* win = SDL_CreateWindow(
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
//         SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
//         if (win == (SDL_Window*)data) {
//             printf("resizing.....\n");
//         }
//     }
//     return 0;
// }



void* win_get_handle(win_t* win)
/*
    NSView vs. NSView?
            https://developer.apple.com/swift/blog/?id=25
    When to use __bridge...
            https://stackoverflow.com/questions/20842310/arc-bridge-versus-bridge-retained-using-contextinfo-test-case
            https://medium.com/@hadhi631/when-to-use-bridge-bridge-transfer-cfbridgingrelease-and-bridge-retained-cfbridgingretain-4b3d2fc932df

*/
{
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

void win_destroy(win_t* win)
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

void win_did_render(win_t* win)
{
    #if GFX_WASM
        // TODO: is swap window needed on iOS / ES in general, seems slow w/ WASM...
    #elif GFX_GL || GFX_GLES
        SDL_GL_SwapWindow(win);
    #endif
}

void win_set_size(void* win, int w, int h) {
    SDL_SetWindowSize((SDL_Window*)win, w, h);
}

void win_max_size(void* win) {
    #if GFX_WASM
        double w = 0;
        double h = 0;
        emscripten_get_element_css_size("#html",&w, &h);
        // printf("win_max_size w=%i, h=%i",(int)w,(int)h);
        win_set_size(win, (int)w, (int)h);
    #else
        SDL_MaximizeWindow((SDL_Window*)win);
    #endif
}

void win_set_pos(void* win, int x, int y) {
    SDL_SetWindowPosition((SDL_Window*)win, x, y);
}

void win_measure() {
}

bool win_is_always_on_top(win_t* win, bool val) {
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


void win_set_transparent(win_t* win, bool val)
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


void* nwin_get_layer(void* hnd)
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

void* nview_get_layer(void* hnd)
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