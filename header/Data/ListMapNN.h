#ifndef _SM_DATA_LISTMAPNN
#define _SM_DATA_LISTMAPNN
#include "Data/DataMapNN.h"
#include "Data/ReadingListNN.h"

namespace Data
{
	template <class T, class V> class ListMapNN : public DataMapNN<T, V>, public ReadingListNN<V>
	{
	public:
		typedef void (*FreeFunc)(NotNullPtr<V> v);
	public:
		virtual ~ListMapNN() {};

		virtual T GetKey(UOSInt index) const = 0;
		void PutAll(NotNullPtr<const ListMapNN<T,V>> map);
		void FreeAll(FreeFunc freeFunc);
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

	template <class T, class V> void ListMapNN<T, V>::FreeAll(FreeFunc freeFunc)
	{
		UOSInt i = this->GetCount();
		while (i-- > 0)
		{
			freeFunc(this->GetItemNoCheck(i));
		}
		this->Clear();
	}

}

#endif
