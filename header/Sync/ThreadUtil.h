#ifndef _SM_SYNC_THREADUTIL
#define _SM_SYNC_THREADUTIL
#include "Data/Duration.h"
#include "Text/CString.h"
namespace Sync
{
	typedef UInt32 (__stdcall *ThreadProc)(void *userObj);
	struct ThreadHandle;

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
		static void Create(ThreadProc tProc, void *userObj);
		static void Create(ThreadProc tProc, void *userObj, UInt32 threadSize);
		static ThreadHandle *CreateWithHandle(ThreadProc tProc, void *userObj);
		static ThreadHandle *CreateWithHandle(ThreadProc tProc, void *userObj, UInt32 threadSize);
		static void CloseHandle(ThreadHandle *handle);
		static UInt32 GetThreadId(ThreadHandle *handle);
		static UInt32 GetThreadId();
		static UOSInt GetThreadCnt();
		static void SetPriority(ThreadPriority priority);
		static Bool SetName(Text::CString name);
	};
}
#endif

