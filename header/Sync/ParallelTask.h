#ifndef _SM_SYNC_PARALLELTASK
#define _SM_SYNC_PARALLELTASK
#include "Sync/Event.h"

namespace Sync
{
	class ParallelTask
	{
	public:
		typedef void (__stdcall *TaskFunc)(void *taskObj);
	private:
		typedef struct
		{
			ParallelTask *me;
			Sync::Event *evt;
			Bool running;
			Bool toStop;
			TaskFunc currTaskFunc;
			void *currTaskObj;
		} ThreadStatus;

		Sync::Event mainEvt;
		UOSInt threadCnt;
		ThreadStatus *stats;

		static UInt32 __stdcall WorkerThread(void *userObj);
	public:
		ParallelTask(UOSInt threadCnt, Bool taskQueue);
		~ParallelTask();

		UOSInt GetThreadCnt();
		void AddTask(TaskFunc func, void *taskObj);
		void WaitForIdle();
	};
}
#endif

