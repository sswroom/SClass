#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/PointM.h"
#include "Math/Geometry/PointZM.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::WKTWriter::SetLastError(Text::CString lastError)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(lastError).Ptr();
}

void Math::WKTWriter::AppendLineString(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::LineString> pl, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	UOSInt nPoint;
	Math::Coord2DDbl *pointList = pl->GetPointList(nPoint);
	Double *zArr = pl->GetZList(nPoint);
	Double *mArr = pl->GetMList(nPoint);
	UOSInt i;
	if (zArr || mArr)
	{
		if (zArr && mArr)
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				if (reverseAxis)
				{
					sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
				}
				else
				{
					sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
				}
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(zArr[i], &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(mArr[i], &Text::DoubleStyleC);
				i++;
			}
		}
		else if (mArr)
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				if (reverseAxis)
				{
					sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
				}
				else
				{
					sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
				}
				sb->AppendC(UTF8STRC(" NaN "));
				sb->AppendDouble(mArr[i], &Text::DoubleStyleC);
				i++;
			}
		}
		else
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				if (reverseAxis)
				{
					sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
				}
				else
				{
					sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
				}
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(zArr[i], &Text::DoubleStyleC);
				i++;
			}
		}
	}
	else
	{
		if (reverseAxis)
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
				i++;
			}
		}
		else
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
 				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
				i++;
			}
		}
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolygon(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::Polygon> pg, Bool reverseAxis)
{
	Data::ArrayIterator<NotNullPtr<Math::Geometry::LinearRing>> it = pg->Iterator();
	Bool found = false;
	sb->AppendUTF8Char('(');
	while (it.HasNext())
	{
		if (found)
			sb->AppendUTF8Char(',');
		AppendLineString(sb, it.Next(), reverseAxis);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolyline(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::Polyline> pl, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	Data::ArrayIterator<NotNullPtr<Math::Geometry::LineString>> it = pl->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		AppendLineString(sb, it.Next(), reverseAxis);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendCompoundCurve(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::CompoundCurve> cc, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	NotNullPtr<Math::Geometry::LineString> pl;
	Data::ArrayIterator<NotNullPtr<Math::Geometry::LineString>> it = cc->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		pl = it.Next();
		if (pl->GetVectorType() == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			sb->AppendC(UTF8STRC("CIRCULARSTRING"));
		}
		AppendLineString(sb, pl, reverseAxis);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendCurvePolygon(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::CurvePolygon> cpg, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	NotNullPtr<Math::Geometry::Vector2D> geometry;
	Data::ArrayIterator<NotNullPtr<Math::Geometry::Vector2D>> it = cpg->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		geometry = it.Next();
		Math::Geometry::Vector2D::VectorType t = geometry->GetVectorType();
		if (t == Math::Geometry::Vector2D::VectorType::LineString)
		{
			AppendLineString(sb, NotNullPtr<Math::Geometry::LineString>::ConvertFrom(geometry), reverseAxis);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			sb->AppendC(UTF8STRC("CIRCULARSTRING"));
			AppendLineString(sb, NotNullPtr<Math::Geometry::LineString>::ConvertFrom(geometry), reverseAxis);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
			AppendCompoundCurve(sb, NotNullPtr<Math::Geometry::CompoundCurve>::ConvertFrom(geometry), reverseAxis);
		}
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendMultiPolygon(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::MultiPolygon> mpg, Bool reverseAxis)
{
	Data::ArrayIterator<NotNullPtr<Math::Geometry::Polygon>> it = mpg->Iterator();
	Bool found = false;
	sb->AppendUTF8Char('(');
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		AppendPolygon(sb, it.Next(), reverseAxis);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendMultiSurface(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::MultiSurface> ms, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	NotNullPtr<Math::Geometry::Vector2D> geometry;
	Data::ArrayIterator<NotNullPtr<Math::Geometry::Vector2D>> it = ms->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		geometry = it.Next();
		Math::Geometry::Vector2D::VectorType t = geometry->GetVectorType();
		if (t == Math::Geometry::Vector2D::VectorType::CurvePolygon)
		{
			sb->AppendC(UTF8STRC("CURVEPOLYGON"));
			AppendCurvePolygon(sb, NotNullPtr<Math::Geometry::CurvePolygon>::ConvertFrom(geometry), reverseAxis);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			sb->AppendC(UTF8STRC("POLYGON"));
			AppendPolygon(sb, NotNullPtr<Math::Geometry::Polygon>::ConvertFrom(geometry), reverseAxis);
		}
		else
		{
			printf("Unknown type in multisurface: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(t).v);
		}
		found = true;
	}
	sb->AppendUTF8Char(')');
}

Bool Math::WKTWriter::AppendGeometryCollection(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::GeometryCollection> geoColl)
{
	sb->AppendUTF8Char('(');
	NotNullPtr<Math::Geometry::Vector2D> geometry;
	Data::ArrayIterator<NotNullPtr<Math::Geometry::Vector2D>> it = geoColl->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		geometry = it.Next();
		if (!ToText(sb, geometry))
			return false;
		found = true;
	}
	sb->AppendUTF8Char(')');
	return true;
}

Math::WKTWriter::WKTWriter()
{
	this->lastError = 0;
	this->reverseAxis = false;
}

Math::WKTWriter::~WKTWriter()
{
	SDEL_STRING(this->lastError);
}

Text::CStringNN Math::WKTWriter::GetWriterName() const
{
	return CSTR("Well Known Text (WKT)");
}

Bool Math::WKTWriter::ToText(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("POINT("));
		if (vec->HasZ())
		{
			Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec.Ptr();
			Math::Vector3 pos = pt->GetPos3D();
			if (this->reverseAxis)
			{
				sb->AppendDouble(pos.GetY(), &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pos.GetX(), &Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(pos.GetX(), &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pos.GetY(), &Text::DoubleStyleC);
			}
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pos.GetZ(), &Text::DoubleStyleC);
			if (vec->HasM())
			{
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(((Math::Geometry::PointZM*)pt)->GetM(), &Text::DoubleStyleC);
			}
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec.Ptr();
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			if (this->reverseAxis)
			{
				sb->AppendDouble(coord.y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(coord.x, &Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(coord.x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(coord.y, &Text::DoubleStyleC);
			}
			if (vec->HasM())
			{
				sb->AppendC(UTF8STRC(" NAN "));
				sb->AppendDouble(((Math::Geometry::PointM*)pt)->GetM(), &Text::DoubleStyleC);
			}
		}
		sb->AppendC(UTF8STRC(")"));
		return true;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("POLYGON"));
		AppendPolygon(sb, NotNullPtr<Math::Geometry::Polygon>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("MULTILINESTRING"));
		AppendPolyline(sb, NotNullPtr<Math::Geometry::Polyline>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		sb->AppendC(UTF8STRC("MULTIPOLYGON"));
		AppendMultiPolygon(sb, NotNullPtr<Math::Geometry::MultiPolygon>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::LineString:
		sb->AppendC(UTF8STRC("LINESTRING"));
		AppendLineString(sb, NotNullPtr<Math::Geometry::LineString>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::CircularString:
		sb->AppendC(UTF8STRC("CIRCULARSTRING"));
		AppendLineString(sb, NotNullPtr<Math::Geometry::LineString>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
		sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
		AppendCompoundCurve(sb, NotNullPtr<Math::Geometry::CompoundCurve>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
		sb->AppendC(UTF8STRC("CURVEPOLYGON"));
		AppendCurvePolygon(sb, NotNullPtr<Math::Geometry::CurvePolygon>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		sb->AppendC(UTF8STRC("MULTISURFACE"));
		AppendMultiSurface(sb, NotNullPtr<Math::Geometry::MultiSurface>::ConvertFrom(vec), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
		sb->AppendC(UTF8STRC("GEOMETRYCOLLECTION"));
		return AppendGeometryCollection(sb, NotNullPtr<Math::Geometry::GeometryCollection>::ConvertFrom(vec));
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
	case Math::Geometry::Vector2D::VectorType::LinearRing:
	case Math::Geometry::Vector2D::VectorType::Image:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	case Math::Geometry::Vector2D::VectorType::Unknown:
	default:
		sb->AppendC(UTF8STRC("Unsupported vector type"));
		this->SetLastError(CSTR("Unsupported vector type"));
		return false;
	}

}

Text::String *Math::WKTWriter::GetLastError()
{
	return this->lastError;
}

void Math::WKTWriter::SetReverseAxis(Bool reverseAxis)
{
	this->reverseAxis = reverseAxis;
}
