#ifndef _SM_DATA_SYNCARRAYLIST
#define _SM_DATA_SYNCARRAYLIST
#include "Data/ArrayList.h"
#include "Sync/Mutex.h"

namespace Data
{
	class SyncArrayList
	{
	private:
		Data::ArrayList<void*> *arr;
		Sync::Mutex *mut;

	public:
		SyncArrayList();
		SyncArrayList(OSInt Capacity);
		~SyncArrayList();

		void Add(void *val);
		void AddRange(ArrayList<void*> *arr);
		void *RemoveAt(OSInt Index);
		void Insert(OSInt Index, void *Val);
		OSInt IndexOf(void *Val);
		void Clear();
		SyncArrayList *Clone();

		OSInt GetCount();
		OSInt GetCapacity();

		void *GetItem(OSInt Index);
		void SetItem(OSInt Index, void *Val);
	};
};
#endif
