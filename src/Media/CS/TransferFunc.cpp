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
#include "Media/CS/TransferFuncVLog.h"

Media::CS::TransferParam::TransferParam()
{
	this->lut = 0;
	this->tranType = Media::CS::TRANT_sRGB;
	this->gamma = 2.2;
	this->params = 0;
	this->paramCnt = 0;
}

Media::CS::TransferParam::TransferParam(TransferParam *tran)
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

Media::CS::TransferParam::TransferParam(Media::LUT *lut)
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

void Media::CS::TransferParam::Set(Media::LUT *lut)
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

void Media::CS::TransferParam::Set(const TransferParam *tran)
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

Media::CS::TransferType Media::CS::TransferParam::GetTranType()
{
	return this->tranType;
}

Double Media::CS::TransferParam::GetGamma()
{
	return this->gamma;
}

Media::LUT *Media::CS::TransferParam::GetLUT()
{
	return this->lut;
}

Bool Media::CS::TransferParam::Equals(const TransferParam *tran)
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

Media::CS::TransferFunc::TransferFunc(TransferType tranType, Double gamma)
{
	NEW_CLASS(this->param, Media::CS::TransferParam(tranType, gamma));
}

Media::CS::TransferFunc::TransferFunc(Media::LUT *lut)
{
	NEW_CLASS(this->param, Media::CS::TransferParam(lut));
}

Media::CS::TransferFunc::~TransferFunc()
{
	DEL_CLASS(this->param);
}

Media::CS::TransferType Media::CS::TransferFunc::GetTransferType()
{
	return this->param->GetTranType();
}

Double Media::CS::TransferFunc::GetTransferGamma()
{
	return this->param->GetGamma();
}

const Media::CS::TransferParam *Media::CS::TransferFunc::GetTransferParam()
{
	return this->param;
}

Media::CS::TransferFunc *Media::CS::TransferFunc::CreateFunc(Media::CS::TransferParam *param)
{
	Media::CS::TransferFunc *func;
	if (param->GetTranType() == Media::CS::TRANT_sRGB)
	{
		NEW_CLASS(func, Media::CS::TransferFuncSRGB());
	}
	else if (param->GetTranType() == Media::CS::TRANT_BT709)
	{
		NEW_CLASS(func, Media::CS::TransferFuncBT709());
	}
	else if (param->GetTranType() == Media::CS::TRANT_GAMMA)
	{
		NEW_CLASS(func, Media::CS::TransferFuncCGamma(param->GetGamma()));
	}
	else if (param->GetTranType() == Media::CS::TRANT_BT1361)
	{
		NEW_CLASS(func, Media::CS::TransferFuncBT1361());
	}
	else if (param->GetTranType() == Media::CS::TRANT_SMPTE240)
	{
		NEW_CLASS(func, Media::CS::TransferFuncSMPTE240());
	}
	else if (param->GetTranType() == Media::CS::TRANT_LINEAR)
	{
		NEW_CLASS(func, Media::CS::TransferFuncLinear());
	}
	else if (param->GetTranType() == Media::CS::TRANT_LOG100)
	{
		NEW_CLASS(func, Media::CS::TransferFuncLog100());
	}
	else if (param->GetTranType() == Media::CS::TRANT_LOGSQRT10)
	{
		NEW_CLASS(func, Media::CS::TransferFuncLogSqrt10());
	}
	else if (param->GetTranType() == Media::CS::TRANT_NTSC)
	{
		NEW_CLASS(func, Media::CS::TransferFuncNTSC());
	}
	else if (param->GetTranType() == Media::CS::TRANT_SLOG)
	{
		NEW_CLASS(func, Media::CS::TransferFuncSLog());
	}
	else if (param->GetTranType() == Media::CS::TRANT_SLOG1)
	{
		NEW_CLASS(func, Media::CS::TransferFuncSLog1());
	}
	else if (param->GetTranType() == Media::CS::TRANT_SLOG2)
	{
		NEW_CLASS(func, Media::CS::TransferFuncSLog2());
	}
	else if (param->GetTranType() == Media::CS::TRANT_SLOG3)
	{
		NEW_CLASS(func, Media::CS::TransferFuncSLog3());
	}
	else if (param->GetTranType() == Media::CS::TRANT_VLOG)
	{
		NEW_CLASS(func, Media::CS::TransferFuncVLog());
	}
	else if (param->GetTranType() == Media::CS::TRANT_PROTUNE)
	{
		NEW_CLASS(func, Media::CS::TransferFuncProtune());
	}
	else if (param->GetTranType() == Media::CS::TRANT_LUT)
	{
		NEW_CLASS(func, Media::CS::TransferFuncLUT(param->GetLUT()));
	}
	else if (param->GetTranType() == Media::CS::TRANT_BT2100)
	{
		NEW_CLASS(func, Media::CS::TransferFuncBT2100());
	}
	else if (param->GetTranType() == Media::CS::TRANT_HLG)
	{
		NEW_CLASS(func, Media::CS::TransferFuncHLG());
	}
	else if (param->GetTranType() == Media::CS::TRANT_NLOG)
	{
		NEW_CLASS(func, Media::CS::TransferFuncNLog());
	}
	else if (param->GetTranType() == Media::CS::TRANT_PARAM1)
	{
		NEW_CLASS(func, Media::CS::TransferFuncParam1(param->params));
	}
	else
	{
		NEW_CLASS(func, Media::CS::TransferFuncSRGB());
	}
	return func;
}

