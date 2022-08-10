#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(__FreeBSD__)
#include <limits.h>
#include <sys/sysctl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#if !defined(PATH_MAX)
#define PATH_MAX 512
#endif

struct IO::Path::FindFileSession
{
	Text::String *searchPattern;
	DIR *dirObj;
	UTF8Char pathBuff[512];
	UTF8Char *pathEnd;
};

UTF8Char IO::Path::PATH_SEPERATOR = (UTF8Char)'/';
const UTF8Char *IO::Path::ALL_FILES = (const UTF8Char*)"*";
UOSInt IO::Path::ALL_FILES_LEN = 1;

UTF8Char *IO::Path::GetTempFile(UTF8Char *buff, const UTF8Char *fileName, UOSInt fileNameLen)
{
	return Text::StrConcatC(Text::StrConcatC(buff, UTF8STRC("/tmp/")), fileName, fileNameLen);
}

WChar *IO::Path::GetTempFileW(WChar *buff, const WChar *fileName)
{
	return Text::StrConcat(Text::StrConcat(buff, L"/tmp/"), fileName);
}

Bool IO::Path::IsDirectoryExist(Text::CString dir)
{
	return GetPathType(dir) == PathType::Directory;
}

Bool IO::Path::IsDirectoryExistW(const WChar *dir)
{
	return GetPathTypeW(dir) == PathType::Directory;
}

