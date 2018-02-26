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
   Copyright (C) Wallix 2012
   Author(s): Christophe Grosjean

   Unit test for mouse cursor primitives
*/

#define RED_TEST_MODULE TestCursor
#include "system/redemption_unit_tests.hpp"
#include "test_only/check_sig.hpp"

#include "core/RDP/rdp_pointer.hpp"

RED_AUTO_TEST_CASE(TestDataSize)
{
    Pointer p;

    RED_CHECK_EQUAL(p.data_size(), 32 * 32 * 3);
}

RED_AUTO_TEST_CASE(TestDataSize1)
{
    Pointer p;

    const Pointer::CursorSize dimensions(24, 24);
    p.set_dimensions(dimensions);

    RED_CHECK_EQUAL(p.data_size(), 24 * 24 * 3);
}

RED_AUTO_TEST_CASE(TestMaskSize)
{
    Pointer p;
 
    const Pointer::CursorSize dimensions(7, 7);
    p.set_dimensions(dimensions);

    RED_CHECK_EQUAL(p.mask_size(), 14);
}

RED_AUTO_TEST_CASE(TestPointerNormal)
{
    Pointer p(Pointer::POINTER_NORMAL);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);
    
    uint8_t expected[] = {
        /* 0000 */ 0xFF, 0xFF, 0xFF, 0xFF,
                   0xFF, 0xFF, 0xFF, 0xFF,
        /* 0008 */ 0xFF, 0xFF, 0xFF, 0xFF,
                   0xFF, 0xFF, 0xFF, 0xFF,
        /* 0010 */ 0xFF, 0xFF, 0xFF, 0xFF,
                   0xFF, 0xFF, 0xFF, 0xFF,
        /* 0018 */ 0xFF, 0xFF, 0xFF, 0xFF,
                   0xFF, 0xFF, 0xFF, 0xFF,
        /* 0020 */ 0xFF, 0xFF, 0xFF, 0xFF,
                   0xFF, 0xFF, 0xFF, 0xFF,
        /* 0028 */ 0xFF, 0xFF, 0xFF, 0xFF,
                   0xFF, 0xFF, 0xFF, 0xFF,
        /* 0030 */ 0xFF, 0xFF, 0xFF, 0xFF,
                   0xFE, 0x7F, 0xFF, 0xFF, // 11111110 01111111
        /* 0038 */ 0xFC, 0x3F, 0xFF, 0xFF, // 11111100 00111111
                   0xFC, 0x3F, 0xFF, 0xFF,
        /* 0040 */ 0xF8, 0x7F, 0xFF, 0xFF,
                   0x78, 0x7F, 0xFF, 0xFF,
        /* 0048 */ 0x30, 0xFF, 0xFF, 0xFF,
                   0x10, 0xFF, 0xFF, 0xFF, // 00010000
        /* 0050 */ 0x01, 0xFF, 0xFF, 0xFF,
                   0x00, 0x1F, 0xFF, 0xFF,
        /* 0058 */ 0x00, 0x3F, 0xFF, 0xFF,
                   0x00, 0x7F, 0xFF, 0xFF,
        /* 0060 */ 0x00, 0xFF, 0xFF, 0xFF,
                   0x01, 0xFF, 0xFF, 0xFF,
        /* 0068 */ 0x03, 0xFF, 0xFF, 0xFF,
                   0x07, 0xFF, 0xFF, 0xFF,
        /* 0070 */ 0x0F, 0xFF, 0xFF, 0xFF,
                   0x1F, 0xFF, 0xFF, 0xFF,
        /* 0078 */ 0x3F, 0xFF, 0xFF, 0xFF,
                   0x7F, 0xFF, 0xFF, 0xFF,
    };

    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\x48\x58\x41\x28\xb0\xb4\x03\x27\x69\x31\x47\xc8\x2b\x43\x3e\xf3\x33\x80\x1c\xb2";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);

}

