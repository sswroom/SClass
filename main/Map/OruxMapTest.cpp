#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/OruxMapExporter.h"
#include "IO/PackageFile.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Parser/FullParserList.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CStringNN fileName = CSTR("/media/sswroom/Extreme SSD/Map/HKTIle.spk");
	Text::CStringNN destFile = CSTR("/media/sswroom/Extreme SSD/Map/Temp/Temp.otrk2.xml");
	Parser::ParserList *parsers;
	IO::PackageFile *pkg;
	Map::OSM::OSMLocalTileMap *tileMap;
	Map::TileMapLayer *mapLyr;
	NEW_CLASS(parsers, Parser::FullParserList());

	{
		IO::StmData::FileData fd(fileName, false);
		pkg = (IO::PackageFile*)parsers->ParseFileType(fd, IO::ParserType::PackageFile);
	}
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