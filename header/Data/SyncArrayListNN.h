#ifndef _SM_DATA_SYNCARRAYLISTNN
#define _SM_DATA_SYNCARRAYLISTNN
#include "Data/ArrayListNN.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <class T> class SyncArrayListNN : public ReadingListNN<T>
	{
	private:
		Data::ArrayListNN<T> arr;
		Sync::Mutex mut;

	public:
		SyncArrayListNN();
		SyncArrayListNN(UOSInt capacity);
		virtual ~SyncArrayListNN();

		virtual UOSInt Add(NotNullPtr<T> val);
		virtual UOSInt AddRange(const NotNullPtr<T> *arr, UOSInt cnt);
		virtual Bool Remove(NotNullPtr<T> val);
		virtual Optional<T> RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, NotNullPtr<T> val);
		virtual UOSInt IndexOf(NotNullPtr<T> val) const;
		virtual void Clear();
		Optional<T> RemoveLast();
		NotNullPtr<SyncArrayListNN> Clone() const;

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;

		virtual Optional<T> GetItem(UOSInt index) const;
		virtual NotNullPtr<T> GetItemNoCheck(UOSInt index) const;
		virtual void SetItem(UOSInt index, T val);
		NotNullPtr<Data::ArrayListNN<T>> GetArrayList(NotNullPtr<Sync::MutexUsage> mutUsage);
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

	template <class T> UOSInt SyncArrayListNN<T>::Add(NotNullPtr<T> val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Add(val);
	}

	template <class T> UOSInt Data::SyncArrayListNN<T>::AddRange(const NotNullPtr<T> *arr, UOSInt cnt)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.AddRange(arr, cnt);
	}

	template <class T> Bool Data::SyncArrayListNN<T>::Remove(NotNullPtr<T> val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.Remove(val);
	}

	template <class T> Optional<T> Data::SyncArrayListNN<T>::RemoveAt(UOSInt index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.RemoveAt(index);
	}

	template <class T> void Data::SyncArrayListNN<T>::Insert(UOSInt index, NotNullPtr<T> val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.Insert(index, val);
	}

	template <class T> UOSInt Data::SyncArrayListNN<T>::IndexOf(NotNullPtr<T> val) const
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

	template <class T> NotNullPtr<Data::SyncArrayListNN<T>> Data::SyncArrayListNN<T>::Clone() const
	{
		NotNullPtr<Data::SyncArrayListNN<T>> newArr;
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

	template <class T> NotNullPtr<T> Data::SyncArrayListNN<T>::GetItemNoCheck(UOSInt index) const
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->arr.GetItemNoCheck(index);
	}

	template <class T> void Data::SyncArrayListNN<T>::SetItem(UOSInt index, T val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->arr.SetItem(index, val);
	}

	template <class T> NotNullPtr<Data::ArrayListNN<T>> Data::SyncArrayListNN<T>::GetArrayList(NotNullPtr<Sync::MutexUsage> mutUsage)
	{
		mutUsage->ReplaceMutex(this->mut);
		return this->arr;
	}
}
#endif
