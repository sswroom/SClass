#ifndef _SM_IO_STRINGLOGGER
#define _SM_IO_STRINGLOGGER
#include "Data/ArrayListStringNN.h"
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
		Data::ArrayListStringNN strList;

	public:
		StringLogger();
		~StringLogger();

		Bool IsModified();
		void ReadLogs(NN<IO::Reader> reader);
		void LogStr(UnsafeArray<const UTF8Char> s, UIntOS len);
		void WriteLogs(NN<IO::Writer> writer);
	};
}
#endif
