#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileParser.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"

IO::ParsedObject *IO::FileParser::ParseFilePath(const UTF8Char *filePath)
{
	IO::PackageFile *pkg = 0;
	IO::StmData::FileData *fd;
	UOSInt i = Text::StrLastIndexOf(filePath, IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		const UTF8Char *dir = Text::StrCopyNewC(filePath, i);
		NEW_CLASS(pkg, IO::DirectoryPackage(dir));
		Text::StrDelNew(dir);
	}
	NEW_CLASS(fd, IO::StmData::FileData(filePath, false));	
	IO::ParsedObject *pobj = this->ParseFile(fd, pkg, IO::ParserType::Unknown);
	DEL_CLASS(fd);
	SDEL_CLASS(pkg);
	return pobj;
}
