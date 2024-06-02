#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganImages.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganImages::OrganImages(NN<OrganImageItem> imgItem, const UTF8Char *srcImgDir)
{
	this->imgItem = imgItem->Clone();
	this->srcImgDir = Text::StrCopyNew(srcImgDir).Ptr();
}

SSWR::OrganMgr::OrganImages::~OrganImages()
{
	this->imgItem.Delete();
	Text::StrDelNew(this->srcImgDir);
}

NN<SSWR::OrganMgr::OrganImageItem> SSWR::OrganMgr::OrganImages::GetImgItem() const
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

UnsafeArray<UTF8Char> SSWR::OrganMgr::OrganImages::GetItemName(UnsafeArray<UTF8Char> buff) const
{
	return this->imgItem->GetDispName()->ConcatTo(buff);
}

UnsafeArray<UTF8Char> SSWR::OrganMgr::OrganImages::GetEngName(UnsafeArray<UTF8Char> buff) const
{
	return this->imgItem->GetDispName()->ConcatTo(buff);
}

NN<SSWR::OrganMgr::OrganGroupItem> SSWR::OrganMgr::OrganImages::Clone() const
{
	NN<OrganImages> newItem;
	NEW_CLASSNN(newItem, OrganImages(this->imgItem, this->srcImgDir));
	return newItem;
}