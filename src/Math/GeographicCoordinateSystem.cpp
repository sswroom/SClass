#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
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

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(NotNullPtr<Text::String> sourceName, UInt32 srid, Text::CString csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
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

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
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

Double Math::GeographicCoordinateSystem::CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const
{
	return this->datum.spheroid.ellipsoid->CalSurfaceDistance(pos1.GetLat(), pos1.GetLon(), pos2.GetLat(), pos2.GetLon(), unit);
}

Double Math::GeographicCoordinateSystem::CalPLDistance(NotNullPtr<Math::Geometry::Polyline> pl, Math::Unit::Distance::DistanceUnit unit) const
{
	return this->datum.spheroid.ellipsoid->CalPLDistance(pl, unit);
}

Double Math::GeographicCoordinateSystem::CalPLDistance3D(NotNullPtr<Math::Geometry::Polyline> pl, Math::Unit::Distance::DistanceUnit unit) const
{
	return this->datum.spheroid.ellipsoid->CalPLDistance3D(pl, unit);
}

NotNullPtr<Math::CoordinateSystem> Math::GeographicCoordinateSystem::Clone() const
{
	NotNullPtr<Math::CoordinateSystem> csys;
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

void Math::GeographicCoordinateSystem::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
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

Text::CString Math::GeographicCoordinateSystem::GetDatumName() const
{
	return Text::CString(this->datum.name, this->datum.nameLen);
}

NotNullPtr<const Math::GeographicCoordinateSystem::DatumData1> Math::GeographicCoordinateSystem::GetDatum() const
{
	return this->datum;
}

Math::CoordinateSystem::PrimemType Math::GeographicCoordinateSystem::GetPrimem() const
{
	return this->primem;
}

Math::CoordinateSystem::UnitType Math::GeographicCoordinateSystem::GetUnit() const
{
	return this->unit;
}

Math::Vector3 Math::GeographicCoordinateSystem::ToCartesianCoordRad(Math::Vector3 lonLatH) const
{
	Math::Vector3 tmpPos = this->datum.spheroid.ellipsoid->ToCartesianCoordRad(lonLatH);
	if (this->datum.scale == 0 && this->datum.xAngle == 0 && this->datum.yAngle == 0 && this->datum.zAngle == 0)
	{
		return Math::Vector3(
			tmpPos.val[0] + datum.cX,
			tmpPos.val[1] + datum.cY,
			tmpPos.val[2] + datum.cZ);
	}
	else
	{
		tmpPos.val[0] -= this->datum.x0;
		tmpPos.val[1] -= this->datum.y0;
		tmpPos.val[2] -= this->datum.z0;
		Double s = 1 + this->datum.scale * 0.000001;
		return Math::Vector3(
			s * (                tmpPos.val[0] - datum.zAngle * tmpPos.val[1] + datum.yAngle * tmpPos.val[2]) + datum.cX + this->datum.x0,
			s * ( datum.zAngle * tmpPos.val[0] +                tmpPos.val[1] - datum.xAngle * tmpPos.val[2]) + datum.cY + this->datum.y0,
			s * (-datum.yAngle * tmpPos.val[0] + datum.xAngle * tmpPos.val[1] +                tmpPos.val[2]) + datum.cZ + this->datum.z0);
	}
}

Math::Vector3 Math::GeographicCoordinateSystem::FromCartesianCoordRad(Math::Vector3 coord) const
{
	Math::Vector3 tmpPos;
	if (this->datum.scale == 0 && this->datum.xAngle == 0 && this->datum.yAngle == 0 && this->datum.zAngle == 0)
	{
		tmpPos = Math::Vector3(
			coord.val[0] - this->datum.cX,
			coord.val[1] - this->datum.cY,
			coord.val[2] - this->datum.cZ);
	}
	else
	{
		coord.val[0] = coord.val[0] - this->datum.x0 - datum.cX;
		coord.val[1] = coord.val[1] - this->datum.y0 - datum.cY;
		coord.val[2] = coord.val[2] - this->datum.z0 - datum.cZ;
		Double s = 1 / (1 + this->datum.scale * 0.000001);
		tmpPos = Math::Vector3(
			s * (                      coord.val[0] + this->datum.zAngle * coord.val[1] - this->datum.yAngle * coord.val[2]) + this->datum.x0,
			s * (-this->datum.zAngle * coord.val[0] +                      coord.val[1] + this->datum.xAngle * coord.val[2]) + this->datum.y0,
			s * ( this->datum.yAngle * coord.val[0] - this->datum.xAngle * coord.val[1] +                      coord.val[2]) + this->datum.z0);
	}
	return this->datum.spheroid.ellipsoid->FromCartesianCoordRad(tmpPos);
}
