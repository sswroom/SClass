#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"

IO::PackageFile::PackageFile(NotNullPtr<Text::String> fileName) : IO::ParsedObject(fileName)
{
}

IO::PackageFile::PackageFile(Text::CStringNN fileName) : IO::ParsedObject(fileName)
{
}

IO::PackageFile::~PackageFile()
{
}

IO::ParserType IO::PackageFile::GetParserType() const
{
	return IO::ParserType::PackageFile;
}


Optional<IO::StreamData> IO::PackageFile::GetItemStmDataNew(Text::CStringNN name) const
{
	UOSInt index = GetItemIndex(name);
	if (index == INVALID_INDEX)
	{
		return 0;
	}
	return this->GetItemStmDataNew(index);
}
