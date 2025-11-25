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
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#if !defined(PATH_MAX)
#define PATH_MAX 512
#endif

struct IO::Path::FindFileSession
{
	NN<Text::String> searchPattern;
	DIR *dirObj;
	UTF8Char pathBuff[512];
	UnsafeArray<UTF8Char> pathEnd;
};

UTF8Char IO::Path::PATH_SEPERATOR = (UTF8Char)'/';
UnsafeArray<const UTF8Char> IO::Path::ALL_FILES = U8STR("*");
UOSInt IO::Path::ALL_FILES_LEN = 1;

UnsafeArray<UTF8Char> IO::Path::GetTempFile(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName, UOSInt fileNameLen)
{
	return Text::StrConcatC(Text::StrConcatC(buff, UTF8STRC("/tmp/")), fileName, fileNameLen);
}

UnsafeArray<WChar> IO::Path::GetTempFileW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName)
{
	return Text::StrConcat(Text::StrConcat(buff, L"/tmp/"), fileName);
}

Bool IO::Path::IsDirectoryExist(Text::CStringNN dir)
{
	return GetPathType(dir) == PathType::Directory;
}

Bool IO::Path::IsDirectoryExistW(UnsafeArray<const WChar> dir)
{
	return GetPathTypeW(dir) == PathType::Directory;
}

