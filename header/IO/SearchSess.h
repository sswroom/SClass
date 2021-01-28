#ifndef _SM_IO_SEARCHSESS
#define _SM_IO_SEARCHSESS
#include "IO/Path.h"
#include "Data/DateTime.h"

namespace IO
{
	class SearchSess
	{
	private:
		UTF8Char *nameBuff;
		UTF8Char *nameStart;
		IO::Path::PathType pt;
		void *sess;
		Int64 fileSize;
		Data::DateTime *modTime;

	public:
		SearchSess(const UTF8Char *searchName);
		~SearchSess();

		Bool NextFile();
		const UTF8Char *GetFileName();
		const UTF8Char *GetFilePath();
		IO::Path::PathType GetPathType();
		Int64 GetFileSize();
		Data::DateTime *GetModTime();
	};
};
#endif
