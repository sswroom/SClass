#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganImages.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganImages::OrganImages(OrganImageItem *imgItem, const UTF8Char *srcImgDir)
{
	this->imgItem = imgItem->Clone();
	this->srcImgDir = Text::StrCopyNew(srcImgDir).Ptr();
}

SSWR::OrganMgr::OrganImages::~OrganImages()
{
	DEL_CLASS(this->imgItem);
	Text::StrDelNew(this->srcImgDir);
}

SSWR::OrganMgr::OrganImageItem *SSWR::OrganMgr::OrganImages::GetImgItem() const
{
	return this->imgItem;
}

const UTF8Char *SSWR::OrganMgr::OrganImages::GetSrcImgDir() const
{
	return this->srcImgDir;
}

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganImages::GetItemType() const
{
	return OrganGroupItem::IT_IMAGE;
}

UTF8Char *SSWR::OrganMgr::OrganImages::GetItemName(UTF8Char *buff) const
{
	return this->imgItem->GetDispName()->ConcatTo(buff);
}

UTF8Char *SSWR::OrganMgr::OrganImages::GetEngName(UTF8Char *buff) const
{
	return this->imgItem->GetDispName()->ConcatTo(buff);
}

SSWR::OrganMgr::OrganGroupItem *SSWR::OrganMgr::OrganImages::Clone() const
{
	OrganImages *newItem;
	NEW_CLASS(newItem, OrganImages(this->imgItem, this->srcImgDir));
	return newItem;
}