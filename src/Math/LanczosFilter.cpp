#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Math/LanczosFilter.h"
#include <float.h>

Math::LanczosFilter::LanczosFilter(UIntOS nTap)
{
	this->nTap = UIntOS2Double(nTap);
	this->iTap = 2 / this->nTap;
}

Math::LanczosFilter::~LanczosFilter()
{
}

Double Math::LanczosFilter::Weight(Double phase)
{
	if (phase < 0)
	{
		if(phase > -DBL_EPSILON)
		{
			return 1.0;
		}
		if ((phase * -2) >= nTap){
			return 0.0;
		}

	}
	else
	{
		if(phase < DBL_EPSILON)
		{
			return 1.0;
		}
		if ((phase * 2) >= nTap){
			return 0.0;
		}
	}

	Double pp = Math::PI * phase;
	return Math_Sin(pp) * Math_Sin(pp * this->iTap) / (pp * pp * this->iTap);
}
