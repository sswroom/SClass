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

#define DOUBLESIGFIG 17

void Math::WKTWriter::SetLastError(Text::CStringNN lastError)
{
	OPTSTR_DEL(this->lastError);
	this->lastError = Text::String::New(lastError);
}

void Math::WKTWriter::AppendLineString(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::LineString> pl, Bool reverseAxis, Bool no3D)
{
	sb->AppendUTF8Char('(');
	UOSInt nPoint;
	UnsafeArray<Math::Coord2DDbl> pointList = pl->GetPointList(nPoint);
	UnsafeArrayOpt<Double> zArr = pl->GetZList(nPoint);
	UnsafeArrayOpt<Double> mArr = pl->GetMList(nPoint);
	UnsafeArray<Double> nnzArr;
	UnsafeArray<Double> nnmArr;
	UOSInt i;
	if (!no3D && zArr.SetTo(nnzArr))
	{
		if (mArr.SetTo(nnmArr))
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				if (reverseAxis)
				{
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
				}
				else
				{
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
				}
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(nnzArr[i], DOUBLESIGFIG, Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(nnmArr[i], DOUBLESIGFIG, Text::DoubleStyleC);
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
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
				}
				else
				{
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
				}
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(nnzArr[i], DOUBLESIGFIG, Text::DoubleStyleC);
				i++;
			}
		}
	}
	else
	{
		if (!no3D && mArr.SetTo(nnmArr))
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				if (reverseAxis)
				{
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
				}
				else
				{
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
				}
				sb->AppendC(UTF8STRC(" NaN "));
				sb->AppendDouble(nnmArr[i], DOUBLESIGFIG, Text::DoubleStyleC);
				i++;
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
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
					i++;
				}
			}
			else
			{
				i = 0;
				while (i < nPoint)
				{
					if (i > 0) sb->AppendUTF8Char(',');
					sb->AppendDouble(pointList[i].x, DOUBLESIGFIG, Text::DoubleStyleC);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[i].y, DOUBLESIGFIG, Text::DoubleStyleC);
					i++;
				}
			}
		}
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::Polygon> pg, Bool reverseAxis, Bool no3D)
{
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = pg->Iterator();
	Bool found = false;
	sb->AppendUTF8Char('(');
	while (it.HasNext())
	{
		if (found)
			sb->AppendUTF8Char(',');
		AppendLineString(sb, it.Next(), reverseAxis, no3D);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolyline(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::Polyline> pl, Bool reverseAxis, Bool no3D)
{
	sb->AppendUTF8Char('(');
	Data::ArrayIterator<NN<Math::Geometry::LineString>> it = pl->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		AppendLineString(sb, it.Next(), reverseAxis, no3D);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendCompoundCurve(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::CompoundCurve> cc, Bool reverseAxis, Bool no3D)
{
	sb->AppendUTF8Char('(');
	NN<Math::Geometry::LineString> pl;
	Data::ArrayIterator<NN<Math::Geometry::LineString>> it = cc->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		pl = it.Next();
		if (pl->GetVectorType() == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			sb->AppendC(UTF8STRC("CIRCULARSTRING"));
		}
		AppendLineString(sb, pl, reverseAxis, no3D);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendCurvePolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::CurvePolygon> cpg, Bool reverseAxis, Bool no3D)
{
	sb->AppendUTF8Char('(');
	NN<Math::Geometry::Vector2D> geometry;
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = cpg->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		geometry = it.Next();
		Math::Geometry::Vector2D::VectorType t = geometry->GetVectorType();
		if (t == Math::Geometry::Vector2D::VectorType::LineString || t == Math::Geometry::Vector2D::VectorType::LinearRing)
		{
			AppendLineString(sb, NN<Math::Geometry::LineString>::ConvertFrom(geometry), reverseAxis, no3D);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			sb->AppendC(UTF8STRC("CIRCULARSTRING"));
			AppendLineString(sb, NN<Math::Geometry::LineString>::ConvertFrom(geometry), reverseAxis, no3D);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
			AppendCompoundCurve(sb, NN<Math::Geometry::CompoundCurve>::ConvertFrom(geometry), reverseAxis, no3D);
		}
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendMultiPolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiPolygon> mpg, Bool reverseAxis, Bool no3D)
{
	Data::ArrayIterator<NN<Math::Geometry::Polygon>> it = mpg->Iterator();
	Bool found = false;
	sb->AppendUTF8Char('(');
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		AppendPolygon(sb, it.Next(), reverseAxis, no3D);
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendMultiSurface(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiSurface> ms, Bool reverseAxis, Bool no3D)
{
	sb->AppendUTF8Char('(');
	NN<Math::Geometry::Vector2D> geometry;
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = ms->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		geometry = it.Next();
		Math::Geometry::Vector2D::VectorType t = geometry->GetVectorType();
		if (t == Math::Geometry::Vector2D::VectorType::CurvePolygon)
		{
			sb->AppendC(UTF8STRC("CURVEPOLYGON"));
			AppendCurvePolygon(sb, NN<Math::Geometry::CurvePolygon>::ConvertFrom(geometry), reverseAxis, no3D);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			sb->AppendC(UTF8STRC("POLYGON"));
			AppendPolygon(sb, NN<Math::Geometry::Polygon>::ConvertFrom(geometry), reverseAxis, no3D);
		}
		else
		{
			printf("Unknown type in multisurface: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(t).v.Ptr());
		}
		found = true;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendMultiCurve(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiCurve> mc, Bool reverseAxis, Bool no3D)
{
	sb->AppendUTF8Char('(');
	NN<Math::Geometry::Vector2D> geometry;
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = mc->Iterator();
	Bool found = false;
	while (it.HasNext())
	{
		if (found) sb->AppendUTF8Char(',');
		geometry = it.Next();
		Math::Geometry::Vector2D::VectorType t = geometry->GetVectorType();
		if (t == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
			AppendCompoundCurve(sb, NN<Math::Geometry::CompoundCurve>::ConvertFrom(geometry), reverseAxis, no3D);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::LineString)
		{
			AppendLineString(sb, NN<Math::Geometry::LineString>::ConvertFrom(geometry), reverseAxis, no3D);
		}
		else
		{
			printf("Unknown type in multisurface: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(t).v.Ptr());
		}
		found = true;
	}
	sb->AppendUTF8Char(')');
}

Bool Math::WKTWriter::AppendGeometryCollection(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::GeometryCollection> geoColl)
{
	sb->AppendUTF8Char('(');
	NN<Math::Geometry::Vector2D> geometry;
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = geoColl->Iterator();
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
	this->no3D = false;
}

Math::WKTWriter::~WKTWriter()
{
	OPTSTR_DEL(this->lastError);
}

Text::CStringNN Math::WKTWriter::GetWriterName() const
{
	return CSTR("Well Known Text (WKT)");
}

Bool Math::WKTWriter::ToText(NN<Text::StringBuilderUTF8> sb, NN<const Math::Geometry::Vector2D> vec)
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
				sb->AppendDouble(pos.GetY(), DOUBLESIGFIG, Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pos.GetX(), DOUBLESIGFIG, Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(pos.GetX(), DOUBLESIGFIG, Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pos.GetY(), DOUBLESIGFIG, Text::DoubleStyleC);
			}
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pos.GetZ(), DOUBLESIGFIG, Text::DoubleStyleC);
			if (vec->HasM())
			{
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(((Math::Geometry::PointZM*)pt)->GetM(), DOUBLESIGFIG, Text::DoubleStyleC);
			}
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec.Ptr();
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			if (this->reverseAxis)
			{
				sb->AppendDouble(coord.y, DOUBLESIGFIG, Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(coord.x, DOUBLESIGFIG, Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(coord.x, DOUBLESIGFIG, Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(coord.y, DOUBLESIGFIG, Text::DoubleStyleC);
			}
			if (vec->HasM())
			{
				sb->AppendC(UTF8STRC(" NAN "));
				sb->AppendDouble(((Math::Geometry::PointM*)pt)->GetM(), DOUBLESIGFIG, Text::DoubleStyleC);
			}
		}
		sb->AppendC(UTF8STRC(")"));
		return true;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("POLYGON"));
		AppendPolygon(sb, NN<Math::Geometry::Polygon>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("MULTILINESTRING"));
		AppendPolyline(sb, NN<Math::Geometry::Polyline>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		sb->AppendC(UTF8STRC("MULTIPOLYGON"));
		AppendMultiPolygon(sb, NN<Math::Geometry::MultiPolygon>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::LineString:
		sb->AppendC(UTF8STRC("LINESTRING"));
		AppendLineString(sb, NN<Math::Geometry::LineString>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::CircularString:
		sb->AppendC(UTF8STRC("CIRCULARSTRING"));
		AppendLineString(sb, NN<Math::Geometry::LineString>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
		sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
		AppendCompoundCurve(sb, NN<Math::Geometry::CompoundCurve>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
		sb->AppendC(UTF8STRC("CURVEPOLYGON"));
		AppendCurvePolygon(sb, NN<Math::Geometry::CurvePolygon>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		sb->AppendC(UTF8STRC("MULTISURFACE"));
		AppendMultiSurface(sb, NN<Math::Geometry::MultiSurface>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
		sb->AppendC(UTF8STRC("GEOMETRYCOLLECTION"));
		return AppendGeometryCollection(sb, NN<Math::Geometry::GeometryCollection>::ConvertFrom(vec));
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
		sb->AppendC(UTF8STRC("MULTICURVE"));
		AppendMultiCurve(sb, NN<Math::Geometry::MultiCurve>::ConvertFrom(vec), this->reverseAxis, this->no3D);
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
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

Optional<Text::String> Math::WKTWriter::GetLastError()
{
	return this->lastError;
}
