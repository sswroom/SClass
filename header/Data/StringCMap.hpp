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
		virtual const Char *GetKey(UOSInt index);
		virtual void Clear();
	};


	template <class T> StringCMap<T>::StringCMap() : ArrayMap<const Char*, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListStrC());
	}

	template <class T> StringCMap<T>::~StringCMap()
	{
		UOSInt i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew((UTF8Char*)this->keys->GetItem(i));
		}
		DEL_CLASS(this->keys);
	}

	template <class T> T StringCMap<T>::Put(const Char *key, T val)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			T oldVal = this->vals->GetItem((UOSInt)i);
            this->vals->SetItem((UOSInt)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UOSInt)~i, Text::StrCopyNew(key));
			this->vals->Insert((UOSInt)~i, val);
			return 0;
		}
	}

	template <class T> T StringCMap<T>::Get(const Char *key)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals->GetItem((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringCMap<T>::Remove(const Char *key)
	{
		OSInt i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			Text::StrDelNew(this->keys->RemoveAt((UOSInt)i));
			return this->vals->RemoveAt((UOSInt)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> const Char *StringCMap<T>::GetKey(UOSInt index)
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringCMap<T>::Clear()
	{
		UOSInt i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->keys->RemoveAt(i));
		}
		this->vals->Clear();
	}
}

#endif