RED_AUTO_TEST_CASE(TestPointerEdit)
{
    Pointer p(Pointer::POINTER_EDIT);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);
    
    uint8_t expected[] = {
        /* 0000 */   0xff ,0xff ,0xff ,0xff
                    ,0xff ,0xff ,0xff ,0xff
        /* 0008 */  ,0xff ,0xff ,0xff ,0xff
                    ,0xff ,0xff ,0xff ,0xff
        /* 0010 */  ,0xff ,0xff ,0xff ,0xff
                    ,0xff ,0xff ,0xff ,0xff
        /* 0018 */  ,0xff ,0xff ,0xff ,0xff
                    ,0xff ,0xe1 ,0x0f ,0xff
        /* 0020 */  ,0xff ,0xe0 ,0x0f ,0xff
                    ,0xff ,0xe0 ,0x0f ,0xff
        /* 0028 */  ,0xff ,0xfc ,0x7f ,0xff
                    ,0xff ,0xfc ,0x7f ,0xff
        /* 0030 */  ,0xff ,0xfc ,0x7f ,0xff
                    ,0xff ,0xfc ,0x7f ,0xff
        /* 0038 */  ,0xff ,0xfc ,0x7f ,0xff
                    ,0xff ,0xfc ,0x7f ,0xff
        /* 0040 */  ,0xff ,0xfc ,0x7f ,0xff
                    ,0xff ,0xfc ,0x7f ,0xff
        /* 0048 */  ,0xff ,0xfc ,0x7f ,0xff
                    ,0xff ,0xfc ,0x7f ,0xff
        /* 0050 */  ,0xff ,0xfc ,0x7f ,0xff
                    ,0xff ,0xfc ,0x7f ,0xff
        /* 0058 */  ,0xff ,0xe0 ,0x0f ,0xff
                    ,0xff ,0xe0 ,0x0f ,0xff
        /* 0060 */  ,0xff ,0xe1 ,0x0f ,0xff
                    ,0xff ,0xff ,0xff ,0xff
        /* 0068 */  ,0xff ,0xff ,0xff ,0xff
                    ,0xff ,0xff ,0xff ,0xff
        /* 0070 */  ,0xff ,0xff ,0xff ,0xff
                    ,0xff ,0xff ,0xff ,0xff
        /* 0078 */  ,0xff ,0xff ,0xff ,0xff
                    ,0xff ,0xff ,0xff ,0xff
    };
    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\x09\xb3\xc6\x81\x07\xef\x2e\xd4\xcb\x1a\xa9\xeb\x2c\xb8\xe9\x13\xfb\xaf\xe2\xe3";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);
}

RED_AUTO_TEST_CASE(TestPointerDrawableDefault)
{
    Pointer p(Pointer::POINTER_DRAWABLE_DEFAULT);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);

    uint8_t expected[] = {
        /* 0000 */   0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0008 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0010 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0018 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0020 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0028 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0030 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0038 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xbc, 0x3f, 0xFF, 0xFF
        /* 0040 */ , 0x18, 0x7f, 0xFF, 0xFF
                   , 0x08, 0x7f, 0xFF, 0xFF
        /* 0048 */ , 0x00, 0xff, 0xFF, 0xFF
                   , 0x00, 0x0f, 0xFF, 0xFF
        /* 0050 */ , 0x00, 0x0f, 0xFF, 0xFF
                   , 0x00, 0x1f, 0xFF, 0xFF
        /* 0058 */ , 0x00, 0x3f, 0xFF, 0xFF
                   , 0x00, 0x7f, 0xFF, 0xFF
        /* 0060 */ , 0x00, 0xff, 0xFF, 0xFF
                   , 0x01, 0xff, 0xFF, 0xFF
        /* 0068 */ , 0x03, 0xff, 0xFF, 0xFF
                   , 0x07, 0xff, 0xFF, 0xFF
        /* 0070 */ , 0x0f, 0xff, 0xFF, 0xFF
                   , 0x1f, 0xff, 0xFF, 0xFF
        /* 0078 */ , 0x3f, 0xff, 0xFF, 0xFF
                   , 0x7f, 0xff, 0xFF, 0xFF
    };
    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\x46\x59\xfc\xbc\x13\x24\x18\xd6\x83\xa3\x6a\xc2\xea\xf8\x93\x2b\x30\x4f\x80\x0e";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);
}

