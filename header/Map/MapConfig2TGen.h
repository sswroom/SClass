#ifndef _SM_MAP_MAPCONFIG2TGEN
#define _SM_MAP_MAPCONFIG2TGEN
#include "IO/Writer.h"
#include "Map/MapConfigBase.h"
#include "Map/MapLayerData.h"
#include "Map/MapScheduler.h"
#include "Media/ImageResizer.h"
#include "Parser/ParserList.h"
#include "Text/String.h"

namespace Map
{
	class MapConfig2TGen
	{
	public:
		typedef struct
		{
			Int32 tileX;
			Int32 tileY;
			Int32 labelType; // 0 = write only, 1 = read neighbour, 2 = read from dbStream
			Optional<IO::Stream> dbStream;
		} DrawParam;
	private:
		typedef struct
		{
			NN<Text::String> label;
			Math::Coord2DDbl pos;
			UInt32 fontStyle;
			Double scaleW;
			Double scaleH;
			Int32 priority;
			Double totalSize;
			Double currSize;
			Double mapRate;
			UOSInt nPoints;
			Int32 shapeType;
			UnsafeArrayOpt<Math::Coord2DDbl> points;
			Double xOfst;
			Double yOfst;
			UInt32 flags;
		} MapLabels2;

	private:
		Bool inited;
		UInt32 bgColor;
		UInt32 nLine;
		UInt32 nFont;
		UOSInt nStr;
		UnsafeArrayOpt<Optional<Data::ArrayListNN<MapLineStyle>>> lines;
		UnsafeArrayOpt<Optional<Data::ArrayListNN<MapFontStyle>>> fonts;
		Optional<Data::ArrayListNN<MapLayerStyle>> drawList;
		NN<Media::DrawEngine> drawEng;

		static Bool IsDoorNum(UnsafeArray<const UTF8Char> txt);
		static void DrawChars(NN<Media::DrawImage> img, Text::CStringNN str1, Double xPos, Double yPos, Double scaleW, Double scaleH, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, Bool isAlign);
		static void DrawCharsLA(NN<Media::DrawImage> img, Text::CStringNN str1, UnsafeArray<Math::Coord2DDbl> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, OutParam<Math::RectAreaDbl> realBounds);
		static void DrawCharsLAo(NN<Media::DrawImage> img, Text::CStringNN str1, UnsafeArray<Double> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle);
		static void DrawCharsL(NN<Media::DrawImage> img, Text::CStringNN str1, UnsafeArray<Math::Coord2DDbl> mapPts, UnsafeArray<Math::Coord2DDbl> scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, OutParam<Math::RectAreaDbl> realBounds);
		static void GetCharsSize(NN<Media::DrawImage> img, OutParam<Math::Coord2DDbl> size, Text::CStringNN label, Optional<Data::ArrayListNN<MapFontStyle>> fontStyle, Double scaleW, Double scaleH);
		static UInt32 ToColor(UnsafeArray<const UTF8Char> str);
		static Optional<Map::MapDrawLayer> GetDrawLayer(Text::CStringNN name, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, NN<IO::Writer> errWriter);
		static void DrawPoints(NN<Media::DrawImage> img, NN<MapLayerStyle> lyrs, NN<Map::MapView> view, OutParam<Bool> isLayerEmpty, NN<Map::MapScheduler> mapSch, NN<Media::DrawEngine> eng, Optional<Media::ImageResizer> resizer, UnsafeArray<Math::RectAreaDbl> objBounds, InOutParam<UOSInt> objCnt, UOSInt maxObjCnt);
		static void DrawString(NN<Media::DrawImage> img, NN<MapLayerStyle> lyrs, NN<Map::MapView> view, UnsafeArray<Optional<Data::ArrayListNN<MapFontStyle>>> fonts, UnsafeArray<MapLabels2> labels, UOSInt maxLabels, UnsafeArray<UOSInt> labelCnt, OutParam<Bool> isLayerEmpty);
		static UOSInt NewLabel(UnsafeArray<MapLabels2> labels, UOSInt maxLabel, UnsafeArray<UOSInt> labelCnt, Int32 priority);
		static Bool AddLabel(UnsafeArray<MapLabels2> labels, UOSInt maxLabel, UnsafeArray<UOSInt> labelCnt, Text::CStringNN label, UOSInt nPoints, UnsafeArray<Math::Coord2DDbl> points, Int32 priority, Int32 recType, UInt32 fntStyle, UInt32 flag, NN<Map::MapView> view, Double xOfst, Double yOfst);
		static void SwapLabel(UnsafeArray<MapLabels2> mapLabels, UOSInt index, UOSInt index2);
		static Bool LabelOverlapped(UnsafeArray<Math::RectAreaDbl> points, UOSInt nPoints, Math::RectAreaDbl rect);
		static void DrawLabels(NN<Media::DrawImage> img, UnsafeArray<MapLabels2> labels, UOSInt maxLabel, UnsafeArray<UOSInt> labelCnt, NN<Map::MapView> view, UnsafeArray<Optional<Data::ArrayListNN<MapFontStyle>>> fonts, NN<Media::DrawEngine> drawEng, UnsafeArray<Math::RectAreaDbl> objBounds, InOutParam<UOSInt> objCnt);
		static void LoadLabels(NN<Media::DrawImage> img, UnsafeArray<MapLabels2> labels, UOSInt maxLabel, UnsafeArray<UOSInt> labelCnt, NN<Map::MapView> view, UnsafeArray<Optional<Data::ArrayListNN<MapFontStyle>>> fonts, NN<Media::DrawEngine> drawEng, UnsafeArray<Math::RectAreaDbl> objBounds, InOutParam<UOSInt> objCnt, Text::CStringNN fileName, Int32 xId, Int32 yId, Double xOfst, Double yOfst, Optional<IO::Stream> dbStream);

	public:
		MapConfig2TGen(Text::CStringNN fileName, NN<Media::DrawEngine> eng, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, NN<Parser::ParserList> parserList, UnsafeArrayOpt<const UTF8Char> forceBase, NN<IO::Writer> errWriter, Int32 maxScale, Int32 minScale);
		~MapConfig2TGen();

		Bool IsError();
		Optional<Media::DrawPen> CreatePen(NN<Media::DrawImage> img, UInt32 lineStyle, UOSInt lineLayer);
		Bool DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OutParam<Bool> isLayerEmpty, NN<Map::MapScheduler> mapSch, Optional<Media::ImageResizer> resizer, Text::CString dbOutput, NN<DrawParam> params);

		UInt32 GetBGColor();
		Bool SupportMCC(Int32 mcc);
		Int32 GetLocMCC(Int32 lat, Int32 lon);
		static void ReleaseLayers(NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList);
	};
}
#endif