Bool IO::Path::CreateDirectory(Text::CString dirInput)
{
	UOSInt i = dirInput.LastIndexOf('/');
	if (i != INVALID_INDEX && i > 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(dirInput.v, (UOSInt)i);
		if (GetPathType(sb.ToCString()) == PathType::Unknown)
		{
			CreateDirectory(sb.ToCString());
		}
	}
	int status = mkdir((const Char*)dirInput.v, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	return status == 0;
}

Bool IO::Path::CreateDirectoryW(const WChar *dirInput)
{
	UOSInt i = Text::StrLastIndexOfChar(dirInput, '/');
	if (i != INVALID_INDEX && i > 0)
	{
		const WChar *wptr = Text::StrCopyNewC(dirInput, (UOSInt)i);
		Text::String *s = Text::String::NewNotNull(wptr);
		if (GetPathType(s->ToCString()) == PathType::Unknown)
		{
			CreateDirectory(s->ToCString());
		}
		s->Release();
		Text::StrDelNew(wptr);
	}
	const UTF8Char *utfPath = Text::StrToUTF8New(dirInput);
	int status = mkdir((const Char*)utfPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	Text::StrDelNew(utfPath);
	return status == 0;
}

Bool IO::Path::RemoveDirectory(const UTF8Char *dir)
{
	int status = rmdir((const Char*)dir);
	return status == 0;
}

Bool IO::Path::RemoveDirectoryW(const WChar *dir)
{
	const UTF8Char *utfPath = Text::StrToUTF8New(dir);
	int status = rmdir((const Char*)utfPath);
	Text::StrDelNew(utfPath);
	return status == 0;
}

Bool IO::Path::DeleteFile(const UTF8Char *fileName)
{
	int status = unlink((const Char*)fileName);
	return status == 0;
}

Bool IO::Path::DeleteFileW(const WChar *fileName)
{
	const UTF8Char *utfPath = Text::StrToUTF8New(fileName);
	int status = unlink((const Char*)utfPath);
	Text::StrDelNew(utfPath);
	return status == 0;
}

OSInt IO::Path::FileNameCompare(const UTF8Char *file1, const UTF8Char *file2)
{
	return Text::StrCompare(file1, file2);
}

OSInt IO::Path::FileNameCompareW(const WChar *file1, const WChar *file2)
{
	return Text::StrCompare(file1, file2);
}

UTF8Char *IO::Path::GetFileDirectory(UTF8Char *buff, const UTF8Char *fileName)
{
	return Text::StrConcat(buff, fileName);
}

WChar *IO::Path::GetFileDirectoryW(WChar *buff, const WChar *fileName)
{
	return Text::StrConcat(buff, fileName);
}

UTF8Char *IO::Path::GetProcessFileName(UTF8Char *buff)
{
#if defined(__FreeBSD__)
	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PATHNAME;
	mib[3] = -1;
	size_t size = 512;
	if (sysctl(mib, 4, (Char*)buff, &size, 0, 0) != 0)
		return 0;
#else
	ssize_t size = readlink("/proc/self/exe", (Char*)buff, 1024);
	if (size == -1)
		return 0;
#endif
	buff[size] = 0;
	return &buff[size];
}

WChar *IO::Path::GetProcessFileNameW(WChar *buff)
{
	Char sbuff[1024];
#if defined(__FreeBSD__)
	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PATHNAME;
	mib[3] = -1;
	size_t size = sizeof(sbuff);
	if (sysctl(mib, 4, sbuff, &size, 0, 0) != 0)
		return 0;
#else
	ssize_t size = readlink("/proc/self/exe", sbuff, 1024);
	if (size == -1)
		return 0;
#endif
	return Text::StrUTF8_WCharC(buff, (UInt8*)sbuff, (UOSInt)size, 0);
}

Bool IO::Path::GetProcessFileName(Text::StringBuilderUTF8 *sb)
{
	Char sbuff[512];
	ssize_t size = readlink("/proc/self/exe", sbuff, 512);
	if (size == -1)
		return 0;
	sb->AppendC((const UTF8Char*)sbuff, (UOSInt)size);
	return true;
}

UTF8Char *IO::Path::ReplaceExt(UTF8Char *fileName, const UTF8Char *ext, UOSInt extLen)
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
	return Text::StrConcatC(oldExt, ext, extLen);
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

UTF8Char *IO::Path::GetFileExt(UTF8Char *fileBuff, const UTF8Char *path, UOSInt pathLen)
{
	if (pathLen >= 4 && path[pathLen - 4] == '.')
	{
		return Text::StrConcatC(fileBuff, &path[pathLen - 3], 3);
	}
	UOSInt i = pathLen;
	while (i-- > 0)
	{
		if (path[i] == '.')
		{
			return Text::StrConcatC(fileBuff, &path[i + 1], pathLen - i - 1);
		}
		else if (path[i] == '/')
		{
			*fileBuff = 0;
			return fileBuff;
		}
	}
	*fileBuff = 0;
	return fileBuff;
}

WChar *IO::Path::GetFileExtW(WChar *fileBuff, const WChar *path)
{
	UOSInt i = Text::StrLastIndexOfChar(path, '/');
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

UTF8Char *IO::Path::AppendPath(UTF8Char *path, UTF8Char *pathEnd, Text::CString toAppend)
{
	if (toAppend.v[0] == '/')
		return toAppend.ConcatTo(path);
	UOSInt pathLen = (UOSInt)(pathEnd - path);
	UOSInt i = Text::StrLastIndexOfCharC(path, pathLen, '/');
	IO::Path::PathType pt = GetPathType({path, pathLen});
	if (pt == PathType::File && i != INVALID_INDEX)
	{
		path[i] = 0;
		pathLen = i;
		i = Text::StrLastIndexOfCharC(path, pathLen, '/');
	}
	else if (i == pathLen - 1)
	{
		path[i] = 0;
		pathLen = i;
		i = Text::StrLastIndexOfCharC(path, pathLen, '/');
	}
	while (toAppend.StartsWith(UTF8STRC("../")))
	{
		if (i != INVALID_INDEX)
		{
			path[i] = 0;
			pathLen = i;
			i = Text::StrLastIndexOfCharC(path, pathLen, '/');
		}
		toAppend = toAppend.Substring(3);
	}
	path[pathLen] = '/';
	return toAppend.ConcatTo(&path[pathLen + 1]);
}

WChar *IO::Path::AppendPathW(WChar *path, const WChar *toAppend)
{
	if (toAppend[0] == '/')
		return Text::StrConcat(path, toAppend);
	UOSInt i = Text::StrLastIndexOfChar(path, '/');
	if (GetPathTypeW(path) == PathType::File && i != INVALID_INDEX)
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
	UOSInt j = Text::StrCharCnt(path);
	path[j] = '/';
	return Text::StrConcat(&path[j + 1], toAppend);
}

Bool IO::Path::AppendPath(Text::StringBuilderUTF8 *sb, const UTF8Char *toAppend, UOSInt toAppendLen)
{
	if (toAppend[0] == '/')
	{
		sb->ClearStr();
		sb->AppendC(toAppend, toAppendLen);
		return true;
	}
	UTF8Char *sptr = sb->v;
	UOSInt i = Text::StrLastIndexOfCharC(sptr, sb->GetLength(), '/');
	if (GetPathType({sptr, sb->GetLength()}) == PathType::File && i != INVALID_INDEX)
	{
		sb->RemoveChars(sb->GetLength() - i);
		i = Text::StrLastIndexOfCharC(sptr, sb->GetLength(), '/');
	}
	else if (i == sb->GetCharCnt() - 1)
	{
		sb->RemoveChars(1);
		i = Text::StrLastIndexOfCharC(sptr, sb->GetLength(), '/');
	}
	while (Text::StrStartsWithC(toAppend, toAppendLen, UTF8STRC("../")))
	{
		if (i != INVALID_INDEX)
		{
			sb->RemoveChars(sb->GetLength() - (UOSInt)i);
			i = Text::StrLastIndexOfCharC(sptr, sb->GetLength(), '/');
		}
		toAppend += 3;
		toAppendLen -= 3;
	}
	sb->AppendUTF8Char('/');
	sb->AppendC(toAppend, toAppendLen);
	return true;
}

IO::Path::FindFileSession *IO::Path::FindFile(Text::CString path)
{
	FindFileSession *sess = 0;
	const UTF8Char *searchPattern;
	UOSInt searchPatternLen;
	Text::CString searchDir;
	UOSInt i = path.LastIndexOf('/');
	DIR *dirObj;
	if (i == INVALID_INDEX)
	{
		dirObj = opendir(".");
		searchPattern = path.v;
		searchPatternLen = path.leng;
		searchDir = CSTR("./");
	}
	else if (i == 0)
	{
		dirObj = opendir("/");
		searchPattern = path.v + 1;
		searchPatternLen = path.leng - 1;
		searchDir = CSTR("/");
	}
	else
	{
		UTF8Char *tmpBuff = MemAlloc(UTF8Char, i + 2);
		Text::StrConcatC(tmpBuff, path.v, i);
		dirObj = opendir((const Char*)tmpBuff);
		searchPattern = path.v + i + 1;
		searchPatternLen = path.leng - i - 1;
		searchDir = {path.v, i + 1};
		MemFree(tmpBuff);
	}
	if (dirObj)
	{
		sess = MemAlloc(FindFileSession, 1);
		sess->searchPattern = Text::String::New(searchPattern, searchPatternLen);
		sess->dirObj = dirObj;
		sess->pathEnd = searchDir.ConcatTo(sess->pathBuff);
	}
	return sess;
}

IO::Path::FindFileSession *IO::Path::FindFileW(const WChar *path)
{
	FindFileSession *sess = 0;
	Text::String *utfPath = Text::String::NewNotNull(path);
	Text::CString searchPattern;
	Text::CString searchDir;
	UOSInt i = Text::StrLastIndexOfCharC(utfPath->v, utfPath->leng, '/');
	DIR *dirObj;
	if (i == INVALID_INDEX)
	{
		dirObj = opendir(".");
		searchPattern = utfPath->ToCString();
		searchDir = CSTR("./");
	}
	else if (i == 0)
	{
		dirObj = opendir("/");
		searchPattern = utfPath->ToCString().Substring(1);
		searchDir = CSTR("/");
	}
	else
	{
		utfPath->v[i] = 0;
		dirObj = opendir((const Char*)utfPath->v);
		searchPattern = utfPath->ToCString().Substring(i + 1);
		utfPath->v[i] = '/';
		searchDir = {utfPath->v, i + 1};
	}
	if (dirObj)
	{
		sess = MemAlloc(FindFileSession, 1);
		sess->searchPattern = Text::String::New(searchPattern);
		sess->dirObj = dirObj;
		sess->pathEnd = searchDir.ConcatTo(sess->pathBuff);
	}
	utfPath->Release();
	return sess;
}

UTF8Char *IO::Path::FindNextFile(UTF8Char *buff, IO::Path::FindFileSession *sess, Data::DateTime *modTime, IO::Path::PathType *pt, UInt64 *fileSize)
{
	struct dirent *ent;
	while ((ent = readdir(sess->dirObj)) != 0)
	{
		UOSInt len = Text::StrCharCnt(ent->d_name);
		if (FileNameMatch((const UTF8Char*)ent->d_name, len, sess->searchPattern->v, sess->searchPattern->leng))
		{
			Text::StrConcatC(sess->pathEnd, (const UTF8Char*)ent->d_name, len);
#if defined(__USE_LARGEFILE64)
			struct stat64 s;
			int status = lstat64((const Char*)sess->pathBuff, &s);
#else
			struct stat s;
			int status = lstat((const Char*)sess->pathBuff, &s);
#endif
			if (status == 0)
			{
				if (modTime)
				{
					modTime->SetUnixTimestamp(s.st_mtime);
				}
				if (pt)
				{
					if (S_ISREG(s.st_mode))
						*pt = PathType::File;
					else if (S_ISDIR(s.st_mode))
						*pt = PathType::Directory;
					else
						*pt = PathType::Unknown;
				}
				if (fileSize)
				{
					*fileSize = (UInt64)s.st_size;
				}
				return Text::StrConcatC(buff, (const UTF8Char*)ent->d_name, len);
			}
		}
		
	}
	return 0;
}

WChar *IO::Path::FindNextFileW(WChar *buff, IO::Path::FindFileSession *sess, Data::DateTime *modTime, IO::Path::PathType *pt, UInt64 *fileSize)
{
	struct dirent *ent;
	while ((ent = readdir(sess->dirObj)) != 0)
	{
		UOSInt len = Text::StrCharCnt(ent->d_name);
		if (FileNameMatch((const UTF8Char*)ent->d_name, len, sess->searchPattern->v, sess->searchPattern->leng))
		{
			Text::StrConcatC(sess->pathEnd, (const UTF8Char*)ent->d_name, len);
#if defined(__USE_LARGEFILE64)
			struct stat64 s;
			int status = lstat64((const Char*)sess->pathBuff, &s);
#else
			struct stat s;
			int status = lstat((const Char*)sess->pathBuff, &s);
#endif
			if (status == 0)
			{
				if (modTime)
				{
					modTime->SetUnixTimestamp(s.st_mtime);
				}
				if (pt)
				{
					if (S_ISREG(s.st_mode))
						*pt = PathType::File;
					else if (S_ISDIR(s.st_mode))
						*pt = PathType::Directory;
					else
						*pt = PathType::Unknown;
				}
				if (fileSize)
				{
					*fileSize = (UInt64)s.st_size;
				}
				return Text::StrUTF8_WChar(buff, (const UTF8Char*)ent->d_name, 0);
			}
		}
	}
	return 0;
}

void IO::Path::FindFileClose(IO::Path::FindFileSession *sess)
{
	sess->searchPattern->Release();
	closedir(sess->dirObj);
	MemFree(sess);
}

IO::Path::PathType IO::Path::GetPathType(Text::CString path)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path.v, &s);
#else
	struct stat s;
	int status = lstat((const Char*)path, &s);
#endif
	if (status != 0)
	{
		return PathType::Unknown;
	}
	if (S_ISREG(s.st_mode))
	{
		return PathType::File;
	}
	else if (S_ISCHR(s.st_mode))
	{
		return PathType::File;
	}
	else if (S_ISBLK(s.st_mode))
	{
		return PathType::File;
	}
	else if (S_ISDIR(s.st_mode))
	{
		return PathType::Directory;
	}
	else if (S_ISLNK(s.st_mode))
	{
		UTF8Char pathBuff[512];
		UTF8Char *pathBuffEnd;
		Char cbuff[512];
		ssize_t size = readlink((const Char*)path.v, cbuff, 511);
		cbuff[size] = 0;
		pathBuffEnd = path.ConcatTo(pathBuff);
		UOSInt i = Text::StrLastIndexOfCharC(pathBuff, (UOSInt)(pathBuffEnd - pathBuff), '/');
		if (i != INVALID_INDEX)
		{
			pathBuff[i + 1] = 0;
			pathBuffEnd = IO::Path::AppendPath(pathBuff, &pathBuff[i + 1], Text::CString((const UTF8Char*)cbuff, (UOSInt)size));
		}
		return GetPathType(CSTRP(pathBuff, pathBuffEnd));
	}
	return PathType::Unknown;
}

IO::Path::PathType IO::Path::GetPathTypeW(const WChar *path)
{
	Text::String *utfPath = Text::String::NewNotNull(path);
	IO::Path::PathType pt = IO::Path::GetPathType(utfPath->ToCString());
	utfPath->Release();
	return pt;
}

Bool IO::Path::PathExists(const UTF8Char *path, UOSInt pathLen)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path, &s);
#else
	struct stat s;
	int status = lstat((const Char*)path, &s);
