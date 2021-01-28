#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncSRGB.h"

#define cssRGBK1 0.0031308
#define cssRGBK2 12.92
#define cssRGBK3 0.055
#define cssRGBK4 (1.0 / 2.4)

#define cssRGBC1 (cssRGBK1 * cssRGBK2)
#define cssRGBC2 (1.0 / cssRGBK4)

Media::CS::TransferFuncSRGB::TransferFuncSRGB() : Media::CS::TransferFunc(Media::CS::TRANT_sRGB, 2.2)
{
}

Media::CS::TransferFuncSRGB::~TransferFuncSRGB()
{
}

Double Media::CS::TransferFuncSRGB::ForwardTransfer(Double linearVal)
{
	if (linearVal < -cssRGBK1)
		return (-1 - cssRGBK3) * Math::Pow(-linearVal, cssRGBK4) + cssRGBK3;
	else if (linearVal <= cssRGBK1)
		return cssRGBK2 * linearVal;
	else
		return (1 + cssRGBK3) * Math::Pow(linearVal, cssRGBK4) - cssRGBK3;
}

Double Media::CS::TransferFuncSRGB::InverseTransfer(Double gammaVal)
{
	if (gammaVal <= -cssRGBC1)
		return -Math::Pow((-gammaVal + cssRGBK3) / (1 + cssRGBK3), cssRGBC2);
	else if (gammaVal < cssRGBC1)
		return gammaVal / cssRGBK2;
	else
		return Math::Pow((gammaVal + cssRGBK3) / (1 + cssRGBK3), cssRGBC2);
}
