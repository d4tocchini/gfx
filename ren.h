#ifndef GFX_REN_H
#define GFX_REN_H


#define GFX_REN

#define REN_DEFAULT_FONT_SIZE 11.8f

#if defined(REN_MICROUI)
    #include "microui/src/microui.h"
    typedef mu_Id ren_id_t;
    typedef mu_Vec2 ren_vec2_t;
    typedef mu_Rect ren_rect_t;
    typedef mu_Color ren_color_t;
    typedef mu_Font ren_font_t;
#else
    typedef unsigned ren_id_t;
    typedef struct { int x, y; } ren_vec2_t;
    typedef struct { int x, y, w, h; } ren_rect_t;
    typedef struct { unsigned char r, g, b, a; } ren_color_t;
    typedef void* ren_font_t;
#endif
typedef struct ren_ctx_t {
    #if defined(REN_MICROUI)
        mu_Context mu;
    #endif
    win_t* win;
    vg_t* vg;
    // int fonts[8];
    int win_w, win_h;
    int fb_w, fb_h;
    int mouse_x, mouse_y;
    int mouse_dx, mouse_dy; unsigned int mouse_t0;
    int wheel_dx, wheel_dy; unsigned int wheel_t0;
    float px_ratio;
} ren_ctx_t;

static ren_ctx_t ctx;
enum {
  REN_KEY_SHIFT        = (1 << 0),
  REN_KEY_CTRL         = (1 << 1),
  REN_KEY_ALT          = (1 << 2),
  REN_KEY_BACKSPACE    = (1 << 3),
  REN_KEY_RETURN       = (1 << 4)
};
enum {
  REN_MOUSE_LEFT       = (1 << 0),
  REN_MOUSE_RIGHT      = (1 << 1),
  REN_MOUSE_MIDDLE     = (1 << 2)
};

/*

TODO: Handle quit in emscripten properly

[emscripten_set_main_loop](https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop)
[emscripten_sleep](https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_sleep)
* NOTE: emscripten_set_main_loop will not return into calling stack
    * Asyncify is a solution, but that comes with a perf price tag
        * https://kripken.github.io/blog/wasm/2019/07/16/asyncify.html
        * https://github.com/emscripten-core/emscripten/wiki/Emterpreter#emterpreter-async-run-synchronous-code
    * see https://github.com/emscripten-core/emscripten/issues/6227

#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#   define GFX_DELAY(x) emscripten_sleep(x)
#else
#   include <SDL2/SDL_timer.h>
#   define GFX_DELAY(x) SDL_Delay(x)
#endif

*/
static int REN_QUIT = 0;
static int REN_QUIT_FLAG = (1<<16);
static int REN_FRAME_MS = 16;

#if GFX_WASM_ASYNCIFY

    #define REN_LOOP(loop_fn)   while (!REN_QUIT) {loop_fn();}
    #define REN_SLEEP(ms)       emscripten_sleep(ms);

#elif GFX_WASM

    #define REN_LOOP(loop_fn)   emscripten_set_main_loop(loop_fn, 0, 1);
    #define REN_SLEEP(ms)       // NOTE: sleep not supported w/o asyncify

#else

    #define REN_LOOP(loop_fn)   while (!REN_QUIT) {loop_fn();}
    #define REN_SLEEP(ms)       SDL_Delay(ms);

#endif

#define REN_DELAY_FRAME()   REN_SLEEP(REN_FRAME_MS);

void ren_init(void);
ren_ctx_t* ren_init_ctx(void);
int ren_INPUT(void);
void ren_BEGIN(void);
void ren_DRAW(void);
void ren_END(void);
void ren_measure_frame(void);
void ren_get_size(int* w, int*h);
void ren_draw_rect(ren_rect_t rect, ren_color_t color);
void ren_draw_text(const char* text, ren_vec2_t pos, ren_color_t color);
void ren_draw_icon(int id, ren_rect_t rect, ren_color_t color);
int ren_get_text_width(const char* text, int len);
float ren_get_text_height(void);
void ren_set_clip_rect(ren_rect_t rect);
void ren_clear(ren_color_t color);
void ren_present(void);
void ren_invalidate(void);
void ren_destroy(void);

int ren_install_font(const char* name, const char* file);
void ren_install_font_defaults(void);

