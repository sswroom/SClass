#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/FrameInfo.h"
#include "Media/CS/CSConverter.h"
#include "Media/OpenCV/OCVFrame.h"
#include "Media/OpenCV/OCVInit.h"

#include <opencv2/imgproc.hpp>
#include <stdio.h>

Media::OpenCV::OCVFrame::OCVFrame(void *frame)
{
	this->frame = frame;
}

Media::OpenCV::OCVFrame::~OCVFrame()
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	delete fr;
}

void *Media::OpenCV::OCVFrame::GetFrame()
{
	return this->frame;
}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CreateYFrame(UInt8 **imgData, UOSInt dataSize, UInt32 fourcc, UOSInt dispWidth, UOSInt dispHeight, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf)
{
	Media::CS::CSConverter *converter = Media::CS::CSConverter::NewConverter(fourcc, storeBPP, pf, 0, *(UInt32*)"Y800", 8, Media::PF_UNKNOWN, 0, Media::ColorProfile::YUVT_UNKNOWN, 0);
	if (converter)
	{
		cv::Mat *fr;
		fr = new cv::Mat(dispHeight, dispWidth, CV_8UC1);
		converter->ConvertV2(imgData, fr->ptr(0), dispWidth, dispHeight, storeWidth, dispHeight, dispWidth, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);

		DEL_CLASS(converter);
		Media::OpenCV::OCVFrame *frame;
		NEW_CLASS(frame, Media::OpenCV::OCVFrame(fr));
		return frame;
	}
	else
	{
		if (fourcc < 1024)
		{
			printf("OCVFrame: Unsupported Format: %d\r\n", fourcc);
		}
		else
		{
			UTF8Char sbuff[5];
			*(Int32*)sbuff = fourcc;
			sbuff[4] = 0;
			printf("OCVFrame: Unsupported Format: %s\r\n", sbuff);
		}
		
		return 0;		
	}
}
