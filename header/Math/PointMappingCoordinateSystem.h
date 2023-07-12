#ifndef _SM_MATH_POINTMAPPINGCOORDINATESYSTEM
#define _SM_MATH_POINTMAPPINGCOORDINATESYSTEM
#include "Math/CoordinateSystem.h"

namespace Math
{
	class PointMappingCoordinateSystem : public Math::CoordinateSystem
	{
	private:
		Math::CoordinateSystem *baseCSys;
		Data::ArrayList<Double*> *mappingList;

	public:
		PointMappingCoordinateSystem(NotNullPtr<Text::String> sourceName, UInt32 srid, Text::CString csysName, Math::CoordinateSystem *baseCSys);
		PointMappingCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName, Math::CoordinateSystem *baseCSys);
		virtual ~PointMappingCoordinateSystem();

		void AddMappingPoint(Double mapX, Double mapY, Double baseX, Double baseY);
		Math::Coord2DDbl CalcBaseXY(Math::Coord2DDbl mapPt) const;

		virtual Double CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance(Math::Geometry::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance3D(Math::Geometry::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual CoordinateSystem *Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;
		virtual Bool IsProjected() const;
		virtual void ToString(Text::StringBuilderUTF8 *sb) const;

		virtual Bool Equals(CoordinateSystem *csys) const;
	};
}
#endif
