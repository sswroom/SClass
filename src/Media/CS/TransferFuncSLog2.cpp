#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CS/TransferFuncSLog2.h"

//https://pro.sony.com/bbsccms/assets/files/micro/dmpc/training/S-Log2_Technical_PaperV1_0.pdf

Media::CS::TransferFuncSLog2::TransferFuncSLog2() : Media::CS::TransferFunc(Media::CS::TRANT_SLOG2, 2.2)
{
}

Media::CS::TransferFuncSLog2::~TransferFuncSLog2()
{
}

Double Media::CS::TransferFuncSLog2::ForwardTransfer(Double linearVal)
{
	if (linearVal >= 0)
	{
		return (0.432699 * Math_Log10(linearVal * 155.0 / 219.0 + 0.037584) + 0.616596) + 0.03;
	}
	else
	{
		return linearVal * 3.53881278538813 + 0.030001222851889303;
	}
}

Double Media::CS::TransferFuncSLog2::InverseTransfer(Double gammaVal)
{
	if (gammaVal >= 0.030001222851889303)
	{
		return 219.0 * (Math_Pow(10, ((gammaVal - 0.616596 - 0.03) / 0.432699)) - 0.037584) / 155.0;
	}
	else
	{
		return (gammaVal - 0.030001222851889303) / 3.53881278538813;
	}
}