static const char REN_BTN_MAP[256] = {
    [SDL_BUTTON_LEFT & 0xff] = REN_MOUSE_LEFT,
    [SDL_BUTTON_RIGHT & 0xff] = REN_MOUSE_RIGHT,
    [SDL_BUTTON_MIDDLE & 0xff] = REN_MOUSE_MIDDLE,
};

static const char REN_KEY_MAP[256] = {
    [SDLK_LSHIFT & 0xff] = REN_KEY_SHIFT,
    [SDLK_RSHIFT & 0xff] = REN_KEY_SHIFT,
    [SDLK_LCTRL & 0xff] = REN_KEY_CTRL,
    [SDLK_RCTRL & 0xff] = REN_KEY_CTRL,
    [SDLK_LALT & 0xff] = REN_KEY_ALT,
    [SDLK_RALT & 0xff] = REN_KEY_ALT,
    [SDLK_RETURN & 0xff] = REN_KEY_RETURN,
    [SDLK_BACKSPACE & 0xff] = REN_KEY_BACKSPACE,
};

int ren_onmouseup(int x, int y, int btn)
{
    int b = REN_BTN_MAP[btn & 0xff];
    if (!b)
        return 0;
    #if defined(REN_MICROUI)
        mu_input_mouseup(&ctx, x, y, b);
    #endif
    return 1;
}

int ren_onmousedown(int x, int y, int btn)
{
    int b = REN_BTN_MAP[btn & 0xff];
    if (!b)
        return 0;
    #if defined(REN_MICROUI)
        mu_input_mousedown(&ctx, x, y, b);
    #endif
    return 1;
}

int ren_onkeyup(int key)
{
    int k = REN_KEY_MAP[key & 0xff];
    if (!k)
        return 0;
    #if defined(REN_MICROUI)
        mu_input_keyup(&ctx, k);
    #endif
    return 1;
}

int ren_onkeydown(int key)
{
    int k = REN_KEY_MAP[key & 0xff];
    if (!k)
        return 0;
    #if defined(REN_MICROUI)
        mu_input_keydown(&ctx, k);
    #endif
    return 1;
}

int ren_onmousemove(int x, int y, int dx, int dy, unsigned int timestamp)
{
    ctx.mouse_x = x;
    ctx.mouse_y = y;
    ctx.mouse_dx = dx;
    ctx.mouse_dy = dy;
    ctx.mouse_t0 = timestamp;
    #if defined(REN_MICROUI)
        mu_input_mousemove(&ctx, x, y);
    #endif
    return 1;
}

int ren_onwheel(int wheel_dx, int wheel_dy, unsigned int timestamp)
{
    wheel_dy *= -1;
    ctx.wheel_dx = wheel_dx;
    ctx.wheel_dy = wheel_dy;
    ctx.wheel_t0 = timestamp;
    // printf("wheel_dy = %i\n",wheel_dy);
    #if defined(REN_MICROUI)
        mu_input_scroll(&ctx, wheel_dx, wheel_dy << 4);
    #endif
    return 1;
}

int ren_ontextinput(const char *text)
{
    #if defined(REN_MICROUI)
        mu_input_text(&ctx, text);
    #endif
    return 1;
}

int ren_onresize(void)
{
    ren_measure_frame();
    return 1;
}
//

// coloren_t clear = {0.f,0.f,0.f,0.f};
// } ren_ctx_t;
// static ren_ctx_t ren_ctx;

// #define GLUE_HELPER(x, y) x##y
// #define GLUE(x, y) GLUE_HELPER(x, y)
// #define WIN ren_ctx.win
// #define VG ren_ctx.vg

// #define WIN_(fn_name, ...) GLUE(win_,fname)(WIN, __VA_ARGS__)
// WIN_(create_vg, VG_VSYNC|VG_ACCEL);

#if defined(REN_MICROUI)
    static int _mu_get_text_width(ren_font_t font, const char* text, int len) {
        return ren_get_text_width(text, len);
        // int l = 0;
        // const char *p = text;
        // for (; *p && len--; p++) {
        //     if ((*p & 0xc0) == 0x80) {
        //         if (l)
        //             w += ren_get_text_width(p-l, l);
        //         l = 0;
        //         continue;
        //     }
        //     l++;
        // }
        // if (l)
        //     w += ren_get_text_width(p-l, l);
        // puts("--------");
        // return w;
    }
    static int _mu_get_text_height(ren_font_t font) {
        return ren_get_text_height();
    }
