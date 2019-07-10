/**
 * @file iftFrameOperate.c
 * @brief
 * @author jerry.king
 * @date 2012-12-17 Created
 **/
/******************************************************************
 * @note
 * &copy; Copyright Beijing iTarge Software Technologies, Ltd
 * http://www.itarge.com
 * ALL RIGHTS RESERVED
 ******************************************************************/

#include "iftFrameOperate.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "YUVUtils.h"
#include "iftosdint.h"
//static void dumpframe(SFrameData * frame, int b_num);
/**
 * type YUV420SP_UV
 */
struct SUVNode
{
    uint8_t u, v;
};
struct SPlansHdr
{
    uint8_t *plan_y;
    struct SUVNode *plan_uv;
    uint8_t *I420_U;
    uint8_t *I420_V;
};

#define DEF_POS_INREGIN(pos,omin,omax) ((pos)>=(omin) && (pos)<=(omax))
static inline iftBool posInRect(SCoordinate2D pos, iftRect *rect)
{
    return (DEF_POS_INREGIN(pos.x, rect->orgi.x, rect->wide) && DEF_POS_INREGIN(pos.y, rect->orgi.y, rect->hight));
}
#define DEF_ASS_FRMDATA(t) assert((t)&&(t)->params)

#define DEF_YUV420BUFF_BYTELEN(with,hight) ((with)*(hight)*3/2)

#define DEF_FRM_ARRY2D_POS(y,width,x) ((y)*(width)+(x))

/*get_framePos_ptr 这个函数 当metor不为1的时候 有问题，慎用*/
inline static uint8_t* get_framePos_ptr(SFrameData *fontFrame, int gpos_x, int gpos_y, int metor)
{
    return &fontFrame->data.un.d[metor * DEF_FRM_ARRY2D_POS(gpos_y, fontFrame->params->pixWidth, gpos_x)];
}
inline static uint8_t* get_framePos_Optimized_ptr(SFrameData *fontFrame, int gpos_x, int font_y_off, int metor)
{
    //return &fontFrame->data.un.d[metor * (font_y_off + gpos_x)];
    return &fontFrame->data.un.d[(font_y_off + gpos_x)];
}

/**
 * get banding frame
 * return banding data alloc
 */
const SFrameData * banding_frame_gray_data_alloc(int banding_pixs,  SFrameData *fontFrame, SFrameData *bandingFontFrame, iftRange * innerProcRange)
{
    if (banding_pixs < 0 || banding_pixs > MAX_BANDING_PIXS)
    {
        dprintf(DBG_LEV_ERR, "param range error ! banding_pixs < %d (cur banding_pixs=%d)", MAX_BANDING_PIXS, banding_pixs);
        return NULL ;
    }

    assert(bandingFontFrame && fontFrame && fontFrame->params && innerProcRange && innerProcRange->data);
    if (fontFrame->dtype != E_FRMDATA_TYPE_UNION)
        if ((fontFrame->params->type != E_Frame_GRAY256))
        {
            dprintf(DBG_LEV_ERR, "param  error in frame->params");
            return NULL ;
        }

    bandingFontFrame->dtype = fontFrame->dtype;
    bandingFontFrame->data.un.len = fontFrame->data.un.len;
    int bandingFramDataLen = fontFrame->data.un.len;
    //innerProcRange
    if ((bandingFramDataLen + sizeof(struct SFrameParam)) > innerProcRange->len)
    {
        dprintf(DBG_LEV_ERR, "  data size error !");
        return NULL;
    }
    bandingFontFrame->params = innerProcRange->data ;
    memcpy(bandingFontFrame->params, fontFrame->params, sizeof(struct SFrameParam));
    if (NULL == (bandingFontFrame->data.un.d = innerProcRange->data + sizeof(struct SFrameParam)))
    {
        dprintf(DBG_LEV_ERR, "alloc data error !");
        return NULL;
    }
    memset(bandingFontFrame->data.un.d, 0, bandingFramDataLen);
    unsigned char banding_neighborhood_data[2][MAX_BANDING_PIXS * MAX_BANDING_PIXS];
    memset(banding_neighborhood_data, 0, 2 * MAX_BANDING_PIXS * MAX_BANDING_PIXS);
    int cur_high = 0 ;
    int cur_wide = 0 ;
    dprintf(DBG_LEV_DBG,
            "banding %p|%p,cnt=%d",
            &banding_neighborhood_data[0], &banding_neighborhood_data[1], &banding_neighborhood_data[1] - &banding_neighborhood_data[0]);

    for (cur_high = 0 ; cur_high < fontFrame->params->pixHight - banding_pixs ; cur_high++)
    {
        for (cur_wide = 0 ; cur_wide < fontFrame->params->pixWidth - banding_pixs ; cur_wide++)
        {
            //             memcpy()
            /**
             *  get neighborhood_data
             */
            memset(banding_neighborhood_data, 0, MAX_BANDING_PIXS * MAX_BANDING_PIXS);
            int i = 0 ;
            for (; i < banding_pixs; i++)
            {
                memcpy(((unsigned char *)(&banding_neighborhood_data[0]) + DEF_FRM_ARRY2D_POS(i, banding_pixs, 0)),
                       fontFrame->data.un.d + DEF_FRM_ARRY2D_POS(cur_high + i, fontFrame->params->pixWidth, cur_wide),
                       banding_pixs);
            }

            if (memcmp(&banding_neighborhood_data[0], &banding_neighborhood_data[1], banding_pixs * banding_pixs) != 0)
            {

                for (i = 0; i < banding_pixs * banding_pixs; i++)
                {
                    int bdfrm_ft_pos =
                        DEF_FRM_ARRY2D_POS(cur_high + (i / banding_pixs), fontFrame->params->pixWidth, cur_wide + i % banding_pixs);

                    if (banding_neighborhood_data[0][i] == 0)
                    {
                        //set bandingFontFrame
                        bandingFontFrame->data.un.d[bdfrm_ft_pos]++;
                    }
                    else
                    {
                        bandingFontFrame->data.un.d[bdfrm_ft_pos] = 0 ;
                    }
                }
            }//end  if( memcmp(banding_neighborhood_data,&banding_neighborhood_data[1],banding_pixs*banding_pixs) != 0 )

        }//end for( cur_wide = 0 ; cur_wide < fontFrame->params->pixWidth - banding_pixs ; cur_wide++)
    }//end for( cur_high = 0 ; cur_high < fontFrame->params->pixHight - banding_pixs ; cur_high++)
    return bandingFontFrame ;
}
int  banding_frame_gray_data_free(SFrameData * frame)
{
    if (!(frame))
    {
        return -1 ;
    }
    if (frame->params)
    {
        frame->params = NULL;
    }
    if (frame->data.un.d)
    {

        frame->data.un.d = NULL;
    }
    return 0 ;
}
enum
{
    OSD_POS_TOP_LEFT = 0,
    OSD_POS_TOP_RIGHT,
    OSD_POS_BOTTOM_LEFT,
    OSD_POS_BOTTOM_RIGHT,
    OSD_POSITIOIN_END,
};
#define DEF_PIX_TRANPARENT_TO(pix_match,pix_background,dt,degree) (((pix_match)*(dt)+(pix_background)*((degree)-(dt)))/2*(degree))
#define OSD_BG_COEFF_FLOAT    (0.75)
static int32_t overlay_YUV420SP_UV_Black(iftColorYUV *bgColor, iftColorYUV *ftColor, iftRect *canRect,
        SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    int font_y_off = 0, y_off = 0;
    //int uv_off = 0;
    int y_cdt = 0;//, blank = 0;//uv_cdt = 0,
    int offset = 0;
    DEF_ASS_FRMDATA(fontFrame);
    DEF_ASS_FRMDATA(backgroundFrame);

    if ((fontFrame->params->type != E_Frame_GRAY256)
            && (backgroundFrame->params->type != E_Frame_YUV420SP_UV))
    {
        MYTRACE();
        return -1;
    }
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        if (fontFrame->data.un.len < fontFrame->params->pixHight * fontFrame->params->pixLinePitch)
        {
            MYTRACE();
            return -1;
        }
        if (backgroundFrame->data.un.len
                < DEF_YUV420BUFF_BYTELEN(backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch))
        {
            MYTRACE();
            return -1;
        }

    }
    else if (backgroundFrame->dtype == E_FRMDATA_TYPE_SPAN)
    {
        //need 2span when type is E_Frame_YUV420SP_UV
        if (!backgroundFrame->data.span[0].d || !backgroundFrame->data.span[1].d)
        {
            MYTRACE();
            return -1;
        }
    }
    else
    {
        MYTRACE();
        return -1;
    }

    struct SPlansHdr plansHdr;
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        plansHdr.plan_y = backgroundFrame->data.un.d;
        plansHdr.plan_uv = (struct SUVNode *)(backgroundFrame->data.un.d
                                              + backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight);
    }
    else
    {
        plansHdr.plan_y = backgroundFrame->data.span[0].d;
        plansHdr.plan_uv = (struct SUVNode *) backgroundFrame->data.span[1].d;
    }
    /**
         * revise origin coordinate
         */
    //canRect->orgi.x =MOD_CEILING(canRect->orgi.x,2);
    //canRect->orgi.y =MOD_CEILING(canRect->orgi.y,2);

    int bgnpos_x;
    int endpos_x;
    int bgnpos_y;
    int endpos_y;
    int pos_y, pos_x;
    int ftpos_x, ftpos_y; //font frame
    bgnpos_x = backgroundFrame->bgn_point.x;
    endpos_x = backgroundFrame->end_point.x;
    bgnpos_y = backgroundFrame->bgn_point.y;
    endpos_y = backgroundFrame->end_point.y;

    //printf("bgnpos_x = %d, endpos_x = %d, bgnpos_y = %d, endpos_y = %d\n", bgnpos_x, endpos_x, bgnpos_y, endpos_y);
    SGray256Sub *pos_gray;
    //!2 pixes offset for semi
    if (canRect->semi == FONT_BG_MODE_SEMI)
    {
        for (offset = -2, pos_y = bgnpos_y,
                y_off = (pos_y - 3) * backgroundFrame->params->pixLinePitch; offset < 0; offset++)
        {
            y_off += backgroundFrame->params->pixLinePitch;
            for (pos_x = bgnpos_x; pos_x < endpos_x; pos_x++)
            {
                y_cdt = y_off + pos_x;
                plansHdr.plan_y[y_cdt] = 60 + (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
            }
        }
        //!for osd and semi transparent backgroud
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off += backgroundFrame->params->pixLinePitch;
            //uv_off = (y_off>>2);
            font_y_off += fontFrame->params->pixWidth;
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = 0;
                }
                else
                {
                    //blank++;
                    y_cdt = y_off + pos_x;
                    plansHdr.plan_y[y_cdt] = 60 + (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
                }
            }
            //if((fontFrame->params->pixWidth<<1) < blank)break;
        }
    }
    else
    {
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            //uv_off = (pos_y>>1)*(backgroundFrame->params->pixLinePitch>>1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = 0;
                }
                else
                {
                    //blank++;
                }
            }
            //if(fontFrame->params->pixWidth < blank)break;
        }
    }
    return 0;
}

