#ifndef _SM_SYNC_SIMPLETHREAD
#define _SM_SYNC_SIMPLETHREAD
namespace Sync
{
	class SimpleThread
	{
	private:
		static UInt32 usScale;
	public:
		static void Sleep(UOSInt ms);
		static void Sleepus(UOSInt us);
	};
}
#endif
