#ifndef _SM_MAP_MERCATORTILEMAP
#define _SM_MAP_MERCATORTILEMAP
#include "IO/PackageFile.h"
#include "IO/SPackageFile.h"
#include "Map/TileMap.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Text/CString.h"

namespace Map
{
	class MercatorTileMap : public Map::TileMap
	{
	protected:
		Optional<Text::String> cacheDir;
		IO::SPackageFile *spkg;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		UOSInt minLevel;
		UOSInt maxLevel;

		UOSInt tileWidth;
		UOSInt tileHeight;
		NN<Math::CoordinateSystem> csys;

	public:
		MercatorTileMap(Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		virtual ~MercatorTileMap();

		void SetSPackageFile(IO::SPackageFile *spkg);
		Bool HasSPackageFile();
		Bool ImportTiles(NN<IO::PackageFile> pkg);
		Bool OptimizeToFile(Text::CStringNN fileName);

		virtual Bool IsError() const;
		virtual UOSInt GetMinLevel() const;
		virtual UOSInt GetMaxLevel() const;
		virtual Double GetLevelScale(UOSInt level) const;
		virtual UOSInt GetNearestLevel(Double scale) const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UOSInt GetTileSize() const;

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		static Int32 Lon2TileX(Double lon, UOSInt level);
		static Int32 Lat2TileY(Double lat, UOSInt level);
		static Int32 Lon2TileXR(Double lon, UOSInt level);
		static Int32 Lat2TileYR(Double lat, UOSInt level);
		static Double TileX2Lon(Int32 x, UOSInt level);
		static Double TileY2Lat(Int32 y, UOSInt level);
	};
}
#endif
