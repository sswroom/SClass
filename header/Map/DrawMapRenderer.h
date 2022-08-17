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
		ASTRUCT MapLabels
		{
			Math::Coord2DDbl pos;
			Text::String *label;
			Double scaleW;
			Double scaleH;
			Int32 priority;
			UOSInt nPoints;
			Map::DrawLayerType layerType;
			Math::Coord2DDbl *points;
			OSInt xOfst;
			OSInt yOfst;
			Int32 flags;
			Map::MapEnv::FontType fontType;
			UOSInt fontStyle;
			Double totalSize;
			Double currSize;
		};

		typedef struct
		{
			Media::DrawFont *font;
			Media::DrawBrush *fontBrush;
			UOSInt buffSize;
			Media::DrawBrush *buffBrush;
		} DrawFontStyle;

		class DrawEnv
		{
		public:
			Map::MapEnv *env;
			Media::DrawImage *img;
			Map::MapView *view;
			Bool isLayerEmpty;
			Math::RectAreaDbl *objBounds;
			UOSInt objCnt;
			MapLabels *labels;
			UOSInt labelCnt;
			UOSInt maxLabels;
			UOSInt fontStyleCnt;
			DrawFontStyle *fontStyles;
			UInt32 imgDurMS;

			Data::ArrayListInt64 idArr;
			Data::ArrayList<Media::DrawFont*> layerFont;
			Data::ArrayList<Media::DrawBrush*> layerFontColor;
		};

	private:
		Map::MapScheduler mapSch;
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
		static void SwapLabel(MapLabels *mapLabels, UOSInt index, UOSInt index2);
		static Bool LabelOverlapped(Math::RectAreaDbl *points, UOSInt nPoints, Math::RectAreaDbl rect);
		static Bool AddLabel(MapLabels *labels, UOSInt maxLabel, UOSInt *labelCnt, Text::CString label, UOSInt nPoints, Math::Coord2DDbl *points, Int32 priority, Map::DrawLayerType recType, UOSInt fntStyle, Int32 flag, Map::MapView *view, OSInt xOfst, OSInt yOfst, Map::MapEnv::FontType fontType);
		static void DrawLabels(DrawEnv *denv);
		static OSInt __stdcall VImgCompare(void *obj1, void *obj2);
	private:
		void DrawLayers(DrawEnv *denv, Map::MapEnv::GroupItem *group);
		void DrawShapes(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt lineStyle, UInt32 fillStyle, UOSInt lineThick, UInt32 lineColor);
		void DrawShapesPoint(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt imgIndex);
		void DrawLabel(DrawEnv *denv, Map::IMapDrawLayer *layer, UOSInt fontSytle, UOSInt labelCol, Int32 priority, Int32 flags, UOSInt imgWidth, UOSInt imgHeight, Map::MapEnv::FontType fontType);
		void DrawImageLayer(DrawEnv *denv, Map::IMapDrawLayer *layer);
		void DrawImageObject(DrawEnv *denv, Media::StaticImage *img, Double scnX1, Double scnY1, Double scnX2, Double scnY2, Double srcAlpha);

		static void GetCharsSize(DrawEnv *denv, Math::Coord2DDbl *size, Text::CString label, Map::MapEnv::FontType fontType, UOSInt fontStyle, Double scaleW, Double scaleH);
		static void DrawChars(DrawEnv *denv, Text::CString str1, Double xPos, Double yPos, Double scaleW, Double scaleH, Map::MapEnv::FontType fontType, UOSInt fontStyle, Bool isAlign);
		static void DrawCharsL(DrawEnv *denv, Text::CString str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Math::RectAreaDbl *realBounds);
		static void DrawCharsLA(DrawEnv *denv, Text::CString str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Math::RectAreaDbl *realBounds);
	public:
		DrawMapRenderer(Media::DrawEngine *eng, Map::MapEnv *env, const Media::ColorProfile *color, Media::ColorManagerSess *colorSess, DrawType drawType);
		virtual ~DrawMapRenderer();

		virtual void DrawMap(Media::DrawImage *img, Map::MapView *view, UInt32 *imgDurMS);
		virtual void SetUpdatedHandler(Map::MapRenderer::UpdatedHandler updHdlr, void *userObj);
		Bool GetLastsLayerEmpty();
		void SetEnv(Map::MapEnv *env);
		void ColorUpdated();
	};
}
#endif
