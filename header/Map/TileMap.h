#ifndef _SM_MAP_TILEMAP
#define _SM_MAP_TILEMAP
#include "Data/ArrayList.h"
#include "Math/CoordinateSystem.h"
#include "Math/Size2D.h"
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
			IT_JPG
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
			TT_CUSTOM
		} TileType;
	public:
		virtual ~TileMap() {};

		virtual Text::CString GetName() = 0;
		virtual Bool IsError() = 0;
		virtual TileType GetTileType() = 0;
		virtual UOSInt GetLevelCount() = 0;
		virtual Double GetLevelScale(UOSInt level) = 0;
		virtual UOSInt GetNearestLevel(Double scale) = 0;
		virtual UOSInt GetConcurrentCount() = 0;
		virtual Bool GetBounds(Math::RectAreaDbl *bounds) = 0;
		virtual Math::CoordinateSystem *GetCoordinateSystem() = 0;
		virtual Bool IsMercatorProj() = 0;
		virtual UOSInt GetTileSize() = 0;
		virtual Bool CanQuery() const { return false; };
		virtual Bool QueryInfos(Math::Coord2DDbl coord, UOSInt level, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList) const { return false; };
		virtual void SetDispSize(Math::Size2D<Double> size, Double dpi) {};

		virtual UOSInt GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids) = 0;
		virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly) = 0;
		virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId) = 0;
		virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Math::RectAreaDbl *bounds, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it) = 0;
	};
}
#endif
