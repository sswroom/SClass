#ifndef _SM_MATH_POINTMAPPINGCOORDINATESYSTEM
#define _SM_MATH_POINTMAPPINGCOORDINATESYSTEM
#include "Data/ArrayList.hpp"
#include "Math/CoordinateSystem.h"

namespace Math
{
	class PointMappingCoordinateSystem : public Math::CoordinateSystem
	{
	private:
		NN<Math::CoordinateSystem> baseCSys;
		Data::ArrayList<Double*> mappingList;

	public:
		PointMappingCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName, NN<Math::CoordinateSystem> baseCSys);
		PointMappingCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName, NN<Math::CoordinateSystem> baseCSys);
		virtual ~PointMappingCoordinateSystem();

		void AddMappingPoint(Double mapX, Double mapY, Double baseX, Double baseY);
		Math::Coord2DDbl ToBaseXY(Math::Coord2DDbl mapPt) const;
		Math::Coord2DDbl ToMapXY(Math::Coord2DDbl basePt) const;
		NN<Math::CoordinateSystem> GetBaseCSys() const;

		virtual Double CalSurfaceDistance(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const;
		virtual NN<CoordinateSystem> Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;
		virtual Bool IsProjected() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		virtual Bool Equals(NN<const CoordinateSystem> csys) const;
	};
}
#endif
