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
const UTF8Char *IO::Path::ALL_FILES = (const UTF8Char*)"*.*";

UTF8Char *IO::Path::GetTempFile(UTF8Char *buff, const UTF8Char *fileName)
{
	WChar tmpBuff[MAX_PATH];
	::GetTempPathW(MAX_PATH, tmpBuff);
	Path::CreateDirectoryW(tmpBuff);
	return Text::StrConcat(Text::StrWChar_UTF8(buff, tmpBuff, -1), fileName);
}

WChar *IO::Path::GetTempFileW(WChar *buff, const WChar *fileName)
{
	WChar tmpBuff[MAX_PATH];
	::GetTempPathW(MAX_PATH, tmpBuff);
	Path::CreateDirectoryW(tmpBuff);
	return Text::StrConcat(Text::StrConcat(buff, tmpBuff), fileName);
}

Bool IO::Path::IsDirectoryExist(const UTF8Char *dir)
{
	const WChar *wdir = Text::StrToWCharNew(dir);
	UInt32 ret = GetFileAttributesW(wdir);
	Text::StrDelNew(wdir);
	if (ret == INVALID_FILE_ATTRIBUTES)
		return false;
	else
		return (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

Bool IO::Path::IsDirectoryExistW(const WChar *dir)
{
	UInt32 ret = GetFileAttributesW(dir);
	if (ret == INVALID_FILE_ATTRIBUTES)
		return false;
	else
		return (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

Bool IO::Path::CreateDirectory(const UTF8Char *dirInput)
{
	WChar dir[MAX_PATH];
	if (IsDirectoryExist(dirInput))
		return true;
	Text::StrUTF8_WChar(dir, dirInput, 0);
	OSInt i = Text::StrLastIndexOf(dir, '\\');
	if (i == -1)
		return ::CreateDirectoryW(dir, 0) != 0;
	if (dir[i - 1] != ':')
	{
		dir[i] = 0;
		CreateDirectoryW(dir);
		dir[i] = '\\';
	}
	return ::CreateDirectoryW(dir, 0) != 0;
}

Bool IO::Path::CreateDirectoryW(const WChar *dirInput)
{
	WChar dir[MAX_PATH];
	Text::StrConcat(dir, dirInput);
	if (IsDirectoryExistW(dir))
		return true;
	OSInt i = Text::StrLastIndexOf(dir, '\\');
	if (i == -1)
		return ::CreateDirectoryW(dir, 0) != 0;
	if (dir[i - 1] != ':')
	{
		dir[i] = 0;
		CreateDirectoryW(dir);
		dir[i] = '\\';
	}
	return ::CreateDirectoryW(dir, 0) != 0;
}

Bool IO::Path::RemoveDirectory(const UTF8Char *dir)
{
	const WChar *wdir = Text::StrToWCharNew(dir);
	Bool ret = ::RemoveDirectoryW(wdir) != 0;
	Text::StrDelNew(wdir);
	return ret;
}

Bool IO::Path::RemoveDirectoryW(const WChar *dir)
{
	return ::RemoveDirectoryW(dir) != 0;
}

Bool IO::Path::DeleteFile(const UTF8Char *fileName)
{
	const WChar *wfileName = Text::StrToWCharNew(fileName);
	Bool ret = ::DeleteFileW(wfileName) != 0;
	Text::StrDelNew(wfileName);
	return ret;
}

Bool IO::Path::DeleteFileW(const WChar *fileName)
{
	return ::DeleteFileW(fileName) != 0;
}

OSInt IO::Path::FileNameCompare(const UTF8Char *file1, const UTF8Char *file2)
{
	return Text::StrCompareICase(file1, file2);
}

OSInt IO::Path::FileNameCompareW(const WChar *file1, const WChar *file2)
{
	return Text::StrCompareICase(file1, file2);
}

UTF8Char *IO::Path::GetFileDirectory(UTF8Char *buff, const UTF8Char *fileName)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, fileName);
#else
	WChar *ptr;
	UTF8Char *ptr2;
	WChar *ptr3 = 0;
	if (fileName[1] == ':')
	{
		OSInt i = Text::StrLastIndexOf(fileName, '\\');
		if (i >= 0)
		{
			return Text::StrConcatC(buff, fileName, (UOSInt)i);
		}
		else
		{
			return Text::StrConcat(buff, fileName);
		}
	}
	else
	{
		ptr3 = MemAlloc(WChar, MAX_PATH);
		const WChar *wfileName = Text::StrToWCharNew(fileName);
		GetFullPathNameW(wfileName, MAX_PATH, ptr3, &ptr);
		Text::StrDelNew(wfileName);
		if (ptr)
		{
			ptr[-1] = 0;
		}
		ptr2 = Text::StrWChar_UTF8(buff, ptr3, -1);
		MemFree(ptr3);
		return ptr2;
	}
#endif
}

WChar *IO::Path::GetFileDirectoryW(WChar *buff, const WChar *fileName)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, fileName);
#else
	WChar *ptr;
	WChar *ptr3 = 0;
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
		ptr3 = MemAlloc(WChar, MAX_PATH);
		GetFullPathNameW(fileName, MAX_PATH, ptr3, &ptr);
		if (ptr)
		{
			ptr[-1] = 0;
		}
		ptr = Text::StrConcat(buff, ptr3);
		MemFree(ptr3);
		return ptr;
	}
