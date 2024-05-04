#ifndef GFX_UI_H
#define GFX_UI_H
/*

#define UI_VERSION "2.01"
#define UI_COMMANDLIST_SIZE     (256 * 1024)
#define UI_ROOTLIST_SIZE        32
#define UI_CONTAINERSTACK_SIZE  32
#define UI_CLIPSTACK_SIZE       32
#define UI_IDSTACK_SIZE         32
#define UI_LAYOUTSTACK_SIZE     16
#define UI_CONTAINERPOOL_SIZE   48
#define UI_TREENODEPOOL_SIZE    48
#define UI_MAX_WIDTHS           16
#define UI_REAL                 float
#define UI_REAL_FMT             "%.3g"
#define UI_SLIDER_FMT           "%.2f"
#define UI_MAX_FMT              127

#define ui_stack(T, n)          struct { int idx; T items[n]; }
#define ui_min(a, b)            ((a) < (b) ? (a) : (b))
#define ui_max(a, b)            ((a) > (b) ? (a) : (b))
#define ui_clamp(x, a, b)       ui_min(b, ui_max(a, x))

typedef struct UI_style {
  UI_font font;
  UI_vec2 size;
  int padding;
  int spacing;
  int indent;
  int title_height;
  int scrollbar_size;
  int thumb_size;
  ui_Color colors[MU_COLOR_MAX];
} UI_style;




h("div#box");


{
    .tag="div"; .id=""; .class="";
}

*/

#define ui_min(a, b)            ((a) < (b) ? (a) : (b))
#define ui_max(a, b)            ((a) > (b) ? (a) : (b))
#define ui_clamp(x, a, b)       mu_min(b, mu_max(a, x))

#define ui_stack(T, n)          struct { int idx; T items[n]; }

#define ui_stack_push(stk, val) do {                                                 \
    assert((stk).idx < (int) (sizeof((stk).items) / sizeof(*(stk).items))); \
    (stk).items[(stk).idx] = (val);                                         \
    (stk).idx++; /* incremented after incase `val` uses this value */       \
  } while (0)

#define ui_stack_pop(stk) do {      \
    assert((stk).idx > 0); \
    (stk).idx--;           \
  } while (0)



typedef struct { int x, y, w, h; } ren_rect_t;
typedef struct { unsigned char r, g, b, a; } ren_color_t;
typedef void* ren_font_t;


typedef struct
UI_ctx {
    /* callbacks */
    int (*text_width)(UI_font font, const char *str, int len);
    int (*text_height)(UI_font font);
    void (*draw_frame)(UI_ctx *ctx, UI_rect rect, int colorid);


    /* stacks */
    ui_stack(char, MU_COMMANDLIST_SIZE) command_list;

} UI_ctx;


void ui_init(UI_ctx *ctx);
void ui_begin(mu_Context *ctx);
void ui_end(mu_Context *ctx);

/*****************************************************************************/
#ifndef UI_IMPLEMENTATION

void
ui_init(UI_ctx *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    // ctx->draw_frame = draw_frame;
    // ctx->_style = default_style;
    // ctx->style = &ctx->_style;
}

void
ui_begin(UI_ctx *ctx)
{

}

void
ui_end(UI_ctx *ctx)
{

}


#endif
#endif