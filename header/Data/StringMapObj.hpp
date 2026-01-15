#ifndef _SM_DATA_STRINGMAPOBJ
#define _SM_DATA_STRINGMAPOBJ
#include "MyMemory.h"
#include "Data/ArrayCmpMapObj.hpp"
#include "Data/ArrayListString.h"
#include "Text/CString.h"

namespace Data
{
	template <class T> class StringMapObj : public ArrayCmpMapObj<Optional<Text::String>, T>
	{
	public:
		StringMapObj();
		StringMapObj(NN<const StringMapObj<T>> map);
		virtual ~StringMapObj();

		virtual T Put(Optional<Text::String> key, T val);
		T PutNN(NN<Text::String> key, T val);
		T Put(Text::CStringNN key, T val);
		virtual T Get(Optional<Text::String> key) const;
		T GetNN(NN<Text::String> key) const;
		T Get(Text::CStringNN key) const;
		virtual T Remove(Optional<Text::String> key);
		T RemoveNN(NN<Text::String> key);
		T Remove(Text::CStringNN key);
		virtual Optional<Text::String> GetKey(UIntOS index) const;
		virtual void Clear();
		virtual NN<StringMapObj<T>> Clone() const;
	};


	template <class T> StringMapObj<T>::StringMapObj() : ArrayCmpMapObj<Optional<Text::String>, T>(NEW_CLASS_D(Data::ArrayListString()))
	{
	}

	template <class T> StringMapObj<T>::StringMapObj(NN<const StringMapObj<T>> map) : ArrayCmpMapObj<Optional<Text::String>, T>(NEW_CLASS_D(Data::ArrayListString()))
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

	template <class T> StringMapObj<T>::~StringMapObj()
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

	template <class T> T StringMapObj<T>::Put(Optional<Text::String> key, T val)
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
			return nullptr;
		}
	}

	template <class T> T StringMapObj<T>::PutNN(NN<Text::String> key, T val)
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

	template <class T> T StringMapObj<T>::Put(Text::CStringNN key, T val)
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
			return nullptr;
		}
	}

	template <class T> T StringMapObj<T>::Get(Optional<Text::String> key) const
	{
		IntOS i;
		i = this->keys->SortedIndexOf(key);
		if (i >= 0)
		{
			return this->vals.GetItem((UIntOS)i);
		}
		else
		{
			return nullptr;
		}
	}

	template <class T> T StringMapObj<T>::GetNN(NN<Text::String> key) const
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

	template <class T> T StringMapObj<T>::Get(Text::CStringNN key) const
	{
		IntOS i;
		i = NN<Data::ArrayListString>::ConvertFrom(this->keys)->SortedIndexOfPtr(key.v, key.leng);
		if (i >= 0)
		{
			return this->vals.GetItem((UIntOS)i);
		}
		else
		{
			return nullptr;
		}
	}

	template <class T> T StringMapObj<T>::Remove(Optional<Text::String> key)
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
			return nullptr;
		}
	}

	template <class T> T StringMapObj<T>::RemoveNN(NN<Text::String> key)
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

	template <class T> T StringMapObj<T>::Remove(Text::CStringNN key)
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

	template <class T> Optional<Text::String> StringMapObj<T>::GetKey(UIntOS index) const
	{
		return this->keys->GetItem(index);
	}

	template <class T> void StringMapObj<T>::Clear()
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

	template <class T> NN<StringMapObj<T>> StringMapObj<T>::Clone() const
	{
		NN<StringMapObj<T>> ret;
		NEW_CLASSNN(ret, StringMapObj<T>(NNTHIS));
		return ret;
	}
}

#endif
