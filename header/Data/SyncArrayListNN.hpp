#ifndef _SM_DATA_SYNCARRAYLISTNN
#define _SM_DATA_SYNCARRAYLISTNN
#include "Data/ArrayListNN.hpp"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <class T> class SyncArrayListNN : public ReadingListNN<T>
	{
	public:
		typedef void (CALLBACKFUNC FreeFunc)(NN<T> v);
	private:
		Data::ArrayListNN<T> arr;
		Sync::Mutex mut;

	public:
		SyncArrayListNN();
		SyncArrayListNN(UOSInt capacity);
		virtual ~SyncArrayListNN();

		virtual UOSInt Add(NN<T> val);
		virtual UOSInt AddRange(UnsafeArray<const NN<T>> arr, UOSInt cnt);
		virtual Bool Remove(NN<T> val);
		virtual Optional<T> RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, NN<T> val);
		virtual UOSInt IndexOf(NN<T> val) const;
		virtual void Clear();
		Optional<T> RemoveLast();
		NN<SyncArrayListNN> Clone() const;

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;

		virtual Optional<T> GetItem(UOSInt index) const;
		virtual NN<T> GetItemNoCheck(UOSInt index) const;
		virtual void SetItem(UOSInt index, T val);
		NN<Data::ArrayListNN<T>> GetArrayList(NN<Sync::MutexUsage> mutUsage);

		void DeleteAll();
		void FreeAll(FreeFunc freeFunc);
		void MemFreeAll();
	};


	template <class T> SyncArrayListNN<T>::SyncArrayListNN()
	{
	}

	template <class T> SyncArrayListNN<T>::SyncArrayListNN(UOSInt capacity) : arr(capacity)
	{
	}

	template <class T> SyncArrayListNN<T>::~SyncArrayListNN()
	{
	}

	template <class T> UOSInt SyncArrayListNN<T>::Add(NN<T> val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Add(val);
	}

	template <class T> UOSInt Data::SyncArrayListNN<T>::AddRange(UnsafeArray<const NN<T>> arr, UOSInt cnt)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.AddRange(arr, cnt);
	}

	template <class T> Bool Data::SyncArrayListNN<T>::Remove(NN<T> val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Remove(val);
	}

	template <class T> Optional<T> Data::SyncArrayListNN<T>::RemoveAt(UOSInt index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.RemoveAt(index);
	}

	template <class T> void Data::SyncArrayListNN<T>::Insert(UOSInt index, NN<T> val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.Insert(index, val);
	}

	template <class T> UOSInt Data::SyncArrayListNN<T>::IndexOf(NN<T> val) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.IndexOf(val);
	}

	template <class T> void Data::SyncArrayListNN<T>::Clear()
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.Clear();
	}

	template <class T> Optional<T> Data::SyncArrayListNN<T>::RemoveLast()
	{
		Sync::MutexUsage mutUsage(this->mut);
		UOSInt i = this->arr.GetCount();
		if (i > 0)
		{
			return this->arr.RemoveAt(i - 1);
		}
		return 0;
	}

	template <class T> NN<Data::SyncArrayListNN<T>> Data::SyncArrayListNN<T>::Clone() const
	{
		NN<Data::SyncArrayListNN<T>> newArr;
		Sync::MutexUsage mutUsage(this->mut);
		NEW_CLASSNN(newArr, Data::SyncArrayListNN<T>(this->arr.GetCapacity()));
		newArr->arr.AddRange(&this->arr);
		return newArr;
	}

	template <class T> UOSInt Data::SyncArrayListNN<T>::GetCount() const
	{
		return this->arr.GetCount();
	}

	template <class T> UOSInt Data::SyncArrayListNN<T>::GetCapacity() const
	{
		return this->arr.GetCapacity();
	}

	template <class T> Optional<T> Data::SyncArrayListNN<T>::GetItem(UOSInt index) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.GetItem(index);
	}

	template <class T> NN<T> Data::SyncArrayListNN<T>::GetItemNoCheck(UOSInt index) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.GetItemNoCheck(index);
	}

	template <class T> void Data::SyncArrayListNN<T>::SetItem(UOSInt index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.SetItem(index, val);
	}

	template <class T> NN<Data::ArrayListNN<T>> Data::SyncArrayListNN<T>::GetArrayList(NN<Sync::MutexUsage> mutUsage)
	{
		mutUsage->ReplaceMutex(this->mut);
		return this->arr;
	}

	template <class T> void Data::SyncArrayListNN<T>::DeleteAll()
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.DeleteAll();
	}

	template <class T> void Data::SyncArrayListNN<T>::FreeAll(FreeFunc freeFunc)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.FreeAll(freeFunc);
	}

	template <class T> void Data::SyncArrayListNN<T>::MemFreeAll()
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.MemFreeAll();
	}

}
#endif
