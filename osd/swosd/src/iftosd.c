/////////////////////////////////////////////////////////////////////
/**
 * free type
 *
 */
#include <stdlib.h>
#include "iftsysTypes.h"
#include "iftosdint.h"
#include "osdSeviceInt.h"
#include "iftFrameOperate.h"
#include "osdFontGen.h"
#include "iftOsdServiceInt.h"
#include <unistd.h>
#include <time.h>
//global runtime
SOsdRuntime g_osdRuntime =
{
    .usrAct_tm_s = 3600, //1hour
};

/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_init(int32_t dbgLevel, int32_t iftosd_ver)
{
    debugSetLevel(dbgLevel);
    iftFtgen_runOnce();
    g_osdRuntime.env.dbgLevel = dbgLevel;
    g_osdRuntime.env.version = iftosd_ver;
    iftFtgen_setEnv(&g_osdRuntime.env);
    return 0;
}
int32_t iftosd_handle_minsize()
{
    return IFT_MIN_MMU_ALLOC_SIZE ;
}
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_handle_create(iftOsdHandle hand, iftRange *rgpools)
{
    if (!hand)
    {
        /**
         * no alloc space
         */
        MYTRACE();
        return -1;
    }
    return iftOsdServ_usrInit(hand, rgpools);
}

int32_t iftosd_defstyle_get(iftOsdHandle hand, iftOsdServStyle * servStyle)
{
    return iftFtgen_defStyle(hand->meta.genctx, (iftOsdServStyleInt *) servStyle);
}
int32_t iftosd_setOutline(iftOsdServStyle *style, int outlinePix)
{
    if (!style || outlinePix <= 0)
    {
        MYTRACE();
        return -1 ;
    }
    iftOsdServStyleInt* style_in = (iftOsdServStyleInt*)style;
    style_in->fctrl.extra.bOutline = iftTURE;
    style_in->fctrl.extra.outLinePix = outlinePix;
    return 0;
}
int32_t iftosd_unsetOutline(iftOsdServStyle *style)
{
    if (!style) { return -1; }
    iftOsdServStyleInt* style_in = (iftOsdServStyleInt*) style;
    style_in->fctrl.extra.bOutline = iftFALSE;
    style_in->fctrl.extra.outLinePix = 0;
    return 0 ;
}

int32_t iftosd_overly_oneframe_Text2BitMap(iftOsdHandle hand, iftOsdText *text, SFrameData *oneFrame, E_CHAR_SET charset)
{
    /**
     * check frame pitch valide
     */
    assert(oneFrame);

    oneFrame->params->pixLinePitch = MAX(oneFrame->params->pixWidth, oneFrame->params->pixLinePitch) ;
    return iftOsdServ_overlyingOneFrame_Text2BitMap(hand, text, oneFrame, charset);
}
int32_t iftosd_overly_oneframe_MaxBitMap(iftOsdHandle hand, iftOsdText *text, int lines)
{
    /**
     * check frame pitch valide
     */
    return iftOsdServ_overlyingOneFrame_MaxBitMap(hand, text, lines);
}


int32_t iftosd_overly_oneframe_DrawBitMap(iftOsdHandle hand, iftOsdText *text, SFrameData *oneFrame)
{
    /**
     * check frame pitch valide
     */
    assert(oneFrame);

    oneFrame->params->pixLinePitch = MAX(oneFrame->params->pixWidth, oneFrame->params->pixLinePitch) ;
    return iftOsdServ_overlyingOneFrame_DrawBitMap(hand, text, oneFrame);
}


int32_t iftosd_overly_oneframe(iftOsdHandle hand, iftOsdText *text, SFrameData *oneFrame, E_CHAR_SET charset)
{
    /**
     * check frame pitch valide
     */
    assert(oneFrame);

    oneFrame->params->pixLinePitch = MAX(oneFrame->params->pixWidth, oneFrame->params->pixLinePitch) ;
    return iftOsdServ_overlyingOneFrame(hand, text, oneFrame, charset);
}
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_handle_destroy(iftOsdHandle hand)
{
    iftOsdServ_userDeinit(hand);
    return 0;
}

