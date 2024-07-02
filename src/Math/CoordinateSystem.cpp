#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/PointMappingCoordinateSystem.h"
#include "Math/ProjectedCoordinateSystem.h"
#include "Math/Geometry/CircularString.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LinearRing.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"

Math::CoordinateSystem::CoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName) : IO::ParsedObject(sourceName)
{
	this->csysName = Text::String::New(csysName);
	this->srid = srid;
}

Math::CoordinateSystem::CoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName) : IO::ParsedObject(sourceName)
{
	this->csysName = Text::String::New(csysName);
	this->srid = srid;
}

Math::CoordinateSystem::~CoordinateSystem()
{
	this->csysName->Release();
}

Double Math::CoordinateSystem::CalDistance(NN<Math::Geometry::Vector2D> vec, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const
{
	Double totalDist;
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
		return 0;
	case Math::Geometry::Vector2D::VectorType::LineString:
		return CalLineStringDistance(NN<Math::Geometry::LineString>::ConvertFrom(vec), include3D, unit);
	case Math::Geometry::Vector2D::VectorType::LinearRing:
		return CalLineStringDistance(NN<Math::Geometry::LinearRing>::ConvertFrom(vec), include3D, unit);
	case Math::Geometry::Vector2D::VectorType::Polygon:
	{
		totalDist = 0;
		Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = NN<Math::Geometry::Polygon>::ConvertFrom(vec)->Iterator();
		while (it.HasNext())
		{
			totalDist += CalLineStringDistance(it.Next(), include3D, unit);
		}
		return totalDist;
	}
	case Math::Geometry::Vector2D::VectorType::Polyline:
	{
		totalDist = 0;
		Data::ArrayIterator<NN<Math::Geometry::LineString>> it = NN<Math::Geometry::Polyline>::ConvertFrom(vec)->Iterator();
		while (it.HasNext())
		{
			totalDist += CalLineStringDistance(it.Next(), include3D, unit);
		}
		return totalDist;
	}
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	{
		totalDist = 0;
		Data::ArrayIterator<NN<Math::Geometry::Polygon>> it = NN<Math::Geometry::MultiPolygon>::ConvertFrom(vec)->Iterator();
		while (it.HasNext())
		{
			totalDist += CalDistance(it.Next(), include3D, unit);
		}
		return totalDist;
	}
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	{
		totalDist = 0;
		Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = NN<Math::Geometry::GeometryCollection>::ConvertFrom(vec)->Iterator();
		while (it.HasNext())
		{
			totalDist += CalDistance(it.Next(), include3D, unit);
		}
		return totalDist;
	}
	case Math::Geometry::Vector2D::VectorType::CircularString:
		return CalLineStringDistance(NN<Math::Geometry::CircularString>::ConvertFrom(vec), include3D, unit);
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	{
		totalDist = 0;
		Data::ArrayIterator<NN<Math::Geometry::LineString>> it = NN<Math::Geometry::CompoundCurve>::ConvertFrom(vec)->Iterator();
		while (it.HasNext())
		{
			totalDist += CalLineStringDistance(it.Next(), include3D, unit);
		}
		return totalDist;
	}
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	{
		totalDist = 0;
		Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = NN<Math::Geometry::CurvePolygon>::ConvertFrom(vec)->Iterator();
		while (it.HasNext())
		{
			totalDist += CalDistance(it.Next(), include3D, unit);
		}
		return totalDist;
	}
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	case Math::Geometry::Vector2D::VectorType::Unknown:
	default:
	case Math::Geometry::Vector2D::VectorType::Image:
	case Math::Geometry::Vector2D::VectorType::String:
		return 0;
	}
}

IO::ParserType Math::CoordinateSystem::GetParserType() const
{
	return IO::ParserType::CoordinateSystem;
}

Bool Math::CoordinateSystem::Equals(NN<const Math::CoordinateSystem> csys) const
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
		return pcs1->SameProjection(NN<const Math::ProjectedCoordinateSystem>::ConvertFrom(csys));
	}
}

Math::Coord2DDbl Math::CoordinateSystem::Convert(NN<const Math::CoordinateSystem> srcCoord, NN<const Math::CoordinateSystem> destCoord, Math::Coord2DDbl coord)
{
	return Convert3D(srcCoord, destCoord, Math::Vector3(coord, 0)).GetXY();
}

