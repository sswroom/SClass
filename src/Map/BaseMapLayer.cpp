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
	Map::IMapDrawLayer *lyr;
	Map::TileMap *tileMap;
	switch (blt)
	{
	case BLT_OSM_TILE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMTile");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.openstreetmap.org/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.openstreetmap.org/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.openstreetmap.org/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_CYCLE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMOpenCycleMap");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/cycle/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/cycle/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/cycle/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_TRANSP:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMTransport");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/transport/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/transport/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/transport/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_LANDSCAPE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMLandscape");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/landscape/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/landscape/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/landscape/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_OUTDOORS:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMOutdoors");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/outdoors/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/outdoors/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/outdoors/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_TRANSP_DARK:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMTransportDark");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/transport-dark/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/transport-dark/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/transport-dark/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_SPINAL:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMSpinalMap");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/spinal-map/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/spinal-map/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/spinal-map/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_MAPQUEST:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMMapQuest");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://otile1.mqcdn.com/tiles/1.0.0/osm/", sbuff, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://otile2.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://otile3.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://otile4.mqcdn.com/tiles/1.0.0/osm/");
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	default:
		return 0;
	}
	
}
