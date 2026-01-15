#ifndef _SM_DATA_FASTMAPNN
#define _SM_DATA_FASTMAPNN
#include "Data/ArrayListNative.hpp"
#include "Data/ListMapNN.hpp"
#include "Data/ReadingListNN.hpp"
#include "Data/TwinItemNN.hpp"

namespace Data
{
	template <class T, class V> class FastMapNNIterator
	{
	private:
		NN<const Data::ArrayListNative<TwinItemNN<T, V>>> arr;
		UIntOS nextIndex;

	public:
		FastMapNNIterator(NN<const Data::ArrayListNative<TwinItemNN<T, V>>> arr)
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
			return this->arr->GetItem(this->nextIndex++).value;
		}
	};

	template <class T, class V> class FastMapNN : public ListMapNN<T, V>
	{
	protected:
		Data::ArrayListNative<TwinItemNN<T, V>> values;

	public:
		FastMapNN();
		virtual ~FastMapNN();

		virtual Optional<V> Put(T key, NN<V> val);
		virtual Optional<V> Get(T key) const;
		virtual Optional<V> Remove(T key);
		IntOS GetIndex(T key) const;
		Bool ContainsKey(T key) const;

		void AllocSize(UIntOS cnt);
		UIntOS AddKeysTo(NN<List<T>> list);
		virtual UIntOS GetCount() const;
		virtual T GetKey(UIntOS index) const;
		virtual Optional<V> GetItem(UIntOS index) const;
		virtual NN<V> GetItemNoCheck(UIntOS index) const;
		Optional<V> RemoveAt(UIntOS index);
		virtual Bool IsEmpty() const;
		virtual UnsafeArray<Optional<V>> ToArray(OutParam<UIntOS> objCnt);
		virtual void Clear();
		FastMapNNIterator<T,V> Iterator() const;
		void DeleteAll();
		void MemFreeAll();
	};


	template <class T, class V> FastMapNN<T, V>::FastMapNN() : ListMapNN<T, V>()
	{
	}

	template <class T, class V> FastMapNN<T, V>::~FastMapNN()
	{
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::Put(T key, NN<V> val)
	{
		IntOS i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			Optional<V> oldVal = this->values.GetItem((UIntOS)i).value;
            this->values.SetItem((UIntOS)i, TwinItemNN<T,V>(key, val));
			return oldVal;
		}
		else
		{
			this->values.Insert((UIntOS)~i, TwinItemNN<T,V>(key, val));
			return nullptr;
		}
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::Get(T key) const
	{
		IntOS i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.GetItem((UIntOS)i).value;
		}
		else
		{
			return nullptr;
		}
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::Remove(T key)
	{
		IntOS i;
		i = this->GetIndex(key);
		if (i >= 0)
		{
			return this->values.RemoveAt((UIntOS)i).value;
		}
		else
		{
			return nullptr;
		}
	}

	template <class T, class V> IntOS FastMapNN<T, V>::GetIndex(T key) const
	{
		IntOS i;
		IntOS j;
		IntOS k;
		T l;
		i = 0;
		j = (IntOS)this->values.GetCount() - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			l = this->values.GetItem((UIntOS)k).key;
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

	template <class T, class V> void FastMapNN<T, V>::AllocSize(UIntOS cnt)
	{
		UIntOS newSize = this->values.GetCount() + cnt;
		this->values.EnsureCapacity(newSize);
	}

	template <class T, class V> UIntOS FastMapNN<T, V>::AddKeysTo(NN<List<T>> list)
	{
		UIntOS i = 0;
		UIntOS j = this->values.GetCount();
		while (i < j)
		{
			list->Add(this->values.GetItem(i).key);
			i++;
		}
		return j;
	}

	template <class T, class V> UIntOS FastMapNN<T, V>::GetCount() const
	{
		return this->values.GetCount();
	}

	template <class T, class V> T FastMapNN<T, V>::GetKey(UIntOS index) const
	{
		return this->values.GetItem(index).key;
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::GetItem(UIntOS index) const
	{
		if (index >= this->values.GetCount())
			return nullptr;
		return this->values.GetItem(index).value;
	}

	template <class T, class V> NN<V> FastMapNN<T, V>::GetItemNoCheck(UIntOS index) const
	{
		return this->values.GetItem(index).value;
	}

	template <class T, class V> Optional<V> FastMapNN<T, V>::RemoveAt(UIntOS index)
	{
		return this->values.RemoveAt(index).value;
	}
	
	template <class T, class V> Bool FastMapNN<T, V>::IsEmpty() const
	{
		return this->values.GetCount() == 0;
	}

	template <class T, class V> UnsafeArray<Optional<V>> FastMapNN<T, V>::ToArray(OutParam<UIntOS> objCnt)
	{
		UIntOS cnt = this->values.GetCount();
		UnsafeArray<Optional<V>> outArr = MemAlloc(Optional<V>, cnt);
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
		UIntOS i = this->values.GetCount();
		while (i-- > 0)
		{
			this->values.GetItem(i).value.Delete();
		}
		this->values.Clear();
	}

	template <class T, class V> void FastMapNN<T, V>::MemFreeAll()
	{
		UIntOS i = this->values.GetCount();
		while (i-- > 0)
		{
			MemFreeNN(this->values.GetItem(i).value);
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
