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
		static UIntOS ALL_FILES_LEN;

		static UnsafeArray<UTF8Char> GetTempFile(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName, UIntOS fileNameLen);
		static UnsafeArray<WChar> GetTempFileW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName);
		static Bool IsDirectoryExist(Text::CStringNN dir);
		static Bool IsDirectoryExistW(UnsafeArray<const WChar> dir);
		static Bool CreateDirectory(Text::CStringNN dir);
		static Bool CreateDirectoryW(UnsafeArray<const WChar> dir);
		static Bool RemoveDirectory(UnsafeArray<const UTF8Char> dir);
		static Bool RemoveDirectoryW(UnsafeArray<const WChar> dir);
		static Bool DeleteFile(UnsafeArray<const UTF8Char> fileName);
		static Bool DeleteFileW(UnsafeArray<const WChar> fileName);
		static IntOS FileNameCompare(UnsafeArray<const UTF8Char> file1, UnsafeArray<const UTF8Char> file2);
		static IntOS FileNameCompareW(UnsafeArray<const WChar> file1, UnsafeArray<const WChar> file2);
		static UnsafeArray<UTF8Char> GetFileDirectory(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName);
		static UnsafeArray<WChar> GetFileDirectoryW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> fileName);
		static UnsafeArrayOpt<UTF8Char> GetProcessFileName(UnsafeArray<UTF8Char> buff);
		static UnsafeArrayOpt<WChar> GetProcessFileNameW(UnsafeArray<WChar> buff);
		static Bool GetProcessFileName(NN<Text::StringBuilderUTF8> sb);
		static UnsafeArray<UTF8Char> ReplaceExt(UnsafeArray<UTF8Char> fileName, UnsafeArray<const UTF8Char> ext, UIntOS extLen);
		static UnsafeArray<WChar> ReplaceExtW(UnsafeArray<WChar> fileName, UnsafeArray<const WChar> ext);
		static UnsafeArray<UTF8Char> GetFileExt(UnsafeArray<UTF8Char> fileBuff, UnsafeArray<const UTF8Char> path, UIntOS pathLen);
		static UnsafeArray<WChar> GetFileExtW(UnsafeArray<WChar> fileBuff, UnsafeArray<const WChar> path);
		static UnsafeArray<UTF8Char> AppendPath(UnsafeArray<UTF8Char> path, UnsafeArray<UTF8Char> pathEnd, Text::CStringNN toAppend);
		static UnsafeArray<WChar> AppendPathW(UnsafeArray<WChar> path, UnsafeArray<const WChar> toAppend);
		static Bool AppendPath(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> toAppend, UIntOS toAppendLen);
		static Optional<FindFileSession> FindFile(Text::CStringNN path);
		static Optional<FindFileSession> FindFileW(UnsafeArray<const WChar> path);
		static UnsafeArrayOpt<UTF8Char> FindNextFile(UnsafeArray<UTF8Char> buff, NN<FindFileSession> session, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize);
		static UnsafeArrayOpt<WChar> FindNextFileW(UnsafeArray<WChar> buff, NN<FindFileSession> session, OptOut<Data::Timestamp> modTime, OptOut<IO::Path::PathType> pt, OptOut<UInt64> fileSize);
		static void FindFileClose(NN<FindFileSession> session);
		static PathType GetPathType(Text::CStringNN path);
		static PathType GetPathTypeW(UnsafeArray<const WChar> path);
		static Bool PathExists(UnsafeArray<const UTF8Char> path, UIntOS pathLen);
		static Bool PathExistsW(UnsafeArray<const WChar> path);
		static UnsafeArray<WChar> GetFullPathW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> path);
	private:
		static Bool FileNameMatch(UnsafeArray<const UTF8Char> fileName, UIntOS fileNameLen, UnsafeArray<const UTF8Char> searchPattern, UIntOS patternLen);
	public:
		static Bool FilePathMatch(UnsafeArray<const UTF8Char> path, UIntOS pathLen, UnsafeArray<const UTF8Char> searchPattern, UIntOS patternLen);
		static Bool FilePathMatchW(UnsafeArray<const WChar> path, UnsafeArray<const WChar> searchPattern);
		static UInt64 GetFileSize(UnsafeArray<const UTF8Char> path);
		static UInt64 GetFileSizeW(UnsafeArray<const WChar> path);
		static UnsafeArrayOpt<WChar> GetSystemProgramPathW(UnsafeArray<WChar> buff);
		static UnsafeArrayOpt<UTF8Char> GetLocAppDataPath(UnsafeArray<UTF8Char> buff);
		static UnsafeArrayOpt<WChar> GetLocAppDataPathW(UnsafeArray<WChar> buff);
		static UnsafeArrayOpt<UTF8Char> GetOSPath(UnsafeArray<UTF8Char> buff);
		static UnsafeArrayOpt<WChar> GetOSPathW(UnsafeArray<WChar> buff);
		static UnsafeArrayOpt<UTF8Char> GetUserHome(UnsafeArray<UTF8Char> buff);
		static Bool GetFileTime(Text::CStringNN path, OptOut<Data::Timestamp> modTime, OptOut<Data::Timestamp> createTime, OptOut<Data::Timestamp> accessTime);
		static Data::Timestamp GetModifyTime(UnsafeArray<const UTF8Char> path);
		static UInt32 GetFileUnixAttr(Text::CStringNN path);
		static UnsafeArrayOpt<UTF8Char> GetCurrDirectory(UnsafeArray<UTF8Char> buff);
		static UnsafeArrayOpt<WChar> GetCurrDirectoryW(UnsafeArray<WChar> buff);
		static Bool SetCurrDirectory(UnsafeArray<const UTF8Char> path);
		static Bool SetCurrDirectoryW(UnsafeArray<const WChar> path);
		static Bool IsSearchPattern(UnsafeArray<const UTF8Char> path);
		static UnsafeArray<UTF8Char> GetRealPath(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> path, UIntOS pathLen);
	};
}
#endif
