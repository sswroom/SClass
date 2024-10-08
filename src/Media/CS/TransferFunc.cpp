#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/TransferFunc.h"
#include "Media/CS/TransferFuncSRGB.h"
#include "Media/CS/TransferFuncBT709.h"
#include "Media/CS/TransferFuncBT1361.h"
#include "Media/CS/TransferFuncBT2100.h"
#include "Media/CS/TransferFuncCGamma.h"
#include "Media/CS/TransferFuncHLG.h"
#include "Media/CS/TransferFuncLinear.h"
#include "Media/CS/TransferFuncLog100.h"
#include "Media/CS/TransferFuncLogSqrt10.h"
#include "Media/CS/TransferFuncLUT.h"
#include "Media/CS/TransferFuncNLog.h"
#include "Media/CS/TransferFuncNTSC.h"
#include "Media/CS/TransferFuncParam1.h"
#include "Media/CS/TransferFuncProtune.h"
#include "Media/CS/TransferFuncSLog.h"
#include "Media/CS/TransferFuncSLog1.h"
#include "Media/CS/TransferFuncSLog2.h"
#include "Media/CS/TransferFuncSLog3.h"
#include "Media/CS/TransferFuncSMPTE240.h"
#include "Media/CS/TransferFuncSMPTE428.h"
#include "Media/CS/TransferFuncVLog.h"

Media::CS::TransferParam::TransferParam()
{
	this->lut = 0;
	this->tranType = Media::CS::TRANT_sRGB;
	this->gamma = 2.2;
	this->params = 0;
	this->paramCnt = 0;
}

Media::CS::TransferParam::TransferParam(NN<const TransferParam> tran)
{
	if (tran->lut)
	{
		this->lut = tran->lut->Clone();
	}
	else
	{
		this->lut = 0;
	}
	this->tranType = tran->tranType;
	this->gamma = tran->gamma;
	this->params = 0;
	this->paramCnt = tran->paramCnt;
	if (tran->params)
	{
		this->params = MemAlloc(Double, this->paramCnt);
		MemCopyNO(this->params, tran->params, sizeof(Double) * this->paramCnt);
	}
}

Media::CS::TransferParam::TransferParam(TransferType tranType, Double gamma)
{
	this->lut = 0;
	this->tranType = tranType;
	this->gamma = gamma;
	this->params = 0;
	this->paramCnt = 0;
}

Media::CS::TransferParam::TransferParam(NN<const Media::LUT> lut)
{
	this->lut = lut->Clone();
	this->tranType = Media::CS::TRANT_LUT;
	this->params = 0;
	this->paramCnt = 0;
	this->gamma = 2.2;
}

Media::CS::TransferParam::~TransferParam()
{
	SDEL_CLASS(this->lut);
	if (this->params)
	{
		MemFree(this->params);
	}
}

void Media::CS::TransferParam::Set(TransferType tranType, Double gamma)
{
	this->tranType = tranType;
	this->gamma = gamma;
	SDEL_CLASS(this->lut);
	if (this->params)
	{
		MemFree(this->params);
		this->params = 0;
	}
}

void Media::CS::TransferParam::Set(TransferType tranType, Double *params, UOSInt paramCnt)
{
	this->tranType = tranType;
	this->gamma = 2.2;
	SDEL_CLASS(this->lut);
	if (this->params)
	{
		MemFree(this->params);
		this->params = 0;
	}
	this->paramCnt = paramCnt;
	this->params = MemAlloc(Double, paramCnt);
	MemCopyNO(this->params, params, sizeof(Double) * paramCnt);
}

void Media::CS::TransferParam::Set(NN<Media::LUT> lut)
{
	SDEL_CLASS(this->lut);
	if (this->params)
	{
		MemFree(this->params);
		this->params = 0;
	}
	this->lut = lut->Clone();
	this->tranType = Media::CS::TRANT_LUT;
}

void Media::CS::TransferParam::Set(NN<const TransferParam> tran)
{
	SDEL_CLASS(this->lut);
	if (this->params)
	{
		MemFree(this->params);
		this->params = 0;
	}
	if (tran->lut)
	{
		this->lut = tran->lut->Clone();
	}
	if (tran->params)
	{
		this->paramCnt = tran->paramCnt;
		this->params = MemAlloc(Double, this->paramCnt);
		MemCopyNO(this->params, tran->params, sizeof(Double) * this->paramCnt);
	}
	this->tranType = tran->tranType;
	this->gamma = tran->gamma;
}

