#ifndef _SM_MAP_ESRI_ESRIMAPSERVER
#define _SM_MAP_ESRI_ESRIMAPSERVER
#include "Data/ArrayListDbl.h"
#include "IO/Stream.h"
#include "Map/DrawMapService.h"
#include "Math/CoordinateSystem.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/JSON.h"
#include "Text/String.h"

namespace Map
{
	namespace ESRI
	{
		class ESRIMapServer : public Map::DrawMapService
		{
		private:
			Text::String *url;
			Text::String *name;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Math::RectAreaDbl bounds;
			Math::RectAreaDbl initBounds;
			Math::CoordinateSystem *csys;

			Bool supportMap;
			Bool supportTileMap;
			Bool supportQuery;
			Bool supportData;

			Math::Coord2DDbl tileOrigin;
			UOSInt tileWidth;
			UOSInt tileHeight;
			Data::ArrayListDbl tileLevels;

		public:
			ESRIMapServer(Text::CString url, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
			virtual ~ESRIMapServer();

			Bool IsError() const;
			Bool HasTile() const;

			Text::String *GetURL() const;
			Math::RectAreaDbl GetBounds() const;

			UOSInt TileGetLevelCount() const;
			Double TileGetLevelResolution(UOSInt level) const;
			UOSInt TileGetWidth() const;
			UOSInt TileGetHeight() const;
			Math::Coord2DDbl TileGetOrigin() const;
			UTF8Char *TileGetURL(UTF8Char *sbuff, UOSInt level, Int32 tileX, Int32 tileY) const;
			Bool TileLoadToStream(IO::Stream *stm, UOSInt level, Int32 tileX, Int32 tileY) const;
			Bool TileLoadToFile(Text::CString fileName, UOSInt level, Int32 tileX, Int32 tileY) const;

			Math::Geometry::Vector2D *Identify(Math::Coord2DDbl pt, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);

			virtual Text::String *GetName() const;
			virtual Math::CoordinateSystem *GetCoordinateSystem() const;
			virtual Math::RectAreaDbl GetInitBounds() const;
			virtual Bool GetBounds(Math::RectAreaDbl *bounds) const;
			virtual Bool CanQuery() const;
			virtual Math::Geometry::Vector2D *QueryInfo(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);
			virtual Media::ImageList *DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Text::StringBuilderUTF8 *sbUrl);

			static Math::Geometry::Vector2D *ParseGeometry(UInt32 srid, Text::String *geometryType, Text::JSONBase *geometry);
		};
	}
}
#endif