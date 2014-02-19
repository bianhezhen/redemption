/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Meng Tan
 */

#ifndef REDEMPTION_UTILS_COLOR_THEME_HPP
#define REDEMPTION_UTILS_COLOR_THEME_HPP

#include "colors.hpp"
#include "cfgloader.hpp"

struct ObjColor {
    int bg;
    int fg;
    int border;

    ObjColor(int bgcolor, int fgcolor)
        : bg(bgcolor)
        , fg(fgcolor)
        , border(fgcolor)
    {
    }

};


struct ColorTheme : public ConfigurationHolder {
    struct {
        int bgcolor;
        int fgcolor;
        int separator_color;
        int focus_color;
    } global;

    // struct {
    //     int bgcolor;
    //     int fgcolor;
    // } label;

    struct {
        int bgcolor;
        int fgcolor;
        int bordercolor;
    } edit;

    struct {
        int fontcolor;
        int bgcolor;
        int bordercolor;
    } tooltip;

    // struct {
    //     int bgcolor;
    //     int fgcolor;
    //     int bordercolor;
    // } button;
    // struct {
    //     int bgcolor;
    //     int fgcolor;
    //     int bordercolor;
    // } button_focus;

    struct {
        int bgcolor;
        int fgcolor;
    } selector_line1;
    struct {
        int bgcolor;
        int fgcolor;
    } selector_line2;
    struct {
        int bgcolor;
        int fgcolor;
    } selector_selected;
    struct {
        int bgcolor;
        int fgcolor;
    } selector_focus;
    struct {
        int bgcolor;
        int fgcolor;
    } selector_label;

    ColorTheme() {
        this->init();
    }

    void init() {
        this->global.bgcolor = DARK_BLUE_BIS;
        this->global.fgcolor = WHITE;
        this->global.separator_color = LIGHT_BLUE;
        this->global.focus_color = WINBLUE;

        // this->label.bgcolor = DARK_BLUE_BIS;
        // this->label.fgcolor = WHITE;

        this->edit.bgcolor = WHITE;
        this->edit.fgcolor = BLACK;
        this->edit.bordercolor = BLACK;

        this->tooltip.fontcolor = BLACK;
        this->tooltip.bgcolor = LIGHT_YELLOW;
        this->tooltip.bordercolor = BLACK;

        // this->button.bgcolor = DARK_BLUE_BIS;
        // this->button.fgcolor = WHITE;
        // this->button.bordercolor = WHITE;

        // this->button_focus.bgcolor = WINBLUE;
        // this->button_focus.fgcolor = WHITE;
        // this->button_focus.bordercolor = WHITE;

        this->selector_line1.fgcolor = BLACK;
        this->selector_line1.bgcolor = PALE_BLUE;
        this->selector_line2.fgcolor = BLACK;
        this->selector_line2.bgcolor = LIGHT_BLUE;

        this->selector_selected.bgcolor = MEDIUM_BLUE;
        this->selector_selected.fgcolor = WHITE;

        this->selector_focus.bgcolor = WINBLUE;
        this->selector_focus.fgcolor = WHITE;

        this->selector_label.bgcolor = MEDIUM_BLUE;
        this->selector_label.fgcolor = WHITE;

        // this->test();
    }
    void test() {
        this->global.bgcolor = 0x1581dc;
        this->global.fgcolor = WHITE;
        this->global.separator_color = 0x006ac4;
        this->global.focus_color = 0x221c9d;

        this->edit.bgcolor = WHITE;
        this->edit.fgcolor = BLACK;

        this->selector_line1.fgcolor = BLACK;
        this->selector_line1.bgcolor = 0x9ad5ff;
        this->selector_line2.fgcolor = BLACK;
        this->selector_line2.bgcolor = 0x64bfff;

        this->selector_selected.bgcolor = 0x302db7;
        this->selector_selected.fgcolor = WHITE;

        this->selector_focus.bgcolor = 0x221c9d;
        this->selector_focus.fgcolor = WHITE;

        this->selector_label.bgcolor = 0x302db7;
        this->selector_label.fgcolor = WHITE;
    }

    virtual void set_value(const char * context, const char * key, const char * value)
    {
        if (0 == strcmp(context, "global")) {
            if (0 == strcmp(key, "bgcolor")){
                this->global.bgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "fgcolor")){
                this->global.fgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "separator_color")){
                this->global.separator_color = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "focus_color")){
                this->global.focus_color = color_from_cstr(value);
            }
        }
        else if (0 == strcmp(context, "edit")) {
            if (0 == strcmp(key, "bgcolor")) {
                this->edit.bgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "fgcolor")) {
                this->edit.fgcolor = color_from_cstr(value);
            }
        }
        else if (0 == strcmp(context, "selector")) {
            if (0 == strcmp(key, "line1_fgcolor")) {
                this->selector_line1.fgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "line1_bgcolor")) {
                this->selector_line1.bgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "line2_fgcolor")) {
                this->selector_line2.fgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "line2_bgcolor")) {
                this->selector_line2.bgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "selected_bgcolor")) {
                this->selector_selected.bgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "selected_fgcolor")) {
                this->selector_selected.fgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "focus_bgcolor")) {
                this->selector_focus.bgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "focus_fgcolor")) {
                this->selector_focus.fgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "label_bgcolor")) {
                this->selector_label.bgcolor = color_from_cstr(value);
            }
            else if (0 == strcmp(key, "label_fgcolor")) {
                this->selector_label.fgcolor = color_from_cstr(value);
            }
        }
    }
};


#endif
