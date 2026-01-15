#ifndef _SM_MAP_REGIONALMAPSOURCE
#define _SM_MAP_REGIONALMAPSOURCE
#include "Map/MapDrawLayer.h"
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
			CustomTile,
			ESRIMap,
			WMS,
			WFS
		};
		struct MapInfo
		{
			const UTF8Char *name;
			UIntOS nameLen;
			const UTF8Char *region;
			UIntOS regionLen;
			const UTF8Char *org;
			UIntOS orgLen;
			MapType mapType;
			UIntOS mapTypeParam; //TileMap = concurrent count, ESRI Map = SRID
			const UTF8Char *url;
			UIntOS urlLen;
			const UTF8Char *desc;
			UIntOS descLen;
			UIntOS minLevel;
			UIntOS maxLevel;
			Double boundsX1;
			Double boundsY1;
			Double boundsX2;
			Double boundsY2;
		};

		static MapInfo maps[];
	public:
		static UnsafeArray<const MapInfo> GetMapInfos(OutParam<UIntOS> cnt);
		static Optional<Map::MapDrawLayer> OpenMap(NN<const MapInfo> map, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, NN<Parser::ParserList> parsers, NN<Net::WebBrowser> browser, NN<Math::CoordinateSystem> envCSys);
		static Text::CStringNN MapTypeGetName(MapType mapType);
	};
}
#endif
