#include "Stdafx.h"
#include "Data/ArrayListA.h"
#include "Data/ByteTool.h"
#include "Math/WKBReader.h"
#include "Math/Geometry/CircularString.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/MultiSurface.h"
#include "Math/Geometry/PointZ.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"

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

Optional<Math::Geometry::Vector2D> Math::WKBReader::ParseWKB(UnsafeArray<const UInt8> wkb, UOSInt wkbLen, OptOut<UOSInt> sizeUsed)
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
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::Point(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
			sizeUsed.Set(ofst + 16);
			return pt;
		}
	case 1001: //PointZ
	case 0x80000001:
		if (wkbLen < ofst + 24)
			return 0;
		else
		{
			Math::Geometry::PointZ *pt;
			NEW_CLASS(pt, Math::Geometry::PointZ(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]), readDouble(&wkb[ofst + 16])));
			sizeUsed.Set(ofst + 24);
			return pt;
		}
	case 2001: //PointM
	case 0x40000001:
		if (wkbLen < ofst + 24)
			return 0;
		else
		{
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::Point(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8])));
			sizeUsed.Set(ofst + 24);
			return pt;
		}
	case 3001: //PointZM
	case 0xC0000001:
		if (wkbLen < ofst + 32)
			return 0;
		else
		{
			Math::Geometry::PointZ *pt;
			NEW_CLASS(pt, Math::Geometry::PointZ(srid, readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]), readDouble(&wkb[ofst + 16])));
			sizeUsed.Set(ofst + 32);
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
			Math::Geometry::LineString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, numPoints, false, false));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
				ofst += 16;
				i++;
			}
			sizeUsed.Set(ofst);
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
			Math::Geometry::LineString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, numPoints, true, false));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			UnsafeArray<Double> zArr;
			if (pl->GetZList(i).SetTo(zArr))
			{
				i = 0;
				while (i < numPoints)
				{
					points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
					zArr[i] = readDouble(&wkb[ofst + 16]);
					ofst += 24;
					i++;
				}
				sizeUsed.Set(ofst);
				return pl;
			}
			else
			{
				DEL_CLASS(pl);
				return 0;
			}
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
			Math::Geometry::LineString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, numPoints, false, true));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			UnsafeArray<Double> mArr;
			if (pl->GetMList(i).SetTo(mArr))
			{
				i = 0;
				while (i < numPoints)
				{
					points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
					mArr[i] = readDouble(&wkb[ofst + 16]);
					ofst += 24;
					i++;
				}
				sizeUsed.Set(ofst);
				return pl;
			}
			else
			{
				DEL_CLASS(pl);
				return 0;
			}
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
			Math::Geometry::LineString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::LineString(srid, numPoints, true, true));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			UnsafeArray<Double> zArr;
			UnsafeArray<Double> mArr;
			if (pl->GetZList(i).SetTo(zArr) && pl->GetMList(i).SetTo(mArr))
			{
				i = 0;
				while (i < numPoints)
				{
					points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
					zArr[i] = readDouble(&wkb[ofst + 16]);
					mArr[i] = readDouble(&wkb[ofst + 24]);
					ofst += 32;
					i++;
				}
				sizeUsed.Set(ofst);
				return pl;
			}
			else
			{
				DEL_CLASS(pl);
				return 0;
			}
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
			if (numParts < 1)
			{
				return 0;
			}
			Math::Geometry::Polygon *pg;
			NEW_CLASS(pg, Math::Geometry::Polygon(srid));
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 16 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UOSInt tmp;
				NN<Math::Geometry::LinearRing> lr;
				NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, numPoints, false, false));
				UnsafeArray<Math::Coord2DDbl> points = lr->GetPointList(tmp);
				j = 0;
				while (j < numPoints)
				{
					points[j] = Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]));
					ofst += 16;
					j++;
				}
				pg->AddGeometry(lr);
				i++;
			}
			sizeUsed.Set(ofst);
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
			if (numParts < 1)
			{
				return 0;
			}
			Math::Geometry::Polygon *pg;
			NEW_CLASS(pg, Math::Geometry::Polygon(srid));
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 24 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UOSInt tmp;
				NN<Math::Geometry::LinearRing> lr;
				NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, numPoints, true, false));
				UnsafeArray<Math::Coord2DDbl> points = lr->GetPointList(tmp);
				UnsafeArray<Double> zList;
				if (lr->GetZList(tmp).SetTo(zList))
				{
					j = 0;
					while (j < numPoints)
					{
						points[j] = Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]));
						zList[j] = readDouble(&wkb[ofst + 16]);
						ofst += 24;
						j++;
					}
					pg->AddGeometry(lr);
				}
				else
				{
					lr.Delete();
				}
				i++;
			}
			sizeUsed.Set(ofst);
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
			if (numParts < 1)
			{
				return 0;
			}
			Math::Geometry::Polygon *pg;
			NEW_CLASS(pg, Math::Geometry::Polygon(srid));
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 24 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UOSInt tmp;
				NN<Math::Geometry::LinearRing> lr;
				NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, numPoints, false, true));
				UnsafeArray<Math::Coord2DDbl> points = lr->GetPointList(tmp);
				UnsafeArray<Double> mList;
				if (lr->GetMList(tmp).SetTo(mList))
				{
					j = 0;
					while (j < numPoints)
					{
						points[j] = Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]));
						mList[j] = readDouble(&wkb[ofst + 16]);
						ofst += 24;
						j++;
					}
					pg->AddGeometry(lr);
				}
				else
				{
					lr.Delete();
				}
				i++;
			}
			sizeUsed.Set(ofst);
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
			if (numParts < 1)
			{
				return 0;
			}
			Math::Geometry::Polygon *pg;
			NEW_CLASS(pg, Math::Geometry::Polygon(srid));
			i = 0;
			while (i < numParts)
			{
				if (ofst + 4 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UInt32 numPoints = readUInt32(&wkb[ofst]);
				ofst += 4;
				if (ofst + numPoints * 32 > wkbLen)
				{
					DEL_CLASS(pg);
					return 0;
				}
				UOSInt tmp;
				NN<Math::Geometry::LinearRing> lr;
				NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, numPoints, true, true))
				UnsafeArray<Math::Coord2DDbl> points = lr->GetPointList(tmp);
				UnsafeArray<Double> zList;
				UnsafeArray<Double> mList;
				if (lr->GetZList(tmp).SetTo(zList) && lr->GetMList(tmp).SetTo(mList))
				{
					j = 0;
					while (j < numPoints)
					{
						points[j] = Math::Coord2DDbl(readDouble(&wkb[ofst]), readDouble(&wkb[ofst + 8]));
						zList[j] = readDouble(&wkb[ofst + 16]);
						mList[j] = readDouble(&wkb[ofst + 24]);
						ofst += 32;
						j++;
					}
					pg->AddGeometry(lr);
				}
				else
				{
					lr.Delete();
				}
				i++;
			}
			sizeUsed.Set(ofst);
			return pg;
		}
	case 6: //MultiPolygon
	case 1006: //MultiPolygonZ
	case 2006: //MultiPolygonM
	case 3006: //MultiPolygonZM
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
			NN<Math::Geometry::Vector2D> vec;
			Math::Geometry::MultiPolygon *mpg;
