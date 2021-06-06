#ifndef _SM_MAP_MAPCONFIG2TGEN
#define _SM_MAP_MAPCONFIG2TGEN
#include "IO/Writer.h"
#include "Map/MapConfig.h"
#include "Map/MapLayerData.h"
#include "Map/MapScheduler.h"
#include "Media/IImgResizer.h"
#include "Parser/ParserList.h"

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
			const UTF8Char *label;
			Double xPos;
			Double yPos;
			UInt32 fontStyle;
			Double scaleW;
			Double scaleH;
			Int32 priority;
			Double totalSize;
			Double currSize;
			Double mapRate;
			UOSInt nPoints;
			Int32 shapeType;
			Double *points;
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
		Media::DrawEngine *drawEng;
		Data::ArrayList<MapArea*> *areaList;

		static Bool IsDoorNum(const UTF8Char *txt);
		static void DrawChars(Media::DrawImage *img, const UTF8Char *str1, Double xPos, Double yPos, Double scaleW, Double scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign);
		static void DrawCharsLA(Media::DrawImage *img, const UTF8Char *str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Double *realBounds);
		static void DrawCharsLAo(Media::DrawImage *img, const UTF8Char *str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle);
		static void DrawCharsL(Media::DrawImage *img, const UTF8Char *str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Double *realBounds);
		static void GetCharsSize(Media::DrawImage *img, Double *size, const UTF8Char *label, Data::ArrayList<MapFontStyle*> *fontStyle, Double scaleW, Double scaleH);
		static UInt32 ToColor(const UTF8Char *str);
		static Map::IMapDrawLayer *GetDrawLayer(const UTF8Char *name, Data::ArrayList<Map::IMapDrawLayer*> *layerList, IO::Writer *errWriter);
		static void DrawPoints(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::DrawEngine *eng, Media::IImgResizer *resizer, Double *objBounds, UOSInt *objCnt, UOSInt maxObjCnt);
		static void DrawString(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels2 *labels, UOSInt maxLabels, UOSInt *labelCnt, Bool *isLayerEmpty);
		static UOSInt NewLabel(MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Int32 priority);
		static Bool AddLabel(MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, const UTF8Char *label, UOSInt nPoints, Double *points, Int32 priority, Int32 recType, UInt32 fntStyle, UInt32 flag, Map::MapView *view, Double xOfst, Double yOfst);
		static void SwapLabel(MapLabels2 *mapLabels, UOSInt index, UOSInt index2);
		static Bool LabelOverlapped(Double *points, UOSInt nPoints, Double tlx, Double tly, Double brx, Double bry);
		static void DrawLabels(Media::DrawImage *img, MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, Media::DrawEngine *drawEng, Double *objBounds, UOSInt *objCnt);
		static void LoadLabels(Media::DrawImage *img, Map::MapConfig2TGen::MapLabels2 *labels, UOSInt maxLabel, UOSInt *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, Media::DrawEngine *drawEng, Double *objBounds, UOSInt *objCnt, const UTF8Char *fileName, Int32 xId, Int32 yId, Double xOfst, Double yOfst, IO::Stream *dbStream);

	public:
		MapConfig2TGen(const UTF8Char *fileName, Media::DrawEngine *eng, Data::ArrayList<Map::IMapDrawLayer*> *layerList, Parser::ParserList *parserList, const UTF8Char *forceBase, IO::Writer *errWriter, Int32 maxScale, Int32 minScale);
		~MapConfig2TGen();

		Bool IsError();
		Media::DrawPen *CreatePen(Media::DrawImage *img, UInt32 lineStyle, UOSInt lineLayer);
		WChar *DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::IImgResizer *resizer, const UTF8Char *dbOutput, DrawParam *params);

		UInt32 GetBGColor();
		Bool SupportMCC(Int32 mcc);
		Int32 GetLocMCC(Int32 lat, Int32 lon);
		static void ReleaseLayers(Data::ArrayList<Map::IMapDrawLayer*> *layerList);
	};
}
#endif
