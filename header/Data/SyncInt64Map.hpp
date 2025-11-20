#ifndef _SM_DATA_SYNCINT64MAP
#define _SM_DATA_SYNCINT64MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt64.h"
#include "Data/SyncArrayMap.h"

namespace Data
{
	template <class T> class SyncInt64Map : public SyncArrayMap<Int64, T>
	{
	public:
		SyncInt64Map();
		virtual ~SyncInt64Map();
	};

	template <class T> SyncInt64Map<T>::SyncInt64Map() : SyncArrayMap<Int64, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListInt64());
	}

	template <class T> SyncInt64Map<T>::~SyncInt64Map()
	{
		DEL_CLASS(this->keys);
	}
}

#endif
