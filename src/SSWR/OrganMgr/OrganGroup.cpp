#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganGroup.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganGroup::OrganGroup()
{
	this->groupId = 0;
	this->cName = 0;
	this->eName = 0;
	this->groupType = 0;
	this->desc = 0;
	this->photoGroup = 0;
	this->photoSpecies = 0;
	this->idKey = 0;
	this->isDefault = 0;
	this->adminOnly = false;
}

SSWR::OrganMgr::OrganGroup::~OrganGroup()
{
	SDEL_STRING(this->cName);
	SDEL_STRING(this->eName);
	SDEL_STRING(this->desc);
	SDEL_STRING(this->idKey);
}

void SSWR::OrganMgr::OrganGroup::SetGroupId(Int32 groupId)
{
	this->groupId = groupId;
}

Int32 SSWR::OrganMgr::OrganGroup::GetGroupId()
{
	return this->groupId;
}

void SSWR::OrganMgr::OrganGroup::SetCName(Text::CString cName)
{
	SDEL_STRING(this->cName);
	this->cName = Text::String::NewOrNull(cName);
}

Text::String *SSWR::OrganMgr::OrganGroup::GetCName()
{
	return this->cName;
}

void SSWR::OrganMgr::OrganGroup::SetEName(Text::CString eName)
{
	SDEL_STRING(this->eName);
	this->eName = Text::String::NewOrNull(eName);
}

Text::String *SSWR::OrganMgr::OrganGroup::GetEName()
{
	return this->eName;
}

void SSWR::OrganMgr::OrganGroup::SetGroupType(Int32 groupType)
{
	this->groupType = groupType;
}

Int32 SSWR::OrganMgr::OrganGroup::GetGroupType()
{
	return this->groupType;
}

void SSWR::OrganMgr::OrganGroup::SetDesc(Text::CString desc)
{
	SDEL_STRING(this->desc);
	this->desc = Text::String::NewOrNull(desc);
}

Text::String *SSWR::OrganMgr::OrganGroup::GetDesc()
{
	return this->desc;
}

void SSWR::OrganMgr::OrganGroup::SetPhotoGroup(Int32 photoGroup)
{
	this->photoGroup = photoGroup;
}

Int32 SSWR::OrganMgr::OrganGroup::GetPhotoGroup()
{
	return this->photoGroup;
}

void SSWR::OrganMgr::OrganGroup::SetPhotoSpecies(Int32 photoSpecies)
{
	this->photoSpecies = photoSpecies;
}

Int32 SSWR::OrganMgr::OrganGroup::GetPhotoSpecies()
{
	return this->photoSpecies;
}

void SSWR::OrganMgr::OrganGroup::SetIDKey(Text::CString idKey)
{
	SDEL_STRING(this->idKey);
	this->idKey = Text::String::NewOrNull(idKey);
}

Text::String *SSWR::OrganMgr::OrganGroup::GetIDKey()
{
	return this->idKey;
}

void SSWR::OrganMgr::OrganGroup::SetIsDefault(Bool isDefault)
{
	this->isDefault = isDefault;
}

Bool SSWR::OrganMgr::OrganGroup::GetIsDefault()
{
	return this->isDefault;
}

void SSWR::OrganMgr::OrganGroup::SetAdminOnly(Bool isAdminOnly)
{
	this->adminOnly = isAdminOnly;
}

Bool SSWR::OrganMgr::OrganGroup::GetAdminOnly()
{
	return this->adminOnly;
}

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganGroup::GetItemType()
{
	return SSWR::OrganMgr::OrganGroupItem::IT_GROUP;
}

UTF8Char *SSWR::OrganMgr::OrganGroup::GetItemName(UTF8Char *buff)
{
	UTF8Char *sptr = buff;
	if (this->isDefault)
	{
		*sptr++ = '*';
	}
	*sptr++ = '-';
	if (this->cName)
	{
		sptr = this->cName->ConcatTo(sptr);
	}
	if (this->eName)
	{
		*sptr++ = ' ';
		*sptr++ = '(';
		sptr = this->eName->ConcatTo(sptr);
		*sptr++ = ')';
	}
	*sptr = 0;
	return sptr;
}

UTF8Char *SSWR::OrganMgr::OrganGroup::GetEngName(UTF8Char *buff)
{
	UTF8Char *sptr = buff;
	sptr = this->eName->ConcatTo(sptr);
	return sptr;
}

SSWR::OrganMgr::OrganGroupItem *SSWR::OrganMgr::OrganGroup::Clone()
{
	OrganGroup *newItem;
	NEW_CLASS(newItem, OrganGroup());
	newItem->SetGroupId(this->groupId);
	newItem->SetCName(STR_CSTR(this->cName));
	newItem->SetEName(STR_CSTR(this->eName));
	newItem->SetGroupType(this->groupType);
	newItem->SetDesc(STR_CSTR(this->desc));
	newItem->SetPhotoGroup(this->photoGroup);
	newItem->SetPhotoSpecies(this->photoSpecies);
	newItem->SetIDKey(STR_CSTR(this->idKey));
	newItem->SetIsDefault(this->isDefault);
	newItem->SetAdminOnly(this->adminOnly);
	return newItem;
}
