#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganParentItem.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

SSWR::OrganMgr::OrganParentItem::OrganParentItem()
{
}

SSWR::OrganMgr::OrganParentItem::~OrganParentItem()
{
}

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganParentItem::GetItemType() const
{
	return OrganGroupItem::IT_PARENT;
}

UnsafeArray<UTF8Char> SSWR::OrganMgr::OrganParentItem::GetItemName(UnsafeArray<UTF8Char> buff) const
{
	return Text::StrWChar_UTF8(buff, L"上移");
}

UnsafeArray<UTF8Char> SSWR::OrganMgr::OrganParentItem::GetEngName(UnsafeArray<UTF8Char> buff) const
{
	return Text::StrConcatC(buff, UTF8STRC("Parent"));
}

NN<SSWR::OrganMgr::OrganGroupItem> SSWR::OrganMgr::OrganParentItem::Clone() const
{
	NN<OrganParentItem> newItem;
	NEW_CLASSNN(newItem, OrganParentItem());
	return newItem;
}