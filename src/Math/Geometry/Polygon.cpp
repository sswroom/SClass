#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListInt32.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/MultiPolygon.h"

Math::Geometry::Polygon::Polygon(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Bool hasZ, Bool hasM) : Math::Geometry::PointOfstCollection(srid, nPtOfst, nPoint, 0, hasZ, hasM)
{
}

Math::Geometry::Polygon::~Polygon()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::Polygon::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::Polygon;
}

NotNullPtr<Math::Geometry::Vector2D> Math::Geometry::Polygon::Clone() const
{
	NotNullPtr<Math::Geometry::Polygon> pg;
	NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid, this->nPtOfst, this->nPoint, this->HasZ(), this->HasM()));
	MemCopyNO(pg->ptOfstArr, this->ptOfstArr, sizeof(Int32) * this->nPtOfst);
	MemCopyAC(pg->pointArr, this->pointArr, sizeof(Math::Coord2DDbl) * nPoint);
	if (this->zArr)
	{	
		MemCopyAC(pg->zArr, this->zArr, sizeof(Double) * nPoint);
	}
	if (this->mArr)
	{	
		MemCopyAC(pg->mArr, this->mArr, sizeof(Double) * nPoint);
	}
	return pg;
}

Double Math::Geometry::Polygon::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	UOSInt k;
	UOSInt l;
	UInt32 m;
	UInt32 *ptOfsts;
	Math::Coord2DDbl *points;

	ptOfsts = this->ptOfstArr;
	points = this->pointArr;

	k = this->nPtOfst;
	l = this->nPoint;

	Double calBase;
	Math::Coord2DDbl calDiff;
	Math::Coord2DDbl calSqDiff;
	Math::Coord2DDbl calPt;
	Math::Coord2DDbl calPtOut = Math::Coord2DDbl(0, 0);
	Double calD;
	Double dist = 0x7fffffff;

	while (k--)
	{
		m = ptOfsts[k];
		l--;
		while (l-- > m)
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
	}
	k = this->nPoint;
	while (k-- > 0)
	{
		calDiff = pt - points[k];
		calSqDiff = calDiff * calDiff;
		calD = calSqDiff.x + calSqDiff.y;
		if (calD < dist)
		{
			dist = calD;
			calPtOut = points[k];
		}
	}
	nearPt.Set(calPtOut);
	return dist;
}

Bool Math::Geometry::Polygon::JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec)
{
	if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polygon)
		return false;
	Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec.Ptr();
	Math::Coord2DDbl *newPoints;
	UOSInt nPoint = this->nPoint + pg->nPoint;
	UInt32 *newPtOfsts;
	UOSInt nPtOfst = this->nPtOfst + pg->nPtOfst;
	
	newPoints = MemAllocA(Math::Coord2DDbl, nPoint);
	newPtOfsts = MemAlloc(UInt32, nPtOfst);
	MemCopyNO(newPoints, this->pointArr, sizeof(Math::Coord2DDbl) * this->nPoint);
	MemCopyNO(&newPoints[this->nPoint], pg->pointArr, sizeof(Math::Coord2DDbl) * pg->nPoint);
	MemCopyNO(newPtOfsts, this->ptOfstArr, sizeof(UInt32) * this->nPtOfst);
	UOSInt i = pg->nPtOfst;
	UOSInt j = i + this->nPtOfst;
	UInt32 k = (UInt32)this->nPoint;
	while (i-- > 0)
	{
		j--;
		newPtOfsts[j] = pg->ptOfstArr[i] + k;
	}
	MemFree(this->ptOfstArr);
	MemFreeA(this->pointArr);
	this->ptOfstArr = newPtOfsts;
	this->pointArr = newPoints;
	this->nPtOfst = nPtOfst;
	this->nPoint = nPoint;
	return true;
}

Bool Math::Geometry::Polygon::InsideVector(Math::Coord2DDbl coord) const
{
	Double thisX;
	Double thisY;
	Double lastX;
	Double lastY;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	Int32 leftCnt = 0;
	Double tmpX;

	k = this->nPtOfst;
	l = this->nPoint;

	while (k--)
	{
		m = this->ptOfstArr[k];

		lastX = this->pointArr[m].x;
		lastY = this->pointArr[m].y;
		while (l-- > m)
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
		l++;
	}

	return (leftCnt & 1) != 0;
}