/*			Bool hasZ;
			Bool hasM;
			if (geomType & 0xC0000000)
			{
				hasZ = (geomType & 0x80000000) != 0;
				hasM = (geomType & 0x40000000) != 0;
			}
			else
			{
				UInt32 t = geomType / 1000;
				hasZ = (t & 1) != 0;
				hasM = (t & 2) != 0;
			}*/
			NEW_CLASS(mpg, Math::Geometry::MultiPolygon(srid));
			i = 0;
			while (i < nPolygon)
			{
				if (!this->ParseWKB(&wkb[ofst], wkbLen - ofst, thisSize).SetTo(vec))
				{
					DEL_CLASS(mpg);
					return 0;
				}
				else if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polygon)
				{
					printf("WKBMultipolygon: wrong type: %d\r\n", (Int32)vec->GetVectorType());
					vec.Delete();
					DEL_CLASS(mpg);
					return 0;
				}
				else
				{
					mpg->AddGeometry(NN<Math::Geometry::Polygon>::ConvertFrom(vec));
					ofst += thisSize;
				}
				i++;
			}
			sizeUsed.Set(ofst);
			return mpg;
		}
	case 7: //GeometryCollection
	case 1007: //GeometryCollectionZ
	case 2007: //GeometryCollectionM
	case 3007: //GeometryCollectionZM
	case 0x80000007:
	case 0x40000007:
	case 0xC0000007:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 nGeometry = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt thisSize;
			UOSInt i;
			NN<Math::Geometry::Vector2D> vec;
			Math::Geometry::GeometryCollection *mpg;
