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
*   Copyright (C) Wallix 2010-2017
*   Author(s): Clément Moroldo
*/

#include "utils/log.hpp"

#define RED_TEST_MODULE TestRDPMetrics
#include "system/redemption_unit_tests.hpp"

#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <linux/hdreg.h>

#include "utils/fileutils.hpp"
#include "utils/sugar/unique_fd.hpp"

#include "core/RDP/rdp_metrics.hpp"



RED_AUTO_TEST_CASE(TestRDPMetricsOutputFileTurnOver) {

    ClientInfo info;
    uint8_t key[32] = {0};
    // tmp/rdp_metrics_file_test
    const char * templace_path_file = "tests/core/RDP/";
    RDPMetrics metrics( templace_path_file
                      , 1
                      , "user"
                      , "admin"
                      , "10.10.13.12"
                      , info
                      , "RDP1"
                      , key
                      , 24);

    char current_date[24] {};
    timeval now = tvtime();
    metrics.set_current_formated_date(current_date, false, now.tv_sec);

    char complete_file_path[4096] = {'\0'};
    ::snprintf(complete_file_path, sizeof(complete_file_path), "%srdp_metrics-%s.log", templace_path_file, current_date);
    int fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);
    ::close(fd);
    remove(complete_file_path);

    time_t yesterday_time = metrics.utc_last_date - 3600*24;
    metrics.utc_last_date = yesterday_time;
    char yesterday_date[24] = {};
    metrics.set_current_formated_date(yesterday_date, false, yesterday_time);
    char yesterday_complete_path[4096] = {'\0'};
    ::snprintf(yesterday_complete_path, sizeof(yesterday_complete_path), "%s-%s.log", templace_path_file, yesterday_date);
    remove(yesterday_complete_path);

    metrics.log();
    metrics.log();

    fd = ::open(yesterday_complete_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd == -1);
    RED_CHECK(yesterday_time <= metrics.utc_last_date);

    fd = ::open(complete_file_path, O_RDONLY | O_APPEND);
    RED_CHECK(fd > 0);

    ::close(fd);
    remove(complete_file_path);
}

RED_AUTO_TEST_CASE(TestRDPMetricsOutputLogHeader) {

    ClientInfo info;
    uint8_t key[32] = {0};
    const char * templace_path_file = "tests/core/RDP/";
    RDPMetrics metrics( templace_path_file
                      , 1
                      , "user"
                      , "admin"
                      , "10.10.13.12"
                      , info
                      , "RDP1"
                      , key
                      , 24);

    char current_date[24] = {'\0'};
    timeval now = tvtime();
    metrics.set_current_formated_date(current_date, false, now.tv_sec);

    char complete_file_path[4096] = {'\0'};
    ::snprintf(complete_file_path, sizeof(complete_file_path), "%srdp_metrics-%s.log", templace_path_file, current_date);
    int fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);
    metrics.log();

    std::string expected_log("Session_starting_time=");
    char start_full_date_time[24];
    metrics.set_current_formated_date(start_full_date_time, true, now.tv_sec);
    expected_log += start_full_date_time;
    expected_log += " Session_id=1 user=8D5F8AEEB64E3CE20B537D04C486407EAF489646617CFCF493E76F5B794FA080 account=5544E527C72AAE51DF22438F3EBA7B8A545F2D2391E64C4CC706EFFACA99D3C1 hostname=B613679A0814D9EC772F95D778C35FC5FF1697C493715653C6C712144292C5AD target_service=6349A3F669CACE1E4C7AE9C48B53A3F1A240EB3910D8B16850ECBC80A4A9B807 session_info=B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7 delta_time(s)=0";

    char log_read[512] = {'\0'};
    auto res = ::read(fd, log_read, expected_log.length());
    (void)res;
    std::string str_log_read(log_read);
    RED_CHECK_EQUAL(str_log_read, expected_log);
    ::close(fd);
    remove(complete_file_path);
}


