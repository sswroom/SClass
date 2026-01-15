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
			UIntOS tileWidth;
			UIntOS tileHeight;
			Data::ArrayListDbl tileLevels;

		public:
			ESRIMapServer(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Bool noResource);
			virtual ~ESRIMapServer();

			Bool IsError() const;
			Bool HasTile() const;
			void SetSRID(UInt32 srid);

			NN<Text::String> GetURL() const;
			Math::RectAreaDbl GetBounds() const;

			UIntOS TileGetLevelCount() const;
			Double TileGetLevelResolution(UIntOS level) const;
			UIntOS TileGetWidth() const;
			UIntOS TileGetHeight() const;
			Math::Coord2DDbl TileGetOrigin() const;
			UnsafeArray<UTF8Char> TileGetURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Int32 tileX, Int32 tileY) const;
			Bool TileGetURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Int32 tileX, Int32 tileY) const;
			Bool TileLoadToStream(NN<IO::Stream> stm, UIntOS level, Int32 tileX, Int32 tileY) const;
			Bool TileLoadToFile(Text::CStringNN fileName, UIntOS level, Int32 tileX, Int32 tileY) const;

			virtual NN<Text::String> GetName() const;
			virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
			virtual Math::RectAreaDbl GetInitBounds() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
			virtual Bool CanQuery() const;
			virtual Bool QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);
			virtual Optional<Media::ImageList> DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Optional<Text::StringBuilderUTF8> sbUrl);

			static Optional<Math::Geometry::Vector2D> ParseGeometry(UInt32 srid, NN<Text::String> geometryType, NN<Text::JSONBase> geometry);
		};
	}
}
#endif