/*			Bool hasZ;
			Bool hasM;
			if (geomType & 0xC0000000)
			{
				hasZ = (geomType & 0x80000000) != 0;
				hasM = (geomType & 0x40000000) != 0;
			}
			else
			{
				UInt32 t = geomType / 1000;
				hasZ = (t & 1) != 0;
				hasM = (t & 2) != 0;
			}*/
			NEW_CLASS(mpg, Math::Geometry::GeometryCollection(srid));
			i = 0;
			while (i < nGeometry)
			{
				if (!this->ParseWKB(&wkb[ofst], wkbLen - ofst, thisSize).SetTo(vec))
				{
					DEL_CLASS(mpg);
					return 0;
				}
				else
				{
					mpg->AddGeometry(vec);
					ofst += thisSize;
				}
				i++;
			}
			sizeUsed.Set(ofst);
			return mpg;
		}
	case 8: //CircularString
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 16 > wkbLen))
				return 0;
			Math::Geometry::CircularString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::CircularString(srid, numPoints, false, false));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			i = 0;
			while (i < numPoints)
			{
				points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
				ofst += 16;
				i++;
			}
			sizeUsed.Set(ofst);
			return pl;
		}
	case 1008: //CircularStringZ
	case 0x80000008:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 24 > wkbLen))
				return 0;
			Math::Geometry::CircularString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::CircularString(srid, numPoints, true, false));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			UnsafeArray<Double> zArr;
			if (pl->GetZList(i).SetTo(zArr))
			{
				i = 0;
				while (i < numPoints)
				{
					points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
					zArr[i] = readDouble(&wkb[ofst + 16]);
					ofst += 24;
					i++;
				}
				sizeUsed.Set(ofst);
				return pl;
			}
			else
			{
				DEL_CLASS(pl);
				return 0;
			}
		}
	case 2008: //CircularStringM
	case 0x40000008:
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 24 > wkbLen))
				return 0;
			Math::Geometry::CircularString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::CircularString(srid, numPoints, false, true));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			UnsafeArray<Double> mArr;
			if (pl->GetMList(i).SetTo(mArr))
			{
				i = 0;
				while (i < numPoints)
				{
					points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
					mArr[i] = readDouble(&wkb[ofst + 16]);
					ofst += 24;
					i++;
				}
				sizeUsed.Set(ofst);
				return pl;
			}
			else
			{
				DEL_CLASS(pl);
				return 0;
			}
		}
	case 3008: //CircularStringZM
	case 0xC0000008:
		if (wkbLen < 9)
			return 0;
		else
		{
			UInt32 numPoints = readUInt32(&wkb[ofst]);
			ofst += 4;
			if (numPoints < 2 || (ofst + numPoints * 32 > wkbLen))
				return 0;
			Math::Geometry::CircularString *pl;
			UOSInt i;
			NEW_CLASS(pl, Math::Geometry::CircularString(srid, numPoints, true, true));
			UnsafeArray<Math::Coord2DDbl> points = pl->GetPointList(i);
			UnsafeArray<Double> zArr;
			UnsafeArray<Double> mArr;
			if (pl->GetZList(i).SetTo(zArr) && pl->GetMList(i).SetTo(mArr))
			{
				i = 0;
				while (i < numPoints)
				{
					points[i] = Math::Coord2DDbl(readDouble(&wkb[ofst + 0]), readDouble(&wkb[ofst + 8]));
					zArr[i] = readDouble(&wkb[ofst + 16]);
					mArr[i] = readDouble(&wkb[ofst + 24]);
					ofst += 32;
					i++;
				}
				sizeUsed.Set(ofst);
				return pl;
			}
			else
			{
				DEL_CLASS(pl);
				return 0;
			}
		}
	case 9: //CompoundCurve
	case 1009: //CompoundCurveZ
	case 2009: //CompoundCurveM
	case 3009: //CompoundCurveZM
	case 0x80000009:
	case 0x40000009:
	case 0xC0000009:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 nPolyline = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt thisSize;
			UOSInt i;
			NN<Math::Geometry::Vector2D> vec;
			Math::Geometry::CompoundCurve *cpl;