Bool Media::CS::TransferParam::Equals(NN<const TransferParam> tran) const
{
	if (this->tranType != tran->tranType)
		return false;
	if (this->tranType == Media::CS::TRANT_GAMMA)
	{
		return this->gamma == tran->gamma;
	}
	else if (this->tranType == Media::CS::TRANT_LUT)
	{
		if ((this->lut == 0) || (tran->lut == 0))
			return false;
		return this->lut->Equals(tran->lut);
	}
	else if (this->tranType == Media::CS::TRANT_PARAM1)
	{
		if (this->paramCnt != tran->paramCnt)
			return false;
		UOSInt i = this->paramCnt;
		while (i-- > 0)
		{
			if (this->params[i] != tran->params[i])
				return false;
		}
		return true;
	}
	else
	{
		return true;
	}
}


Text::CStringNN Media::CS::TransferTypeGetName(Media::CS::TransferType ttype)
{
	switch (ttype)
	{
	case Media::CS::TRANT_sRGB:
		return CSTR("sRGB");
	case Media::CS::TRANT_GAMMA:
		return CSTR("Constant Gamma");
	case Media::CS::TRANT_LINEAR:
		return CSTR("Linear RGB");
	case Media::CS::TRANT_BT709:
		return CSTR("BT.709");
	case Media::CS::TRANT_SMPTE240:
		return CSTR("SMPTE 240M");
	case Media::CS::TRANT_BT1361:
		return CSTR("BT.1361");
	case Media::CS::TRANT_BT2100:
		return CSTR("BT.2100/SMPTE ST 2084 (HDR10)");
	case Media::CS::TRANT_HLG:
		return CSTR("Hybrid Log Gamma (HLG)");
	case Media::CS::TRANT_LOG100:
		return CSTR("Log Transfer (100:1)");
	case Media::CS::TRANT_LOGSQRT10:
		return CSTR("Log Transfer (100 * Sqrt(10) : 1)");
	case Media::CS::TRANT_NLOG:
		return CSTR("N-Log");
	case Media::CS::TRANT_NTSC:
		return CSTR("NTSC");
	case Media::CS::TRANT_SLOG:
		return CSTR("Sony S-Log");
	case Media::CS::TRANT_SLOG1:
		return CSTR("Sony S-Log1");
	case Media::CS::TRANT_SLOG2:
		return CSTR("Sony S-Log2");
	case Media::CS::TRANT_SLOG3:
		return CSTR("Sony S-Log3");
	case Media::CS::TRANT_VLOG:
		return CSTR("Panasonic V-Log");
	case Media::CS::TRANT_PROTUNE:
		return CSTR("GoPro Protune");
	case Media::CS::TRANT_LUT:
		return CSTR("LUT");
	case Media::CS::TRANT_SMPTE428:
		return CSTR("SMPTE ST 428-1");
	case Media::CS::TRANT_PARAM1:
		return CSTR("Parameter Function1");
	case Media::CS::TRANT_VDISPLAY:
		return CSTR("As Display (Video)");
	case Media::CS::TRANT_PDISPLAY:
		return CSTR("As Display (Picture)");
	case Media::CS::TRANT_VUNKNOWN:
		return CSTR("Unknown (Video)");
	case Media::CS::TRANT_PUNKNOWN:
		return CSTR("Unknown (Picture)");
	default:
		return CSTR("Unknown function");
	}
}

Media::CS::TransferFunc::TransferFunc(TransferType tranType, Double gamma) : param(tranType, gamma)
{
}

Media::CS::TransferFunc::TransferFunc(NN<const Media::LUT> lut) : param(lut)
{
}

Media::CS::TransferFunc::~TransferFunc()
{
}

Media::CS::TransferType Media::CS::TransferFunc::GetTransferType()
{
	return this->param.GetTranType();
}

Double Media::CS::TransferFunc::GetTransferGamma()
{
	return this->param.GetGamma();
}

