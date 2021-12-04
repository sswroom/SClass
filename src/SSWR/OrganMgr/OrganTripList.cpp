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
	return dt2.ToString(Text::StrConcat(dt.ToString(sbuff, "yyyy-MM-dd HH:mm"), (const UTF8Char*)" - "), "yyyy-MM-dd HH:mm");
}

SSWR::OrganMgr::Location::Location(Int32 id, Int32 parId, const UTF8Char *ename, const UTF8Char *cname, Int32 locType)
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

SSWR::OrganMgr::LocationType::LocationType(Int32 id, const UTF8Char *engName, const UTF8Char *chiName)
{
	this->id = id;
	this->engName = Text::StrCopyNew(engName);
	this->chiName = Text::StrCopyNew(chiName);
}

SSWR::OrganMgr::LocationType::~LocationType()
{
	Text::StrDelNew(this->engName);
	Text::StrDelNew(this->chiName);
}