static int32_t overlay_YUV420SP_UV_White(iftColorYUV *bgColor, iftColorYUV *ftColor, iftRect *canRect,
        SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    int y_off = 0, font_y_off = 0;//, y_off_bg = 0
    //int uv_off = 0;
    int y_cdt = 0;//, blank = 0;//, uv_cdt = 0
    int offset = 0;
    DEF_ASS_FRMDATA(fontFrame);
    DEF_ASS_FRMDATA(backgroundFrame);
    if ((fontFrame->params->type != E_Frame_GRAY256)
            && (backgroundFrame->params->type != E_Frame_YUV420SP_UV))
    {
        MYTRACE();
        return -1;
    }
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        if (fontFrame->data.un.len < fontFrame->params->pixHight * fontFrame->params->pixLinePitch)
        {
            MYTRACE();
            return -1;
        }
        if (backgroundFrame->data.un.len
                < DEF_YUV420BUFF_BYTELEN(backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch))
        {
            MYTRACE();
            return -1;
        }

    }
    else if (backgroundFrame->dtype == E_FRMDATA_TYPE_SPAN)
    {
        //need 2span when type is E_Frame_YUV420SP_UV
        if (!backgroundFrame->data.span[0].d || !backgroundFrame->data.span[1].d)
        {
            MYTRACE();
            return -1;
        }
    }
    else
    {
        MYTRACE();
        return -1;
    }

    struct SPlansHdr plansHdr;
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        plansHdr.plan_y = backgroundFrame->data.un.d;
        plansHdr.plan_uv = (struct SUVNode *)(backgroundFrame->data.un.d
                                              + backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight);
    }
    else
    {
        plansHdr.plan_y = backgroundFrame->data.span[0].d;
        plansHdr.plan_uv = (struct SUVNode *) backgroundFrame->data.span[1].d;
    }
    /**
         * revise origin coordinate
         */
    //canRect->orgi.x =MOD_CEILING(canRect->orgi.x,2);
    //canRect->orgi.y =MOD_CEILING(canRect->orgi.y,2);

    int bgnpos_x;
    int endpos_x;
    int bgnpos_y;
    int endpos_y;
    int pos_y, pos_x;
    int ftpos_x, ftpos_y; //font frame
    bgnpos_x = backgroundFrame->bgn_point.x;
    endpos_x = backgroundFrame->end_point.x;
    bgnpos_y = backgroundFrame->bgn_point.y;
    endpos_y = backgroundFrame->end_point.y;

    //printf("bgnpos_x = %d, endpos_x = %d, bgnpos_y = %d, endpos_y = %d\n", bgnpos_x, endpos_x, bgnpos_y, endpos_y);
    SGray256Sub *pos_gray;
    //!2 pixes offset for semi
    if (canRect->semi == FONT_BG_MODE_SEMI)
    {
        for (offset = -2, pos_y = bgnpos_y,
                y_off = (pos_y - 3) * backgroundFrame->params->pixLinePitch; offset < 0; offset++)
        {
            y_off += backgroundFrame->params->pixLinePitch;
            for (pos_x = bgnpos_x; pos_x < endpos_x; pos_x++)
            {
                y_cdt = y_off + pos_x;
                plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
            }
        }
        //!for osd and semi transparent backgroud
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off += backgroundFrame->params->pixLinePitch;
            //uv_off = (y_off>>2);
            font_y_off += fontFrame->params->pixWidth;
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = 240;
                }
                else
                {
                    //blank++;
                    y_cdt = y_off + pos_x;
                    plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
                }
            }
            //if((fontFrame->params->pixWidth<<1) < blank)break;
        }
    }
    else
    {
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            //uv_off = (pos_y>>1)*(backgroundFrame->params->pixLinePitch>>1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[ y_off + pos_x] = 240;
                }
                //else
                //{
                //    blank++;
                //}
            }
            //if(fontFrame->params->pixWidth < blank)break;
        }
    }
    return 0;
}

