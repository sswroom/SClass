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

Math::CoordinateSystem::CoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString csysName) : IO::ParsedObject(sourceName)
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

Bool Math::CoordinateSystem::Equals(NotNullPtr<const Math::CoordinateSystem> csys) const
{
	if (this == csys.Ptr())
		return true;
	Math::CoordinateSystem::CoordinateSystemType cst = this->GetCoordSysType();
	if (cst != csys->GetCoordSysType())
		return false;
	if (cst == Math::CoordinateSystem::CoordinateSystemType::Geographic)
	{
		Math::GeographicCoordinateSystem *gcs1 = (Math::GeographicCoordinateSystem*)this;
		Math::GeographicCoordinateSystem *gcs2 = (Math::GeographicCoordinateSystem*)csys.Ptr();
		return gcs1->GetEllipsoid()->Equals(gcs2->GetEllipsoid());
	}
	else if (cst == Math::CoordinateSystem::CoordinateSystemType::PointMapping)
	{
		return false;
	}
	else
	{
		Math::ProjectedCoordinateSystem *pcs1 = (Math::ProjectedCoordinateSystem*)this;
		return pcs1->SameProjection(NotNullPtr<const Math::ProjectedCoordinateSystem>::ConvertFrom(csys));
	}
}

Math::Coord2DDbl Math::CoordinateSystem::Convert(NotNullPtr<const Math::CoordinateSystem> srcCoord, NotNullPtr<const Math::CoordinateSystem> destCoord, Math::Coord2DDbl coord)
{
	return ConvertXYZ(srcCoord, destCoord, Math::Vector3(coord, 0)).GetXY();
}

Math::Vector3 Math::CoordinateSystem::ConvertXYZ(NotNullPtr<const Math::CoordinateSystem> srcCoord, NotNullPtr<const Math::CoordinateSystem> destCoord, Math::Vector3 srcPos)
{
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord.Ptr();
		srcPos = Math::Vector3(pcs->ToGeographicCoordinateDeg(srcPos.GetXY()), srcPos.val[2]);
		srcCoord = pcs->GetGeographicCoordinateSystem();
	}
	if (srcCoord->Equals(destCoord))
	{
		return srcPos;
	}
	srcPos = ((Math::GeographicCoordinateSystem*)srcCoord.Ptr())->ToCartesianCoordDeg(srcPos);

	if (destCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)destCoord.Ptr();
		NotNullPtr<Math::GeographicCoordinateSystem> gcs = pcs->GetGeographicCoordinateSystem();
		srcPos = gcs->FromCartesianCoordRad(srcPos);
		return Math::Vector3(pcs->FromGeographicCoordinateRad(srcPos.GetXY()), srcPos.GetZ());
	}
	else
	{
		Math::GeographicCoordinateSystem *gcs = (Math::GeographicCoordinateSystem*)destCoord.Ptr();
		return gcs->FromCartesianCoordDeg(srcPos);
	}
}

void Math::CoordinateSystem::ConvertXYArray(NotNullPtr<const Math::CoordinateSystem> srcCoord, NotNullPtr<const Math::CoordinateSystem> destCoord, const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints)
{
	UOSInt i;
	Bool srcRad = false;
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord.Ptr();
		i = nPoints;
		while (i-- > 0)
		{
			destArr[i] = pcs->ToGeographicCoordinateRad(srcArr[i]);
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
	Math::Vector3 tmpPos;
	if (destCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)destCoord.Ptr();
		NotNullPtr<Math::GeographicCoordinateSystem> gcs = pcs->GetGeographicCoordinateSystem();
		if (srcRad)
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = ((Math::GeographicCoordinateSystem*)srcCoord.Ptr())->ToCartesianCoordRad(Math::Vector3(srcArr[i], 0));
				tmpPos = gcs->FromCartesianCoordRad(tmpPos);
				destArr[i] = pcs->FromGeographicCoordinateRad(tmpPos.GetXY());
			}
		}
		else
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = ((Math::GeographicCoordinateSystem*)srcCoord.Ptr())->ToCartesianCoordDeg(Math::Vector3(srcArr[i], 0));
				tmpPos = gcs->FromCartesianCoordRad(tmpPos);
				destArr[i] = pcs->FromGeographicCoordinateRad(tmpPos.GetXY());
			}
		}
	}
	else
	{
		Math::GeographicCoordinateSystem *gcs = (Math::GeographicCoordinateSystem*)destCoord.Ptr();
		if (srcRad)
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = ((Math::GeographicCoordinateSystem*)srcCoord.Ptr())->ToCartesianCoordRad(Math::Vector3(srcArr[i], 0));;
				destArr[i] = gcs->FromCartesianCoordDeg(tmpPos).GetXY();
			}
		}
		else
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = ((Math::GeographicCoordinateSystem*)srcCoord.Ptr())->ToCartesianCoordDeg(Math::Vector3(srcArr[i], 0));
				destArr[i] = gcs->FromCartesianCoordDeg(tmpPos).GetXY();
			}
		}
	}
}

Math::Vector3 Math::CoordinateSystem::ConvertToCartesianCoord(NotNullPtr<const Math::CoordinateSystem> srcCoord, Math::Vector3 srcPos)
{
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord.Ptr();
		srcPos = Math::Vector3(pcs->ToGeographicCoordinateDeg(srcPos.GetXY()), srcPos.GetZ());
		srcCoord = pcs->GetGeographicCoordinateSystem();
	}
	return ((Math::GeographicCoordinateSystem*)srcCoord.Ptr())->ToCartesianCoordDeg(srcPos);
}

void Math::CoordinateSystem::DatumData1ToString(NotNullPtr<const DatumData1> datum, NotNullPtr<Text::StringBuilderUTF8> sb)
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
