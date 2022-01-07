#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganGroupType.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganGroupType::OrganGroupType(Int32 seq, const UTF8Char *cName, const UTF8Char *eName)
{
	this->seq = seq;
	this->cName = Text::StrCopyNew(cName);
	this->eName = Text::StrCopyNew(eName);
}

SSWR::OrganMgr::OrganGroupType::~OrganGroupType()
{
	Text::StrDelNew(this->cName);
	Text::StrDelNew(this->eName);
}

Int32 SSWR::OrganMgr::OrganGroupType::GetSeq()
{
	return this->seq;
}

const UTF8Char *SSWR::OrganMgr::OrganGroupType::GetCName()
{
	return this->cName;
}

const UTF8Char *SSWR::OrganMgr::OrganGroupType::GetEName()
{
	return this->eName;
}

UTF8Char *SSWR::OrganMgr::OrganGroupType::ToString(UTF8Char *sbuff)
{
	return Text::StrConcat(Text::StrConcatC(Text::StrInt32(sbuff, this->seq), UTF8STRC(" ")), this->cName);
}
