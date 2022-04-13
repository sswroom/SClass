#ifndef _SM_DATA_SYNCLINKEDLIST
#define _SM_DATA_SYNCLINKEDLIST
#include "Sync/Mutex.h"

namespace Data
{
	struct LinkedListItem
	{
		void *item;
		LinkedListItem *nextItem;
	};

	class SyncLinkedList
	{
	protected:
		Sync::Mutex mut;
		LinkedListItem *firstItem;
		LinkedListItem *lastItem;

	public:
		SyncLinkedList();
		~SyncLinkedList();

		Bool HasItems();
		void Put(void *item);
		void *Get();
		void *GetNoRemove();
		void *GetLastNoRemove();
		UOSInt GetCount();
		UOSInt IndexOf(void *item);
	};
}

#endif
