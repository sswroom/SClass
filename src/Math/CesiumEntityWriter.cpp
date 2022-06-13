#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Point3D.h"
#include "Math/Polygon.h"
#include "Math/Polyline.h"
#include "Math/CesiumEntityWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::CesiumEntityWriter::SetLastError(Text::CString lastError)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(lastError);
}

Math::CesiumEntityWriter::CesiumEntityWriter()
{
	this->lastError = 0;
}

Math::CesiumEntityWriter::~CesiumEntityWriter()
{
	SDEL_STRING(this->lastError);
}

Text::CString Math::CesiumEntityWriter::GetWriterName()
{
	return CSTR("Cesium Entity");
}

Bool Math::CesiumEntityWriter::ToText(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec)
{
	if (vec == 0)
	{
		this->SetLastError(CSTR("Input vector is null"));
		return false;
	}
	switch (vec->GetVectorType())
	{
	case Math::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("{\r\n"));
		if (vec->Support3D())
		{
			Math::Point3D *pt = (Math::Point3D*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetCenter3D(&x, &y, &z);
			sb->AppendC(UTF8STRC("position: Cesium.Cartesian3.fromDegrees("));
			sb->AppendDouble(x);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(y);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(z);
			sb->AppendUTF8Char(')');
		}
		else
		{
			Math::Point *pt = (Math::Point*)vec;
			Math::Coord2DDbl coord;
			coord = pt->GetCenter();
			sb->AppendC(UTF8STRC("position: Cesium.Cartesian3.fromDegrees("));
			sb->AppendDouble(coord.x);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(coord.y);
			sb->AppendC(UTF8STRC(", 0)"));
		}
		sb->AppendC(UTF8STRC("\r\n}"));
		return true;
	case Math::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("{\r\n"));
		{
			Math::Polygon *pg = (Math::Polygon*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			Math::Coord2DDbl *pointList = pg->GetPointList(&nPoint);
			UOSInt k;
			sb->AppendC(UTF8STRC("\tpolygon : {\r\n"));
			sb->AppendC(UTF8STRC("\t\thierarchy : Cesium.Cartesian3.fromDegreesArray([\r\n"));
			k = 0;
			while (k < nPoint)
			{
				sb->AppendC(UTF8STRC("\t\t\t"));
				sb->AppendDouble(pointList[k].x);
				sb->AppendUTF8Char(',');
				sb->AppendDouble(pointList[k].y);
				sb->AppendC(UTF8STRC(",\r\n"));
				k++;
			}
			sb->AppendC(UTF8STRC("\t\t]),\r\n"));
			sb->AppendC(UTF8STRC("\t}\r\n"));
			sb->AppendUTF8Char('}');
		}
		return true;
	case Math::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("{\r\n"));
		{
			Math::Polyline *pl = (Math::Polyline*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			Math::Coord2DDbl *pointList = pl->GetPointList(&nPoint);
			UOSInt k;
			sb->AppendC(UTF8STRC("\tpolyline : {\r\n"));
			sb->AppendC(UTF8STRC("\t\tpositions : Cesium.Cartesian3.fromDegreesArray([\r\n"));
			k = 0;
			while (k < nPoint)
			{
				sb->AppendC(UTF8STRC("\t\t\t"));
				sb->AppendDouble(pointList[k].x);
				sb->AppendUTF8Char(',');
				sb->AppendDouble(pointList[k].y);
				sb->AppendC(UTF8STRC(",\r\n"));
				k++;
			}
			sb->AppendC(UTF8STRC("\t\t]),\r\n"));
			sb->AppendC(UTF8STRC("\t}\r\n"));
			sb->AppendUTF8Char('}');
		}
		return true;
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

}

Text::String *Math::CesiumEntityWriter::GetLastError()
{
	return this->lastError;
}
