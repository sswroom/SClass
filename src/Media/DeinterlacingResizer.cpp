#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/DeinterlacingResizer.h"

Media::DeinterlacingResizer::DeinterlacingResizer(Media::AlphaType srcAlphaType)
{
	this->srcAlphaType = srcAlphaType;
}

void Media::DeinterlacingResizer::SetSrcAlphaType(Media::AlphaType alphaType)
{
	this->srcAlphaType = alphaType;
}

void Media::DeinterlacingResizer::SetDISrcRefLuminance(Double srcRefLuminance)
{
}
