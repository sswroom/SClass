#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganTripList.h"
#include "Text/MyString.h"

SSWR::OrganMgr::Trip::Trip(Data::DateTime *fromDate, Data::DateTime *toDate, Int32 locId)
{
	this->fromDate = fromDate->ToUnixTimestamp();
	this->toDate = toDate->ToUnixTimestamp();
	this->locId = locId;
}

SSWR::OrganMgr::Trip::~Trip()
{
}

UTF8Char *SSWR::OrganMgr::Trip::ToString(UTF8Char *sbuff)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	dt.SetUnixTimestamp(this->fromDate);
	dt2.SetUnixTimestamp(this->toDate);
	dt.ToLocalTime();
	dt2.ToLocalTime();
	return dt2.ToString(Text::StrConcatC(dt.ToString(sbuff, "yyyy-MM-dd HH:mm"), UTF8STRC(" - ")), "yyyy-MM-dd HH:mm");
}

SSWR::OrganMgr::Location::Location(Int32 id, Int32 parId, Text::CString ename, Text::CString cname, Int32 locType)
{
	this->id = id;
	this->parId = parId;
	this->ename = Text::String::New(ename);
	this->cname = Text::String::New(cname);
	this->locType = locType;
}

SSWR::OrganMgr::Location::~Location()
{
	this->ename->Release();
	this->cname->Release();
}

UTF8Char *SSWR::OrganMgr::Location::ToString(UTF8Char *sbuff)
{
	return this->cname->ConcatTo(sbuff);
}

SSWR::OrganMgr::LocationType::LocationType(Int32 id, Text::CString engName, Text::CString chiName)
{
	this->id = id;
	this->engName = Text::String::New(engName);
	this->chiName = Text::String::New(chiName);
}

SSWR::OrganMgr::LocationType::~LocationType()
{
	this->engName->Release();
	this->chiName->Release();
}


