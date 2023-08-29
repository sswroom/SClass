#ifndef _SM_DATA_ARRAYCOLLECTION
#define _SM_DATA_ARRAYCOLLECTION
#include "Data/ArrayIterator.h"
namespace Data
{
	template <class T> class ArrayCollection
	{
	public:
		virtual T* GetArray(OutParam<UOSInt> arraySize) const = 0;

		ArrayIterator<T> Iterator() const
		{
			UOSInt cnt;
			T *arr = this->GetArray(cnt);
			return ArrayIterator<T>(arr, cnt);
		}
	};
}
#endif
