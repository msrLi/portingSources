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
 * Settings.hpp
 * by Mark Gates <mgates@nlanr.net>
 * &  Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * Stores and parses the initial values for all the global variables.
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <assert.h>
 * ------------------------------------------------------------------- */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "headers.h"
#include "Thread.h"

/* -------------------------------------------------------------------
 * constants
 * ------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* Smallest report interval supported. Units is seconds */
#define SMALLEST_INTERVAL 0.005

// server/client mode
typedef enum ThreadMode {
    kMode_Unknown = 0,
    kMode_Server,
    kMode_Client,
    kMode_Reporter,
    kMode_Listener
} ThreadMode;

// report mode
typedef enum ReportMode {
    kReport_Default = 0,
    kReport_CSV,
    //kReport_XML,
    kReport_MAXIMUM
} ReportMode;

// test mode
typedef enum TestMode {
    kTest_Normal = 0,
    kTest_DualTest,
    kTest_TradeOff,
    kTest_Unknown
} TestMode;

// rate request units
typedef enum RateUnits {
    kRate_BW = 0,
    kRate_PPS
} RateUnits;

#include "Reporter.h"

/*
 * The thread_Settings is a structure that holds all
 * options for a given execution of either a client
 * or server. By using this structure rather than
 * a global structure or class we can have multiple
 * clients or servers running with different settings.
 * In version 2.0 and above this structure contains
 * all the information needed for a thread to execute
 * and contains only C elements so it can be manipulated
 * by either C or C++.
 */
#define PEERBUFSIZE 80
typedef struct thread_Settings {
    // Pointers
    char*  mFileName;               // -F
    char*  mHost;                   // -c
    char*  mLocalhost;              // -B
    char*  mOutputFileName;         // -o
    char*  mIfrname;                 // %<device> name
    char*  mSSMMulticastStr;        // --ssm-host
    char*  mIsochronousStr;         // --isochronous
    char*  mUDPHistogramStr;        // --udp-histogram
    FILE*  Extractor_file;
    ReportHeader*  reporthdr;
    MultiHeader*   multihdr;
    struct thread_Settings *runNow;
    struct thread_Settings *runNext;
    // int's
    int mThreads;                   // -P
    int mTOS;                       // -S
#if WIN32
    SOCKET mSock;
#else
    int mSock;
#endif
#if defined(HAVE_LINUX_FILTER_H) && defined(HAVE_AF_PACKET)
    int mSockDrop;
#endif
#ifdef HAVE_UDPTRIGGERS
    int mSockIoctl;
#endif
    int Extractor_size;
    int mBufLen;                    // -l
    int mMSS;                       // -M
    int mTCPWin;                    // -w
    /*   flags is a BitMask of old bools
        bool   mBufLenSet;              // -l
        bool   mCompat;                 // -C
        bool   mDaemon;                 // -D
        bool   mDomain;                 // -V
        bool   mFileInput;              // -F or -I
        bool   mNodelay;                // -N
        bool   mPrintMSS;               // -m
        bool   mRemoveService;          // -R
        bool   mStdin;                  // -I
        bool   mStdout;                 // -o
        bool   mSuggestWin;             // -W
        bool   mUDP;                    // -u
        bool   mMode_time;
        bool   mReportSettings;
        bool   mMulticast;
        bool   mNoSettingsReport;       // -x s
        bool   mNoConnectionReport;     // -x c
        bool   mNoDataReport;           // -x d
        bool   mNoServerReport;         // -x
        bool   mNoMultReport;           // -x m
        bool   mSinlgeClient;           // -1 */
    int flags;
    int flags_extend;
    // enums (which should be special int's)
    ThreadMode mThreadMode;         // -s or -c
    ReportMode mReportMode;
    TestMode mMode;                 // -r or -d
    // Hopefully int64_t's
    max_size_t mUDPRate;            // -b or -u
    RateUnits mUDPRateUnits;        // -b is either bw or pps
    umax_size_t mAmount;             // -n or -t
    // doubles
    double mInterval;               // -i
    // shorts
    unsigned short mListenPort;     // -L
    unsigned short mPort;           // -p
    unsigned short mBindPort;      // -B
    // chars
    char   mFormat;                 // -f
    int mTTL;                    // -T
    char pad1[2];
    // structs or miscellaneous
    iperf_sockaddr peer;
    Socklen_t size_peer;
    iperf_sockaddr local;
    Socklen_t size_local;
    nthread_t mTID;
    int incrdstip;
    char* mCongestion;
    char peerversion[PEERBUFSIZE];
    int mUDPbins;
    int mUDPbinsize;
    unsigned short mUDPunits;
    unsigned short mUDPci_lower;
    unsigned short mUDPci_upper;
#if defined( HAVE_WIN32_THREAD )
    HANDLE mHandle;
#endif
#ifdef HAVE_ISOCHRONOUS
    double mFPS; //frames per second
    double mMean; //variable bit rate mean
    int mJitterBufSize; //Server jitter buffer size, units is frames
    double mBurstIPG; //Interpacket gap
#endif
    int l4offset; // used in l2 mode to offset the raw packet
    int l4payloadoffset;
    int recvflags; // used to set recv flags,e.g. MSG_TRUNC with L
    struct timeval thread_synctime;
    double mTxSyncInterval;
    double mVariance; //vbr variance
} thread_Settings;

