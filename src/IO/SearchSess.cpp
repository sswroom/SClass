#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/SearchSess.h"
#include "IO/Path.h"

IO::SearchSess::SearchSess(const UTF8Char *searchName)
{
	UOSInt i;
	NEW_CLASS(this->modTime, Data::DateTime());
	this->nameBuff = MemAlloc(UTF8Char, 256);
	Text::StrConcat(this->nameBuff, searchName);
	i = Text::StrLastIndexOfChar(this->nameBuff, IO::Path::PATH_SEPERATOR);
	this->nameStart = &this->nameBuff[i + 1];
	this->sess = IO::Path::FindFile(this->nameBuff);
}

IO::SearchSess::~SearchSess()
{
	if (this->sess)
	{
		IO::Path::FindFileClose(this->sess);
	}
	MemFree(this->nameBuff);
	DEL_CLASS(this->modTime);
}

Bool IO::SearchSess::NextFile()
{
	Bool ret;
	if (this->sess == 0)
		return false;
	while (ret = (IO::Path::FindNextFile(this->nameStart, this->sess, this->modTime, &this->pt, &this->fileSize) != 0))
	{
		if (this->nameStart[0] != '.')
			break;
		if (this->nameStart[1] == '.' && this->nameStart[2] == 0)
		{
			continue;
		}
		else if (this->nameStart[1] != 0)
			break;
	}
	return ret;
}

const UTF8Char *IO::SearchSess::GetFileName()
{
	return this->nameStart;
}

const UTF8Char *IO::SearchSess::GetFilePath()
{
	return this->nameBuff;
}

IO::Path::PathType IO::SearchSess::GetPathType()
{
	return this->pt;
}

Int64 IO::SearchSess::GetFileSize()
{
	return this->fileSize;
}

Data::DateTime *IO::SearchSess::GetModTime()
{
	return this->modTime;
}
