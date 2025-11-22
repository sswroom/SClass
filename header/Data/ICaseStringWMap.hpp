#ifndef _SM_DATA_ICASESTRINGWMAP
#define _SM_DATA_ICASESTRINGWMAP
#include "Data/StringWMap.hpp"
#include "Data/ArrayListICaseStrW.hpp"

namespace Data
{
	template <class T> class ICaseStringWMap : public StringWMap<T>
	{
	public:
		ICaseStringWMap();
		virtual ~ICaseStringWMap();
	};


	template <class T> ICaseStringWMap<T>::ICaseStringWMap() : StringWMap<T>()
	{
		DEL_CLASS(this->keys);
		NEW_CLASS(this->keys, Data::ArrayListICaseStrW());
	}

	template <class T> ICaseStringWMap<T>::~ICaseStringWMap()
	{
	}
}

#endif