/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_setEnv(IftEnv *iftenv)
{
    if (!iftenv)
    {
        return -1;
    }
    g_osdRuntime.env = *iftenv;
    debugSetLevel(iftenv->dbgLevel);
    memset(g_osdRuntime.users, 0, sizeof(SOsdUserInt*));
    iftFtgen_setEnv(iftenv);
    return 0;
}
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
//int32_t iftosd_init()
//{
//    return 0;
//}
inline int32_t genOsdUsrID()
{
    static int uid = 100;
    return uid++;
}
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
OsdUser_H iftosd_regUsr(int uid)
{
    int i = 0;
    int selectID = -1;
    SOsdUserInt *usr_h = NULL;
    for (i = 0; i < MAX_OSD_USERCNT; i++)
    {
        if (!g_osdRuntime.users[i])
        {
            selectID = i;
        }
        else
        {
            if (g_osdRuntime.users[i]->super.uid == uid)
            {
                // scan uID
                usr_h = g_osdRuntime.users[i];
                break;
            }
        }
    }
    if (usr_h)
    {
        /**
         * get used uid
         */
        return &usr_h->super;
    }
    else
    {
        //create user
        if (selectID < 0)
        {
            dprintf(DBG_LEV_ERR, "user full! please wait!");
            return NULL ;
        }
        usr_h = iftOsdServ_usrCreate(NULL);
        if (!usr_h)
        {
            //alloc usr stru error
            MYTRACE();
            return NULL ;
        }
    }
    g_osdRuntime.users[selectID] = usr_h;
    return &usr_h->super;
}
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_unregUsr(OsdUser_H *pusr)
{
    if (!pusr)
    {
        MYTRACE();
        return -1;
    }
    SOsdUserInt* usr_in = (SOsdUserInt*)(*pusr);
    int32_t ret = iftOsdServ_usrDestory(usr_in);
    if (ret < 0)
    {
        MYTRACE();
        return -1;
    }
    *pusr = NULL;
    return 0;
}
int32_t iftosd_defStyle(OsdUser_H usr, iftOsdServStyle * servStyle)
{
    if (!usr || !servStyle)
    {
        MYTRACE();
        return -1;
    }
    SOsdUserInt* usr_in = (SOsdUserInt*) usr;
    return iftFtgen_defStyle(usr_in->meta.genctx, (iftOsdServStyleInt *) servStyle);
}
/**
 * @brief Set osdfont env
 * @param[in] iftenv The obj of iftosd set.
 * @return  >=0 return ok ; < 0 return false .
 */
