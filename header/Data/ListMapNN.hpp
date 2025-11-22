#ifndef _SM_DATA_LISTMAPNN
#define _SM_DATA_LISTMAPNN
#include "Data/DataMapNN.hpp"
#include "Data/ReadingListNN.hpp"

namespace Data
{
	template <class T, class V> class ListMapNN : public DataMapNN<T, V>, public ReadingListNN<V>
	{
	public:
		typedef void (__stdcall *FreeFunc)(NN<V> v);
	public:
		virtual ~ListMapNN() {};

		virtual T GetKey(UOSInt index) const = 0;
		void PutAll(NN<const ListMapNN<T,V>> map);
		void FreeAll(FreeFunc freeFunc);
		void DeleteAll();
	};

	template <class T, class V> void ListMapNN<T, V>::PutAll(NN<const ListMapNN<T,V>> map)
	{
		UOSInt i;
		UOSInt j;
		NN<V> v;
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

	template <class T, class V> void ListMapNN<T, V>::DeleteAll()
	{
		UOSInt i = this->GetCount();
		while (i-- > 0)
		{
			this->GetItemNoCheck(i).Delete();
		}
		this->Clear();
	}
}

#endif
