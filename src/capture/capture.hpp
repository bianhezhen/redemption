/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010-2013
   Author(s): Christophe Grosjean, Raphael Zhou, Meng Tan
*/

#ifndef _REDEMPTION_CAPTURE_CAPTURE_HPP_
#define _REDEMPTION_CAPTURE_CAPTURE_HPP_

#include "transport/out_meta_sequence_transport.hpp"
#include "transport/out_meta_sequence_transport_with_sum.hpp"
#include "transport/crypto_out_meta_sequence_transport.hpp"
#include "transport/out_filename_sequence_transport.hpp"

#include "nativecapture.hpp"
#include "staticcapture.hpp"
#include "new_kbdcapture.hpp"

#include "RDP/compress_and_draw_bitmap_update.hpp"

#include "wait_obj.hpp"

#include "utils/pattutils.hpp"

// for extension
// end extension

class Capture final : public RDPGraphicDevice, public RDPCaptureDevice {
public:
    const bool capture_wrm;
    const bool capture_png;
// for extension
// end extension

    StaticCapture * psc;
    NewKbdCapture * pkc;
// for extension
// end extension

    wait_obj capture_event;

private:
    const configs::TraceType trace_type;
    CryptoContext & cctx;

public:
    OutFilenameSequenceTransport * png_trans;
private:
    Transport * wrm_trans;
// for extension
// end extension

    BmpCache      * pnc_bmp_cache;
    GlyphCache    * pnc_gly_cache;
    PointerCache  * pnc_ptr_cache;
    NativeCapture * pnc;

    RDPDrawable * drawable;
    RDPGraphicDevice * gd;

// for extension
// end extension

    timeval last_now;
    int     last_x;
    int     last_y;

    uint8_t order_bpp;
    uint8_t capture_bpp;

    const BGRPalette & mod_palette_rgb = BGRPalette::classic_332_rgb();