#endif
}

UTF8Char *IO::Path::GetProcessFileName(UTF8Char *buff)
{
	UInt32 retSize;
	WChar tmpBuff[1024];
#ifdef _WIN32_WCE
	retSize = GetModuleFileNameW(0, tmpBuff, 1024);
#else
	retSize = GetModuleFileNameExW(GetCurrentProcess(), 0, tmpBuff, 1024);
#endif
	tmpBuff[retSize] = 0;
	return Text::StrWChar_UTF8(buff, tmpBuff, -1);
}

WChar *IO::Path::GetProcessFileNameW(WChar *buff)
{
	UInt32 retSize;
#ifdef _WIN32_WCE
	retSize = GetModuleFileNameW(0, buff, 1024);
#else
	retSize = GetModuleFileNameExW(GetCurrentProcess(), 0, buff, 1024);
#endif
	buff[retSize] = 0;
	return &buff[retSize];
}

Bool IO::Path::GetProcessFileName(Text::StringBuilderUTF *sb)
{
	UInt32 retSize;
	WChar *sptr = MemAlloc(WChar, 1024);
#ifdef _WIN32_WCE
	retSize = GetModuleFileNameW(0, sptr, 1024);
#else
	retSize = GetModuleFileNameExW(GetCurrentProcess(), 0, sptr, 1024);
#endif
	sptr[retSize] = 0;
	const UTF8Char *csptr = Text::StrToUTF8New(sptr);
	sb->Append(csptr);
	Text::StrDelNew(csptr);
	MemFree(sptr);
	return true;
}

UTF8Char *IO::Path::ReplaceExt(UTF8Char *fileName, const UTF8Char *ext)
{
	UTF8Char *oldExt = 0;
	UTF8Char c;
	while ((c = *fileName++) != 0)
	{
		if (c == '\\' || c == '/')
		{
			oldExt = 0;
		}
		else if (c == '.')
		{
			oldExt = fileName;
		}
	}
	if (oldExt == 0)
	{
		oldExt = fileName;
		oldExt[-1] = '.';
	}
	return Text::StrConcat(oldExt, ext);
}

WChar *IO::Path::ReplaceExtW(WChar *fileName, const WChar *ext)
{
	WChar *oldExt = 0;
	WChar c;
	while ((c = *fileName++) != 0)
	{
		if (c == '\\' || c == '/')
		{
			oldExt = 0;
		}
		else if (c == '.')
		{
			oldExt = fileName;
		}
	}
	if (oldExt == 0)
	{
		oldExt = fileName;
		oldExt[-1] = '.';
	}
	return Text::StrConcat(oldExt, ext);
}

UTF8Char *IO::Path::GetFileExt(UTF8Char *fileBuff, const UTF8Char *path)
{
	OSInt i = Text::StrLastIndexOf(path, '\\');
	if (i >= 0)
	{
		path = &path[i + 1];
	}
	i = Text::StrLastIndexOf(path, '.');
	if (i >= 0)
	{
		return Text::StrConcat(fileBuff, &path[i + 1]);
	}
	else
	{
		return Text::StrConcat(fileBuff, (const UTF8Char*)"");
	}
}

WChar *IO::Path::GetFileExtW(WChar *fileBuff, const WChar *path)
{
	OSInt i = Text::StrLastIndexOf(path, '\\');
	if (i >= 0)
	{
		path = &path[i + 1];
	}
	i = Text::StrLastIndexOf(path, '.');
	if (i >= 0)
	{
		return Text::StrConcat(fileBuff, &path[i + 1]);
	}
	else
	{
		return Text::StrConcat(fileBuff, L"");
	}
}

