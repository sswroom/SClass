#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Manage/HiResClock.h"
#include "Media/ImageList.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Parser/FileParser/JPGParser.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN fileName = CSTR("");
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	IO::StmData::FileData fd(fileName, false);
	UOSInt fileLen;
	if ((fileLen = (UOSInt)fd.GetDataSize()) > 0)
	{
		UInt8 *fileBuff = MemAlloc(UInt8, fileLen);
		if (fd.GetRealData(0, fileLen, Data::ByteArray(fileBuff, fileLen)) != fileLen)
		{
			console.WriteLine(CSTR("Error in loading jpg file"));
		}
		else
		{
			NN<IO::ParsedObject> pobj;
			Manage::HiResClock clk;
			{
				IO::StmData::MemoryDataRef md(Data::ByteArray(fileBuff, fileLen));
				Parser::FileParser::GUIImgParser parser;
				clk.Start();
				if (parser.ParseFileHdr(md, 0, IO::ParserType::ImageList, Data::ByteArrayR(fileBuff, fileLen)).SetTo(pobj))
				{
					Double t = clk.GetTimeDiff();
					sb.ClearStr();
					sb.Append(CSTR("GUIImgParser: t = "));
					sb.AppendDouble(t);
					console.WriteLine(sb.ToCString());
					pobj.Delete();
				}
				else
				{
					console.WriteLine(CSTR("GUIImgParser: Parse Error"));
				}
			}
			{
				IO::StmData::MemoryDataRef md(Data::ByteArray(fileBuff, fileLen));
				Parser::FileParser::JPGParser parser;
				clk.Start();
				if (parser.ParseFileHdr(md, 0, IO::ParserType::ImageList, Data::ByteArrayR(fileBuff, fileLen)).SetTo(pobj))
				{
					Double t = clk.GetTimeDiff();
					sb.ClearStr();
					sb.Append(CSTR("JPGParser: t = "));
					sb.AppendDouble(t);
					console.WriteLine(sb.ToCString());
					pobj.Delete();
				}
				else
				{
					console.WriteLine(CSTR("JPGParser: Parse Error"));
				}
			}
		}
		MemFree(fileBuff);
	}
	else
	{
		console.WriteLine(CSTR("Error in opening jpg file"));
	}
	return 0;
}
