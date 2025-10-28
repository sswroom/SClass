#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CS/TransferFuncSMPTE240.h"

#define cs240MK1 0.0228
#define cs240MK2 4.0
#define cs240MK3 0.1115
#define cs240MK4 0.45

#define cs240MC1 (cs240MK1 * cs240MK2)
#define cs240MC2 (1.0 / cs240MK4)

Media::CS::TransferFuncSMPTE240::TransferFuncSMPTE240() : Media::CS::TransferFunc(Media::CS::TRANT_SMPTE240, 2.2)
{
}

Media::CS::TransferFuncSMPTE240::~TransferFuncSMPTE240()
{
}

Double Media::CS::TransferFuncSMPTE240::ForwardTransfer(Double linearVal)
{
	if (linearVal <= -cs240MK1)
		return (-1 - cs240MK3) * Math_Pow(-linearVal, cs240MK4) + cs240MK3;
	else if (linearVal < cs240MK1)
		return cs240MK2 * linearVal;
	else
		return (1 + cs240MK3) * Math_Pow(linearVal, cs240MK4) - cs240MK3;
}

Double Media::CS::TransferFuncSMPTE240::InverseTransfer(Double gammaVal)
{
	if (gammaVal <= -cs240MC1)
		return -Math_Pow((-gammaVal + cs240MK3) / (1 + cs240MK3), cs240MC2);
	else if (gammaVal < cs240MC1)
		return gammaVal / cs240MK2;
	else
		return Math_Pow((gammaVal + cs240MK3) / (1 + cs240MK3), cs240MC2);
}
