/**
 * @file osdFontGen.c
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

#include "iftosdint.h"
#include "iftconfig.h"
#include "iftstring.h"

#include "tgadump.h"
//#include
#define MAX_FTGEN_LEN 5
//static void dumpBitMap(const FT_Bitmap * bmp, int b_num);
static iftFtgen gs_FtgenTable[MAX_FTGEN_LEN];
inline int32_t iftFtgen_renderFontTableSetopt(iftFtgen_H hfg, iftRect *srect, iftStyle *psty); //default use RBGA

static int ftCoreRenderMeta(iftFtgen_H fg, iftChar32 ucs4, iftFontGlyphUnit *fmm);
inline static int ucs4hashMeta_detach(iftUcs4HashTable *table, iftUcs4HashMeta *p);
/////////////////////////////////////////////////////////////////////

#define WIDTH   80
#define HEIGHT  60
/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

/* Replace this function with something useful. */

void draw_bitmap(FT_Bitmap* bitmap, FT_Int x, FT_Int y)
{
    FT_Int i, j, p, q;
    FT_Int x_max = x + bitmap->width;
    FT_Int y_max = y + bitmap->rows;
    memset(image, 0, WIDTH * HEIGHT);
    for (i = x, p = 0; i < x_max; i++, p++)
    {
        for (j = y, q = 0; j < y_max; j++, q++)
        {
            if (i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT)
            {
                continue;
            }
            image[j][i] |= bitmap->buffer[q * bitmap->width + p];
        }
    }
}

void show_image()
{
    int i, j;

    for (i = 0; i < HEIGHT; i++)
    {
        for (j = 0; j < WIDTH; j++)
        {
            putchar(image[i][j] == 0 ? ' ' : image[i][j] < 128 ? '+' : '*');
        }
        putchar('\n');
    }
}
void show_distimage(const unsigned char * img, int hight, int width)
{
    int i, j;

    for (i = 0; i < hight; i++)
    {
        for (j = 0; j < width; j++)
        {
            unsigned char bt = img[i * width + j];
            char tmp = bt / 16;
            if (bt && (tmp == 0))
            {
                tmp = 1 ;
            }
            if (tmp >= 10)
            {
                tmp = 'A' + tmp - 10;
            }
            else
            {
                tmp = '0' + tmp;
            }
            putchar((bt == 0) ? '0' : tmp);
            //char c = img[i * width + j] == 0 ? '0' : img[i * width + j] < 128 ? '+' : '*';
            // putchar(c);

        }
        putchar('\n');
    }
}
#if DBG_TEST_GraySpan
typedef struct SGraySpan
{
    SCoordinate2D coo;
    uint16_t len;
    SGray256Sub graysub;
    uint8_t res[1] ;
} SGraySpan;
typedef struct SFontSpansVec
{
    uint16_t hight;
    uint16_t width;
    uint16_t pitch;
    uint16_t cnt;
    SGraySpan vec[0];
} SFontSpansVec ;
void show_fontimgspans(SFontSpansVec *sps)
{
    //   int i, j;
    static uint8_t img_buf[DEF_MEMMETA_SIZE];
    memset(img_buf, 0, DEF_MEMMETA_SIZE);

    int i = 0 ;
    for (; i < sps->cnt; i++)
    {
        if (sps->vec[i].coo.x + sps->vec[i].len - 1 >= sps->width)
        {
            dprintf(DBG_LEV_ERR, "error in SFontSpans");
            return ;
        }
        memset((img_buf + (sps->hight - sps->vec[i].coo.y)*sps->width + sps->vec[i].coo.x)
               , sps->vec[i].graysub.val
               , sps->vec[i].len);
    }
    show_distimage(img_buf, sps->hight, sps->width);
    return ;
}
///
/////////////////////////////////////////////////////////////////////
/**
 * SPANS list
  */
void
RasterCallback(const int y,
               const int count,
               const FT_Span * const spans,
               void * const user)
{
    SFontSpansVec *sptr = (SFontSpansVec *)user;
    int i = 0;
    for (; i < count; i++)
    {
        if (sptr->cnt >= 2048)
        {
            break;
        }
        SGraySpan *sp =  &sptr->vec[ sptr->cnt];
        sp->coo.x = spans[i].x;
        sp->coo.y = y;
        sp->len = spans[i].len;
        sp->graysub.val = spans[i].coverage;
        sptr->cnt++;
    }

}
void test_outline_draw_by_gryph(iftFtgen_H fg, int32_t glyph_index)
{
    ///////////////////////////////
    /**
     * set boder outline
     */
    if (FT_Load_Glyph(fg->coreCtx.face, glyph_index, FT_LOAD_NO_BITMAP) == 0)
    {
        FT_Face face = fg->coreCtx.face;
        // Need an outline for this to work.
        if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
        {


#if 1 //outline
            {
                char buff[1024 * 16];
                SFontSpansVec *pSpans = (SFontSpansVec *)buff;
                memset(pSpans, 0, 1024 * 16);
                //  outlineSpans.hight =
                FT_Raster_Params params;
                memset(&params, 0, sizeof(params));
                params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
                params.gray_spans = RasterCallback;
                params.user = pSpans;
                int ret = -1;
                if ((ret = FT_Outline_Render(fg->coreCtx.lib, &face->glyph->outline, &params)) != 0)
                {
                    dprintf(DBG_LEV_ERR, "error in FT_Outline_Render ret =%d", ret);
                    return -1;
                }
                FT_BBox  bbox;
                FT_Outline_Get_BBox(&face->glyph->outline, &bbox);

                pSpans->hight = IFT_F26DOT6_TO_PIX_FLOOR(face->glyph->metrics.vertAdvance);
                pSpans->width = IFT_F26DOT6_TO_PIX_FLOOR(face->glyph->metrics.horiAdvance);
                dprintf(DBG_LEV_ERR, "------------begin img------------");
                show_fontimgspans(pSpans);
            }
            // Next we need the spans for the outline.
            char buff[1024 * 16];
            SFontSpansVec *poutlineSpans = (SFontSpansVec *)buff;
            memset(poutlineSpans, 0, 1024 * 16);
            // Set up a stroker.
            FT_Stroker stroker;
            FT_Stroker_New(fg->coreCtx.lib, &stroker);
            FT_Stroker_Set(stroker,
                           (int)(2 * 64),
                           FT_STROKER_LINECAP_ROUND,
                           FT_STROKER_LINEJOIN_ROUND,
                           0);
            FT_Glyph glyph;
            if (FT_Get_Glyph(face->glyph, &glyph) == 0)
            {
                int retcd = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
                //  outlineSpans.hight =
                FT_Raster_Params params;
                memset(&params, 0, sizeof(params));
                params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
                params.gray_spans = RasterCallback;
                params.user = poutlineSpans;
                int ret = -1;
                if ((ret = FT_Outline_Render(fg->coreCtx.lib, &((FT_OutlineGlyph)glyph)->outline, &params)) != 0)
                {
                    dprintf(DBG_LEV_ERR, "error in FT_Outline_Render ret =%d", ret);
                    return -1;
                }
                FT_BBox  bbox;
                FT_Outline_Get_BBox(&((FT_OutlineGlyph)glyph)->outline, &bbox);
                poutlineSpans->hight = MOD_CEILING_DIV(bbox.yMax - bbox.yMin, 64) ; // IFT_F26DOT6_TO_PIX_FLOOR(face->glyph->metrics.vertAdvance);
                poutlineSpans->width = IFT_F26DOT6_TO_PIX_FLOOR(face->glyph->metrics.horiAdvance);
                show_fontimgspans(poutlineSpans);
                // Clean up afterwards.
                FT_Stroker_Done(stroker);
                FT_Done_Glyph(glyph);
            }
#else
            // Next we need the spans for the outline.
            char buff[1024 * 16];
            SFontSpansVec *poutlineSpans = (SFontSpansVec *)buff;
            memset(poutlineSpans, 0, 1024 * 16);
            FT_Raster_Params params;
            memset(&params, 0, sizeof(params));
            params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
            params.gray_spans = RasterCallback;
            params.user = poutlineSpans;
            int ret = -1;
            if ((ret = FT_Outline_Render(fg->coreCtx.lib, &face->glyph->outline, &params)) != 0)
            {
                dprintf(DBG_LEV_ERR, "error in FT_Outline_Render ret =%d", ret);
                return ;
            }
            FT_BBox  bbox;
            FT_Outline_Get_BBox(&(face->glyph->outline), &bbox);
            poutlineSpans->hight = IFT_F26DOT6_TO_PIX_FLOOR(face->glyph->metrics.vertAdvance); //MOD_CEILING_DIV(bbox.yMax-bbox.yMin,128) ;//
            poutlineSpans->width = IFT_F26DOT6_TO_PIX_FLOOR(face->glyph->metrics.horiAdvance); // MOD_CEILING_DIV(bbox.xMax-bbox.xMin,128) ;
            show_fontimgspans(poutlineSpans);
#endif
        }
    }
    ////////////////////////////
}
#endif //DBG_TEST_GraySpan
typedef struct SRederMetaCtx
{
    E_GRAY256SUB_TYPE type ;
    iftFontGlyphUnit *fmm;
    SCoordinate2D correctionVal; //X/Y修正值
} SRederMetaCtx;
#include "debug.h"
/**
 * calc bbox_pix param
 */
