#ifndef _SM_DATA_SYNCINT32MAP
#define _SM_DATA_SYNCINT32MAP
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListInt32.h"
#include "Data/SyncArrayMap.h"

namespace Data
{
	template <class T> class SyncInt32Map : public SyncArrayMap<Int32, T>
	{
	public:
		SyncInt32Map();
		virtual ~SyncInt32Map();
	};

	template <class T> SyncInt32Map<T>::SyncInt32Map() : SyncArrayMap<Int32, T>()
	{
		NEW_CLASS(this->keys, Data::ArrayListInt32());
	}

	template <class T> SyncInt32Map<T>::~SyncInt32Map()
	{
		DEL_CLASS(this->keys);
	}
}

#endif
