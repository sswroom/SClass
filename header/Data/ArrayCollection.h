#ifndef _SM_DATA_ARRAYCOLLECTION
#define _SM_DATA_ARRAYCOLLECTION

namespace Data
{
	template <class T> class ArrayCollection
	{
	public:
		virtual T* GetArray(UOSInt *arraySize) = 0;
	};
}
#endif
