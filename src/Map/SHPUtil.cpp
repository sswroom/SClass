#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Map/SHPUtil.h"
#include "Math/Math.h"
#include "Math/Polyline.h"
#include "Math/Polygon.h"
#include "Math/Point.h"
#include "Math/PointM.h"
#include "Math/PointZM.h"

Math::Vector2D *Map::SHPUtil::ParseShpRecord(UInt32 srid, const UInt8 *buff, UOSInt buffSize)
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
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12])));
			return pt;
		}
		return 0;
	case 3: //Polyline
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Polyline *pl;
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
				NEW_CLASS(pl, Math::Polyline(srid, nPtOfst, nPoint, false, false));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
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
	case 5: //Polygon
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Polygon *pl;
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
				NEW_CLASS(pl, Math::Polygon(srid, nPtOfst, nPoint, false, false));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
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
			Math::PointZ *pt;
			NEW_CLASS(pt, Math::PointZ(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20])));
			return pt;
		}
		return 0;
	case 10: //PolylineZ
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Polyline *pl;
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
				UOSInt tmpV;
				UInt32 *ptOfsts;
				Math::Coord2DDbl *points;
				Double *alts;
				NEW_CLASS(pl, Math::Polyline(srid, nPtOfst, nPoint, true, false));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				alts = pl->GetZList(&tmpV);
				while (tmpV-- > 0)
				{
					*alts++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				return pl;
			}
		}
		return 0;
	case 11: //PointZM
		if (buffSize >= 36)
		{
			Math::PointZM *pt;
			NEW_CLASS(pt, Math::PointZM(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20]), ReadDouble(&buff[28])));
			return pt;
		}
		return 0;
	case 13: //PolylineZM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Polyline *pl;
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
				UOSInt tmpV;
				UInt32 *ptOfsts;
				Math::Coord2DDbl *points;
				Double *zArr;
				Double *mArr;
				NEW_CLASS(pl, Math::Polyline(srid, nPtOfst, nPoint, true, true));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				zArr = pl->GetZList(&tmpV);
				while (tmpV-- > 0)
				{
					*zArr++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				mArr = pl->GetMList(&tmpV);
				while (tmpV-- > 0)
				{
					*mArr++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				return pl;
			}
		}
		return 0;
	case 15: //PolygonZM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Polygon *pl;
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
				NEW_CLASS(pl, Math::Polygon(srid, nPtOfst, nPoint, true, true));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				zArr = pl->GetZList(&tmpV);
				while (tmpV-- > 0)
				{
					*zArr++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				mArr = pl->GetMList(&tmpV);
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
			Math::Polygon *pl;
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
				NEW_CLASS(pl, Math::Polygon(srid, nPtOfst, nPoint, true, false));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				zArr = pl->GetZList(&tmpV);
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
			Math::PointM *pt;
			NEW_CLASS(pt, Math::PointM(srid, ReadDouble(&buff[4]), ReadDouble(&buff[12]), ReadDouble(&buff[20])));
			// measure = ReadDouble(&buff[20]);
			return pt;
		}
		return 0;
	case 23: //PolylineM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Polyline *pl;
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
				NEW_CLASS(pl, Math::Polyline(srid, nPtOfst, nPoint, false, true));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				mArr = pl->GetMList(&tmpV);
				while (tmpV-- > 0)
				{
					*mArr++ = ReadDouble(&buff[0]);
					buff += 8;
				}
				return pl;
			}
		}
		return 0;
	case 25: //PolygonM
		if (buffSize >= 44)
		{
			UInt32 nPtOfst;
			UInt32 nPoint;
			Math::Polygon *pl;
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
				NEW_CLASS(pl, Math::Polygon(srid, nPtOfst, nPoint, false, true));
				ptOfsts = pl->GetPtOfstList(&tmpV);
				buff += 44;
				while (tmpV-- > 0)
				{
					*ptOfsts++ = ReadUInt32(buff);
					buff += 4;
				}
				points = pl->GetPointList(&tmpV);
				while (tmpV-- > 0)
				{
					points->x = ReadDouble(&buff[0]);
					points->y = ReadDouble(&buff[8]);
					points++;
					buff += 16;
				}
				mArr = pl->GetMList(&tmpV);
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
