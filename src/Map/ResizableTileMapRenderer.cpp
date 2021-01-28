#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include "Data/ArrayListInt64.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Map/ResizableTileMapRenderer.h"
#include <windows.h>

UInt32 __stdcall Map::ResizableTileMapRenderer::TaskThread(void *userObj)
{
	Double bounds[4];
	Media::ImageList *imgList;
	CachedImage *cimg;
	ThreadStat *stat = (ThreadStat*)userObj;
	stat->running = true;
	while (!stat->toStop)
	{
		while (!stat->toStop)
		{
			stat->me->taskMut->Lock();
			cimg = (CachedImage*)stat->me->taskQueued->Get();
			stat->me->taskMut->Unlock();
			if (cimg == 0)
				break;

			if (cimg->isCancel)
			{
				cimg->isFinish = true;
			}
			else
			{
				imgList = stat->me->map->LoadTileImage(cimg->level, cimg->imgId, stat->me->parsers, bounds, false);
				if (imgList)
				{
					cimg->img = imgList->GetImage(0, 0)->CreateStaticImage();
					cimg->isFinish = true;
					DEL_CLASS(imgList);
					if (stat->me->updHdlr)
					{
						stat->me->updHdlr(stat->me->updObj);
					}
				}
				else
				{
					cimg->isFinish = true;
				}
			}
		}
		stat->evt->Wait(1000);
	}
	stat->running = false;
	return 0;
}

void Map::ResizableTileMapRenderer::AddTask(CachedImage *cimg)
{
	this->taskMut->Lock();
	this->taskQueued->Put(cimg);
	this->taskMut->Unlock();
	this->threads[this->threadNext].evt->Set();
	this->threadNext = (this->threadNext + 1) % this->threadCnt;
}

void Map::ResizableTileMapRenderer::DrawImage(Map::MapView *view, Media::DrawImage *img, CachedImage *cimg)
{
	Double scnX;
	Double scnY;
	Double scnX2;
	Double scnY2;
	Int32 iScnX;
	Int32 iScnY;
	Int32 iScnX2;
	Int32 iScnY2;
	Int32 imgW;
	Int32 imgH;
	Int32 cimgX2;
	Int32 cimgY2;
	Int32 cimgX;
	Int32 cimgY;

	imgW = img->GetWidth();
	imgH = img->GetHeight();

	view->LatLonToScnXY(cimg->tly, cimg->tlx, &scnX, &scnY);
	view->LatLonToScnXY(cimg->bry, cimg->brx, &scnX2, &scnY2);
	iScnX = Math::Double2Int(scnX);
	iScnY = Math::Double2Int(scnY);
	iScnX2 = Math::Double2Int(scnX2);
	iScnY2 = Math::Double2Int(scnY2);
	if (iScnX < iScnX2 && iScnY < iScnY2)
	{
		if (cimg->img)
		{
			if (imgW > (iScnX2 -iScnX) || imgH > (iScnY2 - iScnY))
			{
				this->resizer->SetTargetWidth(iScnX2 - iScnX);
				this->resizer->SetTargetHeight(iScnY2 - iScnY);
				this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
				Media::StaticImage *newImg = this->resizer->ProcessToNew(cimg->img);
				if (newImg)
				{
					Media::DrawImage *dimg = this->eng->ConvImage(newImg);
					img->DrawImagePt(dimg, iScnX, iScnY);
					this->eng->DeleteImage(dimg);
					DEL_CLASS(newImg);
				}
			}
			else
			{
				cimgX2 = cimg->img->info->width;
				cimgY2 = cimg->img->info->height;
				cimgX = 0;
				cimgY = 0;
				if (iScnX < 0)
				{
					cimgX = MulDiv(cimgX2, -iScnX, (iScnX2 - iScnX));
					iScnX = 0;
				}
				if (iScnY < 0)
				{
					cimgY = MulDiv(cimgY2, -iScnY, (iScnY2 - iScnY));
					iScnY = 0;
				}
				if (iScnX2 > imgW)
				{
					cimgX2 = cimgX + MulDiv(cimgX2 - cimgX, imgW - iScnX, iScnX2 - iScnX);
					iScnX2 = imgW;
				}
				if (iScnY2 > imgH)
				{
					cimgY2 = cimgY + MulDiv(cimgY2 - cimgY, imgH - iScnY, iScnY2 - iScnY);
					iScnY2 = imgH;
				}
				if (cimgX == cimgX2)
				{
					if (cimgX2 >= cimg->img->info->width)
					{
						cimgX = cimgX2 - 1;
					}
					else
					{
						cimgX2++;
					}
				}
				if (cimgY == cimgY2)
				{
					if (cimgY2 >= cimg->img->info->height)
					{
						cimgY = cimgY2 - 1;
					}
					else
					{
						cimgY2++;
					}
				}

				this->resizer->SetTargetWidth(iScnX2 - iScnX);
				this->resizer->SetTargetHeight(iScnY2 - iScnY);
				this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
				Media::StaticImage *newImg = this->resizer->ProcessToNewPartial(cimg->img, cimgX, cimgY, cimgX2, cimgY2);
				if (newImg)
				{
					Media::DrawImage *dimg = this->eng->ConvImage(newImg);
					img->DrawImagePt(dimg, iScnX, iScnY);
					this->eng->DeleteImage(dimg);
					DEL_CLASS(newImg);
				}
			}
		}
		else
		{
			if (iScnX < 0)
			{
				iScnX = 0;
			}
			if (iScnY < 0)
			{
				iScnY = 0;
			}
			if (iScnX2 > img->GetWidth())
			{
				iScnX2 = img->GetWidth();
			}
			if (iScnY2 > img->GetHeight())
			{
				iScnY2 = img->GetHeight();
			}
			
			Media::DrawBrush *emptyBrush = img->NewBrushARGB(0xffffffff);
			img->DrawRect(iScnX, iScnY, iScnX2 - iScnX, iScnY2 - iScnY, 0, emptyBrush);
			img->DelBrush(emptyBrush);
		}
	}
}

