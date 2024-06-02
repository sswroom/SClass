#ifndef _SM_IO_STRINGLOGGER
#define _SM_IO_STRINGLOGGER
#include "Data/ArrayListString.h"
#include "IO/Reader.h"
#include "IO/Writer.h"
#include "Sync/Mutex.h"

namespace IO
{
	class StringLogger
	{
	private:
		Bool modified;
		Sync::Mutex mut;
		Data::ArrayListString strList;

	public:
		StringLogger();
		~StringLogger();

		Bool IsModified();
		void ReadLogs(IO::Reader *reader);
		void LogStr(UnsafeArray<const UTF8Char> s, UOSInt len);
		void WriteLogs(IO::Writer *writer);
	};
}
#endif