#endif

ren_ctx_t* ren_init_ctx(void)
{
    ren_init();
    return &ctx;
}

void ren_init(void)
{
    gfx_init();
    ctx.win = gfx_create_win(WIN_RESIZEABLE
        // |WIN_TRANSPARENT
        );
    if (!(ctx.win)) {
        puts("Failed to create win");
        // return void;
        exit(1);
    }
    ctx.vg = win_create_vg(ctx.win, VG_VSYNC | VG_ACCEL
        // |VG_TRANSPARENT
        );
    // win_set_size(ctx.win, 720, 720);
    win_set_pos(ctx.win, WINPOS_CENTER, WINPOS_CENTER);
    win_show(ctx.win);
    // ctx = malloc(sizeof(ren_ctx_t));

    #if defined(REN_MICROUI)
        mu_init(&ctx.mu);
        ctx.mu.text_width = _mu_get_text_width;
        ctx.mu.text_height =_mu_get_text_height;
    #endif

    ren_measure_frame();
    // return ctx;
}

void ren_get_size(int * w, int * h)
{
    win_get_size(ctx.win, w, h);
}

void ren_measure_frame(void)
{
    win_get_size(ctx.win, &(ctx.win_w), &(ctx.win_h));
    win_get_fbsize(ctx.win, &(ctx.fb_w), &(ctx.fb_h));
    ctx.px_ratio = (float)(ctx.fb_w) / (float)(ctx.win_w);
}

int ren_INPUT(void)
{
    // SDL_WINDOW_MOUSE_CAPTURE
    // SDL_CaptureMouse(): https://hg.libsdl.org/SDL/file/1b1cdbe1cad5/include/SDL_mouse.h#l164

    // https://github.com/electron/electron/blob/master/shell/browser/native_window.h

    // SDL_AddEventWatch https://wiki.libsdl.org/SDL_AddEventWatch
    // SDL_SetEventFilter https://wiki.libsdl.org/SDL_SetEventFilter#Remarks

    int DIRTY = 0;
    ctx.mouse_dx = 0;
    ctx.mouse_dy = 0;
    ctx.wheel_dx = 0;
    ctx.wheel_dy = 0;
    static SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT: {
                exit(EXIT_SUCCESS);
                // return REN_QUIT_FLAG;
            }   break;
            case SDL_MOUSEMOTION: {
                SDL_MouseMotionEvent motion = e.motion;
                DIRTY|=ren_onmousemove(
                    motion.x, motion.y,
                    motion.xrel, motion.yrel,
                    motion.timestamp);
                break;
            }
            case SDL_MOUSEWHEEL:
                DIRTY|=ren_onwheel(e.wheel.x, e.wheel.y, e.wheel.timestamp);
                break;
            // case SDL_TouchFingerEvent: // https://wiki.libsdl.org/SDL_TouchFingerEvent
            case SDL_TEXTINPUT:
                DIRTY|=ren_ontextinput(e.text.text);
                break;
            case SDL_MOUSEBUTTONDOWN:
                DIRTY|=ren_onmousedown(e.button.x, e.button.y, e.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                DIRTY|=ren_onmouseup(e.button.x, e.button.y, e.button.button);
                break;
            case SDL_KEYDOWN:
                DIRTY|=ren_onkeydown(e.key.keysym.sym);
                break;
            case SDL_KEYUP:
                DIRTY|=ren_onkeyup(e.key.keysym.sym);
                break;
            case SDL_WINDOWEVENT: {
                // https://wiki.libsdl.org/SDL_WindowEvent?highlight=%28%5CbCategoryStruct%5Cb%29%7C%28CategoryEvents%29
                // https://wiki.libsdl.org/SDL_WindowEventID
                switch (e.window.event) {
                    // case SDL_WINDOWEVENT_SHOWN:
                    // case SDL_WINDOWEVENT_HIDDEN:
                    // case SDL_WINDOWEVENT_EXPOSED:
                        //  SDL_Log("Window %d shown", event->window.windowID);

                    // case SDL_WINDOWEVENT_MOVED:
                    // case SDL_WINDOWEVENT_RESIZED:
                    // case SDL_WINDOWEVENT_SIZE_CHANGED: // window size has changed, either as a result of an API call or through the system or user changing the window size; this event is followed by SDL_WINDOWEVENT_RESIZED if the size was changed by an external event, i.e. the user or the window manager
                        // SDL_Log("Window %d moved to %d,%d", event->window.windowID, event->window.data1, event->window.data2);
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        DIRTY|=ren_onresize();
                        break;
                        // SDL_RenderClear(r);
                        // SDL_RenderCopy(r, t, 0, 0);
                        // SDL_RenderPresent(r);

                    // case SDL_WINDOWEVENT_MINIMIZED:
                    // case SDL_WINDOWEVENT_MAXIMIZED:
                    // case SDL_WINDOWEVENT_RESTORED: // window has been restored to normal size and position
                    // case SDL_WINDOWEVENT_ENTER:   // window has gained mouse focus
                    // case SDL_WINDOWEVENT_LEAVE:   // window has lost mouse focus
                    // case SDL_WINDOWEVENT_FOCUS_GAINED: // window has gained keyboard focus
                    // case SDL_WINDOWEVENT_FOCUS_LOST:  // window has lost keyboard focus
                    // case SDL_WINDOWEVENT_CLOSE:   // the window manager requests that the window be closed
                    // case SDL_WINDOWEVENT_TAKE_FOCUS: // window is being offered a focus (should SDL_SetWindowInputFocus() on itself or a subwindow, or ignore) (>= SDL 2.0.5)
                }
            }
        }
    }
    return DIRTY;

    //TODO:
    // nvgFontFace(ctx.vg, "sans");
    // nvgFontSize(ctx.vg, REN_DEFAULT_FONT_SIZE);
}

