#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncBT1361.h"

#define csBT1361K1 0.018
#define csBT1361K2 4.5
#define csBT1361K3 0.099
#define csBT1361K4 0.45
#define csBT1361K5 -0.0045

#define csBT1361C1 (csBT1361K1 * csBT1361K2)
#define csBT1361C2 (1.0 / csBT1361K4)
#define csBT1361C3 (csBT1361K1 * csBT1361K5)

Media::CS::TransferFuncBT1361::TransferFuncBT1361() : Media::CS::TransferFunc(Media::CS::TRANT_BT1361, 2.2)
{
}

Media::CS::TransferFuncBT1361::~TransferFuncBT1361()
{
}

Double Media::CS::TransferFuncBT1361::ForwardTransfer(Double linearVal)
{
	if (linearVal < csBT1361K5)
		return ((1 + csBT1361K3) * Math_Pow(-4 * linearVal, csBT1361K4) - csBT1361K3) / -4.0;
	else if (linearVal < csBT1361K1)
		return csBT1361K2 * linearVal;
	else
		return (1 + csBT1361K3) * Math_Pow(linearVal, csBT1361K4) - csBT1361K3;
}

Double Media::CS::TransferFuncBT1361::InverseTransfer(Double gammaVal)
{
	if (gammaVal < csBT1361C3)
		return Math_Pow(((gammaVal * -4.0) + csBT1361K3) / (1 + csBT1361K3), csBT1361C2) / -4.0;
	else if (gammaVal < csBT1361C1)
		return gammaVal / csBT1361K2;
	else
		return Math_Pow((gammaVal + csBT1361K3) / (1 + csBT1361K3), csBT1361C2);
}
