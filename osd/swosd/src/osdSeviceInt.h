/**
 * @file osdSeviceInt.h
 * @brief
 * @author jerry.king
 * @date 2012-12-12 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#ifndef OSDSEVICEINT_H_
#define OSDSEVICEINT_H_

#include "iftOsdService.h"
#define MAX_OSD_USERCNT 3

typedef struct SProcMeta
{
    iftCanvasLayout *comm;
    int8_t *pdata; //canvasLayout
    iftFtgen *genctx;
    struct  SProcMeta * next;
} SProcMeta;

typedef struct  SOsdUserInt
{
    SOsdUser super;
    SProcMeta meta;
    iftRange usrRange;
    iftRange innerFrameProcessRange;;
    void* grayframpool ;
} SOsdUserInt;

/**
 * create uid
 */
SOsdUserInt* iftOsdServ_usrCreate(int32_t *uid);
int iftOsdServ_usrInit(SOsdUserInt *usr, iftRange *rgpools) ;
int32_t iftOsdServ_userDeinit(SOsdUserInt *usr);
/**
 * destroy
 */
int32_t iftOsdServ_usrDestory(SOsdUserInt *usr);

//int32_t genFontsImg(SOsdUser* usr, SProcMeta *pmata);

//int32_t fontOverlying(SOsdUser * user, int32_t pixWidth, int32_t pixHight, int32_t type, int8_t* data);
int32_t iftOsdServ_overlyingOneFrame(SOsdUserInt * usr, iftOsdText *fstru, SFrameData *oneFrame, E_CHAR_SET charset);
int32_t iftOsdServ_overlyingOneFrame_MaxBitMap(SOsdUserInt * usr, iftOsdText *fstru, int lines);
int32_t iftOsdServ_overlyingOneFrame_Text2BitMap(SOsdUserInt * usr, iftOsdText *fstru, SFrameData *oneFrame, E_CHAR_SET charset);
int32_t iftOsdServ_overlyingOneFrame_DrawBitMap(SOsdUserInt * usr, iftOsdText *fstru, SFrameData *oneFrame);

///FIFO
typedef struct SOsdRuntime
{
    SOsdUserInt *users[MAX_OSD_USERCNT];
    IftEnv env ;
    int32_t usrAct_tm_s ; // user act time (s)
} SOsdRuntime;
extern SOsdRuntime osdRuntime;
#endif /* OSDSEVICEINT_H_ */