void ren_BEGIN(void)
{
    vg_t* vg = ctx.vg;
    vg_viewport(ctx.vg, 0, 0, ctx.fb_w, ctx.fb_h);
    ren_clear((ren_color_t) { .r = 0, .g = 0, .b = 0, .a = 0 });
    vg_frame_begin(vg, ctx.win_w, ctx.win_h, ctx.px_ratio);

    vg_font_size(ctx.vg, REN_DEFAULT_FONT_SIZE);
    // mu_begin(&ctx);
}

void ren_DRAW(void)
{
    // ren_clear(mu_color(bg[0], bg[1], bg[2], 255));
    #if defined(REN_MICROUI)
        mu_Command *cmd = NULL;
        while (mu_next_command(&ctx.mu, &cmd)) {

            switch (cmd->type) {
                case MU_COMMAND_TEXT: {
                    // nvgFontSize(vg, 12.0f);
                    nvgFontFace(ctx.vg, "sans");
                    ren_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color);
                    break;
                }
                case MU_COMMAND_RECT: ren_draw_rect(cmd->rect.rect, cmd->rect.color); break;
                case MU_COMMAND_ICON: ren_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
                case MU_COMMAND_CLIP: ren_set_clip_rect(cmd->clip.rect); break;
            }
        }
    #endif
}

void ren_END(void)
{
    vg_frame_end(ctx.vg);
    // ren_present();
    // mu_end(&ctx);
}

void ren_draw_rect(ren_rect_t rect, ren_color_t color)
{
    vg_t* vg = ctx.vg;
    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
    nvgRect(vg, rect.x, rect.y, rect.w, rect.h);
    nvgFill(vg);
}

void ren_draw_text(const char* text, ren_vec2_t pos, ren_color_t color)
{
    int l = strlen(text);
    char last = (*(text + l - 1));
    l -= (bool)( 0 < last && last < 32 );
    if (l == 0)
        return;

    vg_t* vg = ctx.vg;
    nvgFillColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));

    nvgTextAlign(vg, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
    // nvgTextBox(vg, pos.x, pos.y);

    nvgText(vg, pos.x, pos.y, text, text+l);

}



// https://github.com/d4tocchini/entypo
// http://fontello.github.io/entypo/demo.html
// http://fontello.com/
#define ICON_SEARCH 0x1F50D
#define ICON_CIRCLED_CROSS 0x2716
#define ICON_CLOSE 0x2715
#define ICON_CHEVRON_RIGHT 0xE75E
#define ICON_CHEVRON_DOWN 0xE75C
#define ICON_CHECK 0x2713
#define ICON_LOGIN 0xE740
#define ICON_TRASH 0xE729
#define ICON_RESIZE_FULL 0xE744
static const int REN_ICON_LUT[6]= {
        0,
        ICON_CLOSE,         // MU_ICON_CLOSE = 1,
        ICON_CHECK,         // MU_ICON_CHECK,
        ICON_CHEVRON_RIGHT, // MU_ICON_COLLAPSED,
        ICON_CHEVRON_DOWN, // MU_ICON_EXPANDED,
        ICON_CHEVRON_RIGHT  // MU_ICON_MAX
    };

