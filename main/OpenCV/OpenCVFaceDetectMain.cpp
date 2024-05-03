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

UOSInt lastCnt;
OSInt rangeLeft;
OSInt rangeTop;
OSInt rangeRight;
OSInt rangeBottom;
UInt32 preferedFormat;
UOSInt preferedWidth;
UOSInt preferedHeight;
Media::CS::CSConverter *csConv;
Exporter::GUIJPGExporter *exporter;

void __stdcall OnDetectResult(void *userObj, UOSInt objCnt, const Media::OpenCV::OCVObjectDetector::ObjectRect *objRects, Media::FrameInfo *frInfo, UInt8 **imgData)
{
	IO::ConsoleWriter *console = (IO::ConsoleWriter*)userObj;
	UOSInt thisCnt = objCnt;
	if (rangeLeft < rangeRight && rangeTop < rangeBottom)
	{
		UOSInt i = 0;
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
		sb.AppendC(UTF8STRC("People detected, cnt = "));
		sb.AppendUOSInt(thisCnt);
		console->WriteLine(sb.ToCString());

		Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
		if (csConv == 0)
		{
			csConv = Media::CS::CSConverter::NewConverter(frInfo->fourcc, frInfo->storeBPP, frInfo->pf, frInfo->color, 0, 32, Media::PF_B8G8R8A8, srgb, frInfo->yuvType, 0);
		}
		if (exporter == 0)
		{
			NEW_CLASS(exporter, Exporter::GUIJPGExporter());
		}
		if (csConv)
		{
			Media::StaticImage *simg;
			Data::DateTime dt;
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			NEW_CLASS(simg, Media::StaticImage(frInfo->dispSize, 0, 32, Media::PF_B8G8R8A8, 0, srgb, frInfo->yuvType, Media::AT_NO_ALPHA, frInfo->ycOfst));
			csConv->ConvertV2(imgData, simg->data, frInfo->dispSize.x, frInfo->dispSize.y, frInfo->storeSize.x, frInfo->storeSize.y, (OSInt)frInfo->dispSize.x * 4, Media::FT_NON_INTERLACE, frInfo->ycOfst);
			UOSInt i = 0;
			while (i < objCnt)
			{
				ImageUtil_DrawRectNA32(simg->data + (OSInt)frInfo->dispSize.x * 4 * objRects[i].top + objRects[i].left * 4, (UOSInt)(objRects[i].right - objRects[i].left), (UOSInt)(objRects[i].bottom - objRects[i].top), frInfo->dispSize.x * 4, 0xffff0000);
				i++;
			}
			Media::ImageList imgList(CSTR("ImageCapture"));
			imgList.AddImage(simg, 0);

			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("People_"));
			dt.SetCurrTime();
			sptr = dt.ToString(sptr, "yyyyMMdd_HHmmssfff");
			sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			exporter->ExportFile(fs, CSTRP(sbuff, sptr), imgList, 0);
		}		
	}
	lastCnt = thisCnt;
}