Bool IO::Path::CreateDirectory(Text::CStringNN dirInput)
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
	int status = mkdir((const Char*)dirInput.v.Ptr(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	return status == 0;
}

Bool IO::Path::CreateDirectoryW(UnsafeArray<const WChar> dirInput)
{
	UOSInt i = Text::StrLastIndexOfCharW(dirInput, '/');
	if (i != INVALID_INDEX && i > 0)
	{
		UnsafeArray<const WChar> wptr = Text::StrCopyNewC(dirInput, (UOSInt)i);
		NN<Text::String> s = Text::String::NewNotNull(wptr);
		if (GetPathType(s->ToCString()) == PathType::Unknown)
		{
			CreateDirectory(s->ToCString());
		}
		s->Release();
		Text::StrDelNew(wptr);
	}
	UnsafeArray<const UTF8Char> utfPath = Text::StrToUTF8New(dirInput);
	int status = mkdir((const Char*)utfPath.Ptr(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	Text::StrDelNew(utfPath);
	return status == 0;
}

Bool IO::Path::RemoveDirectory(UnsafeArray<const UTF8Char> dir)
{
	int status = rmdir((const Char*)dir.Ptr());
	return status == 0;
}

Bool IO::Path::RemoveDirectoryW(UnsafeArray<const WChar> dir)
{
	UnsafeArray<const UTF8Char> utfPath = Text::StrToUTF8New(dir);
	int status = rmdir((const Char*)utfPath.Ptr());
	Text::StrDelNew(utfPath);
	return status == 0;
}

Bool IO::Path::DeleteFile(UnsafeArray<const UTF8Char> fileName)
{
	int status = unlink((const Char*)fileName.Ptr());
	return status == 0;
}

Bool IO::Path::DeleteFileW(UnsafeArray<const WChar> fileName)
{
	UnsafeArray<const UTF8Char> utfPath = Text::StrToUTF8New(fileName);
	int status = unlink((const Char*)utfPath.Ptr());
	Text::StrDelNew(utfPath);
	return status == 0;
}

OSInt IO::Path::FileNameCompare(UnsafeArray<const UTF8Char> file1, UnsafeArray<const UTF8Char> file2)
{
	return Text::StrCompare(file1, file2);
}

OSInt IO::Path::FileNameCompareW(UnsafeArray<const WChar> file1, UnsafeArray<const WChar> file2)
{
	return Text::StrCompare(file1, file2);
}

UnsafeArray<UTF8Char> IO::Path::GetFileDirectory(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName)
{
	return Text::StrConcat(buff, fileName);
}

UnsafeArray<WChar> IO::Path::GetFileDirectoryW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName)
{
	return Text::StrConcat(buff, fileName);
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetProcessFileName(UnsafeArray<UTF8Char> buff)
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
#elif defined(__APPLE__)
	uint32_t size = PATH_MAX;
	if (_NSGetExecutablePath((char*)buff, &size))
	{
		size = 0;
	}
	else
	{
		size = (uint32_t)Text::StrCharCnt(buff);		
	}
#else
	ssize_t size = readlink("/proc/self/exe", (Char*)buff.Ptr(), 1024);
	if (size == -1)
		return 0;
#endif
	buff[(OSInt)size] = 0;
	return &buff[(OSInt)size];
}

UnsafeArrayOpt<WChar> IO::Path::GetProcessFileNameW(UnsafeArray<WChar> buff)
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

Bool IO::Path::GetProcessFileName(NN<Text::StringBuilderUTF8> sb)
{
	Char sbuff[512];
	ssize_t size = readlink("/proc/self/exe", sbuff, 512);
	if (size == -1)
		return 0;
	sb->AppendC((const UTF8Char*)sbuff, (UOSInt)size);
	return true;
}

UnsafeArray<UTF8Char> IO::Path::ReplaceExt(UnsafeArray<UTF8Char> fileName, UnsafeArray<const UTF8Char> ext, UOSInt extLen)
{
	UnsafeArrayOpt<UTF8Char> oldExt = 0;
	UnsafeArray<UTF8Char> nnoldExt;
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
	if (!oldExt.SetTo(nnoldExt))
	{
		nnoldExt = fileName;
		nnoldExt[-1] = '.';
	}
	return Text::StrConcatC(nnoldExt, ext, extLen);
}

UnsafeArray<WChar> IO::Path::ReplaceExtW(UnsafeArray<WChar> fileName, UnsafeArray<const WChar> ext)
{
	UnsafeArrayOpt<WChar> oldExt = 0;
	UnsafeArray<WChar> nnoldExt;
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
	if (!oldExt.SetTo(nnoldExt))
	{
		nnoldExt = fileName;
		nnoldExt[-1] = '.';
	}
	return Text::StrConcat(nnoldExt, ext);
}

UnsafeArray<UTF8Char> IO::Path::GetFileExt(UnsafeArray<UTF8Char> fileBuff, UnsafeArray<const UTF8Char> path, UOSInt pathLen)
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

UnsafeArray<WChar> IO::Path::GetFileExtW(UnsafeArray<WChar> fileBuff, UnsafeArray<const WChar> path)
{
	UOSInt i = Text::StrLastIndexOfCharW(path, '/');
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
	while (true)
	{
		if (toAppend.StartsWith(UTF8STRC("../")))
		{
			if (i != INVALID_INDEX)
			{
				path[i] = 0;
				pathLen = i;
				i = Text::StrLastIndexOfCharC(path, pathLen, '/');
			}
			toAppend = toAppend.Substring(3);
		}
		else if (toAppend.StartsWith(UTF8STRC("./")))
		{
			toAppend = toAppend.Substring(2);
		}
		else
		{
			break;
		}
	}
	path[pathLen] = '/';
	return toAppend.ConcatTo(&path[pathLen + 1]);
}

UnsafeArray<WChar> IO::Path::AppendPathW(UnsafeArray<WChar> path, UnsafeArray<const WChar> toAppend)
{
	if (toAppend[0] == '/')
		return Text::StrConcat(path, toAppend);
	UOSInt i = Text::StrLastIndexOfCharW(UnsafeArray<const WChar>(path), '/');
	if (GetPathTypeW(path) == PathType::File && i != INVALID_INDEX)
	{
		path[i] = 0;
		i = Text::StrLastIndexOfCharW(UnsafeArray<const WChar>(path), '/');
	}
	while (Text::StrStartsWith(toAppend, L"../"))
	{
		if (i != INVALID_INDEX)
		{
			path[i] = 0;
			i = Text::StrLastIndexOfCharW(UnsafeArray<const WChar>(path), '/');
		}
		toAppend += 3;
	}
	UOSInt j = Text::StrCharCnt(UnsafeArray<const WChar>(path));
	path[j] = '/';
	return Text::StrConcat(&path[j + 1], toAppend);
}

Bool IO::Path::AppendPath(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> toAppend, UOSInt toAppendLen)
{
	if (toAppend[0] == '/')
	{
		sb->ClearStr();
		sb->AppendC(toAppend, toAppendLen);
		return true;
	}
	UnsafeArray<UTF8Char> sptr = sb->v;
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

Optional<IO::Path::FindFileSession> IO::Path::FindFile(Text::CStringNN path)
{
	FindFileSession *sess = 0;
	UnsafeArray<const UTF8Char> searchPattern;
	UOSInt searchPatternLen;
	Text::CStringNN searchDir;
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
		UnsafeArray<UTF8Char> tmpBuff = MemAllocArr(UTF8Char, i + 2);
		Text::StrConcatC(tmpBuff, path.v, i);
		dirObj = opendir((const Char*)tmpBuff.Ptr());
		searchPattern = path.v + i + 1;
		searchPatternLen = path.leng - i - 1;
		searchDir = {path.v, i + 1};
		MemFreeArr(tmpBuff);
	}
	if (dirObj)
	{
		sess = MemAlloc(FindFileSession, 1);
		sess->searchPattern = Text::String::New(searchPattern, searchPatternLen);
		sess->dirObj = dirObj;
		sess->pathEnd = searchDir.ConcatTo(UARR(sess->pathBuff));
	}
	return sess;
}

Optional<IO::Path::FindFileSession> IO::Path::FindFileW(UnsafeArray<const WChar> path)
{
	FindFileSession *sess = 0;
	NN<Text::String> utfPath = Text::String::NewNotNull(path);
	Text::CStringNN searchPattern;
	Text::CStringNN searchDir;
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
		dirObj = opendir((const Char*)utfPath->v.Ptr());
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

UnsafeArrayOpt<UTF8Char> IO::Path::FindNextFile(UnsafeArray<UTF8Char> buff, NN<IO::Path::FindFileSession> sess, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize)
{
	struct dirent *ent;
	while ((ent = readdir(sess->dirObj)) != 0)
	{
		UOSInt len = Text::StrCharCntCh(ent->d_name);
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
				if (modTime.IsNotNull())
				{
#if defined(__APPLE__)
					modTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_mtimespec.tv_sec, (UInt32)s.st_mtimespec.tv_nsec, 0));
#else
					modTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_mtim.tv_sec, (UInt32)s.st_mtim.tv_nsec, 0));
#endif
				}
				if (pt.IsNotNull())
				{
					if (S_ISREG(s.st_mode))
						pt.SetNoCheck(PathType::File);
					else if (S_ISDIR(s.st_mode))
						pt.SetNoCheck(PathType::Directory);
					else
						pt.SetNoCheck(PathType::Unknown);
				}
				fileSize.Set((UInt64)s.st_size);
				return Text::StrConcatC(buff, (const UTF8Char*)ent->d_name, len);
			}
		}
		
	}
	return 0;
}