#endif
	return status == 0;
}

Bool IO::Path::PathExistsW(const WChar *path)
{
	Text::String *utfPath = Text::String::NewNotNull(path);
	Bool ret = IO::Path::PathExists(utfPath->v, utfPath->leng);
	utfPath->Release();
	return ret;
}

WChar *IO::Path::GetFullPathW(WChar *buff, const WChar *path)
{
	Text::StringBuilderUTF8 sb;
	Text::String *str = Text::String::NewNotNull(path);
	sb.Append(str);
	str->Release();
	sb.AllocLeng(512);
	Char cbuff[512];
	UTF8Char *sptr = sb.v;
	int status;
	UOSInt i;
	UOSInt j;
//	printf("GetFullPath %ls\r\n", path);
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	status = lstat64((const Char*)sptr, &s);
#else
	struct stat s;
	status = lstat((const Char*)sptr, &s);
#endif
	if (status != 0)
	{
		i = Text::StrLastIndexOfCharC(sptr, sb.GetLength(), '/');
		while (true)
		{
//			printf("GetFullPath: Loop i = %d\r\n", i);
			if (i == INVALID_INDEX)
			{
				return Text::StrUTF8_WChar(buff, sptr, 0);
			}
			sptr[i] = 0;
#if defined(__USE_LARGEFILE64)
			status = lstat64((const Char*)sptr, &s);
#else
			status = lstat((const Char*)sptr, &s);
#endif
			if (status == 0)
			{
				if (S_ISLNK(s.st_mode))
				{
					ssize_t size = readlink((const Char*)sptr, cbuff, 511);
					cbuff[size] = 0;
//					printf("readlink %s -> %s\r\n", sptr, cbuff);
					//////////////////////////////
					return Text::StrUTF8_WChar(buff, sptr, 0);
				}
				else
				{
					sptr[i] = '/';
					return Text::StrUTF8_WChar(buff, sptr, 0);
				}
			}
			else
			{
				j = i;
				i = Text::StrLastIndexOfCharC(sptr, i, '/');
				sptr[j] = '/';
			}
		}
		
	}
	else if (S_ISREG(s.st_mode))
	{
		return Text::StrConcat(buff, path);
	}
	else if (S_ISDIR(s.st_mode))
	{
		return Text::StrConcat(buff, path);
	}
	else if (S_ISLNK(s.st_mode))
	{
		ssize_t size = readlink((const Char*)sptr, cbuff, 511);
		cbuff[size] = 0;
		Text::StrConcat(buff, path);

		const WChar *wptr = Text::StrToWCharNew((const UTF8Char*)cbuff);
		WChar *ret = IO::Path::AppendPathW(buff, wptr);
		Text::StrDelNew(wptr);
		return ret;
	}
	else
	{
		////////////////////////////////
		return Text::StrConcat(buff, path);
	}
}

