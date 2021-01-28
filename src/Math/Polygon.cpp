#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListInt32.h"
#include "Math/CoordinateSystem.h"
#include "Math/Polygon.h"

Math::Polygon::Polygon(Int32 srid, UOSInt nParts, UOSInt nPoints) : Math::PointCollection(srid)
{
	this->points = MemAlloc(Double, nPoints << 1);
	this->nPoints = nPoints;
	MemClear(this->points, sizeof(Double) * (nPoints << 1));
	this->nParts = nParts;
	this->parts = MemAlloc(UInt32, nParts);
	MemClear(this->parts, sizeof(UInt32) * nParts);
}

Math::Polygon::~Polygon()
{
	MemFree(points);
	MemFree(parts);
}

Math::Vector2D::VectorType Math::Polygon::GetVectorType()
{
	return Math::Vector2D::VT_POLYGON;
}

UInt32 *Math::Polygon::GetPartList(UOSInt *nParts)
{
	*nParts = this->nParts;
	return this->parts;
}

Double *Math::Polygon::GetPointList(UOSInt *nPoints)
{
	*nPoints = this->nPoints;
	return this->points;
}

Math::Vector2D *Math::Polygon::Clone()
{
	Math::Polygon *pg;
	NEW_CLASS(pg, Math::Polygon(this->srid, this->nParts, this->nPoints));
	MemCopyNO(pg->points, this->points, sizeof(Double) * (nPoints << 1));
	MemCopyNO(pg->parts, this->parts, sizeof(UInt32) * nParts);
	return pg;
}

void Math::Polygon::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	OSInt i = this->nPoints << 1;
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	x1 = x2 = this->points[0];
	y1 = y2 = this->points[1];
	while (i > 2)
	{
		i -= 2;
		if (x1 > this->points[i])
		{
			x1 = this->points[i];
		}
		if (x2 < this->points[i])
		{
			x2 = this->points[i];
		}
		if (y1 > this->points[i + 1])
		{
			y1 = this->points[i + 1];
		}
		if (y2 < this->points[i + 1])
		{
			y2 = this->points[i + 1];
		}
		i -= 2;
	}
	*minX = x1;
	*minY = y1;
	*maxX = x2;
	*maxY = y2;
}

Double Math::Polygon::CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY)
{
	if (InsideVector(x, y))
	{
		if (nearPtX && nearPtY)
		{
			*nearPtX = x;
			*nearPtY = y;
		}
		return 0;
	}

	UOSInt k;
	UOSInt l;
	UInt32 m;
	UInt32 *parts;
	Double *points;

	parts = this->parts;
	points = this->points;

	k = this->nParts;
	l = this->nPoints;

	Double calBase;
	Double calH;
	Double calW;
	Double calX;
	Double calY;
	Double calD;
	Double dist = 0x7fffffff;
	Double calPtX = 0;
	Double calPtY = 0;

	while (k--)
	{
		m = parts[k];
		l--;
		while (l-- > m)
		{
			calH = points[(l << 1) + 1] - points[(l << 1) + 3];
			calW = points[(l << 1) + 0] - points[(l << 1) + 2];

			if (calH == 0)
			{
				calX = x;
			}
			else
			{
				calX = (calBase = (calW * calW)) * x;
				calBase += calH * calH;
				calX += calH * calH * (points[(l << 1) + 0]);
				calX += (y - points[(l << 1) + 1]) * calH * calW;
				calX /= calBase;
			}

			if (calW == 0)
			{
				calY = y;
			}
			else
			{
				calY = ((calX - (points[(l << 1) + 0])) * calH / calW) + points[(l << 1) + 1];
			}

			if (calW < 0)
			{
				if (points[(l << 1) + 0] > calX)
					continue;
				if (points[(l << 1) + 2] < calX)
					continue;
			}
			else
			{
				if (points[(l << 1) + 0] < calX)
					continue;
				if (points[(l << 1) + 2] > calX)
					continue;
			}

			if (calH < 0)
			{
				if (points[(l << 1) + 1] > calY)
					continue;
				if (points[(l << 1) + 3] < calY)
					continue;
			}
			else
			{
				if (points[(l << 1) + 1] < calY)
					continue;
				if (points[(l << 1) + 3] > calY)
					continue;
			}

			calH = y - calY;
			calW = x - calX;
			calD = calW * calW + calH * calH;
			if (calD < dist)
			{
				dist = calD;
				calPtX = calX;
				calPtY = calY;
			}
		}
	}
	k = this->nPoints;
	while (k-- > 0)
	{
		calH = y - points[(k << 1) + 1];
		calW = x - points[(k << 1) + 0];
		calD = calW * calW + calH * calH;
		if (calD < dist)
		{
			dist = calD;
			calPtX = points[(k << 1) + 0];
			calPtY = points[(k << 1) + 1];
		}
	}
	if (nearPtX && nearPtY)
	{
		*nearPtX = calPtX;
		*nearPtY = calPtY;
	}
	return dist;
}