int32_t iftosd_destroy()
{
    int i = 0;
    for (i = 0; i < MAX_OSD_USERCNT; i++)
    {
        if (g_osdRuntime.users[i])
        {
            iftOsdServ_usrDestory(g_osdRuntime.users[i]);
            iftMemFree(g_osdRuntime.users[i]);
            g_osdRuntime.users[i] = NULL;
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////
/**
 * out alloc usr hand data ,
 */
int iftOsdServ_usrInit(SOsdUserInt *usr, iftRange *rgpools)
{
    SOsdUserInt *unod = usr;
    if (!unod)
    {
        //alloc error
        MYTRACE();
        return -1;
    }
    int32_t usrid = genOsdUsrID();
    unod->super.uid = usrid;
    unod->meta.comm = NULL;
    unod->meta.next = NULL;
    unod->meta.pdata = NULL;
    unod->grayframpool = rgpools->data;
    if (rgpools->len < IFT_MIN_MMU_ALLOC_SIZE)
    {
        dprintf(DBG_LEV_ERR, "out init memory size too small! need range.len>%d(KB)"
                , MOD_CEILING_DIV(IFT_MIN_MMU_ALLOC_SIZE, 1024));
    }

    unod->innerFrameProcessRange.data = rgpools->data + 2 * DEF_GRAYFRAMPOOL_SIZE;
    unod->innerFrameProcessRange.len = 2 * DEF_GRAYFRAMPOOL_SIZE;

    iftRange  ftgenRange =
    {
        .data = rgpools->data + 3 * DEF_GRAYFRAMPOOL_SIZE,
        .len = rgpools->len - 3 * DEF_GRAYFRAMPOOL_SIZE,
    };
    unod->meta.genctx = iftFtgen_hand(&ftgenRange);
    if (!unod->meta.genctx)
    {
        //alloc error
        MYTRACE();
        return -1;
    }
    return 0;
}
/**
 * destroy
 */
int32_t iftOsdServ_userDeinit(SOsdUserInt *usr)
{
    assert(usr);
    if (usr->grayframpool)
    {
        usr->grayframpool = NULL;
    }
    if (usr->meta.genctx)
    {
        iftFtgen_destory(usr->meta.genctx);
        usr->meta.genctx = NULL;
    }
    memset(usr, 0, sizeof(SOsdUserInt));
    return 0;
}
/**
 * create uid
 */
SOsdUserInt* iftOsdServ_usrCreate(int32_t *uid)
{
    SOsdUserInt *unod = NULL;
    unod = (SOsdUserInt*) iftMemAlloc(sizeof(SOsdUserInt));
    if (!unod)
    {
        //alloc error
        MYTRACE();
        return NULL ;
    }
    unod->usrRange.len = 4 * 1024 * 1024 ;
    unod->usrRange.data = iftMemAlloc(unod->usrRange.len);
    if (!unod->usrRange.data)
    {
        iftMemFree(unod);
        MYTRACE();
        return NULL ;

    }
    if (iftOsdServ_usrInit(unod, &unod->usrRange) < 0)
    {
        iftOsdServ_usrDestory(unod);
        return NULL ;
    }
    return unod;
}
/**
 * destroy
 */
int32_t iftOsdServ_usrDestory(SOsdUserInt *usr)
{
    assert(usr);
    iftOsdServ_userDeinit(usr);
    if (usr->usrRange.data)
    {
        iftMemFree(usr->usrRange.data);
    }
    iftMemFree(usr);
    return 0;
}
#if 0
static int changeCodiSty2Upleft(iftCanvasLayout *canvlayout, iftCanvasInt *canv, SFrameData *pFontFrm)
{
    switch (canv->coosty)
    {
        case E_ORIG_STYLE_UPLEFT:
            break;
        case E_ORIG_STYLE_UPRIGHT:
            canv->coosty = E_ORIG_STYLE_UPLEFT;
            int lpos_x = canvlayout->width - (pFontFrm->params->MaxFontAdvance + canv->rect.orgi.x);
            // int lpos_y = canvlayout->height - (canv->rect.hight + canv->rect.orgi.y);
            canv->rect.orgi.x = (lpos_x > 0) ? lpos_x : 0;
            //  lpos_y = (lpos_y > 0) ? lpos_y : 0;
            break;
    }
    return 0;
}
#endif
#define DEF_IFT_ERR_RET(ret,rcod) {if(ret<0){MYTRACE();\
    return rcod;}}
#define DEF_IFT_ERR_BREAK(ret) {if(ret<0){MYTRACE();\
    break;}}

int32_t iftOsdServ_overlyingOneFrame_MaxBitMap(SOsdUserInt * usr, iftOsdText *fstru, int lines)
{
    if (!(usr && fstru))
    {
        MYTRACE();
        return -1;
    }
    int idx = 0, max;
    SOsdUserInt *usr_in = (SOsdUserInt*)usr;
    SFrameData  *pFontFrm = NULL;
    max = 0;
    for (idx = 0; idx < lines; idx++)
    {
        pFontFrm = (SFrameData *)((char *)usr_in->grayframpool + idx * fstru->poolsize);
        if (pFontFrm->params->pixWidth > max)
        {   
            max = pFontFrm->params->pixWidth;
        }
    }
    if (abs(fstru->maxFontWidth - max) > 8) { fstru->maxFontWidth = max; }
   
    return 0;
}

