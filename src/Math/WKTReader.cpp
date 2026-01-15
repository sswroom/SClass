#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListNative.hpp"
#include "Math/WKTReader.h"
#include "Math/Geometry/CircularString.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/MultiSurface.h"
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
	UIntOS len = (UIntOS)(wkt - sptr);
	if (len > 100 || len == 0)
	{
		return nullptr;
	}
	Text::StrConcatC(sbuff, sptr, len);
	if (Text::StrToDouble(sbuff, val))
	{
		return wkt;
	}
	return nullptr;
}

void Math::WKTReader::SetLastError(UnsafeArrayOpt<const UTF8Char> lastError)
{
	SDEL_TEXT(this->lastError);
	this->lastError = Text::StrSCopyNew(lastError);
}

Optional<Math::Geometry::Point> Math::WKTReader::ParsePoint(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd)
{
	Double x;
	Double y;
	Double z;
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		return nullptr;
	}
	wkt++;
	if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
	{
		return nullptr;
	}
	while (*++wkt == ' ');
	if (!NextDouble(wkt, y).SetTo(wkt))
	{
		return nullptr;
	}
	if (wkt[0] == ')')
	{
		Math::Geometry::Point *pt;
		NEW_CLASS(pt, Math::Geometry::Point(this->srid, x, y));
		wktEnd.Set(wkt + 1);
		return pt;
	}
	else if (wkt[0] != ' ')
	{
		return nullptr;
	}
	while (*++wkt == ' ');
	if (!NextDouble(wkt, z).SetTo(wkt))
	{
		return nullptr;
	}
	if (wkt[0] == ')' && wkt[1] == 0)
	{
		Math::Geometry::PointZ *pt;
		NEW_CLASS(pt, Math::Geometry::PointZ(this->srid, x, y, z));
		wktEnd.Set(wkt + 1);
		return pt;
	}
	return nullptr;
}

Optional<Math::Geometry::LineString> Math::WKTReader::ParseLineString(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd, Bool curve)
{
	Data::ArrayListNative<Double> ptList;
	Data::ArrayListNative<Double> zList;
	Double x;
	Double y;
	Double z;
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		return nullptr;
	}
	while (true)
	{
		while (*++wkt == ' ');
		if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
		{
			return nullptr;
		}
		while (*++wkt == ' ');
		if (!NextDouble(wkt, y).SetTo(wkt))
		{
			return nullptr;
		}
		while (*wkt == ' ')
		{
			while (*++wkt == ' ');
			if (!NextDouble(wkt, z).SetTo(wkt))
			{
				return nullptr;
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
			return nullptr;
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
	if (curve)
	{
		NEW_CLASS(pl, Math::Geometry::CircularString(this->srid, ptList.GetCount() >> 1, hasZ, hasM));
	}
	else
	{
		NEW_CLASS(pl, Math::Geometry::LineString(this->srid, ptList.GetCount() >> 1, hasZ, hasM));
	}
	UIntOS i;
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
	wktEnd.Set(wkt);
	return pl;
}

Optional<Math::Geometry::LinearRing> Math::WKTReader::ParseLinearRing(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd)
{
	Data::ArrayListNative<Double> ptList;
	Data::ArrayListNative<Double> zList;
	Double x;
	Double y;
	Double z;
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		return nullptr;
	}
	while (true)
	{
		while (*++wkt == ' ');
		if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
		{
			return nullptr;
		}
		while (*++wkt == ' ');
		if (!NextDouble(wkt, y).SetTo(wkt))
		{
			return nullptr;
		}
		while (*wkt == ' ')
		{
			while (*++wkt == ' ');
			if (!NextDouble(wkt, z).SetTo(wkt))
			{
				return nullptr;
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
			return nullptr;
		}
	}
	Math::Geometry::LinearRing *lr;
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
	NEW_CLASS(lr, Math::Geometry::LinearRing(this->srid, ptList.GetCount() >> 1, hasZ, hasM));
	UIntOS i;
	UnsafeArray<Math::Coord2DDbl> ptArr = lr->GetPointList(i);
	MemCopyNO(ptArr.Ptr(), ptList.Arr().Ptr(), ptList.GetCount() * sizeof(Double));
	UnsafeArray<Double> zArr;
	UnsafeArray<Double> mArr;
	if (hasM && lr->GetZList(i).SetTo(zArr) && lr->GetMList(i).SetTo(mArr))
	{
		while (i-- > 0)
		{
			zArr[i] = zList.GetItem(i << 1);
			mArr[i] = zList.GetItem((i << 1) + 1);
		}
	}
	else if (hasZ && lr->GetZList(i).SetTo(zArr))
	{
		while (i-- > 0)
		{
			zArr[i] = zList.GetItem(i);
		}
	}
	wktEnd.Set(wkt);
	return lr;
}

Optional<Math::Geometry::CompoundCurve> Math::WKTReader::ParseCompoundCurve(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd)
{
	NN<Math::Geometry::CompoundCurve> cc;
	NEW_CLASSNN(cc, Math::Geometry::CompoundCurve(this->srid));
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		cc.Delete();
		return nullptr;
	}
	while (true)
	{
		while (*++wkt == ' ');
		if (Text::StrStartsWith(wkt, U8STR("CIRCULARSTRING")))
		{
			wkt += 14;
			while (*wkt == ' ') wkt++;
			NN<Math::Geometry::LineString> ls;
			if (!this->ParseLineString(wkt, wkt, true).SetTo(ls))
			{
				cc.Delete();
				return nullptr;
			}
			cc->AddGeometry(ls);
		}
		else
		{
			if (*wkt != '(')
			{
				cc.Delete();
				return nullptr;
			}
			NN<Math::Geometry::LineString> ls;
			if (!this->ParseLineString(wkt, wkt, false).SetTo(ls))
			{
				cc.Delete();
				return nullptr;
			}
			cc->AddGeometry(ls);
		}
		if (*wkt == ')')
		{
			wktEnd.Set(wkt + 1);
			return cc;
		}
		else if (wkt[0] != ',')
		{
			cc.Delete();
			return nullptr;
		}
	}
}

Optional<Math::Geometry::Polygon> Math::WKTReader::ParsePolygon(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd)
{
	NN<Math::Geometry::Polygon> pg;
	NN<Math::Geometry::LinearRing> lr;
	NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid));
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		pg.Delete();
		return nullptr;
	}
	while (true)
	{
		while (*++wkt == ' ');
		if (*wkt != '(')
		{
			pg.Delete();
			return nullptr;
		}
		if (!this->ParseLinearRing(wkt, wkt).SetTo(lr))
		{
			pg.Delete();
			return nullptr;
		}
		pg->AddGeometry(lr);
		if (*wkt == ')')
		{
			wktEnd.Set(wkt + 1);
			return pg;
		}
		else if (wkt[0] != ',')
		{
			pg.Delete();
			return nullptr;
		}
	}
}

