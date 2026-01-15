#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"
#include "Manage/EnvironmentVar.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include <shlobj.h>
#ifndef _WIN32_WCE
#define PSAPI_VERSION 1
#include <psapi.h>
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xffffffff
#endif

#undef CreateDirectory
#undef DeleteFile
#undef FindNextFile
#undef MoveFile
#undef CopyFile
#undef RemoveDirectory

struct IO::Path::FindFileSession
{
	HANDLE handle;
	Bool lastFound;
	WIN32_FIND_DATAW findData;
};

UTF8Char IO::Path::PATH_SEPERATOR = (UTF8Char)'\\';
UnsafeArray<const UTF8Char> IO::Path::ALL_FILES = U8STR("*.*");
UIntOS IO::Path::ALL_FILES_LEN = 3;

UnsafeArray<UTF8Char> IO::Path::GetTempFile(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName, UIntOS fileNameLen)
{
	WChar tmpBuff[MAX_PATH];
	::GetTempPathW(MAX_PATH, tmpBuff);
	if (Text::StrEqualsICase(&tmpBuff[3], L"Windows\\"))
	{
		Text::StrConcat(tmpBuff, L".\\");
	}
	Path::CreateDirectoryW(tmpBuff);
	return Text::StrConcatC(Text::StrWChar_UTF8(buff, tmpBuff), fileName, fileNameLen);
}

UnsafeArray<WChar> IO::Path::GetTempFileW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName)
{
	WChar tmpBuff[MAX_PATH];
	::GetTempPathW(MAX_PATH, tmpBuff);
	if (Text::StrEqualsICase(&tmpBuff[3], L"Windows\\"))
	{
		Text::StrConcat(tmpBuff, L".\\");
	}
	Path::CreateDirectoryW(tmpBuff);
	return Text::StrConcat(Text::StrConcat(buff, tmpBuff), fileName);
}

