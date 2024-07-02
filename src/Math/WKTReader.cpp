#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/WKTReader.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

UnsafeArrayOpt<const UTF8Char> Math::WKTReader::NextDouble(UnsafeArray<const UTF8Char> wkt, OutParam<Double> val)
{
	UTF8Char sbuff[256];
	UnsafeArray<const UTF8Char> sptr = wkt;
	UTF8Char c;
	if ((wkt[0] == 'i' || wkt[0] == 'I') &&
		(wkt[1] == 'n' || wkt[1] == 'N') &&
		(wkt[2] == 'f' || wkt[2] == 'F'))
	{
		val.Set(Math::GetInfinity());
		return wkt + 3;
	}
	if ((wkt[0] == 'n' || wkt[0] == 'N') &&
		(wkt[1] == 'a' || wkt[1] == 'A') &&
		(wkt[2] == 'n' || wkt[2] == 'N'))
	{
		val.Set(Math::GetQNAN());
		return wkt + 3;
	}
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

Optional<Math::Geometry::Vector2D> Math::WKTReader::ParseWKT(UnsafeArray<const UTF8Char> wkt)
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
		if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
		{
			return 0;
		}
		while (*++wkt == ' ');
		if (!NextDouble(wkt, y).SetTo(wkt))
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
		if (!NextDouble(wkt, z).SetTo(wkt))
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
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"LINESTRING"))
	{
		Data::ArrayList<Double> ptList;
		Data::ArrayList<Double> zList;
		Double x;
		Double y;
		Double z;
		wkt += 10;
		while (*wkt == ' ')
		{
			wkt++;
		}
		if (*wkt != '(')
		{
			return 0;
		}
		while (true)
		{
			while (*++wkt == ' ');
			if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
			{
				return 0;
			}
			while (*++wkt == ' ');
			if (!NextDouble(wkt, y).SetTo(wkt))
			{
				return 0;
			}
			while (*wkt == ' ')
			{
				while (*++wkt == ' ');
				if (!NextDouble(wkt, z).SetTo(wkt))
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
				continue;
			}
			else
			{
				return 0;
			}
		}
		Math::Geometry::LineString *pl;
		Bool hasM = false;
		Bool hasZ = false;
		if (zList.GetCount() == ptList.GetCount())
		{
			hasM = true;
			hasZ = true;
		}
		else if (zList.GetCount() == (ptList.GetCount() >> 1))
		{
			hasZ = true;
		}
		NEW_CLASS(pl, Math::Geometry::LineString(this->srid, ptList.GetCount() >> 1, hasZ, hasM));
		UOSInt i;
		UnsafeArray<Math::Coord2DDbl> ptArr = pl->GetPointList(i);
		MemCopyNO(ptArr.Ptr(), ptList.Arr().Ptr(), ptList.GetCount() * sizeof(Double));
		UnsafeArray<Double> zArr;
		UnsafeArray<Double> mArr;
		if (hasM && pl->GetZList(i).SetTo(zArr) && pl->GetMList(i).SetTo(mArr))
		{
			while (i-- > 0)
			{
				zArr[i] = zList.GetItem(i << 1);
				mArr[i] = zList.GetItem((i << 1) + 1);
			}
		}
		else if (hasZ && pl->GetZList(i).SetTo(zArr))
		{
			while (i-- > 0)
			{
				zArr[i] = zList.GetItem(i);
			}
		}
		return pl;
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
		while (true)
		{
			while (*++wkt == ' ');
			if (*wkt != '(')
			{
				return 0;
			}
			ptOfstList.Add((UInt32)(ptList.GetCount() >> 1));
			while (true)
			{
				while (*++wkt == ' ');
				if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
				{
					return 0;
				}
				while (*++wkt == ' ');
				if (!NextDouble(wkt, y).SetTo(wkt))
				{
					return 0;
				}
				while (*wkt == ' ')
				{
					while (*++wkt == ' ');
					if (!NextDouble(wkt, z).SetTo(wkt))
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
					continue;
				}
				else
				{
					return 0;
				}
			}
			if (*wkt == ',')
			{
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
		NN<Math::Geometry::LinearRing> lr;
		Bool hasM = false;
		Bool hasZ = false;
		if (zList.GetCount() == ptList.GetCount())
		{
			hasM = true;
			hasZ = true;
		}
		else if (zList.GetCount() == (ptList.GetCount() >> 1))
		{
			hasZ = true;
		}
		NEW_CLASS(pg, Math::Geometry::Polygon(this->srid));
		UOSInt i = 0;
		UOSInt j = ptOfstList.GetCount();
		UOSInt k = 0;
		UOSInt l;
		UOSInt m;
		while (i < j)
		{
			i++;
			if (i >= j)
				l = ptList.GetCount();
			else
				l = ptOfstList.GetItem(i) << 1;
			NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (l - k) >> 1, hasZ, hasM));
			UnsafeArray<Math::Coord2DDbl> ptArr = lr->GetPointList(m);
			MemCopyNO(ptArr.Ptr(), ptList.Arr().Ptr() + k, (l - k) * sizeof(Double));
			UnsafeArray<Double> zArr;
			UnsafeArray<Double> mArr;
			if (hasM && lr->GetZList(m).SetTo(zArr) && lr->GetMList(m).SetTo(mArr))
			{
				while (m-- > 0)
				{
					zArr[m] = zList.GetItem(((k >> 1) + m) << 1);
					mArr[m] = zList.GetItem((((k >> 1) + m) << 1) + 1);
				}
			}
			else if (hasZ && lr->GetZList(m).SetTo(zArr))
			{
				while (m-- > 0)
				{
					zArr[m] = zList.GetItem((k >> 1) + m);
				}
			}
			pg->AddGeometry(lr);
			k = l;
		}
		return pg;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"MULTILINESTRING"))
	{
		Data::ArrayList<Double> ptList;
		Data::ArrayList<Double> zList;
		Double x;
		Double y;
		Double z;
		Math::Geometry::Polyline *pl;
		NN<Math::Geometry::LineString> lineString;
		wkt += 15;
		while (*wkt == ' ')
		{
			wkt++;
		}
		if (*wkt != '(')
		{
			return 0;
		}
		NEW_CLASS(pl, Math::Geometry::Polyline(this->srid));
		while (true)
		{
			while (*++wkt == ' ');
			if (*wkt != '(')
			{
				DEL_CLASS(pl);
				return 0;
			}
			ptList.Clear();
			zList.Clear();
			while (true)
			{
				while (*++wkt == ' ');
				if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
				{
					DEL_CLASS(pl);
					return 0;
				}
				while (*++wkt == ' ');
				if (!NextDouble(wkt, y).SetTo(wkt))
				{
					DEL_CLASS(pl);
					return 0;
				}
				while (*wkt == ' ')
				{
					while (*++wkt == ' ');
					if (!NextDouble(wkt, z).SetTo(wkt))
					{
						DEL_CLASS(pl);
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
					continue;
				}
				else
				{
					DEL_CLASS(pl);
					return 0;
				}
			}

			Bool hasZ = false;
			Bool hasM = false;
			if (zList.GetCount() == ptList.GetCount())
			{
				hasZ = true;
				hasM = true;
			}
			else if (zList.GetCount() == (ptList.GetCount() >> 1))
			{
				hasZ = true;
			}
			NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, ptList.GetCount() >> 1, hasZ, hasM));
			UOSInt i;
			UnsafeArray<Math::Coord2DDbl> ptArr = lineString->GetPointList(i);
			MemCopyNO(ptArr.Ptr(), ptList.Arr().Ptr(), ptList.GetCount() * sizeof(Double));
			UnsafeArray<Double> zArr;
			UnsafeArray<Double> mArr;
			if (hasM && lineString->GetZList(i).SetTo(zArr) && lineString->GetMList(i).SetTo(mArr))
			{
				while (i-- > 0)
				{
					zArr[i] = zList.GetItem((i << 1));
					mArr[i] = zList.GetItem((i << 1) + 1);
				}
			}
			else if (hasZ && lineString->GetZList(i).SetTo(zArr))
			{
				while (i-- > 0)
				{
					zArr[i] = zList.GetItem(i);
				}
			}
			pl->AddGeometry(lineString);

			if (*wkt == ',')
			{
				continue;
			}
			else if (*wkt == ')')
			{
				wkt++;
				break;
			}
			else
			{
				DEL_CLASS(pl);
				return 0;
			}
		}
		if (*wkt != 0)
		{
			DEL_CLASS(pl);
			return 0;
		}
		return pl;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"MULTIPOLYGON"))
	{
		Math::Geometry::MultiPolygon *mpg = 0;
		Data::ArrayList<Double> ptList;
		Data::ArrayList<Double> zList;
		Data::ArrayList<UInt32> ptOfstList;
		Double x;
		Double y;
		Double z;
		wkt += 12;
		while (*wkt == ' ')
		{
			wkt++;
		}
		if (*wkt != '(')
		{
			return 0;
		}
		while (true)
		{
			while (*++wkt == ' ');
			ptList.Clear();
			zList.Clear();
			ptOfstList.Clear();
			if (*wkt != '(')
			{
				SDEL_CLASS(mpg);
				return 0;
			}
			while (true)
			{
				while (*++wkt == ' ');
				if (*wkt != '(')
				{
					SDEL_CLASS(mpg);
					return 0;
				}
				ptOfstList.Add((UInt32)(ptList.GetCount() >> 1));
				while (true)
				{
					while (*++wkt == ' ');
					if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
					{
						SDEL_CLASS(mpg);
						return 0;
					}
					while (*++wkt == ' ');
					if (!NextDouble(wkt, y).SetTo(wkt))
					{
						SDEL_CLASS(mpg);
						return 0;
					}
					while (*wkt == ' ')
					{
						while (*++wkt == ' ');
						if (!NextDouble(wkt, z).SetTo(wkt))
						{
							SDEL_CLASS(mpg);
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
						continue;
					}
					else
					{
						SDEL_CLASS(mpg);
						return 0;
					}
				}
				if (*wkt == ',')
				{
					continue;
				}
				else if (*wkt == ')')
				{
					wkt++;
					break;
				}
				else
				{
					SDEL_CLASS(mpg);
					return 0;
				}
			}
			NN<Math::Geometry::Polygon> pg;
			NN<Math::Geometry::LinearRing> lr;
			Bool hasM = false;
			Bool hasZ = false;
			if (zList.GetCount() == ptList.GetCount())
			{
				hasM = true;
				hasZ = true;
			}
			else if (zList.GetCount() == (ptList.GetCount() >> 1))
			{
				hasZ = true;
			}
			NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid));
			UOSInt i = 0;
			UOSInt j = ptOfstList.GetCount();
			UOSInt k = 0;
			UOSInt l;
			UOSInt m;
			while (i < j)
			{
				i++;
				if (i >= j)
					l = ptList.GetCount();
				else
					l = ptOfstList.GetItem(i) << 1;
				NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (l - k) >> 1, hasZ, hasM));
				UnsafeArray<Math::Coord2DDbl> ptArr = lr->GetPointList(m);
				MemCopyNO(ptArr.Ptr(), ptList.Arr().Ptr() + k, (l - k) * sizeof(Double));
				UnsafeArray<Double> zArr;
				UnsafeArray<Double> mArr;
				if (hasM && lr->GetZList(m).SetTo(zArr) && lr->GetMList(m).SetTo(mArr))
				{
					while (m-- > 0)
					{
						zArr[m] = zList.GetItem(((k >> 1) + m) << 1);
						mArr[m] = zList.GetItem((((k >> 1) + m) << 1) + 1);
					}
				}
				else if (hasZ && lr->GetZList(m).SetTo(zArr))
				{
					while (m-- > 0)
					{
						zArr[m] = zList.GetItem((k >> 1) + m);
					}
				}
				pg->AddGeometry(lr);
				k = l;
			}
			if (mpg == 0)
			{
				NEW_CLASS(mpg, Math::Geometry::MultiPolygon(this->srid));
			}
			mpg->AddGeometry(pg);

			if (*wkt == ',')
			{
				continue;
			}
			else if (*wkt == ')')
			{
				wkt++;
				break;
			}
			else
			{
				SDEL_CLASS(mpg);
				return 0;
			}
		}
		if (*wkt != 0)
		{
			SDEL_CLASS(mpg);
			return 0;
		}
		return mpg;
	}
	return 0;
}

UnsafeArrayOpt<const UTF8Char> Math::WKTReader::GetLastError()
{
	return this->lastError;
}
