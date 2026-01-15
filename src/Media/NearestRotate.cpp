#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/FrameInfo.h"
#include "Media/ImageResizer.h"
#include "Media/StaticImage.h"
#include "Media/NearestRotate.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"

void Media::NearestRotate::RotateTask(NN<NROT_THREADSTAT> stat)
{
	Int32 destWidth = stat->destWidth;
	Int32 destHeight = stat->destEndY;
	Int32 i = stat->destStartY;
	Int32 j;
	Single thisX;
	Single thisY;
	Single thisXTran;
	Single thisYTran;
	Single ofstCorrX = stat->ofstCorrX;
	Single ofstCorrY = stat->ofstCorrY;
	Single centerX = stat->centerX;
	Single centerY = stat->centerY;
	Single cosAngle = stat->cosAngle;
	Single sinAngle = stat->sinAngle;
	Single srcWidth = stat->srcWidth;
	Single srcHeight = stat->srcHeight;
	Int32 srcIWidth = stat->srcIWidth;
	UnsafeArray<Int32> pBits = stat->pBits;
	UnsafeArray<Int32> sBits = stat->sBits;

	while (i < destHeight)
	{
		j = 0;
		while (j < destWidth)
		{
			thisX = j + ofstCorrX - centerX;
			thisY = i + ofstCorrY - centerY;
			thisXTran = thisX * cosAngle + thisY * sinAngle + centerX;
			thisYTran = -thisX * sinAngle + thisY * cosAngle + centerY;

			if (thisXTran >= 0 && thisXTran < srcWidth && thisYTran >= 0 && thisYTran < srcHeight)
			{
				*pBits++ = sBits[srcIWidth * ((Int32)thisYTran) + (Int32)thisXTran];
			}
			else
			{
				*pBits++ = 0;
			}
			j++;
		}
		i++;
	}
}

UInt32 Media::NearestRotate::WorkerThread(AnyType obj)
{
	NN<Media::NearestRotate> rot = obj.GetNN<Media::NearestRotate>();
	Int32 threadId = rot->currId;
	NN<NROT_THREADSTAT> stat = rot->stats[threadId];

	stat->status = 1;
	rot->evtMain.Set();
	while (true)
	{
		stat->evt->Wait();
		if (stat->status == 2)
		{
			break;
		}
		else if (stat->status == 3)
		{
			Media::NearestRotate::RotateTask(stat);
			stat->status = 4;
			rot->evtMain.Set();
		}
	}
	stat->status = 0;
	rot->evtMain.Set();
	return 0;
}


Media::NearestRotate::NearestRotate()
{
	Int32 i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();

	stats = MemAllocArr(Media::NROT_THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event(L"Media.NearestRotate.stats.evt"));
		stats[i].status = 0;
		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			evtMain.Wait();
		}
	}

}

Media::NearestRotate::~NearestRotate()
{
	Int32 i = nThread;
	Bool exited;
	while (i-- > 0)
	{
		if (stats[i].status != 0)
		{
			stats[i].status = 2;
			stats[i].evt->Set();
		}
	}
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				exited = false;
				break;
			}
		}
		if (exited)
			break;

		this->evtMain.Wait();
	}
	i = nThread;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	MemFreeArr(stats);
}

