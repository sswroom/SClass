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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UOSInt sWidth = 1920;
	UOSInt sHeight = 1080;
	UOSInt dWidth = 3840;
	UOSInt dHeight = 2160;
	Media::StaticImage *simg;
	Media::StaticImage *simg2;
	Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
	IO::ConsoleWriter console;
	Media::ImageGen::RingsImageGen imgGen;
	Media::IImgResizer *resizer;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Double t0;
	Double t1;
	Double t2;
//	NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srgb, &srgb, 0, Media::AT_NO_ALPHA));
	NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer32_32());
//	NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer64_64());
//	NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));

	Manage::HiResClock clk;
	resizer->SetTargetSize(Math::Size2D<UOSInt>(dWidth, dHeight));
	resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);

	simg = (Media::StaticImage*)imgGen.GenerateImage(srgb, Math::Size2D<UOSInt>(sWidth, sHeight));
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
		Media::ImageList *imgList;
		NEW_CLASS(imgList, Media::ImageList(CSTR("Test.tif")));
		imgList->AddImage(simg2, 0);
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("NearestNeighbourTest.tif"));
		{
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			exporter.ExportFile(fs, CSTRP(sbuff, sptr), imgList, 0);
		}
		DEL_CLASS(simg2);
	}
	DEL_CLASS(simg);
	DEL_CLASS(resizer);
	sb.AppendC(UTF8STRC("t0 = "));
	sb.AppendDouble(t0);
	sb.AppendC(UTF8STRC(", t1 = "));
	sb.AppendDouble(t1);
	sb.AppendC(UTF8STRC(", t2 = "));
	sb.AppendDouble(t2);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	return 0;
}