void OutlineCalcBbox_pix(const int y, const int count, const FT_Span * const spans, void * const user)
{
    assert(user);
    SRederMetaCtx *ctx = (SRederMetaCtx *)user;
    if (ctx->fmm->bbox_pix.xMin > spans[0].x)
    {
        ctx->fmm->bbox_pix.xMin = spans[0].x ;
        ctx->fmm->bmpbuf_oo.x = spans[0].x ; //- ctx->correctionVal.x ;
    }
    if (ctx->fmm->bbox_pix.yMin > y)
    {
        ctx->fmm->bbox_pix.yMin = y ;
        ctx->fmm->bmpbuf_oo.y = y ; //- ctx->correctionVal.y ;
    }
    ctx->fmm->bbox_pix.xMax = MAX(ctx->fmm->bbox_pix.xMax, spans[count - 1].x + spans[count - 1].len);
    ctx->fmm->bbox_pix.yMax = MAX(ctx->fmm->bbox_pix.yMax, y + 1);
    /**
     * must <= bmp.pitch
     */
    if (ctx->fmm->bbox_pix.xMax > ctx->fmm->bmp.pitch)
    {
        dprintf(DBG_LEV_WAR, "bbx_pix xmax param too large!!";)
    }
    ctx->fmm->bbox_pix.xMax = MIN(ctx->fmm->bbox_pix.xMax, ctx->fmm->bmp.pitch);
    return;

}
void OutlineRasterCallback(const int y, const int count, const FT_Span * const spans, void * const user)
{
    assert(user);
    SRederMetaCtx *ctx = (SRederMetaCtx *)user;
    SGray256Sub g256;
    g256.sub.type = ctx->type ;
    int i = 0;
    int pos  = 0 ;

    int y_correction = y - ctx->fmm->bmpbuf_oo.y;
    if (y_correction < 0)
    {
        dprintf(DBG_LEV_INF, "error ycorection=%d", y_correction);
        return ;
    }
    for (; i < count; i++)
    {
        //  dprintf(DBG_LEV_INF,"spans[i].x=%d,len=%d,y=%d ",spans[i].x,spans[i].len,y);
        /**
         * flash bboxpix
         */
        int x_correction = spans[i].x - ctx->fmm->bmpbuf_oo.x; // ctx->fmm->bbox_pix.xMin + ctx->correctionVal.x;
        if (ctx->fmm->bmp.rows < y_correction + 1)
        {
            ctx->fmm->bmp.rows = y_correction + 1 ;
            // dprintf(DBG_LEV_INF,"fmm->bmp.rows=%d,y=%d,spans[i].x=%d",ctx->fmm->bmp.rows,y,spans[i].x);
        }
        if (x_correction > ctx->fmm->bbox_pix.xMax - ctx->fmm->bbox_pix.xMin + ctx->correctionVal.x)
        {
            dprintf(DBG_LEV_INF,
                    "error pitch xMax=%d ::spans[i] .y=%d .x=%d .len=%d, crection x|y =(%d|%d) ,pitch=%d",
                    ctx->fmm->bbox_pix.xMax,
                    y, spans[i].x, spans[i].len, ctx->correctionVal.x, ctx->correctionVal.y, ctx->fmm->bmp.pitch);
            continue;
        }
        g256.sub.level =  spans[i].coverage / GRAY256_LEVEL_CNT;
        pos = (y_correction) * ctx->fmm->bmp.pitch + x_correction;
        memset(ctx->fmm->bmp.buffer + pos, g256.val, spans[i].len);
    }
}
void dumpbboxMetric(FT_BBox *bbox, const char * ptr)
{

    dprintf(DBG_LEV_INF, "[%s],height=%d,width=%d,xMin=%d,xmax=%d,ymin=%d, ymax=%d"
            , (ptr == NULL) ? "bbox" : ptr
            , IFT_F26DOT6_TO_PIX_CELING(bbox->yMax - bbox->yMin)
            , IFT_F26DOT6_TO_PIX_CELING(bbox->xMax - bbox->xMin)
            , IFT_F26DOT6_TO_PIX_FLOOR(bbox->xMin)
            , IFT_F26DOT6_TO_PIX_CELING(bbox->xMax)
            , IFT_F26DOT6_TO_PIX_FLOOR(bbox->yMin)
            , IFT_F26DOT6_TO_PIX_CELING(bbox->yMax)
           );
}
static int ftCalcGlapyslotMetric(FT_Glyph_Metrics *glyphmetrics, SPixBbox *bbox, FT_GlyphSlot slot)
{

    glyphmetrics->height = IFT_PIX_TO_F26DOT6(bbox->yMax - bbox->yMin);// p_yMax- p_yMin+1;//IFT_F26DOT6_CELING(bbox->yMax-bbox->yMin);
    glyphmetrics->width = IFT_PIX_TO_F26DOT6(bbox->xMax - bbox->xMin); //p_xMax- p_xMin+1;//IFT_F26DOT6_CELING(bbox->xMax-bbox->xMin);
    glyphmetrics->horiBearingX = IFT_PIX_TO_F26DOT6(bbox->xMin);
    glyphmetrics->horiBearingY = IFT_PIX_TO_F26DOT6(bbox->yMax);


    glyphmetrics->horiAdvance =  MAX(glyphmetrics->width, slot->advance.x);
    /**
     *mod celling to 2*(1<<6)
     */
    glyphmetrics->horiAdvance = MOD_CEILING(glyphmetrics->horiAdvance, IFT_PIX_TO_F26DOT6(2));

    return 0;
}
static int  reflashGlyphUnit(iftFtgen_H fg, FT_Outline outline, iftFontGlyphUnit *fmm, SCoordinate2D correctionVal)
{
    if (fmm->bmpbuf_oo.x < 0)
    {
        fmm->bbox_pix.xMin -= fmm->bmpbuf_oo.x;
        fmm->bbox_pix.xMax -= fmm->bmpbuf_oo.x;
        fmm->bmpbuf_oo.x -= fmm->bmpbuf_oo.x;
    }
    if (fmm->bmpbuf_oo.y < 0)
    {
        fmm->bbox_pix.yMin -= fmm->bmpbuf_oo.y;
        fmm->bbox_pix.yMax -= fmm->bmpbuf_oo.y;
        fmm->bmpbuf_oo.y -= fmm->bmpbuf_oo.y;
    }
    /**
     * pix aligine
     */
    //    if( (fmm->bbox_pix.yMin%2!=0))
    //    {
    //        fmm->bbox_pix.yMax +=1;
    //        fmm->bbox_pix.yMin +=1; ///MOD_FLOOR(fmm->bbox_pix.yMax,2) ;
    //    }
    //    if( (fmm->bbox_pix.xMin%2!=0))
    //     {
    //         fmm->bbox_pix.xMax -=1;
    //         fmm->bbox_pix.xMin -=1 ;//MOD_FLOOR(fmm->bbox_pix.xMax,2) ;
    //     }

    /**
     * change
     */
    if (ftCalcGlapyslotMetric(&fmm->glyphmetrics, &fmm->bbox_pix, fg->coreCtx.face->glyph) < 0)
    {
        MYTRACE();
        return -1;
    }

    return 0 ;
}

