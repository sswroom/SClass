#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganSpecies.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganSpecies::OrganSpecies()
{
	this->speciesId = 0;
	this->cName = nullptr;
	this->eName = nullptr;
	this->sName = nullptr;
	this->groupId = 0;
	this->desc = nullptr;
	this->dirName = nullptr;
	this->photo = nullptr;
	this->idKey = nullptr;
	this->isDefault = 0;
	this->flags = 0;
	this->photoId = 0;
	this->mapColor = 0xff4040ff;
	this->photoWId = 0;
}

SSWR::OrganMgr::OrganSpecies::~OrganSpecies()
{
	OPTSTR_DEL(this->cName);
	OPTSTR_DEL(this->eName);
	OPTSTR_DEL(this->sName);
	OPTSTR_DEL(this->desc)
	OPTSTR_DEL(this->dirName);
	OPTSTR_DEL(this->photo);
	OPTSTR_DEL(this->idKey);
}

void SSWR::OrganMgr::OrganSpecies::SetSpeciesId(Int32 speciesId)
{
	this->speciesId = speciesId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetSpeciesId() const
{
	return this->speciesId;
}

void SSWR::OrganMgr::OrganSpecies::SetCName(Text::CString cName)
{
	OPTSTR_DEL(this->cName);
	this->cName = Text::String::NewOrNull(cName);
}

Optional<Text::String> SSWR::OrganMgr::OrganSpecies::GetCName() const
{
	return this->cName;
}

void SSWR::OrganMgr::OrganSpecies::SetEName(Text::CString eName)
{
	OPTSTR_DEL(this->eName);
	this->eName = Text::String::NewOrNull(eName);
}

Optional<Text::String> SSWR::OrganMgr::OrganSpecies::GetEName() const
{
	return this->eName;
}

void SSWR::OrganMgr::OrganSpecies::SetSName(Text::CStringNN sName)
{
	OPTSTR_DEL(this->sName);
	this->sName = Text::String::New(sName);
}

Optional<Text::String> SSWR::OrganMgr::OrganSpecies::GetSName() const
{
	return this->sName;
}

void SSWR::OrganMgr::OrganSpecies::SetGroupId(Int32 groupId)
{
	this->groupId = groupId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetGroupId() const
{
	return this->groupId;
}

void SSWR::OrganMgr::OrganSpecies::SetDesc(Text::CString desc)
{
	OPTSTR_DEL(this->desc);
	this->desc = Text::String::NewOrNull(desc);
}

Optional<Text::String> SSWR::OrganMgr::OrganSpecies::GetDesc() const
{
	return this->desc;
}

void SSWR::OrganMgr::OrganSpecies::SetDirName(Text::CString dirName)
{
	OPTSTR_DEL(this->dirName);
	this->dirName = Text::String::NewOrNull(dirName);
}

Optional<Text::String> SSWR::OrganMgr::OrganSpecies::GetDirName() const
{
	return this->dirName;
}

void SSWR::OrganMgr::OrganSpecies::SetPhoto(Text::CString photo)
{
	OPTSTR_DEL(this->photo);
	this->photo = Text::String::NewOrNull(photo);
}

Optional<Text::String> SSWR::OrganMgr::OrganSpecies::GetPhoto() const
{
	return this->photo;
}

void SSWR::OrganMgr::OrganSpecies::SetIDKey(Text::CString idKey)
{
	OPTSTR_DEL(this->idKey);
	this->idKey = Text::String::NewOrNull(idKey);
}

Optional<Text::String> SSWR::OrganMgr::OrganSpecies::GetIDKey() const
{
	return this->idKey;
}

void SSWR::OrganMgr::OrganSpecies::SetFlags(Int32 flags)
{
	this->flags = flags;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetFlags() const
{
	return this->flags;
}

void SSWR::OrganMgr::OrganSpecies::SetIsDefault(Bool isDefault)
{
	this->isDefault = isDefault;
}

Bool SSWR::OrganMgr::OrganSpecies::GetIsDefault() const
{
	return this->isDefault;
}

void SSWR::OrganMgr::OrganSpecies::SetPhotoId(Int32 photoId)
{
	this->photoId = photoId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetPhotoId() const
{
	return this->photoId;
}

void SSWR::OrganMgr::OrganSpecies::SetPhotoWId(Int32 photoWId)
{
	this->photoWId = photoWId;
}

Int32 SSWR::OrganMgr::OrganSpecies::GetPhotoWId() const
{
	return this->photoWId;
}

void SSWR::OrganMgr::OrganSpecies::SetMapColor(UInt32 mapColor)
{
	this->mapColor = mapColor;
}

UInt32 SSWR::OrganMgr::OrganSpecies::GetMapColor() const
{
	return this->mapColor;
}

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganSpecies::GetItemType() const
{
	return OrganGroupItem::IT_SPECIES;
}

UnsafeArray<UTF8Char> SSWR::OrganMgr::OrganSpecies::GetItemName(UnsafeArray<UTF8Char> buff) const
{
	NN<Text::String> s;
	UnsafeArray<UTF8Char> sptr = buff;
	if (this->isDefault)
	{
		*sptr++ = '*';
	}
	*sptr++ = '+';
	if (this->cName.SetTo(s))
	{
		sptr = s->ConcatTo(sptr);
	}
	if (this->sName.SetTo(s))
	{
		*sptr++ = ' ';
		*sptr++ = '(';
		sptr = s->ConcatTo(sptr);
		*sptr++ = ')';
	}
	*sptr = 0;
	return sptr;
}

UnsafeArray<UTF8Char> SSWR::OrganMgr::OrganSpecies::GetEngName(UnsafeArray<UTF8Char> buff) const
{
	return Text::String::OrEmpty(this->sName)->ConcatTo(buff);
}

NN<SSWR::OrganMgr::OrganGroupItem> SSWR::OrganMgr::OrganSpecies::Clone() const
{
	NN<OrganSpecies> newItem;
	NEW_CLASSNN(newItem, OrganSpecies());
	newItem->SetSpeciesId(this->speciesId);
	newItem->SetCName(OPTSTR_CSTR(this->cName));
	newItem->SetEName(OPTSTR_CSTR(this->eName));
	newItem->SetSName(Text::String::OrEmpty(this->sName)->ToCString());
	newItem->SetGroupId(this->groupId);
	newItem->SetDesc(OPTSTR_CSTR(this->desc));
	newItem->SetDirName(OPTSTR_CSTR(this->dirName));
	newItem->SetPhoto(OPTSTR_CSTR(this->photo));
	newItem->SetIDKey(OPTSTR_CSTR(this->idKey));
	newItem->SetFlags(this->flags);
	newItem->SetIsDefault(this->isDefault);
	newItem->SetPhotoId(this->photoId);
	newItem->SetPhotoWId(this->photoWId);
	return newItem;
}