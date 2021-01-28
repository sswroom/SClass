#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncSLog1.h"

//https://pro.sony.com/bbsccms/assets/files/micro/dmpc/training/S-Log2_Technical_PaperV1_0.pdf

Media::CS::TransferFuncSLog1::TransferFuncSLog1() : Media::CS::TransferFunc(Media::CS::TRANT_SLOG1, 2.2)
{
}

Media::CS::TransferFuncSLog1::~TransferFuncSLog1()
{
}

Double Media::CS::TransferFuncSLog1::ForwardTransfer(Double linearVal)
{
	if (linearVal >= 0)
	{
		return (0.432699 * Math::Log10(linearVal + 0.037584) + 0.616596) + 0.03;
	}
	else
	{
		return linearVal * 5.0 + 0.030001222851889303;
	}
}

Double Media::CS::TransferFuncSLog1::InverseTransfer(Double gammaVal)
{
	if (gammaVal >= 0.030001222851889303)
	{
		return Math::Pow(10, ((gammaVal - 0.616596 - 0.03) / 0.432699)) - 0.037584;
	}
	else
	{
		return (gammaVal - 0.030001222851889303) / 5.0;
	}
}
