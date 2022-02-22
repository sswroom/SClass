#include "Stdafx.h"
#include "SSWR/SHPConv/ValueFilter.h"

SSWR::SHPConv::ValueFilter::ValueFilter(UOSInt colIndex, Text::CString val, Int32 compareType)
{
	this->colIndex = colIndex;
	this->value = Text::String::New(val);
	this->compareType = compareType;
}

SSWR::SHPConv::ValueFilter::~ValueFilter()
{
	this->value->Release();
}

Bool SSWR::SHPConv::ValueFilter::IsValid(Double left, Double top, Double right, Double bottom, DB::DBReader *dbf)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = dbf->GetStr(this->colIndex, sbuff, sizeof(sbuff));
	switch (this->compareType)
	{
	case 3:
		return Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), this->value->v, this->value->leng);
	case 2:
		return !Text::StrEqualsICaseC(sbuff, (UOSInt)(sptr - sbuff), this->value->v, this->value->leng);
	case 1:
		return Text::StrStartsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), this->value->v, this->value->leng);
	case 0:
	default:
		return !Text::StrStartsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), this->value->v, this->value->leng);
	}
}

UTF8Char *SSWR::SHPConv::ValueFilter::ToString(UTF8Char *buff)
{
	buff = Text::StrConcatC(buff, UTF8STRC("Compare column "));
	buff = Text::StrUOSInt(buff, this->colIndex);
	switch (this->compareType)
	{
	case 3:
		buff = Text::StrConcatC(buff, UTF8STRC(" equal to "));
		buff = this->value->ConcatTo(buff);
		return buff;
	case 2:
		buff = Text::StrConcatC(buff, UTF8STRC(" not equal to "));
		buff = this->value->ConcatTo(buff);
		return buff;
	case 1:
		buff = Text::StrConcatC(buff, UTF8STRC(" starts with "));
		buff = this->value->ConcatTo(buff);
		return buff;
	case 0:
	default:
		buff = Text::StrConcatC(buff, UTF8STRC(" not starts with "));
		buff = this->value->ConcatTo(buff);
		return buff;
	}
}

SSWR::SHPConv::MapFilter *SSWR::SHPConv::ValueFilter::Clone()
{
	MapFilter *filter;
	NEW_CLASS(filter, ValueFilter(this->colIndex, this->value->ToCString(), this->compareType));
	return filter;
}
