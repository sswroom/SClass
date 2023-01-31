#ifndef _SM_IO_LOGFILEMANAGER
#define _SM_IO_LOGFILEMANAGER
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace IO
{
	class LogFileManager
	{
	private:
		Text::String *logPath;
	public:
		LogFileManager(Text::String *logPath);
		~LogFileManager();

		void QueryLogMonths(Data::ArrayList<UInt32> *months);
	};
}
#endif
