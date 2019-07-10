/**
 * @file iftosdusr.h
 * @brief
 * @author jerry.king
 * @date 2012-12-21 Created
**/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
******************************************************************/

#ifndef IFTOSDUSR_H_
#define IFTOSDUSR_H_
#include "iftsysTypes.h"
#include "iftOsdService.h"
#include "iftosd.h"

/**
 * set osd config init
 */
typedef struct IftEnv
{
    int32_t dbgLevel; /**< osdfont debug level*/
    int32_t version; /**< osdfont version*/
} IftEnv;

typedef struct _SOsdUser
{
    int32_t uid;
} SOsdUser;

////////////////////////////////////////////////////////////////////////////
typedef struct  _SOsdUser *OsdUser_H ;
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_setEnv(IftEnv *iftenv);
///**
// * @brief Set osdfont env
// * @param[in] iftenv The obj of iftosd set.
// * @return  >=0 return ok ; < 0 return false .
// */
//int32_t iftosd_init();
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
OsdUser_H iftosd_regUsr();

int32_t iftosd_defStyle(OsdUser_H usr, iftOsdServStyle * servStyle);

int32_t iftosd_overlyOneFrame(OsdUser_H usr, iftOsdText *fstru, SFrameData *oneFrame);

/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_unregUsr(OsdUser_H *pusr);
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_destroy();


#endif /* IFTOSDUSR_H_ */
