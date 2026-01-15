#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Map/SHPUtil.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointM.h"
#include "Math/Geometry/PointZM.h"

Optional<Math::Geometry::Vector2D> Map::SHPUtil::ParseShpRecord(UInt32 srid, UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	if (buffSize < 4)
		return nullptr;
	Int32 shpType = ReadInt32(&buff[0]);
	switch (shpType)
	{
	case 0: //Null
		return nullptr;
	case 1: //Point
		if (buffSize >= 20)
		{
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::Point(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12])));
			return pt;
		}
		return nullptr;
	case 3: //Polyline
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polyline *pl;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 16)
			{
				UIntOS i;
				UIntOS j;
				UIntOS k;
				UInt32 *ptOfsts;
				UnsafeArray<Math::Coord2DDbl> points;
				NN<Math::Geometry::LineString> lineString;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(&buff[0]);
					buff += 4;
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (i + 1 == nPtOfst)
					{
						k = nPoint - ptOfsts[i];
					}
					else
					{
						k = ptOfsts[i + 1] - ptOfsts[i];
					}
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k, false, false));
					points = lineString->GetPointList(j);
					j = 0;
					while (j < k)
					{
						points[j].x = ReadDouble(&buff[0]);
						points[j].y = ReadDouble(&buff[8]);
						buff += 16;
						j++;
					}
					pl->AddGeometry(lineString);
					i++;
				}
				MemFree(ptOfsts);
				return pl;
			}
		}
		return nullptr;
	case 5: //Polygon
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pg;
			NN<Math::Geometry::LinearRing> lr;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 16)
			{
				UIntOS tmpV;
				UnsafeArray<const UInt8> ptOfsts;
				UnsafeArray<Math::Coord2DDbl> points;
				UIntOS i = 0;
				UIntOS j = 0;
				UIntOS k;
				buff += 44;
				ptOfsts = buff;
				buff += nPtOfst * 4;
				NEW_CLASS(pg, Math::Geometry::Polygon(srid));
				while (i < nPtOfst)
				{
					i++;
					if (i >= nPtOfst)
						k = nPoint;
					else
						k = ReadUInt32(&ptOfsts[i * 4]);
					NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, (k - j), false, false));
					points = lr->GetPointList(tmpV);
					while (tmpV-- > 0)
					{
						points[0].x = ReadDouble(&buff[0]);
						points[0].y = ReadDouble(&buff[8]);
						points++;
						buff += 16;
					}
					j = k;
					pg->AddGeometry(lr);
				}
				return pg;
			}
		}
		return nullptr;
	case 8: //Multipoint
		if (false)
		{
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			numPoints = ReadInt32(&buff[36]);
			if (buffSize >= 40 + numPoints * 16)
			{
				ptX = ReadDouble(&buff[40]);
				ptY = ReadDouble(&buff[48]);
			}
			*/
		}
		return nullptr;
	case 9: //PointZ
		if (buffSize >= 28)
		{
			Math::Geometry::PointZ *pt;
			NEW_CLASS(pt, Math::Geometry::PointZ(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20])));
			return pt;
		}
		return nullptr;
	case 10: //PolylineZ
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polyline *pl;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 24)
			{
				UIntOS i;
				UIntOS j;
				UIntOS k;
				UInt32 *ptOfsts;
				NN<Math::Geometry::LineString> lineString;
				UnsafeArray<Math::Coord2DDbl> points;
				UnsafeArray<Double> alts;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(&buff[0]);
					buff += 4;
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (i + 1 == nPtOfst)
					{
						k = nPoint - ptOfsts[i];
					}
					else
					{
						k = ptOfsts[i + 1] - ptOfsts[i];
					}
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k, true, false));
					points = lineString->GetPointList(j);
					j = 0;
					while (j < k)
					{
						points[j].x = ReadDouble(&buff[0]);
						points[j].y = ReadDouble(&buff[8]);
						buff += 16;
						j++;
					}
					pl->AddGeometry(lineString);
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (pl->GetItem(i).SetTo(lineString) && lineString->GetZList(k).SetTo(alts))
					{
						j = 0;
						while (j < k)
						{
							alts[j] = ReadDouble(&buff[0]);
							buff += 8;
							j++;
						}
					}
					i++;
				}
				MemFree(ptOfsts);
				return pl;
			}
		}
		return nullptr;
	case 11: //PointZM
		if (buffSize >= 36)
		{
			Math::Geometry::PointZM *pt;
			NEW_CLASS(pt, Math::Geometry::PointZM(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20]), ReadDouble(&buff[28])));
			return pt;
		}
		return nullptr;
	case 13: //PolylineZM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polyline *pl;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 32)
			{
				UIntOS i;
				UIntOS j;
				UIntOS k;
				UInt32 *ptOfsts;
				NN<Math::Geometry::LineString> lineString;
				UnsafeArray<Math::Coord2DDbl> points;
				UnsafeArray<Double> dArr;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(&buff[0]);
					buff += 4;
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (i + 1 == nPtOfst)
					{
						k = nPoint - ptOfsts[i];
					}
					else
					{
						k = ptOfsts[i + 1] - ptOfsts[i];
					}
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k, true, true));
					points = lineString->GetPointList(j);
					j = 0;
					while (j < k)
					{
						points[j].x = ReadDouble(&buff[0]);
						points[j].y = ReadDouble(&buff[8]);
						buff += 16;
						j++;
					}
					pl->AddGeometry(lineString);
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (pl->GetItem(i).SetTo(lineString) && lineString->GetZList(k).SetTo(dArr))
					{
						j = 0;
						while (j < k)
						{
							dArr[j] = ReadDouble(&buff[0]);
							buff += 8;
							j++;
						}
					}
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (pl->GetItem(i).SetTo(lineString) && lineString->GetMList(k).SetTo(dArr))
					{
						j = 0;
						while (j < k)
						{
							dArr[j] = ReadDouble(&buff[0]);
							buff += 8;
							j++;
						}
					}
					i++;
				}
				MemFree(ptOfsts);
				return pl;
			}
		}
		return nullptr;
	case 15: //PolygonZM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pg;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 16)
			{
				UIntOS i;
				UInt32 *ptOfsts = MemAlloc(UInt32, nPtOfst);
				Math::Coord2DDbl *points = MemAllocA(Math::Coord2DDbl, nPoint);
				Double *zArr = MemAlloc(Double, nPoint);
				Double *mArr = MemAlloc(Double, nPoint);
				NEW_CLASS(pg, Math::Geometry::Polygon(srid));
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(&buff[0]);
					buff += 4;
					i++;
				}
				i = 0;
				while (i < nPoint)
				{
					points[i].x = ReadDouble(&buff[0]);
					points[i].y = ReadDouble(&buff[8]);
					buff += 16;
					i++;
				}
				i = 0;
				while (i < nPoint)
				{
					zArr[i] = ReadDouble(&buff[0]);
					buff += 8;
					i++;
				}
				i = 0;
				while (i < nPoint)
				{
					mArr[i] = ReadDouble(&buff[0]);
					buff += 8;
					i++;
				}
				pg->AddFromPtOfst(ptOfsts, nPtOfst, points, nPoint, zArr, mArr);
				MemFree(mArr);
				MemFree(zArr);
				MemFreeA(points);
				MemFree(ptOfsts);
				return pg;
			}
		}
		return nullptr;
	case 18: //MultipointZM
		return nullptr;
	case 19: //PolygonZ
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pg;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 16)
			{
				UIntOS i;
				UInt32 *ptOfsts = MemAlloc(UInt32, nPtOfst);
				Math::Coord2DDbl *points = MemAllocA(Math::Coord2DDbl, nPoint);
				Double *zArr = MemAlloc(Double, nPoint);
				NEW_CLASS(pg, Math::Geometry::Polygon(srid));
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(&buff[0]);
					buff += 4;
					i++;
				}
				i = 0;
				while (i < nPoint)
				{
					points[i].x = ReadDouble(&buff[0]);
					points[i].y = ReadDouble(&buff[8]);
					buff += 16;
					i++;
				}
				i = 0;
				while (i < nPoint)
				{
					zArr[i] = ReadDouble(&buff[0]);
					buff += 8;
					i++;
				}
				pg->AddFromPtOfst(ptOfsts, nPtOfst, points, nPoint, zArr, nullptr);
				MemFree(zArr);
				MemFreeA(points);
				MemFree(ptOfsts);
				return pg;
			}
		}
		return nullptr;
	case 20: //MultipointZ
		return nullptr;
	case 21: //PointM
		if (buffSize >= 28)
		{
			Math::Geometry::PointM *pt;
			NEW_CLASS(pt, Math::Geometry::PointM(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20])));
			// measure = ReadDouble(&buff[20]);
			return pt;
		}
		return nullptr;
	case 23: //PolylineM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polyline *pl;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 16)
			{
				UIntOS i;
				UIntOS j;
				UIntOS k;
				UInt32 *ptOfsts;
				NN<Math::Geometry::LineString> lineString;
				UnsafeArray<Math::Coord2DDbl> points;
				UnsafeArray<Double> mArr;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(&buff[0]);
					buff += 4;
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (i + 1 == nPtOfst)
					{
						k = nPoint - ptOfsts[i];
					}
					else
					{
						k = ptOfsts[i + 1] - ptOfsts[i];
					}
					NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, k, false, true));
					points = lineString->GetPointList(j);
					j = 0;
					while (j < k)
					{
						points[j].x = ReadDouble(&buff[0]);
						points[j].y = ReadDouble(&buff[8]);
						buff += 16;
						j++;
					}
					pl->AddGeometry(lineString);
					i++;
				}
				i = 0;
				while (i < nPtOfst)
				{
					if (pl->GetItem(i).SetTo(lineString) && lineString->GetMList(k).SetTo(mArr))
					{
						j = 0;
						while (j < k)
						{
							mArr[j] = ReadDouble(&buff[0]);
							buff += 8;
							j++;
						}
					}
					i++;
				}
				MemFree(ptOfsts);
				return pl;
			}
		}
		return nullptr;
	case 25: //PolygonM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pg;
			/*
			xMin = ReadDouble(&buff[4]);
			yMin = ReadDouble(&buff[12]);
			xMax = ReadDouble(&buff[20]);
			yMax = ReadDouble(&buff[28]);
			*/
			nPtOfst = ReadUInt32(&buff[36]);
			nPoint = ReadUInt32(&buff[40]);
			if (buffSize >= 44 + nPtOfst * 4 + nPoint * 16)
			{
				UIntOS i;
				UInt32 *ptOfsts = MemAlloc(UInt32, nPtOfst);
				Math::Coord2DDbl *points = MemAllocA(Math::Coord2DDbl, nPoint);
				Double *mArr = MemAlloc(Double, nPoint);
				NEW_CLASS(pg, Math::Geometry::Polygon(srid));
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(&buff[0]);
					buff += 4;
					i++;
				}
				i = 0;
				while (i < nPoint)
				{
					points[i].x = ReadDouble(&buff[0]);
					points[i].y = ReadDouble(&buff[8]);
					buff += 16;
					i++;
				}
				i = 0;
				while (i < nPoint)
				{
					mArr[i] = ReadDouble(&buff[0]);
					buff += 8;
					i++;
				}
				pg->AddFromPtOfst(ptOfsts, nPtOfst, points, nPoint, nullptr, mArr);
				MemFree(mArr);
				MemFreeA(points);
				MemFree(ptOfsts);
				return pg;
			}
		}
		return nullptr;
	case 28: //MultipointM
	case 31: //MultiPatchM
	case 32: //MultiPatch
	case 50: //GeneralPolyline
	case 51: //GeneralPolygon
	case 52: //GeneralPoint
	case 53: //GeneralMultipoint
	case 54: //GeneralMultiPatch
	default:
		return nullptr;
	}
}
