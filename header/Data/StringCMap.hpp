#ifndef _SM_DATA_STRINGCMAP
#define _SM_DATA_STRINGC8MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrC.h"
#include "Data/ArrayMap.h"

namespace Data
{
	template <class T> class StringCMap : public ArrayMap<const Char*, T>
	{
	public:
		StringCMap();
		virtual ~StringCMap();

		virtual T Put(const Char *key, T val);
		virtual T Get(const Char *key);
		virtual T Remove(const Char *key);
		virtual const Char *GetKey(UIntOS index);
		virtual void Clear();
	};


	template <class T> StringCMap<T>::StringCMap() : ArrayMap<const Char*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListStrC());
	}

	template <class T> StringCMap<T>::~StringCMap()
	{
		UIntOS i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew((UTF8Char*)this->keys->GetItem(i));
		}
		DEL_CLASS(this->keys);
	}

	template <class T> T StringCMap<T>::Put(const Char *key, T val)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			T oldVal = this->vals->GetItem((UIntOS)i);
            this->vals->SetItem((UIntOS)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UIntOS)~i, Text::StrCopyNew(key));
			this->vals->Insert((UIntOS)~i, val);
			return 0;
		}
	}

	template <class T> T StringCMap<T>::Get(const Char *key)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals->GetItem((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringCMap<T>::Remove(const Char *key)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			Text::StrDelNew(this->keys->RemoveAt((UIntOS)i));
			return this->vals->RemoveAt((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> const Char *StringCMap<T>::GetKey(UIntOS index)
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringCMap<T>::Clear()
	{
		UIntOS i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->keys->RemoveAt(i));
		}
		this->vals->Clear();
	}
}

#endif
