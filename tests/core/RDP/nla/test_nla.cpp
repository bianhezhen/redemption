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
  Author(s): Christophe Grosjean, Raphael Zhou, Meng Tan
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestNLA
#include <boost/test/auto_unit_test.hpp>

#define LOGPRINT
#include "log.hpp"
#include "RDP/nla/nla.hpp"
#include "check_sig.hpp"

#include "testtransport.hpp"

// BOOST_AUTO_TEST_CASE(TestNla)
// {
//     LogTransport logtrans;
//     rdpCredssp credssp(logtrans);

//     credssp.credssp_client_authenticate();


// }

BOOST_AUTO_TEST_CASE(TestNlaclient)
{

    const char client[65000] =
        // negotiate
/* 0000 */ "\x30\x37\xa0\x03\x02\x01\x02\xa1\x30\x30\x2e\x30\x2c\xa0\x2a\x04" //07......00.0,.*.
/* 0010 */ "\x28\x4e\x54\x4c\x4d\x53\x53\x50\x00\x01\x00\x00\x00\xb7\x82\x08" //(NTLMSSP........
/* 0020 */ "\xe2\x00\x00\x00\x00\x28\x00\x00\x00\x00\x00\x00\x00\x28\x00\x00" //.....(.......(..
/* 0030 */ "\x00\x05\x01\x28\x0a\x00\x00\x00\x0f"                             //...(.....
        // authenticate
/* 0000 */ "\x30\x82\x01\x47\xa0\x03\x02\x01\x02\xa1\x82\x01\x1a\x30\x82\x01" //0..G.........0..
/* 0010 */ "\x16\x30\x82\x01\x12\xa0\x82\x01\x0e\x04\x82\x01\x0a\x4e\x54\x4c" //.0...........NTL
/* 0020 */ "\x4d\x53\x53\x50\x00\x03\x00\x00\x00\x18\x00\x18\x00\x58\x00\x00" //MSSP.........X..
/* 0030 */ "\x00\x70\x00\x70\x00\x70\x00\x00\x00\x0e\x00\x0e\x00\xe0\x00\x00" //.p.p.p..........
/* 0040 */ "\x00\x0c\x00\x0c\x00\xee\x00\x00\x00\x00\x00\x00\x00\xfa\x00\x00" //................
/* 0050 */ "\x00\x10\x00\x10\x00\xfa\x00\x00\x00\xb7\x82\x08\xe2\x05\x01\x28" //...............(
/* 0060 */ "\x0a\x00\x00\x00\x0f\x45\x7c\xb3\x01\x03\x0a\xf9\x5a\x73\xc9\x98" //.....E|.....Zs..
/* 0070 */ "\x5d\x04\xe4\xbc\x46\x37\x3f\x26\x0c\x38\xce\xe8\xf6\xc7\x83\xbb" //]...F7?&.8......
/* 0080 */ "\x72\x6d\xb1\x4d\xd8\x47\xa2\xe5\xcf\x27\xf7\x3c\x43\xdd\x39\xc4" //rm.M.G...'.<C.9.
/* 0090 */ "\x15\x92\x88\x11\x01\xc2\xbb\x69\xc6\xef\x52\x5d\x58\x01\x01\x00" //.......i..R]X...
/* 00a0 */ "\x00\x00\x00\x00\x00\xc3\x83\xa2\x1c\x6c\xb0\xcb\x01\x47\xa2\xe5" //.........l...G..
/* 00b0 */ "\xcf\x27\xf7\x3c\x43\x00\x00\x00\x00\x01\x00\x08\x00\x57\x00\x49" //.'.<C........W.I
/* 00c0 */ "\x00\x4e\x00\x37\x00\x02\x00\x08\x00\x57\x00\x49\x00\x4e\x00\x37" //.N.7.....W.I.N.7
/* 00d0 */ "\x00\x03\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x04\x00\x08" //.....w.i.n.7....
/* 00e0 */ "\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x07\x00\x08\x00\xc3\x83\xa2" //.w.i.n.7........
/* 00f0 */ "\x1c\x6c\xb0\xcb\x01\x00\x00\x00\x00\x00\x00\x00\x00\x49\x00\x74" //.l...........I.t
/* 0100 */ "\x00\x68\x00\x61\x00\x71\x00\x75\x00\x65\x00\x55\x00\x6c\x00\x79" //.h.a.q.u.e.U.l.y
/* 0110 */ "\x00\x73\x00\x73\x00\x65\x00\xdc\xed\xbe\x32\x02\x07\x8d\x54\xb3" //.s.s.e....2...T.
/* 0120 */ "\x31\x25\x8d\x65\x13\x40\x3d\xa3\x22\x04\x20\x01\x00\x00\x00\xb8" //1%.e.@=.". .....
/* 0130 */ "\x24\x73\x7b\x5d\x6b\xaa\x50\x00\x00\x00\x00\x26\x99\x46\x96\xcf" //$s{]k.P....&.F..
/* 0140 */ "\xfc\x2f\x0b\x41\xe4\x6a\x4c\x4b\xd1\x52\x82"                     //./.A.jLK.R.
        // ts credentials (authinfo)
/* 0000 */ "\x30\x5c\xa0\x03\x02\x01\x02\xa2\x55\x04\x53\x01\x00\x00\x00\xac" //0.......U.S.....
/* 0010 */ "\xfb\x32\xad\xda\xe5\xca\xe5\x01\x00\x00\x00\x9d\x85\xa1\x3f\x17" //.2............?.
/* 0020 */ "\x2b\xcf\x8f\x87\xb3\x27\xf9\xb5\x28\x33\x28\x38\x75\x9f\xdc\x0f" //+....'..(3(8u...
/* 0030 */ "\x97\xf9\x3d\xa6\x8d\x97\xe1\x52\x67\x28\xc7\x6f\x22\x39\x04\xca" //..=....Rg(.o"9..
/* 0040 */ "\xd5\x93\x54\xd6\x4d\xfa\x37\xbf\x2c\xdb\xae\x78\x2a\xaf\x45\xde" //..T.M.7.,..x*.E.
/* 0050 */ "\xfe\x7c\x19\xaa\x78\xac\x2c\x9a\x5e\x12\x87\xf1\x3b\x06"         //.|..x.,.^...;.
        ;

    const char server[65000] =
        // challenge
/* 0000 */ "\x30\x81\x88\xa0\x03\x02\x01\x02\xa1\x81\x80\x30\x7e\x30\x7c\xa0" //0..........0~0|.
/* 0010 */ "\x7a\x04\x78\x4e\x54\x4c\x4d\x53\x53\x50\x00\x02\x00\x00\x00\x00" //z.xNTLMSSP......
/* 0020 */ "\x00\x00\x00\x38\x00\x00\x00\xb7\x82\x08\xe2\x26\x6e\xcd\x75\xaa" //...8.......&n.u.
/* 0030 */ "\x41\xe7\x6f\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x40\x00\x38" //A.o........@.@.8
/* 0040 */ "\x00\x00\x00\x05\x01\x28\x0a\x00\x00\x00\x0f\x01\x00\x08\x00\x57" //.....(.........W
/* 0050 */ "\x00\x49\x00\x4e\x00\x37\x00\x02\x00\x08\x00\x57\x00\x49\x00\x4e" //.I.N.7.....W.I.N
/* 0060 */ "\x00\x37\x00\x03\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x04" //.7.....w.i.n.7..
/* 0070 */ "\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x07\x00\x08\x00\xc3" //...w.i.n.7......
/* 0080 */ "\x83\xa2\x1c\x6c\xb0\xcb\x01\x00\x00\x00\x00"                     //...l.......
        // pubauthkey
/* 0000 */ "\x30\x29\xa0\x03\x02\x01\x02\xa3\x22\x04\x20\x01\x00\x00\x00\x75" //0)......". ....u
/* 0010 */ "\xe2\x83\x53\x5e\xd5\xe0\xae\x00\x00\x00\x00\xc4\xe1\xc1\x4e\xe6" //..S^..........N.
/* 0020 */ "\x25\x8e\x67\x8b\x58\xd9\x46\xed\x92\x3c\xfe"                     //%.g.X.F..<.

        ;

    LOG(LOG_INFO, "TEST CLIENT SIDE");

    TestTransport logtrans("test", server, sizeof(server), client, sizeof(client));
    rdpCredssp credssp(logtrans);
    credssp.hardcodedtests = true;
    credssp.credssp_client_authenticate();

}



