#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncLog100.h"

Media::CS::TransferFuncLog100::TransferFuncLog100() : Media::CS::TransferFunc(Media::CS::TRANT_LOG100, 2.2)
{
}

Media::CS::TransferFuncLog100::~TransferFuncLog100()
{
}

Double Media::CS::TransferFuncLog100::ForwardTransfer(Double linearVal)
{
	if (linearVal >= 0.01)
		return 1.0 + Math_Log10(linearVal) * 0.5;
	else
		return 0;
}

Double Media::CS::TransferFuncLog100::InverseTransfer(Double gammaVal)
{
	if (gammaVal > 0)
		return Math_Pow(10, (gammaVal - 1.0) * 2.0);
	else
		return 0;
}
