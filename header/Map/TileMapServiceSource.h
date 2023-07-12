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
			Text::String *url;
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
		Text::String *tmsURL;
		Text::String *title;
		Text::String *cacheDir;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Data::ArrayList<TileLayer*> layers;
		Math::CoordinateSystem *csys;
		UOSInt concurrCnt;

		void LoadXML();
	public:
		TileMapServiceSource(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString tmsURL);
		virtual ~TileMapServiceSource();

		virtual Text::CString GetName();
		virtual Bool IsError();
		virtual TileType GetTileType();
		virtual UOSInt GetLevelCount();
		virtual Double GetLevelScale(UOSInt level);
		virtual UOSInt GetNearestLevel(Double scale);
		virtual UOSInt GetConcurrentCount();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual Bool IsMercatorProj();
		virtual UOSInt GetTileSize();

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly);
		virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
		virtual IO::StreamData *LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, Math::RectAreaDbl *bounds, Bool localOnly, ImageType *it);

		void SetConcurrentCount(UOSInt concurrCnt);
	};
}
#endif
