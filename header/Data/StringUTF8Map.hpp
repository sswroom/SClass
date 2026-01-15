#ifndef _SM_DATA_STRINGUTF8MAP
#define _SM_DATA_STRINGUTF8MAP
#include "MyMemory.h"
#include "Data/ArrayCmpMapObj.hpp"
#include "Data/ArrayListStrUTF8.h"
#include "Text/MyString.h"

namespace Data
{
	template <class T> class StringUTF8Map : public ArrayCmpMapObj<UnsafeArrayOpt<const UTF8Char>, T>
	{		
	public:
		StringUTF8Map();
		virtual ~StringUTF8Map();

		virtual T Put(UnsafeArrayOpt<const UTF8Char> key, T val);
		virtual T Get(UnsafeArrayOpt<const UTF8Char> key) const;
		virtual T Remove(UnsafeArrayOpt<const UTF8Char> key);
		virtual UnsafeArrayOpt<const UTF8Char> GetKey(UIntOS index) const;
		virtual void Clear();
	};


	template <class T> StringUTF8Map<T>::StringUTF8Map() : ArrayCmpMapObj<UnsafeArrayOpt<const UTF8Char>, T>(NEW_CLASS_D(Data::ArrayListStrUTF8()))
	{
	}

	template <class T> StringUTF8Map<T>::~StringUTF8Map()
	{
		UnsafeArray<const UTF8Char> nnkey;
		UIntOS i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->GetItem(i).SetTo(nnkey))
			{
				Text::StrDelNew(nnkey);
			}
		}
		this->keys.Delete();
	}

	template <class T> T StringUTF8Map<T>::Put(UnsafeArrayOpt<const UTF8Char> key, T val)
	{
		UnsafeArray<const UTF8Char> nnkey = key.Or(U8STR(""));
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			T oldVal = this->vals.GetItem((UIntOS)i);
            this->vals.SetItem((UIntOS)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UIntOS)~i, Text::StrCopyNew(nnkey));
			this->vals.Insert((UIntOS)~i, val);
			return 0;
		}
	}

	template <class T> T StringUTF8Map<T>::Get(UnsafeArrayOpt<const UTF8Char> key) const
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals.GetItem((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringUTF8Map<T>::Remove(UnsafeArrayOpt<const UTF8Char> key)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			UnsafeArray<const UTF8Char> nnkey;
			if (this->keys->RemoveAt((UIntOS)i).SetTo(nnkey))
			{
				Text::StrDelNew(nnkey);
			}
			return this->vals.RemoveAt((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> UnsafeArrayOpt<const UTF8Char> StringUTF8Map<T>::GetKey(UIntOS index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringUTF8Map<T>::Clear()
	{
		UnsafeArray<const UTF8Char> nnkey;
		UIntOS i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->RemoveAt(i).SetTo(nnkey))
				Text::StrDelNew(nnkey);
		}
		this->vals.Clear();
	}
}

#endif
