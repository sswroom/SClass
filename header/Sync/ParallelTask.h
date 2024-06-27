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
			UOSInt index;
			Sync::Event *evt;
			Bool running;
			Bool toStop;
			TaskFunc currTaskFunc;
			AnyType currTaskObj;
		} ThreadStatus;

		Sync::Event mainEvt;
		UOSInt threadCnt;
		ThreadStatus *stats;

		static UInt32 __stdcall WorkerThread(AnyType userObj);
	public:
		ParallelTask(UOSInt threadCnt, Bool taskQueue);
		~ParallelTask();

		UOSInt GetThreadCnt();
		void AddTask(TaskFunc func, AnyType taskObj);
		void WaitForIdle();
	};
}
#endif

