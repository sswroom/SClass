#ifndef _SM_MANAGE_THREADINFO
#define _SM_MANAGE_THREADINFO
#include "Manage/ThreadContext.h"
#include "Sync/ThreadUtil.h"

namespace Manage
{
	class ThreadInfo
	{
	private:
		UOSInt threadId;
		UOSInt procId;
		Sync::ThreadHandle *hand;

	private:
		static Manage::ThreadContext *GetThreadContextHand(UOSInt threadId, UOSInt procId, Sync::ThreadHandle *hand);
		ThreadInfo(UOSInt procId, UOSInt threadId, Sync::ThreadHandle *hand);
	public:
		ThreadInfo(UOSInt procId, UOSInt threadId);
		~ThreadInfo();

		Manage::ThreadContext *GetThreadContext();
		UInt64 GetStartAddress();
		Bool WaitForThreadExit(UInt32 waitTimeout); //true = exited
		UInt32 GetExitCode();
		UOSInt GetThreadId();
		UnsafeArrayOpt<UTF8Char> GetName(UnsafeArray<UTF8Char> buff);
		Bool Suspend();
		Bool Resume();
		Bool IsCurrThread();
		static ThreadInfo *GetCurrThread();
	};
};
#endif
