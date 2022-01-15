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
	Double dnTap = Math::UOSInt2Double(nTap);

	if ((aphase * 2) >= dnTap){
		return 0.0;
	}

	ret = Math_Sin(Math::PI * phase) * Math_Sin(Math::PI * phase / dnTap * 2) / (Math::PI * Math::PI * phase * phase / dnTap * 2);

	return ret;
}
