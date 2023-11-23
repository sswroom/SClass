#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/BaseMapLayer.h"
#include "Map/TileMapLayer.h"
#include "Map/GoogleMap/GoogleTileMap.h"
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
	layerTypes->Add(BLT_GMAP_MAP);
	layerTypes->Add(BLT_GMAP_TRAIN);
	layerTypes->Add(BLT_GMAP_HYBRID);
	layerTypes->Add(BLT_GMAP_SATELITE);
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
	case BLT_GMAP_MAP:
		return CSTR("Google Map");
	case BLT_GMAP_TRAIN:
		return CSTR("Google Map Terrain");
	case BLT_GMAP_HYBRID:
		return CSTR("Google Map Hybrid");
	case BLT_GMAP_SATELITE:
		return CSTR("Google Map Satelite");
	default:
		return CSTR("Unknown");
	}
}

Map::MapDrawLayer *Map::BaseMapLayer::CreateLayer(BaseLayerType blt, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, NotNullPtr<Parser::ParserList> parsers)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Map::MapDrawLayer *lyr;
	NotNullPtr<Map::TileMap> tileMap;
	switch (blt)
	{
	case BLT_OSM_TILE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.openstreetmap.org/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_CYCLE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOpenCycleMap"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/cycle/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/cycle/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/cycle/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_TRANSP:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransport"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/transport/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_LANDSCAPE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMLandscape"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/landscape/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/landscape/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/landscape/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_OUTDOORS:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOutdoors"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/outdoors/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/outdoors/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/outdoors/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_TRANSP_DARK:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransportDark"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/transport-dark/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport-dark/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport-dark/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_SPINAL:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMSpinalMap"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/spinal-map/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/spinal-map/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/spinal-map/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_OSM_MAPQUEST:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMMapQuest"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://otile1.mqcdn.com/tiles/1.0.0/osm/"), CSTRP(sbuff, sptr), 0, 18, sockf, ssl));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://otile2.mqcdn.com/tiles/1.0.0/osm/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://otile3.mqcdn.com/tiles/1.0.0/osm/"));
		((Map::OSM::OSMTileMap*)tileMap.Ptr())->AddAlternateURL(CSTR("http://otile4.mqcdn.com/tiles/1.0.0/osm/"));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_GMAP_MAP:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("GMap"));
		NEW_CLASSNN(tileMap, Map::GoogleMap::GoogleTileMap(CSTRP(sbuff, sptr), Map::GoogleMap::GoogleTileMap::MT_MAP, sockf, ssl));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_GMAP_TRAIN:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("GMapTrain"));
		NEW_CLASSNN(tileMap, Map::GoogleMap::GoogleTileMap(CSTRP(sbuff, sptr), Map::GoogleMap::GoogleTileMap::MT_TRAIN, sockf, ssl));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_GMAP_HYBRID:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("GMapHybrid"));
		NEW_CLASSNN(tileMap, Map::GoogleMap::GoogleTileMap(CSTRP(sbuff, sptr), Map::GoogleMap::GoogleTileMap::MT_HYBRID, sockf, ssl));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	case BLT_GMAP_SATELITE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("GMapSatelite"));
		NEW_CLASSNN(tileMap, Map::GoogleMap::GoogleTileMap(CSTRP(sbuff, sptr), Map::GoogleMap::GoogleTileMap::MT_SATELITE, sockf, ssl));
		NEW_CLASS(lyr, Map::TileMapLayer(tileMap, parsers));
		return lyr;
	default:
		return 0;
	}
	
}
