#ifndef _SM_DB_PAGEREQUEST
#define _SM_DB_PAGEREQUEST
#include "Data/ArrayListArr.hpp"
#include "Data/ArrayListNative.hpp"

namespace DB
{
	class PageRequest
	{
	private:
		UIntOS pageNum;
		UIntOS pageSize;
		Optional<Data::ArrayListArr<const UTF8Char>> sortList;
		Optional<Data::ArrayListNative<Bool>> sortDescList;
	public:
		PageRequest(UIntOS pageNum, UIntOS pageSize);
		~PageRequest();

		UIntOS GetPageNum();
		UIntOS GetPageSize();

		void Sort(UnsafeArray<const UTF8Char> colName, Bool descending);
		UIntOS GetSortingCount();
		UnsafeArrayOpt<const UTF8Char> GetSortColumn(UIntOS index);
		Bool IsSortDesc(UIntOS index);
	};
}
#endif
