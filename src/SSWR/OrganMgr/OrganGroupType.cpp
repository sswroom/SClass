#include "Stdafx.h"
#include "MyMemory.h"
#include "SSWR/OrganMgr/OrganGroupType.h"
#include "Text/MyString.h"

SSWR::OrganMgr::OrganGroupType::OrganGroupType(Int32 seq, Text::CString cName, Text::CString eName)
{
	this->seq = seq;
	this->cName = Text::String::New(cName.v, cName.leng);
	this->eName = Text::String::New(eName.v, eName.leng);
}

SSWR::OrganMgr::OrganGroupType::~OrganGroupType()
{
	this->cName->Release();
	this->eName->Release();
}

Int32 SSWR::OrganMgr::OrganGroupType::GetSeq()
{
	return this->seq;
}

NotNullPtr<Text::String> SSWR::OrganMgr::OrganGroupType::GetCName() const
{
	return this->cName;
}

NotNullPtr<Text::String> SSWR::OrganMgr::OrganGroupType::GetEName() const
{
	return this->eName;
}

UTF8Char *SSWR::OrganMgr::OrganGroupType::ToString(UTF8Char *sbuff)
{
	return this->cName->ConcatTo(Text::StrConcatC(Text::StrInt32(sbuff, this->seq), UTF8STRC(" ")));
}
