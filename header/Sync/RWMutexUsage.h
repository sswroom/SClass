#ifndef _SM_SYNC_RWMUTEXUSAGE
#define _SM_SYNC_RWMUTEXUSAGE
#include "Sync/RWMutex.h"

namespace Sync
{
	class RWMutexUsage
	{
	private:
		Sync::RWMutex *mut;
		Bool used;
		Bool writing;
	public:
		RWMutexUsage(Sync::RWMutex *mut, Bool writing);
		~RWMutexUsage();

		void BeginUse(Bool writing);
		void EndUse();
	};
}
#endif