UnsafeArrayOpt<WChar> IO::Path::FindNextFileW(UnsafeArray<WChar> buff, NN<IO::Path::FindFileSession> sess, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize)
{
	struct dirent *ent;
	while ((ent = readdir(sess->dirObj)) != 0)
	{
		UOSInt len = Text::StrCharCntCh(ent->d_name);
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
				if (modTime.IsNotNull())
				{
#if defined(__APPLE__)
					modTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_mtimespec.tv_sec, (UInt32)s.st_mtimespec.tv_nsec, 0));
#else
					modTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_mtim.tv_sec, (UInt32)s.st_mtim.tv_nsec, 0));
#endif
				}
				if (pt.IsNotNull())
				{
					if (S_ISREG(s.st_mode))
						pt.SetNoCheck(PathType::File);
					else if (S_ISDIR(s.st_mode))
						pt.SetNoCheck(PathType::Directory);
					else
						pt.SetNoCheck(PathType::Unknown);
				}
				fileSize.Set((UInt64)s.st_size);
				return Text::StrUTF8_WChar(buff, (const UTF8Char*)ent->d_name, 0);
			}
		}
	}
	return 0;
}

void IO::Path::FindFileClose(NN<IO::Path::FindFileSession> sess)
{
	sess->searchPattern->Release();
	closedir(sess->dirObj);
	MemFreeNN(sess);
}

