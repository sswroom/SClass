#ifndef _SM_DATA_SYNCLINKEDLIST
#define _SM_DATA_SYNCLINKEDLIST
#include "AnyType.h"
#include "Sync/Mutex.h"

namespace Data
{
	struct LinkedListItem
	{
		AnyType item;
		Optional<LinkedListItem> nextItem;
	};

	class SyncLinkedList
	{
	protected:
		Sync::Mutex mut;
		Optional<LinkedListItem> firstItem;
		Optional<LinkedListItem> lastItem;

	public:
		SyncLinkedList();
		~SyncLinkedList();

		Bool HasItems();
		void Put(AnyType item);
		AnyType Get();
		AnyType GetNoRemove();
		AnyType GetLastNoRemove();
		UOSInt GetCount();
		UOSInt IndexOf(AnyType item);
	};
}

#endif
