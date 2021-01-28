#ifndef _SM_DATA_STRINGUTF8MAP
#define _SM_DATA_STRINGUTF8MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class StringUTF8Map : public ArrayMap<const UTF8Char*, T>
	{
	public:
		StringUTF8Map();
		virtual ~StringUTF8Map();

		virtual T Put(const UTF8Char *key, T val);
		virtual T Get(const UTF8Char *key);
		virtual T Remove(const UTF8Char *key);
		virtual const UTF8Char *GetKey(OSInt index);
		virtual void Clear();
	};


	template <class T> StringUTF8Map<T>::StringUTF8Map() : ArrayMap<const UTF8Char*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListStrUTF8());
	}

	template <class T> StringUTF8Map<T>::~StringUTF8Map()
	{
		OSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew((UTF8Char*)this->keys->GetItem(i));
		}
		DEL_CLASS(this->keys);
	}

	template <class T> T StringUTF8Map<T>::Put(const UTF8Char *key, T val)
	{
		OSInt i;
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

	template <class T> T StringUTF8Map<T>::Get(const UTF8Char *key)
	{
		OSInt i;
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

	template <class T> T StringUTF8Map<T>::Remove(const UTF8Char *key)
	{
		OSInt i;
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

	template <class T> const UTF8Char *StringUTF8Map<T>::GetKey(OSInt index)
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringUTF8Map<T>::Clear()
	{
		OSInt i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->keys->RemoveAt(i));
		}
		this->vals->Clear();
	}
}

#endif
