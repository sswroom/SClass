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

void Media::VideoSourceBase::SetBorderCrop(OSInt cropLeft, OSInt cropTop, OSInt cropRight, OSInt cropBottom)
{
	this->cropLeft = cropLeft;
	this->cropTop = cropTop;
	this->cropRight = cropRight;
	this->cropBottom = cropBottom;
}

void Media::VideoSourceBase::GetBorderCrop(OSInt *cropLeft, OSInt *cropTop, OSInt *cropRight, OSInt *cropBottom)
{
	*cropLeft = this->cropLeft;
	*cropTop = this->cropTop;
	*cropRight = this->cropRight;
	*cropBottom = this->cropBottom;
}
