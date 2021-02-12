//Require DbgHelp.lib
#ifndef _SM_MANAGE_STACKTRACER
#define _SM_MANAGE_STACKTRACER
#include "Manage/ThreadContext.h"
#include "Data/ArrayListInt64.h"

namespace Manage
{
	class StackTracer
	{
	private:
		void *hProc;
		void *hThread;
		Manage::ThreadContext *winContext;
		Manage::ThreadContext *context;
		Data::ArrayListInt64 *addrArr;
		void *stackFrame;

	public:
		StackTracer(Manage::ThreadContext *context);
		~StackTracer();

		Bool IsSupported();

		UInt64 GetCurrentAddr();
		Bool GoToNextLevel();
	};
}
#endif
