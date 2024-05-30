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
		static UnsafeArray<const UTF8Char> ALL_FILES;
		static UOSInt ALL_FILES_LEN;

		static UnsafeArray<UTF8Char> GetTempFile(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName, UOSInt fileNameLen);
		static WChar *GetTempFileW(WChar *buff, const WChar *fileName);
		static Bool IsDirectoryExist(Text::CStringNN dir);
		static Bool IsDirectoryExistW(const WChar *dir);
		static Bool CreateDirectory(Text::CStringNN dir);
		static Bool CreateDirectoryW(const WChar *dir);
		static Bool RemoveDirectory(UnsafeArray<const UTF8Char> dir);
		static Bool RemoveDirectoryW(const WChar *dir);
		static Bool DeleteFile(UnsafeArray<const UTF8Char> fileName);
		static Bool DeleteFileW(const WChar *fileName);
		static OSInt FileNameCompare(UnsafeArray<const UTF8Char> file1, UnsafeArray<const UTF8Char> file2);
		static OSInt FileNameCompareW(const WChar *file1, const WChar *file2);
		static UnsafeArray<UTF8Char> GetFileDirectory(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName);
		static WChar *GetFileDirectoryW(WChar *buff, const WChar *fileName);
		static UnsafeArrayOpt<UTF8Char> GetProcessFileName(UnsafeArray<UTF8Char> buff);
		static WChar *GetProcessFileNameW(WChar *buff);
		static Bool GetProcessFileName(NN<Text::StringBuilderUTF8> sb);
		static UnsafeArray<UTF8Char> ReplaceExt(UnsafeArray<UTF8Char> fileName, UnsafeArray<const UTF8Char> ext, UOSInt extLen);
		static WChar *ReplaceExtW(WChar *fileName, const WChar *ext);
		static UnsafeArray<UTF8Char> GetFileExt(UnsafeArray<UTF8Char> fileBuff, UnsafeArray<const UTF8Char> path, UOSInt pathLen);
		static WChar *GetFileExtW(WChar *fileBuff, const WChar *path);
		static UnsafeArray<UTF8Char> AppendPath(UnsafeArray<UTF8Char> path, UnsafeArray<UTF8Char> pathEnd, Text::CStringNN toAppend);
		static WChar *AppendPathW(WChar *path, const WChar *toAppend);
		static Bool AppendPath(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> toAppend, UOSInt toAppendLen);
		static FindFileSession *FindFile(Text::CStringNN path);
		static FindFileSession *FindFileW(const WChar *path);
		static UnsafeArrayOpt<UTF8Char> FindNextFile(UnsafeArray<UTF8Char> buff, FindFileSession *session, Data::Timestamp *modTime, IO::Path::PathType *pt, UInt64 *fileSize);
		static WChar *FindNextFileW(WChar *buff, FindFileSession *session, Data::Timestamp *modTime, IO::Path::PathType *pt, UInt64 *fileSize);
		static void FindFileClose(FindFileSession *session);
		static PathType GetPathType(Text::CStringNN path);
		static PathType GetPathTypeW(const WChar *path);
		static Bool PathExists(UnsafeArray<const UTF8Char> path, UOSInt pathLen);
		static Bool PathExistsW(const WChar *path);
		static WChar *GetFullPathW(WChar *buff, const WChar *path);
	private:
		static Bool FileNameMatch(UnsafeArray<const UTF8Char> fileName, UOSInt fileNameLen, UnsafeArray<const UTF8Char> searchPattern, UOSInt patternLen);
	public:
		static Bool FilePathMatch(UnsafeArray<const UTF8Char> path, UOSInt pathLen, UnsafeArray<const UTF8Char> searchPattern, UOSInt patternLen);
		static Bool FilePathMatchW(const WChar *path, const WChar *searchPattern);
		static UInt64 GetFileSize(UnsafeArray<const UTF8Char> path);
		static UInt64 GetFileSizeW(const WChar *path);
		static WChar *GetSystemProgramPathW(WChar *buff);
		static UnsafeArrayOpt<UTF8Char> GetLocAppDataPath(UnsafeArray<UTF8Char> buff);
		static WChar *GetLocAppDataPathW(WChar *buff);
		static UnsafeArray<UTF8Char> GetOSPath(UnsafeArray<UTF8Char> buff);
		static WChar *GetOSPathW(WChar *buff);
		static UnsafeArrayOpt<UTF8Char> GetUserHome(UnsafeArray<UTF8Char> buff);
		static Bool GetFileTime(Text::CStringNN path, OptOut<Data::Timestamp> modTime, OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> accessTime);
		static Data::Timestamp GetModifyTime(UnsafeArray<const UTF8Char> path);
		static UInt32 GetFileUnixAttr(Text::CStringNN path);
		static UnsafeArrayOpt<UTF8Char> GetCurrDirectory(UnsafeArray<UTF8Char> buff);
		static WChar *GetCurrDirectoryW(WChar *buff);
		static Bool SetCurrDirectory(UnsafeArray<const UTF8Char> path);
		static Bool SetCurrDirectoryW(const WChar *path);
		static Bool IsSearchPattern(UnsafeArray<const UTF8Char> path);
		static UnsafeArray<UTF8Char> GetRealPath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> path, UOSInt pathLen);
	};
}
#endif