// Creates font by loading it from the disk from specified file name.
// Returns handle / id to the font.
int ren_install_font(const char* name, const char* file)
{
    // int idx = ctx.fonts[0];
    // if (idx )
    // ctx.fonts[0] = idx + 1;
    int id = nvgCreateFont(ctx.vg, name, file);
    if (id == -1) {
        printf("ren_install_font : Could not install \"%s\" font at %s\n", name, file);
    }
    return id;
}

void ren_install_font_defaults(void)
{
    // ren_install_font("sans", "../nanovg/example/Roboto-Regular.ttf");

    ren_install_font("sans", "data/font/InterDisplay-SemiBold.otf");
    // ren_install_font("sans", "../../pkg/@font/InputMono/InputMono-Medium.ttf");
    // ren_install_font("sans", "../../pkg/@font/Inconsolata/Inconsolata-Bold.ttf");
    // ren_install_font("sans", "../../pkg/@font/SF/SF-Mono-Semibold.otf");

    // ren_install_font("sans", "../../@font/Inter-3.13/hinted/ttf/Inter-Bold.ttf");
    ren_install_font("icons", "data/font/entypo.ttf");
}

static char* cpToUTF8(int cp, char* str)
{
	int n = 0;
	if (cp < 0x80) n = 1;
	else if (cp < 0x800) n = 2;
	else if (cp < 0x10000) n = 3;
	else if (cp < 0x200000) n = 4;
	else if (cp < 0x4000000) n = 5;
	else if (cp <= 0x7fffffff) n = 6;
	str[n] = '\0';
	switch (n) {
	case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
	case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
	case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
	case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
	case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
	case 1: str[0] = cp;
	}
	return str;
}

void ren_draw_icon(int id, ren_rect_t rect, ren_color_t color)
{

    char icon[8];
    vg_t* vg = ctx.vg;
    nvgFillColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
    // nvgFontSize(vg, 12.f);
    nvgFontFace(vg, "icons");
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    float x = (float)rect.x+(rect.w/2.f);
    float y = (float)rect.y+(rect.h/2.f);
    if (id < 6)
        nvgText(vg,x,y,cpToUTF8(REN_ICON_LUT[id],icon),NULL);
    else
        nvgText(vg,x,y,&id,NULL);
}

int ren_get_text_width(const char* text, int len)
{
    if (len == 0)
        return 0;
    int l = strlen(text);
    if (len == -1 || len >= l)
        len = l;
    return (int)nvgTextBounds(ctx.vg, 0, 0, text, text+len, NULL);
}

float ren_get_text_height(void)
{
    return REN_DEFAULT_FONT_SIZE;
}

// void _ren_flush(void)
// {
//     // TODO: ???
//     // vg_viewport(ctx.vg, 0, 0, ctx.fb_w, ctx.fb_h);
// }

void ren_set_clip_rect(ren_rect_t rect)
{
    vg_t* vg = ctx.vg;
    // _ren_flush();
    nvgScissor(vg, rect.x, rect.y, rect.w, rect.h);
    // glScissor(rect.x, height - (rect.y + rect.h), rect.w, rect.h);
    // nvgSave(vg);
}

void ren_clear(ren_color_t color)
{
    vg_t* vg = ctx.vg;
    // _ren_flush();
    vg_clear(vg, nvgRGBA(color.r, color.g, color.b, color.a));
    // glClearColor(clr.r / 255., clr.g / 255., clr.b / 255., clr.a / 255.);
    // glClear(GL_COLOREN_BUFFEREN_BIT);
}

void ren_present(void)
{
    // _ren_flush();
    // vg_did_render(vg);
    win_did_render(ctx.win);
    // gfx_did_render();
}

void ren_invalidate(void)
{
    // TODO?
    // rencache_invalidate()
}

void ren_delay(int ms)
{
    SDL_Delay(ms);
}

void ren_destroy(void)
{
    vg_destroy(ctx.vg);
    win_destroy(ctx.win);
    SDL_Quit();
    // ...
}

#endif
