#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/SearchSess.h"
#include "IO/Path.h"

IO::SearchSess::SearchSess(UnsafeArray<const UTF8Char> searchName)
{
	UIntOS i;
	UnsafeArray<UTF8Char> sptr;
	this->nameBuff = MemAllocArr(UTF8Char, 256);
	sptr = Text::StrConcat(this->nameBuff, searchName);
	i = Text::StrLastIndexOfCharC(this->nameBuff, (UIntOS)(sptr - this->nameBuff), IO::Path::PATH_SEPERATOR);
	this->nameStart = &this->nameBuff[i + 1];
	this->sess = IO::Path::FindFile(CSTRP(this->nameBuff, sptr));
}

IO::SearchSess::~SearchSess()
{
	NN<IO::Path::FindFileSession> sess;
	if (this->sess.SetTo(sess))
	{
		IO::Path::FindFileClose(sess);
	}
	MemFreeArr(this->nameBuff);
}

Bool IO::SearchSess::NextFile()
{
	Bool ret;
	NN<IO::Path::FindFileSession> sess;
	if (!this->sess.SetTo(sess))
		return false;
	while (ret = (IO::Path::FindNextFile(this->nameStart, sess, this->modTime, this->pt, this->fileSize).NotNull()))
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

UnsafeArray<const UTF8Char> IO::SearchSess::GetFileName()
{
	return this->nameStart;
}

UnsafeArray<const UTF8Char> IO::SearchSess::GetFilePath()
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

Data::Timestamp IO::SearchSess::GetModTime()
{
	return this->modTime;
}
