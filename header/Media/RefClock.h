#ifndef _SM_MEDIA_REFCLOCK
#define _SM_MEDIA_REFCLOCK

namespace Media
{
	class RefClock
	{
	private:
		Bool started;
		Int64 refStart;
		Int32 refStartTime;

	public:
		RefClock();
		~RefClock();

		void Start(Int32 currTime);
		void Stop();
		Int32 GetCurrTime();
		Bool Running();
	};
};

#endif
