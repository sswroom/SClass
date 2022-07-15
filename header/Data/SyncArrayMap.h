#ifndef _SM_DATA_SYNCARRAYMAP
#define _SM_DATA_SYNCARRAYMAP
#include "Data/IMap.h"
#include "Data/SortableArrayList.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Data
{
	template <class T, class V> class SyncArrayMap : public IMap<T, V>
	{
	protected:
		Data::SortableArrayList<T> *keys;
		Data::ArrayList<V> vals;
		Sync::Mutex mut;

	public:
		SyncArrayMap();
		virtual ~SyncArrayMap();

		virtual V Put(T key, V val);
		virtual V Get(T key);
		virtual V Remove(T key);
		T GetKey(UOSInt index);
		void PutAll(SyncArrayMap<T,V> *map);
		OSInt GetIndex(T key);

		UOSInt GetValues(Data::ArrayList<V> *values);
		UOSInt GetKeys(Data::ArrayList<T> *keys);
		UOSInt GetCount();
		virtual Bool IsEmpty();
		virtual V *ToArray(UOSInt *objCnt);
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
		Sync::MutexUsage mutUsage(&this->mut);
		OSInt i;
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
		Sync::MutexUsage mutUsage(&this->mut);
		OSInt i;
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
		Sync::MutexUsage mutUsage(&this->mut);
		OSInt i;
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

	template <class T, class V> T SyncArrayMap<T, V>::GetKey(UOSInt index)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		return this->keys->GetItem(index);
	}

	template <class T, class V> void SyncArrayMap<T, V>::PutAll(SyncArrayMap<T,V> *map)
	{
		Sync::MutexUsage mutUsage(this->mut);
		Data::ArrayList<T> *tList = map->GetKeys();
		const Data::ArrayList<V> *vList = map->GetValues();
		UOSInt i;
		UOSInt j;
		i = 0;
		j = tList->GetCount();
		while (i < j)
		{
			this->Put(tList->GetItem(i), vList->GetItem(i));
			i++;
		}
	}

	template <class T, class V> OSInt SyncArrayMap<T, V>::GetIndex(T key)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		return this->keys->SortedIndexOf(key);
	}

	template <class T, class V> UOSInt SyncArrayMap<T, V>::GetValues(Data::ArrayList<V> *values)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		return values->AddRange(this->vals);
	}

	template <class T, class V> UOSInt SyncArrayMap<T, V>::GetKeys(Data::ArrayList<T> *keys)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		return keys->AddRange(this->keys);
	}

	template <class T, class V> UOSInt SyncArrayMap<T, V>::GetCount()
	{
		return this->vals.GetCount();
	}

	template <class T, class V> Bool SyncArrayMap<T, V>::IsEmpty()
	{
		return this->vals.GetCount() == 0;
	}

	template <class T, class V> V *SyncArrayMap<T, V>::ToArray(UOSInt *objCnt)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		UOSInt cnt;
		V *arr = this->vals.GetArray(&cnt);
		V *outArr = MemAlloc(V, cnt);
		MemCopyNO(outArr, arr, sizeof(V) * cnt);
		*objCnt = cnt;
		return outArr;
	}

	template <class T, class V> void SyncArrayMap<T, V>::Clear()
	{
		Sync::MutexUsage mutUsage(&this->mut);
		this->keys->Clear();
		this->vals.Clear();
	}
}

#endif
