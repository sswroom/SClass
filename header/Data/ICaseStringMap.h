#ifndef _SM_DATA_ICASESTRINGMAP
#define _SM_DATA_ICASESTRINGMAP
#include "Data/ArrayListICaseString.h"
#include "Data/StringMap.h"

namespace Data
{
	template <class T> class ICaseStringMap : public StringMap<T>
	{
	public:
		ICaseStringMap();
		ICaseStringMap(const ICaseStringMap<T> *map);
		virtual ~ICaseStringMap();

		virtual StringMap<T> *Clone() const;
	};


	template <class T> ICaseStringMap<T>::ICaseStringMap() : StringMap<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseString());
	}

	template <class T> ICaseStringMap<T>::ICaseStringMap(const ICaseStringMap<T> *map) : StringMap<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseString());
		UOSInt i = 0;
		UOSInt j = map->keys->GetCount();
		while (i < j)
		{
			this->keys->Add(map->keys->GetItem(i)->Clone());
			this->vals->Add(map->vals->GetItem(i));
			i++;
		}
	}

	template <class T> ICaseStringMap<T>::~ICaseStringMap()
	{
	}

	template <class T> StringMap<T> *ICaseStringMap<T>::Clone() const
	{
		return NEW_CLASS_D(ICaseStringMap<T>(this));
	}
}

#endif
