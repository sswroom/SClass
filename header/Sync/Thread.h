#ifndef _SM_SYNC_THREAD
#define _SM_SYNC_THREAD
#include "AnyType.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"
#include "Text/String.h"
namespace Sync
{
	class Thread
	{
	public:
		typedef void (CALLBACKFUNC ThreadFunc)(NN<Thread> thread);
	private:
		UInt32 running;
		UInt8 stopping;
		UInt8 waiting;
		NN<Text::String> name;
		Sync::Event evt;
		ThreadFunc func;
		AnyType userObj;

		static UInt32 __stdcall InnerThread(AnyType userObj);
	public:
		Thread(ThreadFunc func, AnyType userObj, Text::CStringNN name);
		~Thread();

		Bool Start();
		UIntOS StartMulti(UIntOS cnt);
		void BeginStop();
		void WaitForEnd();
		void Stop();
		void Wait(Data::Duration period);
		void Notify();

		Bool IsRunning() const;
		Bool IsStopping() const;
		Bool IsWaiting() const;
		AnyType GetUserObj() const;
	};
}
#endif