Map::ResizableTileMapRenderer::ResizableTileMapRenderer(Media::DrawEngine *eng, Map::TileMap *map, Parser::ParserList *parsers, Media::ColorManager *colorMgr, Media::CS::TransferType outputRGBType, Double outputGamma)
{
	this->eng = eng;
	this->map = map;
	this->parsers = parsers;
	this->colorMgr = colorMgr;
	this->outputRGBType = outputRGBType;
	this->outputGamma = outputGamma;
	this->lastLevel = -1;
	this->threadNext = 0;
	this->updHdlr = 0;
	this->updObj = 0;
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(3, Media::CS::TRANT_sRGB, outputRGBType, outputGamma, colorMgr));
	NEW_CLASS(this->lastIds, Data::ArrayListInt64());
	NEW_CLASS(this->lastImgs, Data::ArrayList<CachedImage*>());

	NEW_CLASS(this->idleImgs, Data::ArrayList<CachedImage*>());
	NEW_CLASS(this->taskQueued, Data::LinkedList());
	NEW_CLASS(this->taskEvt, Sync::Event(true, L"Map.ResizableTileMapRenderer.taskEvt"));
	NEW_CLASS(this->taskMut, Sync::Mutex());

	OSInt i;
	this->threadCnt = this->map->GetConcurrentCount();
	if (this->threadCnt <= 0)
		this->threadCnt = 1;
	this->threads = MemAlloc(ThreadStat, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].running = false;
		this->threads[i].toStop = false;
		this->threads[i].me = this;
		NEW_CLASS(this->threads[i].evt, Sync::Event(true, L"Map.ResizableTileMapRenderer.threads.evt"));
		Sync::Thread::Create(TaskThread, &this->threads[i]);
	}
}

Map::ResizableTileMapRenderer::~ResizableTileMapRenderer()
{
	CachedImage *cimg;
	OSInt i;
	Bool running;
	this->updHdlr = 0;
	this->updObj = 0;

	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].toStop = true;
		this->threads[i].evt->Set();
	}
	while (true)
	{
		running = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].running)
			{
				running = true;
				this->threads[i].evt->Set();
				break;
			}
		}
		if (!running)
			break;
	}

	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threads[i].evt);
	}
	MemFree(this->threads);

	i = this->lastImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->lastImgs->GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFree(cimg);
	}
	i = this->idleImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs->GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFree(cimg);
	}
	DEL_CLASS(this->taskQueued);
	DEL_CLASS(this->taskEvt);
	DEL_CLASS(this->taskMut);
	DEL_CLASS(this->idleImgs);
	DEL_CLASS(this->lastImgs);
	DEL_CLASS(this->lastIds);
	DEL_CLASS(this->resizer);
}

