#ifndef _SM_SYNC_EVENTPOOL
#define _SM_SYNC_EVENTPOOL
#include "Data/ArrayList.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Sync
{
	class EventPool
	{
	private:
		Data::ArrayList<void *> handList;
		Data::ArrayList<Sync::Event*> evtList;
		Data::ArrayList<void *> objList;
		Sync::Mutex mut;
		Sync::Event mainEvt;
		Int32 state; //0 = normal, 1 = changing, 2 = closing
		
	public:
		EventPool();
		~EventPool();
		void AddEvent(Sync::Event *evt, void *obj);
		void RemoveEvent(Sync::Event *evt);
		void *Wait(Int32 timeout);
	};
}
#endif