/*
 * Thread based flags
 *
 * Due to the use of thread_Settings in C and C++
 * we are unable to use bool values. To provide
 * the functionality of bools we use the following
 * bitmask over an assumed 32 bit int. This will
 * work fine on 64bit machines we will just be ignoring
 * the upper 32bits.
 *
 * To add a flag simply define it as the next bit then
 * add the 3 support functions below.
 */
#define FLAG_BUFLENSET      0x00000001
#define FLAG_COMPAT         0x00000002
#define FLAG_DAEMON         0x00000004
#define FLAG_DOMAIN         0x00000008
#define FLAG_FILEINPUT      0x00000010
#define FLAG_NODELAY        0x00000020
#define FLAG_PRINTMSS       0x00000040
#define FLAG_REMOVESERVICE  0x00000080
#define FLAG_STDIN          0x00000100
#define FLAG_STDOUT         0x00000200
#define FLAG_SUGGESTWIN     0x00000400
#define FLAG_UDP            0x00000800
#define FLAG_MODETIME       0x00001000
#define FLAG_REPORTSETTINGS 0x00002000
#define FLAG_MULTICAST      0x00004000
#define FLAG_NOSETTREPORT   0x00008000
#define FLAG_NOCONNREPORT   0x00010000
#define FLAG_NODATAREPORT   0x00020000
#define FLAG_NOSERVREPORT   0x00040000
#define FLAG_NOMULTREPORT   0x00080000
#define FLAG_SINGLECLIENT   0x00100000
#define FLAG_SINGLEUDP      0x00200000
#define FLAG_CONGESTION     0x00400000
#define FLAG_REALTIME       0x00800000
#define FLAG_BWSET          0x01000000
#define FLAG_ENHANCEDREPORT 0x02000000
#define FLAG_SERVERMODETIME 0x04000000
#define FLAG_SSM_MULTICAST  0x08000000
/*
 * Extended flags
 */
#define FLAG_PEERVER        0x00000001
#define FLAG_SEQNO64        0x00000002
#define FLAG_REVERSE        0x00000004
#define FLAG_ISOCHRONOUS    0x00000008
#define FLAG_UDPTRIGGERS    0x00000010
#define FLAG_UDPHISTOGRAM   0x00000020
#define FLAG_L2LENGTHCHECK  0x00000100
#define FLAG_TXSYNC         0x00000200
#define FLAG_INCRDSTIP      0x00000400
#define FLAG_VARYLOAD       0x00000800