Math::Vector3 Math::CoordinateSystem::Convert3D(NN<const Math::CoordinateSystem> srcCoord, NN<const Math::CoordinateSystem> destCoord, Math::Vector3 srcPos)
{
	if (srcCoord->GetCoordSysType() == CoordinateSystemType::PointMapping)
	{
		NN<Math::PointMappingCoordinateSystem> pmcsys = NN<Math::PointMappingCoordinateSystem>::ConvertFrom(srcCoord);
		srcPos = Math::Vector3(pmcsys->ToBaseXY(srcPos.GetXY()), srcPos.GetZ());
		srcCoord = pmcsys->GetBaseCSys();
	}
	if (srcCoord->IsProjected())
	{
		NN<Math::ProjectedCoordinateSystem> pcs = NN<Math::ProjectedCoordinateSystem>::ConvertFrom(srcCoord);
		srcPos = Math::Vector3(pcs->ToGeographicCoordinateDeg(srcPos.GetXY()), srcPos.val[2]);
		srcCoord = pcs->GetGeographicCoordinateSystem();
	}
	if (srcCoord->Equals(destCoord))
	{
		return srcPos;
	}
	srcPos = NN<Math::GeographicCoordinateSystem>::ConvertFrom(srcCoord)->ToCartesianCoordDeg(srcPos);

	NN<const Math::CoordinateSystem> destCoordRAW = destCoord;
	if (destCoord->GetCoordSysType() == CoordinateSystemType::PointMapping)
	{
		destCoord = NN<Math::PointMappingCoordinateSystem>::ConvertFrom(destCoord)->GetBaseCSys();
	}
	if (destCoord->IsProjected())
	{
		NN<Math::ProjectedCoordinateSystem> pcs = NN<Math::ProjectedCoordinateSystem>::ConvertFrom(destCoord);
		NN<Math::GeographicCoordinateSystem> gcs = pcs->GetGeographicCoordinateSystem();
		srcPos = gcs->FromCartesianCoordRad(srcPos);
		srcPos = Math::Vector3(pcs->FromGeographicCoordinateRad(srcPos.GetXY()), srcPos.GetZ());
	}
	else
	{
		srcPos = NN<Math::GeographicCoordinateSystem>::ConvertFrom(destCoord)->FromCartesianCoordDeg(srcPos);
	}
	if (destCoordRAW->GetCoordSysType() == CoordinateSystemType::PointMapping)
	{
		return Math::Vector3(NN<Math::PointMappingCoordinateSystem>::ConvertFrom(destCoord)->ToMapXY(srcPos.GetXY()), srcPos.GetZ());
	}
	else
	{
		return srcPos;
	}
}

void Math::CoordinateSystem::ConvertArray(NN<const Math::CoordinateSystem> srcCoord, NN<const Math::CoordinateSystem> destCoord, UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UOSInt nPoints)
{
	UOSInt i;
	Bool srcRad = false;
	if (srcCoord->IsProjected())
	{
		NN<Math::ProjectedCoordinateSystem> pcs = NN<Math::ProjectedCoordinateSystem>::ConvertFrom(srcCoord);
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
			MemCopyAC(destArr.Ptr(), srcArr.Ptr(), nPoints * sizeof(Math::Coord2DDbl));
		}
		return;
	}
	Math::Vector3 tmpPos;
	if (destCoord->IsProjected())
	{
		NN<Math::ProjectedCoordinateSystem> pcs = NN<Math::ProjectedCoordinateSystem>::ConvertFrom(destCoord);
		NN<Math::GeographicCoordinateSystem> gcs = pcs->GetGeographicCoordinateSystem();
		if (srcRad)
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = NN<Math::GeographicCoordinateSystem>::ConvertFrom(srcCoord)->ToCartesianCoordRad(Math::Vector3(srcArr[i], 0));
				tmpPos = gcs->FromCartesianCoordRad(tmpPos);
				destArr[i] = pcs->FromGeographicCoordinateRad(tmpPos.GetXY());
			}
		}
		else
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = NN<Math::GeographicCoordinateSystem>::ConvertFrom(srcCoord)->ToCartesianCoordDeg(Math::Vector3(srcArr[i], 0));
				tmpPos = gcs->FromCartesianCoordRad(tmpPos);
				destArr[i] = pcs->FromGeographicCoordinateRad(tmpPos.GetXY());
			}
		}
	}
	else
	{
		NN<Math::GeographicCoordinateSystem> gcs = NN<Math::GeographicCoordinateSystem>::ConvertFrom(destCoord);
		if (srcRad)
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = NN<Math::GeographicCoordinateSystem>::ConvertFrom(srcCoord)->ToCartesianCoordRad(Math::Vector3(srcArr[i], 0));;
				destArr[i] = gcs->FromCartesianCoordDeg(tmpPos).GetXY();
			}
		}
		else
		{
			i = nPoints;
			while (i-- > 0)
			{
				tmpPos = NN<Math::GeographicCoordinateSystem>::ConvertFrom(srcCoord)->ToCartesianCoordDeg(Math::Vector3(srcArr[i], 0));
				destArr[i] = gcs->FromCartesianCoordDeg(tmpPos).GetXY();
			}
		}
	}
}

Math::Vector3 Math::CoordinateSystem::ConvertToCartesianCoord(NN<const Math::CoordinateSystem> srcCoord, Math::Vector3 srcPos)
{
	if (srcCoord->IsProjected())
	{
		Math::ProjectedCoordinateSystem *pcs = (Math::ProjectedCoordinateSystem*)srcCoord.Ptr();
		srcPos = Math::Vector3(pcs->ToGeographicCoordinateDeg(srcPos.GetXY()), srcPos.GetZ());
		srcCoord = pcs->GetGeographicCoordinateSystem();
	}
	return ((Math::GeographicCoordinateSystem*)srcCoord.Ptr())->ToCartesianCoordDeg(srcPos);
}

void Math::CoordinateSystem::DatumData1ToString(NN<const DatumData1> datum, NN<Text::StringBuilderUTF8> sb)
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

Text::CStringNN Math::CoordinateSystem::CoordinateSystemTypeGetName(CoordinateSystemType csysType)
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
