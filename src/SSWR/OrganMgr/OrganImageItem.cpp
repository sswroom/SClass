#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganImageItem.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganImageItem::OrganImageItem(Int32 userId)
{
	this->userId = userId;
	this->dispName = nullptr;
	this->isCoverPhoto = false;
	this->photoDate = 0;
	this->rotateType = RotateType::None;
	this->fullName = nullptr;
	this->fileType = FileType::Unknown;
	this->srcURL = nullptr;
	this->imgURL = nullptr;
	this->userFile = nullptr;
	this->webFile = nullptr;
}

SSWR::OrganMgr::OrganImageItem::~OrganImageItem()
{
	OPTSTR_DEL(this->dispName);
	OPTSTR_DEL(this->fullName);
	OPTSTR_DEL(this->srcURL);
	OPTSTR_DEL(this->imgURL);
}

Int32 SSWR::OrganMgr::OrganImageItem::GetUserId() const
{
	return this->userId;
}

void SSWR::OrganMgr::OrganImageItem::SetDispName(Optional<Text::String> dispName)
{
	OPTSTR_DEL(this->dispName);
	this->dispName = Text::String::CopyOrNull(dispName);
}

void SSWR::OrganMgr::OrganImageItem::SetDispName(Text::CString dispName)
{
	OPTSTR_DEL(this->dispName);
	this->dispName = Text::String::NewOrNull(dispName);
}

Optional<Text::String> SSWR::OrganMgr::OrganImageItem::GetDispName() const
{
	return this->dispName;
}

void SSWR::OrganMgr::OrganImageItem::SetIsCoverPhoto(Bool isCoverPhoto)
{
	this->isCoverPhoto = isCoverPhoto;
}

Bool SSWR::OrganMgr::OrganImageItem::GetIsCoverPhoto() const
{
	return this->isCoverPhoto;
}

void SSWR::OrganMgr::OrganImageItem::SetPhotoDate(const Data::Timestamp &photoDate)
{
	this->photoDate = photoDate;
}

Data::Timestamp SSWR::OrganMgr::OrganImageItem::GetPhotoDate() const
{
	return this->photoDate;
}

void SSWR::OrganMgr::OrganImageItem::SetRotateType(RotateType rotateType)
{
	this->rotateType = rotateType;
}

SSWR::OrganMgr::OrganImageItem::RotateType SSWR::OrganMgr::OrganImageItem::GetRotateType() const
{
	return this->rotateType;
}

void SSWR::OrganMgr::OrganImageItem::SetFullName(Optional<Text::String> fullName)
{
	OPTSTR_DEL(this->fullName);
	this->fullName = Text::String::CopyOrNull(fullName);
}

void SSWR::OrganMgr::OrganImageItem::SetFullName(Text::CString fullName)
{
	OPTSTR_DEL(this->fullName);
	this->fullName = Text::String::NewOrNull(fullName);
}

Optional<Text::String> SSWR::OrganMgr::OrganImageItem::GetFullName() const
{
	return this->fullName;
}

void SSWR::OrganMgr::OrganImageItem::SetFileType(FileType fileType)
{
	this->fileType = fileType;
}

SSWR::OrganMgr::OrganImageItem::FileType SSWR::OrganMgr::OrganImageItem::GetFileType() const
{
	return this->fileType;
}

void SSWR::OrganMgr::OrganImageItem::SetSrcURL(Optional<Text::String> srcURL)
{
	OPTSTR_DEL(this->srcURL);
	this->srcURL = Text::String::CopyOrNull(srcURL);
}

void SSWR::OrganMgr::OrganImageItem::SetSrcURL(Text::CString srcURL)
{
	OPTSTR_DEL(this->srcURL);
	this->srcURL = Text::String::NewOrNull(srcURL);
}

Optional<Text::String> SSWR::OrganMgr::OrganImageItem::GetSrcURL() const
{
	return this->srcURL;
}

void SSWR::OrganMgr::OrganImageItem::SetImgURL(Optional<Text::String> imgURL)
{
	OPTSTR_DEL(this->imgURL);
	this->imgURL = Text::String::CopyOrNull(imgURL);
}

void SSWR::OrganMgr::OrganImageItem::SetImgURL(Text::CString imgURL)
{
	OPTSTR_DEL(this->imgURL);
	this->imgURL = Text::String::NewOrNull(imgURL);
}

void SSWR::OrganMgr::OrganImageItem::SetUserFile(Optional<UserFileInfo> userFile)
{
	this->userFile = userFile;
}

Optional<SSWR::OrganMgr::UserFileInfo> SSWR::OrganMgr::OrganImageItem::GetUserFile() const
{
	return this->userFile;
}

void SSWR::OrganMgr::OrganImageItem::SetWebFile(Optional<WebFileInfo> webFile)
{
	this->webFile = webFile;
}

Optional<SSWR::OrganMgr::WebFileInfo> SSWR::OrganMgr::OrganImageItem::GetWebFile() const
{
	return this->webFile;
}

NN<SSWR::OrganMgr::OrganImageItem> SSWR::OrganMgr::OrganImageItem::Clone() const
{
	NN<OrganImageItem> newItem;
	NEW_CLASSNN(newItem, OrganImageItem(this->userId));
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

Optional<Text::String> SSWR::OrganMgr::OrganImageItem::GetImgURL() const
{
	return this->imgURL;
}
