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
		Data::ArrayList<T> arr;
		Sync::Mutex mut;

	public:
		SyncArrayList();
		SyncArrayList(UOSInt capacity);
		virtual ~SyncArrayList();

		virtual UOSInt Add(T val);
		virtual UOSInt AddRange(T *arr, UOSInt cnt);
		virtual Bool Remove(T val);
		virtual T RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, T val);
		virtual UOSInt IndexOf(T val) const;
		virtual void Clear();
		T RemoveLast();
		SyncArrayList *Clone() const;

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;

		virtual T GetItem(UOSInt index) const;
		virtual void SetItem(UOSInt index, T val);
		Data::ArrayList<T> *GetArrayList(Sync::MutexUsage *mutUsage);
	};


	template <class T> SyncArrayList<T>::SyncArrayList()
	{
	}

	template <class T> SyncArrayList<T>::SyncArrayList(UOSInt capacity) : arr(capacity)
	{
	}

	template <class T> SyncArrayList<T>::~SyncArrayList()
	{
	}

	template <class T> UOSInt SyncArrayList<T>::Add(T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Add(val);
	}

	template <class T> UOSInt Data::SyncArrayList<T>::AddRange(T *arr, UOSInt cnt)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.AddRange(arr, cnt);
	}

	template <class T> Bool Data::SyncArrayList<T>::Remove(T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Remove(val);
	}

	template <class T> T Data::SyncArrayList<T>::RemoveAt(UOSInt index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.RemoveAt(index);
	}

	template <class T> void Data::SyncArrayList<T>::Insert(UOSInt index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.Insert(index, val);
	}

	template <class T> UOSInt Data::SyncArrayList<T>::IndexOf(T val) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.IndexOf(val);
	}

	template <class T> void Data::SyncArrayList<T>::Clear()
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.Clear();
	}

	template <class T> T Data::SyncArrayList<T>::RemoveLast()
	{
		Sync::MutexUsage mutUsage(this->mut);
		UOSInt i = this->arr.GetCount();
		if (i > 0)
		{
			return this->arr.RemoveAt(i - 1);
		}
		return 0;
	}

	template <class T> Data::SyncArrayList<T> *Data::SyncArrayList<T>::Clone() const
	{
		Data::SyncArrayList<T> *newArr;
		Sync::MutexUsage mutUsage(this->mut);
		NEW_CLASS(newArr, Data::SyncArrayList<T>(this->arr.GetCapacity()));
		newArr->arr.AddRange(&this->arr);
		return newArr;
	}

	template <class T> UOSInt Data::SyncArrayList<T>::GetCount() const
	{
		return this->arr.GetCount();
	}

	template <class T> UOSInt Data::SyncArrayList<T>::GetCapacity() const
	{
		return this->arr.GetCapacity();
	}

	template <class T> T Data::SyncArrayList<T>::GetItem(UOSInt index) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.GetItem(index);
	}

	template <class T> void Data::SyncArrayList<T>::SetItem(UOSInt index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.SetItem(index, val);
	}

	template <class T> Data::ArrayList<T> *Data::SyncArrayList<T>::GetArrayList(Sync::MutexUsage *mutUsage)
	{
		mutUsage->ReplaceMutex(this->mut);
		return &this->arr;
	}
}
#endif
