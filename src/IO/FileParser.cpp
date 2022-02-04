#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileParser.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"

IO::ParsedObject *IO::FileParser::ParseFilePath(Text::CString filePath)
{
	IO::PackageFile *pkg = 0;
	IO::StmData::FileData *fd;
	UOSInt i = filePath.LastIndexOf(IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		Text::String *dir = Text::String::New(filePath.v, i);
		NEW_CLASS(pkg, IO::DirectoryPackage(dir));
		dir->Release();
	}
	NEW_CLASS(fd, IO::StmData::FileData(filePath, false));	
	IO::ParsedObject *pobj = this->ParseFile(fd, pkg, IO::ParserType::Unknown);
	DEL_CLASS(fd);
	SDEL_CLASS(pkg);
	return pobj;
}
