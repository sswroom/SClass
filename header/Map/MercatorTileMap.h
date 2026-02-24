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
		Optional<IO::SPackageFile> spkg;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		UIntOS minLevel;
		UIntOS maxLevel;

		UIntOS tileWidth;
		UIntOS tileHeight;
		NN<Math::CoordinateSystem> csys;

	public:
		MercatorTileMap(Text::CString cacheDir, UIntOS minLevel, UIntOS maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		virtual ~MercatorTileMap();

		void SetSPackageFile(Optional<IO::SPackageFile> spkg);
		Bool HasSPackageFile();
		Bool ImportTiles(NN<IO::PackageFile> pkg);
		Bool OptimizeToFile(Text::CStringNN fileName);

		virtual Bool IsError() const;
		virtual UIntOS GetMinLevel() const;
		virtual UIntOS GetMaxLevel() const;
		virtual Double GetLevelScale(UIntOS level) const;
		virtual UIntOS GetNearestLevel(Double scale) const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UIntOS GetTileSize() const;

		virtual UIntOS GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
		virtual Optional<Media::ImageList> LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual Optional<IO::StreamData> LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<TileFormat> format);

		static Int32 Lon2TileX(Double lon, UIntOS level);
		static Int32 Lat2TileY(Double lat, UIntOS level);
		static Int32 Lon2TileXR(Double lon, UIntOS level);
		static Int32 Lat2TileYR(Double lat, UIntOS level);
		static Double TileX2Lon(Int32 x, UIntOS level);
		static Double TileY2Lat(Int32 y, UIntOS level);
	};
}
#endif
