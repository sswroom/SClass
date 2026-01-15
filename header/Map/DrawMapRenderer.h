#ifndef _SM_MAP_DRAWMAPRENDERER
#define _SM_MAP_DRAWMAPRENDERER

#include "Map/MapRenderer.h"
#include "Map/MapScheduler.h"
#include "Map/MapEnv.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ColorConv.h"
#include "Media/ColorManager.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"

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
			UIntOS nPoints;
			Map::DrawLayerType layerType;
			UnsafeArrayOpt<Math::Coord2DDbl> points;
			IntOS xOfst;
			IntOS yOfst;
			Int32 flags;
			Map::MapEnv::FontType fontType;
			UIntOS fontStyle;
			Double totalSize;
			Double currSize;
		};

		typedef struct
		{
			Optional<Media::DrawFont> font;
			Optional<Media::DrawBrush> fontBrush;
			UIntOS buffSize;
			Optional<Media::DrawBrush> buffBrush;
		} DrawFontStyle;

		class DrawEnv
		{
		public:
			NN<Map::MapEnv> env;
			NN<Media::DrawImage> img;
			NN<Map::MapView> view;
			Bool isLayerEmpty;
			UnsafeArray<Math::RectAreaDbl> objBounds;
			UIntOS objCnt;
			UnsafeArray<MapLabels> labels;
			UIntOS labelCnt;
			UIntOS maxLabels;
			UIntOS fontStyleCnt;
			UnsafeArray<DrawFontStyle> fontStyles;
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
		NN<Media::Resizer::LanczosResizerRGB_C8> resizer;
		Bool lastLayerEmpty;
		Media::ColorProfile color;
		Optional<Media::ColorManagerSess> colorSess;
		NN<Media::ColorConv> colorConv;
		DrawType drawType;

	private:
		static UIntOS NewLabel(UnsafeArray<MapLabels> labels, UIntOS maxLabel, InOutParam<UIntOS> labelCnt, Int32 priority);
		static void SwapLabel(UnsafeArray<MapLabels> mapLabels, UIntOS index, UIntOS index2);
		static Bool LabelOverlapped(UnsafeArray<Math::RectAreaDbl> points, UIntOS nPoints, Math::RectAreaDbl rect);
		static Bool AddLabel(UnsafeArray<MapLabels> labels, UIntOS maxLabel, InOutParam<UIntOS> labelCnt, Text::CStringNN label, UIntOS nPoints, UnsafeArray<Math::Coord2DDbl> points, Int32 priority, Map::DrawLayerType recType, UIntOS fntStyle, Int32 flag, NN<Map::MapView> view, IntOS xOfst, IntOS yOfst, Map::MapEnv::FontType fontType);
		static void DrawLabels(NN<DrawEnv> denv);
		static IntOS __stdcall VImgCompare(NN<Math::Geometry::VectorImage> obj1, NN<Math::Geometry::VectorImage> obj2);
	private:
		void DrawLayers(NN<DrawEnv> denv, Optional<Map::MapEnv::GroupItem> group);
		void DrawShapes(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer, UIntOS lineStyle, UInt32 fillStyle, Double lineThick, UInt32 lineColor);
		void DrawShapesPoint(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer, UIntOS imgIndex);
		void DrawLabel(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer, UIntOS fontSytle, UIntOS labelCol, Int32 priority, Int32 flags, UIntOS imgWidth, UIntOS imgHeight, Map::MapEnv::FontType fontType);
		void DrawImageLayer(NN<DrawEnv> denv, NN<Map::MapDrawLayer> layer);
		void DrawImageObject(NN<DrawEnv> denv, NN<Media::StaticImage> img, Math::Coord2DDbl scnTL, Math::Coord2DDbl scnBR, Double srcAlpha);
		void DrawImageObjectQuad(NN<DrawEnv> denv, NN<Media::StaticImage> img, Math::Quadrilateral scnCoords, Double srcAlpha);

		static void GetCharsSize(NN<DrawEnv> denv, OutParam<Math::Coord2DDbl> size, Text::CStringNN label, Map::MapEnv::FontType fontType, UIntOS fontStyle, Double scaleW, Double scaleH);
		static void DrawChars(NN<DrawEnv> denv, Text::CStringNN str1, Math::Coord2DDbl scnPos, Double scaleW, Double scaleH, Map::MapEnv::FontType fontType, UIntOS fontStyle, Bool isAlign);
		static void DrawCharsL(NN<DrawEnv> denv, Text::CStringNN str1, UnsafeArray<Math::Coord2DDbl> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UIntOS nPoints, UIntOS thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UIntOS fontStyle, OutParam<Math::RectAreaDbl> realBounds);
		static void DrawCharsLA(NN<DrawEnv> denv, Text::CStringNN str1, UnsafeArray<Math::Coord2DDbl> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UIntOS nPoints, UIntOS thisPt, Double scaleN, Double scaleD, Map::MapEnv::FontType fontType, UIntOS fontStyle, OutParam<Math::RectAreaDbl> realBounds);
	public:
		DrawMapRenderer(NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, NN<const Media::ColorProfile> color, Optional<Media::ColorManagerSess> colorSess, DrawType drawType);
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
