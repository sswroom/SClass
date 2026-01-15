#ifndef _SM_MANAGE_THREADINFO
#define _SM_MANAGE_THREADINFO
#include "Manage/ThreadContext.h"
#include "Sync/ThreadUtil.h"

namespace Manage
{
	class ThreadInfo
	{
	private:
		UIntOS threadId;
		UIntOS procId;
		Sync::ThreadHandle *hand;

	private:
		static Manage::ThreadContext *GetThreadContextHand(UIntOS threadId, UIntOS procId, Sync::ThreadHandle *hand);
		ThreadInfo(UIntOS procId, UIntOS threadId, Sync::ThreadHandle *hand);
	public:
		ThreadInfo(UIntOS procId, UIntOS threadId);
		~ThreadInfo();

		Manage::ThreadContext *GetThreadContext();
		UInt64 GetStartAddress();
		Bool WaitForThreadExit(UInt32 waitTimeout); //true = exited
		UInt32 GetExitCode();
		UIntOS GetThreadId();
		UnsafeArrayOpt<UTF8Char> GetName(UnsafeArray<UTF8Char> buff);
		Bool Suspend();
		Bool Resume();
		Bool IsCurrThread();
		static ThreadInfo *GetCurrThread();
	};
};
#endif
