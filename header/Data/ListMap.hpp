#ifndef _SM_DATA_LISTMAP
#define _SM_DATA_LISTMAP
#include "Data/DataMap.hpp"
#include "Data/ReadingList.hpp"

namespace Data
{
	template <class T, class V> class ListMap : public DataMap<T, V>, public ReadingList<V>
	{
	public:
		virtual ~ListMap() {};

		virtual T GetKey(UIntOS index) const = 0;
		void PutAll(NN<const ListMap<T,V>> map);
	};

	template <class T, class V> void ListMap<T, V>::PutAll(NN<const ListMap<T,V>> map)
	{
		UIntOS i;
		UIntOS j;
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
