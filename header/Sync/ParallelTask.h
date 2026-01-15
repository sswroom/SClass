#ifndef _SM_SYNC_PARALLELTASK
#define _SM_SYNC_PARALLELTASK
#include "AnyType.h"
#include "Sync/Event.h"

namespace Sync
{
	class ParallelTask
	{
	public:
		typedef void (CALLBACKFUNC TaskFunc)(AnyType taskObj);
	private:
		typedef struct
		{
			ParallelTask *me;
			UIntOS index;
			Sync::Event *evt;
			Bool running;
			Bool toStop;
			TaskFunc currTaskFunc;
			AnyType currTaskObj;
		} ThreadStatus;

		Sync::Event mainEvt;
		UIntOS threadCnt;
		ThreadStatus *stats;

		static UInt32 __stdcall WorkerThread(AnyType userObj);
	public:
		ParallelTask(UIntOS threadCnt, Bool taskQueue);
		~ParallelTask();

		UIntOS GetThreadCnt();
		void AddTask(TaskFunc func, AnyType taskObj);
		void WaitForIdle();
	};
}
#endif

