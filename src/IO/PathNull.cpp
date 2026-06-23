#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Text/MyStringW.h"

#if !defined(PATH_MAX)
#define PATH_MAX 512
#endif

UTF8Char IO::Path::PATH_SEPERATOR = (UTF8Char)'/';
UnsafeArray<const UTF8Char> IO::Path::ALL_FILES = U8STR("*");
UIntOS IO::Path::ALL_FILES_LEN = 1;

UnsafeArray<WChar> IO::Path::GetTempFileW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName)
{
	return buff;
}

Bool IO::Path::IsDirectoryExistW(UnsafeArray<const WChar> dir)
{
	return false;
}

Bool IO::Path::CreateDirectoryW(UnsafeArray<const WChar> dirInput)
{
	return false;
}

Bool IO::Path::RemoveDirectoryW(UnsafeArray<const WChar> dir)
{
	return false;
}

Bool IO::Path::DeleteFileW(UnsafeArray<const WChar> fileName)
{
	return false;
}

IntOS IO::Path::FileNameCompareW(UnsafeArray<const WChar> file1, UnsafeArray<const WChar> file2)
{
	return Text::StrCompare(file1, file2);
}

UnsafeArray<WChar> IO::Path::GetFileDirectoryW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName)
{
	return Text::StrConcat(buff, fileName);
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetProcessFileName(UnsafeArray<UTF8Char> buff)
{
	return nullptr;
}

UnsafeArrayOpt<WChar> IO::Path::GetProcessFileNameW(UnsafeArray<WChar> buff)
{
	return nullptr;
}

Bool IO::Path::GetProcessFileName(NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

UnsafeArray<UTF8Char> IO::Path::ReplaceExt(UnsafeArray<UTF8Char> fileName, UnsafeArray<const UTF8Char> ext, UIntOS extLen)
{
	UnsafeArrayOpt<UTF8Char> oldExt = nullptr;
	UnsafeArray<UTF8Char> nnoldExt;
	UTF8Char c;
	while ((c = *fileName++) != 0)
	{
		if (c == '\\' || c == '/')
		{
			oldExt = nullptr;
		}
		else if (c == '.')
		{
			oldExt = fileName;
		}
	}
	if (!oldExt.SetTo(nnoldExt))
	{
		nnoldExt = fileName;
		nnoldExt[-1] = '.';
	}
	return Text::StrConcatC(nnoldExt, ext, extLen);
}

UnsafeArray<WChar> IO::Path::ReplaceExtW(UnsafeArray<WChar> fileName, UnsafeArray<const WChar> ext)
{
	UnsafeArrayOpt<WChar> oldExt = nullptr;
	UnsafeArray<WChar> nnoldExt;
	WChar c;
	while ((c = *fileName++) != 0)
	{
		if (c == '\\' || c == '/')
		{
			oldExt = nullptr;
		}
		else if (c == '.')
		{
			oldExt = fileName;
		}
	}
	if (!oldExt.SetTo(nnoldExt))
	{
		nnoldExt = fileName;
		nnoldExt[-1] = '.';
	}
	return Text::StrConcat(nnoldExt, ext);
}

UnsafeArray<WChar> IO::Path::GetFileExtW(UnsafeArray<WChar> fileBuff, UnsafeArray<const WChar> path)
{
	UIntOS i = Text::StrLastIndexOfChar(path, '/');
	if (i != INVALID_INDEX)
	{
		path = &path[i + 1];
	}
	i = Text::StrLastIndexOfChar(path, '.');
	if (i != INVALID_INDEX)
	{
		return Text::StrConcat(fileBuff, &path[i + 1]);
	}
	else
	{
		return Text::StrConcat(fileBuff, L"");
	}
}

UnsafeArray<WChar> IO::Path::AppendPathW(UnsafeArray<WChar> path, UnsafeArray<const WChar> toAppend)
{
	if (toAppend[0] == '/')
		return Text::StrConcat(path, toAppend);
	UIntOS i = Text::StrLastIndexOfChar(path, '/');
	if (GetPathTypeW(path) == IO::Path::PathType::File && i != INVALID_INDEX)
	{
		path[i] = 0;
		i = Text::StrLastIndexOfChar(path, '/');
	}
	while (Text::StrStartsWith(toAppend, L"../"))
	{
		if (i != INVALID_INDEX)
		{
			path[i] = 0;
			i = Text::StrLastIndexOfChar(path, '/');
		}
		toAppend += 3;
	}
	i = Text::StrCharCnt(UnsafeArray<const WChar>(path));
	path[i] = '/';
	return Text::StrConcat(&path[i + 1], toAppend);
}

Bool IO::Path::AppendPath(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> toAppend, UIntOS toAppendLen)
{
	if (toAppend[0] == '/')
	{
		sb->ClearStr();
		sb->AppendC(toAppend, toAppendLen);
		return true;
	}
	UIntOS i = Text::StrLastIndexOfCharC(sb->ToString(), sb->GetLength(), '/');
	if (GetPathType(sb->ToCString()) == IO::Path::PathType::File && i != INVALID_INDEX)
	{
		sb->RemoveChars(sb->GetLength() - i);
		i = Text::StrLastIndexOfCharC(sb->ToString(), sb->GetLength(), '/');
	}
	while (Text::StrStartsWithC(toAppend, toAppendLen, UTF8STRC("../")))
	{
		if (i != INVALID_INDEX)
		{
			sb->RemoveChars(sb->GetLength() - i);
			i = Text::StrLastIndexOfCharC(sb->ToString(), sb->GetLength(), '/');
		}
		toAppend += 3;
		toAppendLen -= 3;
	}
	sb->AppendUTF8Char('/');
	sb->AppendC(toAppend, toAppendLen);
	return true;
}

Optional<IO::Path::FindFileSession> IO::Path::FindFile(Text::CStringNN path)
{
	return nullptr;
}

Optional<IO::Path::FindFileSession> IO::Path::FindFileW(UnsafeArray<const WChar> path)
{
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> IO::Path::FindNextFile(UnsafeArray<UTF8Char> buff, NN<IO::Path::FindFileSession> session, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize)
{
	return nullptr;
}

UnsafeArrayOpt<WChar> IO::Path::FindNextFileW(UnsafeArray<WChar> buff, NN<IO::Path::FindFileSession> session, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize)
{
	return nullptr;
}

void IO::Path::FindFileClose(NN<IO::Path::FindFileSession> session)
{
}

IO::Path::PathType IO::Path::GetPathTypeW(UnsafeArray<const WChar> path)
{
	return IO::Path::PathType::Unknown;
}

Bool IO::Path::PathExists(UnsafeArray<const UTF8Char> path, UIntOS pathLen)
{
	return false;
}

Bool IO::Path::PathExistsW(UnsafeArray<const WChar> path)
{
	return false;
}

UnsafeArray<WChar> IO::Path::GetFullPathW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> path)
{
	return Text::StrConcat(buff, path);
}

Bool IO::Path::FilePathMatch(UnsafeArray<const UTF8Char> path, UIntOS pathLen, UnsafeArray<const UTF8Char> searchPattern, UIntOS patternLen)
{
	UTF8Char sbuff[256];
	UIntOS i = Text::StrLastIndexOfCharC(path, pathLen, '/');
	UnsafeArray<const UTF8Char> fileName = &path[i + 1];
	Text::StrConcatC(sbuff, searchPattern, patternLen);
	Bool isWC = false;
	UnsafeArrayOpt<UTF8Char> patternStart = nullptr;
	UnsafeArray<UTF8Char> nnpatternStart;
	UnsafeArray<UTF8Char> currPattern = sbuff;
	UTF8Char c;
	while (true)
	{
		c = *currPattern;
		if (c == 0)
		{
			if (isWC)
			{
				if (!patternStart.SetTo(nnpatternStart))
					return true;
				return Text::StrEndsWith(fileName, nnpatternStart);
			}
			else if (patternStart.SetTo(nnpatternStart))
			{
				return Text::StrCompareICase(fileName, nnpatternStart) == 0;
			}
			else
			{
				return *fileName == 0;
			}
		}
		else if (c == '?' || c == '*')
		{
			if (isWC)
			{
				if (!patternStart.SetTo(nnpatternStart))
					return false;
				*currPattern = 0;
				if ((i = Text::StrIndexOf(fileName, nnpatternStart)) == INVALID_INDEX)
					return false;
				fileName += i + (currPattern - nnpatternStart);
				patternStart = nullptr;
				isWC = false;
			}
			else if (patternStart.SetTo(nnpatternStart))
			{
				*currPattern = 0;
				if (!Text::StrStartsWithICase(fileName, nnpatternStart))
					return false;
				fileName += currPattern - nnpatternStart;
				patternStart = nullptr;
				isWC = false;
			}
			if (c == '?')
			{
				if (*fileName == 0)
					return false;
				fileName++;
				currPattern++;
			}
			else
			{
				isWC = true;
				patternStart = nullptr;
				currPattern++;
			}
		}
		else
		{
			if (patternStart.IsNull())
			{
				patternStart = currPattern;
			}
			currPattern++;
		}
	}
}

Bool IO::Path::FilePathMatchW(UnsafeArray<const WChar> path, UnsafeArray<const WChar> searchPattern)
{
	WChar wbuff[256];
	UIntOS i = Text::StrLastIndexOfChar(path, '/');
	const WChar *fileName = &path[i + 1];
	Text::StrConcat(wbuff, searchPattern);
	Bool isWC = false;
	WChar *patternStart = 0;
	WChar *currPattern = wbuff;
	WChar c;
	while (true)
	{
		c = *currPattern;
		if (c == 0)
		{
			if (isWC)
			{
				if (patternStart == 0)
					return true;
				return Text::StrEndsWith(fileName, patternStart);
			}
			else if (patternStart)
			{
				return Text::StrCompareICase(fileName, patternStart) == 0;
			}
			else
			{
				return *fileName == 0;
			}
		}
		else if (c == '?' || c == '*')
		{
			if (isWC)
			{
				if (patternStart == 0)
					return false;
				*currPattern = 0;
				if ((i = Text::StrIndexOfW(fileName, patternStart)) == INVALID_INDEX)
					return false;
				fileName += i + currPattern - patternStart;
				patternStart = 0;
				isWC = false;
			}
			else if (patternStart)
			{
				*currPattern = 0;
				if (!Text::StrStartsWithICase(fileName, patternStart))
					return false;
				fileName += currPattern - patternStart;
				patternStart = 0;
				isWC = false;
			}
			if (c == '?')
			{
				if (*fileName == 0)
					return false;
				fileName++;
				currPattern++;
			}
			else
			{
				isWC = true;
				patternStart = 0;
				currPattern++;
			}
		}
		else
		{
			if (patternStart == 0)
			{
				patternStart = currPattern;
			}
			currPattern++;
		}
	}
}

UInt64 IO::Path::GetFileSizeW(UnsafeArray<const WChar> path)
{
	return 0;
}

UnsafeArrayOpt<WChar> IO::Path::GetSystemProgramPathW(UnsafeArray<WChar> buff)
{
	return nullptr;
}

UnsafeArrayOpt<WChar> IO::Path::GetLocAppDataPathW(UnsafeArray<WChar> buff)
{
	return nullptr;
}

UnsafeArrayOpt<WChar> IO::Path::GetOSPathW(UnsafeArray<WChar> buff)
{
	return nullptr;
}


Bool IO::Path::GetFileTime(Text::CStringNN path, OptOut<Data::Timestamp> modTime, OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> accessTime)
{
	return false;
}

Data::Timestamp IO::Path::GetModifyTime(UnsafeArray<const UTF8Char> path)
{
	return Data::Timestamp(0);
}

UnsafeArrayOpt<WChar> IO::Path::GetCurrDirectoryW(UnsafeArray<WChar> buff)
{
	return nullptr;
}

Bool IO::Path::SetCurrDirectoryW(UnsafeArray<const WChar> path)
{
	return false;
}

Bool IO::Path::IsSearchPattern(UnsafeArray<const UTF8Char> path)
{
	Bool isSrch = false;
	UTF8Char c;
	while ((c = *path++) != 0)
	{
		if (c == '*' || c == '?')
		{
			isSrch = true;
			break;
		}
	}
	return isSrch;
}

UnsafeArray<UTF8Char> IO::Path::GetRealPath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> path, UIntOS pathLen)
{
	return Text::StrConcatC(sbuff, path, pathLen);
}
