#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
Media::OpenCV::OCVNumPlateFinder::OCVNumPlateFinder()
{
	this->maxTiltAngle = 30;
	this->minArea = 2000;
	this->maxArea = 20000;
}

Media::OpenCV::OCVNumPlateFinder::~OCVNumPlateFinder()
{
}

void Media::OpenCV::OCVNumPlateFinder::SetMaxTiltAngle(Double maxTiltAngleDegree)
{
	this->maxTiltAngle = maxTiltAngleDegree;
}

void Media::OpenCV::OCVNumPlateFinder::SetAreaRange(Double minArea, Double maxArea)
{
	this->minArea = minArea;
	this->maxArea = maxArea;
}

void Media::OpenCV::OCVNumPlateFinder::Find(NN<Media::OpenCV::OCVFrame> frame, PossibleAreaFunc func, AnyType userObj)
{
}
