#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncProtune.h"

Media::CS::TransferFuncProtune::TransferFuncProtune() : Media::CS::TransferFunc(Media::CS::TRANT_PROTUNE, 2.2)
{
}

Media::CS::TransferFuncProtune::~TransferFuncProtune()
{
}

Double Media::CS::TransferFuncProtune::ForwardTransfer(Double linearVal)
{
	return Math_Log10(linearVal * 112.0 + 1.0) / Math_Log10(113.0);
}

Double Media::CS::TransferFuncProtune::InverseTransfer(Double gammaVal)
{
	return (Math_Pow(113.0, gammaVal) - 1.0) / 112.0;
}
