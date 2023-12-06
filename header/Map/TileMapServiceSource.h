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
			NotNullPtr<Text::String> url;
			Double unitPerPixel;
			UOSInt order;
		};
	private:
		Math::RectAreaDbl bounds;
		Math::Coord2DDbl origin;
		Math::Coord2DDbl csysOrigin;
		UOSInt tileWidth;
		UOSInt tileHeight;
		Text::EncodingFactory *encFact;
		ImageType imgType;
		Text::String *tileExt;
		NotNullPtr<Text::String> tmsURL;
		Text::String *title;
		NotNullPtr<Text::String> cacheDir;
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Data::ArrayList<TileLayer*> layers;
		NotNullPtr<Math::CoordinateSystem> csys;
		UOSInt concurrCnt;

		void LoadXML();
	public:
		TileMapServiceSource(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::CString tmsURL);
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
		virtual NotNullPtr<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual Bool IsMercatorProj() const;
		virtual UOSInt GetTileSize() const;
		virtual ImageType GetImageType() const;

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NotNullPtr<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
		virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

		void SetConcurrentCount(UOSInt concurrCnt);
	};
}
#endif
