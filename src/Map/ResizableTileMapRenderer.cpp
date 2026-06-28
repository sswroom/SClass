#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "Map/ResizableTileMapRenderer.h"
#include "Math/Math_C.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Map::ResizableTileMapRenderer::TaskThread(AnyType userObj)
{
	Math::RectAreaDbl bounds;
	NN<Media::ImageList> imgList;
	Optional<CachedImage> optcimg;
	NN<CachedImage> cimg;
	NN<ThreadStat> stat = userObj.GetNN<ThreadStat>();
	stat->running = true;
	while (!stat->toStop)
	{
		while (!stat->toStop)
		{
			{
				Sync::MutexUsage mutUsage(stat->me->taskMut);
				optcimg = stat->me->taskQueued.Get().GetOpt<CachedImage>();
			}
			if (!optcimg.SetTo(cimg))
				break;

			if (cimg->isCancel)
			{
				cimg->isFinish = true;
			}
			else
			{
				if (stat->me->map->LoadTileImage(cimg->level, cimg->imgId, stat->me->parsers, bounds, false).SetTo(imgList))
				{
					NN<Media::Image> img;
					if (imgList->GetImage2(0, nullptr).SetTo(img))
					{
						cimg->img = img->CreateStaticImage();
					}
					cimg->isFinish = true;
					imgList.Delete();
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
		Sync::MutexUsage mutUsage(this->taskMut);
		this->taskQueued.Put(cimg);
	}
	this->threads[this->threadNext].evt->Set();
	this->threadNext = (this->threadNext + 1) % this->threadCnt;
}

void Map::ResizableTileMapRenderer::DrawImage(NN<Map::MapView> view, NN<Media::DrawImage> img, NN<CachedImage> cimg)
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
	NN<Media::StaticImage> nncimg;

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
		if (cimg->img.SetTo(nncimg))
		{
			if (imgW > (iScnX2 -iScnX) || imgH > (iScnY2 - iScnY))
			{
				this->resizer->SetTargetSize(Math::Size2D<UIntOS>(iScnX2 - iScnX, iScnY2 - iScnY));
				this->resizer->SetResizeAspectRatio(Media::ImageResizer::RAR_IGNOREAR);
				NN<Media::StaticImage> newImg;
				if (this->resizer->ProcessToNew(nncimg).SetTo(newImg))
				{
					NN<Media::DrawImage> dimg;
					if (this->eng->ConvImage(newImg, nullptr).SetTo(dimg))
					{
						img->DrawImagePt(dimg, Math::Coord2DDbl(iScnX, iScnY));
						this->eng->DeleteImage(dimg);
					}
					newImg.Delete();
				}
			}
			else
			{
				cimgX2 = nncimg->info.dispSize.x;
				cimgY2 = nncimg->info.dispSize.y;
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
					if (cimgX2 >= nncimg->info.dispSize.x)
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
					if (cimgY2 >= nncimg->info.dispSize.y)
					{
						cimgY = cimgY2 - 1;
					}
					else
					{
						cimgY2++;
					}
				}

				this->resizer->SetTargetSize(Math::Size2D<UIntOS>(iScnX2 - iScnX, iScnY2 - iScnY));
				this->resizer->SetResizeAspectRatio(Media::ImageResizer::RAR_IGNOREAR);
				NN<Media::StaticImage> newImg;
				if (this->resizer->ProcessToNewPartial(nncimg, Math::Coord2DDbl(cimgX, cimgY), Math::Coord2DDbl(cimgX2, cimgY2)).SetTo(newImg))
				{
					NN<Media::DrawImage> dimg;
					if (this->eng->ConvImage(newImg, nullptr).SetTo(dimg))
					{
						img->DrawImagePt(dimg, Math::Coord2DDbl(iScnX, iScnY));
						this->eng->DeleteImage(dimg);
					}
					newImg.Delete();
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
			
			NN<Media::DrawBrush> emptyBrush = img->NewBrushARGB(0xffffffff);
			img->DrawRect(Math::Coord2DDbl(iScnX, iScnY), Math::Size2DDbl(iScnX2 - iScnX, iScnY2 - iScnY), nullptr, emptyBrush);
			img->DelBrush(emptyBrush);
		}
	}
}

Map::ResizableTileMapRenderer::ResizableTileMapRenderer(NN<Media::DrawEngine> eng, NN<Map::TileMap> map, NN<Parser::ParserList> parsers, Optional<Media::ColorManagerSess> sess, NN<Media::ColorProfile> outputColor) : srcColor(Media::ColorProfile::CPT_SRGB), outputColor(outputColor)
{
	this->eng = eng;
	this->map = map;
	this->parsers = parsers;
	this->lastLevel = -1;
	this->threadNext = 0;
	this->updHdlr = 0;
	this->updObj = 0;
	NEW_CLASSNN(this->resizer, Media::Resizer::LanczosResizerRGB_C8(3, 4, this->srcColor, this->outputColor, sess, Media::AT_IGNORE_ALPHA));

	UIntOS i;
	this->threadCnt = this->map->GetConcurrentCount();
	if (this->threadCnt <= 0)
		this->threadCnt = 1;
	this->threads = MemAllocArr(ThreadStat, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].running = false;
		this->threads[i].toStop = false;
		this->threads[i].me = this;
		NEW_CLASSNN(this->threads[i].evt, Sync::Event(true));
		Sync::ThreadUtil::Create(TaskThread, &this->threads[i]);
	}
}

Map::ResizableTileMapRenderer::~ResizableTileMapRenderer()
{
	NN<CachedImage> cimg;
	IntOS i;
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
		this->threads[i].evt.Delete();
	}
	MemFreeArr(this->threads);

	i = this->lastImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->lastImgs.GetItemNoCheck(i);
		cimg->img.Delete();
		MemFreeNN(cimg);
	}
	i = this->idleImgs.GetCount();
	while (i-- > 0)
	{
		cimg = this->idleImgs.GetItemNoCheck(i);
		cimg->img.Delete();
		MemFreeNN(cimg);
	}
	this->resizer.Delete();
}