NN<const Media::CS::TransferParam> Media::CS::TransferFunc::GetTransferParam()
{
	return this->param;
}

NN<Media::CS::TransferFunc> Media::CS::TransferFunc::CreateFunc(NN<const Media::CS::TransferParam> param)
{
	NN<Media::CS::TransferFunc> func;
	switch (param->GetTranType())
	{
	case Media::CS::TRANT_sRGB:
		NEW_CLASSNN(func, Media::CS::TransferFuncSRGB());
		return func;
	case Media::CS::TRANT_BT709:
		NEW_CLASSNN(func, Media::CS::TransferFuncBT709());
		return func;
	case Media::CS::TRANT_GAMMA:
		NEW_CLASSNN(func, Media::CS::TransferFuncCGamma(param->GetGamma()));
		return func;
	case Media::CS::TRANT_BT1361:
		NEW_CLASSNN(func, Media::CS::TransferFuncBT1361());
		return func;
	case Media::CS::TRANT_SMPTE240:
		NEW_CLASSNN(func, Media::CS::TransferFuncSMPTE240());
		return func;
	case Media::CS::TRANT_LINEAR:
		NEW_CLASSNN(func, Media::CS::TransferFuncLinear());
		return func;
	case Media::CS::TRANT_LOG100:
		NEW_CLASSNN(func, Media::CS::TransferFuncLog100());
		return func;
	case Media::CS::TRANT_LOGSQRT10:
		NEW_CLASSNN(func, Media::CS::TransferFuncLogSqrt10());
		return func;
	case Media::CS::TRANT_NTSC:
		NEW_CLASSNN(func, Media::CS::TransferFuncNTSC());
		return func;
	case Media::CS::TRANT_SLOG:
		NEW_CLASSNN(func, Media::CS::TransferFuncSLog());
		return func;
	case Media::CS::TRANT_SLOG1:
		NEW_CLASSNN(func, Media::CS::TransferFuncSLog1());
		return func;
	case Media::CS::TRANT_SLOG2:
		NEW_CLASSNN(func, Media::CS::TransferFuncSLog2());
		return func;
	case Media::CS::TRANT_SLOG3:
		NEW_CLASSNN(func, Media::CS::TransferFuncSLog3());
		return func;
	case Media::CS::TRANT_VLOG:
		NEW_CLASSNN(func, Media::CS::TransferFuncVLog());
		return func;
	case Media::CS::TRANT_PROTUNE:
		NEW_CLASSNN(func, Media::CS::TransferFuncProtune());
		return func;
	case Media::CS::TRANT_LUT:
	{
		NN<const Media::LUT> lut;
		if (lut.Set(param->GetLUTRead()))
		{
			NEW_CLASSNN(func, Media::CS::TransferFuncLUT(lut));
			return func;
		}
		else
		{
			NEW_CLASSNN(func, Media::CS::TransferFuncSRGB());
			return func;
		}
	}
	case Media::CS::TRANT_BT2100:
		NEW_CLASSNN(func, Media::CS::TransferFuncBT2100());
		return func;
	case Media::CS::TRANT_HLG:
		NEW_CLASSNN(func, Media::CS::TransferFuncHLG());
		return func;
	case Media::CS::TRANT_NLOG:
		NEW_CLASSNN(func, Media::CS::TransferFuncNLog());
		return func;
	case Media::CS::TRANT_PARAM1:
		NEW_CLASSNN(func, Media::CS::TransferFuncParam1(param->params));
		return func;
	case Media::CS::TRANT_SMPTE428:
		NEW_CLASSNN(func, Media::CS::TransferFuncSMPTE428());
		return func;
	case Media::CS::TRANT_VDISPLAY:
	case Media::CS::TRANT_PDISPLAY:
	case Media::CS::TRANT_VUNKNOWN:
	case Media::CS::TRANT_PUNKNOWN:
	default:
		NEW_CLASSNN(func, Media::CS::TransferFuncSRGB());
		return func;
	}
}

Double Media::CS::TransferFunc::GetRefLuminance(NN<const Media::CS::TransferParam> param)
{
	if (param->GetTranType() == Media::CS::TRANT_BT2100)
	{
		return 1000.0;
	}
	return 0.0;
}