Bool Math::Polygon::JoinVector(Math::Vector2D *vec)
{
	if (vec->GetVectorType() != Math::Vector2D::VT_POLYGON)
		return false;
	Math::Polygon *pg = (Math::Polygon*)vec;
	Double *newPoints;
	UOSInt nPoints = this->nPoints + pg->nPoints;
	UInt32 *newParts;
	UOSInt nParts = this->nParts + pg->nParts;
	
	newPoints = MemAlloc(Double, nPoints * 2);
	newParts = MemAlloc(UInt32, nParts);
	MemCopyNO(newPoints, this->points, sizeof(Double) * this->nPoints * 2);
	MemCopyNO(&newPoints[this->nPoints * 2], pg->points, sizeof(Double) * pg->nPoints * 2);
	MemCopyNO(newParts, this->parts, sizeof(UInt32) * this->nParts);
	UOSInt i = pg->nParts;
	UOSInt j = i + this->nParts;
	UInt32 k = (UInt32)this->nPoints;
	while (i-- > 0)
	{
		j--;
		newParts[j] = pg->parts[i] + k;
	}
	MemFree(this->parts);
	MemFree(this->points);
	this->parts = newParts;
	this->points = newPoints;
	this->nParts = nParts;
	this->nPoints = nPoints;
	return true;
}

void Math::Polygon::ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
{
	OSInt i = this->nPoints;
	while (i-- > 0)
	{
		Math::CoordinateSystem::ConvertXYZ(srcCSys, destCSys, this->points[(i << 1)], this->points[(i << 1) + 1], 0, &this->points[(i << 1)], &this->points[(i << 1) + 1], 0);
	}
}

Bool Math::Polygon::InsideVector(Double x, Double y)
{
	Double thisX;
	Double thisY;
	Double lastX;
	Double lastY;
	OSInt j;
	OSInt k;
	OSInt l;
	OSInt m;
	Int32 leftCnt = 0;
	Double tmpX;

	k = this->nParts;
	l = this->nPoints;

	while (k--)
	{
		m = parts[k];

		lastX = points[(m << 1) + 0];
		lastY = points[(m << 1) + 1];
		while (l-- > m)
		{
			thisX = points[(l << 1) + 0];
			thisY = points[(l << 1) + 1];
			j = 0;
			if (lastY > y)
				j += 1;
			if (thisY > y)
				j += 1;

			if (j == 1)
			{
				tmpX = lastX - (lastX - thisX) * (lastY - y) / (lastY - thisY);
				if (tmpX == x)
				{
					return true;
				}
				else if (tmpX < x)
					leftCnt++;
			}
			else if (thisY == y && lastY == y)
			{
				if ((thisX >= x && lastX <= x) || (lastX >= x && thisX <= x))
				{
					return true;
				}
			}
			else if (thisY == y && thisX == x)
			{
				return true;
			}

			lastX = thisX;
			lastY = thisY;
		}
		l++;
	}

	return (leftCnt & 1) != 0;
}

