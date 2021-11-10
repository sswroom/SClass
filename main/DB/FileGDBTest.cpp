#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/DirectoryPackage.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	IO::DirectoryPackage *dir;
	NEW_CLASS(dir, IO::DirectoryPackage((const UTF8Char*)"~/Progs/Temp/E20210522_PLIS.gdb"));
	Map::ESRI::FileGDBDir *fileGDB;
	fileGDB = Map::ESRI::FileGDBDir::OpenDir(dir);
	DEL_CLASS(dir);
	if (fileGDB)
	{
		Data::ArrayList<const UTF8Char*> colNames;
		colNames.Add((const UTF8Char*)"OBJECTID");
		colNames.Add((const UTF8Char*)"Shape");
		DB::QueryConditions cond;
		cond.Int32Equals((const UTF8Char*)"OBJECTID", 40);
		DB::DBReader *r = fileGDB->GetTableData((const UTF8Char*)"LAMPPOST", &colNames, 0, 10, (const UTF8Char*)"OBJECTID desc", 0);//&cond);
		if (r)
		{
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				Data::VariObject *obj = r->CreateVariObject();
				sb.ClearStr();
				obj->ToString(&sb);
				console.WriteLine(sb.ToString());
				DEL_CLASS(obj);
			}
			fileGDB->CloseReader(r);
		}
		DEL_CLASS(fileGDB);
	}
	return 0;
}