int reflashBmpBuff(iftFontGlyphUnit *fmm)
{
    //    int i = 0 ;
    // uint8_t tmpBuf[1024];
    assert(fmm->bmpMaxRows >= fmm->bmp.rows + 1);
    uint8_t * tmpline = fmm->bmp.buffer + fmm->bmp.rows * fmm->bmp.pitch;

    //int pix_fontHoriBearingX =IFT_F26DOT6_TO_PIX_FLOOR(fmm->glyphmetrics.horiBearingX);
    //int pix_fontHoriBearingY =IFT_F26DOT6_TO_PIX_FLOOR(fmm->glyphmetrics.horiBearingY);

    int pix_fontWidth = IFT_F26DOT6_TO_PIX_FLOOR(fmm->glyphmetrics.width);
    int new_pitch = IFT_F26DOT6_TO_PIX_FLOOR(fmm->glyphmetrics.width);
    int pix_fontHeight = IFT_F26DOT6_TO_PIX_FLOOR(fmm->glyphmetrics.height);
    //int pix_YMax = pix_fontHoriBearingY ;

    int part_process = (pix_fontHeight + 1) / 2;
    if (pix_fontHeight == 1)
    {
        part_process = 1 ;
    }
    /**
     * process down part
     */
    int wtpos = 0;
    int irow = 0;
    for (; irow < part_process; irow++)
    {

        int cur_pos = (((pix_fontHeight - 1 - irow) * fmm->bmp.pitch));
        int copy_pos = ((irow * fmm->bmp.pitch));
        memcpy(tmpline, fmm->bmp.buffer + copy_pos, new_pitch);
        memcpy(fmm->bmp.buffer + wtpos, fmm->bmp.buffer + cur_pos, new_pitch);
        memcpy(fmm->bmp.buffer + cur_pos, tmpline, new_pitch);
        wtpos += new_pitch;
    }
#if 1
    /**
     * process up part
     */
    int up_process_line_start = irow ;
    irow = 0 ;
    int part_process_steps = pix_fontHeight - part_process ;
    for (; irow < part_process_steps; irow++)
    {
        int cur_pos = (((up_process_line_start + irow) * fmm->bmp.pitch));
        memcpy(fmm->bmp.buffer + wtpos, fmm->bmp.buffer + cur_pos, new_pitch);
        wtpos += new_pitch;
    }
#endif
    fmm->bmp.pitch = new_pitch ;
    fmm->bmp.rows = pix_fontHeight ;
    fmm->bmp.width = pix_fontWidth ;
    return 0;
    // for(;i<)
}
//#define DEF_OUTLING_PIXS 2
static inline int changeBmpPitchOninit(iftFontGlyphUnit *fmm, int pitch)
{
    if (pitch <= 0)
    {
        MYTRACE();
        return -1;
    }
    if (pitch * pitch >= fmm->bmpbuffMaxsize)
    {
        MYTRACE();
        return -1;
    }
    fmm->bmp.pitch = fmm->bmp.width = fmm->bmp.rows = pitch;
    fmm->bmpMaxRows = fmm->bmpbuffMaxsize / pitch;
    memset(fmm->bmpbuf, 0, fmm->bmpbuffMaxsize);
    return 0;
}
int ftRenderOutline(FT_Library         library,
                    FT_Outline*        outline,
                    FT_Raster_Params  *params)
{
    assert(library && outline && params);
    FT_SpanFunc bak_gray_span = params->gray_spans;
    /**
     * calc bbox_pix
     */
    params->gray_spans =  OutlineCalcBbox_pix ;
    if (FT_Outline_Render(library, outline, params) != 0)
    {
        MYTRACE();
        return -1;
    }
    /**
     * update render
     */
    /**
     * render
     */
    params->gray_spans =  bak_gray_span ;
    if (FT_Outline_Render(library, outline, params) != 0)
    {
        MYTRACE();
        return -1;
    }
    return 0 ;
}
/**
 * gen character
 */
