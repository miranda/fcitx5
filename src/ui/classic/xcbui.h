/*
 * SPDX-FileCopyrightText: 2016-2016 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_XCBUI_H_
#define _FCITX_UI_CLASSIC_XCBUI_H_

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cairo.h>
#include <pango/pango-types.h>
#include <pango/pangocairo.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xproto.h>
#include "fcitx-utils/eventloopinterface.h"
#include "fcitx-utils/handlertable_details.h"
#include "fcitx-utils/misc.h"
#include "fcitx-utils/rect.h"
#include "fcitx/userinterface.h"
#include "classicui.h"

namespace fcitx::classicui {

class XCBInputWindow;
class XCBTrayWindow;

enum class MultiScreenExtension { Randr, Xinerama, EXTNone };

enum class XCBHintStyle { Default, NoHint, Medium, Slight, Full };

enum class XCBRGBA { Default, NoRGBA, RGB, BGR, VRGB, VBGR };

class XCBWindow;

struct XCBFontOption {
    int dpi = -1;
    bool antialias = true;
    XCBHintStyle hint = XCBHintStyle::Default;
    XCBRGBA rgba = XCBRGBA::Default;

    void setupPangoContext(PangoContext *context) const;
};

class XCBUI : public UIInterface {
public:
    XCBUI(ClassicUI *parent, const std::string &name, xcb_connection_t *conn,
          int defaultScreen);
    ~XCBUI();

    ClassicUI *parent() const { return parent_; }
    const std::string &displayName() const { return displayName_; }
    xcb_connection_t *connection() const { return conn_; }
    xcb_ewmh_connection_t *ewmh() const { return ewmh_; }
    xcb_window_t root() const { return root_; }
    int defaultScreen() const { return defaultScreen_; }
    xcb_colormap_t colorMap() const { return colorMap_; }
    xcb_visualid_t visualId() const;
    // Number of X screens on this connection ( > 1 in Zaphod mode).
    int screenCount() const { return screenCount_; }
    // The X screen that currently has keyboard focus (falls back to the
    // screen with the pointer, then the default screen).
    int focusedScreen();
    // Pick a visual for a window on the given screen, honoring that
    // screen's own composite manager selection (_NET_WM_CM_Sn).
    xcb_visualid_t visualIdForScreen(int screenNumber);
    void update(UserInterfaceComponent component,
                InputContext *inputContext) override;
    void updateCursor(InputContext *inputContext) override;
    void updateCurrentInputMethod(InputContext *inputContext) override;
    void suspend() override;
    void resume() override;
    void setEnableTray(bool enable) override;
    void setCairoDevice(cairo_device_t *device);

    const auto &screenRects() { return rects_; }
    int dpiByPosition(int x, int y);
    // Return the Xft.dpi published on the specified X screen's root window.
    // Falls back to the existing default-screen DPI policy when unset.
    int dpiForScreen(int screenNumber);
    int scaledDPI(int dpi);
    const XCBFontOption &fontOption() const { return fontOption_; }

    bool grabPointer(XCBWindow *window);
    void ungrabPointer();
    XCBWindow *pointerGrabber() const { return pointerGrabber_; }

private:
    void refreshCompositeManager();
    void refreshManager();
    void readXSettings();
    void initScreen();
    void scheduleUpdateScreen();
    void refreshFontOption(int screenNumber, xcb_screen_t *screen);

    static void destroyCairoDevice(cairo_device_t *device);

    ClassicUI *parent_;
    UniqueCPtr<cairo_device_t, destroyCairoDevice> device_;
    std::string displayName_;
    xcb_connection_t *conn_;
    XCBWindow *pointerGrabber_ = nullptr;
    xcb_window_t root_ = XCB_WINDOW_NONE;
    xcb_ewmh_connection_t *ewmh_;
    int defaultScreen_;
    int screenCount_ = 1;
    xcb_colormap_t colorMap_;
    bool needFreeColorMap_ = false;
    std::unique_ptr<XCBInputWindow> inputWindow_;
    std::unique_ptr<XCBTrayWindow> trayWindow_;

    std::string iconThemeName_;

    std::string compMgrAtomString_;
    xcb_atom_t compMgrAtom_ = XCB_ATOM_NONE;
    xcb_window_t compMgrWindow_ = XCB_WINDOW_NONE;

    xcb_atom_t managerAtom_ = XCB_ATOM_NONE;
    xcb_atom_t xsettingsSelectionAtom_ = XCB_ATOM_NONE;
    xcb_atom_t xsettingsWindow_ = XCB_WINDOW_NONE;
    xcb_atom_t xsettingsAtom_ = XCB_ATOM_NONE;

    // fontOption_ remains the default X screen's font options for existing
    // callers. screenFontOptions_ preserves each root's RESOURCE_MANAGER.
    XCBFontOption fontOption_;
    std::vector<XCBFontOption> screenFontOptions_;
    int maxDpi_ = -1;
    int primaryDpi_ = -1;
    int screenDpi_ = 96;
    MultiScreenExtension multiScreen_ = MultiScreenExtension::EXTNone;
    int xrandrFirstEvent_ = 0;
    std::unique_ptr<EventSourceTime> initScreenEvent_;

    std::vector<std::pair<Rect, int>> rects_;

    std::vector<std::unique_ptr<HandlerTableEntryBase>> eventHandlers_;
};

void addEventMaskToWindow(xcb_connection_t *conn, xcb_window_t wid,
                          uint32_t mask);
} // namespace fcitx::classicui

#endif // _FCITX_UI_CLASSIC_XCBUI_H_
