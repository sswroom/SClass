#ifndef _SM_DATA_LISTMAPNN
#define _SM_DATA_LISTMAPNN
#include "Data/DataMapNN.h"
#include "Data/ReadingList.h"

namespace Data
{
	template <class T, class V> class ListMapNN : public DataMapNN<T, V>, public ReadingList<Optional<V>>
	{
	public:
		virtual ~ListMapNN() {};

		virtual T GetKey(UOSInt index) const = 0;
		void PutAll(NotNullPtr<const ListMapNN<T,V>> map);
	};

	template <class T, class V> void ListMapNN<T, V>::PutAll(NotNullPtr<const ListMapNN<T,V>> map)
	{
		UOSInt i;
		UOSInt j;
		NotNullPtr<V> v;
		i = 0;
		j = map->GetCount();
		while (i < j)
		{
			if (map->GetItem(i).SetTo(v))
			{
				this->Put(map->GetKey(i), v);
			}
			i++;
		}
	}
}

#endif
