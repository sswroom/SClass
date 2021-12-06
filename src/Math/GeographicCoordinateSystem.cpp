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

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(Text::String *sourceName, UInt32 srid, const UTF8Char *csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->csysName = Text::StrCopyNew(csysName);
	this->datum.spheroid.srid = datum->spheroid.srid;
	this->datum.spheroid.ellipsoid = datum->spheroid.ellipsoid->Clone();
	this->datum.spheroid.name = Text::StrCopyNew(datum->spheroid.name);
	this->datum.srid = datum->srid;
	Double aRatio = Math::Unit::Angle::GetUnitRatio(datum->aunit);
	this->datum.name = (const Char*)Text::StrCopyNew(datum->name);
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

Math::GeographicCoordinateSystem::GeographicCoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *csysName, const DatumData1 *datum, PrimemType primem, UnitType unit) : Math::CoordinateSystem(sourceName, srid, csysName)
{
	this->csysName = Text::StrCopyNew(csysName);
	this->datum.spheroid.srid = datum->spheroid.srid;
	this->datum.spheroid.ellipsoid = datum->spheroid.ellipsoid->Clone();
	this->datum.spheroid.name = Text::StrCopyNew(datum->spheroid.name);
	this->datum.srid = datum->srid;
	Double aRatio = Math::Unit::Angle::GetUnitRatio(datum->aunit);
	this->datum.name = (const Char*)Text::StrCopyNew(datum->name);
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
	Text::StrDelNew(this->csysName);
	Text::StrDelNew(this->datum.name);
	Text::StrDelNew(this->datum.spheroid.name);
	DEL_CLASS(this->datum.spheroid.ellipsoid);
}

Double Math::GeographicCoordinateSystem::CalSurfaceDistanceXY(Double x1, Double y1, Double x2, Double y2, Math::Unit::Distance::DistanceUnit unit)
{
	return this->datum.spheroid.ellipsoid->CalSurfaceDistance(y1, x1, y2, x2, unit);
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
	NEW_CLASS(csys, Math::GeographicCoordinateSystem(this->sourceName, this->srid, this->csysName, &this->datum, this->primem, this->unit));
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

void Math::GeographicCoordinateSystem::ToString(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"Geographic File Name: ");
	sb->Append(this->sourceName);
	sb->Append((const UTF8Char*)"\r\nSRID: ");
	sb->AppendU32(this->srid);
	sb->Append((const UTF8Char*)"\r\nGeographic Name: ");
	sb->Append(this->csysName);
	sb->Append((const UTF8Char*)"\r\nDatum Name: ");
	sb->Append((const UTF8Char*)this->datum.name);
	sb->Append((const UTF8Char*)"\r\nRotate Center: ");
	Text::SBAppendF64(sb, this->datum.x0);
	sb->Append((const UTF8Char*)", ");
	Text::SBAppendF64(sb, this->datum.y0);
	sb->Append((const UTF8Char*)", ");
	Text::SBAppendF64(sb, this->datum.z0);
	sb->Append((const UTF8Char*)"\r\nShifting: ");
	Text::SBAppendF64(sb, this->datum.cX);
	sb->Append((const UTF8Char*)", ");
	Text::SBAppendF64(sb, this->datum.cY);
	sb->Append((const UTF8Char*)", ");
	Text::SBAppendF64(sb, this->datum.cZ);
	sb->Append((const UTF8Char*)"\r\nRotation: ");
	Text::SBAppendF64(sb, this->datum.xAngle);
	sb->Append((const UTF8Char*)", ");
	Text::SBAppendF64(sb, this->datum.yAngle);
	sb->Append((const UTF8Char*)", ");
	Text::SBAppendF64(sb, this->datum.zAngle);
	sb->Append((const UTF8Char*)"\r\nScale Factor: ");
	Text::SBAppendF64(sb, this->datum.scale);
	sb->Append((const UTF8Char*)"\r\nSemi-Major Axis: ");
	Text::SBAppendF64(sb, this->datum.spheroid.ellipsoid->GetSemiMajorAxis());
	sb->Append((const UTF8Char*)"\r\nInverse Flattening: ");
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
