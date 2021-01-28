#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Media/VideoCaptureMgr.h"
#include "Media/CS/CSConverter.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "Media/OpenCV/OCVFrameFeeder.h"
#include "Media/OpenCV/OCVObjectDetector.h"
#include "Media/OpenCV/OCVUtil.h"
#include "Text/MyString.h"

OSInt lastCnt;
OSInt rangeLeft;
OSInt rangeTop;
OSInt rangeRight;
OSInt rangeBottom;
Int32 preferedFormat;
OSInt preferedWidth;
OSInt preferedHeight;
Media::CS::CSConverter *csConv;
Exporter::GUIJPGExporter *exporter;

void __stdcall OnDetectResult(void *userObj, OSInt objCnt, const Media::OpenCV::OCVObjectDetector::ObjectRect *objRects, Media::FrameInfo *frInfo, UInt8 **imgData)
{
	IO::ConsoleWriter *console = (IO::ConsoleWriter*)userObj;
	OSInt thisCnt = objCnt;
	if (rangeLeft < rangeRight && rangeTop < rangeBottom)
	{
		OSInt i = 0;
		thisCnt = 0;
		while (i < objCnt)
		{
			if (objRects[i].left <= rangeRight && objRects[i].right >= rangeLeft && objRects[i].top <= rangeBottom && objRects[i].bottom >= rangeTop)
			{
				thisCnt++;
			}
			i++;
		}
	}
	if ((lastCnt < thisCnt))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"People detected, cnt = ");
		sb.AppendOSInt(thisCnt);
		console->WriteLine(sb.ToString());

		Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
		if (csConv == 0)
		{
			csConv = Media::CS::CSConverter::NewConverter(frInfo->fourcc, frInfo->storeBPP, frInfo->pf, frInfo->color, 0, 32, Media::PF_B8G8R8A8, &srgb, frInfo->yuvType, 0);
		}
		if (exporter == 0)
		{
			NEW_CLASS(exporter, Exporter::GUIJPGExporter());
		}
		if (csConv)
		{
			Media::ImageList *imgList;
			Media::StaticImage *simg;
			IO::FileStream *fs;
			Data::DateTime dt;
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			NEW_CLASS(simg, Media::StaticImage(frInfo->dispWidth, frInfo->dispHeight, 0, 32, Media::PF_B8G8R8A8, 0, &srgb, frInfo->yuvType, Media::AT_NO_ALPHA, frInfo->ycOfst));
			csConv->ConvertV2(imgData, simg->data, frInfo->dispWidth, frInfo->dispHeight, frInfo->storeWidth, frInfo->storeHeight, frInfo->dispWidth * 4, Media::FT_NON_INTERLACE, frInfo->ycOfst);
			OSInt i = 0;
			while (i < objCnt)
			{
				ImageUtil_DrawRectNA32(simg->data + frInfo->dispWidth * 4 * objRects[i].top + objRects[i].left * 4, objRects[i].right - objRects[i].left, objRects[i].bottom - objRects[i].top, frInfo->dispWidth * 4, 0xffff0000);
				i++;
			}
			NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"ImageCapture"));
			imgList->AddImage(simg, 0);

			IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"People_");
			dt.SetCurrTime();
			sptr = dt.ToString(sptr, "yyyyMMdd_HHmmssfff");
			sptr = Text::StrConcat(sptr, (const UTF8Char*)".jpg");
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			exporter->ExportFile(fs, sbuff, imgList, 0);
			DEL_CLASS(fs);

			DEL_CLASS(imgList);
		}		
	}
	lastCnt = thisCnt;
}

