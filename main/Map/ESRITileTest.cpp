#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayListInt64.h"
#include "IO/FileStream.h"
#include "Map/DrawMapRenderer.h"
#include "Map/MercatorMapView.h"
#include "Map/TileMapLayer.h"
#include "Map/ESRI/ESRITileMap.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Media/DrawEngineFactory.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FullParserList.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Parser::ParserList> parsers;
	NN<Net::SocketFactory> sockf;
	NN<Net::TCPClientFactory> clif;
	Optional<Net::SSLEngine> ssl = nullptr;
	NN<Map::TileMap> map;
	NN<Media::DrawEngine> eng;
	NN<Media::DrawImage> dimg;
	NN<Map::MapView> view;
	NN<Map::DrawMapRenderer> renderer;
	NN<Map::TileMapLayer> lyr;
	NN<IO::FileStream> fs;

	NEW_CLASSNN(sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(clif, Net::TCPClientFactory(sockf));
	ssl = Net::SSLEngineFactory::Create(clif, false);
	NEW_CLASSNN(parsers, Parser::FullParserList());
	eng = Media::DrawEngineFactory::CreateDrawEngine();
	NEW_CLASSNN(map, Map::OSM::OSMTileMap(CSTR("https://tile.openstreetmap.org/"), CSTR("OSMTile"), 0, 18, clif, ssl));
//	NEW_CLASS(map, Map::OSM::OSMTileMap(CSTR("http://b.tile.opencyclemap.org/cycle/"), CSTR("OSMOpenCycleMap"), 0, 16, sockf));
//	NEW_CLASS(map, Map::OSM::OSMTileMap(CSTR("http://b.tile2.opencyclemap.org/transport/"), CSTR("OSMTransport"), 0, 18, sockf));
//	NEW_CLASS(map, Map::OSM::OSMTileMap(CSTR("http://otile2.mqcdn.com/tiles/1.0.0/osm/"), CSTR("OSMMapQuest"), 0, 18, sockf));
	{
		Map::MapEnv env(CSTR("TileMapEnv.txt"), 0xffffffff, Math::CoordinateSystemManager::CreateWGS84Csys());
		NEW_CLASSNN(lyr, Map::TileMapLayer(map, parsers));
		env.AddLayer(nullptr, lyr, true);
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		NEW_CLASSNN(renderer, Map::DrawMapRenderer(eng, env, color, nullptr, Map::DrawMapRenderer::DT_PIXELDRAW));
		view = lyr->CreateMapView(Math::Size2DDbl(1024, 768));
		view->SetCenterXY(Math::Coord2DDbl(113.54148023667012, 22.1905749713822295));
		view->SetMapScale(map->GetLevelScale(4));

		if (eng->CreateImage32(Math::Size2D<UIntOS>(1024, 768), Media::AT_ALPHA).SetTo(dimg))
		{
			renderer->DrawMap(dimg, view, nullptr);
			NEW_CLASSNN(fs, IO::FileStream(CSTR("TileMap.png"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			dimg->SavePng(fs);
			fs.Delete();
			eng->DeleteImage(dimg);
		}

		view.Delete();
	}
	renderer.Delete();
	eng.Delete();
	parsers.Delete();
	ssl.Delete();
	clif.Delete();
	sockf.Delete();
	return 0;
}