Optional<Math::Geometry::CurvePolygon> Math::WKTReader::ParseCurvePolygon(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd)
{
	NN<Math::Geometry::CurvePolygon> cpg;
	NEW_CLASSNN(cpg, Math::Geometry::CurvePolygon(this->srid));
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		cpg.Delete();
		return nullptr;
	}
	while (true)
	{
		while (*++wkt == ' ');
		if (Text::StrStartsWith(wkt, U8STR("COMPOUNDCURVE")))
		{
			wkt += 13;
			while (*wkt == ' ') wkt++;
			NN<Math::Geometry::CompoundCurve> cc;
			if (!this->ParseCompoundCurve(wkt, wkt).SetTo(cc))
			{
				cpg.Delete();
				return nullptr;
			}
			cpg->AddGeometry(cc);
		}
		else
		{
			if (*wkt != '(')
			{
				cpg.Delete();
				return nullptr;
			}
			NN<Math::Geometry::LinearRing> lr;
			if (!this->ParseLinearRing(wkt, wkt).SetTo(lr))
			{
				cpg.Delete();
				return nullptr;
			}
			cpg->AddGeometry(lr);
		}
		if (*wkt == ')')
		{
			wktEnd.Set(wkt + 1);
			return cpg;
		}
		else if (wkt[0] != ',')
		{
			cpg.Delete();
			return nullptr;
		}
	}
}

Optional<Math::Geometry::MultiPolygon> Math::WKTReader::ParseMultiPolygon(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd)
{
	NN<Math::Geometry::MultiPolygon> mpg;
	NN<Math::Geometry::Polygon> pg;
	NEW_CLASSNN(mpg, Math::Geometry::MultiPolygon(this->srid));
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		mpg.Delete();
		return nullptr;
	}
	while (true)
	{
		while (*++wkt == ' ');
		if (*wkt != '(')
		{
			mpg.Delete();
			return nullptr;
		}
		if (!this->ParsePolygon(wkt, wkt).SetTo(pg))
		{
			mpg.Delete();
			return nullptr;
		}
		mpg->AddGeometry(pg);
		if (*wkt == ')')
		{
			wktEnd.Set(wkt + 1);
			return mpg;
		}
		else if (wkt[0] != ',')
		{
			mpg.Delete();
			return nullptr;
		}
	}
}

