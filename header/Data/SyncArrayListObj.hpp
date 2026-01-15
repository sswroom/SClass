#ifndef _SM_DATA_SYNCARRAYLISTOBJ
#define _SM_DATA_SYNCARRAYLISTOBJ
#include "Data/ArrayListObj.hpp"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <class T> class SyncArrayListObj : public List<T>
	{
	private:
		Data::ArrayListObj<T> arr;
		Sync::Mutex mut;

	public:
		SyncArrayListObj();
		SyncArrayListObj(UIntOS capacity);
		virtual ~SyncArrayListObj();

		virtual UIntOS Add(T val);
		virtual UIntOS AddRange(UnsafeArray<const T> arr, UIntOS cnt);
		virtual Bool Remove(T val);
		virtual T RemoveAt(UIntOS index);
		virtual void Insert(UIntOS index, T val);
		virtual UIntOS IndexOf(T val) const;
		virtual void Clear();
		T RemoveLast();
		NN<SyncArrayListObj<T>> Clone() const;

		virtual UIntOS GetCount() const;
		virtual UIntOS GetCapacity() const;

		virtual T GetItem(UIntOS index) const;
		virtual void SetItem(UIntOS index, T val);
		NN<Data::ArrayListObj<T>> GetArrayList(NN<Sync::MutexUsage> mutUsage);
	};


	template <class T> SyncArrayListObj<T>::SyncArrayListObj()
	{
	}

	template <class T> SyncArrayListObj<T>::SyncArrayListObj(UIntOS capacity) : arr(capacity)
	{
	}

	template <class T> SyncArrayListObj<T>::~SyncArrayListObj()
	{
	}

	template <class T> UIntOS SyncArrayListObj<T>::Add(T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Add(val);
	}

	template <class T> UIntOS Data::SyncArrayListObj<T>::AddRange(UnsafeArray<const T> arr, UIntOS cnt)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.AddRange(arr, cnt);
	}

	template <class T> Bool Data::SyncArrayListObj<T>::Remove(T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Remove(val);
	}

	template <class T> T Data::SyncArrayListObj<T>::RemoveAt(UIntOS index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.RemoveAt(index);
	}

	template <class T> void Data::SyncArrayListObj<T>::Insert(UIntOS index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.Insert(index, val);
	}

	template <class T> UIntOS Data::SyncArrayListObj<T>::IndexOf(T val) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.IndexOf(val);
	}

	template <class T> void Data::SyncArrayListObj<T>::Clear()
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.Clear();
	}

	template <class T> T Data::SyncArrayListObj<T>::RemoveLast()
	{
		Sync::MutexUsage mutUsage(this->mut);
		UIntOS i = this->arr.GetCount();
		if (i > 0)
		{
			return this->arr.RemoveAt(i - 1);
		}
		return 0;
	}

	template <class T> NN<Data::SyncArrayListObj<T>> Data::SyncArrayListObj<T>::Clone() const
	{
		NN<Data::SyncArrayListObj<T>> newArr;
		Sync::MutexUsage mutUsage(this->mut);
		NEW_CLASSNN(newArr, Data::SyncArrayListObj<T>(this->arr.GetCapacity()));
		newArr->arr.AddRange(&this->arr);
		return newArr;
	}

	template <class T> UIntOS Data::SyncArrayListObj<T>::GetCount() const
	{
		return this->arr.GetCount();
	}

	template <class T> UIntOS Data::SyncArrayListObj<T>::GetCapacity() const
	{
		return this->arr.GetCapacity();
	}

	template <class T> T Data::SyncArrayListObj<T>::GetItem(UIntOS index) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.GetItem(index);
	}

	template <class T> void Data::SyncArrayListObj<T>::SetItem(UIntOS index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.SetItem(index, val);
	}

	template <class T> NN<Data::ArrayListObj<T>> Data::SyncArrayListObj<T>::GetArrayList(NN<Sync::MutexUsage> mutUsage)
	{
		mutUsage->ReplaceMutex(this->mut);
		return this->arr;
	}
}
#endif
