#ifndef _SM_DATA_LISTMAP
#define _SM_DATA_LISTMAP
#include "Data/DataMap.h"
#include "Data/ReadingList.h"

namespace Data
{
	template <class T, class V> class ListMap : public DataMap<T, V>, public ReadingList<V>
	{
	public:
		virtual ~ListMap() {};

		virtual T GetKey(UOSInt index) const = 0;
		void PutAll(NotNullPtr<const ListMap<T,V>> map);
	};

	template <class T, class V> void ListMap<T, V>::PutAll(NotNullPtr<const ListMap<T,V>> map)
	{
		UOSInt i;
		UOSInt j;
		i = 0;
		j = map->GetCount();
		while (i < j)
		{
			this->Put(map->GetKey(i), map->GetItem(i));
			i++;
		}
	}
}

#endif
