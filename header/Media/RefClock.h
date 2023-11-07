#ifndef _SM_MEDIA_REFCLOCK
#define _SM_MEDIA_REFCLOCK
#include "Data/Duration.h"
#include "Data/TimeInstant.h"

namespace Media
{
	class RefClock
	{
	private:
		Bool started;
		Data::TimeInstant refStart;
		Data::Duration refStartTime;

	public:
		RefClock();
		~RefClock();

		void Start(Data::Duration currTime);
		void Stop();
		Data::Duration GetCurrTime();
		Bool Running();
	};
};

#endif
