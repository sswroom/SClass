#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganSpecies.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganSpecies::OrganSpecies()
{
	this->speciesId = 0;
	this->cName = 0;
	this->eName = 0;
	this->sName = 0;
	this->groupId = 0;
	this->desc = 0;
	this->dirName = 0;
	this->photo = 0;
	this->idKey = 0;
	this->isDefault = 0;
	this->flags = 0;
	this->photoId = 0;
	this->mapColor = 0xff4040ff;
	this->photoWId = 0;
}

SSWR::OrganMgr::OrganSpecies::~OrganSpecies()
{
	SDEL_STRING(this->cName);
	SDEL_STRING(this->eName);
	SDEL_STRING(this->sName);
	if (this->desc)
		Text::StrDelNew(this->desc);
	if (this->dirName)
		Text::StrDelNew(this->dirName);
	if (this->photo)
		Text::StrDelNew(this->photo);
	if (this->idKey)
		Text::StrDelNew(this->idKey);
}

void SSWR::OrganMgr::OrganSpecies::SetSpeciesId(Int32 speciesId)
{
	this->speciesId = speciesId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetSpeciesId()
{
	return this->speciesId;
}

void SSWR::OrganMgr::OrganSpecies::SetCName(const UTF8Char *cName)
{
	SDEL_STRING(this->cName);
	this->cName = Text::String::NewOrNull(cName);
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetCName()
{
	return this->cName;
}

void SSWR::OrganMgr::OrganSpecies::SetEName(const UTF8Char *eName)
{
	SDEL_STRING(this->eName);
	this->eName = Text::String::NewOrNull(eName);
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetEName()
{
	return this->eName;
}

void SSWR::OrganMgr::OrganSpecies::SetSName(const UTF8Char *sName)
{
	SDEL_STRING(this->sName);
	this->sName = Text::String::NewOrNull(sName);
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetSName()
{
	return this->sName;
}

void SSWR::OrganMgr::OrganSpecies::SetGroupId(Int32 groupId)
{
	this->groupId = groupId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetGroupId()
{
	return this->groupId;
}

void SSWR::OrganMgr::OrganSpecies::SetDesc(const UTF8Char *desc)
{
	if (this->desc)
	{
		Text::StrDelNew(this->desc);
	}
	if (desc)
	{
		this->desc = Text::StrCopyNew(desc);
	}
	else
	{
		this->desc = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganSpecies::GetDesc()
{
	return this->desc;
}

void SSWR::OrganMgr::OrganSpecies::SetDirName(const UTF8Char *dirName)
{
	if (this->dirName)
	{
		Text::StrDelNew(this->dirName);
	}
	if (dirName)
	{
		this->dirName = Text::StrCopyNew(dirName);
	}
	else
	{
		this->dirName = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganSpecies::GetDirName()
{
	return this->dirName;
}

void SSWR::OrganMgr::OrganSpecies::SetPhoto(const UTF8Char *photo)
{
	if (this->photo)
	{
		Text::StrDelNew(this->photo);
	}
	if (photo)
	{
		this->photo = Text::StrCopyNew(photo);
	}
	else
	{
		this->photo = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganSpecies::GetPhoto()
{
	return this->photo;
}

void SSWR::OrganMgr::OrganSpecies::SetIDKey(const UTF8Char *idKey)
{
	if (this->idKey)
	{
		Text::StrDelNew(this->idKey);
	}
	if (idKey)
	{
		this->idKey = Text::StrCopyNew(idKey);
	}
	else
	{
		this->idKey = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganSpecies::GetIDKey()
{
	return this->idKey;
}

void SSWR::OrganMgr::OrganSpecies::SetFlags(Int32 flags)
{
	this->flags = flags;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetFlags()
{
	return this->flags;
}

void SSWR::OrganMgr::OrganSpecies::SetIsDefault(Bool isDefault)
{
	this->isDefault = isDefault;
}

Bool SSWR::OrganMgr::OrganSpecies::GetIsDefault()
{
	return this->isDefault;
}

void SSWR::OrganMgr::OrganSpecies::SetPhotoId(Int32 photoId)
{
	this->photoId = photoId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetPhotoId()
{
	return this->photoId;
}

void SSWR::OrganMgr::OrganSpecies::SetPhotoWId(Int32 photoWId)
{
	this->photoWId = photoWId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetPhotoWId()
{
	return this->photoWId;
}

void SSWR::OrganMgr::OrganSpecies::SetMapColor(UInt32 mapColor)
{
	this->mapColor = mapColor;
}

UInt32 SSWR::OrganMgr::OrganSpecies::GetMapColor()
{
	return this->mapColor;
}

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganSpecies::GetItemType()
{
	return OrganGroupItem::IT_SPECIES;
}

UTF8Char *SSWR::OrganMgr::OrganSpecies::GetItemName(UTF8Char *buff)
{
	UTF8Char *sptr = buff;
	if (this->isDefault)
	{
		*sptr++ = '*';
	}
	*sptr++ = '+';
	if (this->cName)
	{
		sptr = this->cName->ConcatTo(sptr);
	}
	if (this->sName)
	{
		*sptr++ = ' ';
		*sptr++ = '(';
		sptr = this->sName->ConcatTo(sptr);
		*sptr++ = ')';
	}
	*sptr = 0;
	return sptr;
}

UTF8Char *SSWR::OrganMgr::OrganSpecies::GetEngName(UTF8Char *buff)
{
	return this->sName->ConcatTo(buff);
}

SSWR::OrganMgr::OrganGroupItem *SSWR::OrganMgr::OrganSpecies::Clone()
{
	OrganSpecies *newItem;
	NEW_CLASS(newItem, OrganSpecies());
	newItem->SetSpeciesId(this->speciesId);
	newItem->SetCName(STR_PTR(this->cName));
	newItem->SetEName(STR_PTR(this->eName));
	newItem->SetSName(STR_PTR(this->sName));
	newItem->SetGroupId(this->groupId);
	newItem->SetDesc(this->desc);
	newItem->SetDirName(this->dirName);
	newItem->SetPhoto(this->photo);
	newItem->SetIDKey(this->idKey);
	newItem->SetFlags(this->flags);
	newItem->SetIsDefault(this->isDefault);
	newItem->SetPhotoId(this->photoId);
	newItem->SetPhotoWId(this->photoWId);
	return newItem;
}