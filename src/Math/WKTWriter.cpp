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

Bool Math::WKTWriter::GenerateWKT(Text::StringBuilderUTF *sb, Math::Vector2D *vec)
{
	UTF8Char sbuff[64];
	if (vec == 0)
	{
		this->SetLastError((const UTF8Char*)"Input vector is null");
		return false;
	}
	switch (vec->GetVectorType())
	{
	case Math::Vector2D::VT_POINT:
		sb->Append((const UTF8Char*)"POINT(");
		if (vec->Support3D())
		{
			Math::Point3D *pt = (Math::Point3D*)vec;
			Double x;
			Double y;
			Double z;
			pt->GetCenter3D(&x, &y, &z);
			Text::StrDouble(sbuff, x);
			sb->Append(sbuff);
			sb->AppendChar(' ', 1);
			Text::StrDouble(sbuff, y);
			sb->Append(sbuff);
			sb->AppendChar(' ', 1);
			Text::StrDouble(sbuff, z);
			sb->Append(sbuff);
		}
		else
		{
			Math::Point *pt = (Math::Point*)vec;
			Double x;
			Double y;
			pt->GetCenter(&x, &y);
			Text::StrDouble(sbuff, x);
			sb->Append(sbuff);
			sb->AppendChar(' ', 1);
			Text::StrDouble(sbuff, y);
			sb->Append(sbuff);
		}
		sb->Append((const UTF8Char*)")");
		return true;
	case Math::Vector2D::VT_POLYGON:
		sb->Append((const UTF8Char*)"POLYGON(");
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
				sb->AppendChar('(', 1);
				while (k < ptOfstList[i + 1])
				{
					Text::StrDouble(sbuff, pointList[k * 2]);
					sb->Append(sbuff);
					sb->AppendChar(' ', 1);
					Text::StrDouble(sbuff, pointList[k * 2 + 1]);
					sb->Append(sbuff);
					k++;
					if (k < ptOfstList[i + 1])
					{
						sb->AppendChar(',', 1);
					}
				}
				sb->AppendChar(')', 1);
				sb->AppendChar(',', 1);
				i++;
			}
			sb->AppendChar('(', 1);
			while (k < nPoint)
			{
				Text::StrDouble(sbuff, pointList[k * 2]);
				sb->Append(sbuff);
				sb->AppendChar(' ', 1);
				Text::StrDouble(sbuff, pointList[k * 2 + 1]);
				sb->Append(sbuff);
				k++;
				if (k < nPoint)
				{
					sb->AppendChar(',', 1);
				}
			}
			sb->AppendChar(')', 1);
		}
		sb->Append((const UTF8Char*)")");
		return true;
	case Math::Vector2D::VT_POLYLINE:
		sb->Append((const UTF8Char*)"POLYLINE(");
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
				sb->AppendChar('(', 1);
				while (k < ptOfstList[i + 1])
				{
					Text::StrDouble(sbuff, pointList[k * 2]);
					sb->Append(sbuff);
					sb->AppendChar(' ', 1);
					Text::StrDouble(sbuff, pointList[k * 2 + 1]);
					sb->Append(sbuff);
					k++;
					if (k < ptOfstList[i + 1])
					{
						sb->AppendChar(',', 1);
					}
				}
				sb->AppendChar(')', 1);
				sb->AppendChar(',', 1);
				i++;
			}
			sb->AppendChar('(', 1);
			while (k < nPoint)
			{
				Text::StrDouble(sbuff, pointList[k * 2]);
				sb->Append(sbuff);
				sb->AppendChar(' ', 1);
				Text::StrDouble(sbuff, pointList[k * 2 + 1]);
				sb->Append(sbuff);
				k++;
				if (k < nPoint)
				{
					sb->AppendChar(',', 1);
				}
			}
			sb->AppendChar(')', 1);
		}
		sb->Append((const UTF8Char*)")");
		return true;
	case Math::Vector2D::VT_MULTIPOINT:
	case Math::Vector2D::VT_IMAGE:
	case Math::Vector2D::VT_STRING:
	case Math::Vector2D::VT_ELLIPSE:
	case Math::Vector2D::VT_PIEAREA:
	case Math::Vector2D::VT_UNKNOWN:
	default:
		this->SetLastError((const UTF8Char*)"Unsupported vector type");
		return false;
	}

}

const UTF8Char *Math::WKTWriter::GetLastError()
{
	return this->lastError;
}