Bool IO::Path::IsDirectoryExist(Text::CStringNN dir)
{
	WChar wbuff[256];
	UInt32 ret;
	if (dir.leng < 256)
	{
		Text::StrUTF8_WCharC(wbuff, dir.v, dir.leng, 0);
		ret = GetFileAttributesW(wbuff);
	}
	else
	{
		UnsafeArray<const WChar> wdir = Text::StrToWCharNew(dir.v);
		ret = GetFileAttributesW(wdir.Ptr());
		Text::StrDelNew(wdir);
	}
	if (ret == INVALID_FILE_ATTRIBUTES)
		return false;
	else
		return (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

Bool IO::Path::IsDirectoryExistW(UnsafeArray<const WChar> dir)
{
	UInt32 ret = GetFileAttributesW(dir.Ptr());
	if (ret == INVALID_FILE_ATTRIBUTES)
		return false;
	else
		return (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

Bool IO::Path::CreateDirectory(Text::CStringNN dirInput)
{
	WChar dir[MAX_PATH];
	Text::StrUTF8_WChar(dir, dirInput.v, 0);
	if (IsDirectoryExistW(dir))
		return true;
	UIntOS i = Text::StrLastIndexOfCharW(dir, '\\');
	if (i == INVALID_INDEX)
		return ::CreateDirectoryW(dir, 0) != 0;
	if (dir[i - 1] != ':')
	{
		dir[i] = 0;
		CreateDirectoryW(dir);
		dir[i] = '\\';
	}
	return ::CreateDirectoryW(dir, 0) != 0;
}

Bool IO::Path::CreateDirectoryW(UnsafeArray<const WChar> dirInput)
{
	WChar dir[MAX_PATH];
	Text::StrConcat(dir, dirInput);
	if (IsDirectoryExistW(dir))
		return true;
	UIntOS i = Text::StrLastIndexOfCharW(dir, '\\');
	if (i == INVALID_INDEX)
		return ::CreateDirectoryW(dir, 0) != 0;
	if (dir[i - 1] != ':')
	{
		dir[i] = 0;
		CreateDirectoryW(dir);
		dir[i] = '\\';
	}
	return ::CreateDirectoryW(dir, 0) != 0;
}

Bool IO::Path::RemoveDirectory(UnsafeArray<const UTF8Char> dir)
{
	UnsafeArray<const WChar> wdir = Text::StrToWCharNew(dir);
	Bool ret = ::RemoveDirectoryW(wdir.Ptr()) != 0;
	Text::StrDelNew(wdir);
	return ret;
}

Bool IO::Path::RemoveDirectoryW(UnsafeArray<const WChar> dir)
{
	return ::RemoveDirectoryW(dir.Ptr()) != 0;
}

Bool IO::Path::DeleteFile(UnsafeArray<const UTF8Char> fileName)
{
	UnsafeArray<const WChar> wfileName = Text::StrToWCharNew(fileName);
	Bool ret = ::DeleteFileW(wfileName.Ptr()) != 0;
	Text::StrDelNew(wfileName);
	return ret;
}

Bool IO::Path::DeleteFileW(UnsafeArray<const WChar> fileName)
{
	return ::DeleteFileW(fileName.Ptr()) != 0;
}

IntOS IO::Path::FileNameCompare(UnsafeArray<const UTF8Char> file1, UnsafeArray<const UTF8Char> file2)
{
	return Text::StrCompareICase(file1, file2);
}

IntOS IO::Path::FileNameCompareW(UnsafeArray<const WChar> file1, UnsafeArray<const WChar> file2)
{
	return Text::StrCompareICase(file1, file2);
}

UnsafeArray<UTF8Char> IO::Path::GetFileDirectory(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, fileName);
#else
	WChar *ptr;
	UnsafeArray<UTF8Char> ptr2;
	WChar *ptr3 = 0;
	if (fileName[1] == ':')
	{
		UIntOS i = Text::StrLastIndexOfChar(fileName, '\\');
		if (i != INVALID_INDEX)
		{
			return Text::StrConcatC(buff, fileName, (UIntOS)i);
		}
		else
		{
			return Text::StrConcat(buff, fileName);
		}
	}
	else
	{
		ptr3 = MemAlloc(WChar, MAX_PATH);
		UnsafeArray<const WChar> wfileName = Text::StrToWCharNew(fileName);
		GetFullPathNameW(wfileName.Ptr(), MAX_PATH, ptr3, &ptr);
		Text::StrDelNew(wfileName);
		if (ptr)
		{
			ptr[-1] = 0;
		}
		ptr2 = Text::StrWChar_UTF8(buff, ptr3);
		MemFree(ptr3);
		return ptr2;
	}
#endif
}

UnsafeArray<WChar> IO::Path::GetFileDirectoryW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, fileName);
#else
	UnsafeArray<WChar> ptr;
	UnsafeArray<WChar> ptr3;
	if (fileName[1] == ':')
	{
		ptr = buff;
		ptr3 = buff;
		while ((*ptr++ = *fileName++) != 0)
			if (ptr[-1] == '\\')
				ptr3 = ptr - 1;
		*ptr3 = 0;
		return ptr3;
	}
	else
	{
		ptr3 = MemAllocArr(WChar, MAX_PATH);
		WChar *ptr2;
		GetFullPathNameW(fileName.Ptr(), MAX_PATH, ptr3.Ptr(), &ptr2);
		if (ptr2)
		{
			ptr2[-1] = 0;
		}
		ptr = Text::StrConcat(buff, ptr3);
		MemFreeArr(ptr3);
		return ptr;
	}
#endif
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetProcessFileName(UnsafeArray<UTF8Char> buff)
{
	UInt32 retSize;
	WChar tmpBuff[1024];
#ifdef _WIN32_WCE
	retSize = GetModuleFileNameW(0, tmpBuff, 1024);
#else
	retSize = GetModuleFileNameExW(GetCurrentProcess(), 0, tmpBuff, 1024);
#endif
	tmpBuff[retSize] = 0;
	return Text::StrWChar_UTF8(buff, tmpBuff);
}

UnsafeArrayOpt<WChar> IO::Path::GetProcessFileNameW(UnsafeArray<WChar> buff)
{
	UInt32 retSize;
#ifdef _WIN32_WCE
	retSize = GetModuleFileNameW(0, buff, 1024);
#else
	retSize = GetModuleFileNameExW(GetCurrentProcess(), 0, buff.Ptr(), 1024);
#endif
	buff[retSize] = 0;
	return &buff[retSize];
}

Bool IO::Path::GetProcessFileName(NN<Text::StringBuilderUTF8> sb)
{
	UInt32 retSize;
	WChar *wptr = MemAlloc(WChar, 1024);
#ifdef _WIN32_WCE
	retSize = GetModuleFileNameW(0, wptr, 1024);
#else
	retSize = GetModuleFileNameExW(GetCurrentProcess(), 0, wptr, 1024);
#endif
	wptr[retSize] = 0;
	sb->AppendW(wptr);
	MemFree(wptr);
	return true;
}

UnsafeArray<UTF8Char> IO::Path::ReplaceExt(UnsafeArray<UTF8Char> fileName, UnsafeArray<const UTF8Char> ext, UIntOS extLen)
{
	UnsafeArrayOpt<UTF8Char> oldExt = nullptr;
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
	UnsafeArray<UTF8Char> nnoldExt;
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

UnsafeArray<UTF8Char> IO::Path::GetFileExt(UnsafeArray<UTF8Char> fileBuff, UnsafeArray<const UTF8Char> path, UIntOS pathLen)
{
	if (pathLen >= 4 && path[pathLen - 4] == '.')
	{
		return Text::StrConcatC(fileBuff, &path[pathLen - 3], 3);
	}
	UIntOS i = pathLen;
	while (i-- > 0)
	{
		if (path[i] == '.')
		{
			return Text::StrConcatC(fileBuff, &path[i + 1], pathLen - i - 1);
		}
		else if (path[i] == '\\')
		{
			*fileBuff = 0;
			return fileBuff;
		}
	}
	*fileBuff = 0;
	return fileBuff;
}

UnsafeArray<WChar> IO::Path::GetFileExtW(UnsafeArray<WChar> fileBuff, UnsafeArray<const WChar> path)
{
	UIntOS i = Text::StrLastIndexOfCharW(path, '\\');
	if (i != INVALID_INDEX)
	{
		path = &path[i + 1];
	}
	i = Text::StrLastIndexOfCharW(path, '.');
	if (i != INVALID_INDEX)
	{
		return Text::StrConcat(fileBuff, &path[i + 1]);
	}
	else
	{
		return Text::StrConcat(fileBuff, L"");
	}
}

UnsafeArray<UTF8Char> IO::Path::AppendPath(UnsafeArray<UTF8Char> path, UnsafeArray<UTF8Char> pathEnd, Text::CStringNN toAppend)
{
	UTF8Char pathTmp[512];
	UnsafeArray<UTF8Char> lastSep;
	UnsafeArray<UTF8Char> firstSep;
	Text::PString pathArr[5];
	UIntOS pathCnt;
	UIntOS j;
	UInt32 i;
	Int32 k;
	if (toAppend.v[1] == ':' && toAppend.v[2] == '\\')
	{
		return toAppend.ConcatTo(path);
	}
	else if (toAppend.v[0] == '\\' && toAppend.v[1] == '\\')
	{
		return toAppend.ConcatTo(path);
	}
	if (path[0] == '\\' && path[1] == '\\')
	{
		firstSep = &path[2 + Text::StrIndexOfCharC(&path[2], (UIntOS)(pathEnd - &path[2]), '\\')];
	}
	else if (path[1] == ':' && path[2] == '\\')
	{
		firstSep = &path[2];
	}
	else if (path[1] == ':' && path[2] == 0)
	{
		firstSep = &path[2];
		path[2] = '\\';
		path[3] = 0;
		pathEnd = &path[3];
	}
	else
	{
		firstSep = path;
	}
	if (toAppend.v[0] == '\\')
	{
		return toAppend.ConcatTo(firstSep);
	}
	lastSep = &path[Text::StrLastIndexOfCharC(path, (UIntOS)(pathEnd - path), '\\')];
	if (lastSep < path)
	{
		lastSep = path;
	}
	else if (lastSep[1] == 0)
	{
	}
	else
	{
		if (Text::StrIndexOfChar(lastSep, '*') != INVALID_INDEX)
		{
		}
		else if (IO::Path::GetPathType(CSTRP(path, pathEnd)) == PathType::Directory)
		{
			lastSep = pathEnd;
			*lastSep = '\\';
			lastSep[1] = 0;
		}
		else
		{
		}
	}
	toAppend.ConcatTo(pathTmp);
	pathCnt = Text::StrSplitP(pathArr, 5, {pathTmp, toAppend.leng}, '\\');
	while (true)
	{
		if (pathCnt == 5)
		{
			j = 4;
		}
		else
		{
			j = pathCnt;
		}

		i = 0;
		while (i < j)
		{
			if (pathArr[i].v[0] == '.')
			{
				k = 1;
				while (pathArr[i].v[k] == '.')
				{
					if (lastSep > firstSep)
					{
						UnsafeArray<UTF8Char> sptr = lastSep;
						while (sptr-- > firstSep)
						{
							if (*sptr == '\\')
							{
								break;
							}
						}
						sptr[1] = 0;
						lastSep = sptr;
					}
					k++;
				}
			}
			else
			{
				if (lastSep[0] == '\\')
				{
					lastSep = Text::StrConcatC(&lastSep[1], pathArr[i].v, pathArr[i].leng);
					lastSep[0] = '\\';
					lastSep[1] = 0;
				}
				else
				{
					lastSep = Text::StrConcatC(lastSep, pathArr[i].v, pathArr[i].leng);
					lastSep[0] = '\\';
					lastSep[1] = 0;
				}
			}
			i++;
		}

		if (pathCnt == 5)
		{
			pathCnt = Text::StrSplitP(pathArr, 5, pathArr[4], '\\');
		}
		else
		{
			break;
		}
	}
	if (lastSep[1] == 0)
	{
		*lastSep = 0;
		return lastSep;
	}
	else
	{
		return &lastSep[Text::StrCharCnt(UnsafeArray<const UInt8>(lastSep))];
	}
}

UnsafeArray<WChar> IO::Path::AppendPathW(UnsafeArray<WChar> path, UnsafeArray<const WChar> toAppend)
{
	WChar pathTmp[512];
	UnsafeArray<WChar> lastSep;
	UnsafeArray<WChar> firstSep;
	UnsafeArray<WChar> pathArr[5];
	UIntOS pathCnt;
	UIntOS j;
	UInt32 i;
	Int32 k;
	if (toAppend[1] == ':' && toAppend[2] == '\\')
	{
		return Text::StrConcat(path, toAppend);
	}
	else if (toAppend[0] == '\\' && toAppend[1] == '\\')
	{
		return Text::StrConcat(path, toAppend);
	}
	if (path[0] == '\\' && path[1] == '\\')
	{
		firstSep = &path[2 + Text::StrIndexOfCharW(&path[2], '\\')];
	}
	else if (path[1] == ':' && path[2] == '\\')
	{
		firstSep = &path[2];
	}
	else if (path[1] == ':' && path[2] == 0)
	{
		firstSep = &path[2];
		path[2] = '\\';
		path[3] = 0;
	}
	else
	{
		firstSep = path;
	}
	if (toAppend[0] == '\\')
	{
		return Text::StrConcat(firstSep, toAppend);
	}
	lastSep = &path[Text::StrLastIndexOfCharW(UnsafeArray<const WChar>(path), '\\')];
	if (lastSep < path)
	{
		lastSep = path;
	}
	else if (lastSep[1] == 0)
	{
	}
	else
	{
		if (Text::StrIndexOfCharW(UnsafeArray<const WChar>(lastSep), '*') != INVALID_INDEX)
		{
		}
		else if (IO::Path::GetPathTypeW(path) == PathType::Directory)
		{
			lastSep = &path[Text::StrCharCnt(UnsafeArray<const WChar>(path))];
			*lastSep = '\\';
			lastSep[1] = 0;
		}
		else
		{
		}
	}
	Text::StrConcat(pathTmp, toAppend);
	pathCnt = Text::StrSplit(pathArr, 5, pathTmp, '\\');
	while (true)
	{
		if (pathCnt == 5)
		{
			j = 4;
		}
		else
		{
			j = pathCnt;
		}

		i = 0;
		while (i < j)
		{
			if (pathArr[i][0] == '.')
			{
				k = 1;
				while (pathArr[i][k] == '.')
				{
					if (lastSep > firstSep)
					{
						UnsafeArray<WChar> wptr = lastSep;
						while (wptr-- > firstSep)
						{
							if (*wptr == '\\')
							{
								break;
							}
						}
						wptr[1] = 0;
						lastSep = wptr;
					}
					k++;
				}
			}
			else
			{
				if (lastSep[0] == '\\')
				{
					lastSep = Text::StrConcat(&lastSep[1], pathArr[i]);
					lastSep[0] = '\\';
					lastSep[1] = 0;
				}
				else
				{
					lastSep = Text::StrConcat(lastSep, pathArr[i]);
					lastSep[0] = '\\';
					lastSep[1] = 0;
				}
			}
			i++;
		}

		if (pathCnt == 5)
		{
			pathCnt = Text::StrSplit(pathArr, 5, pathArr[4], '\\');
		}
		else
		{
			break;
		}
	}
	if (lastSep[1] == 0)
	{
		*lastSep = 0;
		return lastSep;
	}
	else
	{
		return &lastSep[Text::StrCharCnt(UnsafeArray<const WChar>(lastSep))];
	}
}

Bool IO::Path::AppendPath(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> toAppend, UIntOS toAppendLen)
{
	UTF8Char pathTmp[512];
	UnsafeArray<UTF8Char> lastSep;
	UnsafeArray<UTF8Char> firstSep;
	Text::PString pathArr[5];
	UIntOS pathCnt;
	UIntOS j;
	UInt32 i;
	Int32 k;
	if (toAppend[1] == ':' && toAppend[2] == '\\')
	{
		sb->ClearStr();
		sb->AppendC(toAppend, toAppendLen);
		return true;
	}
	else if (toAppend[0] == '\\' && toAppend[1] == '\\')
	{
		sb->ClearStr();
		sb->AppendC(toAppend, toAppendLen);
		return true;
	}
	UnsafeArray<UTF8Char> path = sb->v;
	UnsafeArray<UTF8Char> pathEnd = sb->GetEndPtr();
	if (path[0] == '\\' && path[1] == '\\')
	{
		firstSep = &path[2 + Text::StrIndexOfChar(&path[2], '\\')];
	}
	else if (path[1] == ':' && path[2] == '\\')
	{
		firstSep = &path[2];
	}
	else if (path[1] == ':' && path[2] == 0)
	{
		firstSep = &path[2];
		path[2] = '\\';
		path[3] = 0;
		pathEnd = &path[3];
	}
	else
	{
		firstSep = path;
	}
	if (toAppend[0] == '\\')
	{
		sb->SetEndPtr(firstSep);
		sb->AppendC(toAppend, toAppendLen);
		return true;
	}
	lastSep = &path[Text::StrLastIndexOfChar(path, '\\')];
	if (lastSep < path)
	{
		lastSep = path;
	}
	else if (lastSep[1] == 0)
	{
	}
	else
	{
		if (Text::StrIndexOfChar(lastSep, '*') != INVALID_INDEX)
		{
		}
		else if (IO::Path::GetPathType(CSTRP(path, pathEnd)) == PathType::Directory)
		{
			lastSep = pathEnd;
			*lastSep = '\\';
			lastSep[1] = 0;
		}
		else
		{
		}
	}

	Text::StrConcatC(pathTmp, toAppend, toAppendLen);
	pathCnt = Text::StrSplitP(pathArr, 5, {pathTmp, toAppendLen}, '\\');
	while (true)
	{
		if (pathCnt == 5)
		{
			j = 4;
		}
		else
		{
			j = pathCnt;
		}

		i = 0;
		while (i < j)
		{
			if (pathArr[i].v[0] == '.')
			{
				k = 1;
				while (pathArr[i].v[k] == '.')
				{
					if (lastSep > firstSep)
					{
						UnsafeArray<UTF8Char> sptr = lastSep;
						while (sptr-- > firstSep)
						{
							if (*sptr == '\\')
							{
								break;
							}
						}
						sptr[1] = 0;
						lastSep = sptr;
					}
					k++;
				}
			}
			else
			{
				if (lastSep[0] == '\\')
				{
					sb->SetEndPtr(&lastSep[1]);
					sb->AppendC(pathArr[i].v, pathArr[i].leng);
					sb->AppendC(UTF8STRC("\\"));
					lastSep = sb->GetEndPtr() - 1;
				}
				else
				{
					sb->SetEndPtr(lastSep);
					sb->AppendC(pathArr[i].v, pathArr[i].leng);
					sb->AppendC(UTF8STRC("\\"));
					lastSep = sb->GetEndPtr() - 1;
				}
			}
			i++;
		}

		if (pathCnt == 5)
		{
			pathCnt = Text::StrSplitP(pathArr, 5, pathArr[4], '\\');
		}
		else
		{
			break;
		}
	}
	if (lastSep[1] == 0)
	{
		lastSep[0] = 0;
		sb->SetEndPtr(lastSep);
		return true;
	}
	else
	{
		return true;
	}
}

Optional<IO::Path::FindFileSession> IO::Path::FindFile(Text::CStringNN path)
{
	WChar wbuff[MAX_PATH];
	FindFileSession *sess;
	sess = MemAlloc(FindFileSession, 1);
	sess->lastFound = true;
	Text::StrUTF8_WCharC(wbuff, path.v, path.leng, 0);
	sess->handle = FindFirstFileW(wbuff, &sess->findData);
	if (sess->handle != INVALID_HANDLE_VALUE)
	{
		return sess;
	}
	if (GetLastError() == ERROR_NO_MORE_FILES)
	{
		sess->lastFound = false;
		return sess;
	}
	else
	{
		MemFree(sess);
		return nullptr;
	}
}

Optional<IO::Path::FindFileSession> IO::Path::FindFileW(UnsafeArray<const WChar> path)
{
	FindFileSession *sess;
	sess = MemAlloc(FindFileSession, 1);
	sess->lastFound = true;
	sess->handle = FindFirstFileW(path.Ptr(), &sess->findData);
	if (sess->handle != INVALID_HANDLE_VALUE)
	{
		return sess;
	}
	if (GetLastError() == ERROR_NO_MORE_FILES)
	{
		sess->lastFound = false;
		return sess;
	}
	else
	{
		MemFree(sess);
		return nullptr;
	}
}

UnsafeArrayOpt<UTF8Char> IO::Path::FindNextFile(UnsafeArray<UTF8Char> buff, NN<IO::Path::FindFileSession> sess, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize)
{
	UnsafeArray<UTF8Char> outPtr;
	if (sess->lastFound)
	{
		outPtr = Text::StrWChar_UTF8(buff, sess->findData.cFileName);
		if (modTime.IsNotNull())
		{
			modTime.SetNoCheck(Data::Timestamp(Data::TimeInstant::FromFILETIME(&sess->findData.ftLastWriteTime), 0));
		}
		if (pt.IsNotNull())
		{
			if (sess->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				pt.SetNoCheck(IO::Path::PathType::Directory);
			}
			else
			{
				pt.SetNoCheck(IO::Path::PathType::File);
			}
		}
		fileSize.Set((((UInt64)sess->findData.nFileSizeHigh) << 32) | sess->findData.nFileSizeLow);
		sess->lastFound = (::FindNextFileW(sess->handle, &sess->findData) != 0);
		return outPtr;
	}
	else
	{
		return nullptr;
	}
}

UnsafeArrayOpt<WChar> IO::Path::FindNextFileW(UnsafeArray<WChar> buff, NN<IO::Path::FindFileSession> sess, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize)
{
	UnsafeArray<WChar> outPtr;
	if (sess->lastFound)
	{
		outPtr = Text::StrConcat(buff, sess->findData.cFileName);
		if (modTime.IsNotNull())
		{
			modTime.SetNoCheck(Data::Timestamp(Data::TimeInstant::FromFILETIME(&sess->findData.ftLastWriteTime), 0));
		}
		if (pt.IsNotNull())
		{
			if (sess->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				pt.SetNoCheck(IO::Path::PathType::Directory);
			}
			else
			{
				pt.SetNoCheck(IO::Path::PathType::File);
			}
		}
		fileSize.Set((((UInt64)sess->findData.nFileSizeHigh) << 32) | sess->findData.nFileSizeLow);
		sess->lastFound = (::FindNextFileW(sess->handle, &sess->findData) != 0);
		return outPtr;
	}
	else
	{
		return nullptr;
	}
}

void IO::Path::FindFileClose(NN<IO::Path::FindFileSession> sess)
{
	if (sess->handle != INVALID_HANDLE_VALUE)
		FindClose(sess->handle);
	MemFreeNN(sess);
}

IO::Path::PathType IO::Path::GetPathType(Text::CStringNN path)
{
	WChar wbuff[256];
	
	if (path.leng < 256)
	{
		Text::StrUTF8_WChar(wbuff, path.v, 0);
		return GetPathTypeW(wbuff);
	}
	else
	{
		UnsafeArray<const WChar> wpath = Text::StrToWCharNew(path.v);
		PathType ret = GetPathTypeW(wpath);
		Text::StrDelNew(wpath);
		return ret;
	}
}

IO::Path::PathType IO::Path::GetPathTypeW(UnsafeArray<const WChar> path)
{
#if (_WIN32_WINNT >= 0x0600)
	FILE_BASIC_INFO info;
	HANDLE handle = CreateFileW(path.Ptr(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return PathType::Unknown;
	}
	if (!GetFileInformationByHandleEx(handle, FileBasicInfo, &info, sizeof(info)))
	{
		CloseHandle(handle);
		return PathType::Unknown;
	}
	CloseHandle(handle);
	if (info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return PathType::Directory;
	}
	else
	{
		return PathType::File;
	}
#else
	UInt32 fatt = GetFileAttributesW(path.Ptr());
	if (fatt == INVALID_FILE_ATTRIBUTES)
	{
		return PathType::Unknown;
	}
	else if (fatt & FILE_ATTRIBUTE_DIRECTORY)
	{
		return PathType::Directory;
	}
	else
	{
		return PathType::File;
	}
#endif
}

Bool IO::Path::FileNameMatch(UnsafeArray<const UTF8Char> fileName, UIntOS fileNameLen, UnsafeArray<const UTF8Char> searchPattern, UIntOS patternLen)
{
	UnsafeArray<const UTF8Char> fileNameEnd = &fileName[fileNameLen];
	UIntOS i;
	Bool isWC = false;
	UnsafeArrayOpt<const UTF8Char> patternStart = nullptr;
	UnsafeArray<const UTF8Char> nnpatternStart;
	UnsafeArray<const UTF8Char> currPattern = searchPattern;
	UTF8Char c;
	while (true)
	{
		c = *currPattern;
		switch (c)
		{
		case 0:
			if (isWC)
			{
				if (!patternStart.SetTo(nnpatternStart))
					return true;
				return Text::StrEndsWithICaseC(fileName, (UIntOS)(fileNameEnd - fileName), nnpatternStart, (UIntOS)(currPattern - nnpatternStart));
			}
			else if (patternStart.SetTo(nnpatternStart))
			{
				return Text::StrEqualsICaseC(fileName, (UIntOS)(fileNameEnd - fileName), nnpatternStart, (UIntOS)(currPattern - nnpatternStart));
			}
			else
			{
				return *fileName == 0;
			}
		case '?':
		case '*':
			if (isWC)
			{
				if (!patternStart.SetTo(nnpatternStart))
					return false;
				if ((i = Text::StrIndexOfC(fileName, (UIntOS)(fileNameEnd - fileName), nnpatternStart, (UIntOS)(currPattern - nnpatternStart))) == INVALID_INDEX)
					return false;
				fileName += i + (currPattern - nnpatternStart);
				patternStart = nullptr;
				isWC = false;
			}
			else if (patternStart.SetTo(nnpatternStart))
			{
				if (!Text::StrStartsWithICaseC(fileName, (UIntOS)(fileNameEnd - fileName), nnpatternStart, (UIntOS)(currPattern - nnpatternStart)))
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
			break;
		default:
			if (patternStart.IsNull())
			{
				patternStart = currPattern;
			}
			currPattern++;
			break;
		}
	}
}

Bool IO::Path::FilePathMatch(UnsafeArray<const UTF8Char> path, UIntOS pathLen, UnsafeArray<const UTF8Char> searchPattern, UIntOS patternLen)
{
	UIntOS i = Text::StrLastIndexOfCharC(path, pathLen, '\\');
	return FileNameMatch(&path[i + 1], pathLen - i - 1, searchPattern, patternLen);
}

Bool IO::Path::FilePathMatchW(UnsafeArray<const WChar> path, UnsafeArray<const WChar> searchPattern)
{
	WChar wbuff[256];
	UIntOS i = Text::StrLastIndexOfCharW(path, '\\');
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

UInt64 IO::Path::GetFileSize(UnsafeArray<const UTF8Char> path)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(path);
	if (GetFileAttributesExW(wptr.Ptr(), GetFileExInfoStandard, &attr) != 0)
	{
		Text::StrDelNew(wptr);
		return (((UInt64)attr.nFileSizeHigh) << 32) | attr.nFileSizeLow;
	}
	else
	{
		Text::StrDelNew(wptr);
		return 0;
	}
}

UInt64 IO::Path::GetFileSizeW(UnsafeArray<const WChar> path)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	if (GetFileAttributesExW(path.Ptr(), GetFileExInfoStandard, &attr) != 0)
	{
		return (((UInt64)attr.nFileSizeHigh) << 32) | attr.nFileSizeLow;
	}
	else
	{
		return 0;
	}
}

UnsafeArrayOpt<WChar> IO::Path::GetSystemProgramPathW(UnsafeArray<WChar> buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, L"\\Program Files\\");
#else
	UTF8Char sbuff[512];
	Manage::EnvironmentVar::GetEnvValue(sbuff, CSTR("PROGRAMFILES"));
	return Text::StrUTF8_WChar(buff, sbuff, 0);
#endif
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetLocAppDataPath(UnsafeArray<UTF8Char> buff)
{
#ifdef _WIN32_WCE
	return 0;
#else
	WChar wbuff[512];
	if (SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, SHGFP_TYPE_CURRENT, wbuff) != S_OK)
	{
		return nullptr;
	}
	return Text::StrWChar_UTF8(buff, wbuff);
#endif
}

UnsafeArrayOpt<WChar> IO::Path::GetLocAppDataPathW(UnsafeArray<WChar> buff)
{
#ifdef _WIN32_WCE
	return nullptr;
#else
	if (SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, SHGFP_TYPE_CURRENT, buff.Ptr()) != S_OK)
	{
		return nullptr;
	}
	return &buff[Text::StrCharCnt(UnsafeArray<const WChar>(buff))];
#endif
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetOSPath(UnsafeArray<UTF8Char> buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcatC(buff, UTF8STRC("\\Windows\\"));
#else
	return Manage::EnvironmentVar::GetEnvValue(buff, CSTR("windir"));
#endif
}

UnsafeArrayOpt<WChar> IO::Path::GetOSPathW(UnsafeArray<WChar> buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, L"\\Windows\\");
#else
	UTF8Char sbuff[512];
	Manage::EnvironmentVar::GetEnvValue(sbuff, CSTR("windir"));
	return Text::StrUTF8_WChar(buff, sbuff, 0);
#endif
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetUserHome(UnsafeArray<UTF8Char> buff)
{
	buff = Manage::EnvironmentVar::GetEnvValue(buff, CSTR("HOMEDRIVE")).Or(buff);
	return Manage::EnvironmentVar::GetEnvValue(buff, CSTR("HOMEPATH"));
}

Bool IO::Path::GetFileTime(Text::CStringNN path, OptOut<Data::Timestamp> modTime, OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> accessTime)
{
	WIN32_FIND_DATAW fd;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(path.v);
	HANDLE hand = FindFirstFileW(wptr.Ptr(), &fd);
	Text::StrDelNew(wptr);
	if (hand == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hand);
	if (modTime.IsNotNull())
	{
		modTime.SetNoCheck(Data::Timestamp::FromFILETIME(&fd.ftLastWriteTime, Data::DateTimeUtil::GetLocalTzQhr()));
	}
	if (createTime.IsNotNull())
	{
		createTime.SetNoCheck(Data::Timestamp::FromFILETIME(&fd.ftCreationTime, Data::DateTimeUtil::GetLocalTzQhr()));
	}
	if (accessTime.IsNotNull())
	{
		accessTime.SetNoCheck(Data::Timestamp::FromFILETIME(&fd.ftLastAccessTime, Data::DateTimeUtil::GetLocalTzQhr()));
	}
	return true;
}

UInt32 IO::Path::GetFileUnixAttr(Text::CStringNN path)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(path.v);
	DWORD attr = GetFileAttributesW(wptr.Ptr());
	Text::StrDelNew(wptr);
	if (attr == INVALID_FILE_ATTRIBUTES)
		return 0;
	UInt32 unixAttr = 0x124;
	if ((attr & FILE_ATTRIBUTE_READONLY) == 0)
		unixAttr |= 0x92;
	if (attr & FILE_ATTRIBUTE_DIRECTORY)
		unixAttr |= 0x4000 | 0x49;
	return unixAttr;
}

Data::Timestamp IO::Path::GetModifyTime(UnsafeArray<const UTF8Char> path)
{
	WIN32_FIND_DATAW fd;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(path);
	HANDLE hand = FindFirstFileW(wptr.Ptr(), &fd);
	Text::StrDelNew(wptr);
	if (hand == INVALID_HANDLE_VALUE)
		return Data::Timestamp(0);
	FindClose(hand);
	return Data::Timestamp::FromFILETIME(&fd.ftLastWriteTime, Data::DateTimeUtil::GetLocalTzQhr());
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetCurrDirectory(UnsafeArray<UTF8Char> buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, (const UTF8Char*)"\\");
#else
	WChar wbuff[512];
	wbuff[GetCurrentDirectoryW(512, wbuff)] = 0;
	return Text::StrWChar_UTF8(buff, wbuff);
#endif
}

UnsafeArrayOpt<WChar> IO::Path::GetCurrDirectoryW(UnsafeArray<WChar> buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, L"\\");
#else
	return buff + (UIntOS)::GetCurrentDirectoryW(512, buff.Ptr());
#endif
}

Bool IO::Path::SetCurrDirectory(UnsafeArray<const UTF8Char> path)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(path);
	Bool ret = SetCurrDirectoryW(wptr);
	Text::StrDelNew(wptr);
	return ret;
}