void Map::ResizableTileMapRenderer::DrawMap(Media::DrawImage *img, Map::MapView *view)
{
	CachedImage *cimg;
	Data::ArrayListInt64 *cacheIds;
	Data::ArrayList<CachedImage *> *cacheImgs;

	Data::ArrayListInt64 *idList;
	OSInt i;
	OSInt j;
	OSInt k;
	Media::ImageList *imgList;
	NEW_CLASS(idList, Data::ArrayListInt64());
	OSInt level = map->GetNearestLevel(view->GetScale());

	NEW_CLASS(cacheIds, Data::ArrayListInt64());
	NEW_CLASS(cacheImgs, Data::ArrayList<CachedImage*>());
	if (this->lastLevel == level)
	{
		cacheIds->AddRange(this->lastIds);
		cacheImgs->AddRange(this->lastImgs);
		this->lastIds->Clear();
		this->lastImgs->Clear();
	}
	else
	{
		j = this->lastImgs->GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs->GetItem(j);
			if (cimg->isFinish)
			{
				if (cimg->img)
				{
					DEL_CLASS(cimg->img);
				}
				MemFree(cimg);
			}
			else
			{
				cimg->isCancel = true;
				this->idleImgs->Add(cimg);
			}
		}
		this->lastIds->Clear();
		this->lastImgs->Clear();
	}

	Double bounds[4];
	Double lat;
	Double lon;
	Double lat2;
	Double lon2;
	lon = view->GetLeftLon();
	lat = view->GetTopLat();
	lon2 = view->GetRightLon();
	lat2 = view->GetBottomLat();
	map->GetImageIDs(level, lon, lat, lon2, lat2, idList);
	i = idList->GetCount();
	while (i-- > 0)
	{
		j = cacheIds->SortedIndexOf(idList->GetItem(i));
		if (j >= 0)
		{
			k = this->lastIds->SortedInsert(cacheIds->RemoveAt(j));
			this->lastImgs->Insert(k, cimg = cacheImgs->RemoveAt(j));

			DrawImage(view, img, cimg);
		}
		else
		{
			imgList = this->map->LoadTileImage(level, idList->GetItem(i), this->parsers, bounds, true);
			if (imgList)
			{
				cimg = MemAlloc(CachedImage, 1);
				cimg->imgId = idList->GetItem(i);
				cimg->tlx = bounds[0];
				cimg->tly = bounds[1];
				cimg->brx = bounds[2];
				cimg->bry = bounds[3];
				cimg->level = level;
				cimg->isFinish = true;
				cimg->isCancel = false;
				cimg->img = imgList->GetImage(0, 0)->CreateStaticImage();
				DrawImage(view, img, cimg);
				DEL_CLASS(imgList);

				k = this->lastIds->SortedInsert(idList->GetItem(i));
				this->lastImgs->Insert(k, cimg);
			}
			else
			{
				cimg = MemAlloc(CachedImage, 1);
				cimg->imgId = idList->GetItem(i);
				cimg->tlx = bounds[0];
				cimg->tly = bounds[1];
				cimg->brx = bounds[2];
				cimg->bry = bounds[3];
				cimg->level = level;
				cimg->isFinish = false;
				cimg->isCancel = false;
				cimg->img = 0;
				AddTask(cimg);
				DrawImage(view, img, cimg);

				k = this->lastIds->SortedInsert(idList->GetItem(i));
				this->lastImgs->Insert(k, cimg);
			}
		}
	}
	DEL_CLASS(idList);

	lastLevel = level;
	i = cacheImgs->GetCount();
	while (i-- > 0)
	{
		cimg = cacheImgs->GetItem(i);
		if (cimg->isFinish)
		{
			if (cimg->img)
			{
				DEL_CLASS(cimg->img);
			}
			MemFree(cimg);
		}
		else
		{
			cimg->isCancel = true;
			this->idleImgs->Add(cimg);
		}
	}
	DEL_CLASS(cacheIds);
	DEL_CLASS(cacheImgs);

	i = this->idleImgs->GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs->GetItem(i);
		if (cimg->isFinish)
		{
			this->idleImgs->RemoveAt(i);
			if (cimg->img)
			{
				DEL_CLASS(cimg->img);
			}
			MemFree(cimg);
		}
	}
	
}

void Map::ResizableTileMapRenderer::SetUpdatedHandler(UpdatedHandler hdlr, void *userObj)
{
	this->updObj = userObj;
	this->updHdlr = hdlr;
}
