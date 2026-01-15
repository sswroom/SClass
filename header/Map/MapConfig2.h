#ifndef _SM_MAP_MAPCONFIG2
#define _SM_MAP_MAPCONFIG2
#include "IO/Writer.h"
#include "Map/MapConfigBase.h"
#include "Map/MapLayerData.h"
#include "Map/MapScheduler.h"
#include "Media/ImageResizer.h"
#include "Parser/ParserList.h"
#include "Text/String.h"

namespace Map
{
	typedef struct
	{
		Math::Coord2DDbl pos;
		NN<Text::String> label;
		UInt32 fontStyle;
		Double scaleW;
		Double scaleH;
		Int32 priority;
		Double totalSize;
		Double currSize;
		Double mapRate;
		UIntOS nPoints;
		Int32 shapeType;
		UnsafeArrayOpt<Math::Coord2DDbl> points;
		Double xOfst;
		Double yOfst;
		UInt32 flags;
	} MapLabels2;

	class MapConfig2
	{

		typedef struct
		{
			Int32 mcc;
			Optional<Map::MapLayerData> data;
		} MapArea;
	private:
		Bool inited;
		UInt32 bgColor;
		UInt32 nLine;
		UInt32 nFont;
		UInt32 nStr;
		UnsafeArrayOpt<Optional<Data::ArrayListNN<MapLineStyle>>> lines;
		UnsafeArrayOpt<Optional<Data::ArrayListNN<MapFontStyle>>> fonts;
		Optional<Data::ArrayListNN<MapLayerStyle>> drawList;
		NN<Media::DrawEngine> drawEng;
		Data::ArrayListNN<MapArea> areaList;

		static Bool IsDoorNum(UnsafeArray<const UTF8Char> txt);
		static void DrawChars(NN<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl scnPos, Double scaleW, Double scaleH, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, Bool isAlign);
		static void DrawCharsLA(NN<Media::DrawImage> img, Text::CStringNN str1, UnsafeArray<Math::Coord2DDbl> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UIntOS nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, OutParam<Math::RectAreaDbl> realBounds);
		static void DrawCharsLAo(NN<Media::DrawImage> img, Text::CStringNN str1, UnsafeArray<Double> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UIntOS nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle);
		static void DrawCharsL(NN<Media::DrawImage> img, Text::CStringNN str1, UnsafeArray<Math::Coord2DDbl> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UIntOS nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, OutParam<Math::RectAreaDbl> realBounds);
		static void GetCharsSize(NN<Media::DrawImage> img, OutParam<Math::Coord2DDbl> size, Text::CStringNN label, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, Double scaleW, Double scaleH);
		static UInt32 ToColor(UnsafeArray<const UTF8Char> str);
		static Optional<Map::MapDrawLayer> GetDrawLayer(Text::CStringNN name, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, NN<IO::Writer> errWriter);
		static void DrawPoints(NN<Media::DrawImage> img, NN<MapLayerStyle> lyrs, NN<Map::MapView> view, OutParam<Bool> isLayerEmpty, NN<Map::MapScheduler> mapSch, NN<Media::DrawEngine> eng, Optional<Media::ImageResizer> resizer, UnsafeArray<Math::RectAreaDbl> objBounds, InOutParam<UIntOS> objCnt, UIntOS maxObjCnt);
		static void DrawString(NN<Media::DrawImage> img, NN<MapLayerStyle> lyrs, NN<Map::MapView> view, UnsafeArray<Optional<Data::ArrayListNN<MapFontStyle>>> fonts, UnsafeArray<MapLabels2> labels, UInt32 maxLabels, InOutParam<UInt32> labelCnt, OutParam<Bool> isLayerEmpty);
		static UInt32 NewLabel(UnsafeArray<MapLabels2> labels, UInt32 maxLabel, InOutParam<UInt32> labelCnt, Int32 priority);
		static Bool AddLabel(UnsafeArray<MapLabels2> labels, UInt32 maxLabel, InOutParam<UInt32> labelCnt, Text::CStringNN label, UIntOS nPoints, UnsafeArray<Math::Coord2DDbl> points, Int32 priority, Int32 recType, UInt32 fntStyle, UInt32 flags, NN<Map::MapView> view, Double xOfst, Double yOfst);
		static void SwapLabel(UnsafeArray<MapLabels2> mapLabels, UInt32 index, UInt32 index2);
		static Bool LabelOverlapped(UnsafeArray<Math::RectAreaDbl> points, UIntOS nPoints, Math::RectAreaDbl rect);
		static void DrawLabels(NN<Media::DrawImage> img, UnsafeArray<MapLabels2> labels, UInt32 maxLabel, UInt32 labelCnt, NN<Map::MapView> view, UnsafeArray<Optional<Data::ArrayListNN<MapFontStyle>>> fonts, NN<Media::DrawEngine> drawEng, UnsafeArray<Math::RectAreaDbl> objBounds, InOutParam<UIntOS> objCnt);

	public:
		MapConfig2(Text::CStringNN fileName, NN<Media::DrawEngine> eng, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, NN<Parser::ParserList> parserList, Text::CString forceBase, NN<IO::Writer> errWriter, Int32 maxScale, Int32 minScale);
		~MapConfig2();

		Bool IsError();
		Optional<Media::DrawPen> CreatePen(NN<Media::DrawImage> img, UInt32 lineStyle, UIntOS lineLayer);
		UnsafeArrayOpt<UTF8Char> DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OutParam<Bool> isLayerEmpty, NN<Map::MapScheduler> mapSch, Optional<Media::ImageResizer> resizer, UnsafeArrayOpt<UTF8Char> slowLayer, OptOut<Double> slowTime);

		UInt32 GetBGColor();
		Bool SupportMCC(Int32 mcc);
		Int32 QueryMCC(Math::Coord2DDbl pos);
		static void ReleaseLayers(NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList);
	};
}
#endif
