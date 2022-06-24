#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/GeoJSONWriter.h"
#include "Math/Point3D.h"
#include "Math/Polygon.h"
#include "Math/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::GeoJSONWriter::SetLastError(Text::CString lastError)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(lastError);
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

Bool Math::GeoJSONWriter::ToText(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec)
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
	case Math::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"type\": \"Point\",\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"coordinates\": ["));
		if (vec->Support3D())
		{
			Math::Point3D *pt = (Math::Point3D*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetCenter3D(&x, &y, &z);
			sb->AppendDouble(x);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(y);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(z);
		}
		else
		{
			Math::Point *pt = (Math::Point*)vec;
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			sb->AppendDouble(coord.x);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(coord.y);
		}
		sb->AppendC(UTF8STRC("]\r\n"));
		sb->AppendC(UTF8STRC("\t}"));
		break;
	case Math::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("\t\"geometry\": {\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"type\": \"Polygon\",\r\n"));
		sb->AppendC(UTF8STRC("\t\t\"coordinates\": [\r\n"));
		{
			Math::Polygon *pg = (Math::Polygon*)vec;
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
	case Math::Vector2D::VectorType::Polyline:
		{
			Math::Polyline *pg = (Math::Polyline*)vec;
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
	case Math::Vector2D::VectorType::Multipoint:
	case Math::Vector2D::VectorType::Image:
	case Math::Vector2D::VectorType::String:
	case Math::Vector2D::VectorType::Ellipse:
	case Math::Vector2D::VectorType::PieArea:
	case Math::Vector2D::VectorType::Unknown:
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
