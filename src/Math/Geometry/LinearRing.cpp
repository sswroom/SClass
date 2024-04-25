#include "Stdafx.h"
#include "Math/Geometry/LinearRing.h"

Math::Geometry::LinearRing::LinearRing(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM) : LineString(srid, nPoint, hasZ, hasM)
{
}

Math::Geometry::LinearRing::LinearRing(UInt32 srid, const Math::Coord2DDbl *pointArr, UOSInt nPoint, Double *zArr, Double *mArr) : LineString(srid, pointArr, nPoint, zArr, mArr)
{
}

Math::Geometry::LinearRing::~LinearRing()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::LinearRing::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::LinearRing;
}

NN<Math::Geometry::Vector2D> Math::Geometry::LinearRing::Clone() const
{
	NN<Math::Geometry::LinearRing> lr;
	NEW_CLASSNN(lr, Math::Geometry::LinearRing(this->srid, this->nPoint, this->zArr != 0, this->mArr != 0));
	MemCopyNO(lr->pointArr, this->pointArr, sizeof(Math::Coord2DDbl) * this->nPoint);
	if (this->zArr)
	{
		MemCopyNO(lr->zArr, this->zArr, sizeof(Double) * this->nPoint);
	}
	if (this->mArr)
	{
		MemCopyNO(lr->mArr, this->mArr, sizeof(Double) * this->nPoint);
	}
	return lr;
}

Bool Math::Geometry::LinearRing::InsideOrTouch(Math::Coord2DDbl coord) const
{
	Double thisX;
	Double thisY;
	Double lastX;
	Double lastY;
	UOSInt j;
	UOSInt l;
	Int32 leftCnt = 0;
	Double tmpX;

	l = this->nPoint;
	lastX = this->pointArr[0].x;
	lastY = this->pointArr[0].y;
	while (l-- > 0)
	{
		thisX = this->pointArr[l].x;
		thisY = this->pointArr[l].y;
		j = 0;
		if (lastY > coord.y)
			j += 1;
		if (thisY > coord.y)
			j += 1;

		if (j == 1)
		{
			tmpX = lastX - (lastX - thisX) * (lastY - coord.y) / (lastY - thisY);
			if (tmpX == coord.x)
			{
				return true;
			}
			else if (tmpX < coord.x)
				leftCnt++;
		}
		else if (thisY == coord.y && lastY == coord.y)
		{
			if ((thisX >= coord.x && lastX <= coord.x) || (lastX >= coord.x && thisX <= coord.x))
			{
				return true;
			}
		}
		else if (thisY == coord.y && thisX == coord.x)
		{
			return true;
		}

		lastX = thisX;
		lastY = thisY;
	}

	return (leftCnt & 1) != 0;
}

Double Math::Geometry::LinearRing::CalArea() const
{
	Math::Coord2DDbl lastPt = this->pointArr[0];
	Double total = 0;
	UOSInt i = this->nPoint;
	while (i-- > 0)
	{
		total = total + (lastPt.x * this->pointArr[i].y - lastPt.y * this->pointArr[i].x);
		lastPt = this->pointArr[i];
	}
	if (total < 0)
		return total * -0.5;
	else
		return total * 0.5;
}

Bool Math::Geometry::LinearRing::IsOpen() const
{
	return this->pointArr[0] != this->pointArr[this->nPoint - 1];
}

Bool Math::Geometry::LinearRing::IsClose() const
{
	return this->pointArr[0] == this->pointArr[this->nPoint - 1];
}

NN<Math::Geometry::LinearRing> Math::Geometry::LinearRing::CreateFromCircle(UInt32 srid, Math::Coord2DDbl center, Double radiusX, Double radiusY, UOSInt nPoints)
{
	NN<Math::Geometry::LinearRing> lr;
	NEW_CLASSNN(lr, Math::Geometry::LinearRing(srid, nPoints + 1, false, false));
	Double ratio = 2 * Math::PI / UOSInt2Double(nPoints);
	UOSInt i;
	UOSInt j;
	Double angle;
	Math::Coord2DDbl *pointList = lr->GetPointList(j);
	i = 0;
	while (i < j)
	{
		angle = UOSInt2Double(i) * ratio;
		pointList[i] = center + Math::Coord2DDbl(radiusX * Math_Cos(angle), radiusY * Math_Sin(angle));
		i++;
	}
	return lr;
}