Double Media::CS::TransferFunc::GetRefLuminance(Media::CS::TransferParam *param)
{
	if (param->GetTranType() == Media::CS::TRANT_BT2100)
	{
		return 1000.0;
	}
	return 0.0;
}

const UTF8Char *Media::CS::TransferFunc::GetTransferFuncName(Media::CS::TransferType ttype)
{
	switch (ttype)
	{
	case Media::CS::TRANT_sRGB:
		return (const UTF8Char*)"sRGB";
	case Media::CS::TRANT_GAMMA:
		return (const UTF8Char*)"Constant Gamma";
	case Media::CS::TRANT_LINEAR:
		return (const UTF8Char*)"Linear RGB";
	case Media::CS::TRANT_BT709:
		return (const UTF8Char*)"BT.709";
	case Media::CS::TRANT_SMPTE240:
		return (const UTF8Char*)"SMPTE 240M";
	case Media::CS::TRANT_BT1361:
		return (const UTF8Char*)"BT.1361";
	case Media::CS::TRANT_BT2100:
		return (const UTF8Char*)"BT.2100/SMPTE ST 2084 (HDR10)";
	case Media::CS::TRANT_HLG:
		return (const UTF8Char*)"Hybrid Log Gamma (HLG)";
	case Media::CS::TRANT_LOG100:
		return (const UTF8Char*)"Log Transfer (100:1)";
	case Media::CS::TRANT_LOGSQRT10:
		return (const UTF8Char*)"Log Transfer (100 * Sqrt(10) : 1)";
	case Media::CS::TRANT_NLOG:
		return (const UTF8Char*)"N-Log";
	case Media::CS::TRANT_NTSC:
		return (const UTF8Char*)"NTSC";
	case Media::CS::TRANT_SLOG:
		return (const UTF8Char*)"Sony S-Log";
	case Media::CS::TRANT_SLOG1:
		return (const UTF8Char*)"Sony S-Log1";
	case Media::CS::TRANT_SLOG2:
		return (const UTF8Char*)"Sony S-Log2";
	case Media::CS::TRANT_SLOG3:
		return (const UTF8Char*)"Sony S-Log3";
	case Media::CS::TRANT_VLOG:
		return (const UTF8Char*)"Panasonic V-Log";
	case Media::CS::TRANT_PROTUNE:
		return (const UTF8Char*)"GoPro Protune";
	case Media::CS::TRANT_LUT:
		return (const UTF8Char*)"LUT";
	case Media::CS::TRANT_PARAM1:
		return (const UTF8Char*)"Parameter Function1";
	case Media::CS::TRANT_VDISPLAY:
		return (const UTF8Char*)"As Display (Video)";
	case Media::CS::TRANT_PDISPLAY:
		return (const UTF8Char*)"As Display (Picture)";
	case Media::CS::TRANT_VUNKNOWN:
		return (const UTF8Char*)"Unknown (Video)";
	case Media::CS::TRANT_PUNKNOWN:
		return (const UTF8Char*)"Unknown (Picture)";
	default:
		return (const UTF8Char*)"Unknown function";
	}
}
