
/*

Cocoa_CreateWindowFrom

    if ([(id)data isKindOfClass:[NSWindow class]]) {
      nswindow = (NSWindow*)data;
      nsview = [nswindow contentView];
    } else if ([(id)data isKindOfClass:[NSView class]]) {
      nsview = (NSView*)data;
      nswindow = [nsview window];
    } else {
      SDL_assert(false);
    }

*/

void
win_add_child(win_t* parent, win_t* child)
/*
    After added, child is maintained in relative position indicated by place for subsequent ordering operations involving either window.
    moving childWin will not cause the window to move (as in sliding a drawer in or out), but moving the window will cause childWin to move.
*/
{
  NSWindow* _parent = NULL;
  NSWindow* _child = NULL;
  _parent = win_get_handle(parent);
  _child = win_get_handle(child);
  [_parent addChildWindow:_child ordered:NSWindowAbove];
}


void
win_remove_child(win_t* parent, win_t* child)
{
  NSWindow* _parent = NULL;
  NSWindow* _child = NULL;
  _parent = win_get_handle(parent);
  _child = win_get_handle(child);
  [_parent removeChildWindow:_child];
}

// void*
// win_get_contentview(window_t* window, unsigned int tag)
// {
// 	// FOUNDATION_UNUSED(tag);
// 	return (__bridge void*)((window && window->nswindow) ?
// 	                        [(__bridge NSWindow*)window->nswindow contentView] : 0);
// }