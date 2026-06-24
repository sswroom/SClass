#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Media/OpenCV/OCVInit.h"
#include "Media/OpenCV/OCVObjectDetector.h"
#include "Media/OpenCV/OCVUtil.h"
#include "Text/MyString.h"

Media::OpenCV::OCVObjectDetector::OCVObjectDetector(UnsafeArray<const UTF8Char> path, UnsafeArray<const UTF8Char> dataFile)
{
	this->cascade = 0;
	this->detectResultHdlr = 0;
	this->detectResultObj = 0;
}

Media::OpenCV::OCVObjectDetector::~OCVObjectDetector()
{
}

Bool Media::OpenCV::OCVObjectDetector::IsError()
{
	return this->cascade == 0;
}

void Media::OpenCV::OCVObjectDetector::NextFrame(NN<Media::OpenCV::OCVFrame> frame, NN<Media::FrameInfo> info, UnsafeArray<UnsafeArray<UInt8>> imgData)
{
}

void Media::OpenCV::OCVObjectDetector::HandleDetectResult(DetectResultFunc func, AnyType userObj)
{
	this->detectResultObj = userObj;
	this->detectResultHdlr = func;
}