UTF8Char *IO::Path::AppendPath(UTF8Char *path, const UTF8Char *toAppend)
{
	UTF8Char pathTmp[512];
	UTF8Char *lastSep;
	UTF8Char *firstSep;
	UTF8Char *pathArr[5];
	UOSInt pathCnt;
	UOSInt j;
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
		firstSep = &path[2 + Text::StrIndexOf(&path[2], '\\')];
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
	lastSep = &path[Text::StrLastIndexOf(path, '\\')];
	if (lastSep < path)
	{
		lastSep = path;
	}
	else if (lastSep[1] == 0)
	{
	}
	else
	{
		if (Text::StrIndexOf(lastSep, '*') != -1)
		{
		}
		else if (IO::Path::GetPathType(path) == PT_DIRECTORY)
		{
			lastSep = &path[Text::StrCharCnt(path)];
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
						UTF8Char *sptr = lastSep;
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
		return &lastSep[Text::StrCharCnt(lastSep)];
	}
}

WChar *IO::Path::AppendPathW(WChar *path, const WChar *toAppend)
{
	WChar pathTmp[512];
	WChar *lastSep;
	WChar *firstSep;
	WChar *pathArr[5];
	UOSInt pathCnt;
	UOSInt j;
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
		firstSep = &path[2 + Text::StrIndexOf(&path[2], '\\')];
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
	lastSep = &path[Text::StrLastIndexOf(path, '\\')];
	if (lastSep < path)
	{
		lastSep = path;
	}
	else if (lastSep[1] == 0)
	{
	}
	else
	{
		if (Text::StrIndexOf(lastSep, '*') != -1)
		{
		}
		else if (IO::Path::GetPathTypeW(path) == PT_DIRECTORY)
		{
			lastSep = &path[Text::StrCharCnt(path)];
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
						WChar *sptr = lastSep;
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
		return &lastSep[Text::StrCharCnt(lastSep)];
	}
}

Bool IO::Path::AppendPath(Text::StringBuilderUTF8 *sb, const UTF8Char *toAppend)
{
	UTF8Char pathTmp[512];
	UTF8Char *lastSep;
	UTF8Char *firstSep;
	UTF8Char *pathArr[5];
	UOSInt pathCnt;
	UOSInt j;
	UInt32 i;
	Int32 k;
	if (toAppend[1] == ':' && toAppend[2] == '\\')
	{
		sb->ClearStr();
		sb->Append(toAppend);
		return true;
	}
	else if (toAppend[0] == '\\' && toAppend[1] == '\\')
	{
		sb->ClearStr();
		sb->Append(toAppend);
		return true;
	}
	UTF8Char *path = sb->ToString();
	if (path[0] == '\\' && path[1] == '\\')
	{
		firstSep = &path[2 + Text::StrIndexOf(&path[2], '\\')];
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
		sb->SetEndPtr(firstSep);
		sb->Append(toAppend);
		return true;
	}
	lastSep = &path[Text::StrLastIndexOf(path, '\\')];
	if (lastSep < path)
	{
		lastSep = path;
	}
	else if (lastSep[1] == 0)
	{
	}
	else
	{
		if (Text::StrIndexOf(lastSep, '*') != -1)
		{
		}
		else if (IO::Path::GetPathType(path) == PT_DIRECTORY)
		{
			lastSep = &path[Text::StrCharCnt(path)];
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
						UTF8Char *sptr = lastSep;
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
					sb->Append(pathArr[i]);
					sb->Append((const UTF8Char*)"\\");
					lastSep = sb->GetEndPtr() - 1;
				}
				else
				{
					sb->SetEndPtr(lastSep);
					sb->Append(pathArr[i]);
					sb->Append((const UTF8Char*)"\\");
					lastSep = sb->GetEndPtr() - 1;
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
		lastSep[0] = 0;
		sb->SetEndPtr(lastSep);
		return true;
	}
	else
	{
		return true;
	}
}

IO::Path::FindFileSession *IO::Path::FindFile(const UTF8Char *path)
{
	FindFileSession *sess;
	sess = MemAlloc(FindFileSession, 1);
	sess->lastFound = true;
	const WChar *wpath = Text::StrToWCharNew(path);
	sess->handle = FindFirstFileW(wpath, &sess->findData);
	Text::StrDelNew(wpath);
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
		return 0;
	}
}

IO::Path::FindFileSession *IO::Path::FindFileW(const WChar *path)
{
	FindFileSession *sess;
	sess = MemAlloc(FindFileSession, 1);
	sess->lastFound = true;
	sess->handle = FindFirstFileW(path, &sess->findData);
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
		return 0;
	}
}

UTF8Char *IO::Path::FindNextFile(UTF8Char *buff, IO::Path::FindFileSession *sess, Data::DateTime *modTime, IO::Path::PathType *pt, UInt64 *fileSize)
{
	IO::Path::PathType tmp;
	if (pt == 0)
	{
		pt = &tmp;
	}
	UTF8Char *outPtr;
	if (sess->lastFound)
	{
		outPtr = Text::StrWChar_UTF8(buff, sess->findData.cFileName, -1);
		if (modTime)
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(&sess->findData.ftLastWriteTime, &st);
			modTime->ToUTCTime();
			modTime->SetValue(st.wYear, (UInt8)st.wMonth, (UInt8)st.wDay, (UInt8)st.wHour, (UInt8)st.wMinute, (UInt8)st.wSecond, st.wMilliseconds);
		}
		if (pt)
		{
			if (sess->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				*pt = IO::Path::PT_DIRECTORY;
			}
			else
			{
				*pt = IO::Path::PT_FILE;
			}
		}
		if (fileSize)
		{
			*fileSize = (((UInt64)sess->findData.nFileSizeHigh) << 32) | sess->findData.nFileSizeLow;
		}
		sess->lastFound = (::FindNextFileW(sess->handle, &sess->findData) != 0);
		return outPtr;
	}
	else
	{
		return 0;
	}
}