Bool IO::Path::SetCurrDirectoryW(UnsafeArray<const WChar> path)
{
#ifdef _WIN32_WCE
	return false;
#else
	return ::SetCurrentDirectoryW(path.Ptr()) != 0;
#endif
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
	UnsafeArray<UTF8Char> sptr;
	if (Text::StrStartsWithC(path, pathLen, UTF8STRC("~/")))
	{
		sptr = Text::StrConcatC(IO::Path::GetUserHome(sbuff).Or(sbuff), path + 1, pathLen - 1);
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, path, pathLen);
	}
	Text::StrReplace(sbuff, '/', '\\');
	UnsafeArray<UTF8Char> sptr2 = sbuff;
	UIntOS i;
	while (true)
	{
		i = Text::StrIndexOfCharC(sptr2, (UIntOS)(sptr - sptr2), IO::Path::PATH_SEPERATOR);
		if (i == INVALID_INDEX)
		{
			break;
		}
		sptr2 = &sptr2[i + 1];
		if (sptr2[0] == '.' && sptr2[1] == '.' && sptr2[2] == 0)
		{
			i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr2 - sbuff - 1), IO::Path::PATH_SEPERATOR);
			if (i != INVALID_INDEX)
			{
				if (sbuff[i + 1] == '.' && sbuff[i + 2] == '.')
				{
				}
				else
				{
					sptr = &sbuff[i];
					*sptr = 0;
				}
			}
			return sptr;
		}
		else if (sptr2[0] == '.' && sptr2[1] == '.' && sptr2[2] == IO::Path::PATH_SEPERATOR)
		{
			i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr2 - sbuff - 1), IO::Path::PATH_SEPERATOR);
			if (i != INVALID_INDEX)
			{
				if (sbuff[i + 1] == '.' && sbuff[i + 2] == '.')
				{
				}
				else
				{
					sptr = Text::StrConcatC(&sbuff[i + 1], &sptr2[3], (UIntOS)(sptr - &sptr2[3]));
					sptr2 = &sbuff[i + 1];
				}
			}
		}
	}
	return sptr;
}
