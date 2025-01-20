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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UOSInt sWidth = 1920;
	UOSInt sHeight = 1080;
	UOSInt dWidth = 3840;
	UOSInt dHeight = 2160;
	NN<Media::StaticImage> simg;
	Optional<Media::StaticImage> simg2;
	Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
	IO::ConsoleWriter console;
	Media::ImageGen::RingsImageGen imgGen;
	Media::ImageResizer *resizer;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Double t0;
	Double t1;
	Double t2;
//	NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srgb, &srgb, 0, Media::AT_NO_ALPHA));
	NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer32_32());
//	NEW_CLASS(resizer, Media::Resizer::NearestNeighbourResizer64_64());
//	NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));

	Manage::HiResClock clk;
	resizer->SetTargetSize(Math::Size2D<UOSInt>(dWidth, dHeight));
	resizer->SetResizeAspectRatio(Media::ImageResizer::RAR_IGNOREAR);

	if (Optional<Media::StaticImage>::ConvertFrom(imgGen.GenerateImage(srgb, Math::Size2D<UOSInt>(sWidth, sHeight))).SetTo(simg))
	{
		t0 = clk.GetTimeDiff();
		clk.Start();
		simg->To32bpp();
	//	simg->To64bpp();
		t1 = clk.GetTimeDiff();
		simg2 = resizer->ProcessToNew(simg);
		simg2.Delete();
		clk.Start();
		simg2 = resizer->ProcessToNew(simg);
		t2 = clk.GetTimeDiff();
		simg.Delete();
		if (simg2.SetTo(simg))
		{
			Exporter::TIFFExporter exporter;
			NN<Media::ImageList> imgList;
			NEW_CLASSNN(imgList, Media::ImageList(CSTR("Test.tif")));
			imgList->AddImage(simg, 0);
			sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("NearestNeighbourTest.tif"));
			{
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				exporter.ExportFile(fs, CSTRP(sbuff, sptr), imgList, 0);
			}
			imgList.Delete();
		}
	}
	else
	{
		t0 = -1;
		t1 = -1;
		t2 = -1;
	}
	DEL_CLASS(resizer);
	sb.AppendC(UTF8STRC("t0 = "));
	sb.AppendDouble(t0);
	sb.AppendC(UTF8STRC(", t1 = "));
	sb.AppendDouble(t1);
	sb.AppendC(UTF8STRC(", t2 = "));
	sb.AppendDouble(t2);
	console.WriteLine(sb.ToCString());
	return 0;
}
