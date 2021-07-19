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
	const UTF8Char *srcFile;
	const UTF8Char *destFile;
	//UOSInt cmdCnt;
	Int32 ret = 1;
	//UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
/*	if (cmdCnt != 4)
	{
		console.WriteLine((const UTF8Char*)"Error in parameters, should be ImageResize [srcFile] [destFile] [size]");
		return 1;
	}*/

	Text::StringBuilderUTF8 sb;

/*	srcFile = argv[1];
	destFile = argv[2];
	if (!Text::StrToUInt32(argv[3], &pxSize))
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Error in parsing size: ");
		sb.Append(argv[3]);
		console.WriteLine(sb.ToString());
		return 1;
	}*/
	
	srcFile = (const UTF8Char*)"/home/sswroom/Progs/Temp/insp1624622649783.jpg";
	destFile = (const UTF8Char*)"/home/sswroom/Progs/photo2.jpg";
	pxSize = 200;

	Manage::HiResClock clk;
	Parser::FileParser::GUIImgParser parser;
	IO::StmData::FileData *fd;
	Media::ImageList *imgList;
	NEW_CLASS(fd, IO::StmData::FileData(srcFile, false));
	if (fd->GetDataSize() == 0)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Error in opening srcFile: ");
		sb.Append(srcFile);
		console.WriteLine(sb.ToString());
	}
	else if ((imgList = (Media::ImageList*)parser.ParseFile(fd, 0, IO::ParsedObject::PT_IMAGE_LIST_PARSER)) == 0)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Error in parsing srcFile: ");
		sb.Append(srcFile);
		console.WriteLine(sb.ToString());
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
			NEW_CLASS(imgList, Media::ImageList(destFile));
			imgList->AddImage(newImg, 0);
			NEW_CLASS(fs, IO::FileStream(destFile, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			if (exporter.ExportFile(fs, destFile, imgList, 0))
			{
				ret = 0;
			}
			else
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Error in saving destFile: ");
				sb.Append(destFile);
				console.WriteLine(sb.ToString());
			}
			DEL_CLASS(fs);
			DEL_CLASS(imgList);
		}
		else
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Error in resizing image: ");
			sb.Append(srcFile);
			console.WriteLine(sb.ToString());
		}
	}
	DEL_CLASS(fd);
	Double t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Time used = ");
	Text::SBAppendF64(&sb, t);
	sb.Append((const UTF8Char*)"s");
	console.WriteLine(sb.ToString());

	return ret;
}
