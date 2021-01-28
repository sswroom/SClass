#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/TIFFExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Media/ImageGen/RingsImageGen.h"
#include "Media/Resizer/NearestNeighbourResizer32_32.h"
#include "Media/Resizer/NearestNeighbourResizer64_64.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt sWidth = 1920;
	OSInt sHeight = 1080;
	OSInt dWidth = 3840;
	OSInt dHeight = 2160;
	Media::StaticImage *simg;
	Media::StaticImage *simg2;
	Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
	IO::ConsoleWriter console;
	Media::ImageGen::RingsImageGen imgGen;
	Media::IImgResizer *resizer;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	Double t0;
	Double t1;
	Double t2;
//	NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srgb, &srgb, 0, Media::AT_NO_ALPHA));
	NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer32_32());
//	NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer64_64());
//	NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));

	Manage::HiResClock clk;
	resizer->SetTargetWidth(dWidth);
	resizer->SetTargetHeight(dHeight);
	resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);

	simg = (Media::StaticImage*)imgGen.GenerateImage(&srgb, sWidth, sHeight);
	t0 = clk.GetTimeDiff();
	clk.Start();
	simg->To32bpp();
//	simg->To64bpp();
	t1 = clk.GetTimeDiff();
	simg2 = resizer->ProcessToNew(simg);
	SDEL_CLASS(simg2);
	clk.Start();
	simg2 = resizer->ProcessToNew(simg);
	t2 = clk.GetTimeDiff();

	if (simg2)
	{
		Exporter::TIFFExporter exporter;
		IO::FileStream *fs;
		Media::ImageList *imgList;
		NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"Test.tif"));
		imgList->AddImage(simg2, 0);
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"NearestNeighbourTest.tif");
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		exporter.ExportFile(fs, sbuff, imgList, 0);
		DEL_CLASS(fs);
		DEL_CLASS(simg2);
	}
	DEL_CLASS(simg);
	DEL_CLASS(resizer);
	sb.Append((const UTF8Char*)"t0 = ");
	Text::SBAppendF64(&sb, t0);
	sb.Append((const UTF8Char*)", t1 = ");
	Text::SBAppendF64(&sb, t1);
	sb.Append((const UTF8Char*)", t2 = ");
	Text::SBAppendF64(&sb, t2);
	console.WriteLine(sb.ToString());
	return 0;
}
