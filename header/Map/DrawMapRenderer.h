#ifndef _SM_MAP_DRAWMAPRENDERER
#define _SM_MAP_DRAWMAPRENDERER

#include "Map/MapRenderer.h"
#include "Map/MapScheduler.h"
#include "Map/MapEnv.h"
#include "Math/Geometry/VectorImage.h"
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
			Text::String* label;
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
			Optional<Media::DrawBrush> fontBrush;
			UOSInt buffSize;
			Optional<Media::DrawBrush> buffBrush;
		} DrawFontStyle;

		class DrawEnv
		{
		public:
			NN<Map::MapEnv> env;
			NN<Media::DrawImage> img;
			NN<Map::MapView> view;
			Bool isLayerEmpty;
			Math::RectAreaDbl *objBounds;
			UOSInt objCnt;
			MapLabels *labels;
			UOSInt labelCnt;
			UOSInt maxLabels;
			UOSInt fontStyleCnt;
			DrawFontStyle *fontStyles;
			UInt32 imgDurMS;
			Math::Size2DDbl dispSize;

			Data::ArrayListInt64 idArr;
			Data::ArrayListNN<Media::DrawFont> layerFont;
			Data::ArrayListNN<Media::DrawBrush> layerFontColor;
		};

	private:
		Map::MapScheduler mapSch;
		NN<Media::DrawEngine> eng;
		NN<Map::MapEnv> env;
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
		static Bool AddLabel(MapLabels *labels, UOSInt maxLabel, UOSInt *labelCnt, Text::CString label, UOSInt nPoints, Math::Coord2DDbl *points, Int32 priority, Map::DrawLayerType recType, UOSInt fntStyle, Int32 flag, NN<Map::MapView> view, OSInt xOfst, OSInt yOfst, Map::MapEnv::FontType fontType);
		static void DrawLabels(NN<DrawEnv> denv);
		static OSInt __stdcall VImgCompare(Math::Geometry::VectorImage *obj1, Math::Geometry::VectorImage *obj2);
	private:
		void DrawLayers(NN<DrawEnv> denv, Optional<Map::MapEnv::GroupItem> group);
		void DrawShapes(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer, UOSInt lineStyle, UInt32 fillStyle, Double lineThick, UInt32 lineColor);
		void DrawShapesPoint(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer, UOSInt imgIndex);
		void DrawLabel(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer, UOSInt fontSytle, UOSInt labelCol, Int32 priority, Int32 flags, UOSInt imgWidth, UOSInt imgHeight, Map::MapEnv::FontType fontType);
		void DrawImageLayer(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer);
		void DrawImageObject(NN<DrawEnv> denv, NN<Media::StaticImage> img, Math::Coord2DDbl scnTL, Math::Coord2DDbl scnBR, Double srcAlpha);

		static void GetCharsSize(NN<DrawEnv> denv, Math::Coord2DDbl *size, Text::CStringNN label, Map::MapEnv::FontType fontType, UOSInt fontStyle, Double scaleW, Double scaleH);
		static void DrawChars(NN<DrawEnv> denv, Text::CStringNN str1, Math::Coord2DDbl scnPos, Double scaleW, Double scaleH, Map::MapEnv::FontType fontType, UOSInt fontStyle, Bool isAlign);
		static void DrawCharsL(NN<DrawEnv> denv, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Math::RectAreaDbl *realBounds);
		static void DrawCharsLA(NN<DrawEnv> denv, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UOSInt thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UOSInt fontStyle, Math::RectAreaDbl *realBounds);
	public:
		DrawMapRenderer(NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, NN<const Media::ColorProfile> color, Media::ColorManagerSess *colorSess, DrawType drawType);
		virtual ~DrawMapRenderer();

		virtual void DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OptOut<UInt32> imgDurMS);
		virtual void SetUpdatedHandler(Map::MapRenderer::UpdatedHandler updHdlr, AnyType userObj);
		Bool GetLastsLayerEmpty();
		void SetEnv(NN<Map::MapEnv> env);
		NN<Map::MapEnv> GetEnv() const;
		void ColorUpdated();
	};
}
#endif
