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
			NN<Text::String> url;
			NN<Text::String> name;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Math::RectAreaDbl bounds;
			Math::RectAreaDbl initBounds;
			NN<Math::CoordinateSystem> csys;
			Bool noResource;

			Bool supportMap;
			Bool supportTileMap;
			Bool supportQuery;
			Bool supportData;

			Math::Coord2DDbl tileOrigin;
			UOSInt tileWidth;
			UOSInt tileHeight;
			Data::ArrayListDbl tileLevels;

		public:
			ESRIMapServer(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Bool noResource);
			virtual ~ESRIMapServer();

			Bool IsError() const;
			Bool HasTile() const;
			void SetSRID(UInt32 srid);

			NN<Text::String> GetURL() const;
			Math::RectAreaDbl GetBounds() const;

			UOSInt TileGetLevelCount() const;
			Double TileGetLevelResolution(UOSInt level) const;
			UOSInt TileGetWidth() const;
			UOSInt TileGetHeight() const;
			Math::Coord2DDbl TileGetOrigin() const;
			UnsafeArray<UTF8Char> TileGetURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Int32 tileX, Int32 tileY) const;
			Bool TileGetURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Int32 tileX, Int32 tileY) const;
			Bool TileLoadToStream(IO::Stream *stm, UOSInt level, Int32 tileX, Int32 tileY) const;
			Bool TileLoadToFile(Text::CStringNN fileName, UOSInt level, Int32 tileX, Int32 tileY) const;

			virtual NN<Text::String> GetName() const;
			virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
			virtual Math::RectAreaDbl GetInitBounds() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
			virtual Bool CanQuery() const;
			virtual Bool QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);
			virtual Optional<Media::ImageList> DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Optional<Text::StringBuilderUTF8> sbUrl);

			static Optional<Math::Geometry::Vector2D> ParseGeometry(UInt32 srid, NN<Text::String> geometryType, NN<Text::JSONBase> geometry);
		};
	}
}
#endif
