#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapScheduler.h"
#include "Math/Math.h"
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
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
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
	UOSInt nPoint;
	Math::Coord2DDbl *pointArr = pl->GetPointList(nPoint);
	if (this->isFirst)
	{
		if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
			*this->isLayerEmpty = false;
	}

	this->img->DrawPolyline(pointArr, nPoint, this->p);
}

void Map::MapScheduler::DrawPolyline(Math::Geometry::Polyline *pl)
{
	UOSInt nPoint;
	Math::Coord2DDbl *pointArr = pl->GetPointList(nPoint);
	UOSInt nPtOfst;
	UInt32 *ptOfstArr = pl->GetPtOfstList(nPtOfst);
	UOSInt k;
	UOSInt l;
	if (this->isFirst)
	{
		if (this->map->MapXYToScnXY(pointArr, pointArr, nPoint, Math::Coord2DDbl(0, 0)))
			*this->isLayerEmpty = false;
	}

	if (pl->HasColor())
	{
		Media::DrawPen *p = this->img->NewPenARGB(pl->GetColor(), this->p->GetThick(), 0 ,0);
		k = nPtOfst;
		l = 1;
		while (l < k)
		{
			this->img->DrawPolyline(&pointArr[ptOfstArr[l - 1]], ptOfstArr[l] - ptOfstArr[l - 1], p);
			l++;
		}
		this->img->DrawPolyline(&pointArr[ptOfstArr[k - 1]], nPoint - ptOfstArr[k - 1], p);
		this->img->DelPen(p);
	}
	else
	{
		k = nPtOfst;
		l = 1;
		while (l < k)
		{
			this->img->DrawPolyline(&pointArr[ptOfstArr[l - 1]], ptOfstArr[l] - ptOfstArr[l - 1], this->p);
			l++;
		}
		this->img->DrawPolyline(&pointArr[ptOfstArr[k - 1]], nPoint - ptOfstArr[k - 1], this->p);
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

void Map::MapScheduler::SetMapView(Map::MapView *map, NotNullPtr<Media::DrawImage> img)
{
	this->map = map;
	this->img = img.Ptr();
}

void Map::MapScheduler::SetDrawType(NotNullPtr<Map::MapDrawLayer> lyr, Media::DrawPen *p, Media::DrawBrush *b, Media::DrawImage *ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty)
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

void Map::MapScheduler::Draw(Math::Geometry::Vector2D *vec)
{
	Sync::MutexUsage mutUsage(this->taskMut);
	this->taskFinish = false;
	this->tasks.Add(vec);
	mutUsage.EndUse();
	this->taskEvt.Set();
}

void Map::MapScheduler::DrawNextType(Media::DrawPen *p, Media::DrawBrush *b)
{
	while (!this->taskFinish)
	{
		this->finishEvt.Wait();
	}
	if (this->b)
	{
		this->img->DelBrush(this->b);
		this->b = 0;
	}
	if (this->p)
	{
		this->img->DelPen(this->p);
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
	if (this->b)
	{
		this->img->DelBrush(this->b);
		this->b = 0;
	}
	if (this->p)
	{
		this->img->DelPen(this->p);
		this->p = 0;
	}
	this->dt = ThreadState::Clearing;
	this->taskEvt.Set();
}
