#ifndef _SM_DATA_LINKEDLIST
#define _SM_DATA_LINKEDLIST
#include "Sync/Mutex.h"

namespace Data
{
	struct LinkedListItem
	{
		void *item;
		LinkedListItem *nextItem;
	};

	class LinkedList
	{
	protected:
		Sync::Mutex *mut;
		LinkedListItem *firstItem;
		LinkedListItem *lastItem;

	public:
		LinkedList();
		~LinkedList();

		Bool HasItems();
		void Put(void *item);
		void *Get();
		void *GetNoRemove();
		OSInt GetCount();
	};
};

#endif
