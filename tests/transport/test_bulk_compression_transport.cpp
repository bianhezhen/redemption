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
    Copyright (C) Wallix 2013
    Author(s): Christophe Grosjean, Raphael Zhou
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestBulkCompressionTransport
#include <boost/test/auto_unit_test.hpp>

#define LOGNULL
//#define LOGPRINT
#include "log.hpp"

#include "bulk_compression_transport.hpp"
#include "test_transport.hpp"

BOOST_AUTO_TEST_CASE(TestBulkCompressionTransport)
{
    MemoryTransport mt;

    BulkCompressionInTransport  in_trans(mt);
    BulkCompressionOutTransport out_trans(mt);

    out_trans.send(
          "0123456789ABCDEF"
          "0123456789ABCDEF"
          "0123456789ABCDEF"
          "0123456789ABCDEF"
        , 65);
    out_trans.next();
    out_trans.send(
          "azert"
          "azert"
          "azert"
          "azert"
        , 21);

    char   in_data[64] = { 0 };
    char * in_buffer   = in_data;

    in_trans.recv(&in_buffer, 65);
    LOG(LOG_INFO, "in_data=\"%s\"", in_data);

    in_buffer = in_data;
    in_trans.recv(&in_buffer, 21);
    LOG(LOG_INFO, "in_data=\"%s\"", in_data);
}