WChar *IO::Path::FindNextFileW(WChar *buff, IO::Path::FindFileSession *sess, Data::DateTime *modTime, IO::Path::PathType *pt, UInt64 *fileSize)
{
	IO::Path::PathType tmp;
	if (pt == 0)
	{
		pt = &tmp;
	}
	WChar *outPtr;
	if (sess->lastFound)
	{
		outPtr = Text::StrConcat(buff, sess->findData.cFileName);
		if (modTime)
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(&sess->findData.ftLastWriteTime, &st);
			modTime->ToUTCTime();
			modTime->SetValue(st.wYear, (UInt8)st.wMonth, (UInt8)st.wDay, (UInt8)st.wHour, (UInt8)st.wMinute, (UInt8)st.wSecond, st.wMilliseconds);
		}
		if (pt)
		{
			if (sess->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				*pt = IO::Path::PT_DIRECTORY;
			}
			else
			{
				*pt = IO::Path::PT_FILE;
			}
		}
		if (fileSize)
		{
			*fileSize = (((UInt64)sess->findData.nFileSizeHigh) << 32) | sess->findData.nFileSizeLow;
		}
		sess->lastFound = (::FindNextFileW(sess->handle, &sess->findData) != 0);
		return outPtr;
	}
	else
	{
		return 0;
	}
}

void IO::Path::FindFileClose(IO::Path::FindFileSession *sess)
{
	if (sess->handle != INVALID_HANDLE_VALUE)
		FindClose(sess->handle);
	MemFree(sess);
}

IO::Path::PathType IO::Path::GetPathType(const UTF8Char *path)
{
	const WChar *wpath = Text::StrToWCharNew(path);
	UInt32 fatt = GetFileAttributesW(wpath);
	Text::StrDelNew(wpath);
	if (fatt == INVALID_FILE_ATTRIBUTES)
	{
		return PT_UNKNOWN;
	}
	else if (fatt & FILE_ATTRIBUTE_DIRECTORY)
	{
		return PT_DIRECTORY;
	}
	else
	{
		return PT_FILE;
	}
}

IO::Path::PathType IO::Path::GetPathTypeW(const WChar *path)
{
	UInt32 fatt = GetFileAttributesW(path);
	if (fatt == INVALID_FILE_ATTRIBUTES)
	{
		return PT_UNKNOWN;
	}
	else if (fatt & FILE_ATTRIBUTE_DIRECTORY)
	{
		return PT_DIRECTORY;
	}
	else
	{
		return PT_FILE;
	}
}

