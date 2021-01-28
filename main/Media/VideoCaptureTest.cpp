#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
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
	IO::FileStream *fs;
	Int32 fnum = frameCnt;
	if (fnum == 1)
	{
		Exporter::TIFFExporter exporter;
		Media::StaticImage *simg;
		Media::ImageList *imgList;
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		const UTF8Char *fileName = (const UTF8Char*)"Capture.tif";
		NEW_CLASS(simg, Media::StaticImage(info.dispWidth, info.dispHeight, 0, 32, Media::PF_B8G8R8A8, 0, &color, Media::ColorProfile::YUVT_BT601, info.atype, info.ycOfst));
		NEW_CLASS(imgList, Media::ImageList(fileName));
		imgList->AddImage(simg, 0);

		if (converter)
		{
			converter->ConvertV2(imgData, simg->data, info.dispWidth, info.dispHeight, info.storeWidth, info.storeHeight, info.storeWidth * 4, frameType, info.ycOfst);
		}
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		exporter.ExportFile(fs, fileName, imgList, 0);
		DEL_CLASS(fs);
		DEL_CLASS(imgList);
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
	OSInt cnt;
	OSInt i;
	OSInt j;
	OSInt devNo;
	Bool succ;
	Text::StringBuilderUTF8 sb;
	Bool isRunning;
	UTF8Char sbuff[512];
	UInt32 widthLimit = 3840;
	UInt8 tmpBuff[4];
	UInt32 prefFmt = 0;

	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		widthLimit = Text::StrToInt32(argv[1]);
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
			prefFmt = *(Int32*)tmpBuff;
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
		sb.Append((const UTF8Char*)"Device Count = ");
		sb.AppendOSInt(cnt);
		console->WriteLine(sb.ToString());
		i = 0;
		while (i < cnt)
		{
			devInfo = devList.GetItem(i);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Type = ");
			sb.AppendI32(devInfo->devType);
			sb.Append((const UTF8Char*)", Index ");
			sb.AppendOSInt(devInfo->devId);
			sb.Append((const UTF8Char*)", Name = ");
			sb.Append(devInfo->devName);
			console->WriteLine(sb.ToString());

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
				Int32 maxBpp = 0;
				OSInt maxWidth = 0;
				OSInt maxHeight = 0;
				OSInt maxSize = 0;
				Int32 maxRateNumer = 0;
				Int32 maxRateDenom = 0;
				OSInt thisSize;
				
				Int32 frameRateNorm;
				Int32 frameRateDenorm;
				UOSInt frameMaxSize;
				Media::IVideoCapture::VideoFormat *formats;
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);

				console->WriteLine();
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Capture using ");
				capture->GetSourceName(sbuff);
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());

				console->WriteLine((const UTF8Char*)"Supported Formats:");
				formats = MemAlloc(Media::IVideoCapture::VideoFormat, 128);
				i = 0;
				j = 128;
				while (i < j)
				{
					formats[i].info.Init();
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
					sb.Append((const UTF8Char*)"Format: ");
					if (formats[i].info.fourcc == 0)
					{
						sb.Append((const UTF8Char*)"RGB ");
					}
					else
					{
						sb.AppendC((const UTF8Char*)&formats[i].info.fourcc, 4);
					}
					sb.Append((const UTF8Char*)", Size = ");
					sb.AppendOSInt(formats[i].info.dispWidth);
					sb.Append((const UTF8Char*)" x ");
					sb.AppendOSInt(formats[i].info.dispHeight);
					sb.Append((const UTF8Char*)", bpp = ");
					sb.AppendI32(formats[i].info.storeBPP);
					sb.Append((const UTF8Char*)", rate = ");
					sb.AppendI32(formats[i].frameRateNorm);
					sb.Append((const UTF8Char*)" / ");
					sb.AppendI32(formats[i].frameRateDenorm);
					sb.Append((const UTF8Char*)" (");
					Text::SBAppendF64(&sb, formats[i].frameRateNorm / (Double)formats[i].frameRateDenorm);
					sb.Append((const UTF8Char*)")");
					console->WriteLine(sb.ToString());
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
					sb.Append((const UTF8Char*)"Setting Format: ");
					if (maxFmt == 0)
					{
						sb.Append((const UTF8Char*)"RGB ");
					}
					else
					{
						sb.AppendC((const UTF8Char*)&maxFmt, 4);
					}
					sb.Append((const UTF8Char*)", Size = ");
					sb.AppendOSInt(maxWidth);
					sb.Append((const UTF8Char*)" x ");
					sb.AppendOSInt(maxHeight);
					sb.Append((const UTF8Char*)", bpp = ");
					sb.AppendI32(maxBpp);
					console->WriteLine(sb.ToString());
					capture->SetPreferSize(maxWidth, maxHeight, maxFmt, maxBpp, maxRateNumer, maxRateDenom);
				}

				i = 0;
				j = 128;
				while (i < j)
				{
					formats[i].info.Deinit();
					i++;
				}
				MemFree(formats);


				console->WriteLine();

				if (capture->GetVideoInfo(&info, &frameRateNorm, &frameRateDenorm, &frameMaxSize))
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Using Format: ");
					if (info.fourcc == 0)
					{
						sb.Append((const UTF8Char*)"RGB ");
					}
					else
					{
						sb.AppendC((const UTF8Char*)&info.fourcc, 4);
					}
					sb.Append((const UTF8Char*)", Size = ");
					sb.AppendOSInt(info.dispWidth);
					sb.Append((const UTF8Char*)" x ");
					sb.AppendOSInt(info.dispHeight);
					sb.Append((const UTF8Char*)", bpp = ");
					sb.AppendI32(info.storeBPP);
					sb.Append((const UTF8Char*)", rate = ");
					sb.AppendI32(frameRateNorm);
					sb.Append((const UTF8Char*)" / ");
					sb.AppendI32(frameRateDenorm);
					sb.Append((const UTF8Char*)" (");
					Text::SBAppendF64(&sb, frameRateNorm / (Double)frameRateDenorm);
					sb.Append((const UTF8Char*)")");
					console->WriteLine(sb.ToString());
				}
				else
				{
					console->WriteLine((const UTF8Char*)"Error in getting format");
				}
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Frame Delay = ");
				sb.AppendI32(frameCnt);
				console->WriteLine(sb.ToString());
				if (colorSess)
				{
					console->WriteLine((const UTF8Char*)"ColorSess is null");
				}
				converter = Media::CS::CSConverter::NewConverter(info.fourcc, info.storeBPP, info.pf, info.color, 0, 32, Media::PF_B8G8R8A8, &color, info.yuvType, colorSess);
				if (converter == 0)
				{
					console->WriteLine((const UTF8Char*)"Converter is null");
				}
				else if (converter)
				{
					console->WriteLine((const UTF8Char*)"Converter is found");
				}
				isRunning = capture->Init(CaptureTest, FrameChangeHdlr, 0);
				if (!isRunning)
				{
					console->WriteLine((const UTF8Char*)"Error in initializing capture");
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
						sb.Append((const UTF8Char*)"Capture device stopped, count = ");
						sb.AppendI32(frameCnt);
						console->WriteLine(sb.ToString());
					}
					else
					{
						console->WriteLine((const UTF8Char*)"File captured");
						succ = true;
						capture->Stop();
					}				
				}
				else
				{
					console->WriteLine((const UTF8Char*)"Error in starting capture");
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
		console->WriteLine((const UTF8Char*)"No capture device found");
	}
	mgr->FreeDeviceList(&devList);
	colorMgr->DeleteSess(colorSess);

	DEL_CLASS(mgr);
	DEL_CLASS(colorMgr);
	DEL_CLASS(console);

	return 0;
}