#define isBuflenSet(settings)      ((settings->flags & FLAG_BUFLENSET) != 0)
#define isCompat(settings)         ((settings->flags & FLAG_COMPAT) != 0)
#define isDaemon(settings)         ((settings->flags & FLAG_DAEMON) != 0)
#define isIPV6(settings)           ((settings->flags & FLAG_DOMAIN) != 0)
#define isFileInput(settings)      ((settings->flags & FLAG_FILEINPUT) != 0)
#define isNoDelay(settings)        ((settings->flags & FLAG_NODELAY) != 0)
#define isPrintMSS(settings)       ((settings->flags & FLAG_PRINTMSS) != 0)
#define isRemoveService(settings)  ((settings->flags & FLAG_REMOVESERVICE) != 0)
#define isSTDIN(settings)          ((settings->flags & FLAG_STDIN) != 0)
#define isSTDOUT(settings)         ((settings->flags & FLAG_STDOUT) != 0)
#define isSuggestWin(settings)     ((settings->flags & FLAG_SUGGESTWIN) != 0)
#define isUDP(settings)            ((settings->flags & FLAG_UDP) != 0)
#define isModeTime(settings)       ((settings->flags & FLAG_MODETIME) != 0)
#define isReport(settings)         ((settings->flags & FLAG_REPORTSETTINGS) != 0)
#define isMulticast(settings)      ((settings->flags & FLAG_MULTICAST) != 0)
#define isSSMMulticast(settings)   ((settings->flags & FLAG_SSM_MULTICAST) != 0)
// Active Low for Reports
#define isSettingsReport(settings) ((settings->flags & FLAG_NOSETTREPORT) == 0)
#define isConnectionReport(settings)  ((settings->flags & FLAG_NOCONNREPORT) == 0)
#define isDataReport(settings)     ((settings->flags & FLAG_NODATAREPORT) == 0)
#define isServerReport(settings)   ((settings->flags & FLAG_NOSERVREPORT) == 0)
#define isMultipleReport(settings) ((settings->flags & FLAG_NOMULTREPORT) == 0)
// end Active Low
#define isSingleClient(settings)   ((settings->flags & FLAG_SINGLECLIENT) != 0)
#define isSingleUDP(settings)      ((settings->flags & FLAG_SINGLEUDP) != 0)
#define isCongestionControl(settings) ((settings->flags & FLAG_CONGESTION) != 0)
#define isRealtime(settings)       ((settings->flags & FLAG_REALTIME) != 0)
#define isBWSet(settings)          ((settings->flags & FLAG_BWSET) != 0)
#define isEnhanced(settings)       ((settings->flags & FLAG_ENHANCEDREPORT) != 0)
#define isServerModeTime(settings) ((settings->flags & FLAG_SERVERMODETIME) != 0)
#define isPeerVerDetect(settings)  ((settings->flags_extend & FLAG_PEERVER) != 0)
#define isSeqNo64b(settings)       ((settings->flags_extend & FLAG_SEQNO64) != 0)
#define isReverse(settings)        ((settings->flags_extend & FLAG_REVERSE) != 0)
#define isIsochronous(settings)    ((settings->flags_extend & FLAG_ISOCHRONOUS) != 0)
#define isUDPTriggers(settings)    ((settings->flags_extend & FLAG_UDPTRIGGERS) != 0)
#define isUDPHistogram(settings)   ((settings->flags_extend & FLAG_UDPHISTOGRAM) != 0)
#define isL2LengthCheck(settings)  ((settings->flags_extend & FLAG_L2LENGTHCHECK) != 0)
#define isIncrDstIP(settings)       ((settings->flags_extend & FLAG_INCRDSTIP) != 0)
#define isTxSync(settings)         ((settings->flags_extend & FLAG_TXSYNC) != 0)
#define isVaryLoad(settings)         ((settings->flags_extend & FLAG_VARYLOAD) != 0)

