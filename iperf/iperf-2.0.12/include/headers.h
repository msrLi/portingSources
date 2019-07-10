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
 * headers.h
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * All system headers required by iperf.
 * This could be processed to form a single precompiled header,
 * to avoid overhead of compiling it multiple times.
 * This also verifies a few things are defined, for portability.
 * ------------------------------------------------------------------- */

#ifndef HEADERS_H
#define HEADERS_H


#ifdef HAVE_CONFIG_H
    #include "config.h"

/* OSF1 (at least the system I use) needs extern C
 * around the <netdb.h> and <arpa/inet.h> files. */
    #if defined( SPECIAL_OSF1_EXTERN ) && defined( __cplusplus )
        #define SPECIAL_OSF1_EXTERN_C_START extern "C" {
        #define SPECIAL_OSF1_EXTERN_C_STOP  }
    #else
        #define SPECIAL_OSF1_EXTERN_C_START
        #define SPECIAL_OSF1_EXTERN_C_STOP
    #endif
#endif /* HAVE_CONFIG_H */

/* turn off assert debugging */
#define NDEBUG

/* standard C headers */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <sys/types.h>
#include <fcntl.h>
#include <inttypes.h>

// AF_PACKET HEADERS
#if defined(HAVE_LINUX_FILTER_H) && defined(HAVE_AF_PACKET)
#include <net/ethernet.h>
// Bummer, AF_PACKET requires kernel headers as <netpacket/packet.h> isn't sufficient
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
// In older linux kernels, the kernel headers and glibc headers are in conflict,
// specificially <linux/in6.h> and <netinit/in.h>
// preventing the following include:
//   #include <linux/ipv6.h> (which includes <linux/in6.h>)
//   and the use of sizeof(struct ipv6hdr)
//
// See https://patchwork.ozlabs.org/patch/631400/
//
// "In upstream kernel 56c176c9 the _UAPI prefix was stripped and
// this broke our synchronized headers again. We now need to check
// for _LINUX_IN6_H and _IPV6_H, and keep checking the old versions
// of the header guard checks for maximum backwards compatibility
// with older Linux headers (the history is actually a bit muddled
// here and it appears upstream linus kernel broke this 10 months
// *before* our fix was ever applied to glibc, but without glibc
// testing we didn't notice and distro kernels have their own
// testing to fix this)."
//
// The use cases fixed by this patch are:
// #include <linux/in6.h>
// #include <netinet/in.h>
//
// force the ipv6hdr length to 40 vs use of sizeof(struct ipv6hdr)
#define  IPV6HDRLEN 40
#endif // HAVE_AF_PACKET

#ifdef WIN32

/* Windows config file */
//    #include "config.win32.h"

/* Windows headers */
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
    #define _WIN32_WINNT 0x0501 /* use (at least) WinXP API */
    #define WIN32_LEAN_AND_MEAN /* exclude unnecesary headers */
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <process.h>

/* define EINTR, just to help compile; it isn't useful */
    #ifndef EINTR
        #define EINTR  WSAEINTR
    #endif // EINTR

/* Visual C++ has INT64, but not 'long long'.
 * Metrowerks has 'long long', but INT64 doesn't work. */
    #ifdef __MWERKS__
        #define int64_t  long long
    #else
        #define int64_t  INT64
    #endif // __MWERKS__

/* Visual C++ has _snprintf instead of snprintf */
    #ifndef __MWERKS__
        #define snprintf _snprintf
    #endif // __MWERKS__

/* close, read, and write only work on files in Windows.
 * I get away with #defining them because I don't read files. */
    #define close( s )       closesocket( s )
    #define read( s, b, l )  recv( s, (char*) b, l, 0 )
    #define write( s, b, l ) send( s, (char*) b, l, 0 )

/* usleep is in unistd.h, but that would conflict with the
close/read/write redefinitin above */
int usleep(useconds_t usec);

#else /* not defined WIN32 */

/* used in Win32 for error checking,
 * rather than checking rc < 0 as unix usually does */
#define SOCKET_ERROR   -1
#define INVALID_SOCKET -1

#include <unistd.h>

#endif /* not defined WIN32 */

/* required on AIX for FD_SET (requires bzero).
 * often this is the same as <string.h> */
    #ifdef HAVE_STRINGS_H
        #include <strings.h>
    #endif // HAVE_STRINGS_H

/* unix headers */
#ifdef HAVE_SYS_SOCKET_H
    #include <sys/socket.h>
#endif
    #include <sys/time.h>
#ifdef HAVE_SIGNAL_H
    #include <signal.h>
#endif

#ifdef HAVE_SYSLOG_H
/** Added for daemonizing the process */
    #include <syslog.h>
#endif

#ifdef HAVE_NETDB_H
SPECIAL_OSF1_EXTERN_C_START
    #include <netdb.h>
SPECIAL_OSF1_EXTERN_C_STOP
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#include <netinet/tcp.h>
SPECIAL_OSF1_EXTERN_C_START
    #include <arpa/inet.h>   /* netinet/in.h must be before this on SunOS */
SPECIAL_OSF1_EXTERN_C_STOP
#endif



#ifdef HAVE_POSIX_THREAD
#include <pthread.h>
#endif // HAVE_POSIX_THREAD

#ifndef INET6_ADDRSTRLEN
    #define INET6_ADDRSTRLEN 40
#endif
#ifndef INET_ADDRSTRLEN
    #define INET_ADDRSTRLEN 15
#endif

//#ifdef __cplusplus
    #ifdef HAVE_IPV6
        #define REPORT_ADDRLEN (INET6_ADDRSTRLEN + 1)
typedef struct sockaddr_storage iperf_sockaddr;
    #else
        #define REPORT_ADDRLEN (INET_ADDRSTRLEN + 1)
typedef struct sockaddr_in iperf_sockaddr;
    #endif
//#endif

// Rationalize stdint definitions and sizeof, thanks to ac_create_stdint_h.m4
// from the gnu archive

#include <iperf-int.h>
// Override <stdint.h> PRIdMAX (hack for now, fix this to use <stdint.h> properly)
#ifdef HAVE_QUAD_SUPPORT
  #ifdef WIN32
    #define IPERFdMAX "I64d"
  #elif defined HAVE_PRINTF_QD
    #define IPERFdMAX "qd"
  #else
    #define IPERFdMAX "lld"
  #endif
#else
  #define IPERFdMAX "d"
#endif

#ifdef HAVE_QUAD_SUPPORT
#  ifdef HAVE_INT64_T
typedef int64_t max_size_t;
typedef u_int64_t umax_size_t;
#  else
typedef long long max_size_t;
typedef unsigned long long umax_size_t;
#  endif // INT64
#else
#  ifdef HAVE_INT32_T
typedef int32_t max_size_t;
typedef u_int32_t umax_size_t;
#  else
typedef long max_size_t;
typedef unsigned long umax_size_t;
#  endif // INT32
#endif

/* in case the OS doesn't have these, we provide our own implementations */
#include "gettimeofday.h"
#include "inet_aton.h"
#include "snprintf.h"

#ifndef SHUT_RD
    #define SHUT_RD   0
    #define SHUT_WR   1
    #define SHUT_RDWR 2
#endif // SHUT_RD

#endif /* HEADERS_H */
