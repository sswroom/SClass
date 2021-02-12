#ifndef _SM_IO_STRINGLOGGER
#define _SM_IO_STRINGLOGGER
#include "Data/ArrayListStrUTF8.h"
#include "IO/Reader.h"
#include "IO/Writer.h"
#include "Sync/Mutex.h"

namespace IO
{
	class StringLogger
	{
	private:
		Bool modified;
		Sync::Mutex *mut;
		Data::ArrayListStrUTF8 *strList;

	public:
		StringLogger();
		~StringLogger();

		Bool IsModified();
		void ReadLogs(IO::Reader *reader);
		void LogStr(const UTF8Char *s);
		void WriteLogs(IO::Writer *writer);
	};
}
#endif
