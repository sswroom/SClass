#ifndef _SM_MAP_DRAWMAPRENDERER
#define _SM_MAP_DRAWMAPRENDERER

#include "Map/MapRenderer.h"
#include "Map/MapScheduler.h"
#include "Map/MapEnv.h"
#include "Media/ColorConv.h"
#include "Media/ColorManager.h"
#include "Media/Resizer/LanczosResizer8_C8.h"

namespace Map
{
	class DrawMapRenderer : public Map::MapRenderer
	{
	public:
		typedef enum
		{
			DT_PIXELDRAW,
			DT_VECTORDRAW
		} DrawType;
	private:
		typedef struct
		{
			const UTF8Char *label;
			Double lat;
			Double lon;
			UOSInt fontStyle;
			Double scaleW;
			Double scaleH;
			Int32 priority;
			Double totalSize;
			Double currSize;
			UOSInt nPoints;
			Map::DrawLayerType layerType;
			Double *points;
			OSInt xOfst;
			OSInt yOfst;
			Int32 flags;
		} MapLabels;

		typedef struct
		{
			Media::DrawFont *font;
			Media::DrawBrush *fontBrush;
			UOSInt buffSize;
			Media::DrawBrush *buffBrush;
		} DrawFontStyle;

		typedef struct
		{
			Map::MapEnv *env;
			Media::DrawImage *img;
			Map::MapView *view;
			Bool isLayerEmpty;
			Double *objBounds;
			UOSInt objCnt;
			MapLabels *labels;
			UOSInt labelCnt;
			UOSInt maxLabels;
			UOSInt fontStyleCnt;
			DrawFontStyle *fontStyles;
			Int32 imgDurMS;

			Data::ArrayListInt64 *idArr;
			Data::ArrayList<Media::DrawFont*> *layerFont;
			Data::ArrayList<Media::DrawBrush*> *layerFontColor;
		} DrawEnv;

	private:
		Map::MapScheduler *mapSch;
		Media::DrawEngine *eng;
		Map::MapEnv *env;
		Media::Resizer::LanczosResizer8_C8 *resizer;
		Bool lastLayerEmpty;
		Media::ColorProfile color;
		Media::ColorManagerSess *colorSess;
		Media::ColorConv *colorConv;
		DrawType drawType;

	private:
		static UOSInt NewLabel(MapLabels *labels, UOSInt maxLabel, UOSInt *labelCnt, Int32 priority);
		static void SwapLabel(MapLabels *mapLabels, OSInt index, OSInt index2);
		static Bool LabelOverlapped(Double *points, UOSInt nPoints, Double tlx, Double tly, Double brx, Double bry);
		static Bool AddLabel(MapLabels *labels, UOSInt maxLabel, UOSInt *labelCnt, const UTF8Char *label, UOSInt nPoints, Double *points, Int32 priority, Map::DrawLayerType recType, UOSInt fntStyle, Int32 flag, Map::MapView *view, OSInt xOfst, OSInt yOfst);
		static void DrawLabels(DrawEnv *denv);
		static OSInt __stdcall VImgCompare(void *obj1, void *obj2);
	private:
		void DrawLayers(DrawEnv *denv, Map::MapEnv::GroupItem *group);
		void DrawPLLayer(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt lineStyle, UOSInt lineThick, UInt32 lineColor);
		void DrawPGLayer(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt lineStyle, Int32 fillStyle, UOSInt lineThick, UInt32 lineColor);
		void DrawPTLayer(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt imgIndex);
		void DrawLabel(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt fontSytle, Int32 labelCol, Int32 priority, Int32 flags, UOSInt imgWidth, UOSInt imgHeight, Map::DrawLayerType layerType);
		void DrawImageLayer(DrawEnv *denv, Map::IMapDrawLayer *layer);
		void DrawImageObject(DrawEnv *denv, Media::StaticImage *img, Double scnX1, Double scnY1, Double scnX2, Double scnY2, Double srcAlpha);

		static void GetCharsSize(DrawEnv *denv, Double *size, const UTF8Char *label, UOSInt fontStyle, Double scaleW, Double scaleH);
		static void DrawChars(DrawEnv *denv, const UTF8Char *str1, Double xPos, Double yPos, Double scaleW, Double scaleH, UOSInt fontStyle, Bool isAlign);
		static void DrawCharsL(DrawEnv *denv, const UTF8Char *str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, UOSInt fontStyle, Double *realBounds);
		static void DrawCharsLA(DrawEnv *denv, const UTF8Char *str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, UOSInt fontStyle, Double *realBounds);
	public:
		DrawMapRenderer(Media::DrawEngine *eng, Map::MapEnv *env, const Media::ColorProfile *color, Media::ColorManagerSess *colorSess, DrawType drawType);
		virtual ~DrawMapRenderer();

		virtual void DrawMap(Media::DrawImage *img, Map::MapView *view, Int32 *imgDurMS);
		virtual void SetUpdatedHandler(Map::MapRenderer::UpdatedHandler updHdlr, void *userObj);
		Bool GetLastsLayerEmpty();
		void SetEnv(Map::MapEnv *env);
		void ColorUpdated();
	};
}
#endif
