#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Text/MyString.h"

Math::CoordinateSystem::CoordinateSystem(NotNullPtr<Text::String> sourceName, UInt32 srid, Text::CString csysName) : IO::ParsedObject(sourceName)
{
	this->csysName = Text::String::New(csysName);
	this->srid = srid;
}

Math::CoordinateSystem::CoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName) : IO::ParsedObject(sourceName)
{
	this->csysName = Text::String::New(csysName);
	this->srid = srid;
}

Math::CoordinateSystem::~CoordinateSystem()
{
	this->csysName->Release();
}

IO::ParserType Math::CoordinateSystem::GetParserType() const
{
	return IO::ParserType::CoordinateSystem;
}

Bool Math::CoordinateSystem::Equals(Math::CoordinateSystem *csys) const
{
	if (this == csys)
		return true;
	Math::CoordinateSystem::CoordinateSystemType cst = this->GetCoordSysType();
	if (cst != csys->GetCoordSysType())
		return false;
	if (cst == Math::CoordinateSystem::CoordinateSystemType::Geographic)
	{
		Math::GeographicCoordinateSystem *gcs1 = (Math::GeographicCoordinateSystem*)this;
		Math::GeographicCoordinateSystem *gcs2 = (Math::GeographicCoordinateSystem*)csys;
		return gcs1->GetEllipsoid()->Equals(gcs2->GetEllipsoid());
	}
	else if (cst == Math::CoordinateSystem::CoordinateSystemType::PointMapping)
	{
		return false;
	}
	else
	{
		Math::ProjectedCoordinateSystem *pcs1 = (Math::ProjectedCoordinateSystem*)this;
		Math::ProjectedCoordinateSystem *pcs2 = (Math::ProjectedCoordinateSystem*)csys;
		return pcs1->SameProjection(pcs2);
	}
}

Math::Coord2DDbl Math::CoordinateSystem::Convert(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, Math::Coord2DDbl coord)
{
	Double x;
	Double y;
	ConvertXYZ(srcCoord, destCoord, coord.x, coord.y, 0, &x, &y, 0);
	return Math::Coord2DDbl(x, y);
}

void Math::CoordinateSystem::ConvertXYZ(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, Double srcX, Double srcY, Double srcZ, Double *destX, Double *destY, Double *destZ)
{
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord;
		pcs->ToGeographicCoordinateDeg(srcX, srcY, &srcX, &srcY);
		srcCoord = pcs->GetGeographicCoordinateSystem();
	}
	if (srcCoord->Equals(destCoord))
	{
		*destX = srcX;
		*destY = srcY;
		if (destZ)
			*destZ = srcZ;
		return;
	}
	((Math::GeographicCoordinateSystem*)srcCoord)->ToCartesianCoordDeg(srcY, srcX, srcZ, &srcX, &srcY, &srcZ);

	if (destCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)destCoord;
		Math::GeographicCoordinateSystem *gcs = pcs->GetGeographicCoordinateSystem();
		gcs->FromCartesianCoordRad(srcX, srcY, srcZ, &srcY, &srcX, &srcZ);
		pcs->FromGeographicCoordinateRad(srcX, srcY, destX, destY);
		if (destZ)
			*destZ = srcZ;
	}
	else
	{
		Math::GeographicCoordinateSystem *gcs = (Math::GeographicCoordinateSystem*)destCoord;;
		gcs->FromCartesianCoordDeg(srcX, srcY, srcZ, destY, destX, &srcZ);
		if (destZ)
			*destZ = srcZ;
	}
}