IO::Path::PathType IO::Path::GetPathType(Text::CStringNN path)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path.v.Ptr(), &s);
#else
	struct stat s;
	int status = lstat((const Char*)path.v.Ptr(), &s);
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
		UnsafeArray<UTF8Char> pathBuffEnd;
		Char cbuff[512];
		ssize_t size = readlink((const Char*)path.v.Ptr(), cbuff, 511);
		cbuff[size] = 0;
		pathBuffEnd = path.ConcatTo(pathBuff);
		UOSInt i = Text::StrLastIndexOfCharC(pathBuff, (UOSInt)(pathBuffEnd - pathBuff), '/');
		if (i != INVALID_INDEX)
		{
			pathBuff[i + 1] = 0;
			pathBuffEnd = IO::Path::AppendPath(pathBuff, &pathBuff[i + 1], Text::CStringNN((const UTF8Char*)cbuff, (UOSInt)size));
		}
		return GetPathType(CSTRP(pathBuff, pathBuffEnd));
	}
	return PathType::Unknown;
}

IO::Path::PathType IO::Path::GetPathTypeW(UnsafeArray<const WChar> path)
{
	NN<Text::String> utfPath = Text::String::NewNotNull(path);
	IO::Path::PathType pt = IO::Path::GetPathType(utfPath->ToCString());
	utfPath->Release();
	return pt;
}

Bool IO::Path::PathExists(UnsafeArray<const UTF8Char> path, UOSInt pathLen)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path.Ptr(), &s);
#else
	struct stat s;
	int status = lstat((const Char*)path.Ptr(), &s);
#endif
	return status == 0;
}

Bool IO::Path::PathExistsW(UnsafeArray<const WChar> path)
{
	NN<Text::String> utfPath = Text::String::NewNotNull(path);
	Bool ret = IO::Path::PathExists(utfPath->v, utfPath->leng);
	utfPath->Release();
	return ret;
}

UnsafeArray<WChar> IO::Path::GetFullPathW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> path)
{
	Text::StringBuilderUTF8 sb;
	NN<Text::String> str = Text::String::NewNotNull(path);
	sb.Append(str);
	str->Release();
	sb.AllocLeng(512);
	Char cbuff[512];
	UnsafeArray<UTF8Char> sptr = sb.v;
	int status;
	UOSInt i;
	UOSInt j;
//	printf("GetFullPath %ls\r\n", path);
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	status = lstat64((const Char*)sptr.Ptr(), &s);
#else
	struct stat s;
	status = lstat((const Char*)sptr.Ptr(), &s);
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
			status = lstat64((const Char*)sptr.Ptr(), &s);
#else
			status = lstat((const Char*)sptr.Ptr(), &s);
#endif
			if (status == 0)
			{
				if (S_ISLNK(s.st_mode))
				{
					ssize_t size = readlink((const Char*)sptr.Ptr(), cbuff, 511);
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
		ssize_t size = readlink((const Char*)sptr.Ptr(), cbuff, 511);
		cbuff[size] = 0;
		Text::StrConcat(buff, path);

		UnsafeArray<const WChar> wptr = Text::StrToWCharNew((const UTF8Char*)cbuff);
		UnsafeArray<WChar> ret = IO::Path::AppendPathW(buff, wptr);
		Text::StrDelNew(wptr);
		return ret;
	}
	else
	{
		////////////////////////////////
		return Text::StrConcat(buff, path);
	}
}

Bool IO::Path::FileNameMatch(UnsafeArray<const UTF8Char> fileName, UOSInt fileNameLen, UnsafeArray<const UTF8Char> searchPattern, UOSInt patternLen)
{
	UnsafeArray<const UTF8Char> fileNameEnd = &fileName[fileNameLen];
	UOSInt i;
	Bool isWC = false;
	UnsafeArrayOpt<const UTF8Char> patternStart = 0;
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
				return Text::StrEndsWithC(fileName, (UOSInt)(fileNameEnd - fileName), nnpatternStart, (UOSInt)(currPattern - nnpatternStart));
			}
			else if (patternStart.SetTo(nnpatternStart))
			{
				return Text::StrEqualsC(fileName, (UOSInt)(fileNameEnd - fileName), nnpatternStart, (UOSInt)(currPattern - nnpatternStart));
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
				if ((i = Text::StrIndexOfC(fileName, (UOSInt)(fileNameEnd - fileName), nnpatternStart, (UOSInt)(currPattern - nnpatternStart))) == INVALID_INDEX)
					return false;
				fileName += i + (UOSInt)(currPattern - nnpatternStart);
				patternStart = 0;
				isWC = false;
			}
			else if (patternStart.SetTo(nnpatternStart))
			{
				if (!Text::StrStartsWithC(fileName, (UOSInt)(fileNameEnd - fileName), nnpatternStart, (UOSInt)(currPattern - nnpatternStart)))
					return false;
				fileName += currPattern - nnpatternStart;
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
			if (patternStart.IsNull())
			{
				patternStart = currPattern;
			}
			currPattern++;
			break;
		}
	}
}

