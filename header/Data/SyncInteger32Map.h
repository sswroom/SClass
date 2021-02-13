#ifndef _SM_DATA_SYNCINTEGER32MAP
#define _SM_DATA_SYNCINTEGER32MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt32.h"
#include "Data/SyncArrayMap.h"

namespace Data
{
	template <class T> class SyncInteger32Map : public SyncArrayMap<Int32, T>
	{
	public:
		SyncInteger32Map();
		virtual ~SyncInteger32Map();
	};

	template <class T> SyncInteger32Map<T>::SyncInteger32Map() : SyncArrayMap<Int32, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListInt32());
	}

	template <class T> SyncInteger32Map<T>::~SyncInteger32Map()
	{
		DEL_CLASS(this->keys);
	}
}

#endif
