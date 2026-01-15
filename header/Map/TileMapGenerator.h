#ifndef _SM_MAP_TILEMAPGENERATOR
#define _SM_MAP_TILEMAPGENERATOR
#include "Map/MapConfig2TGen.h"


namespace Map
{
	class TileMapGenerator
	{
	private:
		UInt32 imgSize;
		NN<Text::String> tileDir;
		NN<Map::MapConfig2TGen> mcfg;
		NN<Media::DrawEngine> geng;
		UIntOS osSize;

		Data::ArrayListInt64 dbGenList;
		Sync::Mutex dbMut;
		Sync::Event dbEvt;
		NN<Media::ImageResizer> resizer;

	private:
		void InitMapView(NN<Map::MapView> view, Int32 x, Int32 y, UInt32 scale);
		UnsafeArray<UTF8Char> GenFileName(UnsafeArray<UTF8Char> sbuff, Int32 x, Int32 y, UInt32 scale, Text::CStringNN ext);
		void AppendDBFile(NN<IO::Writer> writer, Int32 x, Int32 y, UInt32 scale, Int32 xOfst, Int32 yOfst);
		Bool GenerateDBFile(Int32 x, Int32 y, UInt32 scale, NN<Map::MapScheduler> mapSch);
	public:
		TileMapGenerator(NN<Map::MapConfig2TGen> mcfg, NN<Media::DrawEngine> geng, Text::CStringNN tileDir, UIntOS osSize);
		~TileMapGenerator();

		static Math::Coord2D<Int32> GetTileID(Double lat, Double lon, UInt32 scale, UInt32 imgSize);

		Bool GenerateTile(Int64 tileId, UInt32 scale, NN<Map::MapScheduler> mapSch);
		Bool GenerateTileArea(Double lat1, Double lon1, Double lat2, Double lon2, UInt32 scale, NN<Map::MapScheduler> mapSch);
	};
}
#endif
