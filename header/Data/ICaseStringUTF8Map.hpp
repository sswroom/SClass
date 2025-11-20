#ifndef _SM_DATA_ICASESTRINGUTF8MAP
#define _SM_DATA_ICASESTRINGUTF8MAP
#include "Data/ArrayListICaseStrUTF8.h"
#include "Data/StringUTF8Map.hpp"

namespace Data
{
	template <class T> class ICaseStringUTF8Map : public StringUTF8Map<T>
	{
	public:
		ICaseStringUTF8Map();
		virtual ~ICaseStringUTF8Map();
	};


	template <class T> ICaseStringUTF8Map<T>::ICaseStringUTF8Map() : StringUTF8Map<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseStrUTF8());
	}

	template <class T> ICaseStringUTF8Map<T>::~ICaseStringUTF8Map()
	{
	}
}

#endif
