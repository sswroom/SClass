#ifndef _SM_SYNC_MUTEX
#define _SM_SYNC_MUTEX

namespace Sync
{
	typedef struct
	{
		void *hand;
		const WChar *debName;
#ifdef _DEBUG
		Int32 locked;
		UInt32 lockId;
#endif
	} MutexData;

	void Mutex_Create(MutexData *data);
	void Mutex_Destroy(MutexData *data);
	void Mutex_Lock(MutexData *data);
	void Mutex_Unlock(MutexData *data);
	Bool Mutex_TryLock(MutexData *data);
	void Mutex_SetDebName(MutexData *data, const WChar *name);

	class Mutex
	{
	private:
		MutexData data;
	public:
		Mutex();
		~Mutex();
		void Lock();
		void Unlock();
		Bool TryLock();
		void SetDebName(const WChar *name);
	};
}
#endif
