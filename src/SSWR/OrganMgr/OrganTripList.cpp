#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganTripList.h"
#include "Text/MyString.h"

SSWR::OrganMgr::Trip::Trip(const Data::Timestamp &fromDate, const Data::Timestamp &toDate, Int32 locId)
{
	this->fromDate = fromDate;
	this->toDate = toDate;
	this->locId = locId;
}

SSWR::OrganMgr::Trip::~Trip()
{
}

UnsafeArray<UTF8Char> SSWR::OrganMgr::Trip::ToString(UnsafeArray<UTF8Char> sbuff)
{
	return this->toDate.ToLocalTime().ToString(Text::StrConcatC(this->fromDate.ToLocalTime().ToString(sbuff, "yyyy-MM-dd HH:mm"), UTF8STRC(" - ")), "yyyy-MM-dd HH:mm");
}

SSWR::OrganMgr::Location::Location(Int32 id, Int32 parId, Text::CStringNN ename, Text::CStringNN cname, Int32 locType)
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

UnsafeArray<UTF8Char> SSWR::OrganMgr::Location::ToString(UnsafeArray<UTF8Char> sbuff)
{
	return this->cname->ConcatTo(sbuff);
}

SSWR::OrganMgr::LocationType::LocationType(Int32 id, Text::CStringNN engName, Text::CStringNN chiName)
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


