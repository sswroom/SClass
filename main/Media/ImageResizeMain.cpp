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

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	UInt32 pxSize;
	Text::CString srcFile;
	Text::CString destFile;
	//UOSInt cmdCnt;
	Int32 ret = 1;
	//UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
/*	if (cmdCnt != 4)
	{
		console.WriteLineC(UTF8STRC("Error in parameters, should be ImageResize [srcFile] [destFile] [size]");
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
		console.WriteLineC(sb.ToString(), sb.GetLength());
		return 1;
	}*/
	
	srcFile = CSTR("/home/sswroom/Progs/Temp/src.jpg");
	destFile = CSTR("/home/sswroom/Progs/Temp/photo2.jpg");
	pxSize = 7680;

	Manage::HiResClock clk;
	Parser::FileParser::GUIImgParser parser;
	IO::StmData::FileData *fd;
	Media::ImageList *imgList;
	NEW_CLASS(fd, IO::StmData::FileData(srcFile.v, false));
	if (fd->GetDataSize() == 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in opening srcFile: "));
		sb.Append(srcFile);
		console.WriteLineC(sb.ToString(), sb.GetLength());
	}
	else if ((imgList = (Media::ImageList*)parser.ParseFile(fd, 0, IO::ParserType::ImageList)) == 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in parsing srcFile: "));
		sb.Append(srcFile);
		console.WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		UInt32 delay;
		imgList->ToStaticImage(0);
		Media::Image *img = imgList->GetImage(0, &delay);
		Media::Resizer::LanczosResizer8_C8 resizer(4, 4, img->info->color, img->info->color, 0, img->info->atype);
		resizer.SetTargetWidth(pxSize);
		resizer.SetTargetHeight(pxSize);
		Media::StaticImage *simg = (Media::StaticImage*)img;
		simg->To32bpp();
		Media::StaticImage *newImg = resizer.ProcessToNew(simg);
		DEL_CLASS(imgList);
		if (newImg)
		{
			Exporter::GUIJPGExporter exporter;
			IO::FileStream *fs;
			NEW_CLASS(imgList, Media::ImageList(destFile.v));
			imgList->AddImage(newImg, 0);
			NEW_CLASS(fs, IO::FileStream(destFile.v, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			if (exporter.ExportFile(fs, destFile.v, imgList, 0))
			{
				ret = 0;
			}
			else
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Error in saving destFile: "));
				sb.Append(destFile);
				console.WriteLineC(sb.ToString(), sb.GetLength());
			}
			DEL_CLASS(fs);
			DEL_CLASS(imgList);
		}
		else
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Error in resizing image: "));
			sb.Append(srcFile);
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
	}
	DEL_CLASS(fd);
	Double t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Time used = "));
	Text::SBAppendF64(&sb, t);
	sb.AppendC(UTF8STRC("s"));
	console.WriteLineC(sb.ToString(), sb.GetLength());

	return ret;
}
