#ifndef _SM_DB_PAGEREQUEST
#define _SM_DB_PAGEREQUEST
#include "Data/ArrayListArr.hpp"
#include "Data/ArrayListNative.hpp"

namespace DB
{
	class PageRequest
	{
	private:
		UOSInt pageNum;
		UOSInt pageSize;
		Optional<Data::ArrayListArr<const UTF8Char>> sortList;
		Optional<Data::ArrayListNative<Bool>> sortDescList;
	public:
		PageRequest(UOSInt pageNum, UOSInt pageSize);
		~PageRequest();

		UOSInt GetPageNum();
		UOSInt GetPageSize();

		void Sort(UnsafeArray<const UTF8Char> colName, Bool descending);
		UOSInt GetSortingCount();
		UnsafeArrayOpt<const UTF8Char> GetSortColumn(UOSInt index);
		Bool IsSortDesc(UOSInt index);
	};
}
#endif
