#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/GeoJSONWriter.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::GeoJSONWriter::SetLastError(Text::CStringNN lastError)
{
	OPTSTR_DEL(this->lastError);
	this->lastError = Text::String::New(lastError);
}

Math::GeoJSONWriter::GeoJSONWriter()
{
	this->lastError = nullptr;
}

Math::GeoJSONWriter::~GeoJSONWriter()
{
	OPTSTR_DEL(this->lastError);
}

Text::CStringNN Math::GeoJSONWriter::GetWriterName() const
{
	return CSTR("GeoJSON");
}

Bool Math::GeoJSONWriter::ToText(NN<Text::StringBuilderUTF8> sb, NN<const Math::Geometry::Vector2D> vec)
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
			NN<Math::Geometry::LinearRing> lr;
			UIntOS nPoint;
			UnsafeArray<Math::Coord2DDbl> pointList;
			Math::Coord2DDbl initPt;
			UIntOS j;
			Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = pg->Iterator();
			Bool found = false;
			while (it.HasNext())
			{
				if (!found)
				{
					sb->AppendC(UTF8STRC("\t\t\t[\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t[\r\n"));
				}
				lr = it.Next();
				pointList = lr->GetPointList(nPoint);
				sb->AppendC(UTF8STRC("\t\t\t\t["));
				sb->AppendDouble(pointList[0].x);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendDouble(pointList[0].y);
				sb->AppendUTF8Char(']');
				initPt = pointList[0];
				j = 1;
				while (j < nPoint)
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t\t["));
					sb->AppendDouble(pointList[j].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[j].y);
					sb->AppendUTF8Char(']');
					j++;
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
				found = true;
			}
			sb->AppendC(UTF8STRC("\r\n\t\t]\r\n"));
			sb->AppendC(UTF8STRC("\t}"));
		}
		break;
	case Math::Geometry::Vector2D::VectorType::LineString:
		{
			NN<Math::Geometry::LineString> lineString = NN<Math::Geometry::LineString>::ConvertFrom(vec);
			sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"type\": \"LineString\",\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
			UIntOS i = 0;
			UIntOS nPoint;
			UnsafeArray<Math::Coord2DDbl> pointList = lineString->GetPointList(nPoint);
			UnsafeArray<Double> zList;
			if (lineString->GetZList(nPoint).SetTo(zList))
			{
				sb->AppendC(UTF8STRC("\t\t\t["));
				sb->AppendDouble(pointList[i].x);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendDouble(pointList[i].y);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendDouble(zList[i]);
				sb->AppendUTF8Char(']');
				i++;
				while (i < nPoint)
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t["));
					sb->AppendDouble(pointList[i].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[i].y);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(zList[i]);
					sb->AppendUTF8Char(']');
					i++;
				}
			}
			else
			{
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
			}
			sb->AppendC(UTF8STRC("\r\n\t\t]\r\n"));
			sb->AppendC(UTF8STRC("\t}"));
		}
		break;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		{
			NN<Math::Geometry::Polyline> pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
			sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"type\": \"MultiLineString\",\r\n"));
			sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
			Data::ArrayIterator<NN<Math::Geometry::LineString>> it = pl->Iterator();
			Bool found = false;
			while (it.HasNext())
			{
				NN<Math::Geometry::LineString> lineString = it.Next();
				UIntOS nPoint;
				UnsafeArray<Math::Coord2DDbl> pointList = lineString->GetPointList(nPoint);
				UnsafeArray<Double> zList;
				UIntOS k;
				if (!found)
				{
					sb->AppendC(UTF8STRC("\t\t\t[\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t[\r\n"));
				}
				if (lineString->GetZList(nPoint).SetTo(zList))
				{
					k = 0;
					sb->AppendC(UTF8STRC("\t\t\t\t["));
					sb->AppendDouble(pointList[k].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[k].y);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(zList[k]);
					sb->AppendUTF8Char(']');
					k++;
					while (k < nPoint)
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t["));
						sb->AppendDouble(pointList[k].x);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(pointList[k].y);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(zList[k]);
						sb->AppendUTF8Char(']');
						k++;
					}
				}
				else
				{
					k = 0;
					sb->AppendC(UTF8STRC("\t\t\t\t["));
					sb->AppendDouble(pointList[k].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[k].y);
					sb->AppendUTF8Char(']');
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
				}
				sb->AppendC(UTF8STRC("\r\n\t\t\t]"));
				found = true;
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
			Data::ArrayIterator<NN<Math::Geometry::Polygon>> itPG = mpg->Iterator();
			Bool found = false;
			while (itPG.HasNext())
			{
				if (!found)
				{
					sb->AppendC(UTF8STRC("\t\t\t[\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC(",\r\n\t\t\t[\r\n"));
				}
				NN<Math::Geometry::Polygon> pg = itPG.Next();
				UIntOS nPoint;
				UnsafeArray<Math::Coord2DDbl> pointList;
				Math::Coord2DDbl initPt;
				NN<Math::Geometry::LinearRing> lr;
				Data::ArrayIterator<NN<Math::Geometry::LinearRing>> itLR = pg->Iterator();
				Bool foundLR = false;
				UIntOS j = 0;
				while (itLR.HasNext())
				{
					if (!foundLR)
					{
						sb->AppendC(UTF8STRC("\t\t\t\t[\r\n"));
					}
					else
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t[\r\n"));
					}
					lr = itLR.Next();
					pointList = lr->GetPointList(nPoint);
					sb->AppendC(UTF8STRC("\t\t\t\t\t["));
					sb->AppendDouble(pointList[0].x);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendDouble(pointList[0].y);
					sb->AppendUTF8Char(']');
					initPt = pointList[0];
					j = 1;
					while (j < nPoint)
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t\t["));
						sb->AppendDouble(pointList[j].x);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(pointList[j].y);
						sb->AppendUTF8Char(']');
						j++;
					}
					if (pointList[nPoint - 1] != initPt)
					{
						sb->AppendC(UTF8STRC(",\r\n\t\t\t\t\t["));
						sb->AppendDouble(initPt.x);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendDouble(initPt.y);
						sb->AppendUTF8Char(']');
					}
					sb->AppendC(UTF8STRC("\r\n\t\t\t\t]"));
					foundLR = true;
				}
				sb->AppendC(UTF8STRC("\r\n\t\t\t]"));
				found = true;
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

Optional<Text::String> Math::GeoJSONWriter::GetLastError()
{
	return this->lastError;
}

Bool Math::GeoJSONWriter::ToGeometry(NN<Text::JSONBuilder> json, NN<const Math::Geometry::Vector2D> vec)
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
			NN<Math::Geometry::Polygon> pg = NN<Math::Geometry::Polygon>::ConvertFrom(vec);
			UIntOS nPoint;
			UIntOS j;
			UnsafeArray<Math::Coord2DDbl> pointList;
			Math::Coord2DDbl initPt;
			NN<Math::Geometry::LinearRing> lr;
			Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = pg->Iterator();
			while (it.HasNext())
			{
				lr = it.Next();
				pointList = lr->GetPointList(nPoint);
				json->ArrayBeginArray();
				json->ArrayAddCoord2D(pointList[0]);
				initPt = pointList[0];
				j = 1;
				while (j < nPoint)
				{
					json->ArrayAddCoord2D(pointList[j]);
					j++;
				}
				if (pointList[nPoint - 1] != initPt)
				{
					json->ArrayAddCoord2D(initPt);
				}
				json->ArrayEnd();
			}
		}
		json->ArrayEnd();
		return true;
	case Math::Geometry::Vector2D::VectorType::LineString:
		json->ObjectAddStr(CSTR("type"), CSTR("LineString"));
		json->ObjectBeginArray(CSTR("coordinates"));
		{
			NN<Math::Geometry::LineString> lineString = NN<Math::Geometry::LineString>::ConvertFrom(vec);
			UIntOS nPoint;
			UnsafeArray<Math::Coord2DDbl> pointList = lineString->GetPointList(nPoint);
			UIntOS i = 0;
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
			NN<Math::Geometry::LineString> lineString;
			UnsafeArray<Math::Coord2DDbl> pointList;
			UIntOS k;
			UIntOS l;
			Data::ArrayIterator<NN<Math::Geometry::LineString>> it = pl->Iterator();
			while (it.HasNext())
			{
				lineString = it.Next();
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
