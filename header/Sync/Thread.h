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
		Bool running;
		Bool stopping;
		NotNullPtr<Text::String> name;
		Sync::ThreadHandle *hand;
		Sync::Event evt;
		ThreadFunc func;
		void *userObj;

		static UInt32 __stdcall InnerThread(void *userObj);
	public:
		Thread(ThreadFunc func, void *userObj, Text::CStringNN name);
		~Thread();

		Bool Start();
		void BeginStop();
		void WaitForEnd();
		void Stop();
		void Wait(Data::Duration period);
		void Notify();

		Bool IsRunning() const;
		Bool IsStopping() const;
		void *GetUserObj() const;
	};
}
#endif

