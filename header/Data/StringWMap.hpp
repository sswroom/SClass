#ifndef _SM_DATA_STRINGWMAP
#define _SM_DATA_STRINGWMAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrW.h"
#include "Data/ArrayMap.hpp"

namespace Data
{
	template <class T> class StringWMap : public ArrayMap<const WChar*, T>
	{
	public:
		StringWMap();
		virtual ~StringWMap();

		virtual T Put(const WChar *key, T val);
		virtual T Get(const WChar *key);
		virtual T Remove(const WChar *key);
		virtual const WChar *GetKey(IntOS index);
		virtual void Clear();
	};


	template <class T> StringWMap<T>::StringWMap() : ArrayMap<const WChar*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListStrW());
	}

	template <class T> StringWMap<T>::~StringWMap()
	{
		IntOS i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew((WChar*)this->keys->GetItem(i));
		}
		DEL_CLASS(this->keys);
	}

	template <class T> T StringWMap<T>::Put(const WChar *key, T val)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			T oldVal = this->vals->GetItem(i);
            this->vals->SetItem(i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert(~i, Text::StrCopyNew(key));
			this->vals->Insert(~i, val);
			return 0;
		}
	}

	template <class T> T StringWMap<T>::Get(const WChar *key)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals->GetItem(i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringWMap<T>::Remove(const WChar *key)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			Text::StrDelNew(this->keys->RemoveAt(i));
			return this->vals->RemoveAt(i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> const WChar *StringWMap<T>::GetKey(IntOS index)
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringWMap<T>::Clear()
	{
		IntOS i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->keys->RemoveAt(i));
		}
		this->vals->Clear();
	}
}

#endif