Bool Math::Polygon::HasJunction()
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	Int32 nextPart;
	Double lastPtX;
	Double lastPtY;
	OSInt lastIndex;
	Double thisPtX;
	Double thisPtY;
	Int32 nextChkPart;
	Double lastChkPtX;
	Double lastChkPtY;
	OSInt lastChkIndex;
	Double thisChkPtX;
	Double thisChkPtY;

	Double m1;
	Double m2 = 0;
	Double intX;
	Double intY;


	i = this->nPoints;
	j = this->nParts;
	while (j-- > 0)
	{
		nextPart = this->parts[j];
		lastPtX = this->points[(nextPart << 1) + 0];
		lastPtY = this->points[(nextPart << 1) + 1];
		lastIndex = nextPart;
		while (i-- > nextPart)
		{
			thisPtX = this->points[(i << 1) + 0];
			thisPtY = this->points[(i << 1) + 1];

			if (thisPtX != lastPtX || thisPtY != lastPtY)
			{
				m1 = (lastPtY - thisPtY) / (lastPtX - thisPtX);

				nextChkPart = nextPart;
				lastChkPtX = thisPtX;
				lastChkPtY = thisPtY;
				lastChkIndex = i;
				k = i;
				l = j;
				l++;
				while (l-- > 0)
				{
					nextChkPart = this->parts[l];
					if (l != j)
					{
						lastChkPtX = this->points[(nextChkPart << 1) + 0];
						lastChkPtY = this->points[(nextChkPart << 1) + 1];
					}

					while (k-- > nextChkPart)
					{
						thisChkPtX = this->points[(k << 1) + 0];
						thisChkPtY = this->points[(k << 1) + 1];

						if (k == i || k == lastIndex || lastChkIndex == i || lastChkIndex == lastIndex)
						{
						}
						else if (thisChkPtX != lastChkPtX || thisChkPtY != lastChkPtY)
						{
							if (lastChkPtX == thisChkPtX && lastPtX == thisPtX)
							{
							}
							else if (lastChkPtX == thisChkPtX)
							{
								intY = m1 * thisChkPtX - m1 * thisPtX + thisPtY;
								if (intY == lastChkPtY || intY == thisChkPtY || ((intY > thisChkPtY) ^ (intY > lastChkPtY)))
									return true;
							}
							else if (lastPtX == thisPtX)
							{
								intY = m2 * thisPtX - m2 * thisChkPtX + thisChkPtY;
								if (intY == lastPtY || intY == thisPtY || ((intY > thisPtY) ^ (intY > lastPtY)))
									return true;
							}
							else
							{
								m2 = (lastChkPtY - thisChkPtY) / (lastChkPtX - thisChkPtX);
								if (m1 != m2)
								{
									intX = (m1 * thisPtX - m2 * thisChkPtX + thisChkPtY - thisPtY) / (m1 - m2);
									if ((intX == thisChkPtX || intX == lastChkPtX || ((intX > thisChkPtX) ^ (intX > lastChkPtX))) && (intX == thisPtX || intX == lastPtX || ((intX > thisPtX) ^ (intX > lastPtX))))
										return true;
								}
							}
						}

						lastChkPtX = thisChkPtX;
						lastChkPtY = thisChkPtY;
						lastChkIndex = k;
					}
					k++;
				}
			}
			

			lastPtX = thisPtX;
			lastPtY = thisPtY;
			lastIndex = i;
		}
		i++;
	}
	return false;
}

