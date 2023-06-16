#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "Map/ResizableTileMapRenderer.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

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
			{
				Sync::MutexUsage mutUsage(&stat->me->taskMut);
				cimg = (CachedImage*)stat->me->taskQueued.Get();
			}
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
	{
		Sync::MutexUsage mutUsage(&this->taskMut);
		this->taskQueued.Put(cimg);
	}
	this->threads[this->threadNext].evt->Set();
	this->threadNext = (this->threadNext + 1) % this->threadCnt;
}

void Map::ResizableTileMapRenderer::DrawImage(Map::MapView *view, Media::DrawImage *img, CachedImage *cimg)
{
	Math::Coord2DDbl scnPt;
	Math::Coord2DDbl scnPt2;
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

	scnPt = view->MapXYToScnXY(Math::Coord2DDbl(cimg->tlx, cimg->tly));
	scnPt2 = view->MapXYToScnXY(Math::Coord2DDbl(cimg->brx, cimg->bry));
	iScnX = Double2Int32(scnPt.x);
	iScnY = Double2Int32(scnPt.y);
	iScnX2 = Double2Int32(scnPt2.x);
	iScnY2 = Double2Int32(scnPt2.y);
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
				cimgX2 = cimg->img->info.dispWidth;
				cimgY2 = cimg->img->info.dispHeight;
				cimgX = 0;
				cimgY = 0;
				if (iScnX < 0)
				{
					cimgX = MulDiv32(cimgX2, -iScnX, (iScnX2 - iScnX));
					iScnX = 0;
				}
				if (iScnY < 0)
				{
					cimgY = MulDiv32(cimgY2, -iScnY, (iScnY2 - iScnY));
					iScnY = 0;
				}
				if (iScnX2 > imgW)
				{
					cimgX2 = cimgX + MulDiv32(cimgX2 - cimgX, imgW - iScnX, iScnX2 - iScnX);
					iScnX2 = imgW;
				}
				if (iScnY2 > imgH)
				{
					cimgY2 = cimgY + MulDiv32(cimgY2 - cimgY, imgH - iScnY, iScnY2 - iScnY);
					iScnY2 = imgH;
				}
				if (cimgX == cimgX2)
				{
					if (cimgX2 >= cimg->img->info.dispWidth)
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
					if (cimgY2 >= cimg->img->info.dispHeight)
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

Map::ResizableTileMapRenderer::ResizableTileMapRenderer(Media::DrawEngine *eng, Map::TileMap *map, Parser::ParserList *parsers, Media::ColorManagerSess *sess, Media::ColorProfile *outputColor) : srcColor(Media::ColorProfile::CPT_SRGB), outputColor(outputColor)
{
	this->eng = eng;
	this->map = map;
	this->parsers = parsers;
	this->lastLevel = -1;
	this->threadNext = 0;
	this->updHdlr = 0;
	this->updObj = 0;
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(3, 4, &this->srcColor, &this->outputColor, sess, Media::AT_NO_ALPHA));

	UOSInt i;
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
		NEW_CLASS(this->threads[i].evt, Sync::Event(true));
		Sync::ThreadUtil::Create(TaskThread, &this->threads[i]);
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

	i = this->lastImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->lastImgs.GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFree(cimg);
	}
	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItem(i);
		if (cimg->img)
		{
			DEL_CLASS(cimg->img);
		}
		MemFree(cimg);
	}
	DEL_CLASS(this->resizer);
}

void Map::ResizableTileMapRenderer::DrawMap(Media::DrawImage *img, Map::MapView *view)
{
	CachedImage *cimg;

	OSInt i;
	OSInt j;
	OSInt k;
	Media::ImageList *imgList;
	Data::ArrayListInt64 idList;
	UOSInt level = map->GetNearestLevel(view->GetMapScale());

	Data::ArrayListInt64 cacheIds;
	Data::ArrayList<CachedImage *> cacheImgs;
	if (this->lastLevel == level)
	{
		cacheIds.AddAll(&this->lastIds);
		cacheImgs.AddAll(&this->lastImgs);
		this->lastIds.Clear();
		this->lastImgs.Clear();
	}
	else
	{
		j = this->lastImgs.GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs.GetItem(j);
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
				this->idleImgs.Add(cimg);
			}
		}
		this->lastIds.Clear();
		this->lastImgs.Clear();
	}

	Double bounds[4];
	Math::Quadrilateral bounds4;
	bounds4 = view->GetBounds();
	map->GetImageIDs(level, Math::RectAreaDbl(bounds4.tl, bounds4.br), &idList);
	i = idList.GetCount();
	while (i-- > 0)
	{
		j = cacheIds.SortedIndexOf(idList.GetItem(i));
		if (j >= 0)
		{
			k = this->lastIds.SortedInsert(cacheIds.RemoveAt(j));
			this->lastImgs.Insert(k, cimg = cacheImgs.RemoveAt(j));

			DrawImage(view, img, cimg);
		}
		else
		{
			imgList = this->map->LoadTileImage(level, idList.GetItem(i), this->parsers, bounds, true);
			if (imgList)
			{
				cimg = MemAlloc(CachedImage, 1);
				cimg->imgId = idList.GetItem(i);
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

				k = this->lastIds.SortedInsert(idList.GetItem(i));
				this->lastImgs.Insert(k, cimg);
			}
			else
			{
				cimg = MemAlloc(CachedImage, 1);
				cimg->imgId = idList.GetItem(i);
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

				k = this->lastIds.SortedInsert(idList.GetItem(i));
				this->lastImgs.Insert(k, cimg);
			}
		}
	}

	lastLevel = level;
	i = cacheImgs.GetCount();
	while (i-- > 0)
	{
		cimg = cacheImgs.GetItem(i);
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
			this->idleImgs.Add(cimg);
		}
	}

	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItem(i);
		if (cimg->isFinish)
		{
			this->idleImgs.RemoveAt(i);
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
