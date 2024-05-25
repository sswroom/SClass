#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/OruxMapExporter.h"
#include "IO/PackageFile.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Parser/FullParserList.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Text::CStringNN fileName = CSTR("/media/sswroom/Extreme SSD/Map/HKTIle.spk");
	Text::CStringNN destFile = CSTR("/media/sswroom/Extreme SSD/Map/Temp/Temp.otrk2.xml");
	NN<Parser::ParserList> parsers;
	Optional<IO::PackageFile> pkg;
	NN<IO::PackageFile> pkgFile;
	NN<Map::OSM::OSMLocalTileMap> tileMap;
	NN<Map::TileMapLayer> mapLyr;
	NEW_CLASSNN(parsers, Parser::FullParserList());

	{
		IO::StmData::FileData fd(fileName, false);
		pkg = Optional<IO::PackageFile>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::PackageFile));
	}
	if (pkg.SetTo(pkgFile))
	{
		NEW_CLASSNN(tileMap, Map::OSM::OSMLocalTileMap(pkgFile));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, parsers));
		Exporter::OruxMapExporter exporter;
		exporter.ExportNewFile(destFile, mapLyr, 0);
		mapLyr.Delete();
	}
	parsers.Delete();
	return 0;
}