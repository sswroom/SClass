#ifndef _SM_IO_LOGFILEMANAGER
#define _SM_IO_LOGFILEMANAGER
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Text/String.h"
#include "Text/StyledTextWriter.h"

namespace IO
{
	class LogFileManager
	{
	private:
		NN<Text::String> logPath;
	public:
		LogFileManager(NN<Text::String> logPath);
		~LogFileManager();

		void QueryLogMonths(Data::ArrayList<UInt32> *months);
		void QueryLogByMonth(Data::ArrayList<UInt32> *dates, UInt32 month);
		IO::Stream *OpenLogFile(UInt32 date);

		static void WriteLogText(NN<IO::Stream> fs, Text::StyledTextWriter *writer);
	};
}
#endif