#define setBuflenSet(settings)     settings->flags |= FLAG_BUFLENSET
#define setCompat(settings)        settings->flags |= FLAG_COMPAT
#define setDaemon(settings)        settings->flags |= FLAG_DAEMON
#define setIPV6(settings)          settings->flags |= FLAG_DOMAIN
#define setFileInput(settings)     settings->flags |= FLAG_FILEINPUT
#define setNoDelay(settings)       settings->flags |= FLAG_NODELAY
#define setPrintMSS(settings)      settings->flags |= FLAG_PRINTMSS
#define setRemoveService(settings) settings->flags |= FLAG_REMOVESERVICE
#define setSTDIN(settings)         settings->flags |= FLAG_STDIN
#define setSTDOUT(settings)        settings->flags |= FLAG_STDOUT
#define setSuggestWin(settings)    settings->flags |= FLAG_SUGGESTWIN
#define setUDP(settings)           settings->flags |= FLAG_UDP
#define setModeTime(settings)      settings->flags |= FLAG_MODETIME
#define setReport(settings)        settings->flags |= FLAG_REPORTSETTINGS
#define setMulticast(settings)     settings->flags |= FLAG_MULTICAST
#define setSSMMulticast(settings)  settings->flags |= FLAG_SSM_MULTICAST
#define setNoSettReport(settings)  settings->flags |= FLAG_NOSETTREPORT
#define setNoConnReport(settings)  settings->flags |= FLAG_NOCONNREPORT
#define setNoDataReport(settings)  settings->flags |= FLAG_NODATAREPORT
#define setNoServReport(settings)  settings->flags |= FLAG_NOSERVREPORT
#define setNoMultReport(settings)  settings->flags |= FLAG_NOMULTREPORT
#define setSingleClient(settings)  settings->flags |= FLAG_SINGLECLIENT
#define setSingleUDP(settings)     settings->flags |= FLAG_SINGLEUDP
#define setCongestionControl(settings) settings->flags |= FLAG_CONGESTION
#define setRealtime(settings)      settings->flags |= FLAG_REALTIME
#define setBWSet(settings)         settings->flags |= FLAG_BWSET
#define setEnhanced(settings)      settings->flags |= FLAG_ENHANCEDREPORT
#define setServerModeTime(settings)      settings->flags |= FLAG_SERVERMODETIME
#define setPeerVerDetect(settings) settings->flags_extend |= FLAG_PEERVER
#define setSeqNo64b(settings)      settings->flags_extend |= FLAG_SEQNO64
#define setReverse(settings)       settings->flags_extend |= FLAG_REVERSE
#define setIsochronous(settings)   settings->flags_extend |= FLAG_ISOCHRONOUS
#define setUDPTriggers(settings)   settings->flags_extend |= FLAG_UDPTRIGGERS
#define setUDPHistogram(settings)  settings->flags_extend |= FLAG_UDPHISTOGRAM
#define setL2LengthCheck(settings)    settings->flags_extend |= FLAG_L2LENGTHCHECK
#define setIncrDstIP(settings)     settings->flags_extend |= FLAG_INCRDSTIP
#define setTxSync(settings)        settings->flags_extend |= FLAG_TXSYNC
#define setVaryLoad(settings)      settings->flags_extend |= FLAG_VARYLOAD

#define unsetBuflenSet(settings)   settings->flags &= ~FLAG_BUFLENSET
#define unsetCompat(settings)      settings->flags &= ~FLAG_COMPAT
#define unsetDaemon(settings)      settings->flags &= ~FLAG_DAEMON
#define unsetIPV6(settings)        settings->flags &= ~FLAG_DOMAIN
#define unsetFileInput(settings)   settings->flags &= ~FLAG_FILEINPUT
#define unsetNoDelay(settings)     settings->flags &= ~FLAG_NODELAY
#define unsetPrintMSS(settings)    settings->flags &= ~FLAG_PRINTMSS
#define unsetRemoveService(settings)  settings->flags &= ~FLAG_REMOVESERVICE
#define unsetSTDIN(settings)       settings->flags &= ~FLAG_STDIN
#define unsetSTDOUT(settings)      settings->flags &= ~FLAG_STDOUT
#define unsetSuggestWin(settings)  settings->flags &= ~FLAG_SUGGESTWIN
#define unsetUDP(settings)         settings->flags &= ~FLAG_UDP
#define unsetModeTime(settings)    settings->flags &= ~FLAG_MODETIME
#define unsetReport(settings)      settings->flags &= ~FLAG_REPORTSETTINGS
#define unsetMulticast(settings)   settings->flags &= ~FLAG_MULTICAST
#define unsetSSMMulticast(settings)   settings->flags &= ~FLAG_SSM_MULTICAST
#define unsetNoSettReport(settings)   settings->flags &= ~FLAG_NOSETTREPORT
#define unsetNoConnReport(settings)   settings->flags &= ~FLAG_NOCONNREPORT
#define unsetNoDataReport(settings)   settings->flags &= ~FLAG_NODATAREPORT
#define unsetNoServReport(settings)   settings->flags &= ~FLAG_NOSERVREPORT
#define unsetNoMultReport(settings)   settings->flags &= ~FLAG_NOMULTREPORT
#define unsetSingleClient(settings)   settings->flags &= ~FLAG_SINGLECLIENT
#define unsetSingleUDP(settings)      settings->flags &= ~FLAG_SINGLEUDP
#define unsetCongestionControl(settings) settings->flags &= ~FLAG_CONGESTION
#define unsetRealtime(settings)    settings->flags &= ~FLAG_REALTIME
#define unsetBWSet(settings)       settings->flags &= ~FLAG_BWSET
#define unsetEnhanced(settings)    settings->flags &= ~FLAG_ENHANCEDREPORT
#define unsetServerModeTime(settings)    settings->flags &= ~FLAG_SERVERMODETIME
#define unsetPeerVerDetect(settings)    settings->flags_extend &= ~FLAG_PEERVER
#define unsetSeqNo64b(settings)    settings->flags_extend &= ~FLAG_SEQNO64
#define unsetReverse(settings)     settings->flags_extend &= ~FLAG_REVERSE
#define unsetIsochronous(settings) settings->flags_extend &= ~FLAG_ISOCHRONOUS
#define unsetUDPTriggers(settings) settings->flags_extend &= ~FLAG_UDPTRIGGERS
#define unsetUDPHistogram(settings)    settings->flags_extend &= ~FLAG_UDPHISTOGRAM
#define unsetL2LengthCheck(settings)  settings->flags_extend &= ~FLAG_L2LENGTHCHECK
#define unsetIncrDstIP(settings)   settings->flags_extend &= ~FLAG_INCRDSTIP
#define unsetTxSync(settings)      settings->flags_extend &= ~FLAG_TXSYNC
#define unsetVaryLoad(settings)      settings->flags_extend &= ~FLAG_VARYLOAD

