#ifndef _SM_IO_PATH
#define _SM_IO_PATH
#include "Data/DateTime.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class Path
	{
	public:
		enum class PathType
		{
			Unknown,
			File,
			Directory
		};
		struct FindFileSession;

		static UTF8Char PATH_SEPERATOR;
		static const UTF8Char *ALL_FILES;
		static UOSInt ALL_FILES_LEN;

		static UTF8Char *GetTempFile(UTF8Char *buff, const UTF8Char *fileName, UOSInt fileNameLen);
		static WChar *GetTempFileW(WChar *buff, const WChar *fileName);
		static Bool IsDirectoryExist(Text::CString dir);
		static Bool IsDirectoryExistW(const WChar *dir);
		static Bool CreateDirectory(Text::CString dir);
		static Bool CreateDirectoryW(const WChar *dir);
		static Bool RemoveDirectory(const UTF8Char *dir);
		static Bool RemoveDirectoryW(const WChar *dir);
		static Bool DeleteFile(const UTF8Char *fileName);
		static Bool DeleteFileW(const WChar *fileName);
		static OSInt FileNameCompare(const UTF8Char *file1, const UTF8Char *file2);
		static OSInt FileNameCompareW(const WChar *file1, const WChar *file2);
		static UTF8Char *GetFileDirectory(UTF8Char *buff, const UTF8Char *fileName);
		static WChar *GetFileDirectoryW(WChar *buff, const WChar *fileName);
		static UTF8Char *GetProcessFileName(UTF8Char *buff);
		static WChar *GetProcessFileNameW(WChar *buff);
		static Bool GetProcessFileName(Text::StringBuilderUTF8 *sb);
		static UTF8Char *ReplaceExt(UTF8Char *fileName, const UTF8Char *ext, UOSInt extLen);
		static WChar *ReplaceExtW(WChar *fileName, const WChar *ext);
		static UTF8Char *GetFileExt(UTF8Char *fileBuff, const UTF8Char *path, UOSInt pathLen);
		static WChar *GetFileExtW(WChar *fileBuff, const WChar *path);
		static UTF8Char *AppendPath(UTF8Char *path, UTF8Char *pathEnd, Text::CString toAppend);
		static WChar *AppendPathW(WChar *path, const WChar *toAppend);
		static Bool AppendPath(Text::StringBuilderUTF8 *sb, const UTF8Char *toAppend, UOSInt toAppendLen);
		static FindFileSession *FindFile(Text::CString path);
		static FindFileSession *FindFileW(const WChar *path);
		static UTF8Char *FindNextFile(UTF8Char *buff, FindFileSession *session, Data::Timestamp *modTime, IO::Path::PathType *pt, UInt64 *fileSize);
		static WChar *FindNextFileW(WChar *buff, FindFileSession *session, Data::Timestamp *modTime, IO::Path::PathType *pt, UInt64 *fileSize);
		static void FindFileClose(FindFileSession *session);
		static PathType GetPathType(Text::CString path);
		static PathType GetPathTypeW(const WChar *path);
		static Bool PathExists(const UTF8Char *path, UOSInt pathLen);
		static Bool PathExistsW(const WChar *path);
		static WChar *GetFullPathW(WChar *buff, const WChar *path);
	private:
		static Bool FileNameMatch(const UTF8Char *fileName, UOSInt fileNameLen, const UTF8Char *searchPattern, UOSInt patternLen);
	public:
		static Bool FilePathMatch(const UTF8Char *path, UOSInt pathLen, const UTF8Char *searchPattern, UOSInt patternLen);
		static Bool FilePathMatchW(const WChar *path, const WChar *searchPattern);
		static UInt64 GetFileSize(const UTF8Char *path);
		static UInt64 GetFileSizeW(const WChar *path);
		static WChar *GetSystemProgramPathW(WChar *buff);
		static UTF8Char *GetLocAppDataPath(UTF8Char *buff);
		static WChar *GetLocAppDataPathW(WChar *buff);
		static UTF8Char *GetOSPath(UTF8Char *buff);
		static WChar *GetOSPathW(WChar *buff);
		static UTF8Char *GetUserHome(UTF8Char *buff);
		static Bool GetFileTime(const UTF8Char *path, Data::DateTime *modTime, Data::DateTime *createTime, Data::DateTime *accessTime);
		static Data::Timestamp GetModifyTime(const UTF8Char *path);
		static UTF8Char *GetCurrDirectory(UTF8Char *buff);
		static WChar *GetCurrDirectoryW(WChar *buff);
		static Bool SetCurrDirectory(const UTF8Char *path);
		static Bool SetCurrDirectoryW(const WChar *path);
		static Bool IsSearchPattern(const UTF8Char *path);
		static UTF8Char *GetRealPath(UTF8Char *sbuff, const UTF8Char *path, UOSInt pathLen);
	};
}
#endif
