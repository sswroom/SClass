#ifndef _SM_MEDIA_REFCLOCK
#define _SM_MEDIA_REFCLOCK

namespace Media
{
	class RefClock
	{
	private:
		Bool started;
		Int64 refStart;
		UInt32 refStartTime;

	public:
		RefClock();
		~RefClock();

		void Start(UInt32 currTime);
		void Stop();
		UInt32 GetCurrTime();
		Bool Running();
	};
};

#endif
