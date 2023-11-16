#ifndef _SM_MAP_MAPCONFIGBASE
#define _SM_MAP_MAPCONFIGBASE
#include "Map/MapDrawLayer.h"
#include "Media/DrawEngine.h"
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
		NotNullPtr<Text::String> fontName;
		UInt32 fontSizePt;
		Double thick;
		UInt32 color;
		NotNullPtr<Media::DrawFont> font;
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
		Map::MapDrawLayer *lyr;
	} MapLayerStyle;
}
#endif
