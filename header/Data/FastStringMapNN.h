#ifndef _SM_DATA_FASTSTRINGMAPNN
#define _SM_DATA_FASTSTRINGMAPNN
#include "MyMemory.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/ListMapNN.h"
#include "Text/MyString.h"
#include "Text/String.h"

namespace Data
{
	template <class T> struct FastStringNNItem
	{
		UInt32 hash;
		NN<Text::String> s;
		NN<T> val;
	};

	template <class T> class FastStringNNKeyIterator
	{
	private:
		const FastStringNNItem<T> *arr;
		UOSInt cnt;
	public:
		FastStringNNKeyIterator(const FastStringNNItem<T> *arr, UOSInt cnt)
		{
			this->arr = arr;
			this->cnt = cnt;
		}

		Bool HasNext() const
		{
			return this->cnt > 0;
		}

		NN<Text::String> Next()
		{
			this->cnt--;
			NN<Text::String> ret = arr->s;
			this->arr++;
			return ret;
		}
	};

	template <class T> class FastStringMapNN : public ListMapNN<Text::String *, T>
	{
	private:
		UOSInt capacity;
		UOSInt cnt;
		FastStringNNItem<T> *items;
		Crypto::Hash::CRC32RC crc;

		void Insert(UOSInt index, UInt32 hash, NN<Text::String> s, NN<T> val);
	public:
		FastStringMapNN();
		FastStringMapNN(const FastStringMapNN<T> *map);
		virtual ~FastStringMapNN();

		virtual UOSInt GetCount() const;
		virtual Optional<T> GetItem(UOSInt index) const;
		virtual NN<T> GetItemNoCheck(UOSInt index) const;
		virtual Text::String *GetKey(UOSInt index) const;
		virtual OSInt IndexOf(UInt32 hash, const UTF8Char *s, UOSInt len) const;
		OSInt IndexOf(NN<Text::String> s) const;
		OSInt IndexOfC(Text::CStringNN s) const;

		virtual Optional<T> Put(Text::String *key, NN<T> val);
		Optional<T> PutNN(NN<Text::String> key, NN<T> val);
		Optional<T> PutC(Text::CStringNN key, NN<T> val);
		virtual Optional<T> Get(Text::String *key) const;
		Optional<T> GetNN(NN<Text::String> key) const;
		Optional<T> GetC(Text::CStringNN key) const;
		virtual Optional<T> Remove(Text::String *key);
		Optional<T> RemoveNN(NN<Text::String> key);
		Optional<T> RemoveC(Text::CStringNN key);
		Optional<T> RemoveAt(UOSInt index);
		virtual Bool IsEmpty() const;
		virtual void Clear();
		FastStringNNKeyIterator<T> KeyIterator() const;
		
		UInt32 CalcHash(const UTF8Char *s, UOSInt len) const;
	};

	template <class T> void FastStringMapNN<T>::Insert(UOSInt index, UInt32 hash, NN<Text::String> s, NN<T> val)
	{
		if (index > this->cnt)
		{
			index = this->cnt;
		}
		if (this->cnt == this->capacity)
		{
			this->capacity = this->capacity << 1;
			FastStringNNItem<T> *newItems = MemAlloc(FastStringNNItem<T>, this->capacity);
			if (index > 0)
			{
				MemCopyNO(newItems, this->items, sizeof(FastStringNNItem<T>) * index);
			}
			newItems[index].hash = hash;
			newItems[index].s = s;
			newItems[index].val = val;
			if (index < this->cnt)
			{
				MemCopyNO(&newItems[index + 1], &this->items[index], sizeof(FastStringNNItem<T>) * (this->cnt - index));
			}
			MemFree(this->items);
			this->items = newItems;
			this->cnt++;
		}
		else
		{
			if (index < this->cnt)
			{
				MemCopyO(&this->items[index + 1], &this->items[index], sizeof(FastStringNNItem<T>) * (this->cnt - index));
			}
			this->items[index].hash = hash;
			this->items[index].s = s;
			this->items[index].val = val;
			this->cnt++;
		}
	}

	template <class T> FastStringMapNN<T>::FastStringMapNN()
	{
		this->capacity = 64;
		this->cnt = 0;
		this->items = MemAlloc(FastStringNNItem<T>, this->capacity);
	}

	template <class T> FastStringMapNN<T>::FastStringMapNN(const FastStringMapNN<T> *map)
	{
		this->capacity = map->capacity;
		this->cnt = map->cnt;
		this->items = MemAlloc(FastStringNNItem<T>, this->capacity);
		UOSInt i = 0;
		UOSInt j = this->cnt;
		while (i < j)
		{
			this->items[i].hash = map->items[i].hash;
			this->items[i].s = map->items[i].s->Clone();
			this->items[i].val = map->items[i].val;
			i++;
		}
	}

	template <class T> FastStringMapNN<T>::~FastStringMapNN()
	{
		UOSInt i = this->cnt;
		while (i-- > 0)
		{
			this->items[i].s->Release();
		}
		MemFree(this->items);
	}

	template <class T> UOSInt FastStringMapNN<T>::GetCount() const
	{
		return this->cnt;
	}

