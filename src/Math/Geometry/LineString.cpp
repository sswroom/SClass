#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Math/CoordinateSystem.h"
#include "Math/Math.h"
#include "Math/Geometry/LineString.h"

Math::Geometry::LineString::LineString(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM) : PointCollection(srid, nPoint, 0)
{
	if (hasZ)
	{
		this->zArr = MemAllocA(Double, nPoint);
	}
	else
	{
		this->zArr = 0;
	}
	if (hasM)
	{
		this->mArr = MemAllocA(Double, nPoint);
	}
	else
	{
		this->mArr = 0;
	}
}

Math::Geometry::LineString::LineString(UInt32 srid, const Math::Coord2DDbl *pointArr, UOSInt nPoint, Bool hasZ, Bool hasM) : PointCollection(srid, nPoint, pointArr)
{
	if (hasZ)
	{
		this->zArr = MemAllocA(Double, nPoint);
	}
	else
	{
		this->zArr = 0;
	}
	if (hasM)
	{
		this->mArr = MemAllocA(Double, nPoint);
	}
	else
	{
		this->mArr = 0;
	}
}

Math::Geometry::LineString::~LineString()
{
	if (this->zArr)
		MemFreeA(this->zArr);
	if (this->mArr)
		MemFreeA(this->mArr);
}

Math::Geometry::Vector2D::VectorType Math::Geometry::LineString::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::LineString;
}

Math::Geometry::Vector2D *Math::Geometry::LineString::Clone() const
{
	Math::Geometry::LineString *pl;
	NEW_CLASS(pl, Math::Geometry::LineString(this->srid, this->nPoint, this->zArr != 0, this->mArr != 0));
	MemCopyAC(pl->pointArr, this->pointArr, sizeof(Math::Coord2DDbl) * nPoint);
	if (this->zArr)
	{	
		MemCopyAC(pl->zArr, this->zArr, sizeof(Double) * nPoint);
	}
	if (this->mArr)
	{	
		MemCopyAC(pl->mArr, this->mArr, sizeof(Double) * nPoint);
	}
	return pl;
}

Double Math::Geometry::LineString::CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
{
	UOSInt l;
	Math::Coord2DDbl *points;

	points = this->pointArr;

	l = this->nPoint;

	Double calBase;
	Math::Coord2DDbl calDiff;
	Math::Coord2DDbl calSqDiff;
	Math::Coord2DDbl calPt;
	Math::Coord2DDbl calPtOut = Math::Coord2DDbl(0, 0);
	Double calD;
	Double dist = 0x7fffffff;

	l--;
	while (l-- > 0)
	{
		calDiff = points[l] - points[l + 1];

		if (calDiff.y == 0)
		{
			calPt.x = pt.x;
		}
		else
		{
			calSqDiff = calDiff * calDiff;
			calBase = calSqDiff.x + calSqDiff.y;
			calPt.x = calSqDiff.x * pt.x;
			calPt.x += calSqDiff.y * points[l].x;
			calPt.x += (pt.y - points[l].y) * calDiff.x * calDiff.y;
			calPt.x /= calBase;
		}

		if (calDiff.x == 0)
		{
			calPt.y = pt.y;
		}
		else
		{
			calPt.y = ((calPt.x - points[l].x) * calDiff.y / calDiff.x) + points[l].y;
		}

		if (calDiff.x < 0)
		{
			if (points[l].x > calPt.x)
				continue;
			if (points[l + 1].x < calPt.x)
				continue;
		}
		else
		{
			if (points[l].x < calPt.x)
				continue;
			if (points[l + 1].x > calPt.x)
				continue;
		}

		if (calDiff.y < 0)
		{
			if (points[l].y > calPt.y)
				continue;
			if (points[l + 1].y < calPt.y)
				continue;
		}
		else
		{
			if (points[l].y < calPt.y)
				continue;
			if (points[l + 1].y > calPt.y)
				continue;
		}

		calDiff = pt - calPt;
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = calPt;
		}
	}
	l = this->nPoint;
	while (l-- > 0)
	{
		calDiff = pt - points[l];
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = points[l];
		}
	}
	if (nearPt)
	{
		*nearPt = calPtOut;
	}
	return dist;
}

Bool Math::Geometry::LineString::JoinVector(Math::Geometry::Vector2D *vec)
{
	if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::LineString || this->HasZ() != vec->HasZ() || this->HasM() != vec->HasM())
	{
		return false;
	}
	return false;
}

Bool Math::Geometry::LineString::HasZ() const
{
	return this->zArr != 0;
}

Bool Math::Geometry::LineString::HasM() const
{
	return this->mArr != 0;
}

