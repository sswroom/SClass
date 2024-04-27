#ifndef _SM_MAP_MAPCONFIG2TGEN
#define _SM_MAP_MAPCONFIG2TGEN
#include "IO/Writer.h"
#include "Map/MapConfigBase.h"
#include "Map/MapLayerData.h"
#include "Map/MapScheduler.h"
#include "Media/IImgResizer.h"
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
			IO::Stream *dbStream;
		} DrawParam;
	private:
		typedef struct
		{
			Text::String *label;
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
			Math::Coord2DDbl *points;
			Double xOfst;
			Double yOfst;
			UInt32 flags;
		} MapLabels2;

		typedef struct
		{
			Int32 mcc;
			Map::MapLayerData *data;
		} MapArea;

	private:
		Bool inited;
		UInt32 bgColor;
		UInt32 nLine;
		UInt32 nFont;
		UOSInt nStr;
		Data::ArrayList<MapLineStyle*> **lines;
		Data::ArrayList<MapFontStyle*> **fonts;
		Data::ArrayList<MapLayerStyle*> *drawList;
		NN<Media::DrawEngine> drawEng;
		Data::ArrayList<MapArea*> *areaList;

		static Bool IsDoorNum(const UTF8Char *txt);
		static void DrawChars(NN<Media::DrawImage> img, Text::CStringNN str1, Double xPos, Double yPos, Double scaleW, Double scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign);
		static void DrawCharsLA(NN<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Math::RectAreaDbl *realBounds);
		static void DrawCharsLAo(NN<Media::DrawImage> img, Text::CStringNN str1, Double *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle);
		static void DrawCharsL(NN<Media::DrawImage> img, Text::CStringNN str1, Math::Coord2DDbl *mapPts, Math::Coord2D<Int32> *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Math::RectAreaDbl *realBounds);
		static void GetCharsSize(NN<Media::DrawImage> img, Math::Coord2DDbl *size, Text::CStringNN label, Data::ArrayList<MapFontStyle*> *fontStyle, Double scaleW, Double scaleH);
		static UInt32 ToColor(const UTF8Char *str);
		static Optional<Map::MapDrawLayer> GetDrawLayer(Text::CStringNN name, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, NN<IO::Writer> errWriter);
		static void DrawPoints(NN<Media::DrawImage> img, MapLayerStyle *lyrs, NN<Map::MapView> view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, NN<Media::DrawEngine> eng, Media::IImgResizer *resizer, Math::RectAreaDbl *objBounds, UOSInt *objCnt, UOSInt maxObjCnt);
		static void DrawString(NN<Media::DrawImage> img, MapLayerStyle *lyrs, NN<Map::MapView> view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels2 *labels, UOSInt maxLabels, UOSInt *labelCnt, Bool *isLayerEmpty);
		static UOSInt NewLabel(MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Int32 priority);
		static Bool AddLabel(MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Text::CString label, UOSInt nPoints, Math::Coord2DDbl *points, Int32 priority, Int32 recType, UInt32 fntStyle, UInt32 flag, NN<Map::MapView> view, Double xOfst, Double yOfst);
		static void SwapLabel(MapLabels2 *mapLabels, UOSInt index, UOSInt index2);
		static Bool LabelOverlapped(Math::RectAreaDbl *points, UOSInt nPoints, Math::RectAreaDbl rect);
		static void DrawLabels(NN<Media::DrawImage> img, MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, NN<Map::MapView> view, Data::ArrayList<MapFontStyle*> **fonts, NN<Media::DrawEngine> drawEng, Math::RectAreaDbl *objBounds, UOSInt *objCnt);
		static void LoadLabels(NN<Media::DrawImage> img, Map::MapConfig2TGen::MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, NN<Map::MapView> view, Data::ArrayList<MapFontStyle*> **fonts, NN<Media::DrawEngine> drawEng, Math::RectAreaDbl *objBounds, UOSInt *objCnt, Text::CString fileName, Int32 xId, Int32 yId, Double xOfst, Double yOfst, IO::Stream *dbStream);

	public:
		MapConfig2TGen(Text::CStringNN fileName, NN<Media::DrawEngine> eng, NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList, Parser::ParserList *parserList, const UTF8Char *forceBase, NN<IO::Writer> errWriter, Int32 maxScale, Int32 minScale);
		~MapConfig2TGen();

		Bool IsError();
		Optional<Media::DrawPen> CreatePen(NN<Media::DrawImage> img, UInt32 lineStyle, UOSInt lineLayer);
		WChar *DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::IImgResizer *resizer, Text::CString dbOutput, DrawParam *params);

		UInt32 GetBGColor();
		Bool SupportMCC(Int32 mcc);
		Int32 GetLocMCC(Int32 lat, Int32 lon);
		static void ReleaseLayers(NN<Data::ArrayListNN<Map::MapDrawLayer>> layerList);
	};
}
#endif
