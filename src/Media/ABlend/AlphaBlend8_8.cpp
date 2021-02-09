#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ABlend/AlphaBlend8_8.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

extern "C"
{
	void AlphaBlend8_8_DoBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height);
	void AlphaBlend8_8_DoBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height);
}

void Media::ABlend::AlphaBlend8_8::MTBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	if (height < (this->threadCnt << 4))
	{
		AlphaBlend8_8_DoBlend(dest, dbpl, src, sbpl, width, height);
	}
	else
	{
		OSInt lastHeight = height;
		OSInt currHeight;
		OSInt i = this->threadCnt;
		while (i-- > 0)
		{
			currHeight = MulDivOS(height, i, this->threadCnt);
			this->stats[i].dest = dest + dbpl * currHeight;
			this->stats[i].dbpl = dbpl;
			this->stats[i].src = src + sbpl * currHeight;
			this->stats[i].sbpl = sbpl;
			this->stats[i].width = width;
			this->stats[i].height = lastHeight - currHeight;
			this->stats[i].status = 4;
			this->stats[i].evt->Set();
			lastHeight = currHeight;
		}
		Bool found;
		while (true)
		{
			this->mainEvt->Wait(1000);
			found = false;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->stats[i].status == 4)
				{
					found = true;
					break;
				}
			}
			if (!found)
				break;
		}
	}
}

void Media::ABlend::AlphaBlend8_8::MTBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	if (height < (this->threadCnt << 4))
	{
		AlphaBlend8_8_DoBlendPA(dest, dbpl, src, sbpl, width, height);
	}
	else
	{
		OSInt lastHeight = height;
		OSInt currHeight;
		OSInt i = this->threadCnt;
		while (i-- > 0)
		{
			currHeight = MulDivOS(height, i, this->threadCnt);
			this->stats[i].dest = dest + dbpl * currHeight;
			this->stats[i].dbpl = dbpl;
			this->stats[i].src = src + sbpl * currHeight;
			this->stats[i].sbpl = sbpl;
			this->stats[i].width = width;
			this->stats[i].height = lastHeight - currHeight;
			this->stats[i].status = 5;
			this->stats[i].evt->Set();
			lastHeight = currHeight;
		}
		Bool found;
		while (true)
		{
			this->mainEvt->Wait(1000);
			found = false;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->stats[i].status == 5)
				{
					found = true;
					break;
				}
			}
			if (!found)
				break;
		}
	}
}

UInt32 __stdcall Media::ABlend::AlphaBlend8_8::ProcessThread(void *userObj)
{
	ThreadStat *stat = (ThreadStat *)userObj;
	stat->status = 1;
	stat->me->mainEvt->Set();
	while (true)
	{
		stat->evt->Wait(10000);
		if (stat->status == 2)
		{
			break;
		}
		else if (stat->status == 4)
		{
			AlphaBlend8_8_DoBlend(stat->dest, stat->dbpl, stat->src, stat->sbpl, stat->width, stat->height);
			stat->status = 1;
			stat->me->mainEvt->Set();
		}
		else if (stat->status == 5)
		{
			AlphaBlend8_8_DoBlendPA(stat->dest, stat->dbpl, stat->src, stat->sbpl, stat->width, stat->height);
			stat->status = 1;
			stat->me->mainEvt->Set();
		}
	}
	stat->status = 3;
	stat->me->mainEvt->Set();
	return 0;
}

Media::ABlend::AlphaBlend8_8::AlphaBlend8_8() : Media::ImageAlphaBlend()
{
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"Media.ABlend.AlphaBlend8_C8.mainEvt"));
	this->threadCnt = Sync::Thread::GetThreadCnt();
	if (this->threadCnt > 4)
	{
		this->threadCnt = 4;
	}
	this->stats = MemAlloc(ThreadStat, this->threadCnt);
	OSInt i = this->threadCnt;
	while (i-- > 0)
	{
		this->stats[i].me = this;
		NEW_CLASS(this->stats[i].evt, Sync::Event(true, (const UTF8Char*)"Media.ABlend.AlphaBlend8_8.stats.evt"));
		this->stats[i].status = 0;
		Sync::Thread::Create(ProcessThread, &this->stats[i], 65536);
	}
	Bool found;
	while (true)
	{
		this->mainEvt->Wait(1000);
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->stats[i].status != 1)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}
}

Media::ABlend::AlphaBlend8_8::~AlphaBlend8_8()
{
	OSInt i = this->threadCnt;
	Bool found;
	while (i-- > 0)
	{
		this->stats[i].status = 2;
		this->stats[i].evt->Set();
	}
	while (true)
	{
		this->mainEvt->Wait(1000);
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->stats[i].status != 3)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->stats[i].evt);
	}
	MemFree(this->stats);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->mainEvt);
}

void Media::ABlend::AlphaBlend8_8::Blend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, Media::AlphaType srcAType)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (srcAType == Media::AT_PREMUL_ALPHA)
	{
		this->MTBlendPA(dest, dbpl, src, sbpl, width, height);
//		this->DoBlendPA(dest, dbpl, src, sbpl, width, height);
	}
	else
	{
		this->MTBlend(dest, dbpl, src, sbpl, width, height);
//		this->DoBlend(dest, dbpl, src, sbpl, width, height);
	}
	mutUsage.EndUse();
}

void Media::ABlend::AlphaBlend8_8::PremulAlpha(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	OSInt i;
	sbpl -= width << 2;
	dbpl -= width << 2;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			dest[0] = src[0] * src[3] / 255;
			dest[1] = src[1] * src[3] / 255;
			dest[2] = src[2] * src[3] / 255;
			dest[3] = src[3];

			src += 4;
			dest += 4;
		}
		src += sbpl;
		dest += dbpl;
	}
}
