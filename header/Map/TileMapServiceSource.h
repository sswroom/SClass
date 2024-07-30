#ifndef _SM_MAP_TILEMAPSERVICESOURCE
#define _SM_MAP_TILEMAPSERVICESOURCE
#include "Data/ArrayList.h"
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
			UOSInt order;
		};
	private:
		Math::RectAreaDbl bounds;
		Math::Coord2DDbl origin;
		Math::Coord2DDbl csysOrigin;
		UOSInt tileWidth;
		UOSInt tileHeight;
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
		UOSInt concurrCnt;

		void LoadXML();
	public:
		TileMapServiceSource(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN tmsURL);
		virtual ~TileMapServiceSource();

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

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		void SetConcurrentCount(UOSInt concurrCnt);
	};
}
#endif
