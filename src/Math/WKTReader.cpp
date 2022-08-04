#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Point.h"
#include "Math/PointZ.h"
#include "Math/WKTReader.h"
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

Math::Vector2D *Math::WKTReader::ParseWKT(const UTF8Char *wkt)
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
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(this->srid, x, y));
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
			Math::PointZ *pt;
			NEW_CLASS(pt, Math::PointZ(this->srid, x, y, z));
			return pt;
		}
		return 0;
	}
	return 0;
}

const UTF8Char *Math::WKTReader::GetLastError()
{
	return this->lastError;
}
