#ifndef _SM_MANAGE_THREADINFO
#define _SM_MANAGE_THREADINFO
#include "Manage/ThreadContext.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"

namespace Manage
{
	class ThreadInfo
	{
	public:
		typedef void (CALLBACKFUNC ContextHandler)(NN<Manage::ThreadContext> context, AnyType userObj) ;
	private:
		static Optional<ThreadInfo> me;
		UIntOS threadId;
		UIntOS procId;
		Optional<Sync::ThreadHandle> hand;
		AnyType userObj;
		ContextHandler hdlr;
		Optional<Sync::Event> evt;

	private:
		static Optional<Manage::ThreadContext> GetThreadContextHand(UIntOS threadId, UIntOS procId, Optional<Sync::ThreadHandle> hand);
		ThreadInfo(UIntOS procId, UIntOS threadId, Optional<Sync::ThreadHandle> hand);
	public:
		ThreadInfo(UIntOS procId, UIntOS threadId);
		~ThreadInfo();

		Bool GetThreadContext(ContextHandler hdlr, AnyType userObj);
		UInt64 GetStartAddress();
		Bool WaitForThreadExit(UInt32 waitTimeout); //true = exited
		UInt32 GetExitCode();
		UIntOS GetThreadId();
		UnsafeArrayOpt<UTF8Char> GetName(UnsafeArray<UTF8Char> buff);
		Bool Suspend();
		Bool Resume();
		Bool IsCurrThread();
		static void OnContext(NN<Manage::ThreadContext> context, UIntOS threadId);
		static Optional<ThreadInfo> GetCurrThread();
	};
};
#endif
