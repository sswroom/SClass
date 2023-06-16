#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageCopy.h"
#include "Media/ImageCopyC.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

#if defined(CPU_X86_64)
extern "C" Int32 UseAVX;
extern "C" Int32 CPUBrand;
#endif

void Media::ImageCopy::MT_Copy(UInt8 *inPt, UInt8 *outPt, UOSInt copySize, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	Bool fin;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->stats[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->stats[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->stats[i].copySize = copySize;
		this->stats[i].height = lastHeight - currHeight;
		this->stats[i].sstep = sstep;
		this->stats[i].dstep = dstep;

		this->stats[i].status = 3;
		this->stats[i].evt->Set();
		lastHeight = currHeight;
	}
	while (true)
	{
		fin = true;
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
}

UInt32 Media::ImageCopy::WorkerThread(void *obj)
{
	IMGCOPYSTAT *stat = (IMGCOPYSTAT*)obj;
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->status = 1;
		stat->evtMain->Set();
		while (true)
		{
			stat->evt->Wait();
			if (stat->status == 2)
			{
				break;
			}
			else if (stat->status == 3)
			{
				ImageCopy_ImgCopy(stat->inPt, stat->outPt, stat->copySize, stat->height, stat->sstep, stat->dstep);
				stat->status = 1;
				stat->evtMain->Set();
			}
			else if (stat->status == 4)
			{
				Sync::ThreadUtil::SetPriority((Sync::ThreadUtil::ThreadPriority)stat->copySize);
				stat->status = 1;
				stat->evtMain->Set();
			}
		}
	}
	stat->status = 0;
	return 0;
}

Media::ImageCopy::ImageCopy()
{
	UOSInt i;
	Bool found;
#if defined(CPU_X86_64)
	if (CPUBrand == 2)
	{
		nThread = Sync::ThreadUtil::GetThreadCnt();
		if (nThread > 2)
		{
			nThread = 2;
		}
	}
	else
	{
		nThread = 1;
	}
#else
	nThread = 1;
#endif
	stats = MemAlloc(IMGCOPYSTAT, nThread);
	i = nThread;
	while (i-- > 0)
	{
		stats[i].status = 0;
		stats[i].evtMain = &this->evtMain;
		Sync::ThreadUtil::Create(WorkerThread, &stats[i]);
	}
	while (true)
	{
		this->evtMain.Wait();
		i = nThread;
		found = false;
		while (i-- > 0)
		{
			if (stats[i].status != 1)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			break;
		}
	}

}

Media::ImageCopy::~ImageCopy()
{
	UOSInt i = nThread;
	Bool found;
	while (i-- > 0)
	{
		stats[i].status = 2;
		stats[i].evt->Set();;
	}

	while (true)
	{
		i = nThread;
		found = false;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				stats[i].evt->Set();
				found = true;
				break;
			}
		}
		if (!found)
		{
			MemFree(stats);
			break;
		}
	}
}

void Media::ImageCopy::Copy32(UInt8 *src, OSInt sbpl, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	if (dheight < 16 || this->nThread == 1)
	{
		ImageCopy_ImgCopy(src, dest, dwidth << 2, dheight, sbpl, dbpl);
	}
	else
	{
		MT_Copy(src, dest, dwidth << 2, dheight, sbpl, dbpl);
	}
}

void Media::ImageCopy::Copy16(UInt8 *src, OSInt sbpl, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	if (dheight < 16 || this->nThread == 1)
	{
		ImageCopy_ImgCopy(src, dest, dwidth << 1, dheight, sbpl, dbpl);
	}
	else
	{
		MT_Copy(src, dest, dwidth << 1, dheight, sbpl, dbpl);
	}
}

void Media::ImageCopy::SetThreadPriority(Sync::ThreadUtil::ThreadPriority tp)
{
	Bool fin;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		this->stats[i].copySize = (UOSInt)tp;
		this->stats[i].status = 4;
		this->stats[i].evt->Set();
	}
	while (true)
	{
		fin = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == 4)
			{
				fin = false;
				break;
			}
		}
		if (fin)
			break;
		this->evtMain.Wait();
	}
}