void Math::Geometry::LineString::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	if (this->zArr)
	{
		UOSInt i = this->nPoint;
		while (i-- > 0)
		{
			Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->pointArr[i].x, this->pointArr[i].y, this->zArr[i], &this->pointArr[i].x, &this->pointArr[i].y, &this->zArr[i]);
		}
		this->srid = destCSys->GetSRID();
	}
	else
	{
		Math::CoordinateSystem::ConvertXYArray(srcCSys, destCSys, this->pointArr, this->pointArr, this->nPoint);
		this->srid = destCSys->GetSRID();
	}
}

Bool Math::Geometry::LineString::Equals(Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == this->GetVectorType() && this->HasZ() == vec->HasZ() && this->HasM() == vec->HasM())
	{
		Math::Geometry::LineString *pl = (Math::Geometry::LineString*)vec;
		UOSInt nPoint;
		Math::Coord2DDbl *ptList = pl->GetPointList(nPoint);
		Double *valArr;
		if (nPoint != this->nPoint)
		{
			return false;
		}
		UOSInt i = nPoint;
		while (i-- > 0)
		{
			if (ptList[i] != this->pointArr[i])
			{
				return false;
			}
		}
		if (this->zArr)
		{
			valArr = pl->zArr;
			i = nPoint;
			while (i-- > 0)
			{
				if (valArr[i] != this->zArr[i])
				{
					return false;
				}
			}
		}
		if (this->mArr)
		{
			valArr = pl->mArr;
			i = nPoint;
			while (i-- > 0)
			{
				if (valArr[i] != this->mArr[i])
				{
					return false;
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool Math::Geometry::LineString::EqualsNearly(Vector2D *vec) const
{
	if (vec == 0)
		return false;
	if (vec->GetSRID() != this->srid)
	{
		return false;
	}
	if (vec->GetVectorType() == this->GetVectorType() && this->HasZ() == vec->HasZ() && this->HasM() == vec->HasM())
	{
		Math::Geometry::LineString *pl = (Math::Geometry::LineString*)vec;
		UOSInt nPoint;
		Math::Coord2DDbl *ptList = pl->GetPointList(nPoint);
		Double *valArr;
		if (nPoint != this->nPoint)
		{
			return false;
		}
		UOSInt i = nPoint;
		while (i-- > 0)
		{
			if (!ptList[i].EqualsNearly(this->pointArr[i]))
			{
				return false;
			}
		}
		if (this->zArr)
		{
			valArr = pl->zArr;
			i = nPoint;
			while (i-- > 0)
			{
				if (!Math::NearlyEqualsDbl(valArr[i], this->zArr[i]))
				{
					return false;
				}
			}
		}
		if (this->mArr)
		{
			valArr = pl->mArr;
			i = nPoint;
			while (i-- > 0)
			{
				if (!Math::NearlyEqualsDbl(valArr[i], this->mArr[i]))
				{
					return false;
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Double *Math::Geometry::LineString::GetZList(UOSInt *nPoint) const
{
	if (nPoint)
		*nPoint = this->nPoint;
	return this->zArr;
}

Double *Math::Geometry::LineString::GetMList(UOSInt *nPoint) const
{
	if (nPoint)
		*nPoint = this->nPoint;
	return this->mArr;
}

Math::Geometry::LineString *Math::Geometry::LineString::SplitByPoint(Math::Coord2DDbl pt)
{
	UOSInt l;
	l = this->nPoint;

	Math::Coord2DDbl calPt;
	Double calZ;
	Double calM;
	Bool isPoint;
	UOSInt minId = (UOSInt)this->GetPointNo(pt, &isPoint, &calPt, &calZ, &calM);

	Math::Coord2DDbl *oldPoints;
	Math::Coord2DDbl *newPoints;
	Double *oldZ;
	Double *newZ;
	Double *oldM;
	Double *newM;
	Math::Geometry::LineString *newPL;
	if (isPoint)
	{
		if (minId == this->nPoint - 1 || minId == 0 || minId == (UOSInt)-1)
		{
			return 0;
		}
		
		oldPoints = this->pointArr;
		oldZ = this->zArr;
		oldM = this->mArr;

		newPoints = MemAllocA(Math::Coord2DDbl, (minId + 1));
		if (oldZ)
		{
			newZ = MemAllocA(Double, (minId + 1));
			MemCopyAC(newZ, oldZ, sizeof(Double) * (minId + 1));
		}
		else
		{
			newZ = 0;
		}
		if (oldM)
		{
			newM = MemAllocA(Double, (minId + 1));
			MemCopyAC(newM, oldM, sizeof(Double) * (minId + 1));
		}
		else
		{
			newM = 0;
		}
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
		}

		this->pointArr = newPoints;
		this->zArr = newZ;
		this->mArr = newM;
		NEW_CLASS(newPL, Math::Geometry::LineString(this->srid, this->nPoint - minId, this->zArr != 0, this->mArr != 0));
		newPoints = newPL->GetPointList(l);
		l = this->nPoint;
		while (l-- > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		if (oldZ)
		{
			l = this->nPoint;
			newZ = newPL->GetZList(&l);
			while (l-- > minId)
			{
				newZ[l - minId] = oldZ[l];
			}
			MemFreeA(oldZ);
		}
		if (oldM)
		{
			l = this->nPoint;
			newM = newPL->GetMList(&l);
			while (l-- > minId)
			{
				newM[l - minId] = oldM[l];
			}
			MemFreeA(oldM);
		}
		this->nPoint = minId + 1;
		MemFreeA(oldPoints);

		return newPL;
	}
	else
	{
		oldPoints = this->pointArr;
		oldZ = this->zArr;
		oldM = this->mArr;

		newPoints = MemAllocA(Math::Coord2DDbl, (minId + 2));
		if (oldZ)
		{
			newZ = MemAllocA(Double, (minId + 2));
			MemCopyAC(newZ, oldZ, sizeof(Double) * (minId + 1));
			newZ[minId + 1] = calZ;
		}
		else
		{
			newZ = 0;
		}
		if (oldM)
		{
			newM = MemAllocA(Double, (minId + 2));
			MemCopyAC(newM, oldM, sizeof(Double) * (minId + 1));
			newM[minId + 1] = calM;
		}
		else
		{
			newM = 0;
		}
		l = minId + 1;
		while (l-- > 0)
		{
			newPoints[l] = oldPoints[l];
		}
		newPoints[minId + 1] = calPt;

		this->pointArr = newPoints;
		this->zArr = newZ;
		this->mArr = newM;
		NEW_CLASS(newPL, Math::Geometry::LineString(this->srid, this->nPoint - minId, oldZ != 0, oldM != 0));

		newPoints = newPL->GetPointList(l);
		l = this->nPoint;
		while (--l > minId)
		{
			newPoints[l - minId] = oldPoints[l];
		}
		newPoints[0] = calPt;
		MemFreeA(oldPoints);

		if (oldZ)
		{
			newZ = newPL->GetZList(&l);
			l = this->nPoint;
			while (--l > minId)
			{
				newZ[l - minId] = oldZ[l];
			}
			newZ[0] = calZ;
			MemFreeA(oldZ);
		}

		if (oldM)
		{
			newM = newPL->GetMList(&l);
			l = this->nPoint;
			while (--l > minId)
			{
				newM[l - minId] = oldM[l];
			}
			newM[0] = calM;
			MemFreeA(oldM);
		}

		this->nPoint = minId + 2;

		return newPL;
	}
}

OSInt Math::Geometry::LineString::GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPtOutPtr, Double *calZOutPtr, Double *calMOutPtr)
{
	UOSInt l;
	Math::Coord2DDbl *points;
	Double *zArr;
	Double *mArr;

	points = this->pointArr;
	zArr = this->zArr;
	mArr = this->mArr;

	l = this->nPoint;

	Double calBase;
	Math::Coord2DDbl calDiff;
	Math::Coord2DDbl calSqDiff;
	Math::Coord2DDbl calPt;
	Math::Coord2DDbl calPtOut = Math::Coord2DDbl(0, 0);
	Double calZOut = 0;
	Double calMOut = 0;
	Double calZ;
	Double calM;
	Double calD;
	Double dist = 0x7fffffff;
	OSInt minId = -1;
	Bool isPointI = false;

	l--;
	while (l-- > 0)
	{
		calDiff = points[l] - points[l + 1];

		if (calDiff.x == 0 && calDiff.y == 0)
		{
			calPt.x = pt.x;
			calPt.y = pt.y;
			if (zArr)
			{
				calZ = zArr[l];
			}
			else
			{
				calZ = 0;
			}
			if (mArr)
			{
				calM = mArr[l];
			}
			else
			{
				calM = 0;
			}
		}
		else
		{
			if (calDiff.y == 0)
			{
				calPt.x = pt.x;
			}
			else
			{
				calSqDiff = calDiff * calDiff;
				calBase = calSqDiff.x + calSqDiff.y;
				calPt.x = calSqDiff.x * pt.x;
				calPt.x += calSqDiff.y * points[l].x;
				calPt.x += (pt.y - points[l].y) * calDiff.x * calDiff.y;
				calPt.x /= calBase;

				if (calDiff.x == 0)
				{
					////////////////////////////////
					calZ = 0;
					calM = 0;
				}
			}

			if (calDiff.x == 0)
			{
				calPt.y = pt.y;
			}
			else
			{
				Double ratio = (calPt.x - (points[l].x)) / calDiff.x;
				calPt.y = (ratio * calDiff.y) + points[l].y;
				if (zArr)
				{
					calZ = (ratio * (zArr[l] - zArr[l + 1])) + zArr[l];
				}
				else
				{
					calZ = 0;
				}
				if (mArr)
				{
					calM = (ratio * (mArr[l] - mArr[l + 1])) + mArr[l];
				}
				else
				{
					calM = 0;
				}
			}
		}

		if (calDiff.x < 0)
		{
			if (points[l].x > calPt.x)
				continue;
			if (points[l + 1].x < calPt.x)
				continue;
		}
		else
		{
			if (points[l].x < calPt.x)
				continue;
			if (points[l + 1].x > calPt.x)
				continue;
		}

		if (calDiff.y < 0)
		{
			if (points[l].y > calPt.y)
				continue;
			if (points[l + 1].y < calPt.y)
				continue;
		}
		else
		{
			if (points[l].y < calPt.y)
				continue;
			if (points[l + 1].y > calPt.y)
				continue;
		}

		calDiff = pt - calPt;
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = calPt;
			calZOut = calZ;
			calMOut = calM;
			isPointI = false;
			minId = (OSInt)l;
		}
	}
	l = this->nPoint;
	while (l-- > 0)
	{
		calDiff = pt - points[l];
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = points[l];
			calZOut = zArr?zArr[l]:0;
			calMOut = mArr?mArr[l]:0;
			minId = (OSInt)l;
			isPointI = true;
		}
	}

	if (isPoint)
	{
		*isPoint = isPointI;
	}
	if (calPtOutPtr)
	{
		*calPtOutPtr = calPtOut;
	}
	if (calZOutPtr)
	{
		*calZOutPtr = calZOut;
	}
	if (calMOutPtr)
	{
		*calMOutPtr = calMOut;
	}
	return minId;
}

Math::Geometry::Polygon *Math::Geometry::LineString::CreatePolygonByDist(Double dist) const
{
	if (this->nPoint < 2)
		return 0;

	Data::ArrayListDbl outPoints;
	Double lastPtX = 0;
	Double lastPtY = 0;
	Double thisPtX = 0;
	Double thisPtY = 0;
	Double nextPtX;
	Double nextPtY;
	Double t1;
	Double t2;
	Double deg;
	UOSInt i;

	deg = Math_ArcTan2(this->pointArr[1].x - this->pointArr[0].x, this->pointArr[1].y - this->pointArr[0].y);
	lastPtX = -Math_Cos(deg) * dist + this->pointArr[0].x;
	lastPtY = Math_Sin(deg) * dist + this->pointArr[0].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	i = 2;
	while (i < this->nPoint)
	{
		deg = Math_ArcTan2(this->pointArr[i].x - this->pointArr[i - 1].x, this->pointArr[i].y - this->pointArr[i - 1].y);
		nextPtX = -Math_Cos(deg) * dist + this->pointArr[i - 1].x;
		nextPtY = Math_Sin(deg) * dist + this->pointArr[i - 1].y;

		t1 = (this->pointArr[i - 2].y - this->pointArr[i - 1].y) / (this->pointArr[i - 2].x - this->pointArr[i - 1].x);
		t2 = (this->pointArr[i - 1].y - this->pointArr[i].y) / (this->pointArr[i - 1].x - this->pointArr[i].x);
		if (t1 != t2)
		{
			Double x1 = this->pointArr[i - 2].x;
			Double x2 = this->pointArr[i - 1].x;
			Double x3 = this->pointArr[i].x;
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->pointArr[i - 2].y;
			Double y2 = this->pointArr[i - 1].y;
			Double y3 = this->pointArr[i].y;
			Double y4 = lastPtY;
			Double y6 = nextPtY;

			thisPtX = (x4 * (x2 - x3) * (y2 - y1) - x6 * (x2 - x1) * (y2 - y3) + y6 * (x2 - x1) * (x2 - x3) - y4 * (x2 - x1) * (x2 - x3)) / ((y2 - y1) * (x2 - x3) - (x2 - x1) * (y2 - y3));
			if ((x2 - x1) == 0)
				thisPtY = y6 + (y2 - y3) / (x2 - x3) * (thisPtX - x6);
			else
				thisPtY = y4 + (y2 - y1) / (x2 - x1) * (thisPtX - x4);

			outPoints.Add(thisPtX);
			outPoints.Add(thisPtY);

		}
		lastPtX = thisPtX;
		lastPtY = thisPtY;
		i += 1;
	}

	deg = Math_ArcTan2(this->pointArr[this->nPoint - 1].x - this->pointArr[this->nPoint - 2].x, this->pointArr[this->nPoint - 1].y - this->pointArr[this->nPoint - 2].y);
	lastPtX = -Math_Cos(deg) * dist + this->pointArr[this->nPoint - 1].x;
	lastPtY = Math_Sin(deg) * dist + this->pointArr[this->nPoint- 1].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	lastPtX = Math_Cos(deg) * dist + this->pointArr[this->nPoint - 1].x;
	lastPtY = -Math_Sin(deg) * dist + this->pointArr[this->nPoint - 1].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	i = this->nPoint;
	while (i > 2)
	{
		i -= 1;
		deg = Math_ArcTan2(this->pointArr[i - 2].x - this->pointArr[i - 1].x, this->pointArr[i - 2].y - this->pointArr[i - 1].y);
		nextPtX = -Math_Cos(deg) * dist + this->pointArr[i - 2].x;
		nextPtY = Math_Sin(deg) * dist + this->pointArr[i - 2].y;

		t2 = (this->pointArr[i - 2].y - this->pointArr[i - 1].y) / (this->pointArr[i - 2].x - this->pointArr[i - 1].x);
		t1 = (this->pointArr[i - 1].y - this->pointArr[i].y) / (this->pointArr[i - 1].x - this->pointArr[i].x);

		if (t1 != t2)
		{
			Double x1 = this->pointArr[i].x;
			Double x2 = this->pointArr[i - 1].x;
			Double x3 = this->pointArr[i - 2].x;
			Double x4 = lastPtX;
			Double x6 = nextPtX;
			Double y1 = this->pointArr[i].y;
			Double y2 = this->pointArr[i - 1].y;
			Double y3 = this->pointArr[i - 2].y;
			Double y4 = lastPtY;
			Double y6 = nextPtY;



			thisPtX = (x4 * (x2 - x3) * (y2 - y1) - x6 * (x2 - x1) * (y2 - y3) + y6 * (x2 - x1) * (x2 - x3) - y4 * (x2 - x1) * (x2 - x3)) / ((y2 - y1) * (x2 - x3) - (x2 - x1) * (y2 - y3));
			if ((x2 - x1) == 0)
				thisPtY = y6 + (y2 - y3) / (x2 - x3) * (thisPtX - x6);
			else
				thisPtY = y4 + (y2 - y1) / (x2 - x1) * (thisPtX - x4);

			outPoints.Add(thisPtX);
			outPoints.Add(thisPtY);
		}

		lastPtX = thisPtX;
		lastPtY = thisPtY;
	}
	deg = Math_ArcTan2(this->pointArr[1].x - this->pointArr[0].x, this->pointArr[1].y - this->pointArr[0].y);

	lastPtX = Math_Cos(deg) * dist + this->pointArr[0].x;
	lastPtY = -Math_Sin(deg) * dist + this->pointArr[0].y;

	outPoints.Add(lastPtX);
	outPoints.Add(lastPtY);

	Math::Geometry::Polygon *pg;
	UOSInt nPoints;
	Math::Coord2DDbl *pts;
	NEW_CLASS(pg, Math::Geometry::Polygon(this->srid, 1, outPoints.GetCount() >> 1, false, false));
	pts = pg->GetPointList(nPoints);
	i = 0;
	while (i < nPoints)
	{
		pts[i].x = outPoints.GetItem((i << 1));
		pts[i].y = outPoints.GetItem((i << 1) + 1);
		i++;
	}
	return pg;
}

Math::Geometry::Polyline *Math::Geometry::LineString::CreatePolyline() const
{
	Math::Geometry::Polyline *pl;
	NEW_CLASS(pl, Math::Geometry::Polyline(this->srid, this->pointArr, this->nPoint, this->zArr != 0, this->mArr != 0));
	if (this->zArr)
	{
		UOSInt nPoint;
		MemCopyNO(pl->GetZList(&nPoint), this->zArr, sizeof(Double) * this->nPoint);
	}
	if (this->mArr)
	{
		UOSInt nPoint;
		MemCopyNO(pl->GetMList(&nPoint), this->mArr, sizeof(Double) * this->nPoint);
	}
	return pl;
}
