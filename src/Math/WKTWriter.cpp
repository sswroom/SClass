#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/MultiPolygon.h"
#include "Math/PointZ.h"
#include "Math/Polygon.h"
#include "Math/Polyline.h"
#include "Math/WKTWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::WKTWriter::SetLastError(Text::CString lastError)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(lastError);
}

void Math::WKTWriter::AppendPolygon(Text::StringBuilderUTF8 *sb, Math::Polygon *pg)
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
			sb->AppendDouble(pointList[k].x);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y);
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
		sb->AppendDouble(pointList[k].x);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolyline(Text::StringBuilderUTF8 *sb, Math::Polyline *pl)
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
			sb->AppendDouble(pointList[k].x);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y);
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
		sb->AppendDouble(pointList[k].x);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
}

void Math::WKTWriter::AppendPolyline3D(Text::StringBuilderUTF8 *sb, Math::Polyline *pl)
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
			sb->AppendDouble(pointList[k].x);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(pointList[k].y);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(zList[k]);
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
		sb->AppendDouble(pointList[k].x);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(pointList[k].y);
		sb->AppendUTF8Char(' ');
		sb->AppendDouble(zList[k]);
		k++;
		if (k < nPoint)
		{
			sb->AppendUTF8Char(',');
		}
	}
	sb->AppendUTF8Char(')');
	sb->AppendUTF8Char(')');
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

Bool Math::WKTWriter::ToText(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec)
{
	if (vec == 0)
	{
		this->SetLastError(CSTR("Input vector is null"));
		return false;
	}
	switch (vec->GetVectorType())
	{
	case Math::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("POINT("));
		if (vec->HasZ())
		{
			Math::PointZ *pt = (Math::PointZ*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetPos3D(&x, &y, &z);
			sb->AppendDouble(x);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(y);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(z);
		}
		else
		{
			Math::Point *pt = (Math::Point*)vec;
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			sb->AppendDouble(coord.x);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(coord.y);
		}
		sb->AppendC(UTF8STRC(")"));
		return true;
	case Math::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("POLYGON"));
		AppendPolygon(sb, (Math::Polygon*)vec);
		return true;
	case Math::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("POLYLINE"));
		{
			Math::Polyline *pl = (Math::Polyline*)vec;
			if (pl->HasZ())
			{
				sb->AppendUTF8Char('Z');
				AppendPolyline3D(sb, pl);
			}
			else
			{
				AppendPolyline(sb, pl);
			}
		}
		return true;
	case Math::Vector2D::VectorType::Multipolygon:
		sb->AppendC(UTF8STRC("MULTIPOLYGON"));
		{
			Math::MultiPolygon *mpg = (Math::MultiPolygon*)vec;
			UOSInt i = 0;
			UOSInt j = mpg->GetCount();
			sb->AppendUTF8Char('(');
			while (i < j)
			{
				if (i > 0)
				{
					sb->AppendUTF8Char(',');
				}
				AppendPolygon(sb, mpg->GetItem(i));
				i++;
			}
			sb->AppendUTF8Char(')');
		}
		return true;
	case Math::Vector2D::VectorType::Multipoint:
	case Math::Vector2D::VectorType::Image:
	case Math::Vector2D::VectorType::String:
	case Math::Vector2D::VectorType::Ellipse:
	case Math::Vector2D::VectorType::PieArea:
	case Math::Vector2D::VectorType::Unknown:
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