/*			Bool hasZ;
			Bool hasM;
			if (geomType & 0xC0000000)
			{
				hasZ = (geomType & 0x80000000) != 0;
				hasM = (geomType & 0x40000000) != 0;
			}
			else
			{
				UInt32 t = geomType / 1000;
				hasZ = (t & 1) != 0;
				hasM = (t & 2) != 0;
			}*/
			NEW_CLASS(cpl, Math::Geometry::CompoundCurve(srid));
			i = 0;
			while (i < nPolyline)
			{
				if (!this->ParseWKB(&wkb[ofst], wkbLen - ofst, thisSize).SetTo(vec))
				{
					DEL_CLASS(cpl);
					return 0;
				}
				else
				{
					Math::Geometry::Vector2D::VectorType t = vec->GetVectorType();
					if (t == Math::Geometry::Vector2D::VectorType::CircularString || t == Math::Geometry::Vector2D::VectorType::LineString)
					{
						cpl->AddGeometry(NN<Math::Geometry::LineString>::ConvertFrom(vec));
						ofst += thisSize;
					}
					else
					{
						printf("WKBCurvePolyline: wrong type: %d\r\n", (Int32)vec->GetVectorType());
						vec.Delete();
						DEL_CLASS(cpl);
						return 0;
					}
				}
				i++;
			}
			sizeUsed.Set(ofst);
			return cpl;
		}
	case 10: //CurvePolygon
	case 1010: //CurvePolygonZ
	case 2010: //CurvePolygonM
	case 3010: //CurvePolygonZM
	case 0x8000000A:
	case 0x4000000A:
	case 0xC000000A:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 nCPolyline = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt thisSize;
			UOSInt i;
			NN<Math::Geometry::Vector2D> vec;
			Math::Geometry::CurvePolygon *cpg;
/*			Bool hasZ;
			Bool hasM;
			if (geomType & 0xC0000000)
			{
				hasZ = (geomType & 0x80000000) != 0;
				hasM = (geomType & 0x40000000) != 0;
			}
			else
			{
				UInt32 t = geomType / 1000;
				hasZ = (t & 1) != 0;
				hasM = (t & 2) != 0;
			}*/
			NEW_CLASS(cpg, Math::Geometry::CurvePolygon(srid));
			i = 0;
			while (i < nCPolyline)
			{
				if (!this->ParseWKB(&wkb[ofst], wkbLen - ofst, thisSize).SetTo(vec))
				{
					DEL_CLASS(cpg);
					return 0;
				}
				else
				{
					Math::Geometry::Vector2D::VectorType t = vec->GetVectorType();
					if (t == Math::Geometry::Vector2D::VectorType::CircularString || t == Math::Geometry::Vector2D::VectorType::CompoundCurve || t == Math::Geometry::Vector2D::VectorType::LineString)
					{
						cpg->AddGeometry(vec);
						ofst += thisSize;
					}
					else
					{
						printf("WKBCurvePolygon: wrong type: %d\r\n", (Int32)vec->GetVectorType());
						vec.Delete();
						DEL_CLASS(cpg);
						return 0;
					}
				}
				i++;
			}
			sizeUsed.Set(ofst);
			return cpg;
		}
	case 12: //MultiSurface
	case 1012: //MultiSurfaceZ
	case 2012: //MultiSurfaceM
	case 3012: //MultiSurfaceZM
	case 0x8000000C:
	case 0x4000000C:
	case 0xC000000C:
		if (wkbLen < ofst + 4)
			return 0;
		else
		{
			UInt32 nCPolyline = readUInt32(&wkb[ofst]);
			ofst += 4;
			UOSInt thisSize;
			UOSInt i;
			NN<Math::Geometry::Vector2D> vec;
			Math::Geometry::MultiSurface *cpg;
/*			Bool hasZ;
			Bool hasM;
			if (geomType & 0xC0000000)
			{
				hasZ = (geomType & 0x80000000) != 0;
				hasM = (geomType & 0x40000000) != 0;
			}
			else
			{
				UInt32 t = geomType / 1000;
				hasZ = (t & 1) != 0;
				hasM = (t & 2) != 0;
			}*/
			NEW_CLASS(cpg, Math::Geometry::MultiSurface(srid));
			i = 0;
			while (i < nCPolyline)
			{
				if (!this->ParseWKB(&wkb[ofst], wkbLen - ofst, thisSize).SetTo(vec))
				{
					DEL_CLASS(cpg);
					return 0;
				}
				else
				{
					Math::Geometry::Vector2D::VectorType t = vec->GetVectorType();
					if (t == Math::Geometry::Vector2D::VectorType::CurvePolygon || t == Math::Geometry::Vector2D::VectorType::Polygon)
					{
						cpg->AddGeometry(vec);
						ofst += thisSize;
					}
					else
					{
						printf("WKBMultiSurface: wrong type: %d\r\n", (Int32)vec->GetVectorType());
						vec.Delete();
						DEL_CLASS(cpg);
						return 0;
					}
				}
				i++;
			}
			sizeUsed.Set(ofst);
			return cpg;
		}
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(wkb, wkbLen, ' ', Text::LineBreakType::CRLF);
			printf("WKBReader: Unsupported type: %d\r\n%s\r\n", geomType, sb.ToPtr());
		}
		break;
	}
	return 0;
}