Optional<Media::IVideoCapture> OpenCapture(UOSInt defIndex)
{
	Optional<Media::IVideoCapture> capture = 0;
	NN<Media::IVideoCapture> nncapture;
	Media::VideoCaptureMgr *videoCap;
	Data::ArrayListNN<Media::VideoCaptureMgr::DeviceInfo> devList;
	UOSInt i;
	UOSInt j;
	NN<Media::VideoCaptureMgr::DeviceInfo> dev;
	NEW_CLASS(videoCap, Media::VideoCaptureMgr());
	videoCap->GetDeviceList(devList);
	if (devList.GetCount())
	{
		Media::IVideoCapture::VideoFormat *formats;
		formats = MemAlloc(Media::IVideoCapture::VideoFormat, 512);
		i = 0;
		j = 512;
		while (i < j)
		{
			formats[i].info.InitFrameInfo();
			formats[i].info.Clear();
			formats[i].frameRateDenorm = 0;
			formats[i].frameRateNorm = 0;
			i++;
		}
		if (devList.GetItem(defIndex).SetTo(dev))
		{
			capture = videoCap->CreateDevice(dev->devType, dev->devId);
			if (!capture.SetTo(nncapture) || nncapture->GetSupportedFormats(formats, 512) <= 0)
			{
				capture.Delete();
			}
		}
		if (capture.IsNull())
		{
			i = 0;
			j = devList.GetCount();
			while (i < j)
			{
				if (defIndex != i)
				{
					dev = devList.GetItemNoCheck(i);
					capture = videoCap->CreateDevice(dev->devType, dev->devId);
					if (capture.SetTo(nncapture) && nncapture->GetSupportedFormats(formats, 512) > 0)
					{
						break;
					}
					capture.Delete();
				}
				i++;
			}
		}

		i = 0;
		j = 512;
		while (i < j)
		{
			formats[i].info.DeinitFrameInfo();
			i++;
		}
		MemFree(formats);
	}
	videoCap->FreeDeviceList(devList);
	DEL_CLASS(videoCap);
	return capture;
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<Media::IVideoCapture> capture;
	IO::ConsoleWriter *console;
	UOSInt defIndex;
	lastCnt = 0;
	OSInt frameSkip;

	Media::Decoder::FFMPEGDecoder::Enable();

	NEW_CLASS(console, IO::ConsoleWriter());
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		defIndex = Text::StrToUOSInt(argv[1]);
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
		NN<Text::String> s;
		if (cfg->GetValue(CSTR("FrameSkip")).SetTo(s))
		{
			frameSkip = s->ToOSInt();
		}
		if (cfg->GetValue(CSTR("RangeLeft")).SetTo(s))
		{
			rangeLeft = s->ToOSInt();
		}
		if (cfg->GetValue(CSTR("RangeTop")).SetTo(s))
		{
			rangeTop = s->ToOSInt();
		}
		if (cfg->GetValue(CSTR("RangeRight")).SetTo(s))
		{
			rangeRight = s->ToOSInt();
		}
		if (cfg->GetValue(CSTR("RangeBottom")).SetTo(s))
		{
			rangeBottom = s->ToOSInt();
		}
		if (cfg->GetValue(CSTR("PreferedFormat")).SetTo(s))
		{
			if (s->leng == 4)
			preferedFormat = *(UInt32*)s->v;
		}
		if (cfg->GetValue(CSTR("PreferedWidth")).SetTo(s))
		{
			preferedWidth = s->ToUOSInt();
		}
		if (cfg->GetValue(CSTR("PreferedHeight")).SetTo(s))
		{
			preferedHeight = s->ToUOSInt();
		}
		DEL_CLASS(cfg);
	}

	console->WriteLine(CSTR("Starting OpenCVPeopleCounting"));
	if (OpenCapture(defIndex).SetTo(capture))
	{
		Text::StringBuilderUTF8 sb;
		capture->GetInfo(sb);
		console->WriteLine(sb.ToCString());

		Media::OpenCV::OCVObjectDetector *objDetect;
		NEW_CLASS(objDetect, Media::OpenCV::OCVObjectDetector((const UTF8Char*)"haarcascades", (const UTF8Char*)"haarcascade_frontalface_alt.xml"));
		if (objDetect->IsError())
		{
			console->WriteLine(CSTR("Error in initializing object detector"));
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
				console->WriteLine(CSTR("Error in starting feeder"));
			}
			else
			{
				console->WriteLine(CSTR("Inited"));
				progCtrl->WaitForExit(progCtrl);
				console->WriteLine(CSTR("Exiting"));
			}
			DEL_CLASS(feeder);
		}
		DEL_CLASS(objDetect);
		capture.Delete();
	}
	else
	{
		console->WriteLine(CSTR("Error in opening video capture"));
	}
	SDEL_CLASS(csConv);
	SDEL_CLASS(exporter);
	
	DEL_CLASS(console);
	return 0;
}
