#ifndef _SM_DATA_STRINGMAPNATIVE
#define _SM_DATA_STRINGMAPNATIVE
#include "MyMemory.h"
#include "Data/ArrayCmpMapNative.hpp"
#include "Data/ArrayListString.h"
#include "Text/CString.h"

namespace Data
{
	template <class T> class StringMapNative : public ArrayCmpMapNative<Optional<Text::String>, T>
	{
	public:
		StringMapNative();
		StringMapNative(NN<const StringMapNative<T>> map);
		virtual ~StringMapNative();

		virtual T Put(Optional<Text::String> key, T val);
		T PutNN(NN<Text::String> key, T val);
		T Put(Text::CStringNN key, T val);
		virtual T Get(Optional<Text::String> key) const;
		T GetNN(NN<Text::String> key) const;
		T Get(Text::CStringNN key) const;
		virtual T Remove(Optional<Text::String> key);
		T RemoveNN(NN<Text::String> key);
		T Remove(Text::CStringNN key);
		IntOS GetIndexC(Text::CStringNN key) const;
		Bool ContainsKeyC(Text::CStringNN key) const;
		virtual Optional<Text::String> GetKey(UIntOS index) const;
		virtual void Clear();
		virtual NN<StringMapNative<T>> Clone() const;
	};


	template <class T> StringMapNative<T>::StringMapNative() : ArrayCmpMapNative<Optional<Text::String>, T>(NEW_CLASS_D(Data::ArrayListString()))
	{
	}

	template <class T> StringMapNative<T>::StringMapNative(NN<const StringMapNative<T>> map) : ArrayCmpMapNative<Optional<Text::String>, T>(NEW_CLASS_D(Data::ArrayListString()))
	{
		UIntOS i = 0;
		UIntOS j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(Text::String::CopyOrNull(map->keys->GetItem(i)));
			this->vals.Add(map->vals.GetItem(i));
			i++;
		}
	}

	template <class T> StringMapNative<T>::~StringMapNative()
	{
		NN<Text::String> s;
		UIntOS i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->GetItem(i).SetTo(s))
				s->Release();
		}
		this->keys.Delete();
	}

	template <class T> T StringMapNative<T>::Put(Optional<Text::String> key, T val)
	{
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
			this->keys->Insert((UIntOS)~i, Text::String::CopyOrNull(key));
			this->vals.Insert((UIntOS)~i, val);
			return 0;
		}
	}

	template <class T> T StringMapNative<T>::PutNN(NN<Text::String> key, T val)
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			T oldVal = this->vals.GetItem((UIntOS)i);
            this->vals.SetItem((UIntOS)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UIntOS)~i, key->Clone());
			this->vals.Insert((UIntOS)~i, val);
			return 0;
		}
	}

	template <class T> T StringMapNative<T>::Put(Text::CStringNN key, T val)
	{
		IntOS i;
		i = NN<Data::ArrayListString>::ConvertFrom(this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			T oldVal = this->vals.GetItem((UIntOS)i);
            this->vals.SetItem((UIntOS)i, val);
			return oldVal;
		}
		else
		{
			this->keys->Insert((UIntOS)~i, Text::String::New(key.v, key.leng));
			this->vals.Insert((UIntOS)~i, val);
			return 0;
		}
	}

	template <class T> T StringMapNative<T>::Get(Optional<Text::String> key) const
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

	template <class T> T StringMapNative<T>::GetNN(NN<Text::String> key) const
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			return this->vals.GetItem((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMapNative<T>::Get(Text::CStringNN key) const
	{
		IntOS i;
		i = NN<Data::ArrayListString>::ConvertFrom(this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			return this->vals.GetItem((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMapNative<T>::Remove(Optional<Text::String> key)
	{
		NN<Text::String> s;
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			if (this->keys->RemoveAt((UIntOS)i).SetTo(s))
				s->Release();
			return this->vals.RemoveAt((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMapNative<T>::RemoveNN(NN<Text::String> key)
	{
		NN<Text::String> s;
		IntOS i;
		i = this->keys->SortedIndexOf(key.Ptr());
		if (i >= 0)
		{
			if (this->keys->RemoveAt((UIntOS)i).SetTo(s))
				s->Release();
			return this->vals.RemoveAt((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> T StringMapNative<T>::Remove(Text::CStringNN key)
	{
		NN<Text::String> s;
		IntOS i;
		i = ((Data::ArrayListString*)this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			if (this->keys->RemoveAt((UIntOS)i).SetTo(s))
				s->Release();
			return this->vals.RemoveAt((UIntOS)i);
		}
		else
		{
			return 0;
		}
	}

	template <class T> Optional<Text::String> StringMapNative<T>::GetKey(UIntOS index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T> IntOS StringMapNative<T>::GetIndexC(Text::CStringNN key) const
	{
		IntOS i;
		i = NN<Data::ArrayListString>::ConvertFrom(this->keys)->SortedIndexOfPtr(key.v, key.leng);
		return i;
	}
	
	template <class T> Bool StringMapNative<T>::ContainsKeyC(Text::CStringNN key) const
	{
		IntOS i;
		i = NN<Data::ArrayListString>::ConvertFrom(this->keys)->SortedIndexOfPtr(key.v, key.leng);
		return i >= 0;
	}
	template <class T> void StringMapNative<T>::Clear()
	{
		NN<Text::String> s;
		UIntOS i;
		i = this->keys->GetCount();
		while (i-- > 0)
		{
			if (this->keys->RemoveAt(i).SetTo(s))
				s->Release();
		}
		this->vals.Clear();
	}

	template <class T> NN<StringMapNative<T>> StringMapNative<T>::Clone() const
	{
		NN<StringMapNative<T>> ret;
		NEW_CLASSNN(ret, StringMapNative<T>(NNTHIS));
		return ret;
	}
}

#endif