int32_t iftOsdServ_overlyingOneFrame_Text2BitMap(SOsdUserInt * usr, iftOsdText *fstru, SFrameData *oneFrame, E_CHAR_SET charset)
{
    //overLay
    if (!(usr && fstru && oneFrame))
    {
        MYTRACE();
        return -1;
    }
    int32_t nret;
    SOsdUserInt *usr_in = (SOsdUserInt*)usr;
    if (fstru->s_ver < DEF_OSDCOMM_VER)
    {
        MYTRACE("iftOsdFontComm struct version error!");
        return -1;
    }
    if (!fstru->bEanble)
    {
        MYTRACE("trace!");
        return -1;

    }
    if (!fstru->wordBuf)
    {
        MYTRACE();
        return -1;
    }
    if (!usr_in->grayframpool)
    {
        MYTRACE();
        return -1;
    }

    int32_t icnt = 0;
    if (fstru->reload[fstru->idx] == 1)
    {
        //    usr_in
        nret = iftFtgen_setStyle(usr_in->meta.genctx, (iftOsdServStyleInt *) &fstru->cavLay[fstru->idx].style);
        DEF_IFT_ERR_RET(nret, -1);
        /**
            * process every canvas
            */
        for (icnt = 0; icnt < fstru->cavLay[fstru->idx].canvCnt; icnt++)
        {

            iftOsdServStyleInt *lcStyle = (iftOsdServStyleInt *) &fstru->cavLay[fstru->idx].style;
            iftCanvasInt * pcurCanv = (iftCanvasInt *) &fstru->cavLay[fstru->idx].canv[icnt];
            if (pcurCanv->bSetParaLayout)
            {
                lcStyle->parag = pcurCanv->parag;
                if (iftFtgen_setStyle(usr_in->meta.genctx, lcStyle) < 0)
                {
                    MYTRACE();
                }
            }
            if (pcurCanv->bSetfontColor)
            {
                lcStyle->fctrl.color = pcurCanv->fontColor;
            }
            /**
             * calc canvas pos wide--hight reset
             */
            pcurCanv->rect.wide = fstru->cavLay[fstru->idx].width;
            pcurCanv->rect.hight = pcurCanv->lineNum * iftFtgen_orgiStyle(usr_in->meta.genctx)->fontLineHight;

            int32_t bcontinue = 0;
            do
            {
                nret = iftFtgen_setRect(usr_in->meta.genctx, pcurCanv->rect.wide, pcurCanv->rect.hight);
                DEF_IFT_ERR_BREAK(nret);

                if (nret < 0)
                {
                    MYTRACE();
                    break;
                }
                nret = iftFtgen_sessionEnter(usr_in->meta.genctx);
                DEF_IFT_ERR_BREAK(nret);
                /**
                 * get gray data
                 */
                nret = iftFtgen_wordBuf(usr_in->meta.genctx,
                                        pcurCanv->lineNum,
                                        pcurCanv->line,
                                        // fstru->wordBuf,E_CHAR_SET_UTF8);
                                        fstru->wordBuf, charset);
                DEF_IFT_ERR_BREAK(nret);
                //todo overlay one carvan to one frame
                nret = iftFtgen_dumpGrayFrame2pool(usr_in->meta.genctx,
                                                   (char *)usr_in->grayframpool + fstru->idx * fstru->poolsize,
                                                   fstru->poolsize/*DEF_GRAYFRAMPOOL_SIZE*/);

                DEF_IFT_ERR_BREAK(nret);
                nret = iftFtgen_sessionExit(usr_in->meta.genctx);
                DEF_IFT_ERR_BREAK(nret);

            }
            while (0);
            if (bcontinue)
            {
                continue;
            }
            if (nret < 0)
            {
                break;
            }
        }
    }
    return 0;
}