/*
 * Message header flags
 *
 * base flags, keep compatible with older versions
 */
#define HEADER_VERSION1 0x80000000
#define HEADER_EXTEND   0x40000000
#define HEADER_UDPTESTS 0x20000000

// Below flags are used to pass test settings in *every* UDP packet
// and not just during the header exchange
#define HEADER_UDP_ISOCH    0x00000001
#define HEADER_L2ETHPIPV6   0x00000002
#define HEADER_L2LENCHECK   0x00000004
#define HEADER_UDPTRIGGERS  0x00000008

#define RUN_NOW         0x00000001
// newer flags
#define UNITS_PPS             0x00000001
#define SEQNO64B              0x00000002
#define REALTIME              0x00000004
#define REVERSE               0x00000008

// later features
#define HDRXACKMAX 2500000 // default 2.5 seconds, units microseconds
#define HDRXACKMIN   10000 // default 10 ms, units microseconds
#define MAGIC_NUMBER_TYPE 0x100 //
#define MAGIC_NUMBER_LEN   0x4
#define MAGIC_DHDHOST_TIMESTAMP 0x18EF0001 // Value used in the UDP payload to trigger the driver to insert DHD host timestamp

/*
 * Structures used for test messages which
 * are exchanged between the client and the Server/Listener
 */
typedef enum MsgType {
    CLIENTHDR = 0x1,
    CLIENTHDRACK,
    SERVERHDR,
    SERVERHDRACK
} MsgType;

/*
 * Structures below will be passed as network i/o
 * between the client, listener and server
 * and must be packed by the compilers
 * Align on 32 bits (4 bytes)
 */
#pragma pack(push,4)
typedef struct UDP_datagram {
// used to reference the 4 byte ID number we place in UDP datagrams
// use int32_t if possible, otherwise a 32 bit bitfield (e.g. on J90)
// Support 64 bit seqno on machines that support them
#ifdef HAVE_INT32_T
    u_int32_t id;
    u_int32_t tv_sec;
    u_int32_t tv_usec;
#else
    unsigned int id      : 32;
    unsigned int tv_sec  : 32;
    unsigned int tv_usec : 32;
#endif //32
#ifdef HAVE_INT32_T
    u_int32_t id2;
#else
    unsigned int id2      : 32;
#endif // 32
} UDP_datagram;

typedef struct hdr_typelen {
#ifdef HAVE_INT32_T
    int32_t type;
    int32_t length;
#else
    signed int type     : 32;
    signed int length    : 32;
#endif
} hdr_typelen;


/*
 * The client_hdr structure is sent from clients
 * to servers to alert them of things that need
 * to happen. Order must be perserved in all
 * future releases for backward compatibility.
 * 1.7 has flags, numThreads, mPort, and bufferlen
 */
typedef struct client_hdr_v1 {
#ifdef HAVE_INT32_T
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     * The lowest order diferentiates between dualtest and
     * tradeoff modes, wheither the speaker needs to start
     * immediately or after the audience finishes.
     */
    int32_t flags;
    int32_t numThreads;
    int32_t mPort;
    int32_t bufferlen;
    int32_t mWinBand;
    int32_t mAmount;
#else
    signed int flags      : 32;
    signed int numThreads : 32;
    signed int mPort      : 32;
    signed int bufferlen  : 32;
    signed int mWinBand : 32;
    signed int mAmount    : 32;
#endif
} client_hdr_v1;

