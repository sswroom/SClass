#ifndef _SM_SYNC_RWMUTEX
#define _SM_SYNC_RWMUTEX

namespace Sync
{
	class RWMutex
	{
	public:
		struct ClassData;
	private:
		ClassData *clsData;
		UInt32 writeTId;
		Int32 readCnt;
	public:
		RWMutex();
		~RWMutex();

		void LockRead();
		void UnlockRead();
		void LockWrite();
		void UnlockWrite();
	};
}
#endif
