#include "Stdafx.h"
#include "Media/VideoSourceBase.h"

Media::VideoSourceBase::VideoSourceBase()
{
	this->cropLeft = 0;
	this->cropTop = 0;
	this->cropRight = 0;
	this->cropBottom = 0;
}

Media::VideoSourceBase::~VideoSourceBase()
{
}

void Media::VideoSourceBase::SetBorderCrop(UIntOS cropLeft, UIntOS cropTop, UIntOS cropRight, UIntOS cropBottom)
{
	this->cropLeft = cropLeft;
	this->cropTop = cropTop;
	this->cropRight = cropRight;
	this->cropBottom = cropBottom;
}

void Media::VideoSourceBase::GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom)
{
	cropLeft.Set(this->cropLeft);
	cropTop.Set(this->cropTop);
	cropRight.Set(this->cropRight);
	cropBottom.Set(this->cropBottom);
}
