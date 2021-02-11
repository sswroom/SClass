#ifndef _SM_SYNC_MUTEXUSAGE
#define _SM_SYNC_MUTEXUSAGE
#include "Sync/Mutex.h"

namespace Sync
{
	class MutexUsage
	{
	private:
		Sync::Mutex *mut;
		Bool used;
	public:
		MutexUsage(Sync::Mutex *mut);
		~MutexUsage();

		void BeginUse();
		void EndUse();

		void ReplaceMutex(Sync::Mutex *mut);
	};
}
#endif
