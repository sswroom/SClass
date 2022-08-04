#include "Stdafx.h"
#include "Data/ArrayListA.h"
#include "Data/ByteTool.h"
#include "Math/MultiPolygon.h"
#include "Math/PointZ.h"
#include "Math/Polyline.h"
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

Math::Vector2D *Math::WKBReader::ParseWKB(const UInt8 *wkb, UOSInt wkbLen, UOSInt *sizeUsed)
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
	UOSInt ofst = 5;
	UInt32 geomType = readUInt32(&wkb[1]);
	UInt32 srid = this->srid;
	if (geomType & 0x20000000)
	{
		geomType = geomType & 0xDFFFFFFF;
		srid = readUInt32(&wkb[ofst]);
		ofst += 4;
	}
/*
	0x80000000 = Z
	0x40000000 = M

#define WKB_POINT_TYPE 1
#define WKB_LINESTRING_TYPE 2
#define WKB_POLYGON_TYPE 3
#define WKB_MULTIPOINT_TYPE 4
#define WKB_MULTILINESTRING_TYPE 5
#define WKB_MULTIPOLYGON_TYPE 6
#define WKB_GEOMETRYCOLLECTION_TYPE 7
#define WKB_CIRCULARSTRING_TYPE 8
#define WKB_COMPOUNDCURVE_TYPE 9
#define WKB_CURVEPOLYGON_TYPE 10
#define WKB_MULTICURVE_TYPE 11
#define WKB_MULTISURFACE_TYPE 12
#define WKB_CURVE_TYPE 13
#define WKB_SURFACE_TYPE 14
#define WKB_POLYHEDRALSURFACE_TYPE 15
#define WKB_TIN_TYPE 16
#define WKB_TRIANGLE_TYPE 17
*/
	switch (geomType)
	{
	case 1: //Point
		if (wkbLen < ofst + 16)
			return 0;
		else
		{
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
			if (sizeUsed)
			{
				*sizeUsed = ofst + 16;
			}
			return pt;
		}
	case 1001: //PointZ
	case 0x80000001:
		if (wkbLen < ofst + 24)
			return 0;
		else
		{
			Math::PointZ *pt;
			NEW_CLASS(pt, Math::PointZ(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]), readDouble(&wkb[ofst + 16])));
			if (sizeUsed)
			{
				*sizeUsed = ofst + 24;
			}
			return pt;
		}
	case 2001: //PointM
	case 0x40000001:
		if (wkbLen < ofst + 24)
			return 0;
		else
		{
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
			if (sizeUsed)
			{
				*sizeUsed = ofst + 24;
			}
			return pt;
		}
	case 3001: //PointZM
	case 0xC0000001:
		if (wkbLen < ofst + 32)
			return 0;
		else
		{
			Math::PointZ *pt;
			NEW_CLASS(pt, Math::PointZ(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]), readDouble(&wkb[ofst + 16])));
			if (sizeUsed)
			{
				*sizeUsed = ofst + 32;
			}
			return pt;
		}
	case 2: //LineString
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 16 > wkbLen))
				return 0;
			Math::Polyline *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline(srid, 1, numPoints, false, false));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
				ofst += 16;
				i++;
			}
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pl;
		}
	case 1002: //LineStringZ
	case 0x80000002:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 24 > wkbLen))
				return 0;
			Math::Polyline *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline(srid, 1, numPoints, true, false));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			Double *zArr = pl->GetZList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
				zArr[i] = readDouble(&wkb[ofst + 16]);
				ofst += 24;
				i++;
			}
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pl;
		}
	case 2002: //LineStringM
	case 0x40000002:
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 24 > wkbLen))
				return 0;
			Math::Polyline *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline(srid, 1, numPoints, false, true));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			Double *mArr = pl->GetMList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
				mArr[i] = readDouble(&wkb[ofst + 16]);
				ofst += 24;
				i++;
			}
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pl;
		}
	case 3002: //LineStringZM
	case 0xC0000002:
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 32 > wkbLen))
				return 0;
			Math::Polyline *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Polyline(srid, 1, numPoints, true, true));
			Math::Coord2DDbl *points = pl->GetPointList(&i);
			Double *zArr = pl->GetZList(&i);
			Double *mArr = pl->GetMList(&i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
				zArr[i] = readDouble(&wkb[ofst + 16]);
				mArr[i] = readDouble(&wkb[ofst + 24]);
				ofst += 32;
				i++;
			}
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pl;
		}
	case 3: //Polygon
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[ofst]);
			ofst += 4;
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
			NEW_CLASS(pg, Math::Polygon(srid, numParts, points.GetCount(), false, false));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyAC(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemFree(parts);
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pg;
		}
	case 1003: //PolygonZ
	case 0x80000003:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			Data::ArrayList<Double> zList;
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
					zList.Add(readDouble(&wkb[ofst + 16]));
					ofst += 24;
					j++;
				}
				i++;
			}
			Math::Polygon *pg;
			NEW_CLASS(pg, Math::Polygon(srid, numParts, points.GetCount(), true, false));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			Double *zArr = pg->GetZList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyAC(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemCopyAC(zArr, zList.GetArray(&j), sizeof(Double) * points.GetCount());
			MemFree(parts);
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pg;
		}
	case 2003: //PolygonM
	case 0x40000003:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			Data::ArrayList<Double> mList;
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
					mList.Add(readDouble(&wkb[ofst + 16]));
					ofst += 24;
					j++;
				}
				i++;
			}
			Math::Polygon *pg;
			NEW_CLASS(pg, Math::Polygon(srid, numParts, points.GetCount(), false, true));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			Double *mArr = pg->GetMList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyAC(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemCopyAC(mArr, mList.GetArray(&j), sizeof(Double) * points.GetCount());
			MemFree(parts);
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pg;
		}
	case 3003: //PolygonZM
	case 0xC0000003:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt i;
			UOSInt j;
			Data::ArrayListA<Math::Coord2DDbl> points;
			Data::ArrayList<Double> zList;
			Data::ArrayList<Double> mList;
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
					zList.Add(readDouble(&wkb[ofst + 16]));
					mList.Add(readDouble(&wkb[ofst + 24]));
					ofst += 32;
					j++;
				}
				i++;
			}
			Math::Polygon *pg;
			NEW_CLASS(pg, Math::Polygon(srid, numParts, points.GetCount(), true, true));
			UInt32 *ptOfsts = pg->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pg->GetPointList(&i);
			Double *zArr = pg->GetZList(&i);
			Double *mArr = pg->GetMList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyAC(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemCopyAC(zArr, zList.GetArray(&j), sizeof(Double) * points.GetCount());
			MemCopyAC(mArr, mList.GetArray(&j), sizeof(Double) * points.GetCount());
			MemFree(parts);
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pg;
		}
	case 6: //MultiPolygon
	case 1006: //MultiPolygonZ
	case 2006: //MultiPolygonZ
	case 3006: //MultiPolygonZ
	case 0x80000006:
	case 0x40000006:
	case 0xC0000006:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 nPolygon = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt thisSize;
			UOSInt i;
			Math::Vector2D *vec;
			Math::MultiPolygon *mpg;
			NEW_CLASS(mpg, Math::MultiPolygon(srid));
			i = 0;
			while (i < nPolygon)
			{
				vec = this->ParseWKB(&wkb[ofst], wkbLen - ofst, &thisSize);
				if (vec == 0)
				{
					DEL_CLASS(mpg);
					return 0;
				}
				else if (vec->GetVectorType() != Math::Vector2D::VectorType::Polygon)
				{
					DEL_CLASS(vec);
					DEL_CLASS(mpg);
					return 0;
				}
				else
				{
					mpg->AddGeometry((Math::Polygon*)vec);
					ofst += thisSize;
				}
				i++;
			}
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return mpg;
		}
	case 1009: //CurvePolylineZ
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numParts = readUInt32(&wkb[ofst]);
			ofst += 4;
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
			Math::Polyline *pl;
			NEW_CLASS(pl, Math::Polyline(srid, numParts, points.GetCount(), true, false));
			UInt32 *ptOfsts = pl->GetPtOfstList(&j);
			Math::Coord2DDbl *pointArr = pl->GetPointList(&i);
			Double *zArr = pl->GetZList(&i);
			MemCopyNO(ptOfsts, parts, sizeof(UInt32) * numParts);
			MemCopyAC(pointArr, points.GetArray(&j), sizeof(Math::Coord2DDbl) * points.GetCount());
			MemCopyAC(zArr, altitudes.GetArray(&j), sizeof(Double) * altitudes.GetCount());
			MemFree(parts);
			if (sizeUsed)
			{
				*sizeUsed = ofst;
			}
			return pl;
		}
	case 1010: //CurvePolygonZ
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(wkb, wkbLen, ' ', Text::LineBreakType::CRLF);
			printf("CurvePolygonZ:\r\n%s\r\n", sb.ToString());
			break;
		}
	default:
		{
			printf("WKBReader: Unsupported type: %d\r\n", geomType);
		}
		break;
	}
	return 0;
}
