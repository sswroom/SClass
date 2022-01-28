#ifndef _SM_MAP_TILEMAP
#define _SM_MAP_TILEMAP
#include "Data/ArrayList.h"
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
			PT_WGS84,
			PT_MERCATOR
		} ProjectionType;
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
			TT_GOOGLE
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
		virtual Bool GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY) = 0;
		virtual ProjectionType GetProjectionType() = 0;
		virtual UOSInt GetTileSize() = 0;

		virtual UOSInt GetImageIDs(UOSInt level, Double x1, Double y1, Double x2, Double y2, Data::ArrayList<Int64> *ids) = 0;
		virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly) = 0;
		virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId) = 0;
		virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it) = 0;
	};
}
#endif
