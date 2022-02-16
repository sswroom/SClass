#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Point3D.h"
#include "Math/Polygon.h"
#include "Math/Polyline.h"
#include "Math/WKTWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void Math::WKTWriter::SetLastError(const UTF8Char *lastError)
{
	SDEL_TEXT(this->lastError);
	this->lastError = Text::StrCopyNew(lastError);
}

Math::WKTWriter::WKTWriter()
{
	this->lastError = 0;
}

Math::WKTWriter::~WKTWriter()
{
	SDEL_TEXT(this->lastError);
}

Bool Math::WKTWriter::GenerateWKT(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec)
{
	if (vec == 0)
	{
		this->SetLastError((const UTF8Char*)"Input vector is null");
		return false;
	}
	switch (vec->GetVectorType())
	{
	case Math::Vector2D::VectorType::Point:
		sb->AppendC(UTF8STRC("POINT("));
		if (vec->Support3D())
		{
			Math::Point3D *pt = (Math::Point3D*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetCenter3D(&x, &y, &z);
			sb->AppendDouble(x);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(y);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(z);
		}
		else
		{
			Math::Point *pt = (Math::Point*)vec;
			Double x;
			Double y;
			pt->GetCenter(&x, &y);
			sb->AppendDouble(x);
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(y);
		}
		sb->AppendC(UTF8STRC(")"));
		return true;
	case Math::Vector2D::VectorType::Polygon:
		sb->AppendC(UTF8STRC("POLYGON("));
		{
			Math::Polygon *pg = (Math::Polygon*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pg->GetPtOfstList(&nPtOfst);
			Double *pointList = pg->GetPointList(&nPoint);
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
					sb->AppendDouble(pointList[k * 2]);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[k * 2 + 1]);
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
				sb->AppendDouble(pointList[k * 2]);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k * 2 + 1]);
				k++;
				if (k < nPoint)
				{
					sb->AppendUTF8Char(',');
				}
			}
			sb->AppendUTF8Char(')');
		}
		sb->AppendC(UTF8STRC(")"));
		return true;
	case Math::Vector2D::VectorType::Polyline:
		sb->AppendC(UTF8STRC("POLYLINE("));
		{
			Math::Polyline *pl = (Math::Polyline*)vec;
			UOSInt nPtOfst;
			UOSInt nPoint;
			UInt32 *ptOfstList = pl->GetPtOfstList(&nPtOfst);
			Double *pointList = pl->GetPointList(&nPoint);
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
					sb->AppendDouble(pointList[k * 2]);
					sb->AppendUTF8Char(' ');
					sb->AppendDouble(pointList[k * 2 + 1]);
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
				sb->AppendDouble(pointList[k * 2]);
				sb->AppendUTF8Char(' ');
				sb->AppendDouble(pointList[k * 2 + 1]);
				k++;
				if (k < nPoint)
				{
					sb->AppendUTF8Char(',');
				}
			}
			sb->AppendUTF8Char(')');
		}
		sb->AppendC(UTF8STRC(")"));
		return true;
	case Math::Vector2D::VectorType::Multipoint:
	case Math::Vector2D::VectorType::Image:
	case Math::Vector2D::VectorType::String:
	case Math::Vector2D::VectorType::Ellipse:
	case Math::Vector2D::VectorType::PieArea:
	case Math::Vector2D::VectorType::Unknown:
	default:
		this->SetLastError((const UTF8Char*)"Unsupported vector type");
		return false;
	}

}

const UTF8Char *Math::WKTWriter::GetLastError()
{
	return this->lastError;
}
