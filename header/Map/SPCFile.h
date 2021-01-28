#ifndef _SM_MAP_SPCFILE
#define _SM_MAP_SPCFILE
#include "Map/IMapConfig.h"

namespace Map
{
	typedef struct
	{
		Int32 thickness;
		Int32 lineColor;
		UInt8 *others;
		Int32 othersLen;
	} SPCLineLayer;

	typedef struct
	{
		Int32 lineStyle;
		Data::ArrayList<SPCLineLayer*> *layers;
	} SPCLineStyle;

	typedef struct
	{
		const WChar *fontName;
		Int32 fontSize;
		Int32 fontStyle; //1 = bold
		Int32 buffSize;
		Int32 buffColor;
		Int32 fontColor;

		Media::DrawFont *font;
		Media::DrawBrush *brush;
		Media::DrawBrush *brushBuff;
	} SPCFontStyle;

	typedef struct
	{
		Int32 drawType;
		Int32 minScale;
		Int32 maxScale;
		Int32 style;
		Int32 fillColor;
		Int32 lineId;
		Int32 fontId;
		Int32 grpId;
		Media::DrawImage *img;
		Map::IMapDrawLayer *lyr;
	} SPCLayerStyle;

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
		Double mapRate;
		Int32 nPoints;
		Int32 shapeType;
		Int32 *points;
		Int32 flags;
	} SPCLabels;

	class SPCFile : public Map::IMapConfig
	{
	private:
		Bool inited;
		Int32 bgColor;
		Int32 nLine;
		Int32 nFont;
		Int32 nStr;

		SPCLineStyle *lines;
		SPCFontStyle *fonts;
		Data::ArrayList<SPCLayerStyle*> *drawList;
		Media::DrawEngine *drawEng;
		Sync::Mutex *drawMut;

		Int32 debug;

		static void DrawChars(Media::DrawImage *img, const WChar *str1, Int32 xPos, Int32 yPos, Int32 scaleW, Int32 scaleH, SPCFontStyle *fontStyle, Bool isAlign);
		static void GetCharsSize(Media::DrawImage *img, Int32 *size, const WChar *label, SPCFontStyle *fontStyle, Int32 scaleW, Int32 scaleH);
		static Int32 ToColor(Int32 c);
		static Map::IMapDrawLayer *GetDrawLayer(const WChar *name, Data::ArrayList<Map::SPDLayer*> *layerList);
		static void DrawPoints(Media::DrawImage *img, SPCLayerStyle *lyrs, Map::MapView *view, Bool *isLayerEmpty);
		static void DrawString(Media::DrawImage *img, SPCLayerStyle *lyrs, Int32 flags, Map::MapView *view, SPCFontStyle *fonts, SPCLabels *labels, Int32 maxLabels, Int32 *labelCnt, Bool *isLayerEmpty, Int32 *debug);
		static Int32 NewLabel(SPCLabels *labels, Int32 maxLabel, Int32 *labelCnt, Int32 priority);
		static Bool AddLabel(SPCLabels *labels, Int32 maxLabel, Int32 *labelCnt, WChar *label, Int32 nPoints, Int32 *points, Int32 priority, Int32 recType, Int32 fntStyle, Int32 flag, Map::MapView *view, Int32 *debug, Double mapRate);
		static void SwapLabel(SPCLabels *mapLabels, Int32 index, Int32 index2);
		static Int32 LabelOverlapped(Int32 *points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry);
		static void DrawLabels(Media::DrawImage *img, SPCLabels *labels, Int32 maxLabel, Int32 *labelCnt, Map::MapView *view, SPCFontStyle *fonts);
		static void FreeLabels(SPCLabels *labels, Int32 maxLabel, Int32 *labelCnt);
	public:
		SPCFile(WChar *fileName, Media::DrawEngine *eng, Data::ArrayList<Map::SPDLayer*> *layerList);
		virtual ~SPCFile();

		virtual Bool IsError();
		Media::DrawPen *CreatePen(Media::DrawImage *img, Int32 lineStyle, Int32 lineLayer, Int32 scale, Int32 maxScale);
		virtual Int32 DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty);
		Int32 GetBGColor();

		WChar *GetPGLabel(WChar *buff, Int32 mapX, Int32 mapY, Double mapRate);
		WChar *GetPLLabel(WChar *buff, Int32 mapX, Int32 mapY, Int32 *lineX, Int32 *lineY, Double mapRate);

		Int32 GetLastError();
		static void ReleaseLayers(Data::ArrayList<Map::SPDLayer*> *layerList);
	};
};
#endif
