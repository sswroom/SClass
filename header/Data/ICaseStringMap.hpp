#ifndef _SM_DATA_ICASESTRINGMAP
#define _SM_DATA_ICASESTRINGMAP
#include "Data/ArrayListICaseString.h"
#include "Data/StringMap.hpp"

namespace Data
{
	template <class T> class ICaseStringMap : public StringMap<T>
	{
	public:
		ICaseStringMap();
		ICaseStringMap(NN<const ICaseStringMap<T>> map);
		virtual ~ICaseStringMap();

		virtual NN<StringMap<T>> Clone() const;
	};


	template <class T> ICaseStringMap<T>::ICaseStringMap() : StringMap<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseString());
	}

	template <class T> ICaseStringMap<T>::ICaseStringMap(NN<const ICaseStringMap<T>> map) : StringMap<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseString());
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(Text::String::CopyOrNull(map->keys->GetItem(i)));
			this->vals.Add(map->vals.GetItem(i));
			i++;
		}
	}

	template <class T> ICaseStringMap<T>::~ICaseStringMap()
	{
	}

	template <class T> NN<StringMap<T>> ICaseStringMap<T>::Clone() const
	{
		NN<ICaseStringMap<T>> ret;
		NEW_CLASSNN(ret, ICaseStringMap<T>(*this));
		return ret;
	}
}

#endif