// This is used for tests that require
// the initial handshake
typedef struct client_hdrext {
    hdr_typelen typelen;
#ifdef HAVE_INT32_T
    int32_t flags;
    int32_t version_u;
    int32_t version_l;
    int32_t reserved;
    int32_t mRate;
    int32_t mUDPRateUnits;
    int32_t mRealtime;
#else
    signed int flags       : 32;
    signed int version_u   : 32;
    signed int version_l   : 32;
    signed int reserved    : 32;
    signed int mRate      : 32;
    signed int mUDPRateUnits : 32;
    signed int mRealtime  : 32;
#endif
} client_hdrext;


/*
 * Isoch payload structure
 *
 *                 0      7 8     15 16    23 24    31
 *                +--------+--------+--------+--------+
 *      0x00  1   |          seqno lower              |
 *                +--------+--------+--------+--------+
 *      0x04  2   |             tv_sec                |
 *                +--------+--------+--------+--------+
 *      0x08  3   |             tv_usec               |
 *                +--------+--------+--------+--------+
 *      0x0c  4   |    (reserved) seqno upper         |
 *                +--------+--------+--------+--------+
 *            5   |         v1 hdr                    |
 *                +--------+--------+--------+--------+
 *            6   |         v1 hdr (continued)        |
 *                +--------+--------+--------+--------+
 *            7   |         v1 hdr (continued)        |
 *                +--------+--------+--------+--------+
 *            8   |         v1 hdr (continued)        |
 *                +--------+--------+--------+--------+
 *            9   |         v1 hdr (continued)        |
 *                +--------+--------+--------+--------+
 *            10  |         v1 hdr (final)            |
 *                +--------+--------+--------+--------+
 *            11  | udp test flags  | tlv offset      |
 *                +--------+--------+--------+--------+
 *            12  |        iperf version major        |
 *                +--------+--------+--------+--------+
 *            13  |        iperf version minor        |
 *                +--------+--------+--------+--------+
 *            14  |        ref sync sample tv_sec     |
 *                +--------+--------+--------+--------+
 *            15  |        ref sync sample tv_used    |
 *                +--------+--------+--------+--------+
 *            16  |        gps sync sample tv_sec     |
 *                +--------+--------+--------+--------+
 *            17  |        gps sync sample tv_usec    |
 *                +--------+--------+--------+--------+
 *            18  |        isoch burst period (us)    |
 *                +--------+--------+--------+--------+
 *            19  |        isoch start timestamp (s)  |
 *                +--------+--------+--------+--------+
 *            20  |        isoch start timestamp (us) |
 *                +--------+--------+--------+--------+
 *            21  |        isoch prev frameid         |
 *                +--------+--------+--------+--------+
 *            22  |        isoch frameid              |
 *                +--------+--------+--------+--------+
 *            23  |        isoch burtsize             |
 *                +--------+--------+--------+--------+
 *            24  |        isoch bytes remaining      |
 *                +--------+--------+--------+--------+
 *            25  |        isoch reserved             |
 *                +--------+--------+--------+--------+
 *            26  |        hw timestamps ...          |
 *                +--------+--------+--------+--------+
 *            n   |        hw timestamps ...
 *                +--------+--------+--------+--------+
 *
 */

typedef struct UDP_isoch_payload {
#ifdef HAVE_INT32_T
    u_int32_t burstperiod; //period units microseconds
    u_int32_t start_tv_sec;
    u_int32_t start_tv_usec;
    u_int32_t prevframeid;
    u_int32_t frameid;
    u_int32_t burstsize;
    u_int32_t remaining;
    u_int32_t resevered;
#else
    unsigned int burstperiod : 32;
    unsigned int start_tv_sec : 32;
    unsigned int start_tv_usec : 32;
    unsigned int prevframeid : 32;
    unsigned int frameid : 32;
    unsigned int burstsize : 32;
    unsigned int remaining : 32;
    unsigned int reserved : 32;
#endif
} UDP_isoch_payload;

// This is used for UDP tests that don't
// require any handshake, i.e they are stateless
typedef struct client_hdr_udp_tests {
// for 32 bit systems, skip over this field
// so it remains interoperable with 64 bit peers
#ifdef HAVE_INT32_T
    u_int16_t testflags;
    u_int16_t tlvoffset;
    u_int32_t version_u;
    u_int32_t version_l;
    u_int32_t ref_sync_tv_sec;
    u_int32_t ref_sync_tv_nsec;
    u_int32_t gps_sync_tv_sec;
    u_int32_t gps_sync_tv_nsec;
#else
    unsigned short testflags   : 16;
    unsigned short tlvoffset   : 16;
    unsigned int version_u   : 32;
    unsigned int version_l   : 32;
    unsigned int ref_sync_tv_sec : 32;
    unsigned int ref_sync_tv_nsec : 32;
    unsigned int gps_sync_tv_sec : 32;
    unsigned int gps_sync_tv_nsec : 32;
#endif
} client_hdr_udp_tests;

