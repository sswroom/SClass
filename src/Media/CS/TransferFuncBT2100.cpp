#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CS/TransferFuncBT2100.h"

#define csBT2100m1 (2610.0/16384.0)
#define csBT2100m2 (2523.0/4096.0*128.0)
#define csBT2100c1 (3424.0/4096.0)
#define csBT2100c2 (2413.0/4096.0*32.0)
#define csBT2100c3 (2392.0/4096.0*32.0)

//Normally 1.0 means 100 cdm2
#define csBT2100Rate 10.0

Media::CS::TransferFuncBT2100::TransferFuncBT2100() : Media::CS::TransferFunc(Media::CS::TRANT_BT2100, 2.2)
{
}

Media::CS::TransferFuncBT2100::~TransferFuncBT2100()
{
}

Double Media::CS::TransferFuncBT2100::ForwardTransfer(Double linearVal)
{
	Double vTmp = Math_Pow(linearVal / csBT2100Rate, csBT2100m1);
	Double v1 = csBT2100c1 + csBT2100c2 * vTmp;
	Double v2 = 1 + csBT2100c3 * vTmp;
	return Math_Pow(v1 / v2, csBT2100m2);
}

Double Media::CS::TransferFuncBT2100::InverseTransfer(Double gammaVal)
{
	Double vTmp = Math_Pow(gammaVal, 1 / csBT2100m2);
	Double v1 = vTmp - csBT2100c1;
	Double v2 = csBT2100c2 - csBT2100c3 * vTmp;
	if (v1 < 0)
		v1 = 0;

	return csBT2100Rate * Math_Pow(v1 / v2, 1 / csBT2100m1);
}
