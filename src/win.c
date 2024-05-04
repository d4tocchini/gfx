// typedef enum
// {
//     SDL_WINDOW_FULLSCREEN = 0x00000001,         /**< fullscreen window */
//     SDL_WINDOW_OPENGL = 0x00000002,             /**< window usable with OpenGL context */
//     SDL_WINDOW_SHOWN = 0x00000004,              /**< window is visible */
//     SDL_WINDOW_HIDDEN = 0x00000008,             /**< window is not visible */
//     SDL_WINDOW_BORDERLESS = 0x00000010,         /**< no window decoration */
//     SDL_WINDOW_RESIZABLE = 0x00000020,          /**< window can be resized */
//     SDL_WINDOW_MINIMIZED = 0x00000040,          /**< window is minimized */
//     SDL_WINDOW_MAXIMIZED = 0x00000080,          /**< window is maximized */
//     SDL_WINDOW_INPUT_GRABBED = 0x00000100,      /**< window has grabbed input focus */
//     SDL_WINDOW_INPUT_FOCUS = 0x00000200,        /**< window has input focus */
//     SDL_WINDOW_MOUSE_FOCUS = 0x00000400,        /**< window has mouse focus */
//     SDL_WINDOW_FULLSCREEN_DESKTOP = ( SDL_WINDOW_FULLSCREEN | 0x00001000 ),
//     SDL_WINDOW_FOREIGN = 0x00000800,            /**< window not created by SDL */
//     SDL_WINDOW_ALLOW_HIGHDPI = 0x00002000,      /**< window should be created in high-DPI mode if supported.
//                                                      On macOS NSHighResolutionCapable must be set true in the
//                                                      application's Info.plist for this to have any effect. */
//     SDL_WINDOW_MOUSE_CAPTURE = 0x00004000,      /**< window has mouse captured (unrelated to INPUT_GRABBED) */
//     SDL_WINDOW_ALWAYS_ON_TOP = 0x00008000,      /**< window should always be above others */
//     SDL_WINDOW_SKIP_TASKBAR  = 0x00010000,      /**< window should not be added to the taskbar */
//     SDL_WINDOW_UTILITY       = 0x00020000,      /**< window should be treated as a utility window */
//     SDL_WINDOW_TOOLTIP       = 0x00040000,      /**< window should be treated as a tooltip */
//     SDL_WINDOW_POPUP_MENU    = 0x00080000,      /**< window should be treated as a popup menu */
//     SDL_WINDOW_VULKAN        = 0x10000000,      /**< window usable for Vulkan surface */
//     SDL_WINDOW_METAL         = 0x20000000       /**< window usable for Metal view */
// } SDL_WindowFlags;




#ifdef GFX_WASM
    #include <emscripten/html5.h>
    // https://emscripten.org/docs/api_reference/html5.h.html#ui
    // https://github.com/emscripten-core/emscripten/blob/master/tests/test_html5_fullscreen.c
#endif


win_t*
win_create(int flags)
{
    flags |= WIN_HIGHDPI | (WIN_HIDE * ((flags & WIN_SHOW) == 0))
        #if GFX_GL || GFX_GLES
            | WIN_GL
        #elif GFX_METAL
            | WIN_METAL
        #endif
        // SDL_VULKAN
    ;

    // win_t win;


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

    win_t* win = SDL_CreateWindow(DEFAULT_WIN_TITLE,
        WINPOS_CENTER, WINPOS_CENTER,
        win_w, win_h, flags);
    #if GFX_GL || GFX_GLES
        if (!win) {
            SDL_Log("fallback");
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
            win = SDL_CreateWindow(DEFAULT_WIN_TITLE,
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
win_from_handle(const void* hnd)
{
    return SDL_CreateWindowFrom(hnd);
}

void*
win_get_handle(win_t* win)
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