#ifdef HAVE_UDPTRIGGERS
/*
 *   UDP FW inline timestamps
 *
 *                 0      7 8     15 16    23 24    31
 *                +--------+--------+--------+--------+
 *            1   |  type (0x100)  |   length (68)    |   length including the 4 bytes tlv
 *                +--------+--------+--------+--------+
 *            2   |             host tx tv sec        |
 *                +--------+--------+--------+--------+
 *            3   |             host tx tv usec       |
 *                +--------+--------+--------+--------+
 *            4   |             host rx tv sec        |
 *                +--------+--------+--------+--------+
 *            5   |             host rx tv usec       |
 *                +--------+--------+--------+--------+
 *            6   |     type (0x1)  |      cnt (2)    |    fw rx tlv
 *                +--------+--------+--------+--------+
 *            7   |        fw rx ts 1 (mac)           |
 *                +--------+--------+--------+--------+
 *            8   |        fw rx ts 2 (pcie)          |
 *                +--------+--------+--------+--------+
 *            9   |     type (0x2)  |      cnt (1)    |    fw tx tlv
 *                +--------+--------+--------+--------+
 *            10  |        seqno lower                |
 *                +--------+--------+--------+--------+
 *            11  |        iperf tv_sec               |
 *                +--------+--------+--------+--------+
 *            12  |        iperf tv_usec              |
 *                +--------+--------+--------+--------+
 *            13  |        seqno upper ??             |
 *                +--------+--------+--------+--------+
 *            14  |        fw tx ts 1  pcie           |
 *                +--------+--------+--------+--------+
 *            15  |        fw tx ts 2  tx dma         |
 *                +--------+--------+--------+--------+
 *            16  |        fw tx ts 3  tx status      |
 *                +--------+--------+--------+--------+
 *            17  |        fw tx ts 4  pcie rt        |
 *                +--------+--------+--------+--------+
 *
 * TSF histograms
 * DHDRX-DHDTX 'DRRx-DRTx;'
 * hs1 = 14,8 T6 "FWR2-FWT1"
 * hs2 = 15,7 T5 "FWR1-FWT2'
 * hs3 = 14,17 T4 'FWT4-FWT1'
 * hs4 = 15,16 T3 'FWT3-FWT2'
 * hs5 = 7,8 T2 'FWR2-FWR1'
 */
typedef struct fwtsfrx_t {
    u_int16_t fwtype;
    u_int16_t fwnct;
    u_int32_t tsf_rxmac;
    u_int32_t tsf_rxpcie;
} fwtsfrx_t;

typedef struct fwtsftx_t {
    struct UDP_datagram udpid;
    u_int32_t tsf_txpcie;
    u_int32_t tsf_txdma;
    u_int32_t tsf_txstatus;
    u_int32_t tsf_txpciert;
} fwtsftx_t;

typedef struct UDPTriggers {
    u_int16_t type;
    u_int16_t length;
    u_int32_t hosttx_tv_sec;
    u_int32_t hosttx_tv_usec;
    u_int32_t hostrx_tv_sec;
    u_int32_t hostrx_tv_usec;
    struct fwtsfrx_t fwtsf_rx;
    u_int16_t fwtsf_type;
    u_int16_t fwtsf_cnt;
    struct fwtsftx_t fwtsf_tx[];
} UDPTriggers;
#endif //UDPTRIGGERS


typedef struct client_hdr_udp_isoch_tests {
    client_hdr_udp_tests udptests;
    UDP_isoch_payload isoch;
} client_hdr_udp_isoch_tests;

typedef struct client_hdr_ack {
    hdr_typelen typelen;
#ifdef HAVE_INT32_T
    int32_t flags;
    int32_t version_u;
    int32_t version_l;
    int32_t reserved;
#else
    signed int flags    : 32;
    signed int version_u   : 32;
    signed int version_l   : 32;
    signed int reserved1   : 32;
    signed int reserved2   : 32;
#endif
} client_hdr_ack;

