#ifndef _SM_SYNC_EVENT
#define _SM_SYNC_EVENT
#include "Data/Duration.h"

namespace Sync
{
	class Event
	{
	private:
		Bool isSet;
		Bool isAuto;
	public:
		void *hand;

		Event();
		Event(Bool isAuto);
		~Event();
		void Wait();
		Bool Wait(Data::Duration timeout); //true if timed out
		void Set();
		void Clear();
		Bool IsSet();
		void *GetHandle();
	};
}
#endif
