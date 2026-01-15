#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListInt32.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/MultiPolygon.h"

Math::Geometry::Polygon::Polygon(UInt32 srid) : Math::Geometry::MultiGeometry<LinearRing>(srid)
{
}

Math::Geometry::Polygon::~Polygon()
{
}

Math::Geometry::Vector2D::VectorType Math::Geometry::Polygon::GetVectorType() const
{
	return Math::Geometry::Vector2D::VectorType::Polygon;
}

NN<Math::Geometry::Vector2D> Math::Geometry::Polygon::Clone() const
{
	NN<Math::Geometry::Polygon> pg;
	NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid));
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		pg->AddGeometry(NN<LinearRing>::ConvertFrom(it.Next()->Clone()));
	}
	return pg;
}

Double Math::Geometry::Polygon::CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
{
	Double minDist = 100000000000;
	Math::Coord2DDbl minPt = Math::Coord2DDbl(0, 0);
	Math::Coord2DDbl thisPt;
	Double thisDist;
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		thisDist = it.Next()->CalBoundarySqrDistance(pt, thisPt);
		if (thisDist < minDist)
		{
			minDist = thisDist;
			minPt = thisPt;
		}
	}
	nearPt.Set(minPt);
	return minDist;
}

Bool Math::Geometry::Polygon::JoinVector(NN<const Math::Geometry::Vector2D> vec)
{
	if (vec->GetVectorType() != Math::Geometry::Vector2D::VectorType::Polygon || this->srid != vec->GetSRID())
		return false;
	NN<Math::Geometry::Polygon> pg = NN<Math::Geometry::Polygon>::ConvertFrom(vec);
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = pg->geometries.Iterator();
	while (it.HasNext())
	{
		this->AddGeometry(NN<LinearRing>::ConvertFrom(it.Next()->Clone()));
	}
	return true;
}

Bool Math::Geometry::Polygon::InsideOrTouch(Math::Coord2DDbl coord) const
{
	UIntOS insideCnt = 0;
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = this->geometries.Iterator();
	while (it.HasNext())
	{
		if (it.Next()->InsideOrTouch(coord))
			insideCnt++;
	}
	return (insideCnt & 1) != 0;
}

/*Bool Math::Geometry::Polygon::HasJunction() const
{
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UIntOS l;
	UInt32 nextPart;
	Double lastPtX;
	Double lastPtY;
	UIntOS lastIndex;
	Double thisPtX;
	Double thisPtY;
	UInt32 nextChkPart;
	Double lastChkPtX;
	Double lastChkPtY;
	UIntOS lastChkIndex;
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
	UIntOS i;
	UIntOS j;
	Math::Geometry::Polygon *tmpPG;
	Math::Coord2DDbl *points;
	Double *zArr;
	Double *mArr;
	UIntOS nPoints;
	Data::ArrayListDbl *junctionX;
	Data::ArrayListDbl *junctionY;
	Data::ArrayListInt32 *junctionPtNum;
	Double lastPtX;
	Double lastPtY;
	Double thisPtX;
	Double thisPtY;
	UIntOS lastIndex;
	Double lastChkPtX;
	Double lastChkPtY;
	Double thisChkPtX;
	Double thisChkPtY;
	UIntOS lastChkIndex;
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
//	IntOS junctionCnt = junctionPtNum->GetCount();
	///////////////////////////////////////////////////////

	DEL_CLASS(junctionPtNum);
	DEL_CLASS(junctionY);
	DEL_CLASS(junctionX);

	results->Add(this);
}*/