RED_AUTO_TEST_CASE(TestRDPMetricsOutputData) {

    ClientInfo info;
    uint8_t key[32] = {0};
    const char * templace_path_file = "tests/core/RDP/";
    RDPMetrics metrics( templace_path_file
                      , 1
                      , "user"
                      , "admin"
                      , "10.10.13.12"
                      , info
                      , "RDP1"
                      , key
                      , 24);

    char current_date[24] = {'\0'};
    timeval now = tvtime();
    metrics.set_current_formated_date(current_date, false, now.tv_sec);

    char complete_file_path[4096] = {'\0'};
    ::snprintf(complete_file_path, sizeof(complete_file_path), "%srdp_metrics-%s.log", templace_path_file, current_date);
    int fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);

    for (int i = 0; i < 33; i++) {
        metrics.current_data[i] = i+1;
    }
    metrics.log();

    std::string expected_log_header("Session_starting_time=");
        char start_full_date_time[24];
    metrics.set_current_formated_date(start_full_date_time, true, now.tv_sec);
    expected_log_header += start_full_date_time;
    expected_log_header += " Session_id=1 user=8D5F8AEEB64E3CE20B537D04C486407EAF489646617CFCF493E76F5B794FA080 account=5544E527C72AAE51DF22438F3EBA7B8A545F2D2391E64C4CC706EFFACA99D3C1 hostname=B613679A0814D9EC772F95D778C35FC5FF1697C493715653C6C712144292C5AD target_service=6349A3F669CACE1E4C7AE9C48B53A3F1A240EB3910D8B16850ECBC80A4A9B807 session_info=B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7 delta_time(s)=0";
    RED_CHECK(fd > 0);

    std::string expected_log_data(" main_channel_data_from_client=1 right_click=2 left_click=3 keys_pressed=4 mouse_displacement=5 main_channel_data_from_serveur=6 clipboard_channel_data_from_server=7 nb_paste_text_on_server=8 nb_paste_image_on_server=9 nb_paste_file_on_server=10 total_data_paste_on_server=11 nb_copy_text_on_server=12 nb_copy_image_on_server=13 nb_copy_file_on_server=14 clipboard_channel_data_from_client=15 nb_paste_text_on_client=16 nb_paste_image_on_client=17 nb_paste_file_on_client=18 total_data_paste_on_client=19 nb_copy_text_on_client=20 nb_copy_image_on_client=21 nb_copy_file_on_client=22 disk_redirection_channel_data_from_client=23 disk_redirection_channel_data_from_server=24 nb_files_1k_read=25 nb_files_or_folders_deleted=26 nb_files_write=27 nb_files_rename=28 rail_channel_data_from_client=29 rail_channel_data_from_server=30 other_channel_data_from_client=31 other_channel_data_from_server=32\n");

    char log_read[4096] = {'\0'};
    auto res = ::read(fd, log_read, expected_log_header.length()+expected_log_data.length()+1);
    (void)res;
    std::string str_log_read(log_read);
    std::string str_log_data = str_log_read.substr(expected_log_header.length(), str_log_read.length());

    RED_CHECK_EQUAL(expected_log_data, str_log_data);
    ::close(fd);
    remove(complete_file_path);

    time_t yesterday_time = metrics.utc_last_date - 3600*24;
    metrics.utc_last_date = yesterday_time;
    for (int i = 0; i < 16; i++) {
        metrics.current_data[i] += i+1;
    }
    metrics.log();

    fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);

    std::string expected_log_data_2(" main_channel_data_from_client=2 right_click=4 left_click=6 keys_pressed=8 mouse_displacement=10 main_channel_data_from_serveur=12 clipboard_channel_data_from_server=14 nb_paste_text_on_server=16 nb_paste_image_on_server=18 nb_paste_file_on_server=20 total_data_paste_on_server=22 nb_copy_text_on_server=24 nb_copy_image_on_server=26 nb_copy_file_on_server=28 clipboard_channel_data_from_client=30 nb_paste_text_on_client=32\n");

    char log_read_2[2048] = {'\0'};
    {
        auto res = ::read(fd, log_read_2, expected_log_header.length()+expected_log_data_2.length()+1);
        (void)res;
    }
    std::string str_log_read_2(log_read_2);
    std::string str_log_data_2 = str_log_read_2.substr(expected_log_header.length(), str_log_read_2.length());

    RED_CHECK_EQUAL(expected_log_data_2, str_log_data_2);
    ::close(fd);
    remove(complete_file_path);
}


