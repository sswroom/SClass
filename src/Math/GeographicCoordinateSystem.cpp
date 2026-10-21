#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Math/Math_C.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

/*
China Geodetic Coordinate System 2000

Semi-major axis: a=6,378,137.0meters 
Flattening     : f=1/298.257222101 
Earth�fs gravitational constant: GM=3.986004418 * 10^14 m^3/s^2
Angular velocity of the Earth: ��=7.292115 * 10^-5 rad/s
*/

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->datum.spheroid.srid = datum->spheroid.srid;
	this->datum.spheroid.ellipsoid = datum->spheroid.ellipsoid->Clone();
	this->datum.spheroid.name = Text::StrCopyNewC(datum->spheroid.name, datum->spheroid.nameLen).Ptr();
	this->datum.spheroid.nameLen = datum->spheroid.nameLen;
	this->datum.srid = datum->srid;
	Double aRatio = Math::Unit::Angle::GetUnitRatio(datum->aunit);
	this->datum.name = Text::StrCopyNewC(datum->name, datum->nameLen).Ptr();
	this->datum.nameLen = datum->nameLen;
	this->datum.x0 = datum->x0;
	this->datum.y0 = datum->y0;
	this->datum.z0 = datum->z0;
	this->datum.cX = datum->cX;
	this->datum.cY = datum->cY;
	this->datum.cZ = datum->cZ;
	this->datum.xAngle = datum->xAngle * aRatio;
	this->datum.yAngle = datum->yAngle * aRatio;
	this->datum.zAngle = datum->zAngle * aRatio;
	this->datum.scale = datum->scale;
	this->datum.aunit = Math::Unit::Angle::AU_RADIAN;
	this->primem = primem;
	this->unit = unit;
}

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->datum.spheroid.srid = datum->spheroid.srid;
	this->datum.spheroid.ellipsoid = datum->spheroid.ellipsoid->Clone();
	this->datum.spheroid.name = Text::StrCopyNewC(datum->spheroid.name, datum->spheroid.nameLen).Ptr();
	this->datum.spheroid.nameLen = datum->spheroid.nameLen;
	this->datum.srid = datum->srid;
	Double aRatio = Math::Unit::Angle::GetUnitRatio(datum->aunit);
	this->datum.name = Text::StrCopyNewC(datum->name, datum->nameLen).Ptr();
	this->datum.nameLen = datum->nameLen;
	this->datum.x0 = datum->x0;
	this->datum.y0 = datum->y0;
	this->datum.z0 = datum->z0;
	this->datum.cX = datum->cX;
	this->datum.cY = datum->cY;
	this->datum.cZ = datum->cZ;
	this->datum.xAngle = datum->xAngle * aRatio;
	this->datum.yAngle = datum->yAngle * aRatio;
	this->datum.zAngle = datum->zAngle * aRatio;
	this->datum.scale = datum->scale;
	this->datum.aunit = Math::Unit::Angle::AU_RADIAN;
	this->primem = primem;
	this->unit = unit;
}

Math::GeographicCoordinateSystem::~GeographicCoordinateSystem()
{
	Text::StrDelNew(this->datum.name);
	Text::StrDelNew(this->datum.spheroid.name);
	this->datum.spheroid.ellipsoid.Delete();
}

Double Math::GeographicCoordinateSystem::CalSurfaceDistance(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const
{
	return this->datum.spheroid.ellipsoid->CalSurfaceDistance(pos1.GetLat(), pos1.GetLon(), pos2.GetLat(), pos2.GetLon(), unit);
}

Double Math::GeographicCoordinateSystem::CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const
{
	return this->datum.spheroid.ellipsoid->CalLineStringDistance(lineString, include3D, unit);
}

NN<Math::CoordinateSystem> Math::GeographicCoordinateSystem::Clone() const
{
	NN<Math::CoordinateSystem> csys;
	NEW_CLASSNN(csys, Math::GeographicCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), &this->datum, this->primem, this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::GeographicCoordinateSystem::GetCoordSysType() const
{
	return Math::CoordinateSystem::CoordinateSystemType::Geographic;
}

Bool Math::GeographicCoordinateSystem::IsProjected() const
{
	return false;
}

void Math::GeographicCoordinateSystem::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Geographic File Name: "));
	sb->Append(this->sourceName);
	sb->AppendC(UTF8STRC("\r\nGeographic SRID: "));
	sb->AppendU32(this->srid);
	sb->AppendC(UTF8STRC("\r\nGeographic Name: "));
	sb->Append(this->csysName);
	sb->AppendC(UTF8STRC("\r\n"));
	DatumData1ToString(this->datum, sb);
}

Math::Vector3D Math::GeographicCoordinateSystem::ToCartesianCoordRad(Math::Vector3D lonLatH) const
{
	Math::Vector3D tmpPos = this->datum.spheroid.ellipsoid->ToCartesianCoordRad(lonLatH);
	if (this->datum.scale == 0 && this->datum.xAngle == 0 && this->datum.yAngle == 0 && this->datum.zAngle == 0)
	{
		return Math::Vector3D(
			tmpPos.xy.x + datum.cX,
			tmpPos.xy.y + datum.cY,
			tmpPos.zn.x + datum.cZ);
	}
	else
	{
		tmpPos.xy.x -= this->datum.x0;
		tmpPos.xy.y -= this->datum.y0;
		tmpPos.zn.x -= this->datum.z0;
		Double s = 1 + this->datum.scale * 0.000001;
		return Math::Vector3D(
			s * (                tmpPos.xy.x - datum.zAngle * tmpPos.xy.y + datum.yAngle * tmpPos.zn.x) + datum.cX + this->datum.x0,
			s * ( datum.zAngle * tmpPos.xy.x +                tmpPos.xy.y - datum.xAngle * tmpPos.zn.x) + datum.cY + this->datum.y0,
			s * (-datum.yAngle * tmpPos.xy.x + datum.xAngle * tmpPos.xy.y +                tmpPos.zn.x) + datum.cZ + this->datum.z0);
	}
}

Math::Vector3D Math::GeographicCoordinateSystem::FromCartesianCoordRad(Math::Vector3D coord) const
{
	Math::Vector3D tmpPos;
	if (this->datum.scale == 0 && this->datum.xAngle == 0 && this->datum.yAngle == 0 && this->datum.zAngle == 0)
	{
		tmpPos = Math::Vector3D(
			coord.xy.x - this->datum.cX,
			coord.xy.y - this->datum.cY,
			coord.zn.x - this->datum.cZ);
	}
	else
	{
		coord.xy.x = coord.xy.x - this->datum.x0 - datum.cX;
		coord.xy.y = coord.xy.y - this->datum.y0 - datum.cY;
		coord.zn.x = coord.zn.x - this->datum.z0 - datum.cZ;
		Double s = 1 / (1 + this->datum.scale * 0.000001);
		tmpPos = Math::Vector3D(
			s * (                      coord.xy.x + this->datum.zAngle * coord.xy.y - this->datum.yAngle * coord.zn.x) + this->datum.x0,
			s * (-this->datum.zAngle * coord.xy.x +                      coord.xy.y + this->datum.xAngle * coord.zn.x) + this->datum.y0,
			s * ( this->datum.yAngle * coord.xy.x - this->datum.xAngle * coord.xy.y +                      coord.zn.x) + this->datum.z0);
	}
	return this->datum.spheroid.ellipsoid->FromCartesianCoordRad(tmpPos);
}
