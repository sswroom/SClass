#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/LanczosFilter.h"
#include <float.h>

Double Math::LanczosFilter::Weight(Double phase, UOSInt nTap)
{
	Double ret;
	Double aphase = Math_Abs(phase);
	
	if(aphase < DBL_EPSILON)
	{
		return 1.0;
	}
	Double dnTap = UOSInt2Double(nTap);

	if ((aphase * 2) >= dnTap){
		return 0.0;
	}

	Double pp = Math::PI * phase;
	Double itap = 2 / dnTap;
	ret = Math_Sin(pp) * Math_Sin(pp * itap) / (pp * pp * itap);

	return ret;
}