static int32_t overlay_YUV420SP_UV_Adaptive(iftColorYUV *bgColor, iftColorYUV *ftColor, iftRect *canRect,
        SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    int y_off = 0, font_y_off = 0;
    //int uv_off = 0;
    int y_cdt = 0;//, blank = 0;//, uv_cdt = 0
    DEF_ASS_FRMDATA(fontFrame);
    DEF_ASS_FRMDATA(backgroundFrame);

    if ((fontFrame->params->type != E_Frame_GRAY256)
            && (backgroundFrame->params->type != E_Frame_YUV420SP_UV))
    {
        MYTRACE();
        return -1;
    }
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        if (fontFrame->data.un.len < fontFrame->params->pixHight * fontFrame->params->pixLinePitch)
        {
            MYTRACE();
            return -1;
        }
        if (backgroundFrame->data.un.len
                < DEF_YUV420BUFF_BYTELEN(backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch))
        {
            MYTRACE();
            return -1;
        }

    }
    else if (backgroundFrame->dtype == E_FRMDATA_TYPE_SPAN)
    {
        //need 2span when type is E_Frame_YUV420SP_UV
        if (!backgroundFrame->data.span[0].d || !backgroundFrame->data.span[1].d)
        {
            MYTRACE();
            return -1;
        }
    }
    else
    {
        MYTRACE();
        return -1;
    }

    struct SPlansHdr plansHdr;
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        plansHdr.plan_y = backgroundFrame->data.un.d;
        plansHdr.plan_uv = (struct SUVNode *)(backgroundFrame->data.un.d
                                              + backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight);
    }
    else
    {
        plansHdr.plan_y = backgroundFrame->data.span[0].d;
        plansHdr.plan_uv = (struct SUVNode *) backgroundFrame->data.span[1].d;
    }
    /**
      * revise origin coordinate
      */
    //canRect->orgi.x =MOD_CEILING(canRect->orgi.x,2);
    //canRect->orgi.y =MOD_CEILING(canRect->orgi.y,2);

    int bgnpos_x;
    int endpos_x;
    int bgnpos_y;
    int endpos_y;
    int pos_y, pos_x;
    int ftpos_x, ftpos_y; //font frame
    bgnpos_x = backgroundFrame->bgn_point.x;
    endpos_x = backgroundFrame->end_point.x;
    bgnpos_y = backgroundFrame->bgn_point.y;
    endpos_y = backgroundFrame->end_point.y;

    //printf("bgnpos_x = %d, endpos_x = %d, bgnpos_y = %d, endpos_y = %d\n", bgnpos_x, endpos_x, bgnpos_y, endpos_y);
    SGray256Sub *pos_gray;
    for (pos_y = bgnpos_y, ftpos_y = 0,
            y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
            font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
    {
        y_off      += backgroundFrame->params->pixLinePitch;
        font_y_off += fontFrame->params->pixWidth;
        //uv_off = (pos_y>>1)*(backgroundFrame->params->pixLinePitch>>1);
        for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
        {
            pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
            if (pos_gray->val > 0)
            {
                //blank = 0;
                y_cdt = y_off + pos_x;
                if (plansHdr.plan_y[y_cdt] > 100) { plansHdr.plan_y[y_cdt] = 0; }
                else { plansHdr.plan_y[y_cdt] = 240; }
            }
            //else
            //{
            //    blank++;
            //}

        }
        //if(fontFrame->params->pixWidth < blank)break;
    }
    return 0;
}

static int32_t overlay_YUV420SP_UV_Color(iftColorYUV *bgColor, iftColorYUV *ftColor, iftRect *canRect,
        SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    volatile int y_off = 0, font_y_off = 0;
    volatile int uv_off = 0;
    volatile int y_cdt = 0, uv_cdt = 0;//, blank = 0;;
    volatile int offset = 0;
    DEF_ASS_FRMDATA(fontFrame);
    DEF_ASS_FRMDATA(backgroundFrame);

    if ((fontFrame->params->type != E_Frame_GRAY256)
            && (backgroundFrame->params->type != E_Frame_YUV420SP_UV))
    {
        MYTRACE();
        return -1;
    }
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        if (fontFrame->data.un.len < fontFrame->params->pixHight * fontFrame->params->pixLinePitch)
        {
            MYTRACE();
            return -1;
        }
        if (backgroundFrame->data.un.len
                < DEF_YUV420BUFF_BYTELEN(backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch))
        {
            MYTRACE();
            return -1;
        }

    }
    else if (backgroundFrame->dtype == E_FRMDATA_TYPE_SPAN)
    {
        //need 2span when type is E_Frame_YUV420SP_UV
        if (!backgroundFrame->data.span[0].d || !backgroundFrame->data.span[1].d)
        {
            MYTRACE();
            return -1;
        }
    }
    else
    {
        MYTRACE();
        return -1;
    }

    struct SPlansHdr plansHdr;
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        plansHdr.plan_y = backgroundFrame->data.un.d;
        plansHdr.plan_uv = (struct SUVNode *)(backgroundFrame->data.un.d
                                              + backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight);
    }
    else
    {
        plansHdr.plan_y = backgroundFrame->data.span[0].d;
        plansHdr.plan_uv = (struct SUVNode *) backgroundFrame->data.span[1].d;
    }
    /**
         * revise origin coordinate
         */
    //canRect->orgi.x =MOD_CEILING(canRect->orgi.x,2);
    //canRect->orgi.y =MOD_CEILING(canRect->orgi.y,2);

    volatile int bgnpos_x;
    volatile int endpos_x;
    volatile int bgnpos_y;
    volatile int endpos_y;
    volatile int pos_y, pos_x;
    volatile int ftpos_x, ftpos_y; //font frame
    bgnpos_x = backgroundFrame->bgn_point.x;
    endpos_x = backgroundFrame->end_point.x;
    bgnpos_y = backgroundFrame->bgn_point.y;
    endpos_y = backgroundFrame->end_point.y;

    //printf("bgnpos_x = %d, bgnpos_y = %d, endpos_x = %d, endpos_y = %d\n", bgnpos_x, bgnpos_y, endpos_x, endpos_y);
    SGray256Sub *pos_gray;
    //!2 pixes offset for semi
    if (canRect->semi == FONT_BG_MODE_SEMI)
    {
        for (offset = -2, pos_y = bgnpos_y,
                y_off = (pos_y - 3) * backgroundFrame->params->pixLinePitch; offset < 0; offset++)
        {
            y_off += backgroundFrame->params->pixLinePitch;
            for (pos_x = bgnpos_x; pos_x < endpos_x; pos_x++)
            {
                y_cdt = y_off + pos_x;
                plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
            }
        }
        //!for osd and semi transparent backgroud
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            uv_off = (pos_y >> 1) * (backgroundFrame->params->pixLinePitch >> 1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = ftColor->y;
                    if ((pos_y & 0x1) || (pos_x & 0x1))
                    {
                        continue;
                    }
                    uv_cdt = uv_off + (pos_x >> 1);
                    plansHdr.plan_uv[uv_cdt].u = ftColor->u;
                    plansHdr.plan_uv[uv_cdt].v = ftColor->v;
                }
                else
                {
                    //blank++;
                    y_cdt = y_off + pos_x;
                    plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
                }
            }
            //if((fontFrame->params->pixWidth<<1) < blank)break;
        }
    }
    else
    {
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            uv_off = (pos_y >> 1) * (backgroundFrame->params->pixLinePitch >> 1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = ftColor->y;

                    if ((pos_y % 2) || (pos_x % 2))
                    {
                        continue;
                    }
                    uv_cdt = uv_off + (pos_x >> 1);
                    plansHdr.plan_uv[uv_cdt].u = ftColor->u;
                    plansHdr.plan_uv[uv_cdt].v = ftColor->v;

                }
                //else
                //{
                //    blank++;
                //}
            }
            //if(fontFrame->params->pixWidth < blank)break;
        }
    }
    return 0;
}

