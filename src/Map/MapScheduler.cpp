#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapScheduler.h"
#include "Math/Math.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

#include <stdio.h>

UInt32 __stdcall Map::MapScheduler::MapThread(AnyType obj)
{
	UInt32 i = 0;
	UOSInt j;
	NN<Map::MapScheduler> me = obj.GetNN<Map::MapScheduler>();
	me->threadRunning = true;
	while (!me->toStop)
	{
		NN<Math::Geometry::Vector2D> vec;
		if (me->dt == ThreadState::Clearing)
		{
			Sync::MutexUsage mutUsage(me->taskMut);
			j = me->tasks.GetCount();
			while (j-- > 0)
			{
				if (me->tasks.GetItem(j).SetTo(vec))
					vec.Delete();
			}
			me->tasks.Clear();
			i = 0;
			me->dt = ThreadState::Drawing;
			mutUsage.EndUse();
			me->finishEvt.Set();
		}
		else
		{
			while (true)
			{
				Sync::MutexUsage mutUsage(me->taskMut);
				j = me->tasks.GetCount();
				if (i < j)
				{
					if (me->tasks.GetItem(i).SetTo(vec))
					{
						mutUsage.EndUse();
						i++;
						me->DrawVector(vec);
					}
					else
					{
						me->tasks.RemoveAt(i);
						i = 0;
						mutUsage.EndUse();
						continue;
					}
				}
				else
				{
					me->taskFinish = true;
					mutUsage.EndUse();
					me->finishEvt.Set();
					break;
				}
			}
		}
		me->taskEvt.Wait();
	}
	me->threadRunning = false;
	me->finishEvt.Set();
	return 0;
}

