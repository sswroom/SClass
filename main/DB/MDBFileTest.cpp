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

Int32 MyMain(Core::IProgControl *progCtrl)
{
	const UTF8Char *fileName = (const UTF8Char*)"E:\\myworks\\0_req\\20120925 Macau Layer\\GeoDatabase\\Basemap.mdb";
	const UTF8Char *destPath = (const UTF8Char*)"E:\\myworks\\0_req\\20120925 Macau Layer\\GeoDatabase\\";
	IO::LogTool *log;
	IO::ConsoleWriter *console;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(log, IO::LogTool());

	Parser::FullParserList *parsers;
	IO::StmData::FileData *fd;
	IO::ParserType pt;
	IO::ParsedObject *pobj;
	NEW_CLASS(parsers, Parser::FullParserList());
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	pobj = parsers->ParseFile(fd, &pt);
	if (pobj)
	{
		UOSInt i;
		UOSInt j;
		Exporter::SHPExporter *shpExp;
		IO::FileStream *fs;

		NEW_CLASS(shpExp, Exporter::SHPExporter());
		shpExp->SetCodePage(65001);

		Map::MapLayerCollection *lyrColl = (Map::MapLayerCollection*)pobj;
		Map::IMapDrawLayer *lyr;
		j = lyrColl->GetCount();
		i = 0;
		while (i < j)
		{
			lyr = lyrColl->GetItem(i);
			sptr = lyr->GetName()->ConcatTo(sbuff);
			sptr = &sbuff[Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR) + 1];
			Text::StrConcatC(Text::StrConcat(Text::StrConcat(sbuff2, destPath), sptr), UTF8STRC(".shp"));

			NEW_CLASS(fs, IO::FileStream(sbuff2, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			shpExp->ExportFile(fs, sbuff2, lyr, 0);
			DEL_CLASS(fs);
			i++;
		}

		DEL_CLASS(shpExp);

		DEL_CLASS(pobj);
	}
	DEL_CLASS(fd);
	DEL_CLASS(parsers);

	DEL_CLASS(log);
	DEL_CLASS(console);
	return 0;
}