RED_AUTO_TEST_CASE(TestPointerSystemDefault)
{

    Pointer p(Pointer::POINTER_SYSTEM_DEFAULT);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);

    uint8_t expected[] = {
        /* 0000 */   0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xcf, 0xFF
        /* 0008 */ , 0xFF, 0xFF, 0x87, 0xFF
                   , 0xFF, 0xFF, 0x87, 0xFF
        /* 0010 */ , 0xFF, 0xFF, 0x0f, 0xFF
                   , 0xFF, 0xdf, 0x0f, 0xFF
        /* 0018 */ , 0xFF, 0xce, 0x1f, 0xFF
                   , 0xFF, 0xc6, 0x1f, 0xFF
        /* 0020 */ , 0xFF, 0xc0, 0x3f, 0xFF
                   , 0xFF, 0xc0, 0x3f, 0xFF
        /* 0028 */ , 0xFF, 0xc0, 0x03, 0xFF
                   , 0xFF, 0xc0, 0x07, 0xFF
        /* 0030 */ , 0xFF, 0xc0, 0x0f, 0xFF
                   , 0xFF, 0xc0, 0x1f, 0xFF
        /* 0038 */ , 0xFF, 0xc0, 0x3f, 0xFF
                   , 0xFF, 0xc0, 0x7f, 0xFF
        /* 0040 */ , 0xFF, 0xc0, 0xFF, 0xFF
                   , 0xFF, 0xc1, 0xFF, 0xFF
        /* 0048 */ , 0xFF, 0xc3, 0xFF, 0xFF
                   , 0xFF, 0xc7, 0xFF, 0xFF
        /* 0050 */ , 0xFF, 0xcf, 0xFF, 0xFF
                   , 0xFF, 0xdf, 0xFF, 0xFF
        /* 0058 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0060 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0068 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0070 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0078 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
    };
    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\xc5\xc1\x0e\x3a\x17\x39\x56\x0c\xf9\xd7\x66\xac\x3b\x23\x23\xad\xec\xb5\xd9\x46";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);
}

RED_AUTO_TEST_CASE(TestPointerSizeNS)
{

    Pointer p(Pointer::POINTER_SIZENS);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);

    uint8_t expected[] = {
        /* 0000 */   0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0008 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0010 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0018 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0020 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0028 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xdf, 0xFF, 0xFF
        /* 0030 */ , 0xFF, 0x8f, 0xFF, 0xFF
                   , 0xFF, 0x07, 0xFF, 0xFF
        /* 0038 */ , 0xfe, 0x03, 0xFF, 0xFF
                   , 0xfc, 0x01, 0xFF, 0xFF
        /* 0040 */ , 0xfc, 0x01, 0xFF, 0xFF
                   , 0xFF, 0x8f, 0xFF, 0xFF
        /* 0048 */ , 0xFF, 0x8f, 0xFF, 0xFF
                   , 0xFF, 0x8f, 0xFF, 0xFF
        /* 0050 */ , 0xFF, 0x8f, 0xFF, 0xFF
                   , 0xFF, 0x8f, 0xFF, 0xFF
        /* 0058 */ , 0xFF, 0x8f, 0xFF, 0xFF
                   , 0xFF, 0x8f, 0xFF, 0xFF
        /* 0060 */ , 0xFF, 0x8f, 0xFF, 0xFF
                   , 0xFF, 0x8f, 0xFF, 0xFF
        /* 0068 */ , 0xfc, 0x01, 0xFF, 0xFF
                   , 0xfc, 0x01, 0xFF, 0xFF
        /* 0070 */ , 0xfe, 0x03, 0xFF, 0xFF
                   , 0xFF, 0x07, 0xFF, 0xFF
        /* 0078 */ , 0xFF, 0x8f, 0xFF, 0xFF
                   , 0xFF, 0xdf, 0xFF, 0xFF
    };
    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\x3a\x74\xfe\x82\xcf\xa9\x15\x9e\xa0\x8a\xf9\x43\x77\x40\xf8\xe6\xdf\x13\xd0\xe1";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);
}