void Math::CoordinateSystem::ConvertXYArray(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints)
{
	UOSInt i;
	Bool srcRad = false;
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord;
		i = nPoints;
		while (i-- > 0)
		{
			pcs->ToGeographicCoordinateRad(srcArr[i].x, srcArr[i].y, &destArr[i].x, &destArr[i].y);
		}
		srcCoord = pcs->GetGeographicCoordinateSystem();
		srcArr = destArr;
		srcRad = true;
	}
	if (srcCoord->Equals(destCoord))
	{
		if (srcRad)
		{
			i = nPoints;
			while (i-- > 0)
			{
				destArr[i] = srcArr[i] * 180.0 / Math::PI;
			}
		}
		else if (srcArr != destArr)
		{
			MemCopyAC(destArr, srcArr, nPoints * sizeof(Math::Coord2DDbl));
		}
		return;
	}
	Double tmpZ;
	if (destCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)destCoord;
		Math::GeographicCoordinateSystem *gcs = pcs->GetGeographicCoordinateSystem();
		if (srcRad)
		{
			i = nPoints;
			while (i-- > 0)
			{
				((Math::GeographicCoordinateSystem*)srcCoord)->ToCartesianCoordRad(srcArr[i].GetLat(), srcArr[i].GetLon(), 0, &destArr[i].x, &destArr[i].y, &tmpZ);
				gcs->FromCartesianCoordRad(destArr[i].x, destArr[i].y, tmpZ, &destArr[i].GetLatRef(), &destArr[i].GetLonRef(), &tmpZ);
				pcs->FromGeographicCoordinateRad(destArr[i].x, destArr[i].y, &destArr[i].x, &destArr[i].y);
			}
		}
		else
		{
			i = nPoints;
			while (i-- > 0)
			{
				((Math::GeographicCoordinateSystem*)srcCoord)->ToCartesianCoordDeg(srcArr[i].GetLat(), srcArr[i].GetLon(), 0, &destArr[i].x, &destArr[i].y, &tmpZ);
				gcs->FromCartesianCoordRad(destArr[i].x, destArr[i].y, tmpZ, &destArr[i].GetLatRef(), &destArr[i].GetLonRef(), &tmpZ);
				pcs->FromGeographicCoordinateRad(destArr[i].x, destArr[i].y, &destArr[i].x, &destArr[i].y);
			}
		}
	}
	else
	{
		Math::GeographicCoordinateSystem *gcs = (Math::GeographicCoordinateSystem*)destCoord;;
		if (srcRad)
		{
			i = nPoints;
			while (i-- > 0)
			{
				((Math::GeographicCoordinateSystem*)srcCoord)->ToCartesianCoordRad(srcArr[i].GetLat(), srcArr[i].GetLon(), 0, &destArr[i].x, &destArr[i].y, &tmpZ);
				gcs->FromCartesianCoordDeg(destArr[i].x, destArr[i].y, tmpZ, &destArr[i].GetLatRef(), &destArr[i].GetLonRef(), &tmpZ);
			}
		}
		else
		{
			i = nPoints;
			while (i-- > 0)
			{
				((Math::GeographicCoordinateSystem*)srcCoord)->ToCartesianCoordDeg(srcArr[i].GetLat(), srcArr[i].GetLon(), 0, &destArr[i].x, &destArr[i].y, &tmpZ);
				gcs->FromCartesianCoordDeg(destArr[i].x, destArr[i].y, tmpZ, &destArr[i].GetLatRef(), &destArr[i].GetLonRef(), &tmpZ);
			}
		}
	}
}

void Math::CoordinateSystem::ConvertToCartesianCoord(Math::CoordinateSystem *srcCoord, Double srcX, Double srcY, Double srcZ, Double *destX, Double *destY, Double *destZ)
{
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord;
		pcs->ToGeographicCoordinateDeg(srcX, srcY, &srcX, &srcY);
		srcCoord = pcs->GetGeographicCoordinateSystem();
	}
	((Math::GeographicCoordinateSystem*)srcCoord)->ToCartesianCoordDeg(srcY, srcX, srcZ, destX, destY, destZ);
}

void Math::CoordinateSystem::DatumData1ToString(const DatumData1 *datum, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("Datum Name: "));
	sb->AppendC(datum->name, datum->nameLen);
	sb->AppendC(UTF8STRC("\r\nRotate Center: "));
	Text::SBAppendF64(sb, datum->x0);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, datum->y0);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, datum->z0);
	sb->AppendC(UTF8STRC("\r\nShifting: "));
	Text::SBAppendF64(sb, datum->cX);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, datum->cY);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, datum->cZ);
	sb->AppendC(UTF8STRC("\r\nRotation: "));
	Text::SBAppendF64(sb, datum->xAngle);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, datum->yAngle);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, datum->zAngle);
	sb->AppendC(UTF8STRC(" ("));
	sb->Append(Math::Unit::Angle::GetUnitName(datum->aunit));
	sb->AppendC(UTF8STRC(")\r\nScale Factor: "));
	Text::SBAppendF64(sb, datum->scale);
	sb->AppendC(UTF8STRC("\r\n"));
	datum->spheroid.ellipsoid->ToString(sb);
}

Text::CString Math::CoordinateSystem::CoordinateSystemTypeGetName(CoordinateSystemType csysType)
{
	switch (csysType)
	{
	case CoordinateSystemType::Geographic:
		return CSTR("Geographic");
	case CoordinateSystemType::MercatorProjected:
		return CSTR("Transverse Mercator");
	case CoordinateSystemType::Mercator1SPProjected:
		return CSTR("Mercator 1SP");
	case CoordinateSystemType::PointMapping:
		return CSTR("Point Mapping");
	case CoordinateSystemType::GausskrugerProjected:
		return CSTR("Gauss-Kruger");
	default:
		return CSTR("Unknown");
	}
}