Bool IO::Path::FileNameMatch(const UTF8Char *fileName, UOSInt fileNameLen, const UTF8Char *searchPattern, UOSInt patternLen)
{
	const UTF8Char *fileNameEnd = &fileName[fileNameLen];
	UOSInt i;
	Bool isWC = false;
	const UTF8Char *patternStart = 0;
	const UTF8Char *currPattern = searchPattern;
	UTF8Char c;
	while (true)
	{
		c = *currPattern;
		switch (c)
		{
		case 0:
			if (isWC)
			{
				if (patternStart == 0)
					return true;
				return Text::StrEndsWithC(fileName, (UOSInt)(fileNameEnd - fileName), patternStart, (UOSInt)(currPattern - patternStart));
			}
			else if (patternStart)
			{
				return Text::StrEqualsC(fileName, (UOSInt)(fileNameEnd - fileName), patternStart, (UOSInt)(currPattern - patternStart));
			}
			else
			{
				return *fileName == 0;
			}
		case '?':
		case '*':
			if (isWC)
			{
				if (patternStart == 0)
					return false;
				if ((i = Text::StrIndexOfC(fileName, (UOSInt)(fileNameEnd - fileName), patternStart, (UOSInt)(currPattern - patternStart))) == INVALID_INDEX)
					return false;
				fileName += i + currPattern - patternStart;
				patternStart = 0;
				isWC = false;
			}
			else if (patternStart)
			{
				if (!Text::StrStartsWithC(fileName, (UOSInt)(fileNameEnd - fileName), patternStart, (UOSInt)(currPattern - patternStart)))
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
			break;
		default:
			if (patternStart == 0)
			{
				patternStart = currPattern;
			}
			currPattern++;
			break;
		}
	}
}

Bool IO::Path::FilePathMatch(const UTF8Char *path, UOSInt pathLen, const UTF8Char *searchPattern, UOSInt patternLen)
{
	UOSInt i = Text::StrLastIndexOfCharC(path, pathLen, '/');
	return FileNameMatch(&path[i + 1], pathLen - i - 1, searchPattern, patternLen);
}

Bool IO::Path::FilePathMatchW(const WChar *path, const WChar *searchPattern)
{
	WChar wbuff[256];
	UOSInt i = Text::StrLastIndexOfChar(path, '/');
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
				if ((i = Text::StrIndexOf(fileName, patternStart)) == INVALID_INDEX)
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
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path, &s);
#else
	struct stat s;
	int status = lstat((const Char*)path, &s);
#endif
	if (status != 0)
		return 0;
	return (UInt64)s.st_size;
}

