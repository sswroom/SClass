#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/OruxMapExporter.h"
#include "IO/PackageFile.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Parser/FullParserList.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString fileName = CSTR("/media/sswroom/Extreme SSD/Map/HKTIle.spk");
	Text::CString destFile = CSTR("/media/sswroom/Extreme SSD/Map/Temp/Temp.otrk2.xml");
	Parser::ParserList *parsers;
	IO::StmData::FileData *fd;
	IO::PackageFile *pkg;
	Map::OSM::OSMLocalTileMap *tileMap;
	Map::TileMapLayer *mapLyr;
	NEW_CLASS(parsers, Parser::FullParserList());

	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	pkg = (IO::PackageFile*)parsers->ParseFileType(fd, IO::ParserType::PackageFile);
	DEL_CLASS(fd);
	if (pkg)
	{
		NEW_CLASS(tileMap, Map::OSM::OSMLocalTileMap(pkg));
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, parsers));
		Exporter::OruxMapExporter exporter;
		exporter.ExportNewFile(destFile, mapLyr, 0);
		DEL_CLASS(mapLyr);
	}
	DEL_CLASS(parsers);
	return 0;
}