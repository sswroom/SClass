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

void Math::WKTWriter::AppendLineString(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::LineString *pl, Bool reverseAxis)
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

void Math::WKTWriter::AppendPolygon(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Polygon *pg, Bool reverseAxis)
{
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pg->GetPtOfstList(nPtOfst);
	Math::Coord2DDbl *pointList = pg->GetPointList(nPoint);
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
			if (reverseAxis)
			{
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			}
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
		if (reverseAxis)
		{
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		}
		else
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		}
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolygonZ(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Polygon *pg, Bool reverseAxis)
{
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pg->GetPtOfstList(nPtOfst);
	Math::Coord2DDbl *pointList = pg->GetPointList(nPoint);
	Double *zList = pg->GetZList(nPoint);
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
			if (reverseAxis)
			{
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			}
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
		if (reverseAxis)
		{
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		}
		else
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		}
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

void Math::WKTWriter::AppendPolyline(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Polyline *pl, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pl->GetPtOfstList(nPtOfst);
	Math::Coord2DDbl *pointList = pl->GetPointList(nPoint);
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
			if (reverseAxis)
			{
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			}
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
		if (reverseAxis)
		{
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		}
		else
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		}
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolylineZ(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Polyline *pl, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pl->GetPtOfstList(nPtOfst);
	Math::Coord2DDbl *pointList = pl->GetPointList(nPoint);
	Double *zList = pl->GetZList(nPoint);
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
			if (reverseAxis)
			{
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			}
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
		if (reverseAxis)
		{
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		}
		else
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		}
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

void Math::WKTWriter::AppendPolylineZM(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Polyline *pl, Bool reverseAxis)
{
	sb->AppendUTF8Char('(');
	UOSInt nPtOfst;
	UOSInt nPoint;
	UInt32 *ptOfstList = pl->GetPtOfstList(nPtOfst);
	Math::Coord2DDbl *pointList = pl->GetPointList(nPoint);
	Double *zList = pl->GetZList(nPoint);
	Double *mList = pl->GetMList(nPoint);
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
			if (reverseAxis)
			{
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			}
			else
			{
				sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			}
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
		if (reverseAxis)
		{
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
		}
		else
		{
			sb->AppendDouble(pointList[k].x, &Text::DoubleStyleC);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y, &Text::DoubleStyleC);
		}
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

void Math::WKTWriter::AppendCompoundCurve(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::CompoundCurve *cc, Bool reverseAxis)
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
		AppendLineString(sb, pl, reverseAxis);
		i++;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendCurvePolygon(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::CurvePolygon *cpg, Bool reverseAxis)
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
			AppendLineString(sb, (Math::Geometry::LineString*)geometry, reverseAxis);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CircularString)
		{
			sb->AppendC(UTF8STRC("CIRCULARSTRING"));
			AppendLineString(sb, (Math::Geometry::LineString*)geometry, reverseAxis);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
			AppendCompoundCurve(sb, (Math::Geometry::CompoundCurve*)geometry, reverseAxis);
		}
		i++;
	}
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendMultiSurface(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::MultiSurface *ms, Bool reverseAxis)
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
			AppendCurvePolygon(sb, (Math::Geometry::CurvePolygon*)geometry, reverseAxis);
		}
		else if (t == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			sb->AppendC(UTF8STRC("POLYGON"));
			if (geometry->HasZ())
			{
				AppendPolygonZ(sb, (Math::Geometry::Polygon*)geometry, reverseAxis);
			}
			else
			{
				AppendPolygon(sb, (Math::Geometry::Polygon*)geometry, reverseAxis);
			}
		}
		i++;
	}
	sb->AppendUTF8Char(')');
}

Bool Math::WKTWriter::AppendGeometryCollection(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::GeometryCollection *geoColl)
{
	sb->AppendUTF8Char('(');
	NotNullPtr<Math::Geometry::Vector2D> geometry;
	UOSInt i = 0;
	UOSInt j = geoColl->GetCount();
	while (i < j)
	{
		if (i > 0) sb->AppendUTF8Char(',');
		if (!geometry.Set(geoColl->GetItem(i)))
			return false;
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
			AppendPolygonZ(sb, (Math::Geometry::Polygon*)vec.Ptr(), this->reverseAxis);
		}
		else
		{
			AppendPolygon(sb, (Math::Geometry::Polygon*)vec.Ptr(), this->reverseAxis);
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("MULTILINESTRING"));
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec.Ptr();
			if (pl->HasZ())
			{
				if (pl->HasM())
				{
					AppendPolylineZM(sb, pl, this->reverseAxis);
				}
				else
				{
					AppendPolylineZ(sb, pl, this->reverseAxis);
				}
			}
			else
			{
				AppendPolyline(sb, pl, this->reverseAxis);
			}
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		sb->AppendC(UTF8STRC("MULTIPOLYGON"));
		{
			Math::Geometry::MultiPolygon *mpg = (Math::Geometry::MultiPolygon*)vec.Ptr();
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
					AppendPolygonZ(sb, mpg->GetItem(i), this->reverseAxis);
				}
				else
				{
					AppendPolygon(sb, mpg->GetItem(i), this->reverseAxis);
				}
				i++;
			}
			sb->AppendUTF8Char(')');
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::LineString:
		sb->AppendC(UTF8STRC("LINESTRING"));
		AppendLineString(sb, (Math::Geometry::LineString*)vec.Ptr(), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::CircularString:
		sb->AppendC(UTF8STRC("CIRCULARSTRING"));
		AppendLineString(sb, (Math::Geometry::LineString*)vec.Ptr(), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
		sb->AppendC(UTF8STRC("COMPOUNDCURVE"));
		AppendCompoundCurve(sb, (Math::Geometry::CompoundCurve*)vec.Ptr(), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
		sb->AppendC(UTF8STRC("CURVEPOLYGON"));
		AppendCurvePolygon(sb, (Math::Geometry::CurvePolygon*)vec.Ptr(), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		sb->AppendC(UTF8STRC("MULTISURFACE"));
		AppendMultiSurface(sb, (Math::Geometry::MultiSurface*)vec.Ptr(), this->reverseAxis);
		return true;
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
		sb->AppendC(UTF8STRC("GEOMETRYCOLLECTION"));
		return AppendGeometryCollection(sb, (Math::Geometry::GeometryCollection*)vec.Ptr());
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

void Math::WKTWriter::SetReverseAxis(Bool reverseAxis)
{
	this->reverseAxis = reverseAxis;
}