UInt64 IO::Path::GetFileSizeW(const WChar *path)
{
	const UTF8Char *utfPath = Text::StrToUTF8New(path);
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)utfPath, &s);
#else
	struct stat s;
	int status = lstat((const Char*)utfPath, &s);
#endif
	Text::StrDelNew(utfPath);
	if (status != 0)
		return 0;
	return (UInt64)s.st_size;
}

WChar *IO::Path::GetSystemProgramPathW(WChar *buff)
{
/////////////////////////////////
	return 0;
}

UTF8Char *IO::Path::GetLocAppDataPath(UTF8Char *buff)
{
/////////////////////////////////
	return 0;
}

WChar *IO::Path::GetLocAppDataPathW(WChar *buff)
{
/////////////////////////////////
	return 0;
}

WChar *IO::Path::GetOSPathW(WChar *buff)
{
	return Text::StrConcat(buff, L"/");
}

UTF8Char *IO::Path::GetUserHome(UTF8Char *buff)
{
	const Char *homeDir = getenv("HOME");
	if (homeDir)
	{
		return Text::StrConcat(buff, (const UTF8Char*)homeDir);
	}
	return 0;
}

Bool IO::Path::GetFileTime(const UTF8Char *path, Data::DateTime *modTime, Data::DateTime *createTime, Data::DateTime *accessTime)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path, &s);
#else
	struct stat s;
	int status = lstat((const Char*)path, &s);
