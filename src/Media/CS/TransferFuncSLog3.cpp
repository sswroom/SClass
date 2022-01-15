#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncSLog3.h"

//http://www.sony.jp/cinealta/knowledge/pdf/TechnicalSummary_for_S-Gamut3Cine_S-Gamut3_S-Log3_V1_00%28J%29.pdf

Media::CS::TransferFuncSLog3::TransferFuncSLog3() : Media::CS::TransferFunc(Media::CS::TRANT_SLOG3, 2.2)
{
}

Media::CS::TransferFuncSLog3::~TransferFuncSLog3()
{
}

Double Media::CS::TransferFuncSLog3::ForwardTransfer(Double linearVal)
{
	if (linearVal >= 0.0112500)
	{
		return (420.0 + Math_Log10((linearVal + 0.01) / (0.18 + 0.01)) * 261.5) / 1023.0;
	}
	else
	{
		return (linearVal * (171.2102946929 - 95.0) / 0.01125000 + 95.0) / 1023.0;
	}
}

Double Media::CS::TransferFuncSLog3::InverseTransfer(Double gammaVal)
{
	if (gammaVal >= 171.2102946929 / 1023.0)
	{
		return Math_Pow(10.0, ((gammaVal * 1023.0 - 420.0) / 261.5)) * (0.18 + 0.01) - 0.01;
	}
	else
	{
		return (gammaVal * 1023.0 - 95.0) * 0.01125000 / (171.2102946929 - 95.0);
	}
}
