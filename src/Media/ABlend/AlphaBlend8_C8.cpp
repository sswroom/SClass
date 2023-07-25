#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/RGBLUTGen.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Media/CS/TransferFunc.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void AlphaBlend8_C8_DoBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height, UInt8 *rgbTable);
	void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height, UInt8 *rgbTable);
}

void Media::ABlend::AlphaBlend8_C8::MTBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height)
{
	if (width < 64 || height < this->threadCnt)
	{
		AlphaBlend8_C8_DoBlend(dest, dbpl, src, sbpl, width, height, this->rgbTable);
	}
	else
	{
		UOSInt lastHeight = height;
		UOSInt currHeight;
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(height, i, this->threadCnt);
			this->stats[i].dest = dest + dbpl * (OSInt)currHeight;
			this->stats[i].dbpl = dbpl;
			this->stats[i].src = src + sbpl * (OSInt)currHeight;
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
			this->mainEvt.Wait(1000);
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

void Media::ABlend::AlphaBlend8_C8::MTBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height)
{
	if (width < 64 || height < this->threadCnt)
	{
		AlphaBlend8_C8_DoBlendPA(dest, dbpl, src, sbpl, width, height, this->rgbTable);
	}
	else
	{
		UOSInt lastHeight = height;
		UOSInt currHeight;
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(height, i, this->threadCnt);
			this->stats[i].dest = dest + dbpl * (OSInt)currHeight;
			this->stats[i].dbpl = dbpl;
			this->stats[i].src = src + sbpl * (OSInt)currHeight;
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
			this->mainEvt.Wait(1000);
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
		UOSInt i;
		i = this->lutList->GetCount();
		while (i-- > 0)
		{
			lut = this->lutList->GetItem(i);
			if (lut->sProfile.Equals(&this->sProfile) && lut->dProfile.Equals(&this->dProfile) && lut->oProfile.Equals(&this->oProfile))
			{
				this->rgbTable = lut->rgbTable;
				return;
			}
		}
		NEW_CLASS(lut, LUTInfo());
		lut->sProfile.Set(&this->sProfile);
		lut->dProfile.Set(&this->dProfile);
		lut->oProfile.Set(&this->oProfile);
		lut->rgbTable = MemAlloc(UInt8, 262144 + 8192 + 8192);
		this->lutList->Add(lut);
		this->rgbTable = lut->rgbTable;
	}

	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_BGRA8(this->rgbTable, &this->oProfile, 14, Media::CS::TransferFunc::GetRefLuminance(&this->sProfile.rtransfer));
	lutGen.GenRGBA8_LRGBC((Int64*)&this->rgbTable[262144], &this->sProfile, &this->oProfile.primaries, 14);
	lutGen.GenRGBA8_LRGBC((Int64*)&this->rgbTable[262144 + 8192], &this->dProfile, &this->oProfile.primaries, 14);
}

UInt32 __stdcall Media::ABlend::AlphaBlend8_C8::ProcessThread(void *userObj)
{
	ThreadStat *stat = (ThreadStat *)userObj;
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("ABlend8_C8_")), stat->index);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
	{
		Sync::Event evt;
		stat->status = 1;
		stat->evt = &evt;
		stat->me->mainEvt.Set();
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
				stat->me->mainEvt.Set();
			}
			else if (stat->status == 5)
			{
				AlphaBlend8_C8_DoBlendPA(stat->dest, stat->dbpl, stat->src, stat->sbpl, stat->width, stat->height, stat->me->rgbTable);
				stat->status = 1;
				stat->me->mainEvt.Set();
			}
		}
	}
	stat->status = 3;
	stat->me->mainEvt.Set();
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
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	if (this->threadCnt > 4)
	{
		this->threadCnt = 4;
	}

	this->stats = MemAlloc(ThreadStat, this->threadCnt);
	UOSInt i = this->threadCnt;
	while (i-- > 0)
	{
		this->stats[i].me = this;
		this->stats[i].index = i;
		this->stats[i].status = 0;
		Sync::ThreadUtil::Create(ProcessThread, &this->stats[i], 65536);
	}
	Bool found;
	while (true)
	{
		this->mainEvt.Wait(1000);
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
	UOSInt i = this->threadCnt;
	Bool found;
	while (i-- > 0)
	{
		this->stats[i].status = 2;
		this->stats[i].evt->Set();
	}
	while (true)
	{
		this->mainEvt.Wait(1000);
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
	MemFree(this->stats);
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
			MemFree(lut->rgbTable);
			DEL_CLASS(lut);
		}
		DEL_CLASS(this->lutList);
	}
	else
	{
		MemFree(this->rgbTable);
	}
}

void Media::ABlend::AlphaBlend8_C8::Blend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height, Media::AlphaType srcAType)
{
	Sync::MutexUsage mutUsage(&this->mut);
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
}

void Media::ABlend::AlphaBlend8_C8::PremulAlpha(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, UOSInt width, UOSInt height)
{
	UOSInt i;
	UInt32 bVal;
	UInt32 gVal;
	UInt32 rVal;
	UInt32 aVal;

	sbpl = sbpl - (OSInt)(width << 2);
	dbpl = dbpl - (OSInt)(width << 2);
	Sync::MutexUsage mutUsage(&this->mut);
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
			aVal = (UInt32)((src[3] << 8) | src[3]); //xmm0
			bVal = (UInt32)(*(Int16*)&rgbTable[262144 + src[2] * 8 + 0] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 0] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 0] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 0]) * aVal; //xmm1
			gVal = (UInt32)(*(Int16*)&rgbTable[262144 + src[2] * 8 + 2] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 2] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 2] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 2]) * aVal;
			rVal = (UInt32)(*(Int16*)&rgbTable[262144 + src[2] * 8 + 4] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 4] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 4] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 4]) * aVal;
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
	mutUsage.EndUse();
}

void Media::ABlend::AlphaBlend8_C8::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void Media::ABlend::AlphaBlend8_C8::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	if (this->lutList)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		LUTInfo *lut;
		UOSInt i;
		i = this->lutList->GetCount();
		while (i-- > 0)
		{
			lut = this->lutList->GetItem(i);
			MemFree(lut->rgbTable);
			DEL_CLASS(lut);
		}
		this->lutList->Clear();
		this->changed = true;
	}
	else
	{
		this->changed = true;
	}
}
