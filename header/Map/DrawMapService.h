#ifndef _SM_MAP_DRAWMAPSERVICE
#define _SM_MAP_DRAWMAPSERVICE
#include "Math/CoordinateSystem.h"
#include "Math/RectAreaDbl.h"
#include "Math/Geometry/Vector2D.h"
#include "Media/ImageList.h"
#include "Text/StringBuilderUTF8.h"

namespace Map
{
	class DrawMapService
	{
	public:
		virtual ~DrawMapService() {};

		virtual Text::String *GetName() const = 0;
		virtual Math::CoordinateSystem *GetCoordinateSystem() const = 0;
		virtual Math::RectAreaDbl GetInitBounds() const = 0;
		virtual Bool GetBounds(Math::RectAreaDbl *bounds) const = 0;
		virtual Bool CanQuery() const = 0;
		virtual Math::Geometry::Vector2D *QueryInfo(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList) = 0;
		virtual Media::ImageList *DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Text::StringBuilderUTF8 *sbUrl) = 0;
	};
}
#endif