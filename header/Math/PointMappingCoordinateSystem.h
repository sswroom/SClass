#ifndef _SM_MATH_POINTMAPPINGCOORDINATESYSTEM
#define _SM_MATH_POINTMAPPINGCOORDINATESYSTEM
#include "Data/ArrayList.h"
#include "Math/CoordinateSystem.h"

namespace Math
{
	class PointMappingCoordinateSystem : public Math::CoordinateSystem
	{
	private:
		NotNullPtr<Math::CoordinateSystem> baseCSys;
		Data::ArrayList<Double*> mappingList;

	public:
		PointMappingCoordinateSystem(NotNullPtr<Text::String> sourceName, UInt32 srid, Text::CString csysName, NotNullPtr<Math::CoordinateSystem> baseCSys);
		PointMappingCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString csysName, NotNullPtr<Math::CoordinateSystem> baseCSys);
		virtual ~PointMappingCoordinateSystem();

		void AddMappingPoint(Double mapX, Double mapY, Double baseX, Double baseY);
		Math::Coord2DDbl CalcBaseXY(Math::Coord2DDbl mapPt) const;

		virtual Double CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalLineStringDistance(NotNullPtr<Math::Geometry::LineString> lineString, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalLineStringDistance3D(NotNullPtr<Math::Geometry::LineString> lineString, Math::Unit::Distance::DistanceUnit unit) const;
		virtual NotNullPtr<CoordinateSystem> Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;
		virtual Bool IsProjected() const;
		virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;

		virtual Bool Equals(NotNullPtr<const CoordinateSystem> csys) const;
	};
}
#endif