NN<Math::Geometry::MultiPolygon> Math::Geometry::Polygon::CreateMultiPolygon() const
{
	NN<Math::Geometry::MultiPolygon> mpg;
	NEW_CLASSNN(mpg, Math::Geometry::MultiPolygon(this->srid));
	if (this->geometries.GetCount() <= 1)
	{
		mpg->AddGeometry(NN<Math::Geometry::Polygon>::ConvertFrom(this->Clone()));
		return mpg;
	}
	Data::ArrayListNN<Math::Geometry::Polygon> pgList;
	NN<Math::Geometry::Polygon> pg;
	NN<Math::Geometry::LinearRing> lr;
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = this->geometries.Iterator();
	UIntOS k;
	Bool found;
	while (it.HasNext())
	{
		lr = it.Next();
		found = false;
		k = pgList.GetCount();
		while (k-- > 0)
		{
			if (pgList.GetItem(k).SetTo(pg) && pg->InsideOrTouch(lr->GetPoint(0)))
			{
				found = true;
				pg->AddGeometry(NN<LinearRing>::ConvertFrom(lr->Clone()));
				break;
			}
		}
		if (!found)
		{
			NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid));
			pg->AddGeometry(NN<LinearRing>::ConvertFrom(lr->Clone()));
			pgList.Add(pg);
		}
	}
	Data::ArrayIterator<NN<Math::Geometry::Polygon>> itPG = pgList.Iterator();
	while (itPG.HasNext())
	{
		mpg->AddGeometry(itPG.Next());
	}
	return mpg;
}

void Math::Geometry::Polygon::AddFromPtOfst(UnsafeArray<UInt32> ptOfstList, UIntOS nPtOfst, UnsafeArray<Math::Coord2DDbl> pointList, UIntOS nPoint, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList)
{
	NN<LinearRing> linearRing;
	UIntOS i = 0;
	UIntOS j;
	UIntOS k;
	UIntOS tmp;
	UnsafeArray<Math::Coord2DDbl> ptArr;
	UnsafeArray<Double> zArr;
	UnsafeArray<Double> mArr;
	UnsafeArray<Double> nnzList;
	UnsafeArray<Double> nnmList;
	while (i < nPtOfst)
	{
		j = ptOfstList[i];
		if (i + 1 >= nPtOfst)
			k = nPoint;
		else
			k = ptOfstList[i + 1];
		NEW_CLASSNN(linearRing, LinearRing(this->srid, k - j, zList.NotNull(), mList.NotNull()));
		ptArr = linearRing->GetPointList(tmp);
		MemCopyNO(ptArr.Ptr(), &pointList[j], (k - j) * sizeof(Math::Coord2DDbl));
		if (linearRing->GetZList(tmp).SetTo(zArr) && zList.SetTo(nnzList))
		{
			MemCopyNO(zArr.Ptr(), &nnzList[j], (k - j) * sizeof(Double));
		}
		if (linearRing->GetMList(tmp).SetTo(mArr) && mList.SetTo(nnmList))
		{
			MemCopyNO(mArr.Ptr(), &nnmList[j], (k - j) * sizeof(Double));
		}
		this->AddGeometry(linearRing);
		i++;
	}
}

UIntOS Math::Geometry::Polygon::FillPointOfstList(UnsafeArray<Math::Coord2DDbl> pointList, UnsafeArray<UInt32> ptOfstList, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList) const
{
	UIntOS totalCnt = 0;
	UIntOS nPoint;
	NN<LineString> lineString;
	UnsafeArray<Math::Coord2DDbl> thisPtList;
	UnsafeArray<Double> dList;
	UnsafeArray<Double> nnList;
	UIntOS k;
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = this->geometries.Iterator();
	UIntOS i = 0;
	while (it.HasNext())
	{
		ptOfstList[i] = (UInt32)totalCnt;
		lineString = it.Next();
		thisPtList = lineString->GetPointList(nPoint);
		MemCopyNO(&pointList[totalCnt], thisPtList.Ptr(), sizeof(Math::Coord2DDbl) * nPoint);
		if (zList.SetTo(nnList))
		{
			if (lineString->GetZList(k).SetTo(dList))
			{
				MemCopyNO(&nnList[totalCnt], dList.Ptr(), sizeof(Double) * k);
			}
		}
		if (mList.SetTo(nnList))
		{
			if (lineString->GetMList(k).SetTo(dList))
			{
				MemCopyNO(&nnList[totalCnt], dList.Ptr(), sizeof(Double) * k);
			}
		}
		totalCnt += nPoint;
		i++;
	}
	return totalCnt;
}
