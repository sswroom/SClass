#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncNLog.h"

//http://download.nikonimglib.com/archive3/hDCmK00m9JDI03RPruD74xpoU905/N-Log_Specification_(En)01.pdf

Media::CS::TransferFuncNLog::TransferFuncNLog() : Media::CS::TransferFunc(Media::CS::TRANT_NLOG, 2.2)
{
}

Media::CS::TransferFuncNLog::~TransferFuncNLog()
{
}

Double Media::CS::TransferFuncNLog::ForwardTransfer(Double linearVal)
{
	if (linearVal < 0.328)
	{
		return 650 * Math_Pow((linearVal + 0.0075), 1 / 3.0) / 1023.0;
	}
	else
	{
		return (150 * Math_Ln(linearVal) + 619) / 1023.0;
	}
}

Double Media::CS::TransferFuncNLog::InverseTransfer(Double gammaVal)
{
	Double iVal = gammaVal * 1023.0;
	if (iVal < 452)
	{
		return Math_Pow(iVal / 650.0, 3) - 0.0075;
	}
	else
	{
		return Math_Exp((iVal - 619.0) / 150.0);
	}
}
