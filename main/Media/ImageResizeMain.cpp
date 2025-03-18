#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Media/ImageList.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UInt32 pxSize;
	Text::CStringNN srcFile;
	Text::CStringNN destFile;
	//UOSInt cmdCnt;
	Int32 ret = 1;
	//UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
/*	if (cmdCnt != 4)
	{
		console.WriteLine(CSTR("Error in parameters, should be ImageResize [srcFile] [destFile] [size]");
		return 1;
	}*/

	Text::StringBuilderUTF8 sb;

/*	srcFile = argv[1];
	destFile = argv[2];
	if (!Text::StrToUInt32(argv[3], &pxSize))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in parsing size: "));
		sb.Append(argv[3]);
		console.WriteLine(sb.ToCString());
		return 1;
	}*/
	
	srcFile = CSTR("/home/sswroom/Progs/Temp/src.jpg");
	destFile = CSTR("/home/sswroom/Progs/Temp/photo2.jpg");
	pxSize = 7680;

	Manage::HiResClock clk;
	Parser::FileParser::GUIImgParser parser;
	NN<Media::ImageList> imgList;
	{
		IO::StmData::FileData fd(srcFile, false);
		if (fd.GetDataSize() == 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Error in opening srcFile: "));
			sb.Append(srcFile);
			console.WriteLine(sb.ToCString());
		}
		else if (!Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(fd, 0, IO::ParserType::ImageList)).SetTo(imgList))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Error in parsing srcFile: "));
			sb.Append(srcFile);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			imgList->ToStaticImage(0);
			NN<Media::StaticImage> simg;
			Optional<Media::StaticImage> newImg;
			if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(simg))
			{
				Media::Resizer::LanczosResizer8_C8 resizer(4, 4, simg->info.color, simg->info.color, 0, simg->info.atype);
				resizer.SetTargetSize(Math::Size2D<UOSInt>(pxSize, pxSize));
				if (!resizer.IsSupported(simg->info))
				{
					simg->ToB8G8R8A8();
				}
				newImg = resizer.ProcessToNew(simg);
			}
			else
			{
				newImg = 0;
			}
			imgList.Delete();
			if (newImg.SetTo(simg))
			{
				Exporter::GUIJPGExporter exporter;
				NEW_CLASSNN(imgList, Media::ImageList(destFile));
				imgList->AddImage(simg, 0);
				IO::FileStream fs(destFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (exporter.ExportFile(fs, destFile, imgList, 0))
				{
					ret = 0;
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Error in saving destFile: "));
					sb.Append(destFile);
					console.WriteLine(sb.ToCString());
				}
				imgList.Delete();
			}
			else
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Error in resizing image: "));
				sb.Append(srcFile);
				console.WriteLine(sb.ToCString());
			}
		}
	}
	Double t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Time used = "));
	sb.AppendDouble(t);
	sb.AppendC(UTF8STRC("s"));
	console.WriteLine(sb.ToCString());

	return ret;
}
