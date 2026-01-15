#ifndef _SM_DATA_ICASESTRINGMAPOBJ
#define _SM_DATA_ICASESTRINGMAPOBJ
#include "Data/ArrayListICaseString.h"
#include "Data/StringMapObj.hpp"

namespace Data
{
	template <class T> class ICaseStringMapObj : public StringMapObj<T>
	{
	public:
		ICaseStringMapObj();
		ICaseStringMapObj(NN<const ICaseStringMapObj<T>> map);
		virtual ~ICaseStringMapObj();

		virtual NN<StringMapObj<T>> Clone() const;
	};


	template <class T> ICaseStringMapObj<T>::ICaseStringMapObj() : StringMapObj<T>()
	{
		this->keys.Delete();
		NEW_CLASSNN(this->keys, Data::ArrayListICaseString());
	}

	template <class T> ICaseStringMapObj<T>::ICaseStringMapObj(NN<const ICaseStringMapObj<T>> map) : StringMapObj<T>()
	{
		this->keys.Delete();
		NEW_CLASSNN(this->keys, Data::ArrayListICaseString());
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(Text::String::CopyOrNull(map->keys->GetItem(i)));
			this->vals.Add(map->vals.GetItem(i));
			i++;
		}
	}

	template <class T> ICaseStringMapObj<T>::~ICaseStringMapObj()
	{
	}

	template <class T> NN<StringMapObj<T>> ICaseStringMapObj<T>::Clone() const
	{
		NN<ICaseStringMapObj<T>> ret;
		NEW_CLASSNN(ret, ICaseStringMapObj<T>(NNTHIS));
		return ret;
	}
}

#endif
