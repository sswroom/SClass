#ifndef _SM_NET_SPRING_SPRINGBOOTAPPLICATION
#define _SM_NET_SPRING_SPRINGBOOTAPPLICATION
#include "IO/ConfigFile.h"
#include "IO/ConsoleLogHandler.h"
#include "IO/ConsoleWriter.h"
#include "IO/LogTool.h"

namespace Net
{
	namespace Spring
	{
		class SpringBootApplication
		{
		private:
			IO::ConfigFile *cfg;
			IO::ConsoleWriter console;
			IO::ConsoleLogHandler consoleLog;
			IO::LogTool log;
			NotNullPtr<Text::String> activeProfile;
		public:
			SpringBootApplication(Text::CString appName);
			virtual ~SpringBootApplication();
		};
	}
}
#endif
