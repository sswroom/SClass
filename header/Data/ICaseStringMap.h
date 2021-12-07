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
		virtual ~ICaseStringMap();
	};


	template <class T> ICaseStringMap<T>::ICaseStringMap() : StringMap<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseString());
	}

	template <class T> ICaseStringMap<T>::~ICaseStringMap()
	{
	}
}

#endif
