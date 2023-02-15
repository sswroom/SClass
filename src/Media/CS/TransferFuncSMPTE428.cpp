#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncSMPTE428.h"

Media::CS::TransferFuncSMPTE428::TransferFuncSMPTE428() : Media::CS::TransferFunc(Media::CS::TRANT_SMPTE428, 2.2)
{
}

Media::CS::TransferFuncSMPTE428::~TransferFuncSMPTE428()
{
}

Double Media::CS::TransferFuncSMPTE428::ForwardTransfer(Double linearVal)
{
	return Math_Pow(48 * linearVal / 52.37, 1.0 / 2.6);
}

Double Media::CS::TransferFuncSMPTE428::InverseTransfer(Double gammaVal)
{
	return 52.37 / 48.0 * Math_Pow(gammaVal, 2.6);
}
