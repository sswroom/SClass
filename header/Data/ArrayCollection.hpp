#ifndef _SM_DATA_ARRAYCOLLECTION
#define _SM_DATA_ARRAYCOLLECTION
#include "Data/ArrayIterator.hpp"
namespace Data
{
	template <class T> class ArrayCollection
	{
	public:
		virtual UnsafeArray<T> GetArr(OutParam<UIntOS> arraySize) const = 0;
		virtual UnsafeArray<T> Arr() const = 0;
		virtual UIntOS GetCount() const = 0;

		ArrayIterator<T> Iterator() const
		{
			return ArrayIterator<T>(this->Arr(), this->GetCount());
		}
	};
}
#endif
