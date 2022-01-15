#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncVLog.h"

//http://pro-av.panasonic.net/en/varicam/common/pdf/VARICAM_V-Log_V-Gamut.pdf

Media::CS::TransferFuncVLog::TransferFuncVLog() : Media::CS::TransferFunc(Media::CS::TRANT_VLOG, 2.2)
{
}

Media::CS::TransferFuncVLog::~TransferFuncVLog()
{
}

Double Media::CS::TransferFuncVLog::ForwardTransfer(Double linearVal)
{
	if (linearVal >= 0.01)
	{
		return 0.598206 + Math_Log10(linearVal + 0.00873) * 0.241514;
	}
	else
	{
		return 5.6 * linearVal + 0.125;
	}
}

Double Media::CS::TransferFuncVLog::InverseTransfer(Double gammaVal)
{
	if (gammaVal >= 0.181)
	{
		return Math_Pow(10.0, ((gammaVal - 0.598206) / 0.241514)) - 0.00873;
	}
	else
	{
		return (gammaVal - 0.125) / 5.6;
	}
}
