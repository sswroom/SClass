#ifndef _SM_MAP_REGIONALMAPSOURCE
#define _SM_MAP_REGIONALMAPSOURCE
#include "Map/IMapDrawLayer.h"
#include "Net/SSLEngine.h"
#include "Net/WebBrowser.h"
#include "Parser/ParserList.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	class RegionalMapSource
	{
	public:
		enum class MapType
		{
			TMS,
			File,
			CustomTile
		};
		struct MapInfo
		{
			const UTF8Char *name;
			UOSInt nameLen;
			const UTF8Char *region;
			UOSInt regionLen;
			const UTF8Char *org;
			UOSInt orgLen;
			MapType mapType;
			UOSInt concurrCnt;
			const UTF8Char *url;
			UOSInt urlLen;
			const UTF8Char *desc;
			UOSInt descLen;
			UOSInt minLevel;
			UOSInt maxLevel;
			Double boundsX1;
			Double boundsY1;
			Double boundsX2;
			Double boundsY2;
		};

		static MapInfo maps[];
	public:
		static const MapInfo *GetMapInfos(UOSInt *cnt);
		static Map::IMapDrawLayer *OpenMap(const MapInfo *map, Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Parser::ParserList *parsers, Net::WebBrowser *browser);
	};
}
#endif