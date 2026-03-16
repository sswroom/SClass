#ifndef _SM_MAP_STMDATATILEMAP
#define _SM_MAP_STMDATATILEMAP
#include "Map/TileMap.h"

namespace Map
{
	class StmDataTileMap : public TileMap
	{
	private:
		struct TileInfo
		{
			Math::Coord2D<Int32> tileId;
			NN<IO::StreamData> data;
		};

		struct TileLayer
		{
			UIntOS level;
			Double resolution;
			Data::ArrayListNN<TileInfo> tiles;
		};

		class TileLayerComparer : public Data::Comparator<NN<TileInfo>>
		{
		public:
			virtual ~TileLayerComparer() {};

			virtual IntOS Compare(NN<TileInfo> a, NN<TileInfo> b) const
			{
				if (a->tileId.y > b->tileId.y || (a->tileId.y == b->tileId.y && a->tileId.x > b->tileId.x))
				{
					return 1;
				}
				else if (a->tileId.y < b->tileId.y || (a->tileId.y == b->tileId.y && a->tileId.x < b->tileId.x))
				{
					return -1;
				}
				else
				{
					return 0;
				}
			}
		};

		Math::Coord2DDbl tileOrigin;
		Math::RectAreaDbl bounds;
		UIntOS tileSize;
		NN<Math::CoordinateSystem> csys;
		TileFormat tileFormat;
		NN<Text::String> name;
		Data::ArrayListNN<TileLayer> layers; 

		Optional<TileLayer> GetLayer(UIntOS level) const;
		Optional<TileInfo> GetTile(NN<TileLayer> layer, Math::Coord2D<Int32> tileId) const;
	public:
		StmDataTileMap(UIntOS tileSize, Math::Coord2DDbl tileOrigin, Math::RectAreaDbl bounds, NN<Math::CoordinateSystem> csys, TileFormat tileFormat, Text::CStringNN name);
		virtual ~StmDataTileMap();

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

		virtual UIntOS GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
		virtual Optional<Media::ImageList> LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<TileFormat> format);

		void AddLayer(UIntOS level, Double resolution);
		void AddTile(UIntOS level, Math::Coord2D<Int32> tileId, NN<IO::StreamData> fd, UInt64 ofst, UInt64 size);
		void BuildIndex();
	};
}
#endif
