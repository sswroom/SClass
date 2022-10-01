#ifndef _SM_MAP_ESRI_ESRIMAPSERVER
#define _SM_MAP_ESRI_ESRIMAPSERVER
#include "Data/ArrayListDbl.h"
#include "IO/Stream.h"
#include "Math/CoordinateSystem.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/JSON.h"
#include "Text/String.h"

namespace Map
{
	namespace ESRI
	{
		class ESRIMapServer
		{
		private:
			Text::String *url;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Math::RectAreaDbl bounds;
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
			~ESRIMapServer();

			Bool IsError() const;
			Bool HasTile() const;

			Text::String *GetURL() const;
			Math::CoordinateSystem *GetCoordinateSystem() const;
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

			static Math::Geometry::Vector2D *ParseGeometry(UInt32 srid, Text::String *geometryType, Text::JSONBase *geometry);
		};
	}
}
#endif
