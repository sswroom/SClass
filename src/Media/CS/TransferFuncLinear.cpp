#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/TransferFuncLinear.h"

Media::CS::TransferFuncLinear::TransferFuncLinear() : Media::CS::TransferFunc(Media::CS::TRANT_LINEAR, 1.0)
{
}

Media::CS::TransferFuncLinear::~TransferFuncLinear()
{
}

Double Media::CS::TransferFuncLinear::ForwardTransfer(Double linearVal)
{
	return linearVal;
}

Double Media::CS::TransferFuncLinear::InverseTransfer(Double gammaVal)
{
	return gammaVal;
}