void Map::ResizableTileMapRenderer::DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view)
{
	NN<CachedImage> cimg;

	UIntOS i;
	UIntOS j;
	UIntOS k;
	NN<Media::ImageList> imgList;
	Data::ArrayListT<Math::Coord2D<Int32>> idList;
	UIntOS level = map->GetNearestLevel(view->GetMapScale());

	Data::ArrayListInt64 cacheIds;
	Data::ArrayListNN<CachedImage> cacheImgs;
	if (this->lastLevel == level)
	{
		cacheIds.AddAll(this->lastIds);
		cacheImgs.AddAll(this->lastImgs);
		this->lastIds.Clear();
		this->lastImgs.Clear();
	}
	else
	{
		j = this->lastImgs.GetCount();
		while (j-- > 0)
		{
			cimg = this->lastImgs.GetItemNoCheck(j);
			if (cimg->isFinish)
			{
				cimg->img.Delete();
				MemFreeNN(cimg);
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

	Math::RectAreaDbl bounds;
	Math::Quadrilateral bounds4;
	bounds4 = view->GetBounds();
	map->GetTileImageIDs(level, Math::RectAreaDbl(bounds4.tl, bounds4.br), idList);
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
			if (this->map->LoadTileImage(level, idList.GetItem(i), this->parsers, bounds, true).SetTo(imgList))
			{
				cimg = MemAllocNN(CachedImage);
				cimg->imgId = idList.GetItem(i);
				cimg->tlx = bounds[0];
				cimg->tly = bounds[1];
				cimg->brx = bounds[2];
				cimg->bry = bounds[3];
				cimg->level = level;
				cimg->isFinish = true;
				cimg->isCancel = false;
				cimg->img = imgList->GetImage2(0, nullptr)->CreateStaticImage();
				DrawImage(view, img, cimg);
				imgList.Delete();

				k = this->lastIds.SortedInsert(idList.GetItem(i));
				this->lastImgs.Insert(k, cimg);
			}
			else
			{
				cimg = MemAllocNN(CachedImage);
				cimg->imgId = idList.GetItem(i);
				cimg->tlx = bounds[0];
				cimg->tly = bounds[1];
				cimg->brx = bounds[2];
				cimg->bry = bounds[3];
				cimg->level = level;
				cimg->isFinish = false;
				cimg->isCancel = false;
				cimg->img = nullptr;
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
		cimg = cacheImgs.GetItemNoCheck(i);
		if (cimg->isFinish)
		{
			cimg->img.Delete();
			MemFreeNN(cimg);
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
		cimg = this->idleImgs.GetItemNoCheck(i);
		if (cimg->isFinish)
		{
			this->idleImgs.RemoveAt(i);
			cimg->img.Delete();
			MemFreeNN(cimg);
		}
	}
	
}

void Map::ResizableTileMapRenderer::SetUpdatedHandler(UpdatedHandler hdlr, AnyType userObj)
{
	this->updObj = userObj;
	this->updHdlr = hdlr;
}
