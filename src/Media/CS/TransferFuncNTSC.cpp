#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncNTSC.h"

#define csGammaC1 (1 / 0.45)
#define csGammaC2 (0.45)

Media::CS::TransferFuncNTSC::TransferFuncNTSC() : Media::CS::TransferFunc(Media::CS::TRANT_NTSC, 2.2)
{
}

Media::CS::TransferFuncNTSC::~TransferFuncNTSC()
{
}

Double Media::CS::TransferFuncNTSC::ForwardTransfer(Double linearVal)
{
	if (linearVal < 0)
		return -Math::Pow(-linearVal, csGammaC2) * 0.925 + 0.075;
	else if (linearVal == 0)
		return 0.075;
	else
		return Math::Pow(linearVal, csGammaC2) * 0.925 + 0.075;
}

Double Media::CS::TransferFuncNTSC::InverseTransfer(Double gammaVal)
{
	if (gammaVal < 0.075)
		return -Math::Pow(-(gammaVal - 0.075) / 0.925, csGammaC1);
	else if (gammaVal == 0.075)
		return 0;
	else
		return Math::Pow((gammaVal - 0.075) / 0.925, csGammaC1);
}