static int32_t overlay_YUV420SP_VU_Color(iftColorYUV *bgColor, iftColorYUV *ftColor, iftRect *canRect,
        SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    volatile int y_off = 0, font_y_off = 0;
    volatile int uv_off = 0;
    volatile int y_cdt = 0, uv_cdt = 0;//, blank = 0;;
    volatile int offset = 0;
    DEF_ASS_FRMDATA(fontFrame);
    DEF_ASS_FRMDATA(backgroundFrame);

    if ((fontFrame->params->type != E_Frame_GRAY256)
            && (backgroundFrame->params->type != E_Frame_YUV420SP_VU))
    {
        MYTRACE();
        return -1;
    }
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        if (fontFrame->data.un.len < fontFrame->params->pixHight * fontFrame->params->pixLinePitch)
        {
            MYTRACE();
            return -1;
        }
        if (backgroundFrame->data.un.len
                < DEF_YUV420BUFF_BYTELEN(backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch))
        {
            MYTRACE();
            return -1;
        }

    }
    else if (backgroundFrame->dtype == E_FRMDATA_TYPE_SPAN)
    {
        //need 2span when type is E_Frame_YUV420SP_UV
        if (!backgroundFrame->data.span[0].d || !backgroundFrame->data.span[1].d)
        {
            MYTRACE();
            return -1;
        }
    }
    else
    {
        MYTRACE();
        return -1;
    }

    struct SPlansHdr plansHdr;
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        plansHdr.plan_y = backgroundFrame->data.un.d;
        plansHdr.plan_uv = (struct SUVNode *)(backgroundFrame->data.un.d
                                              + backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight);
    }
    else
    {
        plansHdr.plan_y = backgroundFrame->data.span[0].d;
        plansHdr.plan_uv = (struct SUVNode *) backgroundFrame->data.span[1].d;
    }
    /**
         * revise origin coordinate
         */
    //canRect->orgi.x =MOD_CEILING(canRect->orgi.x,2);
    //canRect->orgi.y =MOD_CEILING(canRect->orgi.y,2);

    volatile int bgnpos_x;
    volatile int endpos_x;
    volatile int bgnpos_y;
    volatile int endpos_y;
    volatile int pos_y, pos_x;
    volatile int ftpos_x, ftpos_y; //font frame
    bgnpos_x = backgroundFrame->bgn_point.x;
    endpos_x = backgroundFrame->end_point.x;
    bgnpos_y = backgroundFrame->bgn_point.y;
    endpos_y = backgroundFrame->end_point.y;

    //printf("bgnpos_x = %d, bgnpos_y = %d, endpos_x = %d, endpos_y = %d\n", bgnpos_x, bgnpos_y, endpos_x, endpos_y);
    SGray256Sub *pos_gray;
    //!2 pixes offset for semi
    if (canRect->semi == FONT_BG_MODE_SEMI)
    {
        for (offset = -2, pos_y = bgnpos_y,
                y_off = (pos_y - 3) * backgroundFrame->params->pixLinePitch; offset < 0; offset++)
        {
            y_off += backgroundFrame->params->pixLinePitch;
            for (pos_x = bgnpos_x; pos_x < endpos_x; pos_x++)
            {
                y_cdt = y_off + pos_x;
                plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
            }
        }
        //!for osd and semi transparent backgroud
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            uv_off = (pos_y >> 1) * (backgroundFrame->params->pixLinePitch >> 1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = ftColor->y;
                    if ((pos_y & 0x1) || (pos_x & 0x1))
                    {
                        continue;
                    }
                    uv_cdt = uv_off + (pos_x >> 1);
                    plansHdr.plan_uv[uv_cdt].v = ftColor->u;
                    plansHdr.plan_uv[uv_cdt].u = ftColor->v;
                }
                else
                {
                    //blank++;
                    y_cdt = y_off + pos_x;
                    plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
                }
            }
            //if((fontFrame->params->pixWidth<<1) < blank)break;
        }
    }
    else
    {
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            uv_off = (pos_y >> 1) * (backgroundFrame->params->pixLinePitch >> 1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = ftColor->y;

                    if ((pos_y % 2) || (pos_x % 2))
                    {
                        continue;
                    }
                    uv_cdt = uv_off + (pos_x >> 1);
                    plansHdr.plan_uv[uv_cdt].v = ftColor->u;
                    plansHdr.plan_uv[uv_cdt].u = ftColor->v;

                }
                //else
                //{
                //    blank++;
                //}
            }
            //if(fontFrame->params->pixWidth < blank)break;
        }
    }
    return 0;
}




