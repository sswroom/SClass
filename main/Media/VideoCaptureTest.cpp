#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "Exporter/TIFFExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Media/VideoCaptureMgr.h"
#include "Media/CS/CSConverter.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Media::FrameInfo info;
static Media::CS::CSConverter *converter;
Int32 frameCnt;
IO::ConsoleWriter *console;

void __stdcall FrameChangeHdlr(Media::IVideoSource::FrameChange frChg, void *userData)
{
}

void __stdcall CaptureTest(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Int32 fnum = frameCnt;
	if (fnum == 1)
	{
		Exporter::TIFFExporter exporter;
		Media::StaticImage *simg;
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		Text::CString fileName = CSTR("Capture.tif");
		NEW_CLASS(simg, Media::StaticImage(info.dispWidth, info.dispHeight, 0, 32, Media::PF_B8G8R8A8, 0, &color, Media::ColorProfile::YUVT_BT601, info.atype, info.ycOfst));
		Media::ImageList imgList(fileName);
		imgList.AddImage(simg, 0);

		if (converter)
		{
			converter->ConvertV2(imgData, simg->data, info.dispWidth, info.dispHeight, info.storeWidth, info.storeHeight, (OSInt)info.storeWidth * 4, frameType, info.ycOfst);
		}
		{
			IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			exporter.ExportFile(&fs, fileName, &imgList, 0);
		}
	}
	frameCnt--;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Media::VideoCaptureMgr *mgr;
	Media::ColorManager *colorMgr;
	Media::ColorManagerSess *colorSess;
	Data::ArrayList<Media::VideoCaptureMgr::DeviceInfo *> devList;
	Media::VideoCaptureMgr::DeviceInfo *devInfo;
	UOSInt cnt;
	UOSInt i;
	UOSInt j;
	UOSInt devNo;
	Bool succ;
	Text::StringBuilderUTF8 sb;
	Bool isRunning;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt32 widthLimit = 3840;
	UInt8 tmpBuff[4];
	UInt32 prefFmt = 0;

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		widthLimit = Text::StrToUInt32(argv[1]);
		if (widthLimit <= 0)
		{
			widthLimit = 3840;
		}
	}
	if (argc >= 3)
	{
		if (Text::StrCharCnt(argv[2]) == 4)
		{
			tmpBuff[0] = argv[2][0];
			tmpBuff[1] = argv[2][1];
			tmpBuff[2] = argv[2][2];
			tmpBuff[3] = argv[2][3];
			prefFmt = ReadNUInt32(tmpBuff);
		}
	}

	frameCnt = 150;
	converter = 0;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(colorMgr, Media::ColorManager());
	NEW_CLASS(mgr, Media::VideoCaptureMgr());

	colorSess = colorMgr->CreateSess(0);
	cnt = mgr->GetDeviceList(&devList);
	if (cnt > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Device Count = "));
		sb.AppendUOSInt(cnt);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		i = 0;
		while (i < cnt)
		{
			devInfo = devList.GetItem(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Type = "));
			sb.AppendI32(devInfo->devType);
			sb.AppendC(UTF8STRC(", Index "));
			sb.AppendUOSInt(devInfo->devId);
			sb.AppendC(UTF8STRC(", Name = "));
			sb.AppendSlow(devInfo->devName);
			console->WriteLineC(sb.ToString(), sb.GetLength());

			i++;
		}

		devNo = 0;
		while (devNo < cnt)
		{
			devInfo = devList.GetItem(devNo);
			Media::IVideoCapture *capture = mgr->CreateDevice(devInfo->devType, devInfo->devId);
			succ = false;
			
			if (capture)
			{
				UInt32 maxFmt = 0;
				UInt32 maxBpp = 0;
				UOSInt maxWidth = 0;
				UOSInt maxHeight = 0;
				UOSInt maxSize = 0;
				UInt32 maxRateNumer = 0;
				UInt32 maxRateDenom = 0;
				UOSInt thisSize;
				
				UInt32 frameRateNorm;
				UInt32 frameRateDenorm;
				UOSInt frameMaxSize;
				Media::IVideoCapture::VideoFormat *formats;
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);

				console->WriteLine();
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Capture using "));
				sptr = capture->GetSourceName(sbuff);
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());

				console->WriteLineC(UTF8STRC("Supported Formats:"));
				formats = MemAlloc(Media::IVideoCapture::VideoFormat, 128);
				i = 0;
				j = 128;
				while (i < j)
				{
					formats[i].info.InitFrameInfo();
					formats[i].info.Clear();
					formats[i].frameRateDenorm = 0;
					formats[i].frameRateNorm = 0;
					i++;
				}
				i = 0;
				j = capture->GetSupportedFormats(formats, 128);
				while (i < j)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Format: "));
					if (formats[i].info.fourcc == 0)
					{
						sb.AppendC(UTF8STRC("RGB "));
					}
					else
					{
						sb.AppendC((const UTF8Char*)&formats[i].info.fourcc, 4);
					}
					sb.AppendC(UTF8STRC(", Size = "));
					sb.AppendUOSInt(formats[i].info.dispWidth);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendUOSInt(formats[i].info.dispHeight);
					sb.AppendC(UTF8STRC(", bpp = "));
					sb.AppendU32(formats[i].info.storeBPP);
					sb.AppendC(UTF8STRC(", rate = "));
					sb.AppendU32(formats[i].frameRateNorm);
					sb.AppendC(UTF8STRC(" / "));
					sb.AppendU32(formats[i].frameRateDenorm);
					sb.AppendC(UTF8STRC(" ("));
					Text::SBAppendF64(&sb, formats[i].frameRateNorm / (Double)formats[i].frameRateDenorm);
					sb.AppendC(UTF8STRC(")"));
					console->WriteLineC(sb.ToString(), sb.GetLength());
					thisSize = formats[i].info.dispWidth * formats[i].info.dispHeight;
					if (formats[i].info.fourcc == *(UInt32*)"MJPG")
					{

					}
/*					else if (formats[i].info.fourcc == *(Int32*)"YUYV")
					{

					}*/
					else if (thisSize > maxSize && formats[i].info.dispWidth <= widthLimit)
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispWidth;
						maxHeight = formats[i].info.dispHeight;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
					else if (thisSize == maxSize && formats[i].info.fourcc == prefFmt)
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispWidth;
						maxHeight = formats[i].info.dispHeight;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}

					i++;
				}
				if (maxSize > 0)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Setting Format: "));
					if (maxFmt == 0)
					{
						sb.AppendC(UTF8STRC("RGB "));
					}
					else
					{
						sb.AppendC((const UTF8Char*)&maxFmt, 4);
					}
					sb.AppendC(UTF8STRC(", Size = "));
					sb.AppendUOSInt(maxWidth);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendUOSInt(maxHeight);
					sb.AppendC(UTF8STRC(", bpp = "));
					sb.AppendU32(maxBpp);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					capture->SetPreferSize(maxWidth, maxHeight, maxFmt, maxBpp, maxRateNumer, maxRateDenom);
				}

				i = 0;
				j = 128;
				while (i < j)
				{
					formats[i].info.DeinitFrameInfo();
					i++;
				}
				MemFree(formats);


				console->WriteLine();

				if (capture->GetVideoInfo(&info, &frameRateNorm, &frameRateDenorm, &frameMaxSize))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Using Format: "));
					if (info.fourcc == 0)
					{
						sb.AppendC(UTF8STRC("RGB "));
					}
					else
					{
						sb.AppendC((const UTF8Char*)&info.fourcc, 4);
					}
					sb.AppendC(UTF8STRC(", Size = "));
					sb.AppendUOSInt(info.dispWidth);
					sb.AppendC(UTF8STRC(" x "));
					sb.AppendUOSInt(info.dispHeight);
					sb.AppendC(UTF8STRC(", bpp = "));
					sb.AppendU32(info.storeBPP);
					sb.AppendC(UTF8STRC(", rate = "));
					sb.AppendU32(frameRateNorm);
					sb.AppendC(UTF8STRC(" / "));
					sb.AppendU32(frameRateDenorm);
					sb.AppendC(UTF8STRC(" ("));
					Text::SBAppendF64(&sb, frameRateNorm / (Double)frameRateDenorm);
					sb.AppendC(UTF8STRC(")"));
					console->WriteLineC(sb.ToString(), sb.GetLength());
				}
				else
				{
					console->WriteLineC(UTF8STRC("Error in getting format"));
				}
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Frame Delay = "));
				sb.AppendI32(frameCnt);
				console->WriteLineC(sb.ToString(), sb.GetLength());
				if (colorSess)
				{
					console->WriteLineC(UTF8STRC("ColorSess is null"));
				}
				converter = Media::CS::CSConverter::NewConverter(info.fourcc, info.storeBPP, info.pf, info.color, 0, 32, Media::PF_B8G8R8A8, &color, info.yuvType, colorSess);
				if (converter == 0)
				{
					console->WriteLineC(UTF8STRC("Converter is null"));
				}
				else if (converter)
				{
					console->WriteLineC(UTF8STRC("Converter is found"));
				}
				isRunning = capture->Init(CaptureTest, FrameChangeHdlr, 0);
				if (!isRunning)
				{
					console->WriteLineC(UTF8STRC("Error in initializing capture"));
				}
				else
				{
					isRunning = isRunning && capture->Start();
				}
				
				if (isRunning)
				{
					while (frameCnt > 0 && (isRunning = capture->IsRunning()))
					{
						Sync::Thread::Sleep(100);
					}
					if (frameCnt > 0)
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Capture device stopped, count = "));
						sb.AppendI32(frameCnt);
						console->WriteLineC(sb.ToString(), sb.GetLength());
					}
					else
					{
						console->WriteLineC(UTF8STRC("File captured"));
						succ = true;
						capture->Stop();
					}				
				}
				else
				{
					console->WriteLineC(UTF8STRC("Error in starting capture"));
				}

				SDEL_CLASS(converter);
				DEL_CLASS(capture);
			}

			if (succ)
			{
				break;
			}
			else
			{
				devNo++;
			}
			
		}
	}
	else
	{
		console->WriteLineC(UTF8STRC("No capture device found"));
	}
	mgr->FreeDeviceList(&devList);
	colorMgr->DeleteSess(colorSess);

	DEL_CLASS(mgr);
	DEL_CLASS(colorMgr);
	DEL_CLASS(console);

	return 0;
}
