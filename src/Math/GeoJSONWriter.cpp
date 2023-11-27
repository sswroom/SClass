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

Text::CStringNN Math::GeoJSONWriter::GetWriterName() const
{
	return CSTR("GeoJSON");
}

Bool Math::GeoJSONWriter::ToText(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<const Math::Geometry::Vector2D> vec)
{
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
			Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec.Ptr();
			Math::Vector3 pos = pt->GetPos3D();
			sb->AppendDouble(pos.GetX());
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(pos.GetY());
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(pos.GetZ());
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec.Ptr();
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
			Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec.Ptr();
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pg->GetPtOfstList(nPtOfst);
			Math::Coord2DDbl *pointList = pg->GetPointList(nPoint);
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
	case Math::Geometry::Vector2D::VectorType::LineString:
		{
			NotNullPtr<Math::Geometry::LineString> lineString = NotNullPtr<Math::Geometry::LineString>::ConvertFrom(vec);
			sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"type\": \"LineString\",\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
			UOSInt i = 0;
			UOSInt nPoint;
			Math::Coord2DDbl *pointList = lineString->GetPointList(nPoint);
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
		break;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		{
			NotNullPtr<Math::Geometry::Polyline> pl = NotNullPtr<Math::Geometry::Polyline>::ConvertFrom(vec);
			sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"type\": \"MultiLineString\",\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
			UOSInt i = 0;
			UOSInt j = pl->GetCount();
			while (i < j)
			{
				Math::Geometry::LineString *lineString = pl->GetItem(i);
				UOSInt nPoint;
				Math::Coord2DDbl *pointList = lineString->GetPointList(nPoint);
				Math::Coord2DDbl initPt;
				UOSInt k;
				if (i == 0)
				{
					sb->AppendC(UTF8STRC("\t\t\t[\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t[\r\n"));
				}
				k = 0;
				sb->AppendC(UTF8STRC("\t\t\t\t["));
				sb->AppendDouble(pointList[k].x);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendDouble(pointList[k].y);
				sb->AppendUTF8Char(']');
				initPt = pointList[k];
				k++;
				while (k < nPoint)
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t\t["));
					sb->AppendDouble(pointList[k].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[k].y);
					sb->AppendUTF8Char(']');
					k++;
				}
				if (pointList[nPoint - 1] != initPt)
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
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"type\": \"MultiPolygon\",\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
		{
			Math::Geometry::MultiPolygon *mpg = (Math::Geometry::MultiPolygon*)vec.Ptr();
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
				UInt32 *ptOfstList = pg->GetPtOfstList(nPtOfst);
				Math::Coord2DDbl *pointList = pg->GetPointList(nPoint);
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
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
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

Bool Math::GeoJSONWriter::ToGeometry(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		json->ObjectAddStr(CSTR("type"), CSTR("Point"));
		if (vec->HasZ())
		{
			Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec.Ptr();
			json->ObjectAddVector3(CSTR("coordinates"), pt->GetPos3D());
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec.Ptr();
			json->ObjectAddCoord2D(CSTR("coordinates"), pt->GetCenter());
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		json->ObjectAddStr(CSTR("type"), CSTR("Polygon"));
		json->ObjectBeginArray(CSTR("coordinates"));
		{
			NotNullPtr<Math::Geometry::Polygon> pg = NotNullPtr<Math::Geometry::Polygon>::ConvertFrom(vec);
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pg->GetPtOfstList(nPtOfst);
			Math::Coord2DDbl *pointList = pg->GetPointList(nPoint);
			Math::Coord2DDbl initPt;
			UOSInt i = 0;
			UOSInt j = 0;
			UOSInt k;
			while (i < nPtOfst)
			{
				json->ArrayBeginArray();
				if (i + 1 == nPtOfst)
				{
					k = nPoint;
				}
				else
				{
					k = ptOfstList[i + 1];
				}
				json->ArrayAddCoord2D(pointList[j]);
				initPt = pointList[j];
				j++;
				while (j < k)
				{
					json->ArrayAddCoord2D(pointList[j]);
					j++;
				}
				if (pointList[k - 1] != initPt)
				{
					json->ArrayAddCoord2D(initPt);
				}
				json->ArrayEnd();
				i++;
			}
		}
		json->ArrayEnd();
		return true;
	case Math::Geometry::Vector2D::VectorType::LineString:
		json->ObjectAddStr(CSTR("type"), CSTR("LineString"));
		json->ObjectBeginArray(CSTR("coordinates"));
		{
			NotNullPtr<Math::Geometry::LineString> lineString = NotNullPtr<Math::Geometry::LineString>::ConvertFrom(vec);
			UOSInt nPoint;
			Math::Coord2DDbl *pointList = lineString->GetPointList(nPoint);
			UOSInt i = 0;
			while (i < nPoint)
			{
				json->ArrayAddCoord2D(pointList[i]);
				i++;
			}
		}
		json->ArrayEnd();
		return true;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		json->ObjectAddStr(CSTR("type"), CSTR("LineString"));
		json->ObjectBeginArray(CSTR("coordinates"));
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec.Ptr();
			NotNullPtr<Math::Geometry::LineString> lineString;
			UOSInt j = pl->GetCount();
			Math::Coord2DDbl *pointList;
			UOSInt i = 0;
			UOSInt k;
			UOSInt l;
			while (i < j)
			{
				if (lineString.Set(pl->GetItem(i)))
				{
					json->ArrayBeginArray();
					k = 0;
					pointList = lineString->GetPointList(l);
					while (k < l)
					{
						json->ArrayAddCoord2D(pointList[k]);
						k++;
					}
					json->ArrayEnd();
				}
				i++;
			}
		}
		json->ArrayEnd();
		return true;
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
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
		this->SetLastError(CSTR("Unsupported vector type"));
		return false;
	}
}
