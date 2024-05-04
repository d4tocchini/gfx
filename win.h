#ifndef _GFX_WIN_HEADER_
#define _GFX_WIN_HEADER_
/*

    https://github.com/mjansson/window_lib/blob/master/window/window_osx.m
    https://github.com/yue/yue/blob/master/nativeui/mac/window_mac.mm

*/

typedef SDL_Window win_t;

// typedef struct view_t view_t;
// // typedef struct win_t win_t;
// // https://github.com/yue/yue/blob/master/nativeui/view.h
// struct view_t {
//     uint32_t    flags;
//     uint32_t    win_id;
//     view_t**    children;
//     view_t*     parent;
// } ;


// https://github.com/yue/yue/blob/master/nativeui/window.h
// struct win_t {
//     SDL_Window*  sdl;
//     union {
//         const void* handle;
//     #if defined(GFX_MAC)
//         NSWindow*   nswin;
//     #elif defined(GFX_WINDOWS)
//         HWND        hwnd;
//     #elif defined(GFX_LINUX)
//         Window      window; // (unsigned long)
//     #endif
//     };
// } win_t;



// win create flags
#define WIN_GL              SDL_WINDOW_OPENGL
#define WIN_METAL           SDL_WINDOW_METAL
#define WIN_VULKAN          SDL_WINDOW_VULKAN
#define WIN_HIGHDPI         SDL_WINDOW_ALLOW_HIGHDPI
#define WIN_FLOAT           SDL_WINDOW_ALWAYS_ON_TOP
#define WIN_BORDERLESS      SDL_WINDOW_BORDERLESS
#define WIN_RESIZEABLE      SDL_WINDOW_RESIZABLE
#define WIN_FULLSCREEN      SDL_WINDOW_FULLSCREEN
#define WIN_MAXIMIZED       SDL_WINDOW_MAXIMIZED
#define WIN_MINIMIZED       SDL_WINDOW_MINIMIZED
#define WIN_HIDE            SDL_WINDOW_HIDDEN
#define WIN_SHOW            SDL_WINDOW_SHOWN
#define WIN_SKIP_TASKBAR    SDL_WINDOW_SKIP_TASKBAR // x11, windows
#define WIN_TRANSPARENT     (1 << 20)
    // NOTE: 20-28+ bits should be safe w/ SDL flags
    // x11 only: SDL_WINDOW_POPUP_MENU | SDL_WINDOW_UTILITY | SDL_WINDOW_TOOLTIP |

#if !defined(DEFAULT_WIN_TITLE)
    #define DEFAULT_WIN_TITLE ""
#endif

// SDL_WINDOW_FULLSCREEN: fullscreen window
// SDL_WINDOW_FULLSCREEN_DESKTOP: fullscreen window at desktop resolution

#define WINPOS_CENTER   SDL_WINDOWPOS_CENTERED
#define WINPOS_NULL     SDL_WINDOWPOS_UNDEFINED

win_t*  win_create(int flags);
// win_t*  win_recreate(win_t* win, int flags); SDL_RecreateWindow(SDL_Window * window, Uint32 flags)
win_t*  win_from_handle(const void* hnd);
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
    #include "./src/win.c"
#endif

#endif