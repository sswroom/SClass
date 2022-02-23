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
	SDEL_STRING(this->desc)
	SDEL_STRING(this->dirName);
	SDEL_STRING(this->photo);
	SDEL_STRING(this->idKey);
}

void SSWR::OrganMgr::OrganSpecies::SetSpeciesId(Int32 speciesId)
{
	this->speciesId = speciesId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetSpeciesId()
{
	return this->speciesId;
}

void SSWR::OrganMgr::OrganSpecies::SetCName(Text::CString cName)
{
	SDEL_STRING(this->cName);
	this->cName = Text::String::NewOrNull(cName);
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetCName()
{
	return this->cName;
}

void SSWR::OrganMgr::OrganSpecies::SetEName(Text::CString eName)
{
	SDEL_STRING(this->eName);
	this->eName = Text::String::NewOrNull(eName);
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetEName()
{
	return this->eName;
}

void SSWR::OrganMgr::OrganSpecies::SetSName(Text::CString sName)
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

void SSWR::OrganMgr::OrganSpecies::SetDesc(Text::CString desc)
{
	if (this->desc)
	{
		this->desc->Release();
	}
	if (desc.v)
	{
		this->desc = Text::String::New(desc);
	}
	else
	{
		this->desc = 0;
	}
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetDesc()
{
	return this->desc;
}

void SSWR::OrganMgr::OrganSpecies::SetDirName(Text::CString dirName)
{
	if (this->dirName)
	{
		this->dirName->Release();
	}
	if (dirName.v)
	{
		this->dirName = Text::String::New(dirName);
	}
	else
	{
		this->dirName = 0;
	}
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetDirName()
{
	return this->dirName;
}

void SSWR::OrganMgr::OrganSpecies::SetPhoto(Text::CString photo)
{
	if (this->photo)
	{
		this->photo->Release();
	}
	if (photo.v)
	{
		this->photo = Text::String::New(photo);
	}
	else
	{
		this->photo = 0;
	}
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetPhoto()
{
	return this->photo;
}

void SSWR::OrganMgr::OrganSpecies::SetIDKey(Text::CString idKey)
{
	if (this->idKey)
	{
		this->idKey->Release();
	}
	if (idKey.v)
	{
		this->idKey = Text::String::New(idKey);
	}
	else
	{
		this->idKey = 0;
	}
}

Text::String *SSWR::OrganMgr::OrganSpecies::GetIDKey()
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
	newItem->SetCName(STR_CSTR(this->cName));
	newItem->SetEName(STR_CSTR(this->eName));
	newItem->SetSName(STR_CSTR(this->sName));
	newItem->SetGroupId(this->groupId);
	newItem->SetDesc(STR_CSTR(this->desc));
	newItem->SetDirName(STR_CSTR(this->dirName));
	newItem->SetPhoto(STR_CSTR(this->photo));
	newItem->SetIDKey(STR_CSTR(this->idKey));
	newItem->SetFlags(this->flags);
	newItem->SetIsDefault(this->isDefault);
	newItem->SetPhotoId(this->photoId);
	newItem->SetPhotoWId(this->photoWId);
	return newItem;
}