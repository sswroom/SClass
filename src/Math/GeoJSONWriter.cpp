#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/GeoJSONWriter.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::GeoJSONWriter::SetLastError(Text::CString lastError)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(lastError).Ptr();
}

Math::GeoJSONWriter::GeoJSONWriter()
{
	this->lastError = 0;
}

Math::GeoJSONWriter::~GeoJSONWriter()
{
	SDEL_STRING(this->lastError);
}

Text::CString Math::GeoJSONWriter::GetWriterName()
{
	return CSTR("GeoJSON");
}

Bool Math::GeoJSONWriter::ToText(Text::StringBuilderUTF8 *sb, Math::Geometry::Vector2D *vec)
{
	if (vec == 0)
	{
		this->SetLastError(CSTR("Input vector is null"));
		return false;
	}
	sb->AppendC(UTF8STRC("{\r\n"));
	sb->AppendC(UTF8STRC("\t\"type\": \"Feature\",\r\n"));
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"type\": \"Point\",\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"coordinates\": ["));
		if (vec->HasZ())
		{
			Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetPos3D(&x, &y, &z);
			sb->AppendDouble(x);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(y);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(z);
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			sb->AppendDouble(coord.x);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(coord.y);
		}
		sb->AppendC(UTF8STRC("]\r\n"));
		sb->AppendC(UTF8STRC("\t}"));
		break;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"type\": \"Polygon\",\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
		{
			Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
			Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
			Math::Coord2DDbl initPt;
			UOSInt i = 0;
			UOSInt j = 0;
			UOSInt k;
			while (i < nPtOfst)
			{
				if (i == 0)
				{
					sb->AppendC(UTF8STRC("\t\t\t[\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t[\r\n"));
				}
				if (i + 1 == nPtOfst)
				{
					k = nPoint;
				}
				else
				{
					k = ptOfstList[i + 1];
				}
				sb->AppendC(UTF8STRC("\t\t\t\t["));
				sb->AppendDouble(pointList[j].x);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendDouble(pointList[j].y);
				sb->AppendUTF8Char(']');
				initPt = pointList[j];
				j++;
				while (j < k)
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t\t["));
					sb->AppendDouble(pointList[j].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[j].y);
					sb->AppendUTF8Char(']');
					j++;
				}
				if (pointList[k - 1] != initPt)
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t\t["));
					sb->AppendDouble(initPt.x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(initPt.y);
					sb->AppendUTF8Char(']');
				}
				sb->AppendC(UTF8STRC("\r\n\t\t\t]"));
				i++;
			}
			sb->AppendC(UTF8STRC("\r\n\t\t]\r\n"));
			sb->AppendC(UTF8STRC("\t}"));
		}
		break;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		{
			Math::Geometry::Polyline *pg = (Math::Geometry::Polyline*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
			Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
			if (nPtOfst == 1)
			{
				sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
				sb->AppendC(UTF8STRC("\t\t\"type\": \"LineString\",\r\n"));
				sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
				UOSInt i = 0;
				sb->AppendC(UTF8STRC("\t\t\t["));
				sb->AppendDouble(pointList[i].x);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendDouble(pointList[i].y);
				sb->AppendUTF8Char(']');
				i++;
				while (i < nPoint)
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t["));
					sb->AppendDouble(pointList[i].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[i].y);
					sb->AppendUTF8Char(']');
					i++;
				}
				sb->AppendC(UTF8STRC("\r\n\t\t]\r\n"));
				sb->AppendC(UTF8STRC("\t}"));
			}
			else
			{
				sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
				sb->AppendC(UTF8STRC("\t\t\"type\": \"MultiLineString\",\r\n"));
				sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
				Math::Coord2DDbl initPt;
				UOSInt i = 0;
				UOSInt j = 0;
				UOSInt k;
				while (i < nPtOfst)
				{
					if (i == 0)
					{
						sb->AppendC(UTF8STRC("\t\t\t[\r\n"));
					}
					else
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t[\r\n"));
					}
					if (i + 1 == nPtOfst)
					{
						k = nPoint;
					}
					else
					{
						k = ptOfstList[i + 1];
					}
					sb->AppendC(UTF8STRC("\t\t\t\t["));
					sb->AppendDouble(pointList[j].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[j].y);
					sb->AppendUTF8Char(']');
					initPt = pointList[j];
					j++;
					while (j < k)
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t["));
						sb->AppendDouble(pointList[j].x);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(pointList[j].y);
						sb->AppendUTF8Char(']');
						j++;
					}
					if (pointList[k - 1] != initPt)
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t["));
						sb->AppendDouble(initPt.x);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(initPt.y);
						sb->AppendUTF8Char(']');
					}
					sb->AppendC(UTF8STRC("\r\n\t\t\t]"));
					i++;
				}
				sb->AppendC(UTF8STRC("\r\n\t\t]\r\n"));
				sb->AppendC(UTF8STRC("\t}"));
			}
		}
		break;
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"type\": \"MultiPolygon\",\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
		{
			Math::Geometry::MultiPolygon *mpg = (Math::Geometry::MultiPolygon*)vec;
			UOSInt pgIndex = 0;
			UOSInt pgCnt = mpg->GetCount();
			while (pgIndex < pgCnt)
			{
				if (pgIndex == 0)
				{
					sb->AppendC(UTF8STRC("\t\t\t[\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t[\r\n"));
				}
				Math::Geometry::Polygon *pg = mpg->GetItem(pgIndex);
				UOSInt nPtOfst;
				UOSInt nPoint;
				UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
				Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
				Math::Coord2DDbl initPt;
				UOSInt i = 0;
				UOSInt j = 0;
				UOSInt k;
				while (i < nPtOfst)
				{
					if (i == 0)
					{
						sb->AppendC(UTF8STRC("\t\t\t\t[\r\n"));
					}
					else
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t[\r\n"));
					}
					if (i + 1 == nPtOfst)
					{
						k = nPoint;
					}
					else
					{
						k = ptOfstList[i + 1];
					}
					sb->AppendC(UTF8STRC("\t\t\t\t\t["));
					sb->AppendDouble(pointList[j].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[j].y);
					sb->AppendUTF8Char(']');
					initPt = pointList[j];
					j++;
					while (j < k)
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t\t["));
						sb->AppendDouble(pointList[j].x);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(pointList[j].y);
						sb->AppendUTF8Char(']');
						j++;
					}
					if (pointList[k - 1] != initPt)
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t\t["));
						sb->AppendDouble(initPt.x);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(initPt.y);
						sb->AppendUTF8Char(']');
					}
					sb->AppendC(UTF8STRC("\r\n\t\t\t\t]"));
					i++;
				}
				sb->AppendC(UTF8STRC("\r\n\t\t\t]"));
				pgIndex++;
			}
			sb->AppendC(UTF8STRC("\r\n\t\t]\r\n"));
			sb->AppendC(UTF8STRC("\t}"));
		}
		break;
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::LineString:
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
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
		this->SetLastError(CSTR("Unsupported vector type"));
		return false;
	}
	sb->AppendC(UTF8STRC("\r\n}"));
	return true;
}

