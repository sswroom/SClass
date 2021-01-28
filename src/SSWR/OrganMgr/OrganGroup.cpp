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
	if (this->cName)
	{
		Text::StrDelNew(this->cName);
	}
	if (this->eName)
	{
		Text::StrDelNew(this->eName);
	}
	if (this->desc)
	{
		Text::StrDelNew(this->desc);
	}
	if (this->idKey)
	{
		Text::StrDelNew(this->idKey);
	}
}

void SSWR::OrganMgr::OrganGroup::SetGroupId(Int32 groupId)
{
	this->groupId = groupId;
}

Int32 SSWR::OrganMgr::OrganGroup::GetGroupId()
{
	return this->groupId;
}

void SSWR::OrganMgr::OrganGroup::SetCName(const UTF8Char *cName)
{
	if (this->cName)
	{
		Text::StrDelNew(this->cName);
	}
	if (cName)
	{
		this->cName = Text::StrCopyNew(cName);
	}
	else
	{
		this->cName = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganGroup::GetCName()
{
	return this->cName;
}

void SSWR::OrganMgr::OrganGroup::SetEName(const UTF8Char *eName)
{
	if (this->eName)
	{
		Text::StrDelNew(this->eName);
	}
	if (eName)
	{
		this->eName = Text::StrCopyNew(eName);
	}
	else
	{
		this->eName = 0;
	}
}

const UTF8Char *SSWR::OrganMgr::OrganGroup::GetEName()
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

void SSWR::OrganMgr::OrganGroup::SetDesc(const UTF8Char *desc)
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

const UTF8Char *SSWR::OrganMgr::OrganGroup::GetDesc()
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

void SSWR::OrganMgr::OrganGroup::SetIDKey(const UTF8Char *idKey)
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

const UTF8Char *SSWR::OrganMgr::OrganGroup::GetIDKey()
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
		sptr = Text::StrConcat(sptr, this->cName);
	}
	if (this->eName)
	{
		*sptr++ = ' ';
		*sptr++ = '(';
		sptr = Text::StrConcat(sptr, this->eName);
		*sptr++ = ')';
	}
	*sptr = 0;
	return sptr;
}

UTF8Char *SSWR::OrganMgr::OrganGroup::GetEngName(UTF8Char *buff)
{
	UTF8Char *sptr = buff;
	sptr = Text::StrConcat(sptr, this->eName);
	return sptr;
}

SSWR::OrganMgr::OrganGroupItem *SSWR::OrganMgr::OrganGroup::Clone()
{
	OrganGroup *newItem;
	NEW_CLASS(newItem, OrganGroup());
	newItem->SetGroupId(this->groupId);
	newItem->SetCName(this->cName);
	newItem->SetEName(this->eName);
	newItem->SetGroupType(this->groupType);
	newItem->SetDesc(this->desc);
	newItem->SetPhotoGroup(this->photoGroup);
	newItem->SetPhotoSpecies(this->photoSpecies);
	newItem->SetIDKey(this->idKey);
	newItem->SetIsDefault(this->isDefault);
	newItem->SetAdminOnly(this->adminOnly);
	return newItem;
}
