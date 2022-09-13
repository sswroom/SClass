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
		UOSInt tileWidth;
		UOSInt tileHeight;
		Text::EncodingFactory *encFact;
		Text::String *tileExt;
		Text::String *tmsURL;
		Text::String *title;
		Net::SocketFactory *sockf;
		Data::ArrayList<TileLayer*> layers;
		Math::CoordinateSystem *csys;

		void LoadXML();
	public:
		TileMapServiceSource(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, Text::CString tmsURL);
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

		virtual UOSInt GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids);
		virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly);
		virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
		virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it);
	};
}
#endif
