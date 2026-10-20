#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "DB/DBTool.h"
#include "DB/MDBFile.h"
#include "Exporter/SHPExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/MapLayerCollection.h"
#include "Parser/FullParserList.h"
#include "Text/MyString.h"
#include "Text/StringTool.h"

Int32 Test1()
{
	Text::CStringNN fileName = CSTR("E:\\myworks\\0_req\\20120925 Macau Layer\\GeoDatabase\\Basemap.mdb");
	UnsafeArray<const UTF8Char> destPath = U8STR("E:\\myworks\\0_req\\20120925 Macau Layer\\GeoDatabase\\");
	NN<IO::LogTool> log;
	NN<IO::ConsoleWriter> console;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;

	NEW_CLASSNN(console, IO::ConsoleWriter());
	NEW_CLASSNN(log, IO::LogTool());

	NN<Parser::FullParserList> parsers;
	NN<IO::ParsedObject> pobj;
	NEW_CLASSNN(parsers, Parser::FullParserList());
	IO::StmData::FileData fd(fileName, false);
	if (parsers->ParseFile(fd).SetTo(pobj))
	{
		UIntOS i;
		UIntOS j;
		Exporter::SHPExporter shpExp;
		shpExp.SetCodePage(65001);

		NN<Map::MapLayerCollection> lyrColl = NN<Map::MapLayerCollection>::ConvertFrom(pobj);
		NN<Map::MapDrawLayer> lyr;
		j = lyrColl->GetCount();
		i = 0;
		while (i < j)
		{
			if (lyrColl->GetItem(i).SetTo(lyr))
			{
				sptr = lyr->GetName()->ConcatTo(sbuff);
				sptr = &sbuff[Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), IO::Path::PATH_SEPERATOR) + 1];
				sptr2 = Text::StrConcatC(Text::StrConcat(Text::StrConcat(sbuff2, destPath), sptr), UTF8STRC(".shp"));

				IO::FileStream fs(CSTRP(sbuff2, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				shpExp.ExportFile(fs, CSTRP(sbuff2, sptr2), lyr, nullptr);
			}
			i++;
		}

		pobj.Delete();
	}
	parsers.Delete();

	log.Delete();
	console.Delete();
	return 0;
}

Int32 Test2()
{
	Text::CStringNN fileName = CSTR("/home/sswroom/ProgsHome/RECORD/AudioCD.mdb");
	IO::StmData::FileData fd(fileName, false);
	DB::MDBFile mdb(fd);
	Data::ArrayListStringNN tableNames;
	mdb.QueryTableNames(nullptr, tableNames);
	UIntOS i = 0;
	UIntOS j = tableNames.GetCount();
	printf("Found %u tables\r\n", (UInt32)j);
	while (i < j)
	{
		printf("%s\r\n", tableNames.GetItemNoCheck(i)->v.Ptr());
		i++;
	}
	printf("--------------------\r\n");
	NN<DB::DBReader> r;
	UTF8Char colName[256];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbHex;
	if (mdb.QueryTableData(nullptr, tableNames.GetItemNoCheck(0)->ToCString(), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		if (r->ReadNext())
		{
			i = 0;
			j = r->ColCount();
			while (i < j)
			{
				sptr = r->GetName(i, colName).Or(colName);
				sb.ClearStr();
				r->GetStr(i, sb);
				if (Text::StringTool::IsTextUTF8(sb.ToByteArray()))
				{
					printf("%s: %s\r\n", colName, sb.v.Ptr());
				}
				else
				{
					sbHex.ClearStr();
					sbHex.AppendHexBuff(sb.ToByteArray(), ' ', Text::LineBreakType::None);
					printf("%s: (Hex) %s\r\n", colName, sbHex.v.Ptr());
				}
				i++;
			}
		}
		else
		{
			printf("Error in reading table %s data\r\n", tableNames.GetItemNoCheck(0)->v.Ptr());
		}
		mdb.CloseReader(r);
	}
	tableNames.FreeAll();
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt32 id = 2;
	switch (id)
	{
	case 1:
		return Test1();
	case 2:
		return Test2();
	default:
		return 0;
	}
}