static int32_t overlay_YUV420SP_UV(iftColorYUV *bgColor, iftColorYUV *ftColor, iftRect *canRect,
                                   SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    int y_off, font_y_off, font_y_off_next;
    int pos;//, blank = 0;;
    // int charlenth = ch_width * ch_high;
    DEF_ASS_FRMDATA(fontFrame);
    DEF_ASS_FRMDATA(backgroundFrame);
    iftColorYUV overline_color;
    overline_color.y = 0;
    overline_color.u = 120;
    overline_color.v = 120;
    //int statistics = 0;

    if ((fontFrame->params->type != E_Frame_GRAY256)
            && (backgroundFrame->params->type != E_Frame_YUV420SP_UV))
    {
        MYTRACE();
        return -1;
    }
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        if (fontFrame->data.un.len < fontFrame->params->pixHight * fontFrame->params->pixLinePitch)
        {
            MYTRACE();
            return -1;
        }
        if (backgroundFrame->data.un.len
                < DEF_YUV420BUFF_BYTELEN(backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch))
        {
            MYTRACE();
            return -1;
        }

    }
    else if (backgroundFrame->dtype == E_FRMDATA_TYPE_SPAN)
    {
        //need 2span when type is E_Frame_YUV420SP_UV
        if (!backgroundFrame->data.span[0].d || !backgroundFrame->data.span[1].d)
        {
            MYTRACE();
            return -1;
        }
    }
    else
    {
        MYTRACE();
        return -1;
    }
    struct SPlansHdr plansHdr;
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        plansHdr.plan_y = backgroundFrame->data.un.d;
        plansHdr.plan_uv = (struct SUVNode *)(backgroundFrame->data.un.d
                                              + backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight);
    }
    else
    {
        plansHdr.plan_y = backgroundFrame->data.span[0].d;
        plansHdr.plan_uv = (struct SUVNode *) backgroundFrame->data.span[1].d;
    }
    /**
     * revise origin coordinate
     */
    //canRect->orgi.x =MOD_CEILING(canRect->orgi.x,2);
    //canRect->orgi.y =MOD_CEILING(canRect->orgi.y,2);

    int bgnpos_x;
    int endpos_x;
    int bgnpos_y;
    int endpos_y;
    int pos_y, pos_x;
    int ftpos_x, ftpos_y; //font frame
    bgnpos_x = backgroundFrame->bgn_point.x;
    endpos_x = backgroundFrame->end_point.x;
    bgnpos_y = backgroundFrame->bgn_point.y;
    endpos_y = backgroundFrame->end_point.y;

    //   int32_t frm_yuv_pos_y ;
    //  int32_t ch_gray_pos ;
    //printf("bgnpos_x = %d, endpos_x = %d, bgnpos_y = %d, endpos_y = %d\n", bgnpos_x, endpos_x, bgnpos_y, endpos_y);
    SGray256Sub *pos_gray;
    //SGray256Sub *pos_gray_next_y = NULL;

    for (pos_y = bgnpos_y, ftpos_y = 0, y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
            font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
    {
        //#define DEF_FRM_POS_PHY_Y DEF_FRM_ARRY2D_POS(pos_y,backgroundFrame->params->pixLinePitch,pos_x)
        //#define DEF_FRM_POS_PHY_UV DEF_FRM_ARRY2D_POS(pos_y / 2,backgroundFrame->params->pixLinePitch/2,pos_x/2)
        y_off += backgroundFrame->params->pixLinePitch;
        font_y_off += fontFrame->params->pixWidth;
        font_y_off_next = font_y_off + fontFrame->params->pixWidth;

        for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
        {
            //  frm_yuv_pos_y = pos_y * backgroundFrame->params->pixWidth + pos_x;
            //  ch_gray_pos = ftpos_y * fontFrame->params->pixWidth + ftpos_x;
            pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
            //if(pos_y< endpos_y-1)
            //{
            //    pos_gray_next_y = (SGray256Sub*)get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off_next, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y+1, 1);
            //}
            if (!pos_gray)
            {
                MYTRACE();
                continue;
            }
#if 0
            iftColorYUV *colorDraw ;
            SGray256Sub cmpare_sub ;;
            int bset_frame_pos = 1 ;
#endif
#if 0
            if ((pos_x == 2) && (pos_y == 8))
            {
                MYTRACE();
            }
#endif

#if 0
            do
            {

                if (pos_gray->val > 0)
                {
                    cmpare_sub = *pos_gray ;
                    break;
                }
                if ((((ftpos_x + 1) % fontFrame->params->pixWidth) != 0) && ((pos_gray + 1)->val > 0))
                {
                    cmpare_sub = *(pos_gray + 1);
                    break;
                }
                if (pos_gray_next_y)
                {
                    if (pos_gray_next_y->val > 0)
                    {
                        cmpare_sub = *pos_gray_next_y ;
                        break;
                    }
                    if ((((ftpos_x + 1) % fontFrame->params->pixWidth) != 0) && ((pos_gray_next_y + 1)->val > 0))
                    {
                        cmpare_sub = *(pos_gray_next_y + 1);
                        break;
                    }
                }
                /**
                 * not set frame
                 */
                bset_frame_pos = 0;
                colorDraw = bgColor ;
            }
            while (0);

            if (bset_frame_pos)
            {
                switch (cmpare_sub.sub.type)
                {
                    case E_GRAY256SUB_TYPE_OUTLINE:
                        colorDraw = &overline_color;
                        break;
                    default:
                        colorDraw = ftColor;
                        break;
                }
            }
            uint8_t draw_y_data = 0;
#endif

            if (pos_gray->val > 0)
            {
                //blank = 0;
                pos = y_off + pos_x;
                switch (pos_gray->sub.type)
                {
                    case E_GRAY256SUB_TYPE_OUTLINE:
                        plansHdr.plan_y[pos] = overline_color.y;
                        break;
                    default:
                        //statistics++;
                        //if(plansHdr.plan_y[pos]>60)plansHdr.plan_y[pos] = 0;
                        //else plansHdr.plan_y[pos] = 240;
                        //plansHdr.plan_y[pos] = plansHdr.plan_y[pos]>40?0:255;//ftColor->y;
                        plansHdr.plan_y[pos] = 240;
                        break;
                }
            }
            else
            {
                //blank++;
                pos = y_off + pos_x;
                plansHdr.plan_y[pos] = plansHdr.plan_y[pos] * 0.7;
#if 0
                if ((pos_x % 2) != 0)
                {
                    continue;
                }
                if (pos_y % 2 == 0)
                {
                    plansHdr.plan_uv[DEF_FRM_POS_PHY_UV].u = 0;//plansHdr.plan_uv[DEF_FRM_POS_PHY_UV].u * 0.8;
                    plansHdr.plan_uv[DEF_FRM_POS_PHY_UV].v = 0;//plansHdr.plan_uv[DEF_FRM_POS_PHY_UV].v * 0.8;
                }
#endif
            }
#if 0

            // uint8_t pst = (pos_gray ? 240 : ((pos_gray > 240) ? 240 : pos_gray));
            // if ((bgColor->a == 0)&&((pos_gray->val == 0)&& (pos_gray_next_y->val == 0 )) )
            if ((bgColor->a == 0) && !bset_frame_pos)
            {
                //tran
                continue;
            }
            /**
             * set uvpix data
             */

            {

                if ((pos_x % 2) != 0)
                {
                    continue;
                }

                // int32_t frm_yuv_pos_uv = (pos_y / 2) * (backgroundFrame->params->pixWidth / 2) + (pos_x / 2);
                if (pos_y % 2 == 0)
                {
                    if (bset_frame_pos)
                    {
                        plansHdr.plan_uv[DEF_FRM_POS_PHY_UV].u = colorDraw->u;
                        plansHdr.plan_uv[DEF_FRM_POS_PHY_UV].v = colorDraw->v;
                    }
                }
                //#if 0
                else
                {
                    plansHdr.plan_uv[DEF_FRM_POS_PHY_UV].v = colorDraw->v;
                }
                //#endif
            }
#endif
        }
        //if(fontFrame->params->pixWidth < blank)break;
    }
    // dumpframe(backgroundFrame,0);
    //printf("======================statistics = %d=====================\n", statistics);
    return 0;
}