int32_t iftOsdServ_overlyingOneFrame_DrawBitMap(SOsdUserInt * usr, iftOsdText *fstru, SFrameData *bgFrame)
{
    //overLay
    if (!(usr && fstru && bgFrame))
    {
        MYTRACE();
        return -1;
    }
    int32_t nret;
    SOsdUserInt *usr_in = (SOsdUserInt*)usr;
    if (fstru->s_ver < DEF_OSDCOMM_VER)
    {
        MYTRACE("iftOsdFontComm struct version error!");
        return -1;
    }
    if (!fstru->bEanble)
    {
        MYTRACE("trace!");
        return -1;

    }
    if (!fstru->wordBuf)
    {
        MYTRACE();
        return -1;
    }
    if (!usr_in->grayframpool)
    {
        MYTRACE();
        return -1;
    }
    int32_t icnt = 0;
    //printf("fstru->cavLay[fstru->idx].canvCnt = %d\n", fstru->cavLay[fstru->idx].canvCnt);
    for (icnt = 0; icnt < fstru->cavLay[fstru->idx].canvCnt; icnt++)
    {
        iftOsdServStyleInt *lcStyle = (iftOsdServStyleInt *) &fstru->cavLay[fstru->idx].style;
        iftCanvasInt * pcurCanv     = (iftCanvasInt *) &fstru->cavLay[fstru->idx].canv[icnt];
        pcurCanv->fg_mode = fstru->fg_mode;
        pcurCanv->bg_mode = fstru->bg_mode;
        pcurCanv->position = fstru->position;
        pcurCanv->maxFontWidth = fstru->maxFontWidth;
        SFrameData *pFontFrm = NULL;
        do
        {
            pFontFrm = (SFrameData *)((char *)usr_in->grayframpool + fstru->idx * fstru->poolsize);
            //printf("======================pFontFrm = %p, lcStyle = %p [0]=====================\n", pFontFrm, lcStyle);
            nret = iftFrmOper_overlay(&lcStyle->fctrl.color,
                                      pcurCanv,
                                      pFontFrm,
                                      bgFrame);
        }
        while (0);
    }
    return 0;
}


