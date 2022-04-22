#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
Media::OpenCV::OCVNumPlateFinder::OCVNumPlateFinder()
{
	this->maxTileAngle = 30;
	this->minArea = 2000;
	this->maxArea = 20000;
}

Media::OpenCV::OCVNumPlateFinder::~OCVNumPlateFinder()
{
}

void Media::OpenCV::OCVNumPlateFinder::SetMaxTileAngle(Double maxTileAngleDegree)
{
	this->maxTileAngle = maxTileAngleDegree;
}

void Media::OpenCV::OCVNumPlateFinder::SetAreaRange(Double minArea, Double maxArea)
{
	this->minArea = minArea;
	this->maxArea = maxArea;
}

void Media::OpenCV::OCVNumPlateFinder::Find(Media::OpenCV::OCVFrame *frame, PossibleAreaFunc func, void *userObj)
{
}