static int32_t overlay_YUV420P_I420_Color(iftColorYUV *bgColor, iftColorYUV *ftColor, iftRect *canRect,
        SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    volatile int y_off = 0, font_y_off = 0;
    volatile int uv_off = 0;
    volatile int y_cdt = 0, uv_cdt = 0;//, blank = 0;;
    volatile int offset = 0;
    DEF_ASS_FRMDATA(fontFrame);
    DEF_ASS_FRMDATA(backgroundFrame);
/*
    if ((fontFrame->params->type != E_Frame_GRAY256)
            && (backgroundFrame->params->type != E_Frame_YUV420SP_UV))
    {
        MYTRACE();
        return -1;
    }
*/    
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        if (fontFrame->data.un.len < fontFrame->params->pixHight * fontFrame->params->pixLinePitch)
        {
            MYTRACE();
            return -1;
        }
        if (backgroundFrame->data.un.len
                < DEF_YUV420BUFF_BYTELEN(backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch))
        {
            MYTRACE();
            return -1;
        }

    }
    else if (backgroundFrame->dtype == E_FRMDATA_TYPE_SPAN)
    {
        //need 2span when type is E_Frame_YUV420SP_UV
        if (!backgroundFrame->data.span[0].d || !backgroundFrame->data.span[1].d)
        {
            MYTRACE();
            return -1;
        }
    }
    else
    {
        MYTRACE();
        return -1;
    }

    struct SPlansHdr plansHdr;
    /*  
    if (backgroundFrame->dtype == E_FRMDATA_TYPE_UNION)
    {
        plansHdr.plan_y = backgroundFrame->data.un.d;
        plansHdr.plan_i420 = (struct SUVNode_I420 *)(backgroundFrame->data.un.d
                                              + backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight);
    }
    else
    */    
    {
        plansHdr.plan_y = backgroundFrame->data.span[0].d;
        plansHdr.I420_U = backgroundFrame->data.span[1].d;
        plansHdr.I420_V = plansHdr.I420_U + (backgroundFrame->params->pixWidth * backgroundFrame->params->pixHight / 4);
    }
    /**
         * revise origin coordinate
         */
    //canRect->orgi.x =MOD_CEILING(canRect->orgi.x,2);
    //canRect->orgi.y =MOD_CEILING(canRect->orgi.y,2);

    volatile int bgnpos_x;
    volatile int endpos_x;
    volatile int bgnpos_y;
    volatile int endpos_y;
    volatile int pos_y, pos_x;
    volatile int ftpos_x, ftpos_y; //font frame
    bgnpos_x = backgroundFrame->bgn_point.x;
    endpos_x = backgroundFrame->end_point.x;
    bgnpos_y = backgroundFrame->bgn_point.y;
    endpos_y = backgroundFrame->end_point.y;

    //printf("2222bgnpos_x = %d, bgnpos_y = %d, endpos_x = %d, endpos_y = %d\n", bgnpos_x, bgnpos_y, endpos_x, endpos_y);
    SGray256Sub *pos_gray;

    //!2 pixes offset for semi
    if (canRect->semi == FONT_BG_MODE_SEMI)
    {
        /*
        for (offset = -2, pos_y = bgnpos_y,
                y_off = (pos_y - 3) * backgroundFrame->params->pixLinePitch; offset < 0; offset++)
        {
            y_off += backgroundFrame->params->pixLinePitch;
            for (pos_x = bgnpos_x; pos_x < endpos_x; pos_x++)
            {
                y_cdt = y_off + pos_x;
                plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
            }
        }
        //!for osd and semi transparent backgroud
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            uv_off = (pos_y >> 1) * (backgroundFrame->params->pixLinePitch >> 1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = ftColor->y;
                    if ((pos_y & 0x1) || (pos_x & 0x1))
                    {
                        continue;
                    }
                    uv_cdt = uv_off + (pos_x >> 1);
                    plansHdr.plan_uv[uv_cdt].u = ftColor->u;
                    plansHdr.plan_uv[uv_cdt].v = ftColor->v;
                }
                else
                {
                    //blank++;
                    y_cdt = y_off + pos_x;
                    plansHdr.plan_y[y_cdt] = (plansHdr.plan_y[y_cdt] >> 1) + (plansHdr.plan_y[y_cdt] >> 2);
                }
            }
            //if((fontFrame->params->pixWidth<<1) < blank)break;
        }
        */
    }
    else
    {
        for (pos_y = bgnpos_y, ftpos_y = 0,
                y_off = (pos_y - 1) * backgroundFrame->params->pixLinePitch,
                font_y_off = (ftpos_y - 1) * fontFrame->params->pixWidth; pos_y < endpos_y; pos_y++, ftpos_y++)
        {
            y_off      += backgroundFrame->params->pixLinePitch;
            font_y_off += fontFrame->params->pixWidth;
            //  uv_off = (pos_y >> 1) * (backgroundFrame->params->pixLinePitch >> 1);
            for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
            {
                pos_gray = (SGray256Sub*) get_framePos_Optimized_ptr(fontFrame, ftpos_x, font_y_off, 1);//get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
                if (pos_gray->val > 0)
                {
                    //blank = 0;
                    plansHdr.plan_y[y_off + pos_x] = ftColor->y;//每个像素点都要填充Y

                    if ((pos_y % 2) || (pos_x % 2))//每四个Y只有一次需要填充UV
                    {
                        continue;
                    }
                    
                    uv_off = (pos_y >> 1) * (backgroundFrame->params->pixLinePitch >> 1);
                    uv_cdt = uv_off  + (pos_x >> 1);
                    
                    plansHdr.I420_U[uv_cdt] = ftColor->u;
                    plansHdr.I420_V[uv_cdt] = ftColor->v;                  

                }
                //else
                //{
                //    blank++;
                //}
            }
            //if(fontFrame->params->pixWidth < blank)break;
        }
    }
    
    return 0;
}                                   
static int32_t overlay_frame_RGB_24(iftColor *bgColor, iftColor *ftColor, iftRect *canRect,
                                    SFrameData *fontFrame, SFrameData *backgroundFrame)
{

    assert(backgroundFrame->dtype == E_FRMDATA_TYPE_UNION);
    assert(backgroundFrame->params->type == E_Frame_RGB_24);

    int32_t bgnpos_x = canRect->orgi.x;
    int32_t endpos_x = canRect->orgi.x + fontFrame->params->pixWidth;
    int32_t bgnpos_y = canRect->orgi.y;
    int32_t endpos_y = canRect->orgi.y + fontFrame->params->pixHight;
    int32_t pos_y, pos_x;
    int32_t ftpos_x, ftpos_y; //font frame
    //uint8_t pos_gray = 0;
#define DEF_RGB_FRONT 0
#define DEF_RGB_OUTLINE 1
    uint8_t draw_rgb24[2][3] =
    {
        {
            ftColor->r,
            ftColor->g,
            ftColor->b
        },
        {
            30,//outline color
            30,
            30
        }
    };

    for (pos_y = bgnpos_y, ftpos_y = 0; pos_y < endpos_y; pos_y++, ftpos_y++)
    {
        for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
        {

            // pos_gray = *get_framePos_ptr(fontFrame, ftpos_x, ftpos_y,1);
            SGray256Sub cmpare_sub = (SGray256Sub) * get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);

            if ((bgColor->a == 0) && (cmpare_sub.val == 0))
            {
                //tran
                continue;
            }
            int clor_idx = DEF_RGB_FRONT;
            switch (cmpare_sub.sub.type)
            {
                case E_GRAY256SUB_TYPE_OUTLINE:
                {
                    clor_idx = DEF_RGB_OUTLINE;
                    break;
                }
                default:
                {
                    clor_idx = DEF_RGB_FRONT;
                }
            }
            memcpy(get_framePos_ptr(backgroundFrame, pos_x, pos_y, 3), &draw_rgb24[clor_idx][0], 3);
        }
    }
    return 0 ;

}
static int32_t overlay_frame_RGB_1555(iftColor *bgColor, iftColor *ftColor, iftRect *canRect,
                                      SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    int32_t bgnpos_x = backgroundFrame->bgn_point.x;
    int32_t endpos_x = backgroundFrame->end_point.x;
    int32_t bgnpos_y = backgroundFrame->bgn_point.y;
    int32_t endpos_y = backgroundFrame->end_point.y;
    
    int32_t pos_y, pos_x;
    int32_t ftpos_x, ftpos_y; //font frame

    SGray256Sub *pos_gray = NULL;

    //rgb_1555其实是ARGB1555,A不能为0,否则为透明 
    unsigned short rgb1555 = ( 0x8000 | ((ftColor->r >> 3) << 10) | ((ftColor->g >> 3) <<5 ) | (ftColor->b >> 3));
   

    //printf("RGB1555====[bgn.x = %d, bgn.y = %d, end.x = %d, end.y = %d, B_pixWidth = %d, B_pixHight = %d, B_pixLinePitch = %d]====\n",
    //bgnpos_x, bgnpos_y, endpos_x, endpos_y, backgroundFrame->params->pixWidth, backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch);
    
    //printf("RGB1555====[F_width = %d, F_hight = %d F_LinePitch =%d]\n",fontFrame->params->pixWidth,fontFrame->params->pixHight,fontFrame->params->pixLinePitch);

    for (pos_y = bgnpos_y, ftpos_y = 0; pos_y < endpos_y; pos_y++, ftpos_y++)
    {
        for (pos_x = bgnpos_x, ftpos_x = 0; pos_x < endpos_x; pos_x++, ftpos_x++)
        {
            pos_gray = (SGray256Sub*) get_framePos_ptr(fontFrame, ftpos_x, ftpos_y, 1);
            //printf("%02d",pos_gray->val);//这样可以打印出font字符,可以看出来有点的地方灰度值还不一样的。

            if(pos_gray->val > 0)
            {
                //memcpy(get_framePos_ptr_new(backgroundFrame, ftpos_x, ftpos_y, 2), &rgb1555, 2);
                memcpy(&backgroundFrame->data.un.d[pos_y*backgroundFrame->params->pixLinePitch + pos_x*2], &rgb1555, 2);
            }            
        }
    }
                                                                   
    return 0 ;

}

