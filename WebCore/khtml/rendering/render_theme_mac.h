/*
 * This file is part of the theme implementation for form controls in WebCore.
 *
 * Copyright (C) 2005 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef RENDER_THEME_MAC_H
#define RENDER_THEME_MAC_H

#import "render_theme.h"

namespace khtml {

class RenderStyle;

class RenderThemeMac : public RenderTheme {
public:
    RenderThemeMac();
    virtual ~RenderThemeMac() { /* Have to just leak the cells, since statics are destroyed with no autorelease pool available */ }

    // A method to obtain the baseline position for a "leaf" control.  This will only be used if a baseline
    // position cannot be determined by examining child content. Checkboxes and radio buttons are examples of
    // controls that need to do this.
    virtual short baselinePosition(const RenderObject* o) const;

    // A method asking if the control changes its tint when the window has focus or not.
    virtual bool controlSupportsTints(const RenderObject* o) const;

    // A general method asking if any control tinting is supported at all.
    virtual bool supportsControlTints() const { return true; }

    virtual void adjustRepaintRect(const RenderObject* o, QRect& r);

protected:
    // Methods for each appearance value.
    virtual bool paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& i, const QRect& r);
    virtual void setCheckboxSize(RenderStyle* style) const;
    
    virtual bool paintRadio(RenderObject* o, const RenderObject::PaintInfo& i, const QRect& r);
    virtual void setRadioSize(RenderStyle* style) const;
    
    virtual void adjustButtonStyle(CSSStyleSelector* selector, RenderStyle* style, DOM::ElementImpl* e) const;
    virtual bool paintButton(RenderObject* o, const RenderObject::PaintInfo& i, const QRect& r);
    virtual void setButtonSize(RenderStyle* style) const;

private:
    QRect inflateRect(const QRect& r, const QSize& size, const int* margins) const;

    // Get the control size based off the font.  Used by some of the controls (like buttons).
    NSControlSize controlSizeForFont(RenderStyle* style) const;
    void setControlSize(NSCell* cell, const QSize* sizes, const QSize& minSize);
    void setSizeFromFont(RenderStyle* style, const QSize* sizes) const;
    QSize sizeForFont(RenderStyle* style, const QSize* sizes) const;
    void setFontFromControlSize(CSSStyleSelector* selector, RenderStyle* style, NSControlSize size) const;
    
    void addIntrinsicMargins(RenderStyle* style, NSControlSize size) const;
    
    void updateCheckedState(NSCell* cell, const RenderObject* o);
    void updateEnabledState(NSCell* cell, const RenderObject* o);
    void updateFocusedState(NSCell* cell, const RenderObject* o);
    void updatePressedState(NSCell* cell, const RenderObject* o);

    // Helpers for adjusting appearance and for painting
    const QSize* checkboxSizes() const;
    const int* checkboxMargins() const;
    void setCheckboxCellState(const RenderObject* o, const QRect& r);

    const QSize* radioSizes() const;
    const int* radioMargins() const;
    void setRadioCellState(const RenderObject* o, const QRect& r);

    void setButtonPaddingFromControlSize(RenderStyle* style, NSControlSize size) const;
    const QSize* buttonSizes() const;
    const int* buttonMargins() const;
    void setButtonCellState(const RenderObject* o, const QRect& r);

private:
    NSButtonCell* checkbox;
    NSButtonCell* radio;
    NSButtonCell* button;
};

}

#endif