RED_AUTO_TEST_CASE(TestRDPMetricsCLIPRDRReadChunk) {

    ClientInfo info;
    uint8_t key[32] = {0};
    const char * templace_path_file = "tests/core/RDP/";
    RDPMetrics metrics( templace_path_file
                      , 1
                      , "user"
                      , "admin"
                      , "10.10.13.12"
                      , info
                      , "RDP1"
                      , key
                      , 24);

    char current_date[24] = {'\0'};
    timeval now = tvtime();
    metrics.set_current_formated_date(current_date, false, now.tv_sec);

    char complete_file_path[4096] = {'\0'};
    ::snprintf(complete_file_path, sizeof(complete_file_path), "%srdp_metrics-%s.log", templace_path_file, current_date);
    int fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);

    metrics.cliprdr_init_format_list_done = true;
    {
        StaticOutStream<1600> out_stream;
        RDPECLIP::CliprdrHeader format_list_header(RDPECLIP::CB_FORMAT_LIST, 0, 26+4);
        format_list_header.emit(out_stream);
        RDPECLIP::FormatListPDU_LongName format(49562, RDPECLIP::FILECONTENTS, 13);
        format.emit(out_stream);
        InStream chunk(out_stream.get_data(), out_stream.get_offset());

        metrics.set_server_cliprdr_metrics(chunk, out_stream.get_offset(), CHANNELS::CHANNEL_FLAG_FIRST);
    }
    {
        StaticOutStream<1600> out_stream;
        RDPECLIP::CliprdrHeader format_list_header(RDPECLIP::CB_FORMAT_LIST, 0, 4+2);
        format_list_header.emit(out_stream);
        std::string text_name;
        RDPECLIP::FormatListPDU_LongName format(RDPECLIP::CF_TEXT, "\0", 1);
        format.emit(out_stream);
        InStream chunk(out_stream.get_data(), out_stream.get_offset());

        metrics.set_server_cliprdr_metrics(chunk, out_stream.get_offset(), CHANNELS::CHANNEL_FLAG_FIRST);
    }
    {
        StaticOutStream<1600> out_stream;
        RDPECLIP::FileContentsRequestPDU fileContentsRequest( 0
                                                            , RDPECLIP::FILECONTENTS_RANGE
                                                            , 0
                                                            , 8);
        fileContentsRequest.emit(out_stream);
        InStream chunk(out_stream.get_data(), out_stream.get_offset());

        metrics.set_server_cliprdr_metrics(chunk, out_stream.get_offset(), CHANNELS::CHANNEL_FLAG_FIRST);
    }

    metrics.log();

    fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);

    std::string expected_log_header("Session_starting_time=");
    char start_full_date_time[24];
    metrics.set_current_formated_date(start_full_date_time, true, now.tv_sec);
    expected_log_header += start_full_date_time;
    expected_log_header += " Session_id=1 user=8D5F8AEEB64E3CE20B537D04C486407EAF489646617CFCF493E76F5B794FA080 account=5544E527C72AAE51DF22438F3EBA7B8A545F2D2391E64C4CC706EFFACA99D3C1 hostname=B613679A0814D9EC772F95D778C35FC5FF1697C493715653C6C712144292C5AD target_service=6349A3F669CACE1E4C7AE9C48B53A3F1A240EB3910D8B16850ECBC80A4A9B807 session_info=B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7 delta_time(s)=0";

    std::string expected_log_data_2(" clipboard_channel_data_from_server=84 total_data_paste_on_server=8 nb_copy_text_on_server=1 nb_copy_file_on_server=1\n");

    char log_read_2[2048] = {'\0'};
    auto res = ::read(fd, log_read_2, expected_log_header.length()+expected_log_data_2.length()+1);
    (void)res;
    std::string str_log_read_2(log_read_2);
    std::string str_log_data_2 = str_log_read_2.substr(expected_log_header.length(), str_log_read_2.length());

    RED_CHECK_EQUAL(expected_log_data_2, str_log_data_2);
    ::close(fd);
    remove(complete_file_path);
}