BOOST_AUTO_TEST_CASE(TestNlaserver)
{

    const char client[65000] =
        // negotiate
/* 0000 */ "\x30\x37\xa0\x03\x02\x01\x02\xa1\x30\x30\x2e\x30\x2c\xa0\x2a\x04" //07......00.0,.*.
/* 0010 */ "\x28\x4e\x54\x4c\x4d\x53\x53\x50\x00\x01\x00\x00\x00\xb7\x82\x08" //(NTLMSSP........
/* 0020 */ "\xe2\x00\x00\x00\x00\x28\x00\x00\x00\x00\x00\x00\x00\x28\x00\x00" //.....(.......(..
/* 0030 */ "\x00\x05\x01\x28\x0a\x00\x00\x00\x0f"                             //...(.....
        // authenticate + pubauthkey
/* 0000 */ "\x30\x82\x01\x47\xa0\x03\x02\x01\x02\xa1\x82\x01\x1a\x30\x82\x01" //0..G.........0..
/* 0010 */ "\x16\x30\x82\x01\x12\xa0\x82\x01\x0e\x04\x82\x01\x0a\x4e\x54\x4c" //.0...........NTL
/* 0020 */ "\x4d\x53\x53\x50\x00\x03\x00\x00\x00\x18\x00\x18\x00\x58\x00\x00" //MSSP.........X..
/* 0030 */ "\x00\x70\x00\x70\x00\x70\x00\x00\x00\x0e\x00\x0e\x00\xe0\x00\x00" //.p.p.p..........
/* 0040 */ "\x00\x0c\x00\x0c\x00\xee\x00\x00\x00\x00\x00\x00\x00\xfa\x00\x00" //................
/* 0050 */ "\x00\x10\x00\x10\x00\xfa\x00\x00\x00\xb7\x82\x08\xe2\x05\x01\x28" //...............(
/* 0060 */ "\x0a\x00\x00\x00\x0f\x45\x7c\xb3\x01\x03\x0a\xf9\x5a\x73\xc9\x98" //.....E|.....Zs..
/* 0070 */ "\x5d\x04\xe4\xbc\x46\x37\x3f\x26\x0c\x38\xce\xe8\xf6\xc7\x83\xbb" //]...F7?&.8......
/* 0080 */ "\x72\x6d\xb1\x4d\xd8\x47\xa2\xe5\xcf\x27\xf7\x3c\x43\xdd\x39\xc4" //rm.M.G...'.<C.9.
/* 0090 */ "\x15\x92\x88\x11\x01\xc2\xbb\x69\xc6\xef\x52\x5d\x58\x01\x01\x00" //.......i..R]X...
/* 00a0 */ "\x00\x00\x00\x00\x00\xc3\x83\xa2\x1c\x6c\xb0\xcb\x01\x47\xa2\xe5" //.........l...G..
/* 00b0 */ "\xcf\x27\xf7\x3c\x43\x00\x00\x00\x00\x01\x00\x08\x00\x57\x00\x49" //.'.<C........W.I
/* 00c0 */ "\x00\x4e\x00\x37\x00\x02\x00\x08\x00\x57\x00\x49\x00\x4e\x00\x37" //.N.7.....W.I.N.7
/* 00d0 */ "\x00\x03\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x04\x00\x08" //.....w.i.n.7....
/* 00e0 */ "\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x07\x00\x08\x00\xc3\x83\xa2" //.w.i.n.7........
/* 00f0 */ "\x1c\x6c\xb0\xcb\x01\x00\x00\x00\x00\x00\x00\x00\x00\x49\x00\x74" //.l...........I.t
/* 0100 */ "\x00\x68\x00\x61\x00\x71\x00\x75\x00\x65\x00\x55\x00\x6c\x00\x79" //.h.a.q.u.e.U.l.y
/* 0110 */ "\x00\x73\x00\x73\x00\x65\x00\xdc\xed\xbe\x32\x02\x07\x8d\x54\xb3" //.s.s.e....2...T.
/* 0120 */ "\x31\x25\x8d\x65\x13\x40\x3d\xa3\x22\x04\x20\x01\x00\x00\x00\xb8" //1%.e.@=.". .....
/* 0130 */ "\x24\x73\x7b\x5d\x6b\xaa\x50\x00\x00\x00\x00\x26\x99\x46\x96\xcf" //$s{]k.P....&.F..
/* 0140 */ "\xfc\x2f\x0b\x41\xe4\x6a\x4c\x4b\xd1\x52\x82"                     //./.A.jLK.R.
        // ts credentials (authinfo)
/* 0000 */ "\x30\x5c\xa0\x03\x02\x01\x02\xa2\x55\x04\x53\x01\x00\x00\x00\xac" //0.......U.S.....
/* 0010 */ "\xfb\x32\xad\xda\xe5\xca\xe5\x01\x00\x00\x00\x9d\x85\xa1\x3f\x17" //.2............?.
/* 0020 */ "\x2b\xcf\x8f\x87\xb3\x27\xf9\xb5\x28\x33\x28\x38\x75\x9f\xdc\x0f" //+....'..(3(8u...
/* 0030 */ "\x97\xf9\x3d\xa6\x8d\x97\xe1\x52\x67\x28\xc7\x6f\x22\x39\x04\xca" //..=....Rg(.o"9..
/* 0040 */ "\xd5\x93\x54\xd6\x4d\xfa\x37\xbf\x2c\xdb\xae\x78\x2a\xaf\x45\xde" //..T.M.7.,..x*.E.
/* 0050 */ "\xfe\x7c\x19\xaa\x78\xac\x2c\x9a\x5e\x12\x87\xf1\x3b\x06"         //.|..x.,.^...;.
        ;

    const char server[65000] =
        // challenge
/* 0000 */ "\x30\x81\x88\xa0\x03\x02\x01\x02\xa1\x81\x80\x30\x7e\x30\x7c\xa0" //0..........0~0|.
/* 0010 */ "\x7a\x04\x78\x4e\x54\x4c\x4d\x53\x53\x50\x00\x02\x00\x00\x00\x00" //z.xNTLMSSP......
/* 0020 */ "\x00\x00\x00\x38\x00\x00\x00\xb7\x82\x08\xe2\x26\x6e\xcd\x75\xaa" //...8.......&n.u.
/* 0030 */ "\x41\xe7\x6f\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x40\x00\x38" //A.o........@.@.8
/* 0040 */ "\x00\x00\x00\x05\x01\x28\x0a\x00\x00\x00\x0f\x01\x00\x08\x00\x57" //.....(.........W
/* 0050 */ "\x00\x49\x00\x4e\x00\x37\x00\x02\x00\x08\x00\x57\x00\x49\x00\x4e" //.I.N.7.....W.I.N
/* 0060 */ "\x00\x37\x00\x03\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x04" //.7.....w.i.n.7..
/* 0070 */ "\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x07\x00\x08\x00\xc3" //...w.i.n.7......
/* 0080 */ "\x83\xa2\x1c\x6c\xb0\xcb\x01\x00\x00\x00\x00"                     //...l.......
        // pubauthkey
/* 0000 */ "\x30\x29\xa0\x03\x02\x01\x02\xa3\x22\x04\x20\x01\x00\x00\x00\x75" //0)......". ....u
/* 0010 */ "\xe2\x83\x53\x5e\xd5\xe0\xae\x00\x00\x00\x00\xc4\xe1\xc1\x4e\xe6" //..S^..........N.
/* 0020 */ "\x25\x8e\x67\x8b\x58\xd9\x46\xed\x92\x3c\xfe"                     //%.g.X.F..<.
        ;

    LOG(LOG_INFO, "TEST SERVER SIDE");
    TestTransport logtrans("test", client, sizeof(client), server, sizeof(server));
    rdpCredssp credssp(logtrans);
    credssp.hardcodedtests = true;
    credssp.credssp_server_authenticate();
}

