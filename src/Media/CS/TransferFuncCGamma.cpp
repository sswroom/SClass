#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncCGamma.h"

#define csGammaC1 (this->param->GetGamma())
#define csGammaC2 (1 / this->param->GetGamma())

Media::CS::TransferFuncCGamma::TransferFuncCGamma(Double rgbGamma) : Media::CS::TransferFunc(Media::CS::TRANT_GAMMA, rgbGamma)
{
}

Media::CS::TransferFuncCGamma::~TransferFuncCGamma()
{
}

Double Media::CS::TransferFuncCGamma::ForwardTransfer(Double linearVal)
{
	if (linearVal < 0)
		return -Math_Pow(-linearVal, csGammaC2);
	else if (linearVal == 0)
		return 0;
	else
		return Math_Pow(linearVal, csGammaC2);
}

Double Media::CS::TransferFuncCGamma::InverseTransfer(Double gammaVal)
{
	if (gammaVal < 0)
		return -Math_Pow(-gammaVal, csGammaC1);
	else if (gammaVal == 0)
		return 0;
	else
		return Math_Pow(gammaVal, csGammaC1);
}
