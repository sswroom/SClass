#ifndef _SM_MAP_MAPCONFIG2
#define _SM_MAP_MAPCONFIG2
#include "IO/Writer.h"
#include "Map/MapConfig.h"
#include "Map/MapLayerData.h"
#include "Map/MapScheduler.h"
#include "Media/IImgResizer.h"
#include "Parser/ParserList.h"
#include "Text/String.h"

namespace Map
{
	typedef struct
	{
		Text::String *label;
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

	class MapConfig2
	{

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
		UInt32 nStr;
		Data::ArrayList<MapLineStyle*> **lines;
		Data::ArrayList<MapFontStyle*> **fonts;
		Data::ArrayList<MapLayerStyle*> *drawList;
		Media::DrawEngine *drawEng;
		Data::ArrayList<MapArea*> *areaList;

		static Bool IsDoorNum(const UTF8Char *txt);
		static void DrawChars(Media::DrawImage *img, Text::CString str1, Double xPos, Double yPos, Double scaleW, Double scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign);
		static void DrawCharsLA(Media::DrawImage *img, Text::CString str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Double *realBounds);
		static void DrawCharsLAo(Media::DrawImage *img, Text::CString str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle);
		static void DrawCharsL(Media::DrawImage *img, Text::CString str1, Double *mapPts, Int32 *scnPts, UOSInt nPoints, UInt32 thisPt, Double scaleN, Double scaleD, Data::ArrayList<MapFontStyle*> *fontStyle, Double *realBounds);
		static void GetCharsSize(Media::DrawImage *img, Double *size, Text::CString label, Data::ArrayList<MapFontStyle*> *fontStyle, Double scaleW, Double scaleH);
		static UInt32 ToColor(const UTF8Char *str);
		static Map::IMapDrawLayer *GetDrawLayer(Text::CString name, Data::ArrayList<Map::IMapDrawLayer*> *layerList, IO::Writer *errWriter);
		static void DrawPoints(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::DrawEngine *eng, Media::IImgResizer *resizer, Double *objBounds, UOSInt *objCnt, UOSInt maxObjCnt);
		static void DrawString(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels2 *labels, UInt32 maxLabels, UInt32 *labelCnt, Bool *isLayerEmpty);
		static UInt32 NewLabel(MapLabels2 *labels, UInt32 maxLabel, UInt32 *labelCnt, Int32 priority);
		static Bool AddLabel(MapLabels2 *labels, UInt32 maxLabel, UInt32 *labelCnt, Text::CString label, UInt32 nPoints, Double *points, Int32 priority, Int32 recType, UInt32 fntStyle, UInt32 flags, Map::MapView *view, Double xOfst, Double yOfst);
		static void SwapLabel(MapLabels2 *mapLabels, UInt32 index, UInt32 index2);
		static Bool LabelOverlapped(Double *points, UOSInt nPoints, Double tlx, Double tly, Double brx, Double bry);
		static void DrawLabels(Media::DrawImage *img, MapLabels2 *labels, UInt32 maxLabel, UInt32 *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, Media::DrawEngine *drawEng, Double *objBounds, UOSInt *objCnt);
		static void LoadLabels(Media::DrawImage *img, Map::MapLabels2 *labels, UInt32 maxLabel, UInt32 *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, Media::DrawEngine *drawEng, Double *objBounds, UOSInt *objCnt, const WChar *fileName, Int32 xId, Int32 yId, Int32 xOfst, Int32 yOfst, const WChar *dbName);

	public:
		MapConfig2(Text::CString fileName, Media::DrawEngine *eng, Data::ArrayList<Map::IMapDrawLayer*> *layerList, Parser::ParserList *parserList, Text::CString forceBase, IO::Writer *errWriter, Int32 maxScale, Int32 minScale);
		~MapConfig2();

		Bool IsError();
		Media::DrawPen *CreatePen(Media::DrawImage *img, UInt32 lineStyle, UOSInt lineLayer);
		UTF8Char *DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch, Media::IImgResizer *resizer, UTF8Char *slowLayer, Double *slowTime);

		UInt32 GetBGColor();
		Bool SupportMCC(Int32 mcc);
		Int32 GetLocMCCXY(Double x, Double y);
		static void ReleaseLayers(Data::ArrayList<Map::IMapDrawLayer*> *layerList);
	};
}
#endif
