#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Media/OpenCV/OCVInit.h"
#include "Media/OpenCV/OCVObjectDetector.h"
#include "Media/OpenCV/OCVUtil.h"
#include "Text/MyString.h"

Media::OpenCV::OCVObjectDetector::OCVObjectDetector(const UTF8Char *path, const UTF8Char *dataFile)
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

void Media::OpenCV::OCVObjectDetector::NextFrame(Media::OpenCV::OCVFrame *frame, Media::FrameInfo *info, UInt8 **imgData)
{
}

void Media::OpenCV::OCVObjectDetector::HandleDetectResult(DetectResultFunc func, void *userObj)
{
	this->detectResultObj = userObj;
	this->detectResultHdlr = func;
}
