#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/RGBLUTGen.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Media/CS/TransferFunc.h"
#include "Sync/Thread.h"

extern "C"
{
	void AlphaBlend8_C8_DoBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);
	void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);
}

void Media::ABlend::AlphaBlend8_C8::MTBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	if (width < 64 || height < this->threadCnt)
	{
		AlphaBlend8_C8_DoBlend(dest, dbpl, src, sbpl, width, height, this->rgbTable);
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

void Media::ABlend::AlphaBlend8_C8::MTBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	if (width < 64 || height < this->threadCnt)
	{
		AlphaBlend8_C8_DoBlendPA(dest, dbpl, src, sbpl, width, height, this->rgbTable);
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

void Media::ABlend::AlphaBlend8_C8::UpdateLUT()
{
	if (this->lutList)
	{
		LUTInfo *lut;
		OSInt i;
		i = this->lutList->GetCount();
		while (i-- > 0)
		{
			lut = this->lutList->GetItem(i);
			if (lut->sProfile->Equals(this->sProfile) && lut->dProfile->Equals(this->dProfile) && lut->oProfile->Equals(this->oProfile))
			{
				this->rgbTable = lut->rgbTable;
				return;
			}
		}
		lut = MemAlloc(LUTInfo, 1);
		NEW_CLASS(lut->sProfile, Media::ColorProfile(this->sProfile));
		NEW_CLASS(lut->dProfile, Media::ColorProfile(this->dProfile));
		NEW_CLASS(lut->oProfile, Media::ColorProfile(this->oProfile));
		lut->rgbTable = MemAlloc(UInt8, 262144 + 8192 + 8192);
		this->lutList->Add(lut);
		this->rgbTable = lut->rgbTable;
	}

	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_BGRA8(this->rgbTable, this->oProfile, 14, Media::CS::TransferFunc::GetRefLuminance(this->sProfile->rtransfer));
	lutGen.GenRGBA8_LRGBC((Int64*)&this->rgbTable[262144], this->sProfile, &this->oProfile->primaries, 14);
	lutGen.GenRGBA8_LRGBC((Int64*)&this->rgbTable[262144 + 8192], this->dProfile, &this->oProfile->primaries, 14);
}

UInt32 __stdcall Media::ABlend::AlphaBlend8_C8::ProcessThread(void *userObj)
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
			AlphaBlend8_C8_DoBlend(stat->dest, stat->dbpl, stat->src, stat->sbpl, stat->width, stat->height, stat->me->rgbTable);
			stat->status = 1;
			stat->me->mainEvt->Set();
		}
		else if (stat->status == 5)
		{
			AlphaBlend8_C8_DoBlendPA(stat->dest, stat->dbpl, stat->src, stat->sbpl, stat->width, stat->height, stat->me->rgbTable);
			stat->status = 1;
			stat->me->mainEvt->Set();
		}
	}
	stat->status = 3;
	stat->me->mainEvt->Set();
	return 0;
}

Media::ABlend::AlphaBlend8_C8::AlphaBlend8_C8(Media::ColorSess *colorSess, Bool multiProfile) : Media::ImageAlphaBlend()
{
	this->colorSess = colorSess;
	if (this->colorSess)
	{
		this->colorSess->AddHandler(this);
	}
	if (multiProfile)
	{
		NEW_CLASS(this->lutList, Data::ArrayList<LUTInfo*>());
		this->rgbTable = 0;
	}
	else
	{
		this->lutList = 0;
		this->rgbTable = MemAlloc(UInt8, 262144 + 8192 + 8192);
	}
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
		NEW_CLASS(this->stats[i].evt, Sync::Event(true, (const UTF8Char*)"Media.ABlend.AlphaBlend8_C8.stats.evt"));
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

Media::ABlend::AlphaBlend8_C8::~AlphaBlend8_C8()
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
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}

	if (this->lutList)
	{
		LUTInfo *lut;
		i = this->lutList->GetCount();
		while (i-- > 0)
		{
			lut = this->lutList->GetItem(i);
			DEL_CLASS(lut->sProfile);
			DEL_CLASS(lut->dProfile);
			DEL_CLASS(lut->oProfile);
			MemFree(lut->rgbTable);
			MemFree(lut);
		}
		DEL_CLASS(this->lutList);
	}
	else
	{
		MemFree(this->rgbTable);
	}
}

void Media::ABlend::AlphaBlend8_C8::Blend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, Media::AlphaType srcAType)
{
	this->mut->Lock();
	if (this->changed)
	{
		this->changed = false;
		this->UpdateLUT();
	}
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
	this->mut->Unlock();
}

void Media::ABlend::AlphaBlend8_C8::PremulAlpha(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	OSInt i;
	UInt32 bVal;
	UInt32 gVal;
	UInt32 rVal;
	UInt32 aVal;

	sbpl -= width << 2;
	dbpl -= width << 2;
	this->mut->Lock();
	if (this->changed)
	{
		this->changed = false;
		this->UpdateLUT();
	}
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			aVal = (src[3] << 8) | src[3]; //xmm0
			bVal = (*(Int16*)&rgbTable[262144 + src[2] * 8 + 0] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 0] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 0] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 0]) * aVal; //xmm1
			gVal = (*(Int16*)&rgbTable[262144 + src[2] * 8 + 2] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 2] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 2] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 2]) * aVal;
			rVal = (*(Int16*)&rgbTable[262144 + src[2] * 8 + 4] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 4] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 4] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 4]) * aVal;
			dest[0] = rgbTable[(bVal >> 16)];
			dest[1] = rgbTable[(gVal >> 16) + 65536];
			dest[2] = rgbTable[(rVal >> 16) + 131072];
			dest[3] = src[3];

			src += 4;
			dest += 4;
		}
		src += sbpl;
		dest += dbpl;
	}
	this->mut->Unlock();
}

void Media::ABlend::AlphaBlend8_C8::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void Media::ABlend::AlphaBlend8_C8::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	if (this->lutList)
	{
		this->mut->Lock();
		LUTInfo *lut;
		OSInt i;
		i = this->lutList->GetCount();
		while (i-- > 0)
		{
			lut = this->lutList->GetItem(i);
			DEL_CLASS(lut->sProfile);
			DEL_CLASS(lut->dProfile);
			DEL_CLASS(lut->oProfile);
			MemFree(lut->rgbTable);
			MemFree(lut);
		}
		this->lutList->Clear();
		this->changed = true;
		this->mut->Unlock();
	}
	else
	{
		this->changed = true;
	}
}
