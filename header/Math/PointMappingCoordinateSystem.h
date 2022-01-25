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
		PointMappingCoordinateSystem(Text::String *sourceName, UInt32 srid, const UTF8Char *csysName, Math::CoordinateSystem *baseCSys);
		PointMappingCoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *csysName, Math::CoordinateSystem *baseCSys);
		virtual ~PointMappingCoordinateSystem();

		void AddMappingPoint(Double mapX, Double mapY, Double baseX, Double baseY);
		void CalcBaseXY(Double mapX, Double mapY, Double *baseX, Double *baseY);

		virtual Double CalSurfaceDistanceXY(Double x1, Double y1, Double x2, Double y2, Math::Unit::Distance::DistanceUnit unit);
		virtual Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual CoordinateSystem *Clone();
		virtual CoordinateSystemType GetCoordSysType();
		virtual Bool IsProjected();
		virtual void ToString(Text::StringBuilderUTF8 *sb);

		virtual Bool Equals(CoordinateSystem *csys);
	};
}
#endif