Bool IO::Path::FilePathMatch(UnsafeArray<const UTF8Char> path, UOSInt pathLen, UnsafeArray<const UTF8Char> searchPattern, UOSInt patternLen)
{
	UOSInt i = Text::StrLastIndexOfCharC(path, pathLen, '/');
	return FileNameMatch(&path[i + 1], pathLen - i - 1, searchPattern, patternLen);
}

Bool IO::Path::FilePathMatchW(UnsafeArray<const WChar> path, UnsafeArray<const WChar> searchPattern)
{
	WChar wbuff[256];
	UOSInt i = Text::StrLastIndexOfCharW(path, '/');
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
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path.Ptr(), &s);
#else
	struct stat s;
	int status = lstat((const Char*)path.Ptr(), &s);
#endif
	if (status != 0)
		return 0;
	return (UInt64)s.st_size;
}

UInt64 IO::Path::GetFileSizeW(UnsafeArray<const WChar> path)
{
	UnsafeArray<const UTF8Char> utfPath = Text::StrToUTF8New(path);
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)utfPath.Ptr(), &s);
#else
	struct stat s;
	int status = lstat((const Char*)utfPath.Ptr(), &s);
#endif
	Text::StrDelNew(utfPath);
	if (status != 0)
		return 0;
	return (UInt64)s.st_size;
}

UnsafeArrayOpt<WChar> IO::Path::GetSystemProgramPathW(UnsafeArray<WChar> buff)
{
/////////////////////////////////
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetLocAppDataPath(UnsafeArray<UTF8Char> buff)
{
/////////////////////////////////
	return 0;
}

UnsafeArrayOpt<WChar> IO::Path::GetLocAppDataPathW(UnsafeArray<WChar> buff)
{
/////////////////////////////////
	return 0;
}

UnsafeArrayOpt<WChar> IO::Path::GetOSPathW(UnsafeArray<WChar> buff)
{
	return Text::StrConcat(buff, L"/");
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetUserHome(UnsafeArray<UTF8Char> buff)
{
	const Char *homeDir = getenv("HOME");
	if (homeDir)
	{
		return Text::StrConcat(buff, (const UTF8Char*)homeDir);
	}
	return 0;
}

Bool IO::Path::GetFileTime(Text::CStringNN path, OptOut<Data::Timestamp> modTime, OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> accessTime)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path.v.Ptr(), &s);
#else
	struct stat s;
	int status = lstat((const Char*)path.v.Ptr(), &s);
#endif
	if (status != 0)
		return false;
#if defined(__APPLE__)
	if (modTime.IsNotNull())
	{
		modTime.SetNoCheck(Data::Timestamp(Data::TimeInstant(s.st_mtimespec.tv_sec, (UInt32)s.st_mtimespec.tv_nsec), Data::DateTimeUtil::GetLocalTzQhr()));
	}
	if (createTime.IsNotNull())
	{
		createTime.SetNoCheck(Data::Timestamp(Data::TimeInstant(s.st_ctimespec.tv_sec, (UInt32)s.st_ctimespec.tv_nsec), Data::DateTimeUtil::GetLocalTzQhr()));
	}
	if (accessTime.IsNotNull())
	{
		accessTime.SetNoCheck(Data::Timestamp(Data::TimeInstant(s.st_atimespec.tv_sec, (UInt32)s.st_atimespec.tv_nsec), Data::DateTimeUtil::GetLocalTzQhr()));
	}
#else
	if (modTime.IsNotNull())
	{
		modTime.SetNoCheck(Data::Timestamp(Data::TimeInstant(s.st_mtim.tv_sec, (UInt32)s.st_mtim.tv_nsec), Data::DateTimeUtil::GetLocalTzQhr()));
	}
	if (createTime.IsNotNull())
	{
		createTime.SetNoCheck(Data::Timestamp(Data::TimeInstant(s.st_ctim.tv_sec, (UInt32)s.st_ctim.tv_nsec), Data::DateTimeUtil::GetLocalTzQhr()));
	}
	if (accessTime.IsNotNull())
	{
		accessTime.SetNoCheck(Data::Timestamp(Data::TimeInstant(s.st_atim.tv_sec, (UInt32)s.st_atim.tv_nsec), Data::DateTimeUtil::GetLocalTzQhr()));
	}
#endif
	return true;
}

