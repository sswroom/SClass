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
	this->lastError = Text::String::New(lastError);
}

void Math::WKTWriter::AppendLineString(Text::StringBuilderUTF8 *sb, Math::Geometry::LineString *pl)
{
	sb->AppendUTF8Char('(');
	UOSInt nPoint;
	Math::Coord2DDbl *pointList = pl->GetPointList(&nPoint);
	Double *zArr = pl->GetZList(&nPoint);
	Double *mArr = pl->GetMList(&nPoint);
	UOSInt i;
	if (zArr || mArr)
	{
		if (zArr && mArr)
		{
			i = 0;
			while (i < nPoint)
			{
				if (i > 0) sb->AppendUTF8Char(',');
				sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
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
				sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
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
				sb->AppendDouble(pointList[i].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[i].y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(zArr[i], &Text::DoubleStyleC);
				i++;
			}
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
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolygon(Text::StringBuilderUTF8 *sb, Math::Geometry::Polygon *pg)
{
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
	Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	k = 0;
	i = 0;
	j = nPtOfst - 1;
	sb->AppendUTF8Char('(');
	while (i < j)
	{
		sb->AppendUTF8Char('(');
		while (k < ptOfstList[i + 1])
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			k++;
			if (k < ptOfstList[i + 1])
			{
				sb->AppendUTF8Char(',');
			}
		}
		sb->AppendUTF8Char(')');
		sb->AppendUTF8Char(',');
		i++;
	}
	sb->AppendUTF8Char('(');
	while (k < nPoint)
	{
		sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolygonZ(Text::StringBuilderUTF8 *sb, Math::Geometry::Polygon *pg)
{
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
	Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
	Double *zList = pg->GetZList(&nPoint);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	k = 0;
	i = 0;
	j = nPtOfst - 1;
	sb->AppendUTF8Char('(');
	while (i < j)
	{
		sb->AppendUTF8Char('(');
		while (k < ptOfstList[i + 1])
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(zList[k], &Text::DoubleStyleC);
			k++;
			if (k < ptOfstList[i + 1])
			{
				sb->AppendUTF8Char(',');
			}
		}
		sb->AppendUTF8Char(')');
		sb->AppendUTF8Char(',');
		i++;
	}
	sb->AppendUTF8Char('(');
	while (k < nPoint)
	{
		sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(zList[k], &Text::DoubleStyleC);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolyline(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl)
{
	sb->AppendUTF8Char('(');
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pl->GetPtOfstList(&nPtOfst);
	Math::Coord2DDbl *pointList = pl->GetPointList(&nPoint);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	k = 0;
	i = 0;
	j = nPtOfst - 1;
	while (i < j)
	{
		sb->AppendUTF8Char('(');
		while (k < ptOfstList[i + 1])
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			k++;
			if (k < ptOfstList[i + 1])
			{
				sb->AppendUTF8Char(',');
			}
		}
		sb->AppendUTF8Char(')');
		sb->AppendUTF8Char(',');
		i++;
	}
	sb->AppendUTF8Char('(');
	while (k < nPoint)
	{
		sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolylineZ(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl)
{
	sb->AppendUTF8Char('(');
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pl->GetPtOfstList(&nPtOfst);
	Math::Coord2DDbl *pointList = pl->GetPointList(&nPoint);
	Double *zList = pl->GetZList(&nPoint);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	k = 0;
	i = 0;
	j = nPtOfst - 1;
	while (i < j)
	{
		sb->AppendUTF8Char('(');
		while (k < ptOfstList[i + 1])
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(zList[k], &Text::DoubleStyleC);
			k++;
			if (k < ptOfstList[i + 1])
			{
				sb->AppendUTF8Char(',');
			}
		}
		sb->AppendUTF8Char(')');
		sb->AppendUTF8Char(',');
		i++;
	}
	sb->AppendUTF8Char('(');
	while (k < nPoint)
	{
		sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(zList[k], &Text::DoubleStyleC);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolylineZM(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl)
{
	sb->AppendUTF8Char('(');
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pl->GetPtOfstList(&nPtOfst);
	Math::Coord2DDbl *pointList = pl->GetPointList(&nPoint);
	Double *zList = pl->GetZList(&nPoint);
	Double *mList = pl->GetMList(&nPoint);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	k = 0;
	i = 0;
	j = nPtOfst - 1;
	while (i < j)
	{
		sb->AppendUTF8Char('(');
		while (k < ptOfstList[i + 1])
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(zList[k], &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(mList[k], &Text::DoubleStyleC);
			k++;
			if (k < ptOfstList[i + 1])
			{
				sb->AppendUTF8Char(',');
			}
		}
		sb->AppendUTF8Char(')');
		sb->AppendUTF8Char(',');
		i++;
	}
	sb->AppendUTF8Char('(');
	while (k < nPoint)
	{
		sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(zList[k], &Text::DoubleStyleC);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(mList[k], &Text::DoubleStyleC);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendCompoundCurve(Text::StringBuilderUTF8 *sb, Math::Geometry::CompoundCurve *cc)
{
	sb->AppendUTF8Char('(');
	Math::Geometry::LineString *pl;
	UOSInt i = 0;
	UOSInt j = cc->GetCount();
	while (i < j)
	{
		if (i > 0) sb->AppendUTF8Char(',');
		pl = cc->GetItem(i);
		if (pl->GetVectorType() == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			sb->AppendC(UTF8STRC("CIRCULARSTRING"));
		}
		AppendLineString(sb, pl);
		i++;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendCurvePolygon(Text::StringBuilderUTF8 *sb, Math::Geometry::CurvePolygon *cpg)
{
	sb->AppendUTF8Char('(');
	Math::Geometry::Vector2D *geometry;
	UOSInt i = 0;
	UOSInt j = cpg->GetCount();
	while (i < j)
	{
		if (i > 0) sb->AppendUTF8Char(',');
		geometry = cpg->GetItem(i);
		Math::Geometry::Vector2D::VectorType t = geometry->GetVectorType();
		if (t == Math::Geometry::Vector2D::VectorType::LineString)
		{
			AppendLineString(sb, (Math::Geometry::LineString*)geometry);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			sb->AppendC(UTF8STRC("CIRCULARSTRING"));
			AppendLineString(sb, (Math::Geometry::LineString*)geometry);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
			AppendCompoundCurve(sb, (Math::Geometry::CompoundCurve*)geometry);
		}
		i++;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendMultiSurface(Text::StringBuilderUTF8 *sb, Math::Geometry::MultiSurface *ms)
{
	sb->AppendUTF8Char('(');
	Math::Geometry::Vector2D *geometry;
	UOSInt i = 0;
	UOSInt j = ms->GetCount();
	while (i < j)
	{
		if (i > 0) sb->AppendUTF8Char(',');
		geometry = ms->GetItem(i);
		Math::Geometry::Vector2D::VectorType t = geometry->GetVectorType();
		if (t == Math::Geometry::Vector2D::VectorType::CurvePolygon)
		{
			sb->AppendC(UTF8STRC("CURVEPOLYGON"));
			AppendCurvePolygon(sb, (Math::Geometry::CurvePolygon*)geometry);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			sb->AppendC(UTF8STRC("POLYGON"));
			if (geometry->HasZ())
			{
				AppendPolygonZ(sb, (Math::Geometry::Polygon*)geometry);
			}
			else
			{
				AppendPolygon(sb, (Math::Geometry::Polygon*)geometry);
			}
		}
		i++;
	}
	sb->AppendUTF8Char(')');
}

Bool Math::WKTWriter::AppendGeometryCollection(Text::StringBuilderUTF8 *sb, Math::Geometry::GeometryCollection *geoColl)
{
	sb->AppendUTF8Char('(');
	Math::Geometry::Vector2D *geometry;
	UOSInt i = 0;
	UOSInt j = geoColl->GetCount();
	while (i < j)
	{
		if (i > 0) sb->AppendUTF8Char(',');
		geometry = geoColl->GetItem(i);
		if (!ToText(sb, geometry))
			return false;
		i++;
	}
	sb->AppendUTF8Char(')');
	return true;
}

Math::WKTWriter::WKTWriter()
{
	this->lastError = 0;
}

Math::WKTWriter::~WKTWriter()
{
	SDEL_STRING(this->lastError);
}

Text::CString Math::WKTWriter::GetWriterName()
{
	return CSTR("Well Known Text (WKT)");
}

Bool Math::WKTWriter::ToText(Text::StringBuilderUTF8 *sb, Math::Geometry::Vector2D *vec)
{
	if (vec == 0)
	{
		this->SetLastError(CSTR("Input vector is null"));
		return false;
	}
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("POINT("));
		if (vec->HasZ())
		{
			Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetPos3D(&x, &y, &z);
			sb->AppendDouble(x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(z, &Text::DoubleStyleC);
			if (vec->HasM())
			{
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(((Math::Geometry::PointZM*)pt)->GetM(), &Text::DoubleStyleC);
			}
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			sb->AppendDouble(coord.x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(coord.y, &Text::DoubleStyleC);
			if (vec->HasM())
			{
				sb->AppendC(UTF8STRC(" NULL "));
				sb->AppendDouble(((Math::Geometry::PointM*)pt)->GetM(), &Text::DoubleStyleC);
			}
		}
		sb->AppendC(UTF8STRC(")"));
		return true;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("POLYGON"));
		if (vec->HasZ())
		{
			AppendPolygonZ(sb, (Math::Geometry::Polygon*)vec);
		}
		else
		{
			AppendPolygon(sb, (Math::Geometry::Polygon*)vec);
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("MULTILINESTRING"));
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
			if (pl->HasZ())
			{
				if (pl->HasM())
				{
					AppendPolylineZM(sb, pl);
				}
				else
				{
					AppendPolylineZ(sb, pl);
				}
			}
			else
			{
				AppendPolyline(sb, pl);
			}
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		sb->AppendC(UTF8STRC("MULTIPOLYGON"));
		{
			Math::Geometry::MultiPolygon *mpg = (Math::Geometry::MultiPolygon*)vec;
			UOSInt i = 0;
			UOSInt j = mpg->GetCount();
			sb->AppendUTF8Char('(');
			while (i < j)
			{
				if (i > 0)
				{
					sb->AppendUTF8Char(',');
				}
				if (mpg->HasZ())
				{
					AppendPolygonZ(sb, mpg->GetItem(i));
				}
				else
				{
					AppendPolygon(sb, mpg->GetItem(i));
				}
				i++;
			}
			sb->AppendUTF8Char(')');
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::LineString:
		sb->AppendC(UTF8STRC("LINESTRING"));
		AppendLineString(sb, (Math::Geometry::LineString*)vec);
		return true;
	case Math::Geometry::Vector2D::VectorType::CircularString:
		sb->AppendC(UTF8STRC("CIRCULARSTRING"));
		AppendLineString(sb, (Math::Geometry::LineString*)vec);
		return true;
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
		sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
		AppendCompoundCurve(sb, (Math::Geometry::CompoundCurve*)vec);
		return true;
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
		sb->AppendC(UTF8STRC("CURVEPOLYGON"));
		AppendCurvePolygon(sb, (Math::Geometry::CurvePolygon*)vec);
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		sb->AppendC(UTF8STRC("MULTISURFACE"));
		AppendMultiSurface(sb, (Math::Geometry::MultiSurface*)vec);
		return true;
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
		sb->AppendC(UTF8STRC("GEOMETRYCOLLECTION"));
		return AppendGeometryCollection(sb, (Math::Geometry::GeometryCollection*)vec);
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
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
