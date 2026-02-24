#ifndef _SM_MEDIA_TIFFTILEMAP
#define _SM_MEDIA_TIFFTILEMAP
#include "Map/TileMap.h"

namespace Media
{
	class TIFFTileMap : public Map::TileMap
	{
	private:
		struct LayerInfo
		{

		};
	private:
		Data::ArrayListNN<LayerInfo> layers;
		UIntOS tileWidth;
		UIntOS tileHeight;
		NN<Math::CoordinateSystem> csys;

	public:
		TIFFTileMap(UIntOS tileWidth, UIntOS tileHeight);
		virtual ~TIFFTileMap();

		virtual Text::CStringNN GetName() const;
		virtual Bool IsError() const;
		virtual TileType GetTileType() const;
		virtual UIntOS GetMinLevel() const;
		virtual UIntOS GetMaxLevel() const;
		virtual Double GetLevelScale(UIntOS level) const;
		virtual UIntOS GetNearestLevel(Double scale) const;
		virtual UIntOS GetConcurrentCount() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UIntOS GetTileSize() const;
		virtual TileFormat GetTileFormat() const;
		virtual Bool CanQuery() const;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, UIntOS level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const;

		virtual UIntOS GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
		virtual Optional<Media::ImageList> LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<TileFormat> format);

		UIntOS GetLayerCount() const;
	};
}
#endif
