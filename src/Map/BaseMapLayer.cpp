#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/BaseMapLayer.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"

UOSInt Map::BaseMapLayer::GetLayerTypes(Data::ArrayList<BaseLayerType> *layerTypes)
{
	UOSInt initCnt = layerTypes->GetCount();
	layerTypes->Add(BLT_OSM_TILE);
	layerTypes->Add(BLT_OSM_CYCLE);
	layerTypes->Add(BLT_OSM_TRANSP);
	layerTypes->Add(BLT_OSM_LANDSCAPE);
	layerTypes->Add(BLT_OSM_OUTDOORS);
	layerTypes->Add(BLT_OSM_TRANSP_DARK);
	layerTypes->Add(BLT_OSM_SPINAL);
	layerTypes->Add(BLT_OSM_MAPQUEST);
	return layerTypes->GetCount() - initCnt;
}

Text::CString Map::BaseMapLayer::BaseLayerTypeGetName(BaseLayerType blt)
{
	switch (blt)
	{
	case BLT_OSM_TILE:
		return CSTR("OSM Tile Map");
	case BLT_OSM_CYCLE:
		return CSTR("Open Cycle Map");
	case BLT_OSM_TRANSP:
		return CSTR("Transport");
	case BLT_OSM_LANDSCAPE:
		return CSTR("Landscape");
	case BLT_OSM_OUTDOORS:
		return CSTR("Outdoors");
	case BLT_OSM_TRANSP_DARK:
		return CSTR("Transport Dark");
	case BLT_OSM_SPINAL:
		return CSTR("Spinal Map");
	case BLT_OSM_MAPQUEST:
		return CSTR("MapQuest");
	default:
		return CSTR("Unknown");
	}
}

Map::IMapDrawLayer *Map::BaseMapLayer::CreateLayer(BaseLayerType blt, Net::SocketFactory *sockf, Net::SSLEngine *ssl, Parser::ParserList *parsers)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Map::IMapDrawLayer *lyr;
	Map::TileMap *tileMap;
	switch (blt)
	{
	case BLT_OSM_TILE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMTile"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.openstreetmap.org/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_CYCLE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMOpenCycleMap"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/cycle/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/cycle/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/cycle/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_TRANSP:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMTransport"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/transport/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_LANDSCAPE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMLandscape"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/landscape/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/landscape/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/landscape/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_OUTDOORS:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMOutdoors"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/outdoors/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/outdoors/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/outdoors/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_TRANSP_DARK:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMTransportDark"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/transport-dark/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport-dark/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport-dark/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_SPINAL:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMSpinalMap"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/spinal-map/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/spinal-map/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/spinal-map/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_MAPQUEST:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("OSMMapQuest"));
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(CSTR("http://otile1.mqcdn.com/tiles/1.0.0/osm/"), {sbuff, (UOSInt)(sptr - sbuff)}, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://otile2.mqcdn.com/tiles/1.0.0/osm/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://otile3.mqcdn.com/tiles/1.0.0/osm/"));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(CSTR("http://otile4.mqcdn.com/tiles/1.0.0/osm/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	default:
		return 0;
	}
	
}