Text::String *Math::GeoJSONWriter::GetLastError()
{
	return this->lastError;
}

Bool Math::GeoJSONWriter::ToGeometry(Text::StringBuilderUTF8 *sb, Math::Geometry::Vector2D *vec)
{
	if (vec == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		this->SetLastError(CSTR("Input vector is null"));
		return false;
	}
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		sb->AppendUTF8Char('{');
		sb->AppendC(UTF8STRC("\"type\":\"Point\","));
		sb->AppendC(UTF8STRC("\"coordinates\":["));
		if (vec->HasZ())
		{
			Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetPos3D(&x, &y, &z);
			sb->AppendDouble(x);
			sb->AppendUTF8Char(',');
			sb->AppendDouble(y);
			sb->AppendUTF8Char(',');
			sb->AppendDouble(z);
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			sb->AppendDouble(coord.x);
			sb->AppendUTF8Char(',');
			sb->AppendDouble(coord.y);
		}
		sb->AppendUTF8Char(']');
		sb->AppendUTF8Char('}');
		return true;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		sb->AppendUTF8Char('{');
		sb->AppendC(UTF8STRC("\"type\":\"Polygon\","));
		sb->AppendC(UTF8STRC("\"coordinates\":["));
		{
			Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
			Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
			Math::Coord2DDbl initPt;
			UOSInt i = 0;
			UOSInt j = 0;
			UOSInt k;
			while (i < nPtOfst)
			{
				if (i == 0)
				{
					sb->AppendUTF8Char('[');
				}
				else
				{
					sb->AppendC(UTF8STRC(",["));
				}
				if (i + 1 == nPtOfst)
				{
					k = nPoint;
				}
				else
				{
					k = ptOfstList[i + 1];
				}
				sb->AppendUTF8Char('[');
				sb->AppendDouble(pointList[j].x);
				sb->AppendUTF8Char(',');
				sb->AppendDouble(pointList[j].y);
				sb->AppendUTF8Char(']');
				initPt = pointList[j];
				j++;
				while (j < k)
				{
					sb->AppendC(UTF8STRC(",["));
					sb->AppendDouble(pointList[j].x);
					sb->AppendUTF8Char(',');
					sb->AppendDouble(pointList[j].y);
					sb->AppendUTF8Char(']');
					j++;
				}
				if (pointList[k - 1] != initPt)
				{
					sb->AppendC(UTF8STRC(",["));
					sb->AppendDouble(initPt.x);
					sb->AppendUTF8Char(',');
					sb->AppendDouble(initPt.y);
					sb->AppendUTF8Char(']');
				}
				sb->AppendUTF8Char(']');
				i++;
			}
			sb->AppendUTF8Char(']');
			sb->AppendUTF8Char('}');
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		{
			Math::Geometry::Polyline *pg = (Math::Geometry::Polyline*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
			Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
			if (nPtOfst == 1)
			{
				sb->AppendUTF8Char('{');
				sb->AppendC(UTF8STRC("\"type\":\"LineString\","));
				sb->AppendC(UTF8STRC("\"coordinates\":["));
				UOSInt i = 0;
				sb->AppendC(UTF8STRC("["));
				sb->AppendDouble(pointList[i].x);
				sb->AppendUTF8Char(',');
				sb->AppendDouble(pointList[i].y);
				sb->AppendUTF8Char(']');
				i++;
				while (i < nPoint)
				{
					sb->AppendC(UTF8STRC(",["));
					sb->AppendDouble(pointList[i].x);
					sb->AppendUTF8Char(',');
					sb->AppendDouble(pointList[i].y);
					sb->AppendUTF8Char(']');
					i++;
				}
				sb->AppendUTF8Char(']');
				sb->AppendUTF8Char('}');
			}
			else
			{
				sb->AppendUTF8Char('{');
				sb->AppendC(UTF8STRC("\"type\":\"MultiLineString\","));
				sb->AppendC(UTF8STRC("\"coordinates\":["));
				Math::Coord2DDbl initPt;
				UOSInt i = 0;
				UOSInt j = 0;
				UOSInt k;
				while (i < nPtOfst)
				{
					if (i == 0)
					{
						sb->AppendUTF8Char('[');
					}
					else
					{
						sb->AppendC(UTF8STRC(",["));
					}
					if (i + 1 == nPtOfst)
					{
						k = nPoint;
					}
					else
					{
						k = ptOfstList[i + 1];
					}
					sb->AppendUTF8Char('[');
					sb->AppendDouble(pointList[j].x);
					sb->AppendUTF8Char(',');
					sb->AppendDouble(pointList[j].y);
					sb->AppendUTF8Char(']');
					initPt = pointList[j];
					j++;
					while (j < k)
					{
						sb->AppendC(UTF8STRC(",["));
						sb->AppendDouble(pointList[j].x);
						sb->AppendUTF8Char(',');
						sb->AppendDouble(pointList[j].y);
						sb->AppendUTF8Char(']');
						j++;
					}
					if (pointList[k - 1] != initPt)
					{
						sb->AppendC(UTF8STRC(",["));
						sb->AppendDouble(initPt.x);
						sb->AppendUTF8Char(',');
						sb->AppendDouble(initPt.y);
						sb->AppendUTF8Char(']');
					}
					sb->AppendUTF8Char(']');
					i++;
				}
				sb->AppendUTF8Char(']');
				sb->AppendUTF8Char('}');
			}
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::LineString:
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
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
		sb->AppendC(UTF8STRC("null"));
		this->SetLastError(CSTR("Unsupported vector type"));
		return false;
	}
}