RED_AUTO_TEST_CASE(TestPointerSizeNESW)
{
    Pointer p(Pointer::POINTER_SIZENESW);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);

    uint8_t expected[] = {

        /* 0000 */   0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0008 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0010 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0018 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0020 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0028 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0030 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0038 */ , 0xe0, 0x3f, 0xFF, 0xFF
                   , 0xe0, 0x3f, 0xFF, 0xFF
        /* 0040 */ , 0xe0, 0x7f, 0xFF, 0xFF
                   , 0xe0, 0xff, 0xFF, 0xFF
        /* 0048 */ , 0xe0, 0x7f, 0xFF, 0xFF
                   , 0xe2, 0x3f, 0xFF, 0xFF
        /* 0050 */ , 0xe7, 0x1f, 0xFF, 0xFF
                   , 0xFF, 0x8f, 0xFF, 0xFF
        /* 0058 */ , 0xFF, 0xc7, 0x3f, 0xFF
                   , 0xFF, 0xe2, 0x3f, 0xFF
        /* 0060 */ , 0xFF, 0xf0, 0x3f, 0xFF
                   , 0xFF, 0xf8, 0x3f, 0xFF
        /* 0068 */ , 0xFF, 0xf0, 0x3f, 0xFF
                   , 0xFF, 0xe0, 0x3f, 0xFF
        /* 0070 */ , 0xFF, 0xe0, 0x3f, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0078 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
    };
    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\x57\xed\x01\xb4\xe3\x9c\xe7\xc1\x0c\x9f\x01\xa1\xdc\x66\x4c\x92\x83\x05\x0e\x91";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);
}

RED_AUTO_TEST_CASE(TestPointerSizeNWSE)
{
    Pointer p(Pointer::POINTER_SIZENWSE);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);

    uint8_t expected[] = {

        /* 0000 */   0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0008 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0010 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0018 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0020 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0028 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0030 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0038 */ , 0xFF, 0xe0, 0x3f, 0xFF
                   , 0xFF, 0xe0, 0x3f, 0xFF
        /* 0040 */ , 0xFF, 0xf0, 0x3f, 0xFF
                   , 0xFF, 0xf8, 0x3f, 0xFF
        /* 0048 */ , 0xFF, 0xf0, 0x3f, 0xFF
                   , 0xFF, 0xe2, 0x3f, 0xFF
        /* 0050 */ , 0xFF, 0xc7, 0x3f, 0xFF
                   , 0xFF, 0x8f, 0xFF, 0xFF
        /* 0058 */ , 0xe7, 0x1f, 0xFF, 0xFF
                   , 0xe2, 0x3f, 0xFF, 0xFF
        /* 0060 */ , 0xe0, 0x7f, 0xFF, 0xFF
                   , 0xe0, 0xff, 0xFF, 0xFF
        /* 0068 */ , 0xe0, 0x7f, 0xFF, 0xFF
                   , 0xe0, 0x3f, 0xFF, 0xFF
        /* 0070 */ , 0xe0, 0x3f, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0078 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
    };
    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\x6b\x3a\xec\xa9\x22\x1f\x23\x5d\x2c\xe6\x91\xcc\x70\x56\xf9\x02\x30\x05\xaa\x3f";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);
}

RED_AUTO_TEST_CASE(TestPointerSizeWE)
{
    Pointer p(Pointer::POINTER_SIZEWE);
 
    RED_CHECK_EQUAL(p.bit_mask_size(), 32*4);

    uint8_t expected[] = {
        /* 0000 */   0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0008 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0010 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0018 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0020 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0028 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0030 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0038 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0040 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xf3, 0xfe, 0x7f, 0xFF
        /* 0048 */ , 0xe3, 0xfe, 0x3f, 0xFF
                   , 0xc3, 0xfe, 0x1f, 0xFF
        /* 0050 */ , 0x80, 0x00, 0x0f, 0xFF
                   , 0x00, 0x00, 0x07, 0xFF
        /* 0058 */ , 0x80, 0x00, 0x0f, 0xFF
                   , 0xc3, 0xfe, 0x1f, 0xFF
        /* 0060 */ , 0xe3, 0xfe, 0x3f, 0xFF
                   , 0xf3, 0xfe, 0x7f, 0xFF
        /* 0068 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0070 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
        /* 0078 */ , 0xFF, 0xFF, 0xFF, 0xFF
                   , 0xFF, 0xFF, 0xFF, 0xFF
    };
    RED_CHECK_MEM(p.get_monochrome_and_mask(), make_array_view(expected, sizeof(expected)));
    const char * sig = "\xae\x83\x2a\x8e\xd0\x73\x0b\x77\xe3\xb5\xf0\x4e\xbd\x0f\x4e\xa9\xd0\x65\xeb\x59";
    auto av = p.get_24bits_xor_mask();
    RED_CHECK_SIG2(av.data(), av.size(), sig);
}