    const Inifile & ini;
    // TODO: why so many uninitialized constants ?
public:
    Capture(
        const timeval & now,
        int width, int height, int order_bpp, int capture_bpp,
        bool clear_png, bool no_timestamp, auth_api * authentifier,
        const Inifile & ini, Random & rnd, CryptoContext & cctx,
        bool full_video, bool extract_meta_data)
    : capture_wrm(bool(ini.get<cfg::video::capture_flags>() & configs::CaptureFlags::wrm))
    , capture_png(ini.get<cfg::video::png_limit>() > 0)
    , psc(nullptr)
    , pkc(nullptr)
    , capture_event{}
    , trace_type(ini.get<cfg::globals::trace_type>())
    , cctx(cctx)
    , png_trans(nullptr)
    , wrm_trans(nullptr)
    , pnc_bmp_cache(nullptr)
    , pnc_gly_cache(nullptr)
    , pnc_ptr_cache(nullptr)
    , pnc(nullptr)
    , drawable(nullptr)
    , gd(nullptr)
    , last_now(now)
    , last_x(width / 2)
    , last_y(height / 2)
    , order_bpp(order_bpp)
    , capture_bpp(capture_bpp)
    , ini(ini)
    {
        TODO("Remove that after change of capture interface")
        (void)full_video;
        TODO("Remove that after change of capture interface")
        (void)extract_meta_data;
        const int groupid = ini.get<cfg::video::capture_groupid>(); // www-data
        const bool capture_drawable = this->capture_wrm || this->capture_png;
        const char * record_tmp_path = ini.get<cfg::video::record_tmp_path>();
        const char * record_path = ini.get<cfg::video::record_path>();
        const char * hash_path = ini.get<cfg::video::hash_path>();

        char path[1024];
        char basename[1024];
        char extension[128];
        strcpy(path, WRM_PATH "/");     // default value, actual one should come from movie_path
        strcpy(basename, "redemption"); // default value actual one should come from movie_path
        strcpy(extension, "");          // extension is currently ignored
        const bool res = canonical_path(ini.get<cfg::globals::movie_path>().c_str(),
                                        path, sizeof(path),
                                        basename, sizeof(basename),
                                        extension, sizeof(extension));
        if (!res) {
            LOG(LOG_ERR, "Buffer Overflowed: Path too long");
            throw Error(ERR_RECORDER_FAILED_TO_FOUND_PATH);
        }


        if (capture_drawable) {
            this->drawable = new RDPDrawable(width, height, capture_bpp);
        }

        if (this->capture_png) {
            if (recursive_create_directory(record_tmp_path, S_IRWXU|S_IRWXG,
                        groupid) != 0) {
                LOG(LOG_ERR, "Failed to create directory: \"%s\"", record_tmp_path);
            }
            this->png_trans = new OutFilenameSequenceTransport(
                                FilenameGenerator::PATH_FILE_COUNT_EXTENSION,
                                record_tmp_path, basename, ".png", groupid, authentifier);
            this->psc = new StaticCapture(now, *this->png_trans,
                                          this->png_trans->seqgen(),
                                          width, height, clear_png, ini,
                                          this->drawable->impl());
        }

        if (this->capture_wrm) {
            if (recursive_create_directory(record_path, S_IRWXU | S_IRGRP | S_IXGRP, groupid) != 0) {
                LOG(LOG_ERR, "Failed to create directory: \"%s\"", record_path);
            }

            if (recursive_create_directory(hash_path, S_IRWXU | S_IRGRP | S_IXGRP, groupid) != 0) {
                LOG(LOG_ERR, "Failed to create directory: \"%s\"", hash_path);
            }

            TODO("there should only be one outmeta, not two."
                 " Capture code should not really care if file is encrypted or not."
                 "Here is not the right level to manage anything related to encryption.")
            TODO("Also we may wonder why we are encrypting wrm and not png"
                 "(This is related to the path split between png and wrm)."
                 "We should stop and consider what we should actually do")

            this->pnc_bmp_cache = new BmpCache( BmpCache::Recorder, capture_bpp, 3, false
                                              , BmpCache::CacheOption(600, 768, false)
                                              , BmpCache::CacheOption(300, 3072, false)
                                              , BmpCache::CacheOption(262, 12288, false)
                                              );
            this->pnc_gly_cache = new GlyphCache();
            const int pointerCacheSize = 0x19;
            this->pnc_ptr_cache = new PointerCache(pointerCacheSize);

            if (this->trace_type == configs::TraceType::cryptofile) {
                auto * trans = new CryptoOutMetaSequenceTransport(
                    &this->cctx, record_path, hash_path, basename, now
                  , width, height, groupid, authentifier);
                this->wrm_trans = trans;
            }
            else if (this->trace_type == configs::TraceType::localfile_hashed) {
                auto * trans = new OutMetaSequenceTransportWithSum(
                    &this->cctx, record_path, hash_path, basename,
                    now, width, height, groupid, authentifier);
                this->wrm_trans = trans;
            }
            else {
                auto * trans = new OutMetaSequenceTransport(
                    &this->cctx,
                    record_path, hash_path, basename, now,
                    width, height, groupid, authentifier);
                this->wrm_trans = trans;
            }
            this->pnc = new NativeCapture( now, *this->wrm_trans, width, height, capture_bpp
                                         , *this->pnc_bmp_cache, *this->pnc_gly_cache, *this->pnc_ptr_cache
                                         , *this->drawable, ini
                                         , NativeCapture::SendInput::YES);
        }

        if (!bool(ini.get<cfg::video::disable_keyboard_log>() & configs::KeyboardLogFlags::syslog) ||
            ini.get<cfg::session_log::enable_session_log>() ||
            ::contains_kbd_pattern(ini.get<cfg::context::pattern_kill>().c_str()) ||
            ::contains_kbd_pattern(ini.get<cfg::context::pattern_notify>().c_str())) {
            const bool is_kc_driven_by_ocr = false;
            this->pkc = new NewKbdCapture(now, authentifier, ini.get<cfg::context::pattern_kill>().c_str(),
                    ini.get<cfg::context::pattern_notify>().c_str(),
                    !bool(ini.get<cfg::video::disable_keyboard_log>() & configs::KeyboardLogFlags::syslog),
                    is_kc_driven_by_ocr,
                    ini.get<cfg::debug::capture>()
                );
        }

        if (this->capture_wrm) {
            this->gd = this->pnc;
        }
        else if (this->drawable) {
            this->gd = this->drawable;
        }
    }

    ~Capture() override {
        delete this->pkc;

        delete this->psc;
        delete this->png_trans;

        if (this->pnc) {
            timeval now = tvtime();
            this->pnc->recorder.timestamp(now);
            this->pnc->recorder.send_timestamp_chunk(false);
            delete this->pnc;
        }
        delete this->pnc_bmp_cache;
        delete this->pnc_gly_cache;
        delete this->pnc_ptr_cache;
        delete this->drawable;
        delete this->wrm_trans;
    }

    void request_full_cleaning()
    {
        this->wrm_trans->request_full_cleaning();
    }

    void pause() {
        if (this->capture_png) {
            timeval now = tvtime();
            this->psc->pause_snapshot(now);
        }
    }

    void resume() {
        if (this->capture_wrm){
            this->wrm_trans->next();
            timeval now = tvtime();
            this->pnc->recorder.timestamp(now);
            this->pnc->recorder.send_timestamp_chunk(true);
        }
    }

    void update_config(const Inifile & ini) {
        if (this->capture_png) {
            this->psc->update_config(ini);
        }
        if (this->capture_wrm) {
            this->pnc->update_config(ini);
        }
    }

