#ifndef _SM_SYNC_THREAD
#define _SM_SYNC_THREAD
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"
#include "Text/String.h"
namespace Sync
{
	class Thread
	{
	public:
		typedef void (__stdcall *ThreadFunc)(NotNullPtr<Thread> thread);
	private:
		UInt32 running;
		UInt8 stopping;
		UInt8 waiting;
		NotNullPtr<Text::String> name;
		Sync::Event evt;
		ThreadFunc func;
		void *userObj;

		static UInt32 __stdcall InnerThread(void *userObj);
	public:
		Thread(ThreadFunc func, void *userObj, Text::CStringNN name);
		~Thread();

		Bool Start();
		UOSInt StartMulti(UOSInt cnt);
		void BeginStop();
		void WaitForEnd();
		void Stop();
		void Wait(Data::Duration period);
		void Notify();

		Bool IsRunning() const;
		Bool IsStopping() const;
		Bool IsWaiting() const;
		void *GetUserObj() const;
	};
}
#endif