void Map::MapScheduler::DrawVector(NN<Math::Geometry::Vector2D> vec)
{
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		this->DrawPoint(NN<Math::Geometry::Point>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		this->DrawPolyline(NN<Math::Geometry::Polyline>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::LineString:
		this->DrawLineString(NN<Math::Geometry::LineString>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		this->DrawPolygon(NN<Math::Geometry::Polygon>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		this->DrawMultiPolygon(NN<Math::Geometry::MultiPolygon>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
		this->DrawGeometryCollection(NN<Math::Geometry::GeometryCollection>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		this->DrawMultiSurface(NN<Math::Geometry::MultiSurface>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
		this->DrawCurvePolygon(NN<Math::Geometry::CurvePolygon>::ConvertFrom(vec));
		break;
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
	case Math::Geometry::Vector2D::VectorType::LinearRing:
	case Math::Geometry::Vector2D::VectorType::Image:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::Ellipse:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	case Math::Geometry::Vector2D::VectorType::Unknown:
		printf("MapScheduler: unsupported type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v.Ptr());
		break;
	}
}

void Map::MapScheduler::DrawPoint(NN<Math::Geometry::Point> pt)
{
	NN<Media::DrawImage> img;
	if (this->ico.SetTo(img))
	{
		Math::RectAreaDbl *objPtr = &this->objBounds[*this->objCnt];
		Math::Coord2DDbl pts;
		Double imgW;
		Double imgH;
		Double scale = this->img->GetHDPI() / img->GetHDPI();
		Double scnW = this->map->GetScnWidth();
		Double scnH = this->map->GetScnHeight();
		Double spotX = this->icoSpotX * scale;
		Double spotY = this->icoSpotY * scale;
		imgW = UOSInt2Double(img->GetWidth()) * scale;
		imgH = UOSInt2Double(img->GetHeight()) * scale;
		pts = this->map->MapXYToScnXY(pt->GetCenter());
		*this->isLayerEmpty = false;
		if (*this->objCnt >= this->maxCnt)
		{
			--(*this->objCnt);
			objPtr -= 4;
		}
		objPtr->min.x = pts.x - spotX;
		objPtr->min.y = pts.y - spotY;
		objPtr->max.x = objPtr->min.x + imgW;
		objPtr->max.y = objPtr->min.y + imgH;
		if (objPtr->min.x < scnW && objPtr->min.y < scnH && objPtr->max.x >= 0 && objPtr->max.y >= 0)
		{
			this->img->DrawImagePt(img, objPtr->min);
			objPtr += 1;
			++*(this->objCnt);
		}
	}
	else
	{
		Math::Coord2DDbl pts;
		pts = this->map->MapXYToScnXY(pt->GetCenter());
		Double scale = this->img->GetHDPI() / 72;
		this->img->DrawRect(pts - 6 * scale, Math::Size2DDbl(13 * scale, 13 * scale), 0, this->b);
	}
}

void Map::MapScheduler::DrawLineString(NN<Math::Geometry::LineString> pl)
{
	NN<Media::DrawPen> nnp;
	if (!this->p.SetTo(nnp))
	{
		return;
	}
	UOSInt nPoint;
	UnsafeArray<Math::Coord2DDbl> pointArr = pl->GetPointList(nPoint);
	if (this->isFirst)
	{
		if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
			*this->isLayerEmpty = false;
	}

	this->img->DrawPolyline(pointArr, nPoint, nnp);
}

void Map::MapScheduler::DrawPolyline(NN<Math::Geometry::Polyline> pl)
{
	NN<Media::DrawPen> nnp;
	if (!this->p.SetTo(nnp))
	{
		return;
	}
	NN<Math::Geometry::LineString> lineString;
	UOSInt nPoint;
	UnsafeArray<Math::Coord2DDbl> pointArr;
	Data::ArrayIterator<NN<Math::Geometry::LineString>> it;
	if (this->isFirst)
	{
		it = pl->Iterator();
		while (it.HasNext())
		{
			lineString = it.Next();
			pointArr = lineString->GetPointList(nPoint);
			if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
				*this->isLayerEmpty = false;
		}
	}

	if (pl->HasColor())
	{
		NN<Media::DrawPen> p = this->img->NewPenARGB(pl->GetColor(), nnp->GetThick(), 0 ,0);
		it = pl->Iterator();
		while (it.HasNext())
		{
			lineString = it.Next();
			pointArr = lineString->GetPointList(nPoint);
			this->img->DrawPolyline(pointArr, nPoint, p);
		}
		this->img->DelPen(p);
	}
	else
	{
		it = pl->Iterator();
		while (it.HasNext())
		{
			lineString = it.Next();
			pointArr = lineString->GetPointList(nPoint);
			this->img->DrawPolyline(pointArr, nPoint, nnp);
		}
	}
}

void Map::MapScheduler::DrawPolygon(NN<Math::Geometry::Polygon> pg)
{
	UOSInt nPoint;
	UnsafeArray<Math::Coord2DDbl> pointArr;
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it;
	if (this->isFirst)
	{
		it = pg->Iterator();
		while (it.HasNext())
		{
			pointArr = it.Next()->GetPointList(nPoint);
			if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
				*this->isLayerEmpty = false;
		}
	}
	UOSInt i = 0;
	UOSInt nPtOfst = pg->GetCount();
	UInt32 *ptOfstArr;
	UInt32 *relArr = 0;
	UInt32 cntArr[20];
	if (nPtOfst <= 20)
		ptOfstArr = cntArr;
	else
	{
		relArr = MemAlloc(UInt32, nPtOfst);
		ptOfstArr = relArr;
	}
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	it = pg->Iterator();
	while (it.HasNext())
	{
		pointArr = it.Next()->GetPointList(nPoint);
		ptList.AddRange(pointArr, nPoint);
		ptOfstArr[i] = (UInt32)nPoint;
		i++;
	}
	this->img->DrawPolyPolygon(ptList.Arr().Ptr(), ptOfstArr, nPtOfst, this->p, this->b);
	if (relArr)
	{
		MemFree(relArr);
	}
}

void Map::MapScheduler::DrawMultiPolygon(NN<Math::Geometry::MultiPolygon> mpg)
{
	Data::ArrayIterator<NN<Math::Geometry::Polygon>> it = mpg->Iterator();
	while (it.HasNext())
	{
		this->DrawPolygon(it.Next());
	}
}

void Map::MapScheduler::DrawMultiSurface(NN<Math::Geometry::MultiSurface> ms)
{
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = ms->Iterator();
	while (it.HasNext())
	{
		this->DrawVector(it.Next());
	}
}

void Map::MapScheduler::DrawCurvePolygon(NN<Math::Geometry::CurvePolygon> cp)
{
	Data::ArrayList<UInt32> ptOfst;
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	UOSInt nPoint;
	NN<Math::Geometry::Vector2D> vec;
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = cp->Iterator();
	while (it.HasNext())
	{
		vec = it.Next();
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			ptOfst.Add((UInt32)ptList.GetCount());
			NN<Math::Geometry::CompoundCurve>::ConvertFrom(vec)->GetDrawPoints(ptList);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
		{
			ptOfst.Add((UInt32)ptList.GetCount());
			UnsafeArray<const Math::Coord2DDbl> ptArr = NN<Math::Geometry::LineString>::ConvertFrom(vec)->GetPointListRead(nPoint);
			ptList.AddRange(ptArr, nPoint);
		}
		else
		{
			printf("MapScheduler: DrawCurvePolygon unexpected type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v.Ptr());
		}
	}
	if (ptList.GetCount() > 0)
	{
		UnsafeArray<Math::Coord2DDbl> pointArr = ptList.GetArr(nPoint);
		UOSInt nPtOfst;
		UInt32 *ptOfstArr = ptOfst.GetArr(nPtOfst).Ptr();
		if (this->isFirst)
		{
			UOSInt k;
			UOSInt l;

			if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
				*this->isLayerEmpty = false;
			k = nPtOfst;
			l = 1;
			while (l < k)
			{
				ptOfstArr[l - 1] = ptOfstArr[l] - ptOfstArr[l - 1];
				l++;
			}
			ptOfstArr[k - 1] = (UInt32)(nPoint - ptOfstArr[k - 1]);
		}
		this->img->DrawPolyPolygon(pointArr, ptOfstArr, nPtOfst, this->p, this->b);
	}
}

void Map::MapScheduler::DrawGeometryCollection(NN<Math::Geometry::GeometryCollection> geomColl)
{
	Data::ArrayIterator<NN<Math::Geometry::Vector2D>> it = geomColl->Iterator();
	while (it.HasNext())
	{
		this->DrawVector(it.Next());
	}
}

Map::MapScheduler::MapScheduler()
{
	this->map = 0;
	this->lyr = 0;
	this->img = 0;
	this->p = 0;
	this->b = 0;
	this->ico = 0;
	this->dt = ThreadState::Drawing;
	this->toStop = false;
	this->threadRunning = false;
	this->taskFinish = true;
	this->isLayerEmpty = 0;
	Sync::ThreadUtil::Create(MapThread, this);
}

Map::MapScheduler::~MapScheduler()
{
	this->toStop = true;
	this->taskEvt.Set();
	while (this->threadRunning)
	{
		this->finishEvt.Wait();
	}
}

void Map::MapScheduler::SetMapView(NN<Map::MapView> map, NN<Media::DrawImage> img)
{
	this->map = map.Ptr();
	this->img = img.Ptr();
}

void Map::MapScheduler::SetDrawType(NN<Map::MapDrawLayer> lyr, Optional<Media::DrawPen> p, Optional<Media::DrawBrush> b, Optional<Media::DrawImage> ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty)
{
	while (this->dt == ThreadState::Clearing)
	{
		this->finishEvt.Wait();
	}
	this->dt = ThreadState::Drawing;
	this->p = p;
	this->b = b;
	this->ico = ico;
	this->icoSpotX = icoSpotX;
	this->icoSpotY = icoSpotY;
	this->lyr = lyr.Ptr();
	this->isFirst = true;
	this->isLayerEmpty = isLayerEmpty;
}

void Map::MapScheduler::SetDrawObjs(UnsafeArray<Math::RectAreaDbl> objBounds, UOSInt *objCnt, UOSInt maxCnt)
{
	this->objBounds = objBounds;
	this->objCnt = objCnt;
	this->maxCnt = maxCnt;
}

void Map::MapScheduler::Draw(NN<Math::Geometry::Vector2D> vec)
{
	Sync::MutexUsage mutUsage(this->taskMut);
	this->taskFinish = false;
	this->tasks.Add(vec);
	mutUsage.EndUse();
	this->taskEvt.Set();
}

void Map::MapScheduler::DrawNextType(Optional<Media::DrawPen> p, Optional<Media::DrawBrush> b)
{
	while (!this->taskFinish)
	{
		this->finishEvt.Wait();
	}
	NN<Media::DrawBrush> nnb;
	if (this->b.SetTo(nnb))
	{
		this->img->DelBrush(nnb);
		this->b = 0;
	}
	NN<Media::DrawPen> nnp;
	if (this->p.SetTo(nnp))
	{
		this->img->DelPen(nnp);
		this->p = 0;
	}
	this->p = p;
	this->b = b;
	Sync::MutexUsage mutUsage(this->taskMut);
	this->taskFinish = false;
	this->isFirst = false;
	this->tasks.Add(0);
	mutUsage.EndUse();
	this->taskEvt.Set();
}

void Map::MapScheduler::WaitForFinish()
{
	while (!this->taskFinish)
	{
		this->finishEvt.Wait();
	}
	NN<Media::DrawBrush> b;
	if (this->b.SetTo(b))
	{
		this->img->DelBrush(b);
		this->b = 0;
	}
	NN<Media::DrawPen> p;
	if (this->p.SetTo(p))
	{
		this->img->DelPen(p);
		this->p = 0;
	}
	this->dt = ThreadState::Clearing;
	this->taskEvt.Set();
}
