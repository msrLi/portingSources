/**
 * @file iftconfig.c
 * @brief
 * @author jerry.king
 * @date 2012-12-3 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#include "iftconfig.h"
#include <string.h>
static int flashOsdServStyle(iftStyle* style);

static IftEnv gs_osdEnv;
static iftFontInfo gs_FontInfoTable;
static iftFtFile gs_FtFileTable;
static iftStyle gs_defaultstyle =
{
    //===================================================
    //style 1 //default
    .next = NULL,
    .sid = 1,
    .ftsrcType = FTSRC_TYPE_FILE,
    .ftsrc.path = DEF_FONTPATH,
    .horz_resolution = DEF_HORZ_RESOLUTION,
    .vert_resolution = DEF_VERT_RESOLUTION,
    .lineconf =
    {
        .charspace = 0,
        .super = // iftParaLayout
        {
            .align = E_ALIGN_LEFT,
            .blineWrap = 0,
            .linegap = 10
        },
    },
    .fontconf =
    {
        .super =
        {
            .pt = DEF_WORD_POINT,
            .color =
            {
                .r = 255,
                .g = 0,
                .b = 0,
            },
            .extra =
            {
                .bOutline = 0,
                .bRotate = 0,
            },
        },
        .ascent = 0,
        .descent = 0,
        .height = IFT_PT2PIX(DEF_WORD_POINT, DEF_VERT_RESOLUTION),
        .maxAdvanceWidth = IFT_PT2PIX(DEF_WORD_POINT, DEF_HORZ_RESOLUTION)
    },
    // .fontLineHight =IFT_PT2PIX(DEF_WORD_POINT,DEF_VERT_RESOLUTION),
    //===================================================
};
iftRect gs_defaultCavasRect =
{
    .orgi =
    {
        0,
        0
    },
    .wide = 800,
    .hight = 600,
};

inline IftEnv* iftosdEnv_get()
{
    return &gs_osdEnv;
}
inline iftFontInfo *iftFontInfo_hdr()
{
    return &gs_FontInfoTable;
}
inline int iftFontInfo_init()
{
    /**
     * g font table
     */
    //todo  add file
    gs_FtFileTable.fpname = DEF_FONTPATH;
    gs_FtFileTable.id = 1;
    gs_FtFileTable.next = NULL;

    gs_FontInfoTable.file = &gs_FtFileTable;

    return 0;
}
inline iftFontInfo *iftFontInfo_getByPath(char *pname)
{
    iftFontInfo *info = iftFontInfo_hdr();
    if (!info->file)
    {
        iftFontInfo_init();
        assert(info->file);
    }
    while (info)
    {
        if (info->file && strcmp(pname, info->file->fpname) == 0)
        {
            return info;
        }
        info = info->next;
    }
    return NULL ;
}
//===================================================

inline iftStyle * iftGlobDefaultStyle()
{
    return &gs_defaultstyle;
}
inline iftStyle * iftStyle_default(iftStyleSheet *sheet)
{
    return sheet->front;
}

inline iftStyle * iftStyle_getByID(iftStyleSheet *sheet, iftStyleID id)
{
    iftStyle *pitem = sheet->front, *retitem = NULL;
    while (pitem->next)
    {
        if (pitem->sid == id)
        {
            retitem = pitem;
            break;
        }
        pitem = pitem->next;
    }
    return retitem;
}
inline iftStyleID iftStyleIDGen()
{
    static iftStyleID iniID = 1000;
    return iniID++;
}
inline int iftStyle_init(iftStyleSheet *sheet)
{
    iftStyle *pitem;
    // add new
    pitem = (iftStyle *) iftMemAlloc(sizeof(iftStyle));
    if (!pitem)
    {
        MYTRACE("error in alloc!");
        return -1 ;
    }
    *pitem = gs_defaultstyle;
    pitem->next = NULL;
    sheet->cnt = 1;
    sheet->front = pitem;
    sheet->tail = pitem;
    flashOsdServStyle(sheet->front);
    dprintf(DBG_LEV_INF, "enter style init,cur param :cnt=%d", sheet->cnt);

    return 0;
}
inline int iftStyle_deinit(iftStyleSheet *sheet)
{
    int i = 0;
    for (; i < sheet->cnt; i++)
    {
        iftStyle_movSty(sheet, sheet->front->next);
    }
    return 0;
}

inline int iftStyle_equal(iftStyle * sty, iftOsdServStyleInt * sersty)

{
    return (DEF_SMEM_EQU(&sersty->fctrl, &sty->fontconf.super, iftFontCtrl)
            && DEF_SMEM_EQU(&sersty->parag, &sty->lineconf.super, iftParaLayout));
}
inline iftStyle * iftStyle_find(iftStyleSheet *sheet, iftOsdServStyleInt *servStyle)
{
    iftStyle *pitem = sheet->front, *fitem = NULL;
    while (pitem)
    {
        if (iftStyle_equal(pitem, servStyle))
        {
            // find style
            fitem = pitem;
            break;
        }
        pitem = pitem->next;
    }
    return fitem;
}
static int flashOsdServStyle(iftStyle* style)
{
    if (!style) { return -1; }
    style->fontconf.height = IFT_PT2PIX(style->fontconf.super.pt, style->vert_resolution);
    style->fontconf.maxAdvanceWidth = IFT_PT2PIX(style->fontconf.super.pt, style->horz_resolution);
    style->fontLineHight = style->fontconf.height;// + style->lineconf.super.linegap;
    return 0;
}

inline iftStyle * iftStyle_add(iftStyleSheet *sheet, iftOsdServStyleInt *servStyle)
{
    iftStyle *pitem;
    // add new
    pitem = (iftStyle *) iftMemAlloc(sizeof(iftStyle));
    if (!pitem)
    {
        MYTRACE("error in alloc!");
        return NULL ;
    }
    *pitem = gs_defaultstyle;
    pitem->next = NULL;
    pitem->sid = iftStyleIDGen();
    pitem->fontconf.super = servStyle->fctrl;
    pitem->lineconf.super = servStyle->parag;
    flashOsdServStyle(pitem);
    /**
     * add to tail
     */
    sheet->tail->next = pitem;
    sheet->tail = pitem;
    sheet->cnt++;
    return pitem;
}
inline int iftStyle_movSty(iftStyleSheet *sheet, iftStyle * sty)
{
    if (!sty)
    {
        return -1;
    }
    iftStyle *preitem = sheet->front;
    while (preitem->next)
    {
        if (preitem->next == sty)
        {
            preitem->next = sty->next;
            //set tail
            if (sheet->tail == sty)
            {
                sheet->tail = preitem;
            }
            iftMemFree(sty);
            sty = NULL;
            sheet->cnt--;
            //dprintf(DBG_LEV_INF, "mov sty to cnt=%d", sheet->cnt);
            return 0;
        }
        preitem = preitem->next;
    }
    return -1;
}
inline int iftStyle_mov(iftStyleSheet *sheet, iftOsdServStyleInt *servStyle)
{
    iftStyle *pitem;
    if (iftStyle_equal(sheet->front, servStyle))
    {
        //not allow to mov this node
        dprintf(DBG_LEV_ERR, "MOV ERRLINK !");
        return -1;
    }
    if (NULL != (pitem = iftStyle_find(sheet, servStyle)))
    {
        //move itm
        return -1;
    }
    return iftStyle_movSty(sheet, pitem);
}

//===================================================

