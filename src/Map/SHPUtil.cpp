#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Map/SHPUtil.h"
#include "Math/Math.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/PointM.h"
#include "Math/Geometry/PointZM.h"

Math::Geometry::Vector2D *Map::SHPUtil::ParseShpRecord(UInt32 srid, const UInt8 *buff, UOSInt buffSize)
{
	if (buffSize < 4)
		return 0;
	Int32 shpType = ReadInt32(buff);
	switch (shpType)
	{
	case 0: //Null
		return 0;
	case 1: //Point
		if (buffSize >= 20)
		{
			Math::Geometry::Point *pt;
			NEW_CLASS(pt, Math::Geometry::Point(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12])));
			return pt;
		}
		return 0;
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
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UInt32 *ptOfsts;
				Math::Coord2DDbl *points;
				NotNullPtr<Math::Geometry::LineString> lineString;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(buff);
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
		return 0;
	case 5: //Polygon
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pl;
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
				UOSInt tmpV;
				UInt32 *ptOfsts;
				Math::Coord2DDbl *points;
				NEW_CLASS(pl, Math::Geometry::Polygon(srid, nPtOfst, nPoint, false, false));
				ptOfsts = pl->GetPtOfstList(tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				return pl;
			}
		}
		return 0;
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
		return 0;
	case 9: //PointZ
		if (buffSize >= 28)
		{
			Math::Geometry::PointZ *pt;
			NEW_CLASS(pt, Math::Geometry::PointZ(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20])));
			return pt;
		}
		return 0;
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
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UInt32 *ptOfsts;
				NotNullPtr<Math::Geometry::LineString> lineString;
				Math::Coord2DDbl *points;
				Double *alts;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(buff);
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
					if (lineString.Set(pl->GetItem(i)))
					{
						alts = lineString->GetZList(k);
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
		return 0;
	case 11: //PointZM
		if (buffSize >= 36)
		{
			Math::Geometry::PointZM *pt;
			NEW_CLASS(pt, Math::Geometry::PointZM(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20]), ReadDouble(&buff[28])));
			return pt;
		}
		return 0;
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
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UInt32 *ptOfsts;
				NotNullPtr<Math::Geometry::LineString> lineString;
				Math::Coord2DDbl *points;
				Double *dArr;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(buff);
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
					if (lineString.Set(pl->GetItem(i)))
					{
						dArr = lineString->GetZList(k);
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
					if (lineString.Set(pl->GetItem(i)))
					{
						dArr = lineString->GetMList(k);
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
		return 0;
	case 15: //PolygonZM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pl;
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
				UOSInt tmpV;
				UInt32 *ptOfsts;
				Math::Coord2DDbl *points;
				Double *zArr;
				Double *mArr;
				NEW_CLASS(pl, Math::Geometry::Polygon(srid, nPtOfst, nPoint, true, true));
				ptOfsts = pl->GetPtOfstList(tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				zArr = pl->GetZList(tmpV);
				while (tmpV-- > 0)
				{
					*zArr++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				mArr = pl->GetMList(tmpV);
				while (tmpV-- > 0)
				{
					*mArr++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				return pl;
			}
		}
		return 0;
	case 18: //MultipointZM
		return 0;
	case 19: //PolygonZ
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pl;
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
				UOSInt tmpV;
				UInt32 *ptOfsts;
				Math::Coord2DDbl *points;
				Double *zArr;
				NEW_CLASS(pl, Math::Geometry::Polygon(srid, nPtOfst, nPoint, true, false));
				ptOfsts = pl->GetPtOfstList(tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				zArr = pl->GetZList(tmpV);
				while (tmpV-- > 0)
				{
					*zArr = ReadDouble(&buff[0]);
					zArr++;
					buff += 8;
				}
				return pl;
			}
		}
		return 0;
	case 20: //MultipointZ
		return 0;
	case 21: //PointM
		if (buffSize >= 28)
		{
			Math::Geometry::PointM *pt;
			NEW_CLASS(pt, Math::Geometry::PointM(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20])));
			// measure = ReadDouble(&buff[20]);
			return pt;
		}
		return 0;
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
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UInt32 *ptOfsts;
				NotNullPtr<Math::Geometry::LineString> lineString;
				Math::Coord2DDbl *points;
				Double *mArr;
				NEW_CLASS(pl, Math::Geometry::Polyline(srid));
				ptOfsts = MemAlloc(UInt32, nPtOfst);
				buff += 44;
				i = 0;
				while (i < nPtOfst)
				{
					ptOfsts[i] = ReadUInt32(buff);
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
					if (lineString.Set(pl->GetItem(i)))
					{
						mArr = lineString->GetMList(k);
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
		return 0;
	case 25: //PolygonM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Geometry::Polygon *pl;
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
				UOSInt tmpV;
				UInt32 *ptOfsts;
				Math::Coord2DDbl *points;
				Double *mArr;
				NEW_CLASS(pl, Math::Geometry::Polygon(srid, nPtOfst, nPoint, false, true));
				ptOfsts = pl->GetPtOfstList(tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				mArr = pl->GetMList(tmpV);
				while (tmpV-- > 0)
				{
					*mArr++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				return pl;
			}
		}
		return 0;
	case 28: //MultipointM
	case 31: //MultiPatchM
	case 32: //MultiPatch
	case 50: //GeneralPolyline
	case 51: //GeneralPolygon
	case 52: //GeneralPoint
	case 53: //GeneralMultipoint
	case 54: //GeneralMultiPatch
	default:
		return 0;
	}
}
