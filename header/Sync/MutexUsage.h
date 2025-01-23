#ifndef _SM_SYNC_MUTEXUSAGE
#define _SM_SYNC_MUTEXUSAGE
#include "Sync/Mutex.h"

namespace Sync
{
	class MutexUsage
	{
	private:
		const Sync::Mutex *mut;
		Bool used;
	public:
		MutexUsage()
		{
			this->mut = 0;
			this->used = false;
		}

		MutexUsage(NN<const Sync::Mutex> mut)
		{
			this->mut = mut.Ptr();
			this->used = true;
			this->mut->Lock();
		}

		~MutexUsage()
		{
			this->EndUse();
		}

		void BeginUse()
		{
			if (this->mut && !this->used)
			{
				this->mut->Lock();
				this->used = true;
			}
		}

		void EndUse()
		{
			if (this->mut && this->used)
			{
				this->mut->Unlock();
				this->used = false;
			}
		}

		void ReplaceMutex(NN<const Sync::Mutex> mut)
		{
			if (this->used)
			{
				if (this->mut == mut.Ptr())
					return;
				if (this->mut)
				{
					this->mut->Unlock();
				}
			}
			this->mut = mut.Ptr();
			this->mut->Lock();
			this->used = true;
		}
	};
}
#endif
