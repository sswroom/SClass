#ifndef _SM_DATA_SYNCINTEGER64MAP
#define _SM_DATA_SYNCINTEGER64MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt64.h"
#include "Data/SyncArrayMap.h"

namespace Data
{
	template <class T> class SyncInteger64Map : public SyncArrayMap<Int64, T>
	{
	public:
		SyncInteger64Map();
		virtual ~SyncInteger64Map();
	};

	template <class T> SyncInteger64Map<T>::SyncInteger64Map() : SyncArrayMap<Int64, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListInt64());
	}

	template <class T> SyncInteger64Map<T>::~SyncInteger64Map()
	{
		DEL_CLASS(this->keys);
	}
}

#endif
