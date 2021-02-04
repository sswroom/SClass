#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/PageRequest.h"
#include "Text/MyString.h"

DB::PageRequest::PageRequest(UOSInt pageNum, UOSInt pageSize)
{
	this->pageNum = pageNum;
	this->pageSize = pageSize;
	this->sortDescList = 0;
	this->sortList = 0;
}

DB::PageRequest::~PageRequest()
{
	if (this->sortList)
	{
		DEL_LIST_FUNC(this->sortList, Text::StrDelNew);
		DEL_CLASS(this->sortList);
		DEL_CLASS(this->sortDescList);
	}
}

UOSInt DB::PageRequest::GetPageNum()
{
	return this->pageNum;
}

UOSInt DB::PageRequest::GetPageSize()
{
	return this->pageSize;
}

void DB::PageRequest::Sort(const UTF8Char *colName, Bool descending)
{
	if (this->sortList == 0)
	{
		NEW_CLASS(this->sortList, Data::ArrayList<const UTF8Char*>());
		NEW_CLASS(this->sortDescList, Data::ArrayList<Bool>());
	}
	this->sortList->Add(Text::StrCopyNew(colName));
	this->sortDescList->Add(descending);
}

UOSInt DB::PageRequest::GetSortingCount()
{
	if (this->sortList == 0)
	{
		return 0;
	}
	return this->sortList->GetCount();
}

const UTF8Char *DB::PageRequest::GetSortColumn(UOSInt index)
{
	return this->sortList->GetItem(index);
}

Bool DB::PageRequest::IsSortDesc(UOSInt index)
{
	return this->sortDescList->GetItem(index);
}
