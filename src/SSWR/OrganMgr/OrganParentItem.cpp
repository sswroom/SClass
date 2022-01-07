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

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganParentItem::GetItemType()
{
	return OrganGroupItem::IT_PARENT;
}

UTF8Char *SSWR::OrganMgr::OrganParentItem::GetItemName(UTF8Char *buff)
{
	return Text::StrWChar_UTF8(buff, L"上移");
}

UTF8Char *SSWR::OrganMgr::OrganParentItem::GetEngName(UTF8Char *buff)
{
	return Text::StrConcatC(buff, UTF8STRC("Parent"));
}

SSWR::OrganMgr::OrganGroupItem *SSWR::OrganMgr::OrganParentItem::Clone()
{
	OrganParentItem *newItem;
	NEW_CLASS(newItem, OrganParentItem());
	return newItem;
}