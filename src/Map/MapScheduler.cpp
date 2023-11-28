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

UInt32 __stdcall Map::MapScheduler::MapThread(void *obj)
{
	Map::MapScheduler *me;
	UInt32 i = 0;
	UOSInt j;

	me = (Map::MapScheduler*)obj;
	me->threadRunning = true;
	while (!me->toStop)
	{
		Math::Geometry::Vector2D *vec;
		if (me->dt == ThreadState::Clearing)
		{
			Sync::MutexUsage mutUsage(me->taskMut);
			j = me->tasks.GetCount();
			while (j-- > 0)
			{
				vec = me->tasks.GetItem(j);
				DEL_CLASS(vec);
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
					vec = me->tasks.GetItem(i);
					if (vec)
					{
						mutUsage.EndUse();
						i++;
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
				me->DrawVector(vec);
			}
		}
		me->taskEvt.Wait();
	}
	me->threadRunning = false;
	me->finishEvt.Set();
	return 0;
}

void Map::MapScheduler::DrawVector(Math::Geometry::Vector2D *vec)
{
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		this->DrawPoint((Math::Geometry::Point*)vec);
		break;
	case Math::Geometry::Vector2D::VectorType::Polyline:
		this->DrawPolyline((Math::Geometry::Polyline*)vec);
		break;
	case Math::Geometry::Vector2D::VectorType::LineString:
		this->DrawLineString((Math::Geometry::LineString*)vec);
		break;
	case Math::Geometry::Vector2D::VectorType::Polygon:
		this->DrawPolygon((Math::Geometry::Polygon*)vec);
		break;
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		this->DrawMultiPolygon((Math::Geometry::MultiPolygon*)vec);
		break;
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
		this->DrawGeometryCollection((Math::Geometry::GeometryCollection*)vec);
		break;
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		this->DrawMultiSurface((Math::Geometry::MultiSurface*)vec);
		break;
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
		this->DrawCurvePolygon((Math::Geometry::CurvePolygon*)vec);
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
		printf("MapScheduler: unsupported type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v);
		break;
	}
}

void Map::MapScheduler::DrawPoint(Math::Geometry::Point *pt)
{
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->ico))
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
		objPtr->tl.x = pts.x - spotX;
		objPtr->tl.y = pts.y - spotY;
		objPtr->br.x = objPtr->tl.x + imgW;
		objPtr->br.y = objPtr->tl.y + imgH;
		if (objPtr->tl.x < scnW && objPtr->tl.y < scnH && objPtr->br.x >= 0 && objPtr->br.y >= 0)
		{
			this->img->DrawImagePt(img, objPtr->tl);
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

void Map::MapScheduler::DrawLineString(Math::Geometry::LineString *pl)
{
	NotNullPtr<Media::DrawPen> nnp;
	if (!this->p.SetTo(nnp))
	{
		return;
	}
	UOSInt nPoint;
	Math::Coord2DDbl *pointArr = pl->GetPointList(nPoint);
	if (this->isFirst)
	{
		if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
			*this->isLayerEmpty = false;
	}

	this->img->DrawPolyline(pointArr, nPoint, nnp);
}

void Map::MapScheduler::DrawPolyline(Math::Geometry::Polyline *pl)
{
	NotNullPtr<Media::DrawPen> nnp;
	if (!this->p.SetTo(nnp))
	{
		return;
	}
	Math::Geometry::LineString *lineString;
	UOSInt nPoint;
	Math::Coord2DDbl *pointArr;
	UOSInt i;
	UOSInt j;
	if (this->isFirst)
	{
		i = 0;
		j = pl->GetCount();
		while (i < j)
		{
			lineString = pl->GetItem(i);
			pointArr = lineString->GetPointList(nPoint);
			if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
				*this->isLayerEmpty = false;
			i++;
		}
	}

	if (pl->HasColor())
	{
		NotNullPtr<Media::DrawPen> p = this->img->NewPenARGB(pl->GetColor(), nnp->GetThick(), 0 ,0);
		i = 0;
		j = pl->GetCount();
		while (i < j)
		{
			lineString = pl->GetItem(i);
			pointArr = lineString->GetPointList(nPoint);
			this->img->DrawPolyline(pointArr, nPoint, p);
			i++;
		}
		this->img->DelPen(p);
	}
	else
	{
		i = 0;
		j = pl->GetCount();
		while (i < j)
		{
			lineString = pl->GetItem(i);
			pointArr = lineString->GetPointList(nPoint);
			this->img->DrawPolyline(pointArr, nPoint, nnp);
			i++;
		}
	}
}

void Map::MapScheduler::DrawPolygon(Math::Geometry::Polygon *pg)
{
	UOSInt nPoint;
	Math::Coord2DDbl *pointArr = pg->GetPointList(nPoint);
	UOSInt nPtOfst;
	UInt32 *ptOfstArr = pg->GetPtOfstList(nPtOfst);
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

void Map::MapScheduler::DrawMultiPolygon(Math::Geometry::MultiPolygon *mpg)
{
	UOSInt pgInd = mpg->GetCount();
	while (pgInd-- > 0)
	{
		Math::Geometry::Polygon *pg = mpg->GetItem(pgInd);
		UOSInt nPoint;
		Math::Coord2DDbl *pointArr = pg->GetPointList(nPoint);
		UOSInt nPtOfst;
		UInt32 *ptOfstArr = pg->GetPtOfstList(nPtOfst);
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

void Map::MapScheduler::DrawMultiSurface(Math::Geometry::MultiSurface *ms)
{
	UOSInt pgInd = ms->GetCount();
	while (pgInd-- > 0)
	{
		this->DrawVector(ms->GetItem(pgInd));
	}
}

void Map::MapScheduler::DrawCurvePolygon(Math::Geometry::CurvePolygon *cp)
{
	Data::ArrayList<UInt32> ptOfst;
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	UOSInt nPoint;
	Math::Geometry::Vector2D *vec;
	UOSInt i = 0;
	UOSInt j = cp->GetCount();
	while (i < j)
	{
		vec = cp->GetItem(i);
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			ptOfst.Add((UInt32)ptList.GetCount());
			((Math::Geometry::CompoundCurve*)vec)->GetDrawPoints(ptList);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
		{
			ptOfst.Add((UInt32)ptList.GetCount());
			const Math::Coord2DDbl *ptArr = ((Math::Geometry::LineString*)vec)->GetPointListRead(nPoint);
			ptList.AddRange(ptArr, nPoint);
		}
		else
		{
			printf("MapScheduler: DrawCurvePolygon unexpected type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v);
		}
		i++;
	}
	if (ptList.GetCount() > 0)
	{
		Math::Coord2DDbl *pointArr = ptList.GetPtr(nPoint);
		UOSInt nPtOfst;
		UInt32 *ptOfstArr = ptOfst.GetPtr(nPtOfst);
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

void Map::MapScheduler::DrawGeometryCollection(Math::Geometry::GeometryCollection *geomColl)
{
	UOSInt pgInd = geomColl->GetCount();
	while (pgInd-- > 0)
	{
		this->DrawVector(geomColl->GetItem(pgInd));
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

void Map::MapScheduler::SetMapView(NotNullPtr<Map::MapView> map, NotNullPtr<Media::DrawImage> img)
{
	this->map = map.Ptr();
	this->img = img.Ptr();
}

void Map::MapScheduler::SetDrawType(NotNullPtr<Map::MapDrawLayer> lyr, Optional<Media::DrawPen> p, Optional<Media::DrawBrush> b, Media::DrawImage *ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty)
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

void Map::MapScheduler::SetDrawObjs(Math::RectAreaDbl *objBounds, UOSInt *objCnt, UOSInt maxCnt)
{
	this->objBounds = objBounds;
	this->objCnt = objCnt;
	this->maxCnt = maxCnt;
}

void Map::MapScheduler::Draw(NotNullPtr<Math::Geometry::Vector2D> vec)
{
	Sync::MutexUsage mutUsage(this->taskMut);
	this->taskFinish = false;
	this->tasks.Add(vec.Ptr());
	mutUsage.EndUse();
	this->taskEvt.Set();
}

void Map::MapScheduler::DrawNextType(Optional<Media::DrawPen> p, Optional<Media::DrawBrush> b)
{
	while (!this->taskFinish)
	{
		this->finishEvt.Wait();
	}
	NotNullPtr<Media::DrawBrush> nnb;
	if (this->b.SetTo(nnb))
	{
		this->img->DelBrush(nnb);
		this->b = 0;
	}
	NotNullPtr<Media::DrawPen> nnp;
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
	NotNullPtr<Media::DrawBrush> b;
	if (this->b.SetTo(b))
	{
		this->img->DelBrush(b);
		this->b = 0;
	}
	NotNullPtr<Media::DrawPen> p;
	if (this->p.SetTo(p))
	{
		this->img->DelPen(p);
		this->p = 0;
	}
	this->dt = ThreadState::Clearing;
	this->taskEvt.Set();
}
