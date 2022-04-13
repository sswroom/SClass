#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapScheduler.h"
#include "Math/Math.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

UInt32 __stdcall Map::MapScheduler::MapThread(void *obj)
{
	Map::MapScheduler *me;
	UInt32 i = 0;
	UOSInt j;

	me = (Map::MapScheduler*)obj;
	me->threadRunning = true;
	while (!me->toStop)
	{
		Map::DrawObjectL *dobj;
		if (me->dt == Map::MapScheduler::MSDT_CLEAR)
		{
			Sync::MutexUsage mutUsage(&me->taskMut);
			j = me->tasks.GetCount();
			while (j-- > 0)
			{
				dobj = me->tasks.GetItem(j);
				me->lyr->ReleaseObject(0, dobj);
			}
			me->tasks.Clear();
			i = 0;
			me->dt = Map::MapScheduler::MSDT_POINTS;
			mutUsage.EndUse();
			me->finishEvt.Set();
		}
		else
		{
			while (true)
			{
				Sync::MutexUsage mutUsage(&me->taskMut);
				j = me->tasks.GetCount();
				if (i < j)
				{
					dobj = me->tasks.GetItem(i);
					if (dobj)
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

				if (me->dt == Map::MapScheduler::MSDT_POINTS)
				{
					me->DrawPoints(dobj);
				}
				else if (me->dt == Map::MapScheduler::MSDT_POLYLINE)
				{
					UInt32 k;
					UInt32 l;
					if (me->isFirst)
					{
						if (me->map->MapXYToScnXY(dobj->pointArr, dobj->pointArr, dobj->nPoint, 0, 0))
							*me->isLayerEmpty = false;
					}

					if (dobj->flags & 1)
					{
						Media::DrawPen *p = me->img->NewPenARGB(dobj->lineColor, me->p->GetThick(), 0 ,0);
						k = dobj->nPtOfst;
						l = 1;
						while (l < k)
						{
							me->img->DrawPolyline(&dobj->pointArr[dobj->ptOfstArr[l - 1] << 1], dobj->ptOfstArr[l] - dobj->ptOfstArr[l - 1], p);
							l++;
						}
						me->img->DrawPolyline(&dobj->pointArr[dobj->ptOfstArr[k - 1] << 1], dobj->nPoint - dobj->ptOfstArr[k - 1], p);
						me->img->DelPen(p);
					}
					else
					{
						k = dobj->nPtOfst;
						l = 1;
						while (l < k)
						{
							me->img->DrawPolyline(&dobj->pointArr[dobj->ptOfstArr[l - 1] << 1], dobj->ptOfstArr[l] - dobj->ptOfstArr[l - 1], me->p);
							l++;
						}
						me->img->DrawPolyline(&dobj->pointArr[dobj->ptOfstArr[k - 1] << 1], dobj->nPoint - dobj->ptOfstArr[k - 1], me->p);
					}
				}
				else if (me->dt == Map::MapScheduler::MSDT_POLYGON)
				{
					if (me->isFirst)
					{
						UInt32 k;
						UInt32 l;
						if (me->map->MapXYToScnXY(dobj->pointArr, dobj->pointArr, dobj->nPoint, 0, 0))
							*me->isLayerEmpty = false;
						k = dobj->nPtOfst;
						l = 1;
						while (l < k)
						{
							dobj->ptOfstArr[l - 1] = dobj->ptOfstArr[l] - dobj->ptOfstArr[l - 1];
							l++;
						}
						dobj->ptOfstArr[k - 1] = dobj->nPoint - dobj->ptOfstArr[k - 1];
					}

					me->img->DrawPolyPolygon(dobj->pointArr, dobj->ptOfstArr, dobj->nPtOfst, me->p, me->b);
				}
			}
		}
		me->taskEvt.Wait();
	}
	me->threadRunning = false;
	me->finishEvt.Set();
	return 0;
}

void Map::MapScheduler::DrawPoints(Map::DrawObjectL *dobj)
{
	UOSInt j;
	Double *objPtr = &this->objBounds[4 * *this->objCnt];
	Double pts[2];
	Double imgW;
	Double imgH;
	Double scale = this->img->GetHDPI() / this->ico->GetHDPI();
	Double scnW = this->map->GetScnWidth();
	Double scnH = this->map->GetScnHeight();
	Double spotX = this->icoSpotX * scale;
	Double spotY = this->icoSpotY * scale;
	imgW = UOSInt2Double(this->ico->GetWidth()) * scale;
	imgH = UOSInt2Double(this->ico->GetHeight()) * scale;
	j = dobj->nPoint;
	while (j-- > 0)
	{
		if (this->map->MapXYToScnXY(&dobj->pointArr[j << 1], pts, 1, 0, 0))
			*this->isLayerEmpty = false;
		if (*this->objCnt >= this->maxCnt)
		{
			--(*this->objCnt);
			objPtr -= 4;
		}
		objPtr[0] = pts[0] - spotX;
		objPtr[1] = pts[1] - spotY;
		objPtr[2] = objPtr[0] + imgW;
		objPtr[3] = objPtr[1] + imgH;
		if (objPtr[0] < scnW && objPtr[1] < scnH && objPtr[2] >= 0 && objPtr[3] >= 0)
		{
			this->img->DrawImagePt(this->ico, objPtr[0], objPtr[1]);
			objPtr += 4;
			++*(this->objCnt);
		}
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
	this->dt = Map::MapScheduler::MSDT_POINTS;
	this->toStop = false;
	this->threadRunning = false;
	this->taskFinish = true;
	this->isLayerEmpty = 0;
	Sync::Thread::Create(MapThread, this);
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

void Map::MapScheduler::SetMapView(Map::MapView *map, Media::DrawImage *img)
{
	this->map = map;
	this->img = img;
}

void Map::MapScheduler::SetDrawType(Map::IMapDrawLayer *lyr, DrawType dt, Media::DrawPen *p, Media::DrawBrush *b, Media::DrawImage *ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty)
{
	while (this->dt == Map::MapScheduler::MSDT_CLEAR)
	{
		this->finishEvt.Wait();
	}
	this->dt = dt;
	this->p = p;
	this->b = b;
	this->ico = ico;
	this->icoSpotX = icoSpotX;
	this->icoSpotY = icoSpotY;
	this->lyr = lyr;
	this->isFirst = true;
	this->isLayerEmpty = isLayerEmpty;
}

void Map::MapScheduler::SetDrawObjs(Double *objBounds, UOSInt *objCnt, UOSInt maxCnt)
{
	this->objBounds = objBounds;
	this->objCnt = objCnt;
	this->maxCnt = maxCnt;
}

void Map::MapScheduler::Draw(Map::DrawObjectL *obj)
{
	Sync::MutexUsage mutUsage(&this->taskMut);
	this->taskFinish = false;
	this->tasks.Add(obj);
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
	Sync::MutexUsage mutUsage(&this->taskMut);
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
	this->dt = Map::MapScheduler::MSDT_CLEAR;
	this->taskEvt.Set();
}
