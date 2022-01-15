#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncLogSqrt10.h"

Media::CS::TransferFuncLogSqrt10::TransferFuncLogSqrt10() : Media::CS::TransferFunc(Media::CS::TRANT_LOGSQRT10, 2.2)
{
}

Media::CS::TransferFuncLogSqrt10::~TransferFuncLogSqrt10()
{
}

Double Media::CS::TransferFuncLogSqrt10::ForwardTransfer(Double linearVal)
{
	if (linearVal >= 0.0031622776601683793319988935444327)
		return 1.0 + Math_Log10(linearVal) * 0.4;
	else
		return 0;
}

Double Media::CS::TransferFuncLogSqrt10::InverseTransfer(Double gammaVal)
{
	if (gammaVal > 0)
		return Math_Pow(10, (gammaVal - 1.0) * 2.5);
	else
		return 0;
}