static int ftCoreRenderMeta(iftFtgen_H fg, iftChar32 ucs4, iftFontGlyphUnit *fmm)
{

    assert(fmm);
    // Load the glyph we are looking for.
    int32_t glyph_index = FT_Get_Char_Index(fg->coreCtx.face, ucs4);
#if DBG_TEST_GraySpan
    test_outline_draw_by_gryph(fg, glyph_index);
#endif //DBG_TEST_GraySpan

    /**
     * set bmp param
     */
#if 1
    if (FT_Load_Glyph(fg->coreCtx.face, glyph_index, FT_LOAD_NO_BITMAP) != 0)
    {
        //load glyph error
        dprintf(DBG_LEV_INF, "error load glyph");
        return -1;
    }
    fmm->fontctrl = &fg->servCtx.pstl->fontconf;
    if (changeBmpPitchOninit(fmm, fg->coreCtx.face->size->metrics.x_ppem) < 0)
    {
        dprintf(DBG_LEV_INF, "error load glyph");
        return -1;
    }
    fmm->bmp.num_grays = 256;
    fmm->bmp.pixel_mode = FT_PIXEL_MODE_GRAY ;
    //fmm->bmp.rows =0;//fg->coreCtx.face->size->metrics.y_ppem;

    fmm->bbox_pix.xMax = DEF_PIX_MIN ;
    fmm->bbox_pix.yMax = DEF_PIX_MIN ;
    fmm->bbox_pix.xMin = DEF_PIX_MAX ;
    fmm->bbox_pix.yMin = DEF_PIX_MAX ;
    if (fg->coreCtx.face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
    {
        dprintf(DBG_LEV_INF, "error load glyph format");
        return -1;
    }
    SRederMetaCtx Renderctx =
    { .fmm = fmm, };

    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = OutlineRasterCallback;
    params.user = &Renderctx;
    if (fmm->fontctrl->super.extra.bOutline)
    {
        FT_Glyph glyph_outline;
        if (FT_Get_Glyph(fg->coreCtx.face->glyph, &glyph_outline) != 0)
        {
            //load glyph error
            dprintf(DBG_LEV_INF, "error load glyph");
            return -1;
        }
        Renderctx.type = E_GRAY256SUB_TYPE_OUTLINE;
        Renderctx.correctionVal.x = fmm->fontctrl->super.extra.outLinePix;
        Renderctx.correctionVal.y = fmm->fontctrl->super.extra.outLinePix; // DEF_OUTLING_PIXS;
        int pitch_new = fmm->bmp.pitch + 2 * fmm->fontctrl->super.extra.outLinePix ;
        if (changeBmpPitchOninit(fmm, pitch_new) < 0)
        {
            dprintf(DBG_LEV_INF, "error load glyph");
            return -1;
        }
        // Set up a stroker.
        FT_Stroker stroker;
        FT_Stroker_New(fg->coreCtx.lib, &stroker);
        FT_Stroker_Set(stroker, (int)(fmm->fontctrl->super.extra.outLinePix * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
        FT_Glyph_StrokeBorder(&glyph_outline, stroker, 0, 1);
        if (ftRenderOutline(fg->coreCtx.lib, &((FT_OutlineGlyph) glyph_outline)->outline, &params) < 0)
        {
            MYTRACE();
            FT_Done_Glyph(glyph_outline);
            return -1 ;
        }
        FT_Stroker_Done(stroker);
        FT_Done_Glyph(glyph_outline);
        // dumpBitMap(&fmm->bmp, 0);
    }
    Renderctx.type = E_GRAY256SUB_TYPE_NORMAL;
    if (ftRenderOutline(fg->coreCtx.lib, &fg->coreCtx.slot->outline, &params) < 0)
    {
        MYTRACE();
        return -1 ;
    }
    if (reflashGlyphUnit(fg, fg->coreCtx.slot->outline, fmm, Renderctx.correctionVal) < 0)
    {
        MYTRACE();
        return -1 ;
    }
    //    dprintf(DBG_LEV_INF,"bbox(xmin,ymin,xmax,ymax)=(%d,%d,%d,%d)",
    //            fmm->bbox_pix.xMin,
    //            fmm->bbox_pix.yMin,
    //            fmm->bbox_pix.xMax,
    //            fmm->bbox_pix.yMax);
    // dumpBitMap(&fmm->bmp, 0);
    if (reflashBmpBuff(fmm))
    {
        MYTRACE();
        return -1 ;
    }
    //  dumpBitMap(&fmm->bmp,0);
    return 0 ;
#endif
#if 0
    FT_Error error ;
    dprintf(DBG_LEV_DBG, "get unicode index=%d", glyph_index);
    /* load glyph image into the slot (erase previous one) */
    error = FT_Load_Char(fg->coreCtx.face, ucs4, FT_LOAD_RENDER);
    if (error)
    {
        dprintf(DBG_LEV_INF, "error load char");
        return -1;
    }
    ////
    /**
     *set mem
     */
    iftFontCtrlInt *ftcfg = &fg->servCtx.pstl->fontconf;
    FT_Bitmap *bmp = &(fg->coreCtx.slot->bitmap);
    if (bmp->width > ftcfg->maxAdvanceWidth || bmp->rows > (2 * ftcfg->height))
    {
        //bmp overflow
        dprintf(DBG_LEV_INF,
                "bmp overflow,w/h [%d/%d]-[%d/%d]",
                bmp->width, bmp->rows, ftcfg->maxAdvanceWidth, ftcfg->height);
        return -1;
    }

    unsigned char * org_bmpBbuff = fmm->bmp.buffer;
    fmm->bmp = *bmp;
    fmm->bmp.buffer = org_bmpBbuff;
    fmm->glyphmetrics = fg->coreCtx.slot->metrics;
    int m =   FT_Outline_Get_Bitmap(fg->coreCtx.lib, &fg->coreCtx.slot->outline, &fmm->bmp);
    //FT_Glyph_To_Bitmap();

    /**
     * clone bmp buff
     */
    int32_t bufflen = fmm->bmp.rows * fmm->bmp.width;
    if (bufflen > DEF_UCS4META_BMPBUF_LEN)
    {
        dprintf(DBG_LEV_INF, "no space for alloc bmpbuff!");
        return -1;
    }
    memcpy(fmm->bmp.buffer, bmp->buffer, bufflen);
    FT_BBox target_bbox ;
    if (FT_Outline_Get_BBox(&fg->coreCtx.slot->outline, &target_bbox) != 0)
    {
        MYTRACE();
        return -1 ;
    }    //updata metric
    dumpbboxMetric(&target_bbox, "tmp");
    dumpBitMap(&fmm->bmp, 0);
    /**
     * set character pos
     */
    //todo pos
    return 0;
#endif // use              FT_Raster_Params params;

}
//ucs4hash fonc
int ucs4hash_cleanparam(iftUcs4HashTable *table)
{
    memset(table->map, 0, UCS4HASH_METRIC * sizeof(iftUcs4HashMeta *));
    mpool_clean(table->ucs4pool);
    table->metacnt = 0;
    //creat hashpool;
    return 0;
}
int ucs4hash_init(iftUcs4HashTable *table, iftRange *rgnode)
{
    int totalsize = DEF_MEMMETA_CNT * DEF_MEMMETA_SIZE;
    if (totalsize > rgnode->len)
    {
        MYTRACE();
        return -1;
    }
    table->ucs4pool = mpool_create(rgnode->data, DEF_MEMMETA_SIZE, rgnode->len);
    if (!table->ucs4pool)
    {
        return -1;
    }
    ucs4hash_cleanparam(table);
    return 0;
}

int ucs4hash_deinit(iftUcs4HashTable *table)
{
    assert(table);
    if (table->ucs4pool)
    {
        mpool_destroy(table->ucs4pool);
    }
    return 0;
}
iftUcs4HashMeta* ucs4hash_find(iftUcs4HashTable *table, iftChar32 ucs4, iftUcs4HashMeta **preout)
{
#if 0
    iftUcs4HashMeta *retmeta = NULL;
    iftUcs4HashMeta *cur = table->map[UCS4HASH_VAL(ucs4)];
    while (cur && UCS4HASHMETA_VALID(cur))
    {
        if (cur->ucs4 == ucs4)
        {
            retmeta = cur;
            break;
        }
        cur = cur->next;
    }
#endif
    iftUcs4HashMeta *find = NULL;
    iftUcs4HashMeta *cur = table->map[UCS4HASH_VAL(ucs4)];
    iftUcs4HashMeta *pre = cur;
    if (preout && *preout)
    {
        *preout = NULL;
    }
    do
    {
        if (!cur)
        {
            break;
        }
        if ((cur->ucs4 == ucs4) && UCS4HASHMETA_VALID(cur))
        {
            find = cur;
            break;
        }
        cur = cur->next;
        while (cur && UCS4HASHMETA_VALID(cur))
        {
            if (cur->ucs4 == ucs4)
            {
                find = cur;
                if (preout && *preout)
                {
                    *preout = pre;
                }
                break;
            }
            pre = cur;
            cur = pre->next;
        }
    }
    while (0);
    return find;
}

iftUcs4HashMeta* ucs4hash_findNoUse(iftUcs4HashTable *table)
{
    int i = 0;
    for (; i < table->ucs4pool->itemcnt; i++)
    {
        iftUcs4HashMeta* mt = (iftUcs4HashMeta*)(table->ucs4pool->items[i]);
        if (mt && mt->meta.refcnt == 0)
        {
            return mt;
        }
    }
    return NULL ;
}
int ucs4hash_del(iftUcs4HashTable *table, iftChar32 ucs4)
{
    iftUcs4HashMeta *find = NULL;
    iftUcs4HashMeta *pre = NULL;
    if ((find = ucs4hash_find(table, ucs4, &pre)) != NULL)
    {
        if (pre)
        {
            pre->next = find->next;
        }
        else
        {
            table->map[UCS4HASH_VAL(ucs4)] = NULL;
        }
        table->metacnt--;
        find = NULL;
        return 0;
    }
    return -1;
}
inline static iftUcs4HashMeta * ucs4hashMeta_attach(iftUcs4HashTable *table)
{
    int bAtta = 0;
    iftUcs4HashMeta * pmeta = NULL;
    if (table->metacnt < DEF_MEMMETA_CNT)
    {
        pmeta = (iftUcs4HashMeta *) mpool_attach(table->ucs4pool);
        if (pmeta)
        {
            //
            bAtta = 1;
        }
    }
    if (!bAtta)
    {
        pmeta = ucs4hash_findNoUse(table);
        if (pmeta)
        {
            ucs4hash_del(table, pmeta->ucs4);
            ucs4hashMeta_detach(table, pmeta);
            if ((pmeta = (iftUcs4HashMeta *) mpool_attach(table->ucs4pool)) != NULL)
            {
                bAtta = 1;
            }
            else
            {
                MYTRACE();
            }
        }
    }
    if (!bAtta)
    {
        MYTRACE();
        return NULL ;
    }
    memset(pmeta, 0, DEF_MEMMETA_SIZE);
    pmeta->magic = DEF_UCS4META_MAGIC;
    pmeta->meta.bmp.buffer = pmeta->meta.bmpbuf;
    pmeta->meta.bmpbuffMaxsize = DEF_MEMMETA_SIZE - sizeof(iftUcs4HashMeta);
    return pmeta;
}
inline static int ucs4hashMeta_detach(iftUcs4HashTable *table, iftUcs4HashMeta *p)
{
    if (!p || p->magic != DEF_UCS4META_MAGIC)
    {
        MYTRACE();
        return -1;
    }
    if (mpool_detach(table->ucs4pool, p) < 0)
    {
        MYTRACE();
        return -1;
    }
    return 0;
}
int ucs4hash_add(iftUcs4HashTable *table, iftUcs4HashMeta* meta)
{
    assert(table && meta && (meta->next == NULL));
    if (ucs4hash_find(table, meta->ucs4, NULL) != NULL)
    {
        MYTRACE();
        return -1;
    }
    iftUcs4HashMeta **pcur = &table->map[UCS4HASH_VAL(meta->ucs4)];
    if (*pcur)
    {
        meta->next = (*pcur)->next;
    }
    (*pcur) = meta;
    table->metacnt++;
    return 0;
}

//===================================================
/**
 * init syswide conf
 */
int32_t iftFtgen_setEnv(IftEnv *env)
{

    debugSetLevel(env->dbgLevel);
    *iftosdEnv_get() = *env;

    return 0;
}
void iftFtgen_runOnce()
{
    static int brun = 0;
    if (!brun)
    {
        memset(gs_FtgenTable, 0, MAX_FTGEN_LEN * sizeof(iftFtgen));
        /**
         * font info set
         */
        iftFontInfo_init();
#if ITE_BIG_ENDIAN
        MYTRACE("BIG_ENDIAN");
#else
        MYTRACE("LITLE_ENDIAN");
#endif
        brun = 1;
    }
}
static inline int ftgenIDAlloc()
{
    static int id = 10;
    return id++;
}

static inline int iftRenderFontTable_create(iftRenderFontTable *rft, iftRange *rgnode, int bAutoLen)
{
    memset(rft, 0, sizeof(iftRenderFontTable));
    int lmu_data_t = DEF_MAX_LINEMEMUNIT_CNT * sizeof(iftLineMemUnit);
    int ftu_data_t = DEF_MAX_FONTUNIT_SIZE * sizeof(iftFontUnit);
    if (rgnode->len < lmu_data_t + ftu_data_t)
    {
        /**
         * min font table len
         */
        MYTRACE();
        return -1;
    }
    rft->lmuPool = mpool_create(rgnode->data, sizeof(iftLineMemUnit), lmu_data_t);
    if (bAutoLen == 1)
    {
        rft->fontUnitPool = mpool_create(rgnode->data + lmu_data_t, sizeof(iftFontUnit), ftu_data_t);
        rgnode->len = lmu_data_t + ftu_data_t;
    }
    else
    {
        rft->fontUnitPool = mpool_create(rgnode->data + lmu_data_t,
                                         sizeof(iftFontUnit),
                                         rgnode->len - lmu_data_t);
    }
    if (!rft->lmuPool || !rft->fontUnitPool)
    {
        MYTRACE();
        if (rft->lmuPool) { mpool_destroy(rft->lmuPool); }
        if (rft->fontUnitPool) { mpool_destroy(rft->fontUnitPool); }
        rft->lmuPool = NULL;
        rft->fontUnitPool = NULL;
        return -1;
    }
    return 0;
}
static inline int iftRenderFontTable_cleanAllLines(iftRenderFontTable *rft)
{
    assert(rft);
    if (rft->lmuPool)
    {
        int i = 0;
        for (i = 0; i < rft->maxLines; i++)
        {
            int j = 0;
            for (j = 0; j < rft->lmu[i].fontcnt; j++)
            {
                iftFontUnit* cur = iftFontUnitHdr_popfront(&rft->lmu[i].lineHdr);

                ((iftFontGlyphUnit *)(cur->glyph))->refcnt--;
            }
            rft->lmu[i].fontcnt = 0;
            rft->lmu[i].widePix = 0;
            rft->lmu[i].lineHdr.head = NULL;
            rft->lmu[i].lineHdr.tail = NULL;
        }
    }
    if (rft->fontUnitPool)
    {
        mpool_clean(rft->fontUnitPool);
    }
    return 0;
}
static inline int iftRenderFontTable_freeAllLines(iftRenderFontTable *rft)
{
    assert(rft);
    iftRenderFontTable_cleanAllLines(rft);
    if (rft->fontUnitPool)
    {
        mpool_clean(rft->fontUnitPool);
    }
    if (rft->lmuPool)
    {
        mpool_clean(rft->lmuPool);
    }
    rft->lmu = NULL;
    rft->maxLines = 0;
    return 0;
}
static inline int iftRenderFontTable_destory(iftRenderFontTable *rft)
{
    assert(rft);
    /**
     * free LineMemUnits
     */
    iftRenderFontTable_freeAllLines(rft);
    if (rft->lmuPool)
    {
        mpool_destroy(rft->lmuPool);
    }
    if (rft->fontUnitPool)
    {
        mpool_destroy(rft->lmuPool);
    }

    return 0;
}

inline int32_t iftFtgen_renderFontTableSetopt(iftFtgen_H hfg, iftRect *srect, iftStyle *psty) //default use RBGA
{
    if (!srect || !psty)
    {
        MYTRACE();
        return -1;
    }
    /**
     * count lines / linecapacity
     */
    int calcMaxlines = (srect->hight) / (psty->fontLineHight);

    iftRenderFontTable *ptable = &hfg->servCtx.renderTable;
    if (ptable->lmu && (calcMaxlines <= ptable->maxLines))
    {
        //MYTRACE();
        return 0;
    }
    iftRenderFontTable_freeAllLines(ptable);
    /**
     * alloc renderTable
     */
    if (ptable->maxLines >= DEF_MAX_LINEMEMUNIT_CNT)
    {
        dprintf(DBG_LEV_ERR, "not enough lines !");
        return -1;
    }
    ptable->maxLines = calcMaxlines;
    ptable->lmu = (iftLineMemUnit*) mpool_attach_n(hfg->servCtx.renderTable.lmuPool, ptable->maxLines);
    return 0;
}

int32_t iftFtgen_init()
{
    return 0;
}

/**
 * get font osd gen handle
 */
iftFtgen_H iftFtgen_hand(iftRange *rgnod)
{
    iftFtgen_H ftgen_h = NULL;
    iftBool bretflag = 0;
    int32_t i = 0;
    for (; i < MAX_FTGEN_LEN; i++)
    {
        if (gs_FtgenTable[i].ftgenid == 0)
        {
            gs_FtgenTable[i].ftgenid = ftgenIDAlloc();
            ftgen_h = &gs_FtgenTable[i];
            iftRange rgRendFt = *rgnod;
            if (iftRenderFontTable_create(&ftgen_h->servCtx.renderTable, &rgRendFt, 1) < 0)
            {
                MYTRACE();
                return NULL ;
            }
            iftRange rgUcs4 =
            {
                .data = rgnod->data + rgRendFt.len,
                .len = rgnod->len - rgRendFt.len
            };
            if (ucs4hash_init(&ftgen_h->ucs4hash, &rgUcs4) < 0)
            {
                iftRenderFontTable_destory(&ftgen_h->servCtx.renderTable);
                MYTRACE();
                return NULL ;
            }
            break;
        }
    }
    if (!ftgen_h)
    {
        MYTRACE();
        return NULL ;
    }
    iftStyle_init(&ftgen_h->stylsheet);
    do
    {
        /**
         * set default style
         */

        ftgen_h->servCtx.pstl = ftgen_h->stylsheet.front;
        FT_Error error;
        iftFontInfo *info = iftFontInfo_hdr();
        error = FT_Init_FreeType(&ftgen_h->coreCtx.lib);
        if (error)
        {
            dprintf(DBG_LEV_ERR, "Init freetype lib error!");
            return NULL ;
        }

        /**
         * reg fontinfo by style
         */
        ftgen_h->fontInfo = iftFontInfo_getByPath(iftGlobDefaultStyle()->ftsrc.path);
        if (!ftgen_h->fontInfo)
        {
            /**
             * set default info file.
             */
            ftgen_h->fontInfo = info;
        }
        ftgen_h->servCtx.rect = gs_defaultCavasRect;
        error = iftFtgen_renderFontTableSetopt(ftgen_h, &ftgen_h->servCtx.rect, ftgen_h->stylsheet.front);
        if (error < 0)
        {
            dprintf(DBG_LEV_ERR, "Init iftFtgen_renderFontTableSetopt error!");
            return NULL ;
        }
        /* create face object */
        error = FT_New_Face(ftgen_h->coreCtx.lib,
                            (char*) ftgen_h->fontInfo->file->fpname,
                            0,
                            &ftgen_h->coreCtx.face);
        if (error)
        {
            dprintf(DBG_LEV_ERR, "error!ret[%d]", error);
            return NULL ;
        }
        bretflag = 1;
    }
    while (0);
    if (!bretflag)
    {
        //clear source.
        iftFtgen_destory(ftgen_h);
        ftgen_h = NULL;
        return NULL ;
    }
    ftgen_h->stage = E_FTGEN_STAGE_NOMAL;
    return ftgen_h;

}
int32_t iftFtgen_destory(iftFtgen_H hfg)
{
    dprintf(DBG_LEV_INF, "iftFtgen_destory enter! sid=%d\n", hfg->ftgenid);
    if (!hfg)
    {
        dprintf(DBG_LEV_ERR, "NULL pointer use!")
        return -1;
    }
    int32_t i = 0;
    for (; i < MAX_FTGEN_LEN; i++)
    {
        if (hfg == &gs_FtgenTable[i])
        {
            //hfg->styleId = INVALD_IFTSTYLEID;
            iftRenderFontTable_destory(&hfg->servCtx.renderTable);
            if (hfg->stage == E_FTGEN_STAGE_SESSION)
            {
                iftFtgen_sessionExit(hfg);
            }
            iftStyle_deinit(&hfg->stylsheet);
            ucs4hash_deinit(&hfg->ucs4hash);
            FT_Done_Face(hfg->coreCtx.face);
            FT_Done_FreeType(hfg->coreCtx.lib);
            gs_FtgenTable[i].ftgenid = 0;
            break;

        }
        else if (hfg->ftgenid == gs_FtgenTable[i].ftgenid)
        {
            //iligal to this part
            dprintf(DBG_LEV_ERR, "hfg->ftgenid invalid idx=%d!", i);
        }
    }
    //
    dprintf(DBG_LEV_INF, "iftFtgen_destory exit,mem check!\n");
    iftMemReport();
    return 0;
}
int32_t iftFtgen_defStyle(iftFtgen_H hfg, iftOsdServStyleInt * servStyle)
{
    if (!servStyle)
    {
        MYTRACE();
        return -1;
    }
    servStyle->fctrl = iftStyle_default(&hfg->stylsheet)->fontconf.super;
    servStyle->parag = iftStyle_default(&hfg->stylsheet)->lineconf.super;
    return 0;
}
int32_t iftFtgen_curStyle(iftFtgen_H hfg, iftOsdServStyleInt * servStyle)
{
    if (!servStyle)
    {
        MYTRACE();
        return -1;
    }
    servStyle->fctrl = hfg->servCtx.pstl->fontconf.super;
    servStyle->parag = hfg->servCtx.pstl->lineconf.super;
    return 0;

}
inline const iftStyle* iftFtgen_orgiStyle(iftFtgen_H hfg)
{
    assert(hfg->servCtx.pstl);
    return hfg->servCtx.pstl;
}
int dumpSevsty(iftOsdServStyle *s)
{
    printf("servStryle::color=%d|%d|%d,linegap=%d,pt=%d\n",
           s->color.r,
           s->color.g,
           s->color.b,
           s->linegap,
           s->pt);
    return 0;
}
int dumpsty(iftStyle *s, char * str)
{
    assert(str);
    if (!s)
    {

        dprintf(DBG_LEV_INF, "style(%s)::NULL\n", str);

    }
    else
    {
        dprintf(DBG_LEV_INF,
                "style(%s)::id=%d,color=%d|%d|%d,linegap=%d,pt=%d\n", str, s->sid
                , s->fontconf.super.color.r, s->fontconf.super.color.g, s->fontconf.super.color.b, s->lineconf.super.linegap, s->fontconf.super.pt);
    }
    return 0;
}
void dumpstySheet(iftStyleSheet *sheet, int id)
{
    iftStyle *pcur = sheet->front;
    dprintf(DBG_LEV_INF, "----style start--id=%d--- num=%d\n", id, sheet->cnt);
    while (pcur)
    {
        dumpsty(pcur, "stysheet");
        pcur = pcur->next;
    }
    dprintf(DBG_LEV_INF, "----style end----- num=%d\n", id, sheet->cnt);
}
//
// /**
//  * return style id
//  */
int32_t iftFtgen_setStyle(iftFtgen_H hfg, iftOsdServStyleInt * servStyle)
{
    if (!servStyle)
    {
        dprintf(DBG_LEV_ERR, "iftFtgen_sessionSetStyle error!");
        return -1;
    }
    //    if ((hfg->servCtx.pstl) && (hfg->servCtx.pstl != iftStyle_default(&hfg->stylsheet)))
    //    {
    //        iftStyle_movSty(&hfg->stylsheet,hfg->servCtx.pstl);
    //        hfg->servCtx.pstl = NULL ;
    //    }
    iftStyle *pitem;
    if (NULL != (pitem = iftStyle_find(&hfg->stylsheet, servStyle)))
    {
        if (hfg->servCtx.pstl == pitem)
        {
            //MYTRACE();
            //clean iftUcs4HashTable
            return 0;
        }
        else
        {
            dprintf(DBG_LEV_INF,
                    "----before set style-----stid=%d--hand=%p",
                    hfg->ftgenid, hfg);
            dumpsty(hfg->servCtx.pstl, "pre");
            hfg->servCtx.pstl = pitem;
        }
    }
    else
    {
        //   dumpstySheet(&hfg->stylsheet,hfg->ftgenid);

        if (hfg->stylsheet.cnt > 3)
        {
            //pop font->next
            if (iftStyle_movSty(&hfg->stylsheet, hfg->stylsheet.front->next) < 0)
            {
                MYTRACE();
                return -1 ;
            }
        }
        dprintf(DBG_LEV_INF,
                "----before set style-------stid=%d---hand=%p-handnum=%d-------------------\n",
                hfg->ftgenid, hfg, hfg->stylsheet.cnt);
        dumpsty(NULL, "cur");
        hfg->servCtx.pstl = iftStyle_add(&hfg->stylsheet, servStyle);
        if (hfg->servCtx.pstl == NULL)
        {
            MYTRACE();
            return -1;
        }
    }
    dumpsty(hfg->servCtx.pstl, "cur");
    dprintf(DBG_LEV_INF, "----after set style-----num=%d-------------------------\n", hfg->stylsheet.cnt);

    ucs4hash_cleanparam(&hfg->ucs4hash);
    return 0;
}
/**
 * set rect of service //one canvas one  session
 */
int32_t iftFtgen_setRect(iftFtgen_H hfg, int32_t rcWidthPix, int32_t rcHightPix)
{
    if (rcWidthPix == hfg->servCtx.rect.wide && rcHightPix == hfg->servCtx.rect.hight)
    {
        return 0;
    }
    hfg->servCtx.rect.wide = rcWidthPix;
    hfg->servCtx.rect.hight =
        (rcHightPix < hfg->servCtx.pstl->fontLineHight) ? hfg->servCtx.pstl->fontLineHight : rcHightPix;
    int32_t error = iftFtgen_renderFontTableSetopt(hfg, &hfg->servCtx.rect, hfg->servCtx.pstl);
    if (error < 0)
    {
        dprintf(DBG_LEV_ERR, "Init iftFtgen_renderFontTableSetopt error!");
        return -1;
    }
    return 0;
}
/**
 * init  param
 */
int32_t iftFtgen_sessionEnter(iftFtgen_H hfg)
{
    FT_Error error;
    //clean table
    //todo need clean
    iftRenderFontTable_cleanAllLines(&hfg->servCtx.renderTable);
    /**
     *  style / rect are ok
     * reset font init  freetype
     */

    hfg->coreCtx.slot = hfg->coreCtx.face->glyph;
    /**
     * set charmap opt
     */
    error = FT_Select_Charmap(hfg->coreCtx.face, FT_ENCODING_UNICODE);
    if (error)
    {
        dprintf(DBG_LEV_ERR, "error! cod = %d", error);
        FT_Done_Face(hfg->coreCtx.face);
        return -1;
    }
    error = FT_Set_Char_Size(hfg->coreCtx.face,
                             IFT_PIX_TO_F26DOT6(hfg->servCtx.pstl->fontconf.maxAdvanceWidth),
                             IFT_PIX_TO_F26DOT6(hfg->servCtx.pstl->fontconf.height),
                             hfg->servCtx.pstl->horz_resolution,
                             hfg->servCtx.pstl->vert_resolution);
    //    error = FT_Set_Pixel_Sizes(hfg->coreCtx.face,
    //            hfg->servCtx.pstl->fontconf.maxAdvanceWidth,
    //            hfg->servCtx.pstl->fontconf.height);
    if (error)
    {
        dprintf(DBG_LEV_ERR, "error!");
        FT_Done_Face(hfg->coreCtx.face);
        return -1;
    }
    hfg->stage = E_FTGEN_STAGE_SESSION;
    return 0;
}

/**
 *
 */
int32_t iftFtgen_sessionExit(iftFtgen_H hfg)
{

    //  hfg->servCtx.pstl =iftStyle_default(&hfg->stylsheet);
    return 0;
}

// 转换UCS2编码到UCS4编码
int32_t UTF16_To_UCS4(const int16_t* pwUTF16, int32_t* dwUCS4)
{
    int16_t w1, w2;

    if (pwUTF16 == NULL)
    {
        // 参数错误
        return 0;
    }

    w1 = pwUTF16[0];
    if (w1 >= 0xD800 && w1 <= 0xDFFF)
    {
        // 编码在替代区域（Surrogate Area）
        if (w1 < 0xDC00)
        {
            w2 = pwUTF16[1];
            if (w2 >= 0xDC00 && w2 <= 0xDFFF)
            {
                *dwUCS4 = (w2 & 0x03FF) + (((w1 & 0x03FF) + 0x40) << 10);
                return 2;
            }
        }

        return 0; // 非法UTF16编码
    }
    else
    {
        *dwUCS4 = w1;
        return 1;
    }
}
typedef enum E_PROC_CHAR_RESULT
{
    E_PROC_CHAR_RESULT_ERROR,
    E_PROC_CHAR_RESULT_BREAK,
    E_PROC_CHAR_RESULT_CONTINUE,
} E_PROC_CHAR_RESULT;

E_PROC_CHAR_RESULT iftFtgen_addOneChar2Line(iftFtgen_H hfg, iftChar32 ucs4, int line,
        int32_t curGlyphAdvanceY)
{
    E_PROC_CHAR_RESULT result = E_PROC_CHAR_RESULT_ERROR;
    iftBool bret = -1;
    //     iftBool bexit = -1;

    //process one character
    iftFontUnit *fmb = (iftFontUnit*) mpool_attach(hfg->servCtx.renderTable.fontUnitPool);
    if (!fmb)
    {
        dprintf(DBG_LEV_ERR, "alloc error! no enough space");
        return result;
    }
    memset(fmb, 0, sizeof(iftFontUnit));

    iftUcs4HashMeta *pmeta = ucs4hash_find(&hfg->ucs4hash, ucs4, NULL);
    /**
     * collect font info
     */
    result = E_PROC_CHAR_RESULT_CONTINUE;
    do
    {
        if (!pmeta)
        {
            pmeta = ucs4hashMeta_attach(&hfg->ucs4hash);
            if (!pmeta)
            {
                //
                MYTRACE();
                result = E_PROC_CHAR_RESULT_ERROR;
                break;
            }
            pmeta->ucs4 = ucs4;
            if ((bret = ftCoreRenderMeta(hfg, ucs4, &pmeta->meta)) < 0)
            {
                //not get rendermata by freetype
                MYTRACE();
                break;
            }
            if ((bret = ucs4hash_add(&hfg->ucs4hash, pmeta)) < 0)
            {
                ucs4hashMeta_detach(&hfg->ucs4hash, pmeta);
                MYTRACE();
                break;
            }
        }
        fmb->glyph = &pmeta->meta;
        pmeta->meta.refcnt++;

        fmb->chfont.font = &hfg->servCtx.pstl->fontconf;
        fmb->chfont.ch.ucs4 = ucs4;
        SCoordinate2D bakcoo = fmb->chfont.ch.coo;
        int32_t bak_adv = hfg->servCtx.renderTable.lmu[line].widePix;
        fmb->chfont.ch.coo.y = curGlyphAdvanceY;
        fmb->chfont.ch.coo.x = hfg->servCtx.renderTable.lmu[line].widePix;
        hfg->servCtx.renderTable.lmu[line].widePix += IFT_F26DOT6_TO_PIX_FLOOR(fmb->glyph->glyphmetrics.horiAdvance); //pos to phx
        if (hfg->servCtx.renderTable.lmu[line].widePix > hfg->servCtx.rect.wide)
        {
            //rollback
            hfg->servCtx.renderTable.lmu[line].widePix = bak_adv;
            fmb->chfont.ch.coo = bakcoo;
            result = E_PROC_CHAR_RESULT_BREAK;
            break;
        }
        hfg->servCtx.renderTable.lmu[line].fontcnt++;
        /**
         * link word
         */
        iftFontUnitHdr_pushback(&hfg->servCtx.renderTable.lmu[line].lineHdr, fmb);
        bret = 0;
    }
    while (0);

    if (bret < 0)
    {
        //free & error return
        mpool_detach(hfg->servCtx.renderTable.fontUnitPool, fmb);
    }
    return result;
}
/**
 *
 */
int32_t iftFtgen_lineString(iftFtgen_H hfg, int32_t line, char* utf8str, int32_t len, E_CHAR_SET chset)
{
    //todo
    //change to unicode
    iftChar32 ucs4;
    int l;
    iftStyle * cursty = hfg->servCtx.pstl;
    char * tmpch = utf8str;
    int32_t curGlyphAdvanceY = cursty->fontLineHight * (line);
    while (len > 0)
    {
        switch (chset)
        {
            case E_CHAR_SET_UTF8:
            {
                l = iftUtf8ToUcs4(tmpch, &ucs4, len);
                break;
            }
            case E_CHAR_SET_GB2312:
            {
                l = iftGb2312ToUcs4(tmpch, &ucs4, len);
                break;
            }
            default:
            {
                l = iftUtf8ToUcs4(tmpch, &ucs4, len);
                break;
            }
        }
        if (l < 0 || ucs4 == 0)
        {
            tmpch += 1;
            len -= 1;
            continue;
        }
        //next character
        tmpch += l;
        len -= l;
        E_PROC_CHAR_RESULT result = iftFtgen_addOneChar2Line(hfg, ucs4, line, curGlyphAdvanceY);
        switch (result)
        {
            case E_PROC_CHAR_RESULT_BREAK:
            case E_PROC_CHAR_RESULT_ERROR:
                break;
            case E_PROC_CHAR_RESULT_CONTINUE:
                continue;
        }
    } //end while
    return 0;
}
#if 0
/**
 * dump render buff to pic (.tga)
 */
int32_t iftFtgen_dumpPic(iftFtgen_H hfg)
{
    //todo to tga pic
    int32_t curline = 0;
    iftStyle * cursty = hfg->servCtx.pstl;
    //int charWith = cursty->fontconf.maxAdvanceWidth + cursty->lineconf.charspace;
    int charHight = cursty->fontconf.height + cursty->lineconf.super.linegap;

    iftColor *imgbuf =
        (iftColor *) iftMemAlloc(hfg->servCtx.rect.hight * hfg->servCtx.rect.wide * sizeof(iftColor));
    if (!imgbuf)
    {
        dprintf(DBG_LEV_ERR, "alloc error! no space");
        return -1;
    }
    memset(imgbuf, 0x00, hfg->servCtx.rect.hight * hfg->servCtx.rect.wide * sizeof(iftColor));
    for (; curline < hfg->renderTable.maxLines; curline++)
    {
        iftFontMemHash *curhash = hfg->renderTable.lmu[curline].lineHdr.head;
        /**
         * dump line
         */
        while (curhash)
        {
            //            int32_t fontAdvance = (curhash->chfont.font->maxAdvanceWidth);
            //            int32_t fontHight = curhash->chfont.font->height;
            show_distimage(curhash->bmp.buffer, curhash->bmp.rows, curhash->bmp.width);
            // curhash->chfont.ch.coo
            //curhash->data
            //curhash->datalen
            //curhash->chfont.font->maxAdvanceWidth
            // curhash->chfont.font->height
            //draw one char
            //*****************(x,y)************(x+width,y)
            //*****************(x,y+1)**********(x+width,y+1)
            //*****************(x,y+2)**********(x+width,y+2)
            //*****************(x,y+hight)*******(x+width,y+hight)

            int relativeX = 0;
            int relativeY = 0;
            iftColor color = curhash->chfont.font->super.color;
            color.a = 255;
            assert(curhash->bmp.pixel_mode == FT_PIXEL_MODE_GRAY);
            for (; relativeY < curhash->bmp.rows; relativeY++)
            {
                relativeX = 0;
                for (; relativeX < curhash->bmp.width; relativeX++)
                {
                    int32_t tranx = curhash->chfont.ch.coo.x
                                    + IFT_F26DOT6_TO_PIX_FLOOR(curhash->glyphmetrics.horiBearingX);
                    int32_t trany = curhash->chfont.ch.coo.y
                                    + charHight - IFT_F26DOT6_TO_PIX_FLOOR(curhash->glyphmetrics.horiBearingY);
                    uint8_t cur_gray = curhash->bmp.buffer[relativeY * curhash->bmp.width + relativeX];
                    if (cur_gray != 0)
                    {
                        int32_t pos = (trany + relativeY) * hfg->servCtx.rect.wide + tranx + relativeX;
                        imgbuf[pos] = color;
                    } //end  if(cur_gray!= 0)
                } //end for(relativeX;
            } //end for(relativeY;
            curhash = curhash->next;
        } //end curhash

    } //end line
    WriteTGA("./tmp.tga", imgbuf, hfg->servCtx.rect.wide, hfg->servCtx.rect.hight);
    iftMemFree(imgbuf);
    return 0;
}
#endif

int32_t iftFtgen_wordBuf(iftFtgen_H hfg, int32_t lineNum, iftParaLineIdx *lines, const int8_t *wordBuf,
                         E_CHAR_SET chset)
{
    if (!((lineNum > 0) && lines && wordBuf))
    {
        dprintf(DBG_LEV_ERR, "err in param: lineNum=%d,lines=%p,wordbuf=%p", lineNum, lines, wordBuf);
    }
    assert((lineNum > 0) && lines && wordBuf);
    int32_t nret = -1;
    int i = 0;
    for (i = 0; i < lineNum && i < hfg->servCtx.renderTable.maxLines; i++)
    {
        nret = iftFtgen_lineString(hfg, i, (char*)(wordBuf + lines[i].linepos), lines[i].len, chset);
        if (nret < 0)
        {
            MYTRACE();
            return -1;
        }
    }
    return 0;
}
static int drawGraydata(iftFtgen_H hfg, SFrameData *pFrm)
{
    uint8_t * imgbuf = pFrm->data.un.d;
    iftRenderFontTable *ptable = &hfg->servCtx.renderTable;
    pFrm->params->MaxFontAdvance = ptable->lmu[0].widePix;
    /**
     * set data
     */
    iftStyle * cursty = hfg->servCtx.pstl;
    int32_t curline = 0;
    int lineWidth = 0, yoffset = 0, hori_y = 0;
    //int fontWidth = 0;

    for (curline = 0; curline < 1/*ptable->maxLines*/; curline++)
    {
        iftFontUnit *fu = ptable->lmu[curline].lineHdr.head;
        int fontWidth = 0;
        //fprintf(stderr, "=====[ %p, ", fu);
        while (fu)
        {
            hori_y = IFT_F26DOT6_TO_PIX_FLOOR(fu->glyph->glyphmetrics.horiBearingY) - cursty->fontconf.height;
            if (hori_y > yoffset) { yoffset = hori_y; }

            lineWidth = fu->chfont.ch.coo.x;
            if (fu->glyph->bmp.width > 0)
            {
                fontWidth = fu->glyph->bmp.width;
            }
            else
            {
                fontWidth = 0;
            }
            fu = fu->next;
        }
        //if(lineWidth > 0)
        lineWidth += fontWidth;
        lineWidth += 8;
        //fprintf(stderr, " pixWidth = %d, fontWidth = %d ]=====\n", lineWidth, fontWidth);
    }

    for (curline = 0; curline < 1/*ptable->maxLines*/; curline++)
    {
        iftFontUnit *fu = ptable->lmu[curline].lineHdr.head;
        if (pFrm->params->MaxFontAdvance < ptable->lmu[curline].widePix)
        {
            pFrm->params->MaxFontAdvance = ptable->lmu[curline].widePix;
        }
        /**
         * dump line
         */
        while (fu)
        {
            // curhash->chfont.ch.coo
            //curhash->data
            //curhash->datalen
            //curhash->chfont.font->maxAdvanceWidth
            // curhash->chfont.font->height
            //draw one char
            //*****************|(x,y)************(x+width,y)
            //*****************|(x,y+1)**********(x+width,y+1)
            //*****************|(x,y+2)**********(x+width,y+2)
            //*****************|(x,y+hight)*******(x+width,y+hight)

            int relativeX = 0;
            int relativeY = 0;
            assert(fu->glyph->bmp.pixel_mode == FT_PIXEL_MODE_GRAY);
            int32_t trany = /*fu->chfont.ch.coo.y*/
                yoffset + cursty->fontconf.height - IFT_F26DOT6_TO_PIX_FLOOR(fu->glyph->glyphmetrics.horiBearingY);
            trany = (trany >> 1) << 1;
            int32_t tranx = fu->chfont.ch.coo.x + IFT_F26DOT6_TO_PIX_FLOOR(fu->glyph->glyphmetrics.horiBearingX);
            int32_t bpos;
            //printf("=======lineWidth = %d, tranx = %d, trany = %d, width = %d, rows = %d, pitch = %d=======\n",
            //    lineWidth, tranx, trany, fu->glyph->bmp.width, fu->glyph->bmp.rows, fu->glyph->bmp.pitch);
            if (fu->glyph->bmp.width <= 0)
            {
                fu = fu->next;
                continue;
            }
            for (; relativeY < fu->glyph->bmp.rows; relativeY++)
            {
                relativeX = 0;
#if 0
                for (; relativeX < curhash->bmp.width; relativeX++)
                {

                    uint8_t cur_gray = curhash->bmp.buffer[relativeY * curhash->bmp.width + relativeX];
                    if (cur_gray != 0)
                    {
                        int32_t pos = (trany + relativeY) * pFrm->params->pixWidth + tranx + relativeX;
                        imgbuf[pos] = cur_gray;
                    } //end  if(cur_gray!= 0)
                } //end for(relativeX;
#else
                bpos = (trany + relativeY) * lineWidth + tranx;
                memcpy(imgbuf + bpos,
                       fu->glyph->bmp.buffer + (relativeY * fu->glyph->bmp.width),
                       (fu->glyph->bmp.width));
#endif
            } //end for(relativeY;
            // dumpBitMap(&fu->glyph->bmp,0);
            fu = fu->next;
        } //end curhash
        //printf("==================================================\n");
    } //end line
    pFrm->params->pixWidth = lineWidth;
    //fprintf(stderr, "=====[ pixWidth = %d ]=====\n", lineWidth);
    return 0;
}
#if 0
static void dumpBitMap(const FT_Bitmap * bmp, int b_num)
{
    int i, j;
    if (!bmp)
    {
        return;
    }
    dprintf(DBG_LEV_INF, "FT_Bitmap:buffer=0x%p,num_grays=%d,pixel_mode=%d,rows=%d,width=%d,pitch=%d"
            , bmp->buffer, bmp->num_grays, bmp->pixel_mode, bmp->rows, bmp->width, bmp->pitch);
    for (i = 0; i < bmp->rows; i++)
    {
        for (j = 0; j < bmp->width; j++)
        {
            //                  putchar(image[i][j] == 0 ? ' ' : image[i][j] < 128 ? '+' : '*');
            uint8_t bt = bmp->buffer[i * bmp->pitch + j];
            if (b_num)
            {
                putchar((bt == 0) ? '0' : (bt + '0'));
            }
            else
            {
                char tmp = bt / 16;
                if (bt && (tmp == 0))
                {
                    tmp = 1 ;
                }
                if (tmp >= 10)
                {
                    tmp = 'A' + tmp - 10;
                }
                else
                {
                    tmp = '0' + tmp;
                }
                putchar((bt == 0) ? '0' : tmp);
            }
        }
        putchar('\n');
    }
}
#endif
int32_t iftFtgen_dumpGrayFrame2pool(iftFtgen_H hfg, void* frampool, int poolsize)
{
    assert(frampool);
    int32_t nStruFrmdata = sizeof(SFrameData);
    int32_t nStruFrmParam = sizeof(SFrameParam);
    int32_t nFrmdata = (hfg->servCtx.rect.hight) * (hfg->servCtx.rect.wide) * sizeof(uint8_t);
    int32_t realUseLen = (nFrmdata + nStruFrmParam + nStruFrmdata);
    if (poolsize < realUseLen)
    {
        MYTRACE();
        return -1;
    }
    /*
     * clean pool data
     */
    memset(frampool, 0, poolsize);
    SFrameData *frm = (SFrameData*) frampool;
    frm->params = (SFrameParam*)(frampool + nStruFrmdata);
    frm->data.un.d = (frampool + nStruFrmdata + nStruFrmParam);

    frm->params->pixHight = hfg->servCtx.rect.hight;
    frm->params->pixWidth = hfg->servCtx.rect.wide;
    frm->params->type = E_Frame_GRAY256;
    frm->dtype = E_FRMDATA_TYPE_UNION;
    frm->data.un.len = nFrmdata;
    return drawGraydata(hfg, frm);
}

int32_t iftFtgen_dumpGreyFrame(iftFtgen_H hfg, SFrameData **ppFtFrm)
{
    assert(hfg && ppFtFrm && (*ppFtFrm == NULL));
    *ppFtFrm = (SFrameData *) iftMemAlloc(sizeof(SFrameData));
    if (!(*ppFtFrm))
    {
        MYTRACE();
        iftFtgen_FreeDumpGreyFrame(ppFtFrm);
        return -1;
    }
    SFrameParam * pFrmPara = (SFrameParam *) iftMemAlloc(sizeof(SFrameParam));
    if (!pFrmPara)
    {
        MYTRACE();
        iftFtgen_FreeDumpGreyFrame(ppFtFrm);
        return -1;
    }
    pFrmPara->type = E_Frame_GRAY256;
    pFrmPara->pixHight = hfg->servCtx.rect.hight;
    pFrmPara->pixWidth = hfg->servCtx.rect.wide;
    (*ppFtFrm)->params = pFrmPara;
    (*ppFtFrm)->dtype = E_FRMDATA_TYPE_UNION;
    (*ppFtFrm)->data.un.len = (pFrmPara->pixHight) * (pFrmPara->pixWidth) * sizeof(uint8_t);
    (*ppFtFrm)->data.un.d = (uint8_t *) iftMemAlloc((*ppFtFrm)->data.un.len);
    uint8_t * imgbuf = (*ppFtFrm)->data.un.d;
    if (!imgbuf)
    {
        dprintf(DBG_LEV_ERR, "alloc error! no space");
        iftFtgen_FreeDumpGreyFrame(ppFtFrm);
        return -1;
    }
    memset(imgbuf, 0x00, (*ppFtFrm)->data.un.len);
    return drawGraydata(hfg, *ppFtFrm);
}
int32_t iftFtgen_FreeDumpGreyFrame(SFrameData **ppFtFrm)
{
    assert(ppFtFrm && (*ppFtFrm));
    if ((*ppFtFrm)->params)
    {
        iftMemFree((*ppFtFrm)->params);
        (*ppFtFrm)->params = NULL;
    }
    if ((*ppFtFrm)->dtype == E_FRMDATA_TYPE_UNION && (*ppFtFrm)->data.un.d)
    {
        iftMemFree((*ppFtFrm)->data.un.d);
        (*ppFtFrm)->data.un.d = NULL;
    }
    iftMemFree(*ppFtFrm);
    (*ppFtFrm) = NULL;
    return 0;

}

#include "iftFrameOperate.h"
#define DEF_YUV_WIDE 2592
#define DEF_YUV_HIGH 1936
int test_dumpYUV(iftFtgen_H hfg, iftColor *bgClr)
{
    FILE *pf_in = fopen("./kepler1.yuv", "r+b");
    FILE *pf_out = fopen("./kepler1.out.yuv", "w+b");
    if (!pf_in || !pf_out)
    {
        return -1;
    }
    fseek(pf_in, 0, SEEK_END);
    int32_t fsize = ftell(pf_in);
    fseek(pf_in, 0, SEEK_SET);
    uint8_t *frm_yuv_buff = (uint8_t *) malloc(fsize);
    int32_t nlen = fread(frm_yuv_buff, 1, fsize, pf_in);
    SFrameData YUVFrmData;
    SFrameParam frmParam;
    frmParam.pixHight = DEF_YUV_HIGH;
    frmParam.pixWidth = DEF_YUV_WIDE;
    frmParam.type = E_Frame_YUV420SP_UV;
    YUVFrmData.params = &frmParam;
    YUVFrmData.dtype = E_FRMDATA_TYPE_UNION;
    YUVFrmData.data.un.d = frm_yuv_buff;
    YUVFrmData.data.un.len = nlen;

    iftCanvas canv;
    canv.bgColor.a = bgClr->a;
    canv.bgColor.r = bgClr->r;
    canv.bgColor.g = bgClr->g;
    canv.bgColor.b = bgClr->b;
    canv.rect = hfg->servCtx.rect;
    canv.rect.orgi.x = 20;
    canv.rect.orgi.y = 20;

    SFrameData *fontFrmData;
    int nret = iftFtgen_dumpGreyFrame(hfg, &fontFrmData);
    nret = iftFrmOper_overlay(&hfg->servCtx.pstl->fontconf.super.color,
                              (iftCanvasInt*) &canv,
                              fontFrmData,
                              &YUVFrmData);

    //    nret = iftFrmOper_overlay_outline(&hfg->servCtx.pstl->fontconf.super.color,
    //            (iftCanvasInt*) &canv,
    //            fontFrmData,
    //            &YUVFrmData,&hfg->servCtx.pstl->fontconf.super.extra,NULL);
    nret = iftFtgen_FreeDumpGreyFrame(&fontFrmData);

    fwrite(frm_yuv_buff, 1, fsize, pf_out);

    free(frm_yuv_buff);
    fclose(pf_in);
    fclose(pf_out);
    return 0;
}
#if 0
int osdFontGenTest(int argc, char** argv)
{
    //process flow
    IftEnv gOsdenv =
    {
        .dbgLevel = 5,
        .version = 0x01010100,
    };
    iftFtgen_setEnv(&gOsdenv);
    /**
     * process
     */
    iftFtgen_H genHand = iftFtgen_hand();
    if (!genHand)
    {
        printf("error return!\n");
        return -1;
    }
    iftOsdServStyle style;
    memset(&style, 0, sizeof(style));
    iftFtgen_curStyle(genHand, (iftOsdServStyleInt*) &style);
    // iftColor clr={255,255,0,255};
    style.pt = 32;
    style.color.a = 255;
    style.color.r = 0;
    style.color.g = 255;
    style.color.b = 0;
    iftFtgen_setStyle(genHand, (iftOsdServStyleInt*) &style);
    iftFtgen_setRect(genHand, 800, 200);
    iftFtgen_sessionEnter(genHand);
    //char bufstr[] = "hello";
    char bufstr[] = "hello中国   :,---/@#%^\'\"";
    char bufstr2[] = "SPS  seq 序列参数集1234567890";

    iftFtgen_utf8String(genHand, 0, bufstr, sizeof(bufstr));
    iftFtgen_utf8String(genHand, 1, bufstr2, sizeof(bufstr2));

    //
    /**
     * dump to pic
     */
    iftColor bgcolor =
    {
        .a = 255,
        .r = 255,
        .g = 0,
        .b = 0,
    };
    test_dumpYUV(genHand, &bgcolor);
    //   iftFtgen_dumpPic(genHand);
    iftFtgen_sessionExit(genHand);

    iftFtgen_destory(genHand);
    return 0;
}
#endif
