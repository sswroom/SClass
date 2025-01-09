#include "Stdafx.h"
#include "Map/ESRI/ESRICurve.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/CircularString.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/MultiCurve.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/MultiSurface.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"

//https://stackoverflow.com/questions/41537950/converting-an-svg-arc-to-lines/41544540#41544540
Map::ESRI::ESRICurve::ESRICurve(UInt32 srid, UnsafeArray<UInt32> ptOfstList, UOSInt nParts, UnsafeArray<Math::Coord2DDbl> ptArr, UOSInt nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr)
{
	this->srid = srid;
	this->ptList.AddAll(Data::DataArray<Math::Coord2DDbl>(ptArr, nPoint));
	this->partList.AddAll(Data::DataArray<UInt32>(ptOfstList, nParts));
	UnsafeArray<Double> nnArr;
	if (zArr.SetTo(nnArr))
	{
		this->zList.AddAll(Data::DataArray<Double>(nnArr, nPoint));
	}
	if (mArr.SetTo(nnArr))
	{
		this->mList.AddAll(Data::DataArray<Double>(nnArr, nPoint));
	}
}

Map::ESRI::ESRICurve::~ESRICurve()
{
	UOSInt i = this->curveList.GetCount();
	while (i-- > 0)
	{
		MemFreeANN(this->curveList.GetItemNoCheck(i));
	}
}

void Map::ESRI::ESRICurve::AddArc(UOSInt index, Math::Coord2DDbl center, UInt32 bits)
{
	if (index >= ptList.GetCount())
	{
		printf("ESRICurve: Arc index out of range: %d\r\n", (UInt32)index);
		return;
	}
	if (center.x < 800000)
	{
		printf("Arc out of range: %d, %lf, %lf, %d\r\n", (UInt32)index, center.x, center.y, bits);
	}
	NN<ArcInfo> arc;
	arc = MemAllocANN(ArcInfo);
	arc->type = 1;
	arc->startIndex = index;
	arc->center = center;
	arc->bits = bits;
	this->curveList.Add(arc);
}

void Map::ESRI::ESRICurve::AddBezier3Curve(UOSInt index, Math::Coord2DDbl point1, Math::Coord2DDbl point2)
{
	if (index >= ptList.GetCount())
	{
		printf("ESRICurve: Bezier3Curve index out of range: %d\r\n", (UInt32)index);
		return;
	}
	if (point1.x < 800000 || point2.x < 800000)
	{
		printf("BezierArc out of range: %d, %lf, %lf, %lf, %lf\r\n", (UInt32)index, point1.x, point1.y, point2.x, point2.y);
	}
	NN<BezierCurveInfo> curve;
	curve = MemAllocANN(BezierCurveInfo);
	curve->type = 5;
	curve->startIndex = index;
	curve->point1 = point1;
	curve->point2 = point2;
	this->curveList.Add(curve);
}

void Map::ESRI::ESRICurve::AddEllipticArc(UOSInt index, Math::Coord2DDbl center, Double rotation, Double semiMajor, Double minorMajorRatio, UInt32 bits)
{
	if (index >= ptList.GetCount())
	{
		printf("ESRICurve: EllipticArc index out of range: %d\r\n", (UInt32)index);
		return;
	}
	NN<EllipticArcInfo> curve;
	curve = MemAllocANN(EllipticArcInfo);
	curve->type = 4;
	curve->startIndex = index;
	curve->center = center;
	curve->rotation = rotation;
	curve->semiMajor = semiMajor;
	curve->minorMajorRatio = minorMajorRatio;
	curve->bits = bits;
	this->curveList.Add(curve);
	printf("ESRICurve: EllipticArc is not supported\r\n");
}

