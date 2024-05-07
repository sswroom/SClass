#ifndef _SM_DATA_READINGLISTARR
#define _SM_DATA_READINGLISTARR
#include "Data/ReadingList.h"
namespace Data
{
	template <class T> class ReadingListArr : public Data::ReadingList<UnsafeArrayOpt<T>>
	{
	public:
		virtual UOSInt GetCount() const = 0;
		virtual UnsafeArrayOpt<T> GetItem(UOSInt index) const = 0;
		virtual UnsafeArray<T> GetItemNoCheck(UOSInt index) const = 0;
	};
}
#endif