typedef struct client_hdr {
    client_hdr_v1 base;
    union {
	client_hdrext extend;
	client_hdr_udp_tests udp;
    };
} client_hdr;

/*
 * The server_hdr structure facilitates the server
 * report of jitter and loss on the client side.
 * It piggy_backs on the existing clear to close
 * packet.
 */
typedef struct server_hdr_v1 {
#ifdef HAVE_INT32_T
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     */
    int32_t flags;
    int32_t total_len1;
    int32_t total_len2;
    int32_t stop_sec;
    int32_t stop_usec;
    int32_t error_cnt;
    int32_t outorder_cnt;
    int32_t datagrams;
#ifdef HAVE_SEQNO64b
    int32_t datagrams2;
#endif // SEQ
    int32_t jitter1;
    int32_t jitter2;
#else // Int32
    signed int flags        : 32;
    signed int total_len1   : 32;
    signed int total_len2   : 32;
    signed int stop_sec     : 32;
    signed int stop_usec    : 32;
    signed int error_cnt    : 32;
    signed int outorder_cnt : 32;
    signed int datagrams    : 32;
#ifdef HAVE_SEQNO64b
    signed int datagrams2   : 32;
#endif // SEQ
    signed int jitter1      : 32;
    signed int jitter2      : 32;
#endif
} server_hdr_v1;

typedef struct server_hdr_extension {
#ifdef HAVE_INT32_T
    int32_t minTransit1;
    int32_t minTransit2;
    int32_t maxTransit1;
    int32_t maxTransit2;
    int32_t sumTransit1;
    int32_t sumTransit2;
    int32_t meanTransit1;
    int32_t meanTransit2;
    int32_t m2Transit1;
    int32_t m2Transit2;
    int32_t vdTransit1;
    int32_t vdTransit2;
    int32_t cntTransit;
    int32_t IPGcnt;
    int32_t IPGsum;
#else
    signed int minTransit1  : 32;
    signed int minTransit2  : 32;
    signed int maxTransit1  : 32;
    signed int maxTransit2  : 32;
    signed int sumTransit1  : 32;
    signed int sumTransit2  : 32;
    signed int meanTransit1  : 32;
    signed int meanTransit2  : 32;
    signed int m2Transit1  : 32;
    signed int m2Transit2  : 32;
    signed int vdTransit1  : 32;
    signed int vdTransit2  : 32;
    signed int cntTransit   : 32;
    signed int IPGcnt       : 32;
    signed int IPGsum       : 32;
#endif
} server_hdr_extension;

typedef struct server_hdr {
    server_hdr_v1 base;
    server_hdr_extension extend;
} server_hdr;

#pragma pack(pop)

#define SIZEOF_UDPCLIENTMSG (sizeof(client_hdr) + sizeof(UDP_datagram))
#define SIZEOF_TCPHDRMSG (int) ((sizeof(client_hdr) > sizeof(server_hdr)) ? (int) sizeof(client_hdr) : (int) sizeof(server_hdr))
#define SIZEOF_UDPHDRMSG (int) ((SIZEOF_UDPCLIENTMSG > sizeof(server_hdr)) ? SIZEOF_UDPCLIENTMSG : sizeof(server_hdr))
#define SIZEOF_MAXHDRMSG (int) ((SIZEOF_TCPHDRMSG > SIZEOF_UDPHDRMSG) ? SIZEOF_TCPHDRMSG : SIZEOF_UDPHDRMSG)

// set to defaults
void Settings_Initialize( thread_Settings* main );

// copy structure
void Settings_Copy( thread_Settings* from, thread_Settings** into );

// free associated memory
void Settings_Destroy( thread_Settings *mSettings );

// parse settings from user's environment variables
void Settings_ParseEnvironment( thread_Settings *mSettings );

// parse settings from app's command line
void Settings_ParseCommandLine( int argc, char **argv, thread_Settings *mSettings );

// convert to lower case for [KMG]bits/sec
void Settings_GetLowerCaseArg(const char *,char *);

// convert to upper case for [KMG]bytes/sec
void Settings_GetUpperCaseArg(const char *,char *);

// generate settings for listener instance
void Settings_GenerateListenerSettings( thread_Settings *client, thread_Settings **listener);

// generate settings for speaker instance
void Settings_GenerateClientSettings( thread_Settings *server,
				      thread_Settings **client,
                                      client_hdr *hdr );

// generate client header for server
int Settings_GenerateClientHdr( thread_Settings *client, client_hdr *hdr );

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif // SETTINGS_H
