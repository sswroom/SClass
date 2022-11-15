#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/WKTReader.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

const UTF8Char *Math::WKTReader::NextDouble(const UTF8Char *wkt, Double *val)
{
	UTF8Char sbuff[256];
	const UTF8Char *sptr = wkt;
	UTF8Char c;
	while (true)
	{
		c = *wkt;
		if (c == 0)
		{
			break;
		}
		else if (c == ' ')
		{
			break;
		}
		else if (c == ',')
		{
			break;
		}
		else if (c == ')')
		{
			break;
		}
		wkt++;
	}
	UOSInt len = (UOSInt)(wkt - sptr);
	if (len > 100 || len == 0)
	{
		return 0;
	}
	Text::StrConcatC(sbuff, sptr, len);
	if (Text::StrToDouble(sbuff, val))
	{
		return wkt;
	}
	return 0;
}

void Math::WKTReader::SetLastError(const UTF8Char* lastError)
{
	SDEL_TEXT(this->lastError);
	this->lastError = SCOPY_TEXT(lastError);
}

Math::WKTReader::WKTReader(UInt32 srid)
{
	this->srid = srid;
	this->lastError = 0;
}

Math::WKTReader::~WKTReader()
{
	SDEL_TEXT(this->lastError);
}

Math::Geometry::Vector2D *Math::WKTReader::ParseWKT(const UTF8Char *wkt)
{
	if (Text::StrStartsWith(wkt, (const UTF8Char*)"POINT"))
	{
		Double x;
		Double y;
		Double z;
		wkt += 5;
		while (*wkt == ' ')
		{
			wkt++;
		}
		if (*wkt != '(')
		{
			return 0;
		}
		wkt++;
		wkt = NextDouble(wkt, &x);
		if (wkt == 0 || *wkt != ' ')
		{
			return 0;
		}
		while (*++wkt == ' ');
		wkt = NextDouble(wkt, &y);
		if (wkt == 0)
		{
			return 0;
		}
		if (wkt[0] == ')' && wkt[1] == 0)
		{
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::Point(this->srid, x, y));
			return pt;
		}
		else if (wkt[0] != ' ')
		{
			return 0;
		}
		while (*++wkt == ' ');
		wkt = NextDouble(wkt, &z);
		if (wkt == 0)
		{
			return 0;
		}
		if (wkt[0] == ')' && wkt[1] == 0)
		{
			Math::Geometry::PointZ *pt;
			NEW_CLASS(pt, Math::Geometry::PointZ(this->srid, x, y, z));
			return pt;
		}
		return 0;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"POLYGON"))
	{
		Data::ArrayList<Double> ptList;
		Data::ArrayList<Double> zList;
		Data::ArrayList<UInt32> ptOfstList;
		Double x;
		Double y;
		Double z;
		wkt += 7;
		while (*wkt == ' ')
		{
			wkt++;
		}
		if (*wkt != '(')
		{
			return 0;
		}
		while (*++wkt == ' ');
		while (true)
		{
			if (*wkt != '(')
			{
				return 0;
			}
			wkt++;
			ptOfstList.Add((UInt32)(ptList.GetCount() >> 1));
			while (true)
			{
				wkt = NextDouble(wkt, &x);
				if (wkt == 0 || *wkt != ' ')
				{
					return 0;
				}
				while (*++wkt == ' ');
				wkt = NextDouble(wkt, &y);
				if (wkt == 0)
				{
					return 0;
				}
				while (*wkt == ' ')
				{
					while (*++wkt == ' ');
					wkt = NextDouble(wkt, &z);
					if (wkt == 0)
					{
						return 0;
					}
					zList.Add(z);
				}
				ptList.Add(x);
				ptList.Add(y);
				if (*wkt == ')')
				{
					wkt++;
					break;
				}
				else if (*wkt == ',')
				{
					wkt++;
					continue;
				}
				else
				{
					return 0;
				}
			}
			if (*wkt == ',')
			{
				wkt++;
				continue;
			}
			else if (*wkt == ')')
			{
				wkt++;
				break;
			}
			else
			{
				return 0;
			}
		}
		if (*wkt != 0)
		{
			return 0;
		}
		Math::Geometry::Polygon *pg;
		NEW_CLASS(pg, Math::Geometry::Polygon(this->srid, ptOfstList.GetCount(), ptList.GetCount() >> 1, zList.GetCount() == (ptList.GetCount() >> 1), false));
		UOSInt i;
		UInt32 *ptOfstArr = pg->GetPtOfstList(&i);
		MemCopyNO(ptOfstArr, ptOfstList.GetArray(&i), ptOfstList.GetCount() * sizeof(UInt32));
		Math::Coord2DDbl *ptArr = pg->GetPointList(&i);
		MemCopyNO(ptArr, ptList.GetArray(&i), ptList.GetCount() * sizeof(Double));
		if (pg->HasZ())
		{
			Double *zArr = pg->GetZList(&i);
			while (i-- > 0)
			{
				zArr[i] = zList.GetItem(i);
			}
		}
		return pg;
	}
	return 0;
}

const UTF8Char *Math::WKTReader::GetLastError()
{
	return this->lastError;
}
