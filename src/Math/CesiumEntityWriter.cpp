#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/CesiumEntityWriter.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::CesiumEntityWriter::SetLastError(Text::CString lastError)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(lastError).Ptr();
}

Math::CesiumEntityWriter::CesiumEntityWriter()
{
	this->lastError = 0;
}

Math::CesiumEntityWriter::~CesiumEntityWriter()
{
	SDEL_STRING(this->lastError);
}

Text::CStringNN Math::CesiumEntityWriter::GetWriterName() const
{
	return CSTR("Cesium Entity");
}

Bool Math::CesiumEntityWriter::ToText(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("{\r\n"));
		if (vec->HasZ())
		{
			Math::Geometry::PointZ *pt = (Math::Geometry::PointZ*)vec.Ptr();
			Math::Vector3 pos = pt->GetPos3D();
			sb->AppendC(UTF8STRC("position: Cesium.Cartesian3.fromDegrees("));
			sb->AppendDouble(pos.GetX());
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(pos.GetY());
			sb->AppendC(UTF8STRC(", "));
			sb->AppendDouble(pos.GetZ());
			sb->AppendUTF8Char(')');
		}
		else
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec.Ptr();
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
	case Math::Geometry::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("{\r\n"));
		{
			Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec.Ptr();
			UOSInt k;
			UOSInt nPoint;
			Math::Coord2DDbl pt;
			Data::ArrayListA<Math::Coord2DDbl> pointList;
			pg->GetCoordinates(pointList);
			sb->AppendC(UTF8STRC("\tpolygon : {\r\n"));
			sb->AppendC(UTF8STRC("\t\thierarchy : Cesium.Cartesian3.fromDegreesArray([\r\n"));
			k = 0;
			nPoint = pointList.GetCount();
			while (k < nPoint)
			{
				pt = pointList.GetItem(k);
				sb->AppendC(UTF8STRC("\t\t\t"));
				sb->AppendDouble(pt.x);
				sb->AppendUTF8Char(',');
				sb->AppendDouble(pt.y);
				sb->AppendC(UTF8STRC(",\r\n"));
				k++;
			}
			sb->AppendC(UTF8STRC("\t\t]),\r\n"));
			sb->AppendC(UTF8STRC("\t}\r\n"));
			sb->AppendUTF8Char('}');
		}
		return true;
	case Math::Geometry::Vector2D::VectorType::LineString:
		sb->AppendC(UTF8STRC("{\r\n"));
		{
			NotNullPtr<Math::Geometry::LineString> lineString = NotNullPtr<Math::Geometry::LineString>::ConvertFrom(vec);
			UOSInt nPoint;
			Math::Coord2DDbl *pointList = lineString->GetPointList(nPoint);
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
	case Math::Geometry::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("{\r\n"));
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec.Ptr();
			Math::Geometry::LineString *lineString;
			sb->AppendC(UTF8STRC("\tpolyline : {\r\n"));
			sb->AppendC(UTF8STRC("\t\tpositions : Cesium.Cartesian3.fromDegreesArray([\r\n"));
			UOSInt i = 0;
			UOSInt j = pl->GetCount();
			while (i < j)
			{
				lineString = pl->GetItem(i);
				UOSInt nPoint;
				Math::Coord2DDbl *pointList = lineString->GetPointList(nPoint);
				UOSInt k;
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
			}
			sb->AppendC(UTF8STRC("\t\t]),\r\n"));
			sb->AppendC(UTF8STRC("\t}\r\n"));
			sb->AppendUTF8Char('}');
		}
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

Text::String *Math::CesiumEntityWriter::GetLastError()
{
	return this->lastError;
}
