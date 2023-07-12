#ifndef _SM_DATA_ARRAYCOLLECTION
#define _SM_DATA_ARRAYCOLLECTION
#include "Data/ArrayIterator.h"
namespace Data
{
	template <class T> class ArrayCollection
	{
	public:
		virtual T* GetArray(UOSInt *arraySize) = 0;

		ArrayIterator<T> Iterator()
		{
			UOSInt cnt;
			T *arr = this->GetArray(&cnt);
			return ArrayIterator<T>(arr, cnt);
		}
	};
}
#endif
