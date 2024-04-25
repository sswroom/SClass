#ifndef _SM_DATA_FASTMAPNN
#define _SM_DATA_FASTMAPNN
#include "Data/ArrayList.h"
#include "Data/ListMapNN.h"
#include "Data/ReadingListNN.h"
#include "Data/TwinItem.h"

namespace Data
{
	template <class T, class V> struct TwinItemNN
	{
	public:
		T key;
		NN<V> value;
	
		TwinItemNN(T key, NN<V> value)
		{
			this->key = key;
			this->value = value;
		}

		TwinItemNN(T key)
		{
			this->key = key;
		}

		Bool operator==(TwinItemNN<T,V> item)
		{
			return this->key == item.key;
		}
	};

	template <class T, class V> class FastMapNNIterator
	{
	private:
		NN<const Data::ArrayList<TwinItemNN<T, V>>> arr;
		UOSInt nextIndex;

	public:
		FastMapNNIterator(NN<const Data::ArrayList<TwinItemNN<T, V>>> arr)
		{
			this->arr = arr;
			this->nextIndex = 0;
		}

		Bool HasNext() const
		{
			return this->nextIndex < this->arr->GetCount();
		}

		NN<V> Next()
		{
			return this->arr->GetItem(this->nextIndex++);
		}
	};

	template <class T, class V> class FastMapNN : public ListMapNN<T, V>
	{
	protected:
		Data::ArrayList<TwinItemNN<T, V>> values;

	public:
		FastMapNN();
		virtual ~FastMapNN();

		virtual Optional<V> Put(T key, NN<V> val);
		virtual Optional<V> Get(T key) const;
		virtual Optional<V> Remove(T key);
		OSInt GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UOSInt cnt);
		UOSInt AddKeysTo(NN<List<T>> list);
		virtual UOSInt GetCount() const;
		virtual T GetKey(UOSInt index) const;
		virtual Optional<V> GetItem(UOSInt index) const;
		virtual NN<V> GetItemNoCheck(UOSInt index) const;
		Optional<V> RemoveAt(UOSInt index);
		virtual Bool IsEmpty() const;
		virtual Optional<V> *ToArray(OutParam<UOSInt> objCnt);
		virtual void Clear();
		FastMapNNIterator<T,V> Iterator() const;
		void DeleteAll();
	};


	template <class T, class V> FastMapNN<T, V>::FastMapNN() : ListMapNN<T, V>()
	{
	}

	template <class T, class V> FastMapNN<T, V>::~FastMapNN()
	{
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::Put(T key, NN<V> val)
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			Optional<V> oldVal = this->values.GetItem((UOSInt)i).value;
            this->values.SetItem((UOSInt)i, TwinItemNN<T,V>(key, val));
			return oldVal;
		}
		else
		{
			this->values.Insert((UOSInt)~i, TwinItemNN<T,V>(key, val));
			return 0;
		}
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::Get(T key) const
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.GetItem((UOSInt)i).value;
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::Remove(T key)
	{
		OSInt i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.RemoveAt((UOSInt)i).value;
		}
		else
		{
			return 0;
		}
	}

	template <class T, class V> OSInt FastMapNN<T, V>::GetIndex(T key) const
	{
		OSInt i;
		OSInt j;
		OSInt k;
		T l;
		i = 0;
		j = (OSInt)this->values.GetCount() - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = this->values.GetItem((UOSInt)k).key;
			if (l > key)
			{
				j = k - 1;
			}
			else if (l < key)
			{
				i = k + 1;
			}
			else
			{
				return k;
			}
		}
		return -i - 1;
	}

	template <class T, class V> Bool FastMapNN<T, V>::ContainsKey(T key) const
	{
		return this->GetIndex(key) >= 0;
	}

	template <class T, class V> void FastMapNN<T, V>::AllocSize(UOSInt cnt)
	{
		UOSInt newSize = this->values.GetCount() + cnt;
		this->values.EnsureCapacity(newSize);
	}

	template <class T, class V> UOSInt FastMapNN<T, V>::AddKeysTo(NN<List<T>> list)
	{
		UOSInt i = 0;
		UOSInt j = this->values.GetCount();
		while (i < j)
		{
			list->Add(this->values.GetItem(i).key);
			i++;
		}
		return j;
	}

	template <class T, class V> UOSInt FastMapNN<T, V>::GetCount() const
	{
		return this->values.GetCount();
	}

	template <class T, class V> T FastMapNN<T, V>::GetKey(UOSInt index) const
	{
		return this->values.GetItem(index).key;
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::GetItem(UOSInt index) const
	{
		if (index >= this->values.GetCount())
			return 0;
		return this->values.GetItem(index).value;
	}

	template <class T, class V> NN<V> FastMapNN<T, V>::GetItemNoCheck(UOSInt index) const
	{
		return this->values.GetItem(index).value;
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::RemoveAt(UOSInt index)
	{
		return this->values.RemoveAt(index).value;
	}
	
	template <class T, class V> Bool FastMapNN<T, V>::IsEmpty() const
	{
		return this->values.GetCount() == 0;
	}

	template <class T, class V> Optional<V> *FastMapNN<T, V>::ToArray(OutParam<UOSInt> objCnt)
	{
		UOSInt cnt = this->values.GetCount();
		Optional<V> *outArr = MemAlloc(Optional<V>, cnt);
		objCnt.Set(cnt);
		while (cnt-- > 0)
		{
			outArr[cnt] = this->values.GetItem(cnt).value;
		}
		return outArr;
	}

	template <class T, class V> void FastMapNN<T, V>::Clear()
	{
		this->values.Clear();
	}

	template <class T, class V> FastMapNNIterator<T,V> FastMapNN<T, V>::Iterator() const
	{
		return FastMapNNIterator<T, V>(this->values);
	}

	template <class T, class V> void FastMapNN<T, V>::DeleteAll()
	{
		UOSInt i = this->values.GetCount();
		while (i-- > 0)
		{
			this->values.GetItem(i).value.Delete();
		}
		this->values.Clear();
	}

	template <class V> class Int32FastMapNN : public FastMapNN<Int32, V>
	{

	};

	template <class V> class UInt32FastMapNN : public FastMapNN<UInt32, V>
	{

	};

	template <class V> class Int64FastMapNN : public FastMapNN<Int64, V>
	{

	};

	template <class V> class UInt64FastMapNN : public FastMapNN<UInt64, V>
	{

	};
}

#endif
