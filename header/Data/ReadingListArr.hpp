#ifndef _SM_DATA_READINGLISTARR
#define _SM_DATA_READINGLISTARR
#include "Data/ReadingList.hpp"
namespace Data
{
	template <class T> class ReadingListArr : public Data::ReadingList<UnsafeArrayOpt<T>>
	{
	public:
		virtual UIntOS GetCount() const = 0;
		virtual UnsafeArrayOpt<T> GetItem(UIntOS index) const = 0;
		virtual UnsafeArray<T> GetItemNoCheck(UIntOS index) const = 0;
	};
}
#endif