    void set_row(size_t rownum, const uint8_t * data) override {
        if (this->drawable){
            this->drawable->set_row(rownum, data);
        }
    }

    void snapshot(const timeval & now, int x, int y, bool ignore_frame_in_timeval,
                          bool const & requested_to_stop) override {
        this->capture_event.reset();

        if (this->drawable) {
            this->drawable->set_mouse_cursor_pos(x, y);
        }

        this->last_now = now;
        this->last_x   = x;
        this->last_y   = y;

        if (this->capture_png) {
            this->psc->snapshot(now, x, y, ignore_frame_in_timeval, requested_to_stop);
            this->capture_event.update(this->psc->time_to_wait);
        }
        if (this->capture_wrm) {
            this->pnc->snapshot(now, x, y, ignore_frame_in_timeval, requested_to_stop);
            this->capture_event.update(this->pnc->time_to_wait);
        }

        if (this->pkc) {
            this->pkc->snapshot(now, x, y, ignore_frame_in_timeval, requested_to_stop);
            this->capture_event.update(this->pkc->time_to_wait);
        }
    }

    void flush() override {
        if (this->capture_wrm) {
            this->pnc->flush();
        }
    }

    bool input(const timeval & now, uint8_t const * input_data_32, std::size_t data_sz) override {
        if (this->capture_wrm) {
            this->pnc->input(now, input_data_32, data_sz);
        }

        bool send_to_mod = true;

        if (this->pkc) {
            send_to_mod = this->pkc->input(now, input_data_32, data_sz);
        }

        return send_to_mod;
    }

    void enable_keyboard_input_mask(bool enable) {
        if (this->capture_wrm) {
            this->pnc->enable_keyboard_input_mask(enable);
        }

        if (this->pkc) {
            this->pkc->enable_keyboard_input_mask(enable);
        }
    }

    void draw(const RDPScrBlt & cmd, const Rect & clip) override {
        if (this->gd) {
            this->gd->draw(cmd, clip);
        }
    }

    void draw(const RDPDestBlt & cmd, const Rect &clip) override {
        if (this->gd) {
            this->gd->draw(cmd, clip);
        }
    }

    void draw(const RDPMultiDstBlt & cmd, const Rect & clip) override {
        if (this->gd) {
            this->gd->draw(cmd, clip);
        }
    }

