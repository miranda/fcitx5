/*
 * SPDX-FileCopyrightText: 2017-2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_XCBINPUTWINDOW_H_
#define _FCITX_UI_CLASSIC_XCBINPUTWINDOW_H_

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include "fcitx-utils/rect.h"
#include "fcitx/inputcontext.h"
#include "inputwindow.h"
#include "xcbui.h"
#include "xcbwindow.h"

namespace fcitx::classicui {

class XCBInputWindow : public XCBWindow, protected InputWindow {
public:
    XCBInputWindow(XCBUI *ui);

    void postCreateWindow() override;
    void update(InputContext *inputContext);
    void updatePosition(InputContext *inputContext);

    bool filterEvent(xcb_generic_event_t *event) override;

    void updateDPI(InputContext *inputContext);

private:
    void repaint();
    // Move the window to the X screen that has input focus (Zaphod mode).
    // Returns true if the window was recreated on a different screen.
    bool updateScreen();
    const Rect *getClosestScreen(const Rect &cursorRect) const;
    int calculatePositionX(const Rect &cursorRect,
                           const Rect *closestScreen) const;
    int calculatePositionY(const Rect &cursorRect,
                           const Rect *closestScreen) const;

    xcb_atom_t atomBlur_;
    // Geometry of the X screen this window currently lives on. Used
    // instead of the default screen's RandR rects when the window is on a
    // non-default screen.
    Rect screenRect_;
};

} // namespace fcitx::classicui

#endif // _FCITX_UI_CLASSIC_XCBINPUTWINDOW_H_
