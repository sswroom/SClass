#ifndef _SM_IO_SEARCHSESS
#define _SM_IO_SEARCHSESS
#include "IO/Path.h"
#include "Data/DateTime.h"

namespace IO
{
	class SearchSess
	{
	private:
		UnsafeArray<UTF8Char> nameBuff;
		UnsafeArray<UTF8Char> nameStart;
		IO::Path::PathType pt;
		Optional<IO::Path::FindFileSession> sess;
		UInt64 fileSize;
		Data::Timestamp modTime;

	public:
		SearchSess(UnsafeArray<const UTF8Char> searchName);
		~SearchSess();

		Bool NextFile();
		UnsafeArray<const UTF8Char> GetFileName();
		UnsafeArray<const UTF8Char> GetFilePath();
		IO::Path::PathType GetPathType();
		Int64 GetFileSize();
		Data::Timestamp GetModTime();
	};
};
#endif
