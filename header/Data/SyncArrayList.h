#ifndef _SM_DATA_SYNCARRAYLIST
#define _SM_DATA_SYNCARRAYLIST
#include "Data/ArrayList.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <class T> class SyncArrayList : public List<T>
	{
	private:
		Data::ArrayList<T> *arr;
		Sync::Mutex *mut;

	public:
		SyncArrayList();
		SyncArrayList(UOSInt capacity);
		virtual ~SyncArrayList();

		virtual UOSInt Add(T val);
		virtual UOSInt AddRange(T *arr, UOSInt cnt);
		virtual Bool Remove(T val);
		virtual T RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, T val);
		virtual UOSInt IndexOf(T val);
		virtual void Clear();
		SyncArrayList *Clone();

		virtual UOSInt GetCount();
		virtual UOSInt GetCapacity();

		virtual T GetItem(UOSInt index);
		virtual void SetItem(UOSInt index, T val);
	};


	template <class T> SyncArrayList<T>::SyncArrayList()
	{
		NEW_CLASS(arr, Data::ArrayList<T>());
		NEW_CLASS(mut, Sync::Mutex());
	}

	template <class T> SyncArrayList<T>::SyncArrayList(UOSInt capacity)
	{
		NEW_CLASS(arr, Data::ArrayList<T>(capacity));
		NEW_CLASS(mut, Sync::Mutex());
	}

	template <class T> SyncArrayList<T>::~SyncArrayList()
	{
		DEL_CLASS(arr);
		DEL_CLASS(mut);
	}

	template <class T> UOSInt SyncArrayList<T>::Add(T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr->Add(val);
	}

	template <class T> UOSInt Data::SyncArrayList<T>::AddRange(T *arr, UOSInt cnt)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr->AddRange(arr, cnt);
	}

	template <class T> Bool Data::SyncArrayList<T>::Remove(T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr->Remove(val);
	}

	template <class T> T Data::SyncArrayList<T>::RemoveAt(UOSInt index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr->RemoveAt(index);
	}

	template <class T> void Data::SyncArrayList<T>::Insert(UOSInt index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr->Insert(index, val);
	}

	template <class T> UOSInt Data::SyncArrayList<T>::IndexOf(T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr->IndexOf(val);
	}

	template <class T> void Data::SyncArrayList<T>::Clear()
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr->Clear();
	}

	template <class T> Data::SyncArrayList<T> *Data::SyncArrayList<T>::Clone()
	{
		Data::SyncArrayList<T> *newArr;
		Sync::MutexUsage mutUsage(this->mut);
		NEW_CLASS(newArr, Data::SyncArrayList<T>(arr->GetCapacity()));
		newArr->arr->AddRange(arr);
		return newArr;
	}

	template <class T> UOSInt Data::SyncArrayList<T>::GetCount()
	{
		return this->arr->GetCount();
	}

	template <class T> UOSInt Data::SyncArrayList<T>::GetCapacity()
	{
		return this->arr->GetCapacity();
	}

	template <class T> T Data::SyncArrayList<T>::GetItem(UOSInt index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr->GetItem(index);
	}

	template <class T> void Data::SyncArrayList<T>::SetItem(UOSInt index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr->SetItem(index, val);
	}
}
#endif
