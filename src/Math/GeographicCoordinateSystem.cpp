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

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->datum.spheroid.srid = datum->spheroid.srid;
	this->datum.spheroid.ellipsoid = datum->spheroid.ellipsoid->Clone();
	this->datum.spheroid.name = Text::StrCopyNewC(datum->spheroid.name, datum->spheroid.nameLen);
	this->datum.spheroid.nameLen = datum->spheroid.nameLen;
	this->datum.srid = datum->srid;
	Double aRatio = Math::Unit::Angle::GetUnitRatio(datum->aunit);
	this->datum.name = Text::StrCopyNewC(datum->name, datum->nameLen);
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

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->datum.spheroid.srid = datum->spheroid.srid;
	this->datum.spheroid.ellipsoid = datum->spheroid.ellipsoid->Clone();
	this->datum.spheroid.name = Text::StrCopyNewC(datum->spheroid.name, datum->spheroid.nameLen);
	this->datum.spheroid.nameLen = datum->spheroid.nameLen;
	this->datum.srid = datum->srid;
	Double aRatio = Math::Unit::Angle::GetUnitRatio(datum->aunit);
	this->datum.name = Text::StrCopyNewC(datum->name, datum->nameLen);
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
	DEL_CLASS(this->datum.spheroid.ellipsoid);
}

Double Math::GeographicCoordinateSystem::CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit)
{
	return this->datum.spheroid.ellipsoid->CalSurfaceDistance(pos1.lat, pos1.lon, pos2.lat, pos2.lon, unit);
}

Double Math::GeographicCoordinateSystem::CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit)
{
	return this->datum.spheroid.ellipsoid->CalPLDistance(pl, unit);
}

Double Math::GeographicCoordinateSystem::CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit)
{
	return this->datum.spheroid.ellipsoid->CalPLDistance3D(pl, unit);
}

Math::CoordinateSystem *Math::GeographicCoordinateSystem::Clone()
{
	Math::CoordinateSystem *csys;
	NEW_CLASS(csys, Math::GeographicCoordinateSystem(this->sourceName, this->srid, this->csysName->ToCString(), &this->datum, this->primem, this->unit));
	return csys;
}

Math::CoordinateSystem::CoordinateSystemType Math::GeographicCoordinateSystem::GetCoordSysType()
{
	return Math::CoordinateSystem::CoordinateSystemType::Geographic;
}

Bool Math::GeographicCoordinateSystem::IsProjected()
{
	return false;
}

void Math::GeographicCoordinateSystem::ToString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("Geographic File Name: "));
	sb->Append(this->sourceName);
	sb->AppendC(UTF8STRC("\r\nSRID: "));
	sb->AppendU32(this->srid);
	sb->AppendC(UTF8STRC("\r\nGeographic Name: "));
	sb->Append(this->csysName);
	sb->AppendC(UTF8STRC("\r\nDatum Name: "));
	sb->AppendC(this->datum.name, this->datum.nameLen);
	sb->AppendC(UTF8STRC("\r\nRotate Center: "));
	Text::SBAppendF64(sb, this->datum.x0);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, this->datum.y0);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, this->datum.z0);
	sb->AppendC(UTF8STRC("\r\nShifting: "));
	Text::SBAppendF64(sb, this->datum.cX);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, this->datum.cY);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, this->datum.cZ);
	sb->AppendC(UTF8STRC("\r\nRotation: "));
	Text::SBAppendF64(sb, this->datum.xAngle);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, this->datum.yAngle);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, this->datum.zAngle);
	sb->AppendC(UTF8STRC("\r\nScale Factor: "));
	Text::SBAppendF64(sb, this->datum.scale);
	sb->AppendC(UTF8STRC("\r\nSemi-Major Axis: "));
	Text::SBAppendF64(sb, this->datum.spheroid.ellipsoid->GetSemiMajorAxis());
	sb->AppendC(UTF8STRC("\r\nInverse Flattening: "));
	Text::SBAppendF64(sb, this->datum.spheroid.ellipsoid->GetInverseFlattening());
}

Math::EarthEllipsoid *Math::GeographicCoordinateSystem::GetEllipsoid()
{
	return this->datum.spheroid.ellipsoid;
}

const UTF8Char *Math::GeographicCoordinateSystem::GetDatumName()
{
	return (const UTF8Char*)this->datum.name;
}

const Math::GeographicCoordinateSystem::DatumData1 *Math::GeographicCoordinateSystem::GetDatum()
{
	return &this->datum;
}

Math::CoordinateSystem::PrimemType Math::GeographicCoordinateSystem::GetPrimem()
{
	return this->primem;
}

Math::CoordinateSystem::UnitType Math::GeographicCoordinateSystem::GetUnit()
{
	return this->unit;
}

void Math::GeographicCoordinateSystem::ToCartesianCoord(Double lat, Double lon, Double h, Double *x, Double *y, Double *z)
{
	Double tmpX;
	Double tmpY;
	Double tmpZ;
	this->datum.spheroid.ellipsoid->ToCartesianCoord(lat, lon, h, &tmpX, &tmpY, &tmpZ);
	if (this->datum.scale == 0 && this->datum.xAngle == 0 && this->datum.yAngle == 0 && this->datum.zAngle == 0)
	{
		*x = tmpX + datum.cX;
		*y = tmpY + datum.cY;
		*z = tmpZ + datum.cZ;
	}
	else
	{
		tmpX -= this->datum.x0;
		tmpY -= this->datum.y0;
		tmpZ -= this->datum.z0;
		Double s = 1 + this->datum.scale * 0.000001;
		*x = s * (                tmpX - datum.zAngle * tmpY + datum.yAngle * tmpZ) + datum.cX + this->datum.x0;
		*y = s * ( datum.zAngle * tmpX +                tmpY - datum.xAngle * tmpZ) + datum.cY + this->datum.y0;
		*z = s * (-datum.yAngle * tmpX + datum.xAngle * tmpY +                tmpZ) + datum.cZ + this->datum.z0;
	}
}

void Math::GeographicCoordinateSystem::FromCartesianCoord(Double x, Double y, Double z, Double *lat, Double *lon, Double *h)
{
	Double tmpX;
	Double tmpY;
	Double tmpZ;
	if (this->datum.scale == 0 && this->datum.xAngle == 0 && this->datum.yAngle == 0 && this->datum.zAngle == 0)
	{
		tmpX = x - this->datum.cX;
		tmpY = y - this->datum.cY;
		tmpZ = z - this->datum.cZ;
	}
	else
	{
		x = x - this->datum.x0 - datum.cX;
		y = y - this->datum.y0 - datum.cY;
		z = z - this->datum.z0 - datum.cZ;
		Double s = 1 / (1 + this->datum.scale * 0.000001);
		tmpX = s * (                      x + this->datum.zAngle * y - this->datum.yAngle * z) + this->datum.x0;
		tmpY = s * (-this->datum.zAngle * x +                      y + this->datum.xAngle * z) + this->datum.y0;
		tmpZ = s * ( this->datum.yAngle * x - this->datum.xAngle * y +                      z) + this->datum.z0;
	}
	this->datum.spheroid.ellipsoid->FromCartesianCoord(tmpX, tmpY, tmpZ, lat, lon, h);
}
