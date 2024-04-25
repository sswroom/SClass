#ifndef _SM_MAP_DRAWMAPSERVICE
#define _SM_MAP_DRAWMAPSERVICE
#include "Data/ArrayListNN.h"
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

		virtual NN<Text::String> GetName() const = 0;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const = 0;
		virtual Math::RectAreaDbl GetInitBounds() const = 0;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const = 0;
		virtual Bool CanQuery() const = 0;
		virtual Bool QueryInfos(Math::Coord2DDbl coord, Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) = 0;
		virtual Optional<Media::ImageList> DrawMap(Math::RectAreaDbl bounds, UInt32 width, UInt32 height, Double dpi, Optional<Text::StringBuilderUTF8> sbUrl) = 0;
	};
}
#endif
