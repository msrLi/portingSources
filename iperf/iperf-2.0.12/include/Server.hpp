/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * Server.hpp
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * A server thread is initiated for each connection accept() returns.
 * Handles sending and receiving data, and then closes socket.
 * ------------------------------------------------------------------- */

#ifndef SERVER_H
#define SERVER_H


#include "Settings.hpp"
#include "util.h"
#include "Timestamp.hpp"



/* ------------------------------------------------------------------- */
class Server {
public:
    // stores server socket, port and TCP/UDP mode
    Server( thread_Settings *inSettings );

    // destroy the server object
    ~Server();

    // accepts connection and receives data
    void RunUDP ( void );
    void RunTCP ( void );

    void write_UDP_AckFIN( );

    static void Sig_Int( int inSigno );

private:
    thread_Settings *mSettings;
    char* mBuf;
    Timestamp mEndTime;
    Timestamp now;
    ReportStruct *reportstruct;

    void InitTimeStamping (void);
    void InitTrafficLoop (void);
    int ReadWithRxTimestamp (int *readerr);
    bool ReadPacketID (void);
    void L2_processing (void);
    int L2_quintuple_filter (void);
    void Isoch_processing (void);
    void UDPTriggers_processing (void);

#if HAVE_DECL_SO_TIMESTAMP
    // Structures needed for recvmsg
    // Use to get kernel timestamps of packets
    struct sockaddr_storage srcaddr;
    struct iovec iov[1];
    struct msghdr message;
    char ctrl[CMSG_SPACE(sizeof(struct timeval))];
    struct cmsghdr *cmsg;
#endif
#if defined(HAVE_LINUX_FILTER_H) && defined(HAVE_AF_PACKET)
    struct ether_header *eth_hdr;
    struct iphdr *ip_hdr;
    struct udphdr *udp_hdr;
#endif
#ifdef HAVE_UDPTRIGGERS
    uint16_t packetidhash (int64_t);
    struct fwtsf_entry_t {
	bool free;
	int64_t packetID;
	uint32_t fwrxts1;
	uint32_t fwrxts2;
    };
#define HASHTABLESIZE (1 << 16)
    fwtsf_entry_t fwtsf_hashtable[HASHTABLESIZE];
#endif
}; // end class Server

#endif // SERVER_H