	template <class T> Optional<T> FastStringMapNN<T>::GetItem(UOSInt index) const
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->items[index].val;
	}

	template <class T> NN<T> FastStringMapNN<T>::GetItemNoCheck(UOSInt index) const
	{
		return this->items[index].val;
	}

	template <class T> Text::String *FastStringMapNN<T>::GetKey(UOSInt index) const
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->items[index].s.Ptr();
	}

	template <class T> OSInt FastStringMapNN<T>::IndexOf(UInt32 hash, const UTF8Char *s, UOSInt len) const
	{
		OSInt i;
		OSInt j;
		OSInt k;
//		OSInt l;
		i = 0;
		j = (OSInt)this->cnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			if (this->items[k].hash == hash)
			{
				while (k > 0 && this->items[k - 1].hash == hash)
				{
					k--;
				}
				while (true)
				{
					if (this->items[k].s->Equals(s, len))
					{
						return k;
					}
					if (++k >= j || this->items[k].hash != hash)
					{
						return ~k;
					}
				}
/*				l = this->items[k].s->CompareToFast({s, len});
				if (l > 0)
				{
					j = k - 1;
				}
				else if (l < 0)
				{
					i = k + 1;
				}
				else
				{
					return k;
				}*/
/*				if (this->items[k].s->Equals(s, len))
				{
					return k;
				}
				i = k;
				while (i > 0 && this->items[i - 1].hash == hash)
				{
					i--;
					if (this->items[i].s->Equals(s, len))
					{
						return i;
					}
				}
				while (++k < j && this->items[k].hash == hash)
				{
					if (this->items[k].s->Equals(s, len))
					{
						return k;
					}
				}
				return ~k;*/
			}
			else if (this->items[k].hash > hash)
			{
				j = k - 1;
			}
			else
			{
				i = k + 1;
			}
		}
		return -i - 1;
	}

	template <class T> OSInt FastStringMapNN<T>::IndexOf(NN<Text::String> s) const
	{
		UInt32 hash = this->crc.CalcDirect(s->v, s->leng);
		return IndexOf(hash, s->v, s->leng);
	}

	template <class T> OSInt FastStringMapNN<T>::IndexOfC(Text::CStringNN s) const
	{
		UInt32 hash = this->crc.CalcDirect(s.v, s.leng);
		return IndexOf(hash, s.v, s.leng);
	}

	template <class T> Optional<T> FastStringMapNN<T>::Put(Text::String *key, NN<T> val)
	{
		return PutNN(NN<Text::String>::FromPtr(key), val);
	}

	template <class T> Optional<T> FastStringMapNN<T>::PutNN(NN<Text::String> key, NN<T> val)
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v, key->leng);
		if (index < 0)
		{
			this->Insert((UOSInt)~index, hash, key->Clone(), val);
			return 0;
		}
		else
		{
			NN<T> oldVal = this->items[index].val;
			this->items[index].val = val;
			return oldVal;
		}
	}

	template <class T> Optional<T> FastStringMapNN<T>::PutC(Text::CStringNN key, NN<T> val)
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		OSInt index = this->IndexOf(hash, key.v, key.leng);
		if (index < 0)
		{
			this->Insert((UOSInt)~index, hash, Text::String::New(key.v, key.leng), val);
			return 0;
		}
		else
		{
			NN<T> oldVal = this->items[index].val;
			this->items[index].val = val;
			return oldVal;
		}
	}

	template <class T> Optional<T> FastStringMapNN<T>::Get(Text::String *key) const
	{
		return GetNN(NN<Text::String>::FromPtr(key));
	}

	template <class T> Optional<T> FastStringMapNN<T>::GetNN(NN<Text::String> key) const
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v, key->leng);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> Optional<T> FastStringMapNN<T>::GetC(Text::CStringNN key) const
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		OSInt index = this->IndexOf(hash, key.v, key.leng);
		if (index >= 0)
		{
			return this->items[index].val;
		}
		return 0;
	}

	template <class T> Optional<T> FastStringMapNN<T>::Remove(Text::String *key)
	{
		return RemoveNN(NN<Text::String>::FromPtr(key));
	}

	template <class T> Optional<T> FastStringMapNN<T>::RemoveNN(NN<Text::String> key)
	{
		UInt32 hash = this->crc.CalcDirect(key->v, key->leng);
		OSInt index = this->IndexOf(hash, key->v, key->leng);
		if (index >= 0)
		{
			NN<T> oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringNNItem<T>) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> Optional<T> FastStringMapNN<T>::RemoveC(Text::CStringNN key)
	{
		UInt32 hash = this->crc.CalcDirect(key.v, key.leng);
		OSInt index = this->IndexOf(hash, key.v, key.leng);
		if (index >= 0)
		{
			NN<T> oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringNNItem<T>) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> Optional<T> FastStringMapNN<T>::RemoveAt(UOSInt index)
	{
		if (index < this->cnt)
		{
			NN<T> oldVal = this->items[index].val;
			this->items[index].s->Release();
			this->cnt--;
			if ((UOSInt)index < this->cnt)
			{
				MemCopyO(&this->items[index], &this->items[index + 1], sizeof(FastStringNNItem<T>) * (this->cnt - (UOSInt)index));
			}
			return oldVal;
		}
		return 0;
	}

	template <class T> Bool FastStringMapNN<T>::IsEmpty() const
	{
		return this->cnt == 0;
	}

	template <class T> void FastStringMapNN<T>::Clear()
	{
		UOSInt i = this->cnt;
		while (i-- > 0)
		{
			this->items[i].s->Release();
		}
		this->cnt = 0;
	}

	template <class T> FastStringNNKeyIterator<T> FastStringMapNN<T>::KeyIterator() const
	{
		return FastStringNNKeyIterator<T>(this->items, this->cnt);
	}

	template <class T> UInt32 FastStringMapNN<T>::CalcHash(const UTF8Char *s, UOSInt len) const
	{
		return this->crc.CalcDirect(s, len);
	}
}

#endif
