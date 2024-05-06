#ifndef _SM_DATA_ARRAYCOLLECTION
#define _SM_DATA_ARRAYCOLLECTION
#include "Data/ArrayIterator.h"
namespace Data
{
	template <class T> class ArrayCollection
	{
	public:
		virtual UnsafeArray<T> GetPtr(OutParam<UOSInt> arraySize) const = 0;
		virtual UnsafeArray<T> Ptr() const = 0;
		virtual UOSInt GetCount() const = 0;

		ArrayIterator<T> Iterator() const
		{
			return ArrayIterator<T>(this->Ptr(), this->GetCount());
		}
	};
}
#endif