Optional<Media::RasterImage> Media::NearestRotate::Rotate(NN<Media::RasterImage> srcImg, Single centerX, Single centerY, Single angleRad, Bool keepCoord, Bool keepSize)
{
	NN<Media::StaticImage> simg = srcImg->CreateStaticImage();
	if (!simg->ToB8G8R8A8())
	{
		simg.Delete();
		return 0;
	}

	Media::StaticImage *destImg;
	Single srcWidth = (Single)simg->info.dispSize.x;
	Single srcHeight = (Single)simg->info.dispSize.y;
	Int32 destWidth;
	Int32 destHeight;

	Single cosAngle = cos(angleRad);
	Single sinAngle = sin(angleRad);

	Single ofstCorrX;
	Single ofstCorrY;
	Single ofstXMax;
	Single ofstXMin;
	Single ofstYMax;
	Single ofstYMin;

	Single thisX = (0 - centerX);
	Single thisY = (0 - centerY);
	Single thisXTran = thisX * cosAngle - thisY * sinAngle + centerX;
	Single thisYTran = thisX * sinAngle + thisY * cosAngle + centerY;

	ofstXMax = thisXTran;
	ofstXMin = thisXTran;
	ofstYMax = thisYTran;
	ofstYMin = thisYTran;

	thisX = (0 - centerX);
	thisY = (srcHeight - centerY);
	thisXTran = thisX * cosAngle - thisY * sinAngle + centerX;
	thisYTran = thisX * sinAngle + thisY * cosAngle + centerY;

	if (thisXTran > ofstXMax)
		ofstXMax = thisXTran;
	if (thisXTran < ofstXMin)
		ofstXMin = thisXTran;
	if (thisYTran > ofstYMax)
		ofstYMax = thisYTran;
	if (thisYTran < ofstYMin)
		ofstYMin = thisYTran;

	thisX = (srcWidth - centerX);
	thisY = (0 - centerY);
	thisXTran = thisX * cosAngle - thisY * sinAngle + centerX;
	thisYTran = thisX * sinAngle + thisY * cosAngle + centerY;

	if (thisXTran > ofstXMax)
		ofstXMax = thisXTran;
	if (thisXTran < ofstXMin)
		ofstXMin = thisXTran;
	if (thisYTran > ofstYMax)
		ofstYMax = thisYTran;
	if (thisYTran < ofstYMin)
		ofstYMin = thisYTran;

	thisX = (srcWidth - centerX);
	thisY = (srcHeight - centerY);
	thisXTran = thisX * cosAngle - thisY * sinAngle + centerX;
	thisYTran = thisX * sinAngle + thisY * cosAngle + centerY;

	if (thisXTran > ofstXMax)
		ofstXMax = thisXTran;
	if (thisXTran < ofstXMin)
		ofstXMin = thisXTran;
	if (thisYTran > ofstYMax)
		ofstYMax = thisYTran;
	if (thisYTran < ofstYMin)
		ofstYMin = thisYTran;

	if (keepCoord)
	{
		destWidth = (Int32)ofstXMax + 1;
		destHeight = (Int32)ofstYMax + 1;
		ofstCorrX = 0;
		ofstCorrY = 0;
	}
	else
	{
		ofstCorrX = ofstXMin;
		ofstCorrY = ofstYMin;
		destWidth = (Int32)(ofstXMax - ofstXMin) + 1;
		destHeight = (Int32)(ofstYMax - ofstYMin) + 1;
	}
	if (keepSize)
	{
		destWidth = simg->info.dispSize.x;
		destHeight = simg->info.dispSize.y;
	}

	NEW_CLASS(destImg, Media::StaticImage(Math::Size2D<UIntOS>(destWidth, destHeight), 0, 32, simg->info.pf, destWidth * destHeight * 4, simg->info.color, simg->info.yuvType, simg->info.atype, simg->info.ycOfst));
	UnsafeArray<Int32> pBits = UnsafeArray<Int32>::ConvertFrom(destImg->data);
	UnsafeArray<Int32> sBits = UnsafeArray<Int32>::ConvertFrom(simg->data);
	Int32 lastHeight = destHeight;
	Int32 thisHeight;
	UIntOS i;
	i = this->nThread;
	while (i-- > 0)
	{
		thisHeight = MulDivUOS(destHeight, i, this->nThread);
		stats[i].srcWidth = srcWidth;
		stats[i].srcHeight = srcHeight;
		stats[i].srcIWidth = simg->info.dispSize.x;
		stats[i].destWidth = destWidth;
		stats[i].destStartY = thisHeight;
		stats[i].destEndY = lastHeight;
		stats[i].ofstCorrX = ofstCorrX;
		stats[i].ofstCorrY = ofstCorrY;
		stats[i].centerX = centerX;
		stats[i].centerY = centerY;
		stats[i].sinAngle = sinAngle;
		stats[i].cosAngle = cosAngle;
		stats[i].sBits = sBits;
		stats[i].pBits = pBits + (destWidth * thisHeight);
		stats[i].status = 3;
		stats[i].evt->Set();
		lastHeight = thisHeight;
	}
	while (true)
	{
		Bool fin = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == 3)
			{
				fin = false;
				break;
			}
		}
		if (fin)
			break;
		this->evtMain.Wait();
	}

/*	Int32 i;
	Int32 j;
	i = 0;
	while (i < destHeight)
	{
		j = 0;
		while (j < destWidth)
		{
			thisX = j + ofstCorrX - centerX;
			thisY = i + ofstCorrY - centerY;
			thisXTran = thisX * cosAngle + thisY * sinAngle + centerX;
			thisYTran = -thisX * sinAngle + thisY * cosAngle + centerY;

			if (thisXTran >= 0 && thisXTran < srcWidth && thisYTran >= 0 && thisYTran < srcHeight)
			{
				*pBits++ = sBits[srcImg->info->width * ((Int32)thisYTran) + (Int32)thisXTran];
			}
			else
			{
				*pBits++ = 0;
			}
			j++;
		}
		i++;
	}*/

	return destImg;
}
