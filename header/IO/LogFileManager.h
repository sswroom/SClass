#ifndef _SM_IO_LOGFILEMANAGER
#define _SM_IO_LOGFILEMANAGER
#include "Data/ArrayList.hpp"
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

		void QueryLogMonths(NN<Data::ArrayList<UInt32>> months);
		void QueryLogByMonth(NN<Data::ArrayList<UInt32>> dates, UInt32 month);
		Optional<IO::Stream> OpenLogFile(UInt32 date);

		static void WriteLogText(NN<IO::Stream> fs, NN<Text::StyledTextWriter> writer);
	};
}
#endif
