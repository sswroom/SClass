#ifndef _SM_CORE_CONSOLECONTROL
#define _SM_CORE_CONSOLECONTROL
#include "Core/Core.h"
#include "Sync/Event.h"

namespace Core
{
	struct ConsoleControl : public Core::IProgControl
	{
		Sync::Event *evt;
		Bool exited;
		Bool ending;
		Bool ended;

		UOSInt argc;
		UTF8Char **argv;
	};		
}
#endif