#define SCREEN_OSD_BORDER_MARGIN    (20)
#define SCREEN_OSD_DETECT_MARGIN    (10)

int32_t iftFrmOper_overlay(iftColor *fontColor, iftCanvasInt *canv, SFrameData *fontFrame, SFrameData *backgroundFrame)
{
    assert(canv && fontFrame && backgroundFrame);

    assert(fontFrame->dtype == E_FRMDATA_TYPE_UNION);
    if (fontFrame->params->type != E_Frame_GRAY256)
    {
        MYTRACE();
        return -1;
    }
    //todo process overlay
    /**
     * convert color
     */
    iftColorYUV yuvBgClr, yuvFtclr;
    IFT_RGB2YUV_Color(fontColor, &yuvFtclr);
    //printf("r = %d, g = %d, b = %d\n", fontColor->r, fontColor->g, fontColor->b);
    IFT_RGB2YUV_Color(&canv->bgColor, &yuvBgClr);

    
    int nret = -1;

    int bgnpos_x;
    int endpos_x;
    int bgnpos_y;
    int endpos_y;
    int maxWidth = 0;
    bgnpos_y = canv->rect.orgi.y;
    endpos_y = canv->rect.orgi.y + fontFrame->params->pixHight;
    if (endpos_y > backgroundFrame->params->pixHight) { return -1; }
    if (canv->position == OSD_POS_TOP_RIGHT || canv->position == OSD_POS_BOTTOM_RIGHT)
    {
        if ((fontFrame->params->pixWidth - canv->maxFontWidth) > SCREEN_OSD_DETECT_MARGIN)
        {
            maxWidth = fontFrame->params->pixWidth;
        }
        else
        {
            maxWidth = canv->maxFontWidth;
        }
        bgnpos_x = backgroundFrame->params->pixWidth - canv->rect.orgi.x - maxWidth;
        if (bgnpos_x < SCREEN_OSD_BORDER_MARGIN)
        {
            bgnpos_x = SCREEN_OSD_BORDER_MARGIN;
        }
        endpos_x = bgnpos_x + fontFrame->params->pixWidth;
        if (endpos_x > (backgroundFrame->params->pixWidth - canv->rect.orgi.x))
        {
            endpos_x = (backgroundFrame->params->pixWidth - canv->rect.orgi.x);
        }
    }
    else
    {
        bgnpos_x = canv->rect.orgi.x;
        endpos_x = canv->rect.orgi.x + fontFrame->params->pixWidth;
        if (endpos_x > (backgroundFrame->params->pixWidth - SCREEN_OSD_BORDER_MARGIN))
        {
            endpos_x = backgroundFrame->params->pixWidth - SCREEN_OSD_BORDER_MARGIN;
        }
    }
    backgroundFrame->bgn_point.x = ((bgnpos_x >> 1) << 1);
    backgroundFrame->bgn_point.y = ((bgnpos_y >> 1) << 1);
    backgroundFrame->end_point.x = ((endpos_x >> 1) << 1);
    backgroundFrame->end_point.y = ((endpos_y >> 1) << 1);

    //printf("====[bgn.x = %d, bgn.y = %d, end.x = %d, end.y = %d, B_pixWidth = %d, B_pixHight = %d, B_pixLinePitch = %d]====\n",
    //bgnpos_x, bgnpos_y, endpos_x, endpos_y, backgroundFrame->params->pixWidth, backgroundFrame->params->pixHight, backgroundFrame->params->pixLinePitch);

    do
    {
        canv->rect.semi         = canv->bg_mode;
        canv->rect.position     = canv->position;
        canv->rect.maxFontWidth = canv->maxFontWidth;
        switch (backgroundFrame->params->type)
        {
            case E_Frame_YUV420SP_UV:
            {
                //printf("Font pixHight = %d, pixWidth = %d\n", fontFrame->params->pixHight, fontFrame->params->pixWidth);
                //printf("backgroundFrame pixHight = %d, pixWidth = %d\n", backgroundFrame->params->pixHight, backgroundFrame->params->pixWidth);
                //printf("rect wide = %d, hight = %d, orgi.x = %d, orgi.y = %d\n", canv->rect.wide, canv->rect.hight, canv->rect.orgi.x, canv->rect.orgi.y);
                if (canv->fg_mode == FONT_FG_MODE_BLACK) //FONT_FG_MODE_BLACK
                {
                    nret = overlay_YUV420SP_UV_Black(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                }
                else if (canv->fg_mode == FONT_FG_MODE_WHITE) //FONT_FG_MODE_WHITE
                {
                    nret = overlay_YUV420SP_UV_White(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                }
                else if (canv->fg_mode == FONT_FG_MODE_ADAPTIVE) //FONT_FG_MODE_ADAPTIVE
                {
                    nret = overlay_YUV420SP_UV_Adaptive(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                }
                else if (canv->fg_mode == FONT_FG_MODE_COLOR) //FONT_FG_MODE_COLOR
                {
                    nret = overlay_YUV420SP_UV_Color(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                    //nret = overlay_YUV420SP_UV_Black(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                }
                else//FONT_FG_MODE_BLACK
                {
                    nret = overlay_YUV420SP_UV_Color(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                }
                break;
            }
            case E_Frame_YUV420SP_VU:
            {
                //默认彩色代码，需要了再添加别的 by wanghaibo 2018.9.12
                //if (canv->fg_mode == FONT_FG_MODE_COLOR) //SWOSD_FONT_FG_COLOR;
                {
                    nret = overlay_YUV420SP_VU_Color(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                }
                break;
            }
            case E_Frame_YUV420P_I420:
            {
                nret = overlay_YUV420P_I420_Color(&yuvBgClr, &yuvFtclr, &canv->rect, fontFrame, backgroundFrame);
                break;
            }
            case E_Frame_RGB_24://(此函数暂时不能使用，需要修改2018.2.27)
            {
                nret = overlay_frame_RGB_24(&canv->bgColor, fontColor, &canv->rect, fontFrame, backgroundFrame);
                break;
            }
            case E_Frame_RGB_1555:
            {
                nret = overlay_frame_RGB_1555(&canv->bgColor, fontColor, &canv->rect, fontFrame, backgroundFrame);
                break;
            }
            default:
                MYTRACE();
                break;
        }
    }
    while (0);
    if (nret < 0)
    {
        MYTRACE();
        return -1;
    }
    return 0;
}

int32_t iftFrmOper_overlay_outline(iftColor *fontColor, iftCanvasInt *canv, SFrameData *fontFrame,
                                   SFrameData *backgroundFrame, iftFontExtra *font_ext_conf, iftRange * innerProcRange)
{
    assert(canv && fontFrame && backgroundFrame && font_ext_conf);

    assert(fontFrame->dtype == E_FRMDATA_TYPE_UNION);
    if (fontFrame->params->type != E_Frame_GRAY256)
    {
        MYTRACE();
        return -1;
    }
    if (canv->coosty != E_ORIG_STYLE_UPLEFT)
    {
        MYTRACE();
        return -1;
    }
    SCoordinate2D posRightDown =
    {
        .x = canv->rect.orgi.x + canv->rect.wide,
        .y = canv->rect.orgi.y + canv->rect.hight,
    };
    //    SCoordinate2D posLeftDown =
    //        { .x = canv->rect.orgi.x,
    //          .y = canv->rect.orgi.y + canv->rect.hight, };
    iftRect backgroundRect =
    {
        .orgi =
        {
            0,
            0,
        },
        .wide = backgroundFrame->params->pixWidth,
        .hight = backgroundFrame->params->pixHight,
    };

    //tmp only process in rect
    if (!(posInRect(canv->rect.orgi, &backgroundRect)))
    {
        MYTRACE("hight overflow");
        return -1;
    }

    if (!posInRect(posRightDown, &backgroundRect))
    {
        /**
         * need cut picx
         */
        int32_t origWideSize = fontFrame->params->pixWidth;

        int32_t cut2WideSize = fontFrame->params->pixWidth; //= backgroundRect.wide - canv->rect.orgi.x;
        int32_t cut2HightSize = fontFrame->params->pixHight; //= backgroundRect.hight - canv->rect.orgi.y;
        if (!DEF_POS_INREGIN(posRightDown.x, 0, backgroundRect.wide))
        {
            cut2WideSize = backgroundRect.wide - canv->rect.orgi.x;
        }
        if (!DEF_POS_INREGIN(posRightDown.y, 0, backgroundRect.hight))
        {
            cut2HightSize = backgroundRect.hight - canv->rect.orgi.y;
        }
#define IFT_MIN(a,b) ((a)>(b)?(b):(a))
        cut2WideSize = IFT_MIN(cut2WideSize, fontFrame->params->MaxFontAdvance);
        cut2WideSize = IFT_MIN(cut2WideSize, fontFrame->params->pixWidth);
        SCoordinate2D curops =
        {
            .x = 0,
            .y = 0
        };
        int32_t wtpos = cut2WideSize;
        if (cut2WideSize < origWideSize)
        {
            for (curops.y = 1; curops.y < cut2HightSize; curops.y++)
            {
                memcpy(&fontFrame->data.un.d[wtpos],
                       &fontFrame->data.un.d[curops.y * origWideSize],
                       cut2WideSize);
                wtpos += cut2WideSize;
            }
        }
        memset(&fontFrame->data.un.d[wtpos], 0, (fontFrame->data.un.len - wtpos));
        fontFrame->params->pixHight = cut2HightSize;
        fontFrame->params->pixWidth = cut2WideSize;
        fontFrame->data.un.len = cut2HightSize * cut2WideSize;

    }
    assert(fontFrame->dtype == E_FRMDATA_TYPE_UNION);
    if (!(canv->rect.hight == fontFrame->params->pixHight)
            || !(canv->rect.wide >= fontFrame->params->pixWidth))
    {
        dprintf(DBG_LEV_INF,
                "H/W,canvas=%d/%d,fontFrame=%d/%d",
                canv->rect.hight, canv->rect.wide, fontFrame->params->pixHight, fontFrame->params->pixWidth);
        return -1;
    }
    //todo process overlay
    /**
     * convert color
     */
    iftColorYUV yuvBgClr, yuvFtclr;
    IFT_RGB2YUV_Color(fontColor, &yuvFtclr);
    IFT_RGB2YUV_Color(&canv->bgColor, &yuvBgClr);
    if (!((backgroundFrame->params->type == E_Frame_YUV420SP_UV)
            || (backgroundFrame->params->type == E_Frame_RGB_24)))
    {
        MYTRACE();
        return -1;
    }
    int nret = -1;
    SFrameData *font_banding_Frame = NULL;
    do
    {
        /**
            *  黑色描边效果
            * banding
            */
        iftColor rgb_banding_color =
        {
            0,
            0,
            0
        };
        iftColorYUV yuv_banding_color =
        {
            .y = 0,
            .u = 128,
            .v = 128
        };
        if (font_ext_conf->bOutline)
        {
            font_banding_Frame = (SFrameData *) iftMemAlloc(sizeof(SFrameData));
            if (!font_banding_Frame)
            {
                MYTRACE();
                break;
            }
            dprintf(DBG_LEV_DBG,
                    "font_banding_Frame sizeof(SFrameData)=%d,sizeof(font_banding_Frame)=%d",
                    sizeof(SFrameData), sizeof(font_banding_Frame));
            memset(font_banding_Frame, 0, sizeof(SFrameData));
            if (!banding_frame_gray_data_alloc(font_ext_conf->outLinePix, fontFrame, font_banding_Frame, innerProcRange))
            {
                MYTRACE();
                break;
            }
        }
        //         dumpframe( fontFrame,0);
        //         dumpframe( font_banding_Frame,1);

        switch (backgroundFrame->params->type)
        {
            case E_Frame_YUV420SP_UV:
            {
                if (font_ext_conf->bOutline)
                {
                    nret = overlay_YUV420SP_UV(&yuvBgClr, &yuv_banding_color, &canv->rect, font_banding_Frame, backgroundFrame);
                }
                break;
            }
            case E_Frame_RGB_24:
            {
                if (font_ext_conf->bOutline)
                {
                    nret = overlay_frame_RGB_24(&canv->bgColor,
                                                &rgb_banding_color,
                                                &canv->rect,
                                                font_banding_Frame,
                                                backgroundFrame);
                }
                break;
            }
            default:
                MYTRACE();
                break;
        }
    }
    while (0);
    if (font_banding_Frame)
    {
        banding_frame_gray_data_free(font_banding_Frame);
        iftMemFree(font_banding_Frame);
        font_banding_Frame = NULL ;
    }
    if (nret < 0)
    {
        MYTRACE();
        return -1;
    }
    return 0;
}

