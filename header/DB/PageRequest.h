#ifndef _SM_DB_PAGEREQUEST
#define _SM_DB_PAGEREQUEST
#include "Data/ArrayList.h"

namespace DB
{
	class PageRequest
	{
	private:
		UOSInt pageNum;
		UOSInt pageSize;
		Data::ArrayList<const UTF8Char*> *sortList;
		Data::ArrayList<Bool> *sortDescList;
	public:
		PageRequest(UOSInt pageNum, UOSInt pageSize);
		~PageRequest();

		UOSInt GetPageNum();
		UOSInt GetPageSize();

		void Sort(UnsafeArray<const UTF8Char> colName, Bool descending);
		UOSInt GetSortingCount();
		const UTF8Char *GetSortColumn(UOSInt index);
		Bool IsSortDesc(UOSInt index);
	};
}
#endif
