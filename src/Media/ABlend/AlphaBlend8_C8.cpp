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

void Media::ABlend::AlphaBlend8_C8::MTBlend(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height)
{
	if (width < 64 || height < this->threadCnt)
	{
		AlphaBlend8_C8_DoBlend(dest.Ptr(), dbpl, src.Ptr(), sbpl, width, height, this->rgbTable.Ptr());
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

void Media::ABlend::AlphaBlend8_C8::MTBlendPA(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height)
{
	if (width < 64 || height < this->threadCnt)
	{
		AlphaBlend8_C8_DoBlendPA(dest.Ptr(), dbpl, src.Ptr(), sbpl, width, height, this->rgbTable.Ptr());
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
	NN<Data::ArrayListNN<LUTInfo>> lutList;
	if (this->lutList.SetTo(lutList))
	{
		NN<LUTInfo> lut;
		UOSInt i;
		i = lutList->GetCount();
		while (i-- > 0)
		{
			lut = lutList->GetItemNoCheck(i);
			if (lut->sProfile.Equals(this->sProfile) && lut->dProfile.Equals(this->dProfile) && lut->oProfile.Equals(this->oProfile))
			{
				this->rgbTable = lut->rgbTable;
				return;
			}
		}
		NEW_CLASSNN(lut, LUTInfo());
		lut->sProfile.Set(this->sProfile);
		lut->dProfile.Set(this->dProfile);
		lut->oProfile.Set(this->oProfile);
		lut->rgbTable = MemAllocArr(UInt8, 262144 + 8192 + 8192);
		lutList->Add(lut);
		this->rgbTable = lut->rgbTable;
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_BGRA8(this->rgbTable, this->oProfile, 14, Media::CS::TransferFunc::GetRefLuminance(this->sProfile.rtransfer));
	lutGen.GenRGBA8_LRGBC((Int64*)&this->rgbTable[262144], this->sProfile, this->oProfile.primaries, 14);
	lutGen.GenRGBA8_LRGBC((Int64*)&this->rgbTable[262144 + 8192], this->dProfile, this->oProfile.primaries, 14);
}

UInt32 __stdcall Media::ABlend::AlphaBlend8_C8::ProcessThread(AnyType userObj)
{
	NN<ThreadStat> stat = userObj.GetNN<ThreadStat>();
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("ABlend8_C8_")), stat->index);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
	{
		Sync::Event evt;
		stat->status = 1;
		stat->evt = evt;
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
				AlphaBlend8_C8_DoBlend(stat->dest.Ptr(), stat->dbpl, stat->src.Ptr(), stat->sbpl, stat->width, stat->height, stat->me->rgbTable.Ptr());
				stat->status = 1;
				stat->me->mainEvt.Set();
			}
			else if (stat->status == 5)
			{
				AlphaBlend8_C8_DoBlendPA(stat->dest.Ptr(), stat->dbpl, stat->src.Ptr(), stat->sbpl, stat->width, stat->height, stat->me->rgbTable.Ptr());
				stat->status = 1;
				stat->me->mainEvt.Set();
			}
		}
	}
	stat->status = 3;
	stat->me->mainEvt.Set();
	return 0;
}

Media::ABlend::AlphaBlend8_C8::AlphaBlend8_C8(Optional<Media::ColorSess> colorSess, Bool multiProfile) : Media::ImageAlphaBlend()
{
	this->colorSess = colorSess;
	NN<Media::ColorSess> nncolorSess;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->AddHandler(*this);
	}
	if (multiProfile)
	{
		NN<Data::ArrayListNN<LUTInfo>> lutList;
		NEW_CLASSNN(lutList, Data::ArrayListNN<LUTInfo>());
		this->lutList = lutList;
	}
	else
	{
		this->lutList = 0;
		this->rgbTable = MemAllocArr(UInt8, 262144 + 8192 + 8192);
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
		this->stats[i].me = *this;
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
	MemFreeArr(this->stats);
	NN<Media::ColorSess> nncolorSess;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->RemoveHandler(*this);
	}

	NN<Data::ArrayListNN<LUTInfo>> lutList;
	if (this->lutList.SetTo(lutList))
	{
		NN<LUTInfo> lut;
		i = lutList->GetCount();
		while (i-- > 0)
		{
			lut = lutList->GetItemNoCheck(i);
			MemFreeArr(lut->rgbTable);
			lut.Delete();
		}
		this->lutList.Delete();
	}
	else
	{
		MemFreeArr(this->rgbTable);
	}
}

void Media::ABlend::AlphaBlend8_C8::Blend(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height, Media::AlphaType srcAType)
{
	Sync::MutexUsage mutUsage(this->mut);
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

void Media::ABlend::AlphaBlend8_C8::PremulAlpha(UnsafeArray<UInt8> dest, OSInt dbpl, UnsafeArray<const UInt8> src, OSInt sbpl, UOSInt width, UOSInt height)
{
	UOSInt i;
	UInt32 bVal;
	UInt32 gVal;
	UInt32 rVal;
	UInt32 aVal;

	sbpl = sbpl - (OSInt)(width << 2);
	dbpl = dbpl - (OSInt)(width << 2);
	Sync::MutexUsage mutUsage(this->mut);
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

void Media::ABlend::AlphaBlend8_C8::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void Media::ABlend::AlphaBlend8_C8::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	NN<Data::ArrayListNN<LUTInfo>> lutList;
	if (this->lutList.SetTo(lutList))
	{
		Sync::MutexUsage mutUsage(this->mut);
		NN<LUTInfo> lut;
		UOSInt i;
		i = lutList->GetCount();
		while (i-- > 0)
		{
			lut = lutList->GetItemNoCheck(i);
			MemFreeArr(lut->rgbTable);
			lut.Delete();
		}
		lutList->Clear();
		this->changed = true;
	}
	else
	{
		this->changed = true;
	}
}

void Media::ABlend::AlphaBlend8_C8::SetColorSess(Optional<Media::ColorSess> colorSess)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Media::ColorSess> nncolorSess;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->RemoveHandler(*this);
		this->changed = true;
	}
	this->colorSess = colorSess;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->AddHandler(*this);
		this->changed = true;
	}
}

void Media::ABlend::AlphaBlend8_C8::EndColorSess(NN<Media::ColorSess> colorSess)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Media::ColorSess> nncolorSess;
	if (this->colorSess.SetTo(nncolorSess) && nncolorSess == colorSess)
	{
		nncolorSess->RemoveHandler(*this);
		this->colorSess = 0;
		this->changed = true;
	}
}
