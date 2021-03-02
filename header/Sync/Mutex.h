#ifndef _SM_SYNC_MUTEX
#define _SM_SYNC_MUTEX
//#define MUTEX_DEBUG
namespace Sync
{
	typedef struct
	{
		void *hand;
		const UTF8Char *debName;
		Int32 locked;
		UInt32 lockId;
	} MutexData;

	void Mutex_Create(MutexData *data);
	void Mutex_Destroy(MutexData *data);
	void Mutex_Lock(MutexData *data);
	void Mutex_Unlock(MutexData *data);
	Bool Mutex_TryLock(MutexData *data);
	void Mutex_SetDebName(MutexData *data, const UTF8Char *name);

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
		void SetDebName(const UTF8Char *name);
	};
}
#endif
