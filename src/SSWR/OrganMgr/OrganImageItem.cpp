#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganImageItem.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganImageItem::OrganImageItem(Int32 userId)
{
	this->userId = userId;
	this->dispName = 0;
	this->isCoverPhoto = false;
	this->photoDate = 0;
	this->rotateType = RT_NONE;
	this->fullName = 0;
	this->fileType = FT_UNKNOWN;
	this->srcURL = 0;
	this->imgURL = 0;
	this->userFile = 0;
	this->webFile = 0;
}

SSWR::OrganMgr::OrganImageItem::~OrganImageItem()
{
	SDEL_STRING(this->dispName);
	SDEL_STRING(this->fullName);
	SDEL_STRING(this->srcURL);
	SDEL_STRING(this->imgURL);
}

Int32 SSWR::OrganMgr::OrganImageItem::GetUserId()
{
	return this->userId;
}

void SSWR::OrganMgr::OrganImageItem::SetDispName(Text::String *dispName)
{
	SDEL_STRING(this->dispName);
	if (dispName) this->dispName = dispName->Clone();
}

void SSWR::OrganMgr::OrganImageItem::SetDispName(const UTF8Char *dispName)
{
	SDEL_STRING(this->dispName);
	if (dispName) this->dispName = Text::String::New(dispName);
}

Text::String *SSWR::OrganMgr::OrganImageItem::GetDispName()
{
	return this->dispName;
}

void SSWR::OrganMgr::OrganImageItem::SetIsCoverPhoto(Bool isCoverPhoto)
{
	this->isCoverPhoto = isCoverPhoto;
}

Bool SSWR::OrganMgr::OrganImageItem::GetIsCoverPhoto()
{
	return this->isCoverPhoto;
}

void SSWR::OrganMgr::OrganImageItem::SetPhotoDate(Int64 photoDate)
{
	this->photoDate = photoDate;
}

Int64 SSWR::OrganMgr::OrganImageItem::GetPhotoDate()
{
	return this->photoDate;
}

void SSWR::OrganMgr::OrganImageItem::SetRotateType(RotateType rotateType)
{
	this->rotateType = rotateType;
}

SSWR::OrganMgr::OrganImageItem::RotateType SSWR::OrganMgr::OrganImageItem::GetRotateType()
{
	return this->rotateType;
}

void SSWR::OrganMgr::OrganImageItem::SetFullName(Text::String *fullName)
{
	SDEL_STRING(this->fullName);
	if (fullName) this->fullName = fullName->Clone();
}

void SSWR::OrganMgr::OrganImageItem::SetFullName(const UTF8Char *fullName)
{
	SDEL_STRING(this->fullName);
	if (fullName) this->fullName = Text::String::New(fullName);
}

Text::String *SSWR::OrganMgr::OrganImageItem::GetFullName()
{
	return this->fullName;
}

void SSWR::OrganMgr::OrganImageItem::SetFileType(FileType fileType)
{
	this->fileType = fileType;
}

SSWR::OrganMgr::OrganImageItem::FileType SSWR::OrganMgr::OrganImageItem::GetFileType()
{
	return this->fileType;
}

void SSWR::OrganMgr::OrganImageItem::SetSrcURL(Text::String *srcURL)
{
	SDEL_STRING(this->srcURL);
	if (srcURL) this->srcURL = srcURL->Clone();
}

void SSWR::OrganMgr::OrganImageItem::SetSrcURL(const UTF8Char *srcURL)
{
	SDEL_STRING(this->srcURL);
	if (srcURL) this->srcURL = Text::String::New(srcURL);
}

Text::String *SSWR::OrganMgr::OrganImageItem::GetSrcURL()
{
	return this->srcURL;
}

void SSWR::OrganMgr::OrganImageItem::SetImgURL(Text::String *imgURL)
{
	SDEL_STRING(this->imgURL);
	if (imgURL) this->imgURL = imgURL->Clone();
}

void SSWR::OrganMgr::OrganImageItem::SetImgURL(const UTF8Char *imgURL)
{
	SDEL_STRING(this->imgURL);
	if (imgURL) this->imgURL = Text::String::New(imgURL);
}

void SSWR::OrganMgr::OrganImageItem::SetUserFile(UserFileInfo *userFile)
{
	this->userFile = userFile;
}

SSWR::OrganMgr::UserFileInfo *SSWR::OrganMgr::OrganImageItem::GetUserFile()
{
	return this->userFile;
}

void SSWR::OrganMgr::OrganImageItem::SetWebFile(WebFileInfo *webFile)
{
	this->webFile = webFile;
}

SSWR::OrganMgr::WebFileInfo *SSWR::OrganMgr::OrganImageItem::GetWebFile()
{
	return this->webFile;
}

SSWR::OrganMgr::OrganImageItem *SSWR::OrganMgr::OrganImageItem::Clone()
{
	OrganImageItem *newItem;
	NEW_CLASS(newItem, OrganImageItem(this->userId));
	newItem->SetDispName(this->dispName);
	newItem->SetIsCoverPhoto(this->isCoverPhoto);
	newItem->SetPhotoDate(this->photoDate);
	newItem->SetRotateType(this->rotateType);
	newItem->SetFullName(this->fullName);
	newItem->SetFileType(this->fileType);
	newItem->SetSrcURL(this->srcURL);
	newItem->SetImgURL(this->imgURL);
	newItem->SetUserFile(this->userFile);
	newItem->SetWebFile(this->webFile);
	return newItem;
}

Text::String *SSWR::OrganMgr::OrganImageItem::GetImgURL()
{
	return this->imgURL;
}
