#ifndef _SM_MAP_TILEMAP
#define _SM_MAP_TILEMAP
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListT.hpp"
#include "Math/CoordinateSystem.h"
#include "Math/Size2D.h"
#include "Math/Geometry/Vector2D.h"
#include "Media/ImageList.h"
#include "Parser/ParserList.h"
#include "Text/CString.h"

namespace Map
{
	class TileMap
	{
	public:
		typedef enum
		{
			IT_PNG,
			IT_JPG,
			IT_WEBP
		} ImageType;

		typedef enum
		{
			TT_OTHER,
			TT_OSM,
			TT_OSMLOCAL,
			TT_ESRI,
			TT_GOOGLE,
			TT_TMS,
			TT_WMTS,
			TT_CUSTOM,
			TT_BING,
			TT_TIFF
		} TileType;
	public:
		virtual ~TileMap() {};

		virtual Text::CStringNN GetName() const = 0;
		virtual Bool IsError() const = 0;
		virtual TileType GetTileType() const = 0;
		virtual UOSInt GetMinLevel() const = 0;
		virtual UOSInt GetMaxLevel() const = 0;
		virtual Double GetLevelScale(UOSInt level) const = 0;
		virtual UOSInt GetNearestLevel(Double scale) const = 0;
		virtual UOSInt GetConcurrentCount() const = 0;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const = 0;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const = 0;
		virtual Bool IsMercatorProj() const = 0;
		virtual UOSInt GetTileSize() const = 0;
		virtual ImageType GetImageType() const = 0;
		virtual Bool CanQuery() const { return false; };
		virtual Bool QueryInfos(Math::Coord2DDbl coord, UOSInt level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const { return false; };
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi) {};

		virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids) = 0;
		virtual Optional<Media::ImageList> LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly) = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId) = 0;
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId) = 0;
		virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it) = 0;

		virtual UOSInt GetScreenObjCnt() { return 0; };
		virtual Optional<Math::Geometry::Vector2D> CreateScreenObjVector(UOSInt index) { return nullptr; };
		virtual UnsafeArrayOpt<UTF8Char> GetScreenObjURL(UnsafeArray<UTF8Char> sbuff, UOSInt index) { return nullptr; };
		virtual Bool GetScreenObjURL(NN<Text::StringBuilderUTF8> sb, UOSInt index) {return false;};
	};
}
#endif
