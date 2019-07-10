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
 * Reporter.h
 * by Kevin Gibbs <kgibbs@nlanr.net>
 *
 * Since version 2.0 this handles all reporting.
 * ________________________________________________________________ */

#ifndef REPORTER_H
#define REPORTER_H

#include "headers.h"
#include "Mutex.h"
#include "histogram.h"
#ifdef HAVE_UDPTRIGGERS
#include "ioctls.h"
#endif

struct thread_Settings;
struct server_hdr;

#include "Settings.hpp"

#define NUM_REPORT_STRUCTS 10000
#define NUM_MULTI_SLOTS    5
// If the minimum latency exceeds the boundaries below
// assume the clocks are not synched and suppress the
// latency output. Units are seconds
#define UNREALISTIC_LATENCYMINMIN -1
#define UNREALISTIC_LATENCYMINMAX 60

#ifdef __cplusplus
extern "C" {
#endif

/*
 *
 * Used for end/end latency measurements
 *
 */
typedef struct TransitStats {
    double maxTransit;
    double minTransit;
    double sumTransit;
    double lastTransit;
    double meanTransit;
    double m2Transit;
    double vdTransit;
    int cntTransit;
    double totmaxTransit;
    double totminTransit;
    double totsumTransit;
    int totcntTransit;
    double totmeanTransit;
    double totm2Transit;
    double totvdTransit;
} TransitStats;

typedef struct ReadStats {
    int cntRead;
    int totcntRead;
    int bins[8];
    int totbins[8];
    int binsize;
} ReadStats;

typedef struct WriteStats {
    int WriteCnt;
    int WriteErr;
    int TCPretry;
    int totWriteCnt;
    int totWriteErr;
    int totTCPretry;
    int lastTCPretry;
    int cwnd;
    int rtt;
} WriteStats;

#ifdef HAVE_ISOCHRONOUS
typedef struct IsochStats {
    int mFPS; //frames per second
    double mMean; //variable bit rate mean
    double mVariance; //vbr variance
    int mJitterBufSize; //Server jitter buffer size, units is frames
    max_size_t slipcnt;
    max_size_t framecnt;
    max_size_t framelostcnt;
    unsigned int mBurstInterval;
    unsigned int mBurstIPG; //IPG of packets within the burst
    int frameID;
} IsochStats;
#endif

/*
 * This struct contains all important information from the sending or
 * recieving thread.
 */
#define L2UNKNOWN  0x01
#define L2LENERR   0x02
#define L2CSUMERR  0x04

typedef struct L2Stats {
    max_size_t cnt;
    max_size_t unknown;
    max_size_t udpcsumerr;
    max_size_t lengtherr;
    max_size_t tot_cnt;
    max_size_t tot_unknown;
    max_size_t tot_udpcsumerr;
    max_size_t tot_lengtherr;
} L2Stats;


#ifdef HAVE_UDPTRIGGERS
/*
 *  FW timestamps are broken accross packets, the rx comes first
 *  then the tx
 *
 *
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
 *            14  |        fw tx ts 1 pcie            |
 *                +--------+--------+--------+--------+
 *            15  |        fw tx ts 2 tx dma          |
 *                +--------+--------+--------+--------+
 *            16  |        fw tx ts 3 tx status       |
 *                +--------+--------+--------+--------+
 *            17  |        fw tx ts 4 pcie rt         |
 *                +--------+--------+--------+--------+
 *
 * TSF histograms
 * hs1 = 14,8
 * hs2 = 15,7
 * hs3 = 14,17
 * hs4 = 15,16
 * hs5 = 7,8
 */

typedef struct fwtsf_report_entry_t {
    u_int32_t tsf_rxmac; // 7
    u_int32_t tsf_rxpcie; // 8
    u_int32_t tsf_txpcie; // 14
    u_int32_t tsf_txdma; // 15
    u_int32_t tsf_txstatus; // 16
    u_int32_t tsf_txpciert; // 17
} fwtsf_report_entry_t;
#endif

typedef struct ReportStruct {
    max_size_t packetID;
    umax_size_t packetLen;
    struct timeval packetTime;
    struct timeval sentTime;
    int errwrite;
    int emptyreport;
    int socket;
    int l2errors;
    int l2len;
    int expected_l2len;
#ifdef HAVE_ISOCHRONOUS
    struct timeval isochStartTime;
    max_size_t prevframeID;
    max_size_t frameID;
    max_size_t burstsize;
    max_size_t burstperiod;
    max_size_t remaining;
#endif
#ifdef HAVE_UDPTRIGGERS
#define MAXTSFCHAIN 1470/32
    struct timeval hostTxTime;
    struct timeval hostRxTime;
    struct timespec ref_sync;
    struct timespec gps_sync;
    bool hashcollision;
    int tsfcount;
    struct fwtsf_report_entry_t tsf[MAXTSFCHAIN];
#endif
} ReportStruct;

/*
 * The type field of ReporterData is a bitmask
 * with one or more of the following
 */
#define    TRANSFER_REPORT      0x00000001
#define    SERVER_RELAY_REPORT  0x00000002
#define    SETTINGS_REPORT      0x00000004
#define    CONNECTION_REPORT    0x00000008
#define    MULTIPLE_REPORT      0x00000010

typedef union {
    ReadStats read;
    WriteStats write;
} SendReadStats;

typedef struct Transfer_Info {
    void *reserved_delay;
    int transferID;
    int groupID;
    max_size_t cntError;
    max_size_t cntOutofOrder;
    max_size_t cntDatagrams;
    max_size_t IPGcnt;
    int socket;
    TransitStats transit;
    SendReadStats sock_callstats;
    // Hopefully int64_t's
    umax_size_t TotalLen;
    double jitter;
    double startTime;
    double endTime;
    double IPGsum;
    // chars
    char   mFormat;                 // -f
    char   mEnhanced;               // -e
    u_char mTTL;                    // -T
    char   mUDP;
    char   mTCP;
    char   free;
    histogram_t *latency_histogram;
    L2Stats l2counts;
#ifdef HAVE_ISOCHRONOUS
    IsochStats isochstats;
    char   mIsochronous;                 // -e
    TransitStats frame;
    histogram_t *framelatency_histogram;
#endif
#ifdef HAVE_UDPTRIGGERS
    histogram_t *hostlatency_histogram;
    histogram_t *h1_histogram;
    histogram_t *h2_histogram;
    histogram_t *h3_histogram;
    histogram_t *h4_histogram;
    histogram_t *h5_histogram;
    histogram_t *h6_histogram;
    /*
     * u_int32_t tsf_rxmac   7
     * u_int32_t tsf_rxpcie  8
     * u_int32_t tsf_txpcie  14
     * u_int32_t tsf_txdma   15
     * u_int32_t tsf_txstatus 16
     * u_int32_t tsf_txpciert  17
     */
    struct tsftv_t tsftv_rxpcie;
    struct tsftv_t tsftv_rxmac;
    struct tsftv_t tsftv_txpcie;
    struct tsftv_t tsftv_txpciert;
    struct tsftv_t tsftv_txdma;
    struct tsftv_t tsftv_txstatus;
#endif
} Transfer_Info;

typedef struct Connection_Info {
    iperf_sockaddr peer;
    Socklen_t size_peer;
    iperf_sockaddr local;
    Socklen_t size_local;
    char *peerversion;
    int l2mode;
} Connection_Info;

typedef struct ReporterData {
    char*  mHost;                   // -c
    char*  mLocalhost;              // -B
    char*  mIfrname;
    char*  mSSMMulticastStr;

    // int's
    int type;
    max_size_t cntError;
    max_size_t lastError;
    max_size_t cntOutofOrder;
    max_size_t lastOutofOrder;
    max_size_t cntDatagrams;
    max_size_t lastDatagrams;
    max_size_t PacketID;

    int mBufLen;                    // -l
    int mMSS;                       // -M
    int mTCPWin;                    // -w
    max_size_t mUDPRate;            // -b or -u
    RateUnits mUDPRateUnits;        // -b is either bw or pps
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
        bool   mUDP;
        bool   mMode_time;*/
    int flags;
    int flags_extend;
    // enums (which should be special int's)
    ThreadMode mThreadMode;         // -s or -c
    ReportMode mode;
    umax_size_t TotalLen;
    umax_size_t lastTotal;
    // doubles
    // shorts
    unsigned short mPort;           // -p
    // structs or miscellaneous
    Transfer_Info info;
    Connection_Info connection;
    struct timeval startTime;
    struct timeval packetTime;
    struct timeval nextTime;
    struct timeval intervalTime;
    struct timeval IPGstart;
#ifdef HAVE_ISOCHRONOUS
    IsochStats isochstats;
#endif
    double TxSyncInterval;
} ReporterData;

typedef struct MultiHeader {
    int reporterindex;
    int agentindex;
    int groupID;
    int threads;
    ReporterData *report;
    Transfer_Info *data;
    Condition barrier;
    struct timeval startTime;
} MultiHeader;

typedef struct ReportHeader {
    int reporterindex;
    int agentindex;
    ReporterData report;
    ReportStruct *data;
    MultiHeader *multireport;
    struct ReportHeader *next;
} ReportHeader;

typedef void* (* report_connection)( Connection_Info*, int );
typedef void (* report_settings)( ReporterData* );
typedef void (* report_statistics)( Transfer_Info* );
typedef void (* report_serverstatistics)( Connection_Info*, Transfer_Info* );

MultiHeader* InitMulti( struct thread_Settings *agent, int inID );
ReportHeader* InitReport( struct thread_Settings *agent );
void ReportPacket( ReportHeader *agent, ReportStruct *packet );
void CloseReport( ReportHeader *agent, ReportStruct *packet );
void EndReport( ReportHeader *agent );
Transfer_Info* GetReport( ReportHeader *agent );
void ReportServerUDP( struct thread_Settings *agent, struct server_hdr *server );
void ReportSettings( struct thread_Settings *agent );
void ReportConnections( struct thread_Settings *agent );
void reporter_peerversion (struct thread_Settings *inSettings, int upper, int lower);

extern report_connection connection_reports[];

extern report_settings settings_reports[];

extern report_statistics statistics_reports[];

extern report_serverstatistics serverstatistics_reports[];

extern report_statistics multiple_reports[];

#define SNBUFFERSIZE 120
extern char buffer[SNBUFFERSIZE]; // Buffer for printing

#define rMillion 1000000

#define TimeDifference( left, right ) (left.tv_sec  - right.tv_sec) +   \
        (left.tv_usec - right.tv_usec) / ((double) rMillion)

#define TimeAdd( left, right )  do {                                    \
                                    left.tv_usec += right.tv_usec;      \
                                    if ( left.tv_usec > rMillion ) {    \
                                        left.tv_usec -= rMillion;       \
                                        left.tv_sec++;                  \
                                    }                                   \
                                    left.tv_sec += right.tv_sec;        \
                                } while ( 0 )
#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif // REPORTER_H
