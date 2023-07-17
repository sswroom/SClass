#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileParser.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"

#define HDRSIZE 512

IO::ParsedObject *IO::FileParser::ParseFile(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[HDRSIZE];
	fd->GetRealData(0, HDRSIZE, BYTEARR(hdr));
	return ParseFileHdr(fd, pkgFile, targetType, hdr);
}

IO::ParsedObject *IO::FileParser::ParseFilePath(Text::CString filePath)
{
	IO::PackageFile *pkg = 0;
	UOSInt i = filePath.LastIndexOf(IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		NotNullPtr<Text::String> dir = Text::String::New(filePath.v, i);
		NEW_CLASS(pkg, IO::DirectoryPackage(dir));
		dir->Release();
	}
	IO::ParsedObject *pobj;
	{
		IO::StmData::FileData fd(filePath, false);	
		pobj = this->ParseFile(&fd, pkg, IO::ParserType::Unknown);
	}
	SDEL_CLASS(pkg);
	return pobj;
}

const UOSInt IO::FileParser::hdrSize = HDRSIZE;
