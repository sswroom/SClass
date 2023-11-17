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
	NotNullPtr<IO::PackageFile> pkgFile;
	NotNullPtr<Map::OSM::OSMLocalTileMap> tileMap;
	NotNullPtr<Map::TileMapLayer> mapLyr;
	NEW_CLASS(parsers, Parser::FullParserList());

	{
		IO::StmData::FileData fd(fileName, false);
		pkg = (IO::PackageFile*)parsers->ParseFileType(fd, IO::ParserType::PackageFile);
	}
	if (pkgFile.Set(pkg))
	{
		NEW_CLASSNN(tileMap, Map::OSM::OSMLocalTileMap(pkgFile));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, parsers));
		Exporter::OruxMapExporter exporter;
		exporter.ExportNewFile(destFile, mapLyr, 0);
		mapLyr.Delete();
	}
	DEL_CLASS(parsers);
	return 0;
}