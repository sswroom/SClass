#ifndef _SM_MAP_MAPCONFIG
#define _SM_MAP_MAPCONFIG
#include "Map/MapScheduler.h"
#include "Parser/ParserList.h"
///////// Not finish

namespace Map
{
	typedef struct
	{
		Int32 lineType;
		Int32 lineWidth;
		UInt32 color;
		UTF8Char *styles;
	} MapLineStyle;

	typedef struct
	{
		Int32 fontType;
		Text::String *fontName;
		UInt32 fontSizePt;
		Double thick;
		UInt32 color;
		Media::DrawFont *font;
		void *other;
	} MapFontStyle;

	typedef struct
	{
		Int32 drawType;
		Int32 minScale;
		Int32 maxScale;
		UInt32 style;
		UInt32 bkColor;
		Int32 priority;
		Media::DrawImage *img;
		Map::IMapDrawLayer *lyr;
	} MapLayerStyle;

	typedef struct
	{
		const WChar *label;
		Int32 xPos;
		Int32 yPos;
		Int32 fontStyle;
		Int32 scaleW;
		Int32 scaleH;
		Int32 priority;
		Int32 totalSize;
		Int32 currSize;
		Int32 nPoints;
		Int32 shapeType;
		Int32 *points;
		Int32 xOfst;
		Int32 yOfst;
		Int32 flags;
	} MapLabels;

	class MapConfig
	{
	private:
		Bool inited;
		Int32 bgColor;
		Int32 nLine;
		Int32 nFont;
		Int32 nStr;
		Data::ArrayList<MapLineStyle*> **lines;
		Data::ArrayList<MapFontStyle*> **fonts;
		Data::ArrayList<MapLayerStyle*> *drawList;
		Media::DrawEngine *drawEng;
		Sync::Mutex *drawMut;

		static void DrawChars(Media::DrawImage *img, const WChar *str1, Int32 xPos, Int32 yPos, Int32 scaleW, Int32 scaleH, Data::ArrayList<MapFontStyle*> *fontStyle, Bool isAlign);
		static void GetCharsSize(Media::DrawImage *img, Int32 *size, const WChar *label, Data::ArrayList<MapFontStyle*> *fontStyle, Int32 scaleW, Int32 scaleH);
		static Int32 ToColor(WChar *str);
		static Map::IMapDrawLayer *GetDrawLayer(WChar *name, Data::ArrayList<Map::IMapDrawLayer*> *layerList);
		static void DrawPoints(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch);
		static void DrawString(Media::DrawImage *img, MapLayerStyle *lyrs, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts, MapLabels *labels, Int32 maxLabels, Int32 *labelCnt, Bool *isLayerEmpty);
		static Int32 NewLabel(MapLabels *labels, Int32 maxLabel, Int32 *labelCnt, Int32 priority);
		static Bool AddLabel(MapLabels *labels, Int32 maxLabel, Int32 *labelCnt, WChar *label, Int32 nPoints, Int32 *points, Int32 priority, Int32 recType, Int32 fntStyle, Int32 flag, Map::MapView *view);
		static void SwapLabel(MapLabels *mapLabels, Int32 index, Int32 index2);
		static Int32 LabelOverlapped(Int32 *points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry);
		static void DrawLabels(Media::DrawImage *img, MapLabels *labels, Int32 maxLabel, Int32 *labelCnt, Map::MapView *view, Data::ArrayList<MapFontStyle*> **fonts);
	public:
		MapConfig(const WChar *fileName, Media::DrawEngine *eng, Data::ArrayList<Map::IMapDrawLayer*> *layerList, Parser::ParserList *parserList, WChar *forceBase);
		~MapConfig();

		Bool IsError();
		Media::DrawPen *CreatePen(Media::DrawImage *img, Int32 lineStyle, Int32 lineLayer);
		void DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch);

		static void ReleaseLayers(Data::ArrayList<Map::IMapDrawLayer*> *layerList);
	};
};
#endif