NN<Math::Geometry::Vector2D> Map::ESRI::ESRICurve::CreatePolygon() const
{
	if (this->curveList.GetCount() > 0)
	{
		Data::ArrayListA<Math::Coord2DDbl> ptList;
		NN<Math::Geometry::MultiSurface> ms;
		NN<Math::Geometry::CurvePolygon> cpg;
		NN<Math::Geometry::CompoundCurve> cc;
		NN<Math::Geometry::LineString> ls;
		ptList.AddAll(this->ptList);
		NEW_CLASSNN(ms, Math::Geometry::MultiSurface(this->srid));
		NEW_CLASSNN(cpg, Math::Geometry::CurvePolygon(this->srid));
		NEW_CLASSNN(cc, Math::Geometry::CompoundCurve(this->srid));
		NN<CurveInfo> curve;
		Math::Coord2DDbl curvePts[3];
		UOSInt indexOfst = 0;
		UOSInt endPart;
		UOSInt i = 0;
		UOSInt currIndex = 0;
		UOSInt partI = 1;
		if (partI >= this->partList.GetCount())
		{
			endPart = ptList.GetCount() - indexOfst;
		}
		else
		{
			endPart = this->partList.GetItem(partI);
		}
		while (i < this->curveList.GetCount())
		{
			curve = this->curveList.GetItemNoCheck(i);
			while (curve->startIndex >= endPart)
			{
				if (currIndex + 1 < endPart + indexOfst)
				{
					if (cc->GetCount() == 0)
					{
						NEW_CLASSNN(ls, Math::Geometry::LinearRing(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
						cpg->AddGeometry(ls);
						currIndex = endPart + indexOfst;
					}
					else
					{
						NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
						cc->AddGeometry(ls);
						cpg->AddGeometry(cc);
						currIndex = endPart + indexOfst;
						NEW_CLASSNN(cc, Math::Geometry::CompoundCurve(this->srid));
					}
				}
				else
				{
					currIndex = endPart + indexOfst;
				}

				partI++;
				if (partI >= this->partList.GetCount())
				{
					endPart = ptList.GetCount() - indexOfst;
				}
				else
				{
					endPart = this->partList.GetItem(partI);
				}
			}
			if (curve->startIndex + indexOfst > currIndex)
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], curve->startIndex + indexOfst - currIndex + 1, 0, 0));
				cc->AddGeometry(ls);
			}
			if (curve->type == 5)
			{
				NN<BezierCurveInfo> bcurve = NN<BezierCurveInfo>::ConvertFrom(curve);
				Data::ArrayListA<Math::Coord2DDbl> tmpPts;
				Math::GeometryTool::BezierCurveToLine(ptList.GetItem(bcurve->startIndex + indexOfst), bcurve->point1, bcurve->point2, ptList.GetItem(bcurve->startIndex + indexOfst + 1), 10, tmpPts);
				ptList.InsertRange(bcurve->startIndex + indexOfst + 1, tmpPts.GetCount() - 2, tmpPts.Arr() + 1);
				indexOfst += tmpPts.GetCount() - 2;
			}
			else if (curve->type == 4)
			{
				//NN<EllipticArcInfo> earc = NN<EllipticArcInfo>::ConvertFrom(curve);
				//ptList.Insert(earc->startIndex + indexOfst + 1, earc->center);
				//indexOfst += 1;
			}
			else //if (curve->type == 1)
			{
				NN<ArcInfo> arc = NN<ArcInfo>::ConvertFrom(curve);
				curvePts[0] = ptList.GetItem(arc->startIndex + indexOfst);
				curvePts[1] = arc->center;
				curvePts[2] = ptList.GetItem(arc->startIndex + indexOfst + 1);
				NEW_CLASSNN(ls, Math::Geometry::CircularString(this->srid, curvePts, 3, 0, 0));
				cc->AddGeometry(ls);
				currIndex = arc->startIndex + indexOfst + 1;
			}
			i++;
		}
		while (partI < this->partList.GetCount())
		{
			if (cc->GetCount() == 0)
			{
				NEW_CLASSNN(ls, Math::Geometry::LinearRing(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				cpg->AddGeometry(ls);
				currIndex = endPart + indexOfst;
			}
			else
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				cc->AddGeometry(ls);
				cpg->AddGeometry(cc);
				currIndex = endPart + indexOfst;
				NEW_CLASSNN(cc, Math::Geometry::CompoundCurve(this->srid));
			}

			partI++;
			if (partI >= this->partList.GetCount())
			{
				endPart = ptList.GetCount() - indexOfst;
			}
			else
			{
				endPart = this->partList.GetItem(partI);
			}
		}
		if (currIndex + 1 < ptList.GetCount() - indexOfst)
		{
			if (cc->GetCount() == 0)
			{
				NEW_CLASSNN(ls, Math::Geometry::LinearRing(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				cpg->AddGeometry(ls);
				currIndex = endPart + indexOfst;
				cc.Delete();
			}
			else
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				cc->AddGeometry(ls);
				cpg->AddGeometry(cc);
				currIndex = endPart + indexOfst;
			}
		}
		else
		{
			if (cc->GetCount() == 0)
			{
				cc.Delete();
			}
			else
			{
				cpg->AddGeometry(cc);
			}
		}
		ms->AddGeometry(cpg);
		return ms;
	}
	else
	{
		UnsafeArrayOpt<Double> zArr = 0;
		UnsafeArrayOpt<Double> mArr = 0;
		if (this->ptList.GetCount() == this->zList.GetCount())
		{
			zArr = this->zList.Arr();
		}
		if (this->ptList.GetCount() == this->mList.GetCount())
		{
			mArr = this->mList.Arr();
		}
		NN<Math::Geometry::Polygon> pg;
		NN<Math::Geometry::MultiPolygon> mpg;
		NEW_CLASSNN(pg, Math::Geometry::Polygon(this->srid));
		pg->AddFromPtOfst(this->partList.Arr(), this->partList.GetCount(), this->ptList.Arr(), this->ptList.GetCount(), zArr, mArr);
		NEW_CLASSNN(mpg, Math::Geometry::MultiPolygon(this->srid));
		mpg->AddGeometry(pg);
		return mpg;
	}
}

