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

		static UTF8Char *GetTempFile(UTF8Char *buff, const UTF8Char *fileName);
		static WChar *GetTempFileW(WChar *buff, const WChar *fileName);
		static Bool IsDirectoryExist(const UTF8Char *dir);
		static Bool IsDirectoryExistW(const WChar *dir);
		static Bool CreateDirectory(const UTF8Char *dir);
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
		static Bool GetProcessFileName(Text::StringBuilderUTF *sb);
		static UTF8Char *ReplaceExt(UTF8Char *fileName, const UTF8Char *ext);
		static WChar *ReplaceExtW(WChar *fileName, const WChar *ext);
		static UTF8Char *GetFileExt(UTF8Char *fileBuff, const UTF8Char *path);
		static WChar *GetFileExtW(WChar *fileBuff, const WChar *path);
		static UTF8Char *AppendPath(UTF8Char *path, const UTF8Char *toAppend);
		static WChar *AppendPathW(WChar *path, const WChar *toAppend);
		static Bool AppendPath(Text::StringBuilderUTF8 *sb, const UTF8Char *toAppend);
		static FindFileSession *FindFile(const UTF8Char *path);
		static FindFileSession *FindFileW(const WChar *path);
		static UTF8Char *FindNextFile(UTF8Char *buff, FindFileSession *session, Data::DateTime *modTime, IO::Path::PathType *pt, UInt64 *fileSize);
		static WChar *FindNextFileW(WChar *buff, FindFileSession *session, Data::DateTime *modTime, IO::Path::PathType *pt, UInt64 *fileSize);
		static void FindFileClose(FindFileSession *session);
		static PathType GetPathType(const UTF8Char *path);
		static PathType GetPathTypeW(const WChar *path);
		static WChar *GetFullPathW(WChar *buff, const WChar *path);
		static Bool FileNameMatch(const UTF8Char *path, const UTF8Char *searchPattern);
		static Bool FileNameMatchW(const WChar *path, const WChar *searchPattern);
		static UInt64 GetFileSize(const UTF8Char *path);
		static UInt64 GetFileSizeW(const WChar *path);
		static WChar *GetSystemProgramPathW(WChar *buff);
		static UTF8Char *GetLocAppDataPath(UTF8Char *buff);
		static WChar *GetLocAppDataPathW(WChar *buff);
		static UTF8Char *GetOSPath(UTF8Char *buff);
		static WChar *GetOSPathW(WChar *buff);
		static UTF8Char *GetUserHome(UTF8Char *buff);
		static Bool GetFileTime(const UTF8Char *path, Data::DateTime *modTime, Data::DateTime *createTime, Data::DateTime *accessTime);
		static UTF8Char *GetCurrDirectory(UTF8Char *buff);
		static WChar *GetCurrDirectoryW(WChar *buff);
		static Bool SetCurrDirectory(const UTF8Char *path);
		static Bool SetCurrDirectoryW(const WChar *path);
		static Bool IsSearchPattern(const UTF8Char *path);
		static UTF8Char *GetRealPath(UTF8Char *sbuff, const UTF8Char *path);
	};
}
#endif
