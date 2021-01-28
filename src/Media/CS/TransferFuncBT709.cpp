#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncBT709.h"

#define csBT709K1 0.018
#define csBT709K2 4.5
#define csBT709K3 0.099
#define csBT709K4 0.45

#define csBT709C1 (csBT709K1 * csBT709K2)
#define csBT709C2 (1.0 / csBT709K4)

Media::CS::TransferFuncBT709::TransferFuncBT709() : Media::CS::TransferFunc(Media::CS::TRANT_BT709, 2.2)
{
}

Media::CS::TransferFuncBT709::~TransferFuncBT709()
{
}

Double Media::CS::TransferFuncBT709::ForwardTransfer(Double linearVal)
{
	if (linearVal <= -csBT709K1)
		return (-1 - csBT709K3) * Math::Pow(-linearVal, csBT709K4) + csBT709K3;
	else if (linearVal < csBT709K1)
		return csBT709K2 * linearVal;
	else
		return (1 + csBT709K3) * Math::Pow(linearVal, csBT709K4) - csBT709K3;
}

Double Media::CS::TransferFuncBT709::InverseTransfer(Double gammaVal)
{
	if (gammaVal <= -csBT709C1)
		return -Math::Pow((-gammaVal + csBT709K3) / (1 + csBT709K3), csBT709C2);
	else if (gammaVal < csBT709C1)
		return gammaVal / csBT709K2;
	else
		return Math::Pow((gammaVal + csBT709K3) / (1 + csBT709K3), csBT709C2);
}