void Math::Polygon::SplitByJunction(Data::ArrayList<Math::Polygon*> *results)
{
	OSInt i;
	OSInt j;
	Math::Polygon *tmpPG;
	Double *points;
	UOSInt nPoints;
	Data::ArrayListDbl *junctionX;
	Data::ArrayListDbl *junctionY;
	Data::ArrayListInt32 *junctionPtNum;
	Double lastPtX;
	Double lastPtY;
	Double thisPtX;
	Double thisPtY;
	OSInt lastIndex;
	Double lastChkPtX;
	Double lastChkPtY;
	Double thisChkPtX;
	Double thisChkPtY;
	OSInt lastChkIndex;
	Double m1;
	Double m2 = 0;
	Double intX;
	Double intY;

	i = this->nPoints;
	while (this->nParts > 1)
	{
		this->nParts--;
		j = this->parts[this->nParts];
		NEW_CLASS(tmpPG, Math::Polygon(this->srid, 1, i - j));
		points = tmpPG->GetPointList(&nPoints);
		MemCopyNO(points, &this->points[(j << 1)], sizeof(Double) * (i - j) << 1);
		tmpPG->SplitByJunction(results);
		
		this->nPoints = j;
		i = j;
	}

	NEW_CLASS(junctionX, Data::ArrayListDbl());
	NEW_CLASS(junctionY, Data::ArrayListDbl());
	NEW_CLASS(junctionPtNum, Data::ArrayListInt32());

	i = this->nPoints;
	lastPtX = this->points[0];
	lastPtY = this->points[1];
	lastIndex = 0;
	while (i-- > 0)
	{
		thisPtX = this->points[(i << 1) + 0];
		thisPtY = this->points[(i << 1) + 1];

		if (thisPtX != lastPtX || thisPtY != lastPtY)
		{
			m1 = (lastPtY - thisPtY) / (lastPtX - thisPtX);

			lastChkPtX = thisPtX;
			lastChkPtY = thisPtY;
			lastChkIndex = i;
			j = i;
			while (j-- > 0)
			{
				thisChkPtX = this->points[(j << 1) + 0];
				thisChkPtY = this->points[(j << 1) + 1];

				if (j == i || j == lastIndex || lastChkIndex == i || lastChkIndex == lastIndex)
				{
				}
				else if (thisChkPtX != lastChkPtX || thisChkPtY != lastChkPtY)
				{
					if (lastChkPtX == thisChkPtX && lastPtX == thisPtX)
					{
					}
					else if (lastChkPtX == thisChkPtX)
					{
						intY = m1 * thisChkPtX - m1 * thisPtX + thisPtY;
						if (intY == lastChkPtY || intY == thisChkPtY || ((intY > thisChkPtY) ^ (intY > lastChkPtY)))
						{
							junctionX->Add(thisChkPtX);
							junctionY->Add(intY);
							junctionPtNum->Add((Int32)j);
							junctionX->Add(thisChkPtX);
							junctionY->Add(intY);
							junctionPtNum->Add((Int32)i);
						}
					}
					else if (lastPtX == thisPtX)
					{
						intY = m2 * thisPtX - m2 * thisChkPtX + thisChkPtY;
						if (intY == lastPtY || intY == thisPtY || ((intY > thisPtY) ^ (intY > lastPtY)))
						{
							junctionX->Add(thisPtX);
							junctionY->Add(intY);
							junctionPtNum->Add((Int32)j);
							junctionX->Add(thisPtX);
							junctionY->Add(intY);
							junctionPtNum->Add((Int32)i);
						}
					}
					else
					{
						m2 = (lastChkPtY - thisChkPtY) / (lastChkPtX - thisChkPtX);
						if (m1 != m2)
						{
							intX = (m1 * thisPtX - m2 * thisChkPtX + thisChkPtY - thisPtY) / (m1 - m2);
							if ((intX == thisChkPtX || intX == lastChkPtX || ((intX > thisChkPtX) ^ (intX > lastChkPtX))) && (intX == thisPtX || intX == lastPtX || ((intX > thisPtX) ^ (intX > lastPtX))))
							{
								intY = m2 * intX - m2 * thisChkPtX + thisChkPtY;
								junctionX->Add(intX);
								junctionY->Add(intY);
								junctionPtNum->Add((Int32)j);
								junctionX->Add(intY);
								junctionY->Add(intY);
								junctionPtNum->Add((Int32)i);
							}
						}
					}
				}

				lastChkPtX = thisChkPtX;
				lastChkPtY = thisChkPtY;
				lastChkIndex = j;
			}
		}
		

		lastPtX = thisPtX;
		lastPtY = thisPtY;
		lastIndex = i;
	}
//	OSInt junctionCnt = junctionPtNum->GetCount();
	///////////////////////////////////////////////////////

	DEL_CLASS(junctionPtNum);
	DEL_CLASS(junctionY);
	DEL_CLASS(junctionX);

	results->Add(this);
}
