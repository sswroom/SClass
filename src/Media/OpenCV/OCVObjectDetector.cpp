#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Media/OpenCV/OCVInit.h"
#include "Media/OpenCV/OCVObjectDetector.h"
#include "Media/OpenCV/OCVUtil.h"
#include "Text/MyString.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

Media::OpenCV::OCVObjectDetector::OCVObjectDetector(const UTF8Char *path, const UTF8Char *dataFile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	this->cascade = 0;
	this->detectResultHdlr = 0;
	this->detectResultObj = 0;
	sptr = Media::OpenCV::OCVUtil::GetDataPath(sbuff, path);
	if (sptr)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcat(sptr, dataFile);
		cv::CascadeClassifier *cascade = new cv::CascadeClassifier((char*)sbuff);
		if (cascade->empty())
		{
			delete cascade;
		}
		else
		{
			this->cascade = cascade;
		}
	}
}

Media::OpenCV::OCVObjectDetector::~OCVObjectDetector()
{
	if (this->cascade)
	{
		cv::CascadeClassifier *cascade = (cv::CascadeClassifier*)this->cascade;
		delete cascade;
		this->cascade = 0;
	}
}

Bool Media::OpenCV::OCVObjectDetector::IsError()
{
	return this->cascade == 0;
}

void Media::OpenCV::OCVObjectDetector::NextFrame(NN<Media::OpenCV::OCVFrame> frame, Media::FrameInfo *info, UInt8 **imgData)
{
	cv::Mat *fr = (cv::Mat*)frame->GetFrame();
	if (this->cascade)
	{
		cv::CascadeClassifier *cascade = (cv::CascadeClassifier*)this->cascade;
		std::vector<cv::Rect> objs;
		cv::equalizeHist(*fr, *fr);
		cascade->detectMultiScale(*fr, objs);

		printf("Frame detect count = %d\r\n", (Int32)objs.size());
		if (this->detectResultHdlr)
		{
			UOSInt i;
			UOSInt j = objs.size();
			if (j > 0)
			{
				ObjectRect *objRects = MemAlloc(ObjectRect, j);
				i = 0;
				while (i < j)
				{
					objRects[i].left = objs[i].x;
					objRects[i].top = objs[i].y;
					objRects[i].right = objRects[i].left + objs[i].width;
					objRects[i].bottom = objRects[i].top + objs[i].height;
					i++;
				}
				this->detectResultHdlr(this->detectResultObj, j, objRects, info, imgData);
				MemFree(objRects);
			}
			else
			{
				this->detectResultHdlr(this->detectResultObj, j, 0, info, imgData);
			}
		}
	}
}

void Media::OpenCV::OCVObjectDetector::HandleDetectResult(DetectResultFunc func, void *userObj)
{
	this->detectResultObj = userObj;
	this->detectResultHdlr = func;
}