Optional<Math::Geometry::MultiSurface> Math::WKTReader::ParseMultiSurface(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd)
{
	NN<Math::Geometry::MultiSurface> ms;
	NEW_CLASSNN(ms, Math::Geometry::MultiSurface(this->srid));
	while (*wkt == ' ')
	{
		wkt++;
	}
	if (*wkt != '(')
	{
		ms.Delete();
		return nullptr;
	}
	while (true)
	{
		Bool curve = false;
		while (*++wkt == ' ');
		if (Text::StrStartsWith(wkt, U8STR("CURVEPOLYGON")))
		{
			curve = true;
			wkt += 12;
			while (*wkt == ' ') wkt++;
		}
		if (*wkt != '(')
		{
			ms.Delete();
			return nullptr;
		}
		if (curve)
		{
			NN<Math::Geometry::CurvePolygon> cpg;
			if (!this->ParseCurvePolygon(wkt, wkt).SetTo(cpg))
			{
				ms.Delete();
				return nullptr;
			}
			ms->AddGeometry(cpg);
		}
		else
		{
			NN<Math::Geometry::Polygon> pg;
			if (!this->ParsePolygon(wkt, wkt).SetTo(pg))
			{
				ms.Delete();
				return nullptr;
			}
			ms->AddGeometry(pg);
		}
		if (*wkt == ')')
		{
			wktEnd.Set(wkt + 1);
			return ms;
		}
		else if (wkt[0] != ',')
		{
			ms.Delete();
			return nullptr;
		}
	}
}

Math::WKTReader::WKTReader(UInt32 srid)
{
	this->srid = srid;
	this->lastError = nullptr;
}

Math::WKTReader::~WKTReader()
{
	SDEL_TEXT(this->lastError);
}

Optional<Math::Geometry::Vector2D> Math::WKTReader::ParseWKT(UnsafeArray<const UTF8Char> wkt)
{
	if (Text::StrStartsWith(wkt, (const UTF8Char*)"POINT"))
	{
		wkt += 5;
		NN<Math::Geometry::Point> vec;
		if (!this->ParsePoint(wkt, wkt).SetTo(vec))
			return nullptr;
		if (wkt[0] == 0)
			return vec;
		vec.Delete();
		return nullptr;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"LINESTRING"))
	{
		wkt += 10;
		NN<Math::Geometry::LineString> vec;
		if (!this->ParseLineString(wkt, wkt, false).SetTo(vec))
			return nullptr;
		if (wkt[0] == 0)
			return vec;
		vec.Delete();
		return nullptr;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"POLYGON"))
	{
		wkt += 7;
		NN<Math::Geometry::Polygon> vec;
		if (!this->ParsePolygon(wkt, wkt).SetTo(vec))
			return nullptr;
		if (wkt[0] == 0)
			return vec;
		vec.Delete();
		return nullptr;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"MULTILINESTRING"))
	{
		Data::ArrayListNative<Double> ptList;
		Data::ArrayListNative<Double> zList;
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
			return nullptr;
		}
		NEW_CLASS(pl, Math::Geometry::Polyline(this->srid));
		while (true)
		{
			while (*++wkt == ' ');
			if (*wkt != '(')
			{
				DEL_CLASS(pl);
				return nullptr;
			}
			ptList.Clear();
			zList.Clear();
			while (true)
			{
				while (*++wkt == ' ');
				if (!NextDouble(wkt, x).SetTo(wkt) || *wkt != ' ')
				{
					DEL_CLASS(pl);
					return nullptr;
				}
				while (*++wkt == ' ');
				if (!NextDouble(wkt, y).SetTo(wkt))
				{
					DEL_CLASS(pl);
					return nullptr;
				}
				while (*wkt == ' ')
				{
					while (*++wkt == ' ');
					if (!NextDouble(wkt, z).SetTo(wkt))
					{
						DEL_CLASS(pl);
						return nullptr;
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
					return nullptr;
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
			UIntOS i;
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
				return nullptr;
			}
		}
		if (*wkt != 0)
		{
			DEL_CLASS(pl);
			return nullptr;
		}
		return pl;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"MULTIPOLYGON"))
	{
		wkt += 12;
		NN<Math::Geometry::MultiPolygon> vec;
		if (!this->ParseMultiPolygon(wkt, wkt).SetTo(vec))
			return nullptr;
		if (wkt[0] == 0)
			return vec;
		vec.Delete();
		return nullptr;
	}
	else if (Text::StrStartsWith(wkt, (const UTF8Char*)"MULTISURFACE"))
	{
		wkt += 12;
		NN<Math::Geometry::MultiSurface> vec;
		if (!this->ParseMultiSurface(wkt, wkt).SetTo(vec))
			return nullptr;
		if (wkt[0] == 0)
			return vec;
		vec.Delete();
		return nullptr;
	}
	return nullptr;
}

UnsafeArrayOpt<const UTF8Char> Math::WKTReader::GetLastError()
{
	return this->lastError;
}
