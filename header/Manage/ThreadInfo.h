#ifndef _SM_MANAGE_THREADINFO
#define _SM_MANAGE_THREADINFO
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadInfo
	{
	private:
		UOSInt threadId;
		UOSInt procId;
		void *hand;

	private:
		static Manage::ThreadContext *GetThreadContextHand(UOSInt threadId, UOSInt procId, void *hand);
		ThreadInfo(UOSInt procId, UOSInt threadId, void *hand);
	public:
		ThreadInfo(UOSInt procId, UOSInt threadId);
		~ThreadInfo();

		Manage::ThreadContext *GetThreadContext();
		UInt64 GetStartAddress();
		Bool WaitForThreadExit(UInt32 waitTimeout); //true = exited
		UInt32 GetExitCode();
		UOSInt GetThreadId();
		Bool Suspend();
		Bool Resume();
		Bool IsCurrThread();
		static ThreadInfo *GetCurrThread();
	};
};
#endif