Data::Timestamp IO::Path::GetModifyTime(UnsafeArray<const UTF8Char> path)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path.Ptr(), &s);
#else
	struct stat s;
	int status = lstat((const Char*)path.Ptr(), &s);
#endif
	if (status != 0)
		return Data::Timestamp(0);
#if defined(__APPLE__)
	return Data::Timestamp::FromSecNS(s.st_mtimespec.tv_sec, (UInt32)s.st_mtimespec.tv_nsec, Data::DateTimeUtil::GetLocalTzQhr());
#else
	return Data::Timestamp::FromSecNS(s.st_mtim.tv_sec, (UInt32)s.st_mtim.tv_nsec, Data::DateTimeUtil::GetLocalTzQhr());
#endif
}

UInt32 IO::Path::GetFileUnixAttr(Text::CStringNN path)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	int status = lstat64((const Char*)path.v.Ptr(), &s);
#else
	struct stat s;
	int status = lstat((const Char*)path.v.Ptr(), &s);
#endif
	if (status != 0)
		return 0;
	return s.st_mode;
}

UnsafeArrayOpt<UTF8Char> IO::Path::GetCurrDirectory(UnsafeArray<UTF8Char> buff)
{
	Char cbuff[PATH_MAX];
	if (getcwd(cbuff, PATH_MAX) == 0)
		return 0;
	return Text::StrConcat(buff, (const UTF8Char*)cbuff);
}

UnsafeArrayOpt<WChar> IO::Path::GetCurrDirectoryW(UnsafeArray<WChar> buff)
{
	Char cbuff[PATH_MAX];
	if (getcwd(cbuff, PATH_MAX) == 0)
		return 0;
	return Text::StrUTF8_WChar(buff, (const UTF8Char*)cbuff, 0);
}

Bool IO::Path::SetCurrDirectoryW(UnsafeArray<const WChar> path)
{
	UnsafeArray<const UTF8Char> utfPath = Text::StrToUTF8New(path);
	int status = chdir((const Char*)utfPath.Ptr());
	Text::StrDelNew(utfPath);
	return status == 0;
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

UnsafeArray<UTF8Char> IO::Path::GetRealPath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> path, UOSInt pathLen)
{
	UnsafeArray<UTF8Char> sptr;
	if (Text::StrStartsWithC(path, pathLen, UTF8STRC("~/")))
	{
		if (!IO::Path::GetUserHome(sbuff).SetTo(sptr))
			sptr = sbuff;
		sptr = Text::StrConcatC(sptr, path + 1, pathLen - 1);
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, path, pathLen);
	}
	UnsafeArray<UTF8Char> sptr2 = sbuff;
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