    void draw(const RDPMultiOpaqueRect & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPMultiOpaqueRect capture_cmd = cmd;

                capture_cmd._Color = color_encode(
                      color_decode_opaquerect(cmd._Color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDP::RDPMultiPatBlt & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDP::RDPMultiPatBlt capture_cmd = cmd;

                capture_cmd.BackColor = color_encode(
                      color_decode_opaquerect(cmd.BackColor, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );
                capture_cmd.ForeColor = color_encode(
                      color_decode_opaquerect(cmd.ForeColor, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDP::RDPMultiScrBlt & cmd, const Rect & clip) override {
        if (this->gd) {
            this->gd->draw(cmd, clip);
        }
    }

    void draw(const RDPPatBlt & cmd, const Rect &clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPPatBlt capture_cmd = cmd;

                capture_cmd.back_color = color_encode(
                      color_decode_opaquerect(cmd.back_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );
                capture_cmd.fore_color = color_encode(
                      color_decode_opaquerect(cmd.fore_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDPMemBlt & cmd, const Rect & clip, const Bitmap & bmp) override {
        if (this->gd) {
            this->gd->draw(cmd, clip, bmp);
        }
    }

    void draw(const RDPMem3Blt & cmd, const Rect & clip, const Bitmap & bmp) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPMem3Blt capture_cmd = cmd;

                capture_cmd.back_color = color_encode(
                      color_decode_opaquerect(cmd.back_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );
                capture_cmd.fore_color = color_encode(
                      color_decode_opaquerect(cmd.fore_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip, bmp);
            }
            else {
                this->gd->draw(cmd, clip, bmp);
            }
        }
    }

    void draw(const RDPOpaqueRect & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPOpaqueRect capture_cmd = cmd;

                capture_cmd.color = color_encode(
                      color_decode_opaquerect(cmd.color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDPLineTo & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPLineTo capture_cmd = cmd;

                capture_cmd.back_color = color_encode(
                      color_decode_opaquerect(cmd.back_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );
                capture_cmd.pen.color = color_encode(
                      color_decode_opaquerect(cmd.pen.color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDPBrushCache & cmd) override {
        if (this->gd) {
            this->gd->draw(cmd);
        }
    }

    void draw(const RDPColCache & cmd) override {
        if (this->gd) {
            this->gd->draw(cmd);
        }
    }

    void draw(const RDPGlyphIndex & cmd, const Rect & clip, const GlyphCache * gly_cache) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPGlyphIndex capture_cmd = cmd;

                capture_cmd.back_color = color_encode(
                      color_decode_opaquerect(cmd.back_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );
                capture_cmd.fore_color = color_encode(
                      color_decode_opaquerect(cmd.fore_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip, gly_cache);
            }
            else {
                this->gd->draw(cmd, clip, gly_cache);
            }
        }
    }

    void draw(const RDPBitmapData & bitmap_data, const uint8_t * data , size_t size, const Bitmap & bmp) override {
        if (this->gd) {
            if (this->capture_wrm) {
                if (bmp.bpp() > this->capture_bpp) {
                    // reducing the color depth of image.
                    Bitmap capture_bmp(this->capture_bpp, bmp);

                    ::compress_and_draw_bitmap_update(bitmap_data, capture_bmp, this->capture_bpp, *this->gd);
                }
                else if (!(bitmap_data.flags & BITMAP_COMPRESSION)) {
                    ::compress_and_draw_bitmap_update(bitmap_data, bmp, this->capture_bpp, *this->gd);
                }
                else {
                    this->gd->draw(bitmap_data, data, size, bmp);
                }
            }
            else {
                this->gd->draw(bitmap_data, data, size, bmp);
            }
        }
    }

    void draw(const RDP::FrameMarker & order) override {
        if (this->gd) {
            this->gd->draw(order);
        }

        if (order.action == RDP::FrameMarker::FrameEnd) {
            if (this->capture_png) {
                bool requested_to_stop = false;
                this->psc->snapshot(this->last_now, this->last_x, this->last_y, false, requested_to_stop);
            }
        }
    }

    void draw(const RDPPolygonSC & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPPolygonSC capture_cmd = cmd;

                capture_cmd.BrushColor = color_encode(
                      color_decode_opaquerect(cmd.BrushColor, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDPPolygonCB & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPPolygonCB capture_cmd = cmd;

                capture_cmd.foreColor = color_encode(
                      color_decode_opaquerect(cmd.foreColor, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );
                capture_cmd.backColor = color_encode(
                      color_decode_opaquerect(cmd.backColor, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDPPolyline & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPPolyline capture_cmd = cmd;

                capture_cmd.PenColor = color_encode(
                      color_decode_opaquerect(cmd.PenColor, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDPEllipseSC & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPEllipseSC capture_cmd = cmd;

                capture_cmd.color = color_encode(
                      color_decode_opaquerect(cmd.color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDPEllipseCB & cmd, const Rect & clip) override {
        if (this->gd) {
            if (this->capture_bpp != this->order_bpp) {
                RDPEllipseCB capture_cmd = cmd;

                capture_cmd.back_color = color_encode(
                      color_decode_opaquerect(cmd.back_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );
                capture_cmd.fore_color = color_encode(
                      color_decode_opaquerect(cmd.fore_color, this->order_bpp, this->mod_palette_rgb)
                    , this->capture_bpp
                    );

                this->gd->draw(capture_cmd, clip);
            }
            else {
                this->gd->draw(cmd, clip);
            }
        }
    }

    void draw(const RDP::RAIL::NewOrExistingWindow & order) override {
        if (this->gd) {
            this->gd->draw(order);
        }
    }

    void draw(const RDP::RAIL::WindowIcon & order) override {
        if (this->gd) {
            this->gd->draw(order);
        }
    }

    void draw(const RDP::RAIL::CachedIcon & order) override {
        if (this->gd) {
            this->gd->draw(order);
        }
    }

    void draw(const RDP::RAIL::DeletedWindow & order) override {
        if (this->gd) {
            this->gd->draw(order);
        }
    }

    void server_set_pointer(const Pointer & cursor) override {
        if (this->gd) {
            this->gd->server_set_pointer(cursor);
        }
    }

    void set_mod_palette(const BGRPalette & palette) override {
        if (this->drawable) {
            this->drawable->set_mod_palette(palette);
        }
    }

    void set_pointer_display() override {
        if (this->drawable) {
            this->drawable->show_mouse_cursor(false);
        }
    }

    // toggles externally genareted breakpoint.
    void external_breakpoint() override {
        if (this->capture_wrm) {
            this->pnc->external_breakpoint();
        }
    }

    void external_time(const timeval & now) override {
        if (this->capture_wrm) {
            this->pnc->external_time(now);
        }
    }

    void session_update(const timeval & now, const char * message) override {
        if (this->capture_wrm) {
            this->pnc->session_update(now, message);
        }

        if (this->pkc) {
            this->pkc->session_update(now, message);
        }
    }

    void possible_active_window_change() override {
        if (this->pkc) {
            this->pkc->possible_active_window_change();
        }
    }
};

#endif
