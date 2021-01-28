#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganImages.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganImages::OrganImages(OrganImageItem *imgItem, const UTF8Char *srcImgDir)
{
	this->imgItem = imgItem->Clone();
	this->srcImgDir = Text::StrCopyNew(srcImgDir);
}

SSWR::OrganMgr::OrganImages::~OrganImages()
{
	DEL_CLASS(this->imgItem);
	Text::StrDelNew(this->srcImgDir);
}

SSWR::OrganMgr::OrganImageItem *SSWR::OrganMgr::OrganImages::GetImgItem()
{
	return this->imgItem;
}

const UTF8Char *SSWR::OrganMgr::OrganImages::GetSrcImgDir()
{
	return this->srcImgDir;
}

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganImages::GetItemType()
{
	return OrganGroupItem::IT_IMAGE;
}

UTF8Char *SSWR::OrganMgr::OrganImages::GetItemName(UTF8Char *buff)
{
	return Text::StrConcat(buff, this->imgItem->GetDispName());
}

UTF8Char *SSWR::OrganMgr::OrganImages::GetEngName(UTF8Char *buff)
{
	return Text::StrConcat(buff, this->imgItem->GetDispName());
}

SSWR::OrganMgr::OrganGroupItem *SSWR::OrganMgr::OrganImages::Clone()
{
	OrganImages *newItem;
	NEW_CLASS(newItem, OrganImages(this->imgItem, this->srcImgDir));
	return newItem;
}