#endif
	if (status != 0)
		return false;
	if (modTime)
	{
		modTime->SetUnixTimestamp(s.st_mtime);
	}
	if (createTime)
	{
		createTime->SetUnixTimestamp(s.st_ctime);
	}
	if (accessTime)
	{
		accessTime->SetUnixTimestamp(s.st_atime);
	}
	return true;
}

UTF8Char *IO::Path::GetCurrDirectory(UTF8Char *buff)
{
	Char cbuff[PATH_MAX];
	if (getcwd(cbuff, PATH_MAX) == 0)
		return 0;
	return Text::StrConcat(buff, (const UTF8Char*)cbuff);
}

WChar *IO::Path::GetCurrDirectoryW(WChar *buff)
{
	Char cbuff[PATH_MAX];
	if (getcwd(cbuff, PATH_MAX) == 0)
		return 0;
	return Text::StrUTF8_WChar(buff, (const UTF8Char*)cbuff, 0);
}

Bool IO::Path::SetCurrDirectoryW(const WChar *path)
{
	const UTF8Char *utfPath = Text::StrToUTF8New(path);
	int status = chdir((const Char*)utfPath);
	Text::StrDelNew(utfPath);
	return status == 0;
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

UTF8Char *IO::Path::GetRealPath(UTF8Char *sbuff, const UTF8Char *path, UOSInt pathLen)
{
	UTF8Char *sptr;
	if (Text::StrStartsWithC(path, pathLen, UTF8STRC("~/")))
	{
		sptr = Text::StrConcatC(IO::Path::GetUserHome(sbuff), path + 1, pathLen);
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, path, pathLen);
	}
	UTF8Char *sptr2 = sbuff;
	UOSInt i;
	while (true)
	{
		i = Text::StrIndexOfCharC(sptr2, (UOSInt)(sptr - sptr2), IO::Path::PATH_SEPERATOR);
		if (i == INVALID_INDEX)
		{
			break;
		}
		sptr2 = &sptr2[i + 1];
		if (sptr2[0] == '.' && sptr2[1] == '.' && sptr2[2] == 0)
		{
			i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr2 - sbuff - 1), IO::Path::PATH_SEPERATOR);
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
			i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr2 - sbuff - 1), IO::Path::PATH_SEPERATOR);
			if (i != INVALID_INDEX)
			{
				if (sbuff[i + 1] == '.' && sbuff[i + 2] == '.')
				{
				}
				else
				{
					sptr = Text::StrConcatC(&sbuff[i + 1], &sptr2[3], (UOSInt)(sptr - &sptr2[3]));
					sptr2 = &sbuff[i + 1];
				}
			}
		}
	}
	return sptr;
}
