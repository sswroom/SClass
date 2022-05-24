#include "Stdafx.h"
#include "Data/ArrayListA.h"
#include "Data/ByteTool.h"
#include "Math/Point3D.h"
#include "Math/Polyline3D.h"
#include "Math/Polygon.h"
#include "Math/WKBReader.h"

#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

Double Math::WKBReader::NDRReadDouble(const UInt8 *buff)
{
	return ReadDouble(buff);
}

UInt32 Math::WKBReader::NDRReadUInt32(const UInt8 *buff)
{
	return ReadUInt32(buff);
}

Double Math::WKBReader::XDRReadDouble(const UInt8 *buff)
{
	return ReadMDouble(buff);
}

UInt32 Math::WKBReader::XDRReadUInt32(const UInt8 *buff)
{
	return ReadMUInt32(buff);
}

Math::WKBReader::WKBReader(UInt32 srid)
{
	this->srid = srid;
}

Math::WKBReader::~WKBReader()
{

}

Math::Vector2D *Math::WKBReader::ParseWKB(const UInt8 *wkb, UOSInt wkbLen)
{
	if (wkbLen < 5)
	{
		return 0;
	}
	UInt8 byteOrder = wkb[0];
	FReadDouble readDouble;
	FReadUInt32 readUInt32;
	if (byteOrder == 1)
	{
		readDouble = NDRReadDouble;
		readUInt32 = NDRReadUInt32;
	}
	else
	{
		readDouble = XDRReadDouble;
		readUInt32 = XDRReadUInt32;
	}
	UInt32 geomType = readUInt32(&wkb[1]);
	switch (geomType)
	{
	case 1: //Point
		if (wkbLen < 21)
			return 0;
		else
		{
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(this->srid, readDouble(&wkb[5]), readDouble(&wkb[13])));
			return pt;
		}
	case 1001: //PointZ
		if (wkbLen < 29)
			return 0;
		else
		{
			Math::Point3D *pt;
			NEW_CLASS(pt, Math::Point3D(this->srid, readDouble(&wkb[5]), readDouble(&wkb[13]), readDouble(&wkb[21])));
			return pt;
		}
	case 2001: //PointM
		if (wkbLen < 29)
			return 0;
		else
		{
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(this->srid, readDouble(&wkb[5]), readDouble(&wkb[13])));
			return pt;
		}
	case 3001: //PointZM
		if (wkbLen < 37)
			return 0;
		else
		{
			Math::Point3D *pt;
			NEW_CLASS(pt, Math::Point3D(this->srid, readDouble(&wkb[5]), readDouble(&wkb[13]), readDouble(&wkb[21])));
			return pt;
		}
	case 2: //LineString
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[5]);
			if (numPoints < 2 || (9 + numPoints * 16 > wkbLen))
				return 0;
			Math::Polyline *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline(this->srid, 1, numPoints));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[9 + i * 16 + 0]), readDouble(&wkb[9 + i * 16 + 8]));
				i++;
			}
			return pl;
		}
	case 1002: //LineStringZ
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[5]);
			if (numPoints < 2 || (9 + numPoints * 24 > wkbLen))
				return 0;
			Math::Polyline3D *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline3D(this->srid, 1, numPoints));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			Double *alts = pl->GetAltitudeList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[9 + i * 24 + 0]), readDouble(&wkb[9 + i * 24 + 8]));
				alts[i] = readDouble(&wkb[9 + i * 24 + 16]);
				i++;
			}
			return pl;
		}
	case 2002: //LineStringM
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[5]);
			if (numPoints < 2 || (9 + numPoints * 24 > wkbLen))
				return 0;
			Math::Polyline *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline(this->srid, 1, numPoints));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[9 + i * 24 + 0]), readDouble(&wkb[9 + i * 24 + 8]));
				i++;
			}
			return pl;
		}
	case 3002: //LineStringZM
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[5]);
			if (numPoints < 2 || (9 + numPoints * 32 > wkbLen))
				return 0;
			Math::Polyline3D *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline3D(this->srid, 1, numPoints));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			Double *alts = pl->GetAltitudeList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[9 + i * 32 + 0]), readDouble(&wkb[9 + i * 32 + 8]));
				alts[i] = readDouble(&wkb[9 + i * 32 + 16]);
				i++;
			}
			return pl;
		}
	case 3: //Polygon
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[5]);
			UOSInt ofst = 9;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			if (numParts < 1)
			{
				return 0;
			}
			UInt32 *parts = MemAlloc(UInt32, numParts);
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 16 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				parts[i] = (UInt32)points.GetCount();
				j = 0;
				while (j < numPoints)
				{
					points.Add(Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
					ofst += 16;
					j++;
				}
				i++;
			}
			Math::Polygon *pg;
			NEW_CLASS(pg, Math::Polygon(this->srid, numParts, points.GetCount()));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyNO(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemFree(parts);
			return pg;
		}
	case 1003: //PolygonZ
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[5]);
			UOSInt ofst = 9;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			if (numParts < 1)
			{
				return 0;
			}
			UInt32 *parts = MemAlloc(UInt32, numParts);
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 24 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				parts[i] = (UInt32)points.GetCount();
				j = 0;
				while (j < numPoints)
				{
					points.Add(Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
					ofst += 24;
					j++;
				}
				i++;
			}
			Math::Polygon *pg;
			NEW_CLASS(pg, Math::Polygon(this->srid, numParts, points.GetCount()));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyNO(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemFree(parts);
			return pg;
		}
	case 2003: //PolygonM
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[5]);
			UOSInt ofst = 9;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			if (numParts < 1)
			{
				return 0;
			}
			UInt32 *parts = MemAlloc(UInt32, numParts);
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 24 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				parts[i] = (UInt32)points.GetCount();
				j = 0;
				while (j < numPoints)
				{
					points.Add(Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
					ofst += 24;
					j++;
				}
				i++;
			}
			Math::Polygon *pg;
			NEW_CLASS(pg, Math::Polygon(this->srid, numParts, points.GetCount()));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyNO(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemFree(parts);
			return pg;
		}
	case 3003: //PolygonZM
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[5]);
			UOSInt ofst = 9;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			if (numParts < 1)
			{
				return 0;
			}
			UInt32 *parts = MemAlloc(UInt32, numParts);
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 32 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				parts[i] = (UInt32)points.GetCount();
				j = 0;
				while (j < numPoints)
				{
					points.Add(Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
					ofst += 32;
					j++;
				}
				i++;
			}
			Math::Polygon *pg;
			NEW_CLASS(pg, Math::Polygon(this->srid, numParts, points.GetCount()));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyNO(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemFree(parts);
			return pg;
		}
	case 1009: //CurvePolylineZ
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[5]);
			UOSInt ofst = 9;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			Data::ArrayList<Double> altitudes;
			if (numParts < 1)
			{
				return 0;
			}
			UInt32 *parts = MemAlloc(UInt32, numParts);
			i = 0;
			while (i < numParts)
			{
				if (ofst + 9 > wkbLen || wkb[ofst] != wkb[0] || readUInt32(&wkb[ofst + 1]) != 1002)
				{
					MemFree(parts);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst + 5]);
				ofst += 9;
				if (ofst + numPoints * 24 > wkbLen)
				{
					MemFree(parts);
					return 0;
				}
				parts[i] = (UInt32)points.GetCount();
				j = 0;
				while (j < numPoints)
				{
					points.Add(Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
					altitudes.Add(readDouble(&wkb[ofst + 16]));
					ofst += 24;
					j++;
				}
				i++;
			}
			Math::Polyline3D *pl;
			NEW_CLASS(pl, Math::Polyline3D(this->srid, numParts, points.GetCount()));
			UInt32 *ptOfsts = pl->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pl->GetPointList(&i);
			Double *altArr = pl->GetAltitudeList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyNO(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemCopyNO(altArr, altitudes.GetArray(&j), sizeof(Double) * altitudes.GetCount());
			MemFree(parts);
			return pl;
		}
	case 1010: //CurvePolygonZ
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(wkb, wkbLen, ' ', Text::LineBreakType::CRLF);
			printf("CurvePolygonZ:\r\n%s\r\n", sb.ToString());
			break;
		}
	}
	return 0;
}
