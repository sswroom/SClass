#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "DB/DBTool.h"
#include "Exporter/SHPExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/MapLayerCollection.h"
#include "Parser/FullParserList.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN fileName = CSTR("E:\\myworks\\0_req\\20120925 Macau Layer\\GeoDatabase\\Basemap.mdb");
	const UTF8Char *destPath = (const UTF8Char*)"E:\\myworks\\0_req\\20120925 Macau Layer\\GeoDatabase\\";
	IO::LogTool *log;
	IO::ConsoleWriter *console;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(log, IO::LogTool());

	Parser::FullParserList *parsers;
	NN<IO::ParsedObject> pobj;
	NEW_CLASS(parsers, Parser::FullParserList());
	IO::StmData::FileData fd(fileName, false);
	if (parsers->ParseFile(fd).SetTo(pobj))
	{
		UOSInt i;
		UOSInt j;
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
				sptr = &sbuff[Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR) + 1];
				sptr2 = Text::StrConcatC(Text::StrConcat(Text::StrConcat(sbuff2, destPath), sptr), UTF8STRC(".shp"));

				IO::FileStream fs(CSTRP(sbuff2, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				shpExp.ExportFile(fs, CSTRP(sbuff2, sptr2), lyr, 0);
			}
			i++;
		}

		pobj.Delete();
	}
	DEL_CLASS(parsers);

	DEL_CLASS(log);
	DEL_CLASS(console);
	return 0;
}
