#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/PageRequest.h"
#include "Text/MyString.h"

DB::PageRequest::PageRequest(UOSInt pageNum, UOSInt pageSize)
{
	this->pageNum = pageNum;
	this->pageSize = pageSize;
	this->sortDescList = nullptr;
	this->sortList = nullptr;
}

DB::PageRequest::~PageRequest()
{
	NN<Data::ArrayListArr<const UTF8Char>> sortList;
	NN<Data::ArrayListNative<Bool>> sortDescList;
	if (this->sortList.SetTo(sortList) && this->sortDescList.SetTo(sortDescList))
	{
		NNLIST_CALL_FUNC(sortList, Text::StrDelNew);
		this->sortList.Delete();
		this->sortDescList.Delete();
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

void DB::PageRequest::Sort(UnsafeArray<const UTF8Char> colName, Bool descending)
{
	NN<Data::ArrayListArr<const UTF8Char>> sortList;
	NN<Data::ArrayListNative<Bool>> sortDescList;
	if (!this->sortList.SetTo(sortList) || !this->sortDescList.SetTo(sortDescList))
	{
		NEW_CLASSNN(sortList, Data::ArrayListArr<const UTF8Char>());
		NEW_CLASSNN(sortDescList, Data::ArrayListNative<Bool>());
		this->sortList = sortList;
		this->sortDescList = sortDescList;
	}
	sortList->Add(Text::StrCopyNew(colName).Ptr());
	sortDescList->Add(descending);
}

UOSInt DB::PageRequest::GetSortingCount()
{
	NN<Data::ArrayListArr<const UTF8Char>> sortList;
	if (!this->sortList.SetTo(sortList))
	{
		return 0;
	}
	return sortList->GetCount();
}

UnsafeArrayOpt<const UTF8Char> DB::PageRequest::GetSortColumn(UOSInt index)
{
	NN<Data::ArrayListArr<const UTF8Char>> sortList;
	if (!this->sortList.SetTo(sortList))
	{
		return nullptr;
	}
	return sortList->GetItem(index);
}

Bool DB::PageRequest::IsSortDesc(UOSInt index)
{
	NN<Data::ArrayListNative<Bool>> sortDescList;
	if (!this->sortDescList.SetTo(sortDescList))
	{
		return false;
	}
	return sortDescList->GetItem(index);
}
