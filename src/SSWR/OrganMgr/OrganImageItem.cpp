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
	this->rotateType = RotateType::None;
	this->fullName = 0;
	this->fileType = FileType::Unknown;
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

Int32 SSWR::OrganMgr::OrganImageItem::GetUserId() const
{
	return this->userId;
}

void SSWR::OrganMgr::OrganImageItem::SetDispName(Text::String *dispName)
{
	SDEL_STRING(this->dispName);
	if (dispName) this->dispName = dispName->Clone().Ptr();
}

void SSWR::OrganMgr::OrganImageItem::SetDispName(Text::CString dispName)
{
	SDEL_STRING(this->dispName);
	Text::CStringNN nndispName;
	if (dispName.SetTo(nndispName)) this->dispName = Text::String::New(nndispName).Ptr();
}

Text::String *SSWR::OrganMgr::OrganImageItem::GetDispName() const
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

void SSWR::OrganMgr::OrganImageItem::SetFullName(Text::String *fullName)
{
	SDEL_STRING(this->fullName);
	if (fullName) this->fullName = fullName->Clone().Ptr();
}

void SSWR::OrganMgr::OrganImageItem::SetFullName(Text::CString fullName)
{
	SDEL_STRING(this->fullName);
	Text::CStringNN nnfullName;
	if (fullName.SetTo(nnfullName)) this->fullName = Text::String::New(nnfullName).Ptr();
}

Text::String *SSWR::OrganMgr::OrganImageItem::GetFullName() const
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

void SSWR::OrganMgr::OrganImageItem::SetSrcURL(Text::String *srcURL)
{
	SDEL_STRING(this->srcURL);
	if (srcURL) this->srcURL = srcURL->Clone().Ptr();
}

void SSWR::OrganMgr::OrganImageItem::SetSrcURL(Text::CString srcURL)
{
	SDEL_STRING(this->srcURL);
	Text::CStringNN nnsrcURL;
	if (srcURL.SetTo(nnsrcURL)) this->srcURL = Text::String::New(nnsrcURL).Ptr();
}

Text::String *SSWR::OrganMgr::OrganImageItem::GetSrcURL() const
{
	return this->srcURL;
}

void SSWR::OrganMgr::OrganImageItem::SetImgURL(Text::String *imgURL)
{
	SDEL_STRING(this->imgURL);
	if (imgURL) this->imgURL = imgURL->Clone().Ptr();
}

void SSWR::OrganMgr::OrganImageItem::SetImgURL(Text::CString imgURL)
{
	SDEL_STRING(this->imgURL);
	Text::CStringNN nnimgURL;
	if (imgURL.SetTo(nnimgURL)) this->imgURL = Text::String::New(nnimgURL).Ptr();
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

Text::String *SSWR::OrganMgr::OrganImageItem::GetImgURL() const
{
	return this->imgURL;
}