Media::IVideoCapture *OpenCapture(OSInt defIndex)
{
	Media::IVideoCapture *capture = 0;
	Media::VideoCaptureMgr *videoCap;
	Data::ArrayList<Media::VideoCaptureMgr::DeviceInfo*> devList;
	OSInt i;
	OSInt j;
	Media::VideoCaptureMgr::DeviceInfo *dev;
	NEW_CLASS(videoCap, Media::VideoCaptureMgr());
	videoCap->GetDeviceList(&devList);
	if (devList.GetCount())
	{
		Media::IVideoCapture::VideoFormat *formats;
		formats = MemAlloc(Media::IVideoCapture::VideoFormat, 512);
		i = 0;
		j = 512;
		while (i < j)
		{
			formats[i].info.Init();
			formats[i].info.Clear();
			formats[i].frameRateDenorm = 0;
			formats[i].frameRateNorm = 0;
			i++;
		}
		dev = devList.GetItem(defIndex);
		if (dev)
		{
			capture = videoCap->CreateDevice(dev->devType, dev->devId);
			if (capture->GetSupportedFormats(formats, 512) <= 0)
			{
				DEL_CLASS(capture);
				capture = 0;
			}
		}
		if (capture == 0)
		{
			i = 0;
			j = devList.GetCount();
			while (i < j)
			{
				if (defIndex != i)
				{
					dev = devList.GetItem(i);
					capture = videoCap->CreateDevice(dev->devType, dev->devId);
					if (capture->GetSupportedFormats(formats, 512) > 0)
					{
						break;
					}
					DEL_CLASS(capture);
					capture = 0;
				}
				i++;
			}
		}

		i = 0;
		j = 512;
		while (i < j)
		{
			formats[i].info.Deinit();
			i++;
		}
		MemFree(formats);
	}
	videoCap->FreeDeviceList(&devList);
	DEL_CLASS(videoCap);
	return capture;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Media::IVideoCapture *capture;
	IO::ConsoleWriter *console;
	OSInt defIndex;
	lastCnt = 0;
	OSInt frameSkip;

	Media::Decoder::FFMPEGDecoder::Enable();

	NEW_CLASS(console, IO::ConsoleWriter());
	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		defIndex = Text::StrToOSInt(argv[1]);
	}
	else
	{
		defIndex = 0;
	}

	rangeLeft = 0;
	rangeTop = 0;
	rangeRight = 0;
	rangeBottom = 0;
	preferedFormat = 0;
	preferedWidth = 0;
	preferedHeight = 0;
	frameSkip = 0;
	csConv = 0;
	exporter = 0;

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg)
	{
		const UTF8Char *csptr;
		if ((csptr = cfg->GetValue((const UTF8Char*)"FrameSkip")) != 0)
		{
			frameSkip = Text::StrToOSInt(csptr);
		}
		if ((csptr = cfg->GetValue((const UTF8Char*)"RangeLeft")) != 0)
		{
			rangeLeft = Text::StrToOSInt(csptr);
		}
		if ((csptr = cfg->GetValue((const UTF8Char*)"RangeTop")) != 0)
		{
			rangeTop = Text::StrToOSInt(csptr);
		}
		if ((csptr = cfg->GetValue((const UTF8Char*)"RangeRight")) != 0)
		{
			rangeRight = Text::StrToOSInt(csptr);
		}
		if ((csptr = cfg->GetValue((const UTF8Char*)"RangeBottom")) != 0)
		{
			rangeBottom = Text::StrToOSInt(csptr);
		}
		if ((csptr = cfg->GetValue((const UTF8Char*)"PreferedFormat")) != 0)
		{
			if (Text::StrCharCnt(csptr) == 4)
			preferedFormat = *(Int32*)csptr;
		}
		if ((csptr = cfg->GetValue((const UTF8Char*)"PreferedWidth")) != 0)
		{
			preferedWidth = Text::StrToOSInt(csptr);
		}
		if ((csptr = cfg->GetValue((const UTF8Char*)"PreferedHeight")) != 0)
		{
			preferedHeight = Text::StrToOSInt(csptr);
		}
		DEL_CLASS(cfg);
	}

	console->WriteLine((const UTF8Char*)"Starting OpenCVPeopleCounting");
	capture = OpenCapture(defIndex);
	if (capture)
	{
		Text::StringBuilderUTF8 sb;
		capture->GetInfo(&sb);
		console->WriteLine(sb.ToString());

		Media::OpenCV::OCVObjectDetector *objDetect;
		NEW_CLASS(objDetect, Media::OpenCV::OCVObjectDetector((const UTF8Char*)"haarcascades", (const UTF8Char*)"haarcascade_frontalface_alt.xml"));
		if (objDetect->IsError())
		{
			console->WriteLine((const UTF8Char*)"Error in initializing object detector");
		}
		else
		{
			Media::OpenCV::OCVFrameFeeder *feeder;
			NEW_CLASS(feeder, Media::OpenCV::OCVFrameFeeder(objDetect, capture));
			feeder->SetFrameSkip(frameSkip);
			feeder->SetPreferedFormat(preferedFormat, preferedWidth, preferedHeight);
			objDetect->HandleDetectResult(OnDetectResult, console);
			if (!feeder->Start())
			{
				console->WriteLine((const UTF8Char*)"Error in starting feeder");
			}
			else
			{
				console->WriteLine((const UTF8Char*)"Inited");
				progCtrl->WaitForExit(progCtrl);
				console->WriteLine((const UTF8Char*)"Exiting");
			}
			DEL_CLASS(feeder);
		}
		DEL_CLASS(objDetect);
		DEL_CLASS(capture);
	}
	else
	{
		console->WriteLine((const UTF8Char*)"Error in opening video capture");
	}
	SDEL_CLASS(csConv);
	SDEL_CLASS(exporter);
	
	DEL_CLASS(console);
	return 0;
}
