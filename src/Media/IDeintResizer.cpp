#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/IDeintResizer.h"

Media::IDeintResizer::IDeintResizer(Media::AlphaType srcAlphaType)
{
	this->srcAlphaType = srcAlphaType;
}

void Media::IDeintResizer::SetSrcAlphaType(Media::AlphaType alphaType)
{
	this->srcAlphaType = alphaType;
}

void Media::IDeintResizer::SetDISrcRefLuminance(Double srcRefLuminance)
{
}