NN<Math::Geometry::Vector2D> Map::ESRI::ESRICurve::CreatePolyline() const
{
	if (this->curveList.GetCount() > 0)
	{
		Data::ArrayListA<Math::Coord2DDbl> ptList;
		NN<Math::Geometry::MultiCurve> mc;
		NN<Math::Geometry::CompoundCurve> cc;
		NN<Math::Geometry::LineString> ls;
		ptList.AddAll(this->ptList);
		NEW_CLASSNN(mc, Math::Geometry::MultiCurve(this->srid));
		NEW_CLASSNN(cc, Math::Geometry::CompoundCurve(this->srid));
		NN<CurveInfo> curve;
		Math::Coord2DDbl curvePts[3];
		UOSInt indexOfst = 0;
		UOSInt endPart;
		UOSInt i = 0;
		UOSInt currIndex = 0;
		UOSInt partI = 1;
		if (partI >= this->partList.GetCount())
		{
			endPart = ptList.GetCount() - indexOfst;
		}
		else
		{
			endPart = this->partList.GetItem(partI);
		}
		while (i < this->curveList.GetCount())
		{
			curve = this->curveList.GetItemNoCheck(i);
			while (curve->startIndex >= endPart)
			{
				if (currIndex + 1 < endPart + indexOfst)
				{
					if (cc->GetCount() == 0)
					{
						NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
						mc->AddGeometry(ls);
						currIndex = endPart + indexOfst;
					}
					else
					{
						NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
						cc->AddGeometry(ls);
						mc->AddGeometry(cc);
						currIndex = endPart + indexOfst;
						NEW_CLASSNN(cc, Math::Geometry::CompoundCurve(this->srid));
					}
				}
				else
				{
					currIndex = endPart + indexOfst;
				}

				partI++;
				if (partI >= this->partList.GetCount())
				{
					endPart = ptList.GetCount() - indexOfst;
				}
				else
				{
					endPart = this->partList.GetItem(partI);
				}
			}
			if (curve->startIndex + indexOfst > currIndex)
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], curve->startIndex + indexOfst - currIndex + 1, 0, 0));
				cc->AddGeometry(ls);
			}
			if (curve->type == 5)
			{
				NN<BezierCurveInfo> bcurve = NN<BezierCurveInfo>::ConvertFrom(curve);
				Data::ArrayListA<Math::Coord2DDbl> tmpPts;
				Math::GeometryTool::BezierCurveToLine(ptList.GetItem(bcurve->startIndex + indexOfst), bcurve->point1, bcurve->point2, ptList.GetItem(bcurve->startIndex + indexOfst + 1), 10, tmpPts);
				ptList.InsertRange(bcurve->startIndex + indexOfst + 1, tmpPts.GetCount() - 2, tmpPts.Arr() + 1);
				indexOfst += tmpPts.GetCount() - 2;
			}
			else if (curve->type == 4)
			{
				//NN<EllipticArcInfo> earc = NN<EllipticArcInfo>::ConvertFrom(curve);
				//ptList.Insert(earc->startIndex + indexOfst + 1, earc->center);
				//indexOfst += 1;
			}
			else //if (curve->type == 1)
			{
				NN<ArcInfo> arc = NN<ArcInfo>::ConvertFrom(curve);
				curvePts[0] = ptList.GetItem(arc->startIndex + indexOfst);
				curvePts[1] = arc->center;
				curvePts[2] = ptList.GetItem(arc->startIndex + indexOfst + 1);
				NEW_CLASSNN(ls, Math::Geometry::CircularString(this->srid, curvePts, 3, 0, 0));
				cc->AddGeometry(ls);
				currIndex = arc->startIndex + indexOfst + 1;
			}
			i++;
		}
		while (partI < this->partList.GetCount())
		{
			if (cc->GetCount() == 0)
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				mc->AddGeometry(ls);
				currIndex = endPart + indexOfst;
			}
			else
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				cc->AddGeometry(ls);
				mc->AddGeometry(cc);
				currIndex = endPart + indexOfst;
				NEW_CLASSNN(cc, Math::Geometry::CompoundCurve(this->srid));
			}

			partI++;
			if (partI >= this->partList.GetCount())
			{
				endPart = ptList.GetCount() - indexOfst;
			}
			else
			{
				endPart = this->partList.GetItem(partI);
			}
		}
		if (currIndex + 1 < ptList.GetCount() - indexOfst)
		{
			if (cc->GetCount() == 0)
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				mc->AddGeometry(ls);
				currIndex = endPart + indexOfst;
				cc.Delete();
			}
			else
			{
				NEW_CLASSNN(ls, Math::Geometry::LineString(this->srid, &ptList.Arr()[currIndex], endPart + indexOfst - currIndex, 0, 0));
				cc->AddGeometry(ls);
				mc->AddGeometry(cc);
				currIndex = endPart + indexOfst;
			}
		}
		else
		{
			if (cc->GetCount() == 0)
			{
				cc.Delete();
			}
			else
			{
				mc->AddGeometry(cc);
			}
		}
		return mc;
	}
	else
	{
		UnsafeArrayOpt<Double> zArr = 0;
		UnsafeArrayOpt<Double> mArr = 0;
		if (this->ptList.GetCount() == this->zList.GetCount())
		{
			zArr = this->zList.Arr();
		}
		if (this->ptList.GetCount() == this->mList.GetCount())
		{
			mArr = this->mList.Arr();
		}
		NN<Math::Geometry::Polyline> pl;
		NEW_CLASSNN(pl, Math::Geometry::Polyline(this->srid));
		pl->AddFromPtOfst(this->partList.Arr(), this->partList.GetCount(), this->ptList.Arr(), this->ptList.GetCount(), zArr, mArr);
		return pl;
	}
}
