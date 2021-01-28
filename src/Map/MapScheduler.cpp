#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapScheduler.h"
#include "Math/Math.h"
#include "Sync/Thread.h"

UInt32 __stdcall Map::MapScheduler::MapThread(void *obj)
{
	Map::MapScheduler *me;
	Int32 i = 0;
	OSInt j;

	me = (Map::MapScheduler*)obj;
	me->threadRunning = true;
	while (!me->toStop)
	{
		Map::DrawObjectL *dobj;
		if (me->dt == Map::MapScheduler::MSDT_CLEAR)
		{
			me->taskMut->Lock();
			j = me->tasks->GetCount();
			while (j-- > 0)
			{
				dobj = me->tasks->GetItem(j);
				me->lyr->ReleaseObject(0, dobj);
			}
			me->tasks->Clear();
			i = 0;
			me->dt = Map::MapScheduler::MSDT_POINTS;
			me->taskMut->Unlock();
			me->finishEvt->Set();
		}
		else
		{
			while (true)
			{
				me->taskMut->Lock();
				j = me->tasks->GetCount();
				if (i < j)
				{
					dobj = me->tasks->GetItem(i);
					if (dobj)
					{
						me->taskMut->Unlock();
						i++;
					}
					else
					{
						me->tasks->RemoveAt(i);
						i = 0;
						me->taskMut->Unlock();
						continue;
					}
				}
				else
				{
					me->taskFinish = true;
					me->taskMut->Unlock();
					me->finishEvt->Set();
					break;
				}

				if (me->dt == Map::MapScheduler::MSDT_POINTS)
				{
					me->DrawPoints(dobj);
				}
				else if (me->dt == Map::MapScheduler::MSDT_POLYLINE)
				{
					Int32 k;
					Int32 l;
					if (me->isFirst)
					{
						if (me->map->MapXYToScnXY(dobj->points, dobj->points, dobj->nPoints, 0, 0))
							*me->isLayerEmpty = false;
					}

					if (dobj->flags & 1)
					{
						Media::DrawPen *p = me->img->NewPenARGB(dobj->lineColor, me->p->GetThick(), 0 ,0);
						k = dobj->nParts;
						l = 1;
						while (l < k)
						{
							me->img->DrawPolyline(&dobj->points[dobj->parts[l - 1] << 1], dobj->parts[l] - dobj->parts[l - 1], p);
							l++;
						}
						me->img->DrawPolyline(&dobj->points[dobj->parts[k - 1] << 1], dobj->nPoints - dobj->parts[k - 1], p);
						me->img->DelPen(p);
					}
					else
					{
						k = dobj->nParts;
						l = 1;
						while (l < k)
						{
							me->img->DrawPolyline(&dobj->points[dobj->parts[l - 1] << 1], dobj->parts[l] - dobj->parts[l - 1], me->p);
							l++;
						}
						me->img->DrawPolyline(&dobj->points[dobj->parts[k - 1] << 1], dobj->nPoints - dobj->parts[k - 1], me->p);
					}
				}
				else if (me->dt == Map::MapScheduler::MSDT_POLYGON)
				{
					if (me->isFirst)
					{
						Int32 k;
						Int32 l;
						if (me->map->MapXYToScnXY(dobj->points, dobj->points, dobj->nPoints, 0, 0))
							*me->isLayerEmpty = false;
						k = dobj->nParts;
						l = 1;
						while (l < k)
						{
							dobj->parts[l - 1] = dobj->parts[l] - dobj->parts[l - 1];
							l++;
						}
						dobj->parts[k - 1] = dobj->nPoints - dobj->parts[k - 1];
					}

					me->img->DrawPolyPolygon(dobj->points, dobj->parts, dobj->nParts, me->p, me->b);
				}
			}
		}
		me->taskEvt->Wait();
	}
	me->threadRunning = false;
	me->finishEvt->Set();
	return 0;
}

void Map::MapScheduler::DrawPoints(Map::DrawObjectL *dobj)
{
	OSInt j;
	Double *objPtr = &this->objBounds[4 * *this->objCnt];
	Double pts[2];
	OSInt imgW;
	OSInt imgH;
	Double scale = this->img->GetHDPI() / this->ico->GetHDPI();
	OSInt scnW = this->map->GetScnWidth();
	OSInt scnH = this->map->GetScnHeight();
	OSInt spotX = Math::Double2Int32(this->icoSpotX * scale);
	OSInt spotY = Math::Double2Int32(this->icoSpotY * scale);
	imgW = Math::Double2Int32(this->ico->GetWidth() * scale);
	imgH = Math::Double2Int32(this->ico->GetHeight() * scale);
	j = dobj->nPoints;
	while (j-- > 0)
	{
		if (this->map->MapXYToScnXY(&dobj->points[j << 1], pts, 1, 0, 0))
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
	NEW_CLASS(taskMut, Sync::Mutex());
	NEW_CLASS(tasks, Data::ArrayList<Map::DrawObjectL *>());
	this->toStop = false;
	NEW_CLASS(taskEvt, Sync::Event((const UTF8Char*)"Map.MapScheduler.TaskEvt"));
	NEW_CLASS(finishEvt, Sync::Event((const UTF8Char*)"Map.MapScheduler.FinishEvt"));
	this->threadRunning = false;
	this->taskFinish = true;
	this->isLayerEmpty = 0;
	Sync::Thread::Create(MapThread, this);
}

Map::MapScheduler::~MapScheduler()
{
	this->toStop = true;
	this->taskEvt->Set();
	while (this->threadRunning)
	{
		this->finishEvt->Wait();
	}
	DEL_CLASS(this->taskEvt);
	DEL_CLASS(this->taskMut);
	DEL_CLASS(this->finishEvt);
	DEL_CLASS(this->tasks);
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
		this->finishEvt->Wait();
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
	this->taskMut->Lock();
	this->taskFinish = false;
	this->tasks->Add(obj);
	this->taskMut->Unlock();
	this->taskEvt->Set();
}

void Map::MapScheduler::DrawNextType(Media::DrawPen *p, Media::DrawBrush *b)
{
	while (!this->taskFinish)
	{
		this->finishEvt->Wait();
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
	this->taskMut->Lock();
	this->taskFinish = false;
	this->isFirst = false;
	this->tasks->Add(0);
	this->taskMut->Unlock();
	this->taskEvt->Set();
}

void Map::MapScheduler::WaitForFinish()
{
	while (!this->taskFinish)
	{
		this->finishEvt->Wait();
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
	this->taskEvt->Set();
}
