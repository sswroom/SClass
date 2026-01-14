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
		UOSInt tileWidth;
		UOSInt tileHeight;
		NN<Math::CoordinateSystem> csys;

	public:
		TIFFTileMap(UOSInt tileWidth, UOSInt tileHeight);
		virtual ~TIFFTileMap();

		virtual Text::CStringNN GetName() const;
		virtual Bool IsError() const;
		virtual TileType GetTileType() const;
		virtual UOSInt GetMinLevel() const;
		virtual UOSInt GetMaxLevel() const;
		virtual Double GetLevelScale(UOSInt level) const;
		virtual UOSInt GetNearestLevel(Double scale) const;
		virtual UOSInt GetConcurrentCount() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UOSInt GetTileSize() const;
		virtual ImageType GetImageType() const;
		virtual Bool CanQuery() const;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, UOSInt level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const;

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
		virtual Optional<Media::ImageList> LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		UOSInt GetLayerCount() const;
	};
}
#endif
