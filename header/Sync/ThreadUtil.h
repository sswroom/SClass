#ifndef _SM_SYNC_THREADUTIL
#define _SM_SYNC_THREADUTIL
#include "Data/Duration.h"
namespace Sync
{
	typedef UInt32 (__stdcall *ThreadProc)(void *userObj);
	class ThreadUtil
	{
	private:
		static UInt32 usScale;
	public:
		typedef enum
		{
			TP_IDLE,
			TP_LOWEST,
			TP_LOW,
			TP_NORMAL,
			TP_HIGH,
			TP_HIGHEST,
			TP_REALTIME
		} ThreadPriority;
	public:
		static void SleepDur(Data::Duration dur);
		static UInt32 Create(ThreadProc tProc, void *userObj);
		static UInt32 Create(ThreadProc tProc, void *userObj, UInt32 threadSize);
		static UInt32 GetThreadId();
		static UOSInt GetThreadCnt();
		static Bool EnableInterrupt();
		static Bool Interrupt(UInt32 threadId);
		static void SetPriority(ThreadPriority priority);
		static Bool SetName(const UTF8Char *name);
	};
}
#endif