int32_t iftOsdServ_overlyingOneFrame(SOsdUserInt * usr, iftOsdText *fstru, SFrameData *oneFrame, E_CHAR_SET charset)
{
    //overLay
    if (!(usr && fstru && oneFrame))
    {
        MYTRACE();
        return -1;
    }
    int32_t nret;
    SOsdUserInt *usr_in = (SOsdUserInt*)usr;
    if (fstru->s_ver < DEF_OSDCOMM_VER)
    {
        MYTRACE("iftOsdFontComm struct version error!");
        return -1;
    }
    if (!fstru->bEanble)
    {
        MYTRACE("trace!");
        return -1;

    }
    if (!fstru->wordBuf)
    {
        MYTRACE();
        return -1;
    }
    if (!usr_in->grayframpool)
    {
        MYTRACE();
        return -1;
    }
    int32_t icnt = 0;
    SFrameData *pFontFrm = NULL;
    if (fstru->reload[fstru->idx] == 1)
    {
        //    usr_in
        nret = iftFtgen_setStyle(usr_in->meta.genctx, (iftOsdServStyleInt *) &fstru->cavLay[fstru->idx].style);
        DEF_IFT_ERR_RET(nret, -1);
        /**
            * process every canvas
            */
        for (icnt = 0; icnt < fstru->cavLay[fstru->idx].canvCnt; icnt++)
        {

            iftOsdServStyleInt *lcStyle = (iftOsdServStyleInt *) &fstru->cavLay[fstru->idx].style;
            iftCanvasInt * pcurCanv = (iftCanvasInt *) &fstru->cavLay[fstru->idx].canv[icnt];
            if (pcurCanv->bSetParaLayout)
            {
                lcStyle->parag = pcurCanv->parag;
                if (iftFtgen_setStyle(usr_in->meta.genctx, lcStyle) < 0)
                {
                    MYTRACE();
                }
            }
            if (pcurCanv->bSetfontColor)
            {
                lcStyle->fctrl.color = pcurCanv->fontColor;
            }
            /**
             * calc canvas pos wide--hight reset
             */
            pcurCanv->rect.wide = fstru->cavLay[fstru->idx].width;
            pcurCanv->rect.hight = pcurCanv->lineNum * iftFtgen_orgiStyle(usr_in->meta.genctx)->fontLineHight;

            int32_t bcontinue = 0;
            do
            {
                nret = iftFtgen_setRect(usr_in->meta.genctx, pcurCanv->rect.wide, pcurCanv->rect.hight);
                DEF_IFT_ERR_BREAK(nret);

                if (nret < 0)
                {
                    MYTRACE();
                    break;
                }
                nret = iftFtgen_sessionEnter(usr_in->meta.genctx);
                DEF_IFT_ERR_BREAK(nret);
                /**
                 * get gray data
                 */
                nret = iftFtgen_wordBuf(usr_in->meta.genctx,
                                        pcurCanv->lineNum,
                                        pcurCanv->line,
                                        // fstru->wordBuf,E_CHAR_SET_UTF8);
                                        fstru->wordBuf, charset);
                DEF_IFT_ERR_BREAK(nret);
                //todo overlay one carvan to one frame
                nret = iftFtgen_dumpGrayFrame2pool(usr_in->meta.genctx,
                                                   (char *)usr_in->grayframpool + fstru->idx * fstru->poolsize,
                                                   fstru->poolsize/*DEF_GRAYFRAMPOOL_SIZE*/);
                DEF_IFT_ERR_BREAK(nret);
                /**
                 * calc rect pos
                 */
                pFontFrm = (SFrameData *)((char *)usr_in->grayframpool + fstru->idx * fstru->poolsize);
                //printf("======================pFontFrm = %p, lcStyle = %p [1]=====================\n", pFontFrm, lcStyle);
                iftOsdServStyleInt  *ft_stlye_in = (iftOsdServStyleInt*) & (fstru->cavLay[fstru->idx].style);
                //changeCodiSty2Upleft(&fstru->cavLay[fstru->idx], pcurCanv,pFontFrm);
                if (ft_stlye_in->fctrl.extra.bOutline)
                {
                    /**
                     * check outline buff
                     */
                    if (oneFrame->data.un.len >= usr_in->innerFrameProcessRange.len)
                    {
                        MYTRACE("data alloc too few to run system!");
                        break;
                    }
                }
                pcurCanv->fg_mode = fstru->fg_mode;
                pcurCanv->bg_mode = fstru->bg_mode;
                pcurCanv->position = fstru->position;
                nret = iftFrmOper_overlay(&lcStyle->fctrl.color,
                                          pcurCanv,
                                          pFontFrm,
                                          oneFrame
                                         );
                DEF_IFT_ERR_BREAK(nret);
                nret = iftFtgen_sessionExit(usr_in->meta.genctx);
                DEF_IFT_ERR_BREAK(nret);

            }
            while (0);
            if (bcontinue)
            {
                continue;
            }
            if (nret < 0)
            {
                break;
            }
        }
    }
    else
    {
        //printf("fstru->cavLay[fstru->idx].canvCnt = %d\n", fstru->cavLay[fstru->idx].canvCnt);
        for (icnt = 0; icnt < fstru->cavLay[fstru->idx].canvCnt; icnt++)
        {
            iftOsdServStyleInt *lcStyle = (iftOsdServStyleInt *) &fstru->cavLay[fstru->idx].style;
            iftCanvasInt * pcurCanv     = (iftCanvasInt *) &fstru->cavLay[fstru->idx].canv[icnt];
            pcurCanv->fg_mode = fstru->fg_mode;
            pcurCanv->bg_mode = fstru->bg_mode;
            pcurCanv->position = fstru->position;
            pcurCanv->maxFontWidth = fstru->maxFontWidth;
            do
            {
                pFontFrm = (SFrameData *)((char *)usr_in->grayframpool + fstru->idx * fstru->poolsize);
                //printf("======================pFontFrm = %p, lcStyle = %p [0]=====================\n", pFontFrm, lcStyle);
                nret = iftFrmOper_overlay(&lcStyle->fctrl.color,
                                          pcurCanv,
                                          pFontFrm,
                                          oneFrame
                                         );

            }
            while (0);

        }
    }
    if (fstru->idx == 0)
    {
        if (abs(fstru->maxFontWidth - fstru->refreshFont) > 8) { fstru->maxFontWidth = fstru->refreshFont; }
        fstru->refreshFont = 0;
    }
    if (pFontFrm->params->pixWidth > fstru->refreshFont)
    {
        fstru->refreshFont = pFontFrm->params->pixWidth;
    }
    return 0;
}
