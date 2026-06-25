#ifndef _SM_DATA_SYNCARRAYMAP
#define _SM_DATA_SYNCARRAYMAP
#include "Data/ArrayListObj.hpp"
#include "Data/ArrayMap.hpp"
#include "Data/SortableArrayListNative.hpp"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <class T, class V> class SyncArrayMap : public DataMap<T, V>
	{
	protected:
		NN<Data::SortableArrayList<T>> keys;
		Data::ArrayList<V> vals;
		Sync::Mutex mut;

	public:
		SyncArrayMap();
		virtual ~SyncArrayMap();

		virtual V Put(T key, V val);
		virtual V Get(T key);
		virtual V Remove(T key);
		T GetKey(UIntOS index);
		void PutAll(SyncArrayMap<T,V> *map);
		IntOS GetIndex(T key);

		UIntOS GetValues(NN<Data::ArrayListObj<V>> values);
		UIntOS GetKeys(NN<Data::ArrayListNative<T>> keys);
		UIntOS GetCount();
		virtual Bool IsEmpty();
		virtual UnsafeArray<V> ToArray(OutParam<UIntOS> objCnt);
		virtual void Clear();
	};


	template <class T, class V> SyncArrayMap<T, V>::SyncArrayMap() : IMap<T, V>()
	{
	}

	template <class T, class V> SyncArrayMap<T, V>::~SyncArrayMap()
	{
	}

	template <class T, class V> V SyncArrayMap<T, V>::Put(T key, V val)
	{
		Sync::MutexUsage mutUsage(this->mut);
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			V oldVal = this->vals.GetItem(i);
            this->vals.SetItem(i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert(~i, key);
			this->vals.Insert(~i, val);
			return 0;
		}
	}

	template <class T, class V> V SyncArrayMap<T, V>::Get(T key)
	{
		Sync::MutexUsage mutUsage(this->mut);
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals.GetItem(i);
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> V SyncArrayMap<T, V>::Remove(T key)
	{
		Sync::MutexUsage mutUsage(this->mut);
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			this->keys->RemoveAt(i);
			return this->vals.RemoveAt(i);
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> T SyncArrayMap<T, V>::GetKey(UIntOS index)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->keys->GetItem(index);
	}

	template <class T, class V> void SyncArrayMap<T, V>::PutAll(SyncArrayMap<T,V> *map)
	{
		Sync::MutexUsage mutUsage(this->mut);
		Data::ArrayList<T> *tList = map->GetKeys();
		const Data::ArrayList<V> *vList = map->GetValues();
		UIntOS i;
		UIntOS j;
		i = 0;
		j = tList->GetCount();
		while (i < j)
		{
			this->Put(tList->GetItem(i), vList->GetItem(i));
			i++;
		}
	}

	template <class T, class V> IntOS SyncArrayMap<T, V>::GetIndex(T key)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> UIntOS SyncArrayMap<T, V>::GetValues(NN<Data::ArrayListObj<V>> values)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return values->AddRange(this->vals);
	}

	template <class T, class V> UIntOS SyncArrayMap<T, V>::GetKeys(NN<Data::ArrayListNative<T>> keys)
	{
		Sync::MutexUsage mutUsage(this->mut);
		return keys->AddRange(this->keys);
	}

	template <class T, class V> UIntOS SyncArrayMap<T, V>::GetCount()
	{
		return this->vals.GetCount();
	}

	template <class T, class V> Bool SyncArrayMap<T, V>::IsEmpty()
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> UnsafeArray<V> SyncArrayMap<T, V>::ToArray(OutParam<UIntOS> objCnt)
	{
		Sync::MutexUsage mutUsage(this->mut);
		UIntOS cnt;
		UnsafeArray<V> arr = this->vals.GetArray(cnt);
		UnsafeArray<V>outArr = MemAllocArr(V, cnt);
		MemCopyNO(&outArr[0], &arr[0], sizeof(V) * cnt);
		objCnt.Set(cnt);
		return outArr;
	}

	template <class T, class V> void SyncArrayMap<T, V>::Clear()
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->keys->Clear();
		this->vals.Clear();
	}
}

#endif