RED_AUTO_TEST_CASE(TestRDPMetricsRDPDRReadChunk) {

    ClientInfo info;
    uint8_t key[32] = {0};
    const char * templace_path_file = "tests/core/RDP/";
    RDPMetrics metrics( templace_path_file
                      , 1
                      , "user"
                      , "admin"
                      , "10.10.13.12"
                      , info
                      , "RDP1"
                      , key
                      , 24);

    char current_date[24] = {'\0'};
    timeval now = tvtime();
    metrics.set_current_formated_date(current_date, false, now.tv_sec);

    char complete_file_path[4096] = {'\0'};
    ::snprintf(complete_file_path, sizeof(complete_file_path), "%srdp_metrics-%s.log", templace_path_file, current_date);
    int fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);

    metrics.cliprdr_init_format_list_done = true;
    {
        StaticOutStream<1600> out_stream;
        rdpdr::SharedHeader header(rdpdr::RDPDR_CTYP_CORE, rdpdr::PAKID_CORE_DEVICE_IOCOMPLETION);
        header.emit(out_stream);
        rdpdr::DeviceIORequest dior(0, 0, 0, rdpdr::IRP_MJ_READ, 0);
        dior.emit(out_stream);

        rdpdr::DeviceReadRequest drr(1001, 0);
        drr.emit(out_stream);
        InStream chunk(out_stream.get_data(), out_stream.get_offset());

        metrics.set_server_rdpdr_metrics(chunk, out_stream.get_offset(), CHANNELS::CHANNEL_FLAG_FIRST);
    }
    {
        StaticOutStream<1600> out_stream;
        rdpdr::SharedHeader header(rdpdr::RDPDR_CTYP_CORE, rdpdr::PAKID_CORE_DEVICE_IOCOMPLETION);
        header.emit(out_stream);
        rdpdr::DeviceIORequest dior(0, 0, 0, rdpdr::IRP_MJ_SET_INFORMATION, 0);
        dior.emit(out_stream);

        rdpdr::ServerDriveSetInformationRequest sdsir(rdpdr::FileRenameInformation, 0);
        sdsir.emit(out_stream);
        InStream chunk(out_stream.get_data(), out_stream.get_offset());

        metrics.set_server_rdpdr_metrics(chunk, out_stream.get_offset(), CHANNELS::CHANNEL_FLAG_FIRST);
    }
    {
        StaticOutStream<1600> out_stream;
        rdpdr::SharedHeader header(rdpdr::RDPDR_CTYP_CORE, rdpdr::PAKID_CORE_DEVICE_IOCOMPLETION);
        header.emit(out_stream);
        rdpdr::DeviceIORequest dior(0, 0, 0, rdpdr::IRP_MJ_WRITE, 0);
        dior.emit(out_stream);

        InStream chunk(out_stream.get_data(), out_stream.get_offset());

        metrics.set_server_rdpdr_metrics(chunk, out_stream.get_offset(), CHANNELS::CHANNEL_FLAG_FIRST);
    }

    metrics.log();

    fd = ::open(complete_file_path, O_RDONLY| O_APPEND);
    RED_CHECK(fd > 0);

    std::string expected_log_header("Session_starting_time=");
    char start_full_date_time[24];
    metrics.set_current_formated_date(start_full_date_time, true, now.tv_sec);
    expected_log_header += start_full_date_time;
    expected_log_header += " Session_id=1 user=8D5F8AEEB64E3CE20B537D04C486407EAF489646617CFCF493E76F5B794FA080 account=5544E527C72AAE51DF22438F3EBA7B8A545F2D2391E64C4CC706EFFACA99D3C1 hostname=B613679A0814D9EC772F95D778C35FC5FF1697C493715653C6C712144292C5AD target_service=6349A3F669CACE1E4C7AE9C48B53A3F1A240EB3910D8B16850ECBC80A4A9B807 session_info=B079C9845904075BAC3DBE0A26CB7364CE0CC0A5F47DC082F44D221EBC6722B7 delta_time(s)=0";

    std::string expected_log_data_2(" disk_redirection_channel_data_from_server=136 nb_files_1k_read=1 nb_files_write=1 nb_files_rename=1\n");

    char log_read_2[2048] = {'\0'};
    auto res = ::read(fd, log_read_2, expected_log_header.length()+expected_log_data_2.length()+1);
    (void)res;
    std::string str_log_read_2(log_read_2);
    std::string str_log_data_2 = str_log_read_2.substr(expected_log_header.length(), str_log_read_2.length());

    RED_CHECK_EQUAL(expected_log_data_2, str_log_data_2);
    ::close(fd);
    remove(complete_file_path);
}

