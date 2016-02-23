/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2015
    Author(s): Christophe Grosjean, Raphael Zhou
*/

#ifndef REDEMPTION_MOD_RDP_CHANNELS_SESPROLAUNCHER_HPP
#define REDEMPTION_MOD_RDP_CHANNELS_SESPROLAUNCHER_HPP

#include "core/wait_obj.hpp"
#include "mod/rdp/channels/base_channel.hpp"

class SessionProbeLauncher {
public:
    virtual ~SessionProbeLauncher() = default;

    virtual wait_obj* get_event() = 0;

    virtual bool on_clipboard_initialize() = 0;

    virtual bool on_drive_access() = 0;

    virtual bool on_event() = 0;

    virtual bool on_image_read(uint64_t offset, uint32_t length) = 0;

    virtual bool on_server_format_data_request() = 0;

    virtual bool on_server_format_list_response() = 0;

    virtual void set_session_probe_virtual_channel(
        BaseVirtualChannel* channel) = 0;

    virtual void stop() = 0;
};

#endif  // #ifndef REDEMPTION_MOD_RDP_CHANNELS_SESPROLAUNCHER_HPP