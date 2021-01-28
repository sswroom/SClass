#ifndef _SM_SYNC_THREAD
#define _SM_SYNC_THREAD
namespace Sync
{
	typedef UInt32 (__stdcall *ThreadProc)(void *userObj);
	class Thread
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
		static void Sleep(UOSInt ms);
		static void Sleepus(UOSInt us);
		static UInt32 Create(ThreadProc tProc, void *userObj);
		static UInt32 Create(ThreadProc tProc, void *userObj, UInt32 threadSize);
		static UInt32 GetThreadId();
		static UOSInt GetThreadCnt();
		static void SetPriority(ThreadPriority priority);
	};
}
#endif

