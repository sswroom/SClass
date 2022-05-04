#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/LanczosFilter.h"
#include <float.h>

Math::LanczosFilter::LanczosFilter(UOSInt nTap)
{
	this->nTap = UOSInt2Double(nTap);
	this->iTap = 2 / this->nTap;
}

Math::LanczosFilter::~LanczosFilter()
{
}

Double Math::LanczosFilter::Weight(Double phase)
{
	Double ret;
	Double aphase = Math_Abs(phase);
	
	if(aphase < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((aphase * 2) >= nTap){
		return 0.0;
	}

	Double pp = Math::PI * phase;
	ret = Math_Sin(pp) * Math_Sin(pp * this->iTap) / (pp * pp * this->iTap);

	return ret;
}