Bool IO::Path::FileNameMatch(const UTF8Char *path, const UTF8Char *searchPattern)
{
	UTF8Char sbuff[256];
	OSInt i = Text::StrLastIndexOf(path, '\\');
	const UTF8Char *fileName = &path[i + 1];
	Text::StrConcat(sbuff, searchPattern);
	Bool isWC = false;
	UTF8Char *patternStart = 0;
	UTF8Char *currPattern = sbuff;
	UTF8Char c;
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
				if ((i = Text::StrIndexOf(fileName, patternStart)) < 0)
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

Bool IO::Path::FileNameMatchW(const WChar *path, const WChar *searchPattern)
{
	WChar sbuff[256];
	OSInt i = Text::StrLastIndexOf(path, '\\');
	const WChar *fileName = &path[i + 1];
	Text::StrConcat(sbuff, searchPattern);
	Bool isWC = false;
	WChar *patternStart = 0;
	WChar *currPattern = sbuff;
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
				if ((i = Text::StrIndexOf(fileName, patternStart)) < 0)
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

UInt64 IO::Path::GetFileSize(const UTF8Char *path)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	const WChar *wptr = Text::StrToWCharNew(path);
	if (GetFileAttributesExW(wptr, GetFileExInfoStandard, &attr) != 0)
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

UInt64 IO::Path::GetFileSizeW(const WChar *path)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	if (GetFileAttributesExW(path, GetFileExInfoStandard, &attr) != 0)
	{
		return (((UInt64)attr.nFileSizeHigh) << 32) | attr.nFileSizeLow;
	}
	else
	{
		return 0;
	}
}

WChar *IO::Path::GetSystemProgramPathW(WChar *buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, L"\\Program Files\\");
#else
	UTF8Char sbuff[512];
	Manage::EnvironmentVar::GetEnvValue(sbuff, (const UTF8Char*)"PROGRAMFILES");
	return Text::StrUTF8_WChar(buff, sbuff, 0);
#endif
}

UTF8Char *IO::Path::GetLocAppDataPath(UTF8Char *buff)
{
#ifdef _WIN32_WCE
	return 0;
#else
	WChar sbuff[512];
	if (SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, SHGFP_TYPE_CURRENT, sbuff) != S_OK)
	{
		return 0;
	}
	return Text::StrWChar_UTF8(buff, sbuff, -1);
#endif
}

WChar *IO::Path::GetLocAppDataPathW(WChar *buff)
{
#ifdef _WIN32_WCE
	return 0;
#else
	if (SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, SHGFP_TYPE_CURRENT, buff) != S_OK)
	{
		return 0;
	}
	return &buff[Text::StrCharCnt(buff)];
#endif
}

UTF8Char *IO::Path::GetOSPath(UTF8Char *buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, (const UTF8Char*)"\\Windows\\");
#else
	return Manage::EnvironmentVar::GetEnvValue(buff, (const UTF8Char*)"windir");
#endif
}

WChar *IO::Path::GetOSPathW(WChar *buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, L"\\Windows\\");
#else
	UTF8Char sbuff[512];
	Manage::EnvironmentVar::GetEnvValue(sbuff, (const UTF8Char*)"windir");
	return Text::StrUTF8_WChar(buff, sbuff, 0);
#endif
}

Bool IO::Path::GetFileTime(const UTF8Char *path, Data::DateTime *modTime, Data::DateTime *createTime, Data::DateTime *accessTime)
{
	WIN32_FIND_DATAW fd;
	const WChar *wptr = Text::StrToWCharNew(path);
	HANDLE hand = FindFirstFileW(wptr, &fd);
	Text::StrDelNew(wptr);
	SYSTEMTIME sysTime;
	if (hand == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hand);
	if (modTime)
	{
		FileTimeToSystemTime(&fd.ftLastWriteTime, &sysTime);
		modTime->SetValueSYSTEMTIME(&sysTime);
	}
	if (createTime)
	{
		FileTimeToSystemTime(&fd.ftCreationTime, &sysTime);
		createTime->SetValueSYSTEMTIME(&sysTime);
	}
	if (accessTime)
	{
		FileTimeToSystemTime(&fd.ftLastAccessTime, &sysTime);
		accessTime->SetValueSYSTEMTIME(&sysTime);
	}
	return true;
}

UTF8Char *IO::Path::GetCurrDirectory(UTF8Char *buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, (const UTF8Char*)"\\");
#else
	WChar sbuff[512];
	sbuff[GetCurrentDirectoryW(512, sbuff)] = 0;
	return Text::StrWChar_UTF8(buff, sbuff, -1);
#endif
}

WChar *IO::Path::GetCurrDirectoryW(WChar *buff)
{
#ifdef _WIN32_WCE
	return Text::StrConcat(buff, L"\\");
#else
	return buff + ::GetCurrentDirectoryW(512, buff);
#endif
}

Bool IO::Path::SetCurrDirectory(const UTF8Char *path)
{
	const WChar *wptr = Text::StrToWCharNew(path);
	Bool ret = SetCurrDirectoryW(wptr);
	Text::StrDelNew(wptr);
	return ret;
}

Bool IO::Path::SetCurrDirectoryW(const WChar *path)
{
#ifdef _WIN32_WCE
	return false;
#else
	return ::SetCurrentDirectoryW(path) != 0;
#endif
}

Bool IO::Path::IsSearchPattern(const UTF8Char *path)
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
