#ifndef _SM_CORE_CONSOLECONTROL
#define _SM_CORE_CONSOLECONTROL
#include "Core/Core.h"
#include "Sync/Event.h"

namespace Core
{
	struct ConsoleControl : public Core::ProgControl
	{
		NN<Sync::Event> evt;
		Bool exited;
		Bool ending;
		Bool ended;

		UIntOS argc;
		UnsafeArrayOpt<UnsafeArray<UTF8Char>> argv;
	};		
}
#endif