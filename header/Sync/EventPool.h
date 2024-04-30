#ifndef _SM_SYNC_EVENTPOOL
#define _SM_SYNC_EVENTPOOL
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Sync
{
	class EventPool
	{
	private:
		Data::ArrayList<void *> handList;
		Data::ArrayListNN<Sync::Event> evtList;
		Data::ArrayList<AnyType> objList;
		Sync::Mutex mut;
		Sync::Event mainEvt;
		Int32 state; //0 = normal, 1 = changing, 2 = closing
		
	public:
		EventPool();
		~EventPool();
		void AddEvent(NN<Sync::Event> evt, AnyType obj);
		void RemoveEvent(NN<Sync::Event> evt);
		void *Wait(Int32 timeout);
	};
}
#endif
