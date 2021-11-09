#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/DirectoryPackage.h"
#include "Map/ESRI/FileGDBDir.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::DirectoryPackage *dir;
	NEW_CLASS(dir, IO::DirectoryPackage((const UTF8Char*)"~/Progs/Temp/E20210522_PLIS.gdb"));
	Map::ESRI::FileGDBDir *fileGDB;
	fileGDB = Map::ESRI::FileGDBDir::OpenDir(dir);
	DEL_CLASS(dir);
	if (fileGDB)
	{
		//fileGDB->GetTableData()
		DEL_CLASS(fileGDB);
	}
	return 0;
}