Bool Math::Geometry::Polygon::HasJunction() const
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UInt32 nextPart;
	Double lastPtX;
	Double lastPtY;
	UOSInt lastIndex;
	Double thisPtX;
	Double thisPtY;
	UInt32 nextChkPart;
	Double lastChkPtX;
	Double lastChkPtY;
	UOSInt lastChkIndex;
	Double thisChkPtX;
	Double thisChkPtY;

	Double m1;
	Double m2 = 0;
	Double intX;
	Double intY;


	i = this->nPoint;
	j = this->nPtOfst;
	while (j-- > 0)
	{
		nextPart = this->ptOfstArr[j];
		lastPtX = this->pointArr[nextPart].x;
		lastPtY = this->pointArr[nextPart].y;
		lastIndex = nextPart;
		while (i-- > nextPart)
		{
			thisPtX = this->pointArr[i].x;
			thisPtY = this->pointArr[i].y;

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
					nextChkPart = this->ptOfstArr[l];
					if (l != j)
					{
						lastChkPtX = this->pointArr[nextChkPart].x;
						lastChkPtY = this->pointArr[nextChkPart].y;
					}

					while (k-- > nextChkPart)
					{
						thisChkPtX = this->pointArr[k].x;
						thisChkPtY = this->pointArr[k].y;

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

void Math::Geometry::Polygon::SplitByJunction(Data::ArrayList<Math::Geometry::Polygon*> *results)
{
	UOSInt i;
	UOSInt j;
	Math::Geometry::Polygon *tmpPG;
	Math::Coord2DDbl *points;
	Double *zArr;
	Double *mArr;
	UOSInt nPoints;
	Data::ArrayListDbl *junctionX;
	Data::ArrayListDbl *junctionY;
	Data::ArrayListInt32 *junctionPtNum;
	Double lastPtX;
	Double lastPtY;
	Double thisPtX;
	Double thisPtY;
	UOSInt lastIndex;
	Double lastChkPtX;
	Double lastChkPtY;
	Double thisChkPtX;
	Double thisChkPtY;
	UOSInt lastChkIndex;
	Double m1;
	Double m2 = 0;
	Double intX;
	Double intY;

	i = this->nPoint;
	while (this->nPtOfst > 1)
	{
		this->nPtOfst--;
		j = this->ptOfstArr[this->nPtOfst];
		NEW_CLASS(tmpPG, Math::Geometry::Polygon(this->srid, 1, i - j, this->zArr != 0, this->mArr != 0));
		points = tmpPG->GetPointList(nPoints);
		MemCopyAC(points, &this->pointArr[j], sizeof(Math::Coord2DDbl) * (i - j));
		if (this->zArr)
		{
			zArr = tmpPG->GetZList(nPoints);
			MemCopyAC(zArr, &this->zArr[j], sizeof(Double) * (i - j));
		}
		if (this->mArr)
		{
			mArr = tmpPG->GetMList(nPoints);
			MemCopyAC(mArr, &this->mArr[j], sizeof(Double) * (i - j));
		}
		tmpPG->SplitByJunction(results);
		
		this->nPoint = j;
		i = j;
	}

	NEW_CLASS(junctionX, Data::ArrayListDbl());
	NEW_CLASS(junctionY, Data::ArrayListDbl());
	NEW_CLASS(junctionPtNum, Data::ArrayListInt32());

	i = this->nPoint;
	lastPtX = this->pointArr[0].x;
	lastPtY = this->pointArr[0].y;
	lastIndex = 0;
	while (i-- > 0)
	{
		thisPtX = this->pointArr[i].x;
		thisPtY = this->pointArr[i].y;

		if (thisPtX != lastPtX || thisPtY != lastPtY)
		{
			m1 = (lastPtY - thisPtY) / (lastPtX - thisPtX);

			lastChkPtX = thisPtX;
			lastChkPtY = thisPtY;
			lastChkIndex = i;
			j = i;
			while (j-- > 0)
			{
				thisChkPtX = this->pointArr[j].x;
				thisChkPtY = this->pointArr[j].y;

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

NotNullPtr<Math::Geometry::MultiPolygon> Math::Geometry::Polygon::CreateMultiPolygon() const
{
	NotNullPtr<Math::Geometry::MultiPolygon> mpg;
	NEW_CLASSNN(mpg, Math::Geometry::MultiPolygon(this->srid, this->HasZ(), this->HasM()));
	if (this->nPtOfst <= 1)
	{
		mpg->AddGeometry(NotNullPtr<Math::Geometry::Polygon>::ConvertFrom(this->Clone()));
		return mpg;
	}
	Data::ArrayListNN<Math::Geometry::Polygon> pgList;
	NotNullPtr<Math::Geometry::Polygon> pg;
	UOSInt i = 0;
	UOSInt j = 0;
	UOSInt k;
	while (i < this->nPtOfst)
	{
		i++;
		if (i >= this->nPtOfst)
		{
			k = this->nPoint;
		}
		else
		{
			k = this->ptOfstArr[i];
		}
		NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid, 1, k - j, this->HasZ(), this->HasM()));
		MemCopyAC(pg->pointArr, &this->pointArr[j], sizeof(this->pointArr[0]) * (k - j));
		if (this->zArr)
		{
			MemCopyNO(pg->zArr, &this->zArr[j], sizeof(this->zArr[0]) * (k - j));
		}
		if (this->mArr)
		{
			MemCopyNO(pg->mArr, &this->mArr[j], sizeof(this->mArr[0]) * (k - j));
		}
		pgList.Add(pg);
		j = k;
	}
	i = 1;
	j = pgList.GetCount();
	while (i < j)
	{
		if (pg.Set(pgList.GetItem(i)))
		{
			UOSInt ptCnt;
			Math::Coord2DDbl *ptArr = pg->GetPointList(ptCnt);
			Math::Geometry::Polygon *lastPG = pgList.GetItem(i - 1);
			Bool inside = true;
			while (ptCnt-- > 0)
			{
				if (!lastPG->InsideVector(ptArr[ptCnt]))
				{
					inside = false;
					break;
				}
			}
			if (inside && lastPG->JoinVector(pg))
			{
				pg.Delete();
				pgList.RemoveAt(i);
				j--;
				i--;
			}
		}
		i++;
	}
	i = 0;
	j = pgList.GetCount();
	while (i < j)
	{
		if (pg.Set(pgList.GetItem(i)))
		{
			mpg->AddGeometry(pg);
		}
		i++;
	}
	return mpg;
}
