#ifndef _SM_MAP_TILEMAPSERVICESOURCE
#define _SM_MAP_TILEMAPSERVICESOURCE
#include "Data/ArrayListT.hpp"
#include "Map/TileMap.h"
#include "Net/SocketFactory.h"
#include "Text/String.h"

namespace Map
{
	class TileMapServiceSource : public Map::TileMap
	{
	private:
		struct TileLayer
		{
			NN<Text::String> url;
			Double unitPerPixel;
			UIntOS order;
		};
	private:
		Math::RectAreaDbl bounds;
		Math::Coord2DDbl origin;
		Math::Coord2DDbl csysOrigin;
		UIntOS tileWidth;
		UIntOS tileHeight;
		Optional<Text::EncodingFactory> encFact;
		ImageType imgType;
		Text::String *tileExt;
		NN<Text::String> tmsURL;
		Text::String *title;
		NN<Text::String> cacheDir;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Data::ArrayListNN<TileLayer> layers;
		NN<Math::CoordinateSystem> csys;
		UIntOS concurrCnt;

		void LoadXML();
	public:
		TileMapServiceSource(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN tmsURL);
		virtual ~TileMapServiceSource();

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
		virtual ImageType GetImageType() const;

		virtual UIntOS GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
		virtual Optional<Media::ImageList> LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		void SetConcurrentCount(UIntOS concurrCnt);
	};
}
#endif
