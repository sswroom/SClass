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
	OPTSTR_DEL(this->cName);
	OPTSTR_DEL(this->eName);
	OPTSTR_DEL(this->desc);
	OPTSTR_DEL(this->idKey);
}

void SSWR::OrganMgr::OrganGroup::SetGroupId(Int32 groupId)
{
	this->groupId = groupId;
}

Int32 SSWR::OrganMgr::OrganGroup::GetGroupId() const
{
	return this->groupId;
}

void SSWR::OrganMgr::OrganGroup::SetCName(Text::CString cName)
{
	OPTSTR_DEL(this->cName);
	this->cName = Text::String::NewOrNull(cName);
}

Optional<Text::String> SSWR::OrganMgr::OrganGroup::GetCName() const
{
	return this->cName;
}

void SSWR::OrganMgr::OrganGroup::SetEName(Text::CString eName)
{
	OPTSTR_DEL(this->eName);
	this->eName = Text::String::NewOrNull(eName);
}

Optional<Text::String> SSWR::OrganMgr::OrganGroup::GetEName() const
{
	return this->eName;
}

void SSWR::OrganMgr::OrganGroup::SetGroupType(Int32 groupType)
{
	this->groupType = groupType;
}

Int32 SSWR::OrganMgr::OrganGroup::GetGroupType() const
{
	return this->groupType;
}

void SSWR::OrganMgr::OrganGroup::SetDesc(Text::CString desc)
{
	OPTSTR_DEL(this->desc);
	this->desc = Text::String::NewOrNull(desc);
}

Optional<Text::String> SSWR::OrganMgr::OrganGroup::GetDesc() const
{
	return this->desc;
}

void SSWR::OrganMgr::OrganGroup::SetPhotoGroup(Int32 photoGroup)
{
	this->photoGroup = photoGroup;
}

Int32 SSWR::OrganMgr::OrganGroup::GetPhotoGroup() const
{
	return this->photoGroup;
}

void SSWR::OrganMgr::OrganGroup::SetPhotoSpecies(Int32 photoSpecies)
{
	this->photoSpecies = photoSpecies;
}

Int32 SSWR::OrganMgr::OrganGroup::GetPhotoSpecies() const
{
	return this->photoSpecies;
}

void SSWR::OrganMgr::OrganGroup::SetIDKey(Text::CString idKey)
{
	OPTSTR_DEL(this->idKey);
	this->idKey = Text::String::NewOrNull(idKey);
}

Optional<Text::String> SSWR::OrganMgr::OrganGroup::GetIDKey() const
{
	return this->idKey;
}

void SSWR::OrganMgr::OrganGroup::SetIsDefault(Bool isDefault)
{
	this->isDefault = isDefault;
}

Bool SSWR::OrganMgr::OrganGroup::GetIsDefault() const
{
	return this->isDefault;
}

void SSWR::OrganMgr::OrganGroup::SetAdminOnly(Bool isAdminOnly)
{
	this->adminOnly = isAdminOnly;
}

Bool SSWR::OrganMgr::OrganGroup::GetAdminOnly() const
{
	return this->adminOnly;
}

SSWR::OrganMgr::OrganGroupItem::ItemType SSWR::OrganMgr::OrganGroup::GetItemType() const
{
	return SSWR::OrganMgr::OrganGroupItem::IT_GROUP;
}

UTF8Char *SSWR::OrganMgr::OrganGroup::GetItemName(UTF8Char *buff) const
{
	UTF8Char *sptr = buff;
	NN<Text::String> s;
	if (this->isDefault)
	{
		*sptr++ = '*';
	}
	*sptr++ = '-';
	if (this->cName.SetTo(s))
	{
		sptr = s->ConcatTo(sptr);
	}
	if (this->eName.SetTo(s))
	{
		*sptr++ = ' ';
		*sptr++ = '(';
		sptr = s->ConcatTo(sptr);
		*sptr++ = ')';
	}
	*sptr = 0;
	return sptr;
}

UTF8Char *SSWR::OrganMgr::OrganGroup::GetEngName(UTF8Char *buff) const
{
	UTF8Char *sptr = buff;
	sptr = Text::String::OrEmpty(this->eName)->ConcatTo(sptr);
	return sptr;
}

NN<SSWR::OrganMgr::OrganGroupItem> SSWR::OrganMgr::OrganGroup::Clone() const
{
	NN<OrganGroup> newItem;
	NEW_CLASSNN(newItem, OrganGroup());
	newItem->SetGroupId(this->groupId);
	newItem->SetCName(OPTSTR_CSTR(this->cName));
	newItem->SetEName(OPTSTR_CSTR(this->eName));
	newItem->SetGroupType(this->groupType);
	newItem->SetDesc(OPTSTR_CSTR(this->desc));
	newItem->SetPhotoGroup(this->photoGroup);
	newItem->SetPhotoSpecies(this->photoSpecies);
	newItem->SetIDKey(OPTSTR_CSTR(this->idKey));
	newItem->SetIsDefault(this->isDefault);
	newItem->SetAdminOnly(this->adminOnly);
	return newItem;
}
