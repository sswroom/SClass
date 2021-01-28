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
	if (this->dispName)
	{
		Text::StrDelNew(this->dispName);
	}
	if (this->fullName)
	{
		Text::StrDelNew(this->fullName);
	}
	if (this->srcURL)
	{
		Text::StrDelNew(this->srcURL);
	}
	if (this->imgURL)
	{
		Text::StrDelNew(this->imgURL);
	}
}

Int32 SSWR::OrganMgr::OrganImageItem::GetUserId()
{
	return this->userId;
}

void SSWR::OrganMgr::OrganImageItem::SetDispName(const UTF8Char *dispName)
{
	if (this->dispName)
	{
		Text::StrDelNew(this->dispName);
	}
	if (dispName)
	{
		this->dispName = Text::StrCopyNew(dispName);
	}
	else
	{
		this->dispName = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganImageItem::GetDispName()
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

void SSWR::OrganMgr::OrganImageItem::SetFullName(const UTF8Char *fullName)
{
	if (this->fullName)
	{
		Text::StrDelNew(this->fullName);
	}
	if (fullName)
	{
		this->fullName = Text::StrCopyNew(fullName);
	}
	else
	{
		this->fullName = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganImageItem::GetFullName()
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

void SSWR::OrganMgr::OrganImageItem::SetSrcURL(const UTF8Char *srcURL)
{
	if (this->srcURL)
	{
		Text::StrDelNew(this->srcURL);
	}
	if (srcURL)
	{
		this->srcURL = Text::StrCopyNew(srcURL);
	}
	else
	{
		this->srcURL = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganImageItem::GetSrcURL()
{
	return this->srcURL;
}

void SSWR::OrganMgr::OrganImageItem::SetImgURL(const UTF8Char *imgURL)
{
	if (this->imgURL)
	{
		Text::StrDelNew(this->imgURL);
	}
	if (imgURL)
	{
		this->imgURL = Text::StrCopyNew(imgURL);
	}
	else
	{
		this->imgURL = 0;
	}
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

const UTF8Char *SSWR::OrganMgr::OrganImageItem::GetImgURL()
{
	return this->imgURL;
}
