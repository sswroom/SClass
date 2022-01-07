#include "Stdafx.h"
#include "SSWR/SHPConv/ValueFilter.h"

SSWR::SHPConv::ValueFilter::ValueFilter(UOSInt colIndex, const UTF8Char *val, Int32 compareType)
{
	this->colIndex = colIndex;
	this->value = Text::StrCopyNew(val);
	this->compareType = compareType;
}

SSWR::SHPConv::ValueFilter::~ValueFilter()
{
	Text::StrDelNew(this->value);
}

Bool SSWR::SHPConv::ValueFilter::IsValid(Double left, Double top, Double right, Double bottom, DB::DBReader *dbf)
{
	UTF8Char sbuff[256];
	dbf->GetStr(this->colIndex, sbuff, sizeof(sbuff));
	switch (this->compareType)
	{
	case 3:
		return Text::StrEqualsICase(sbuff, this->value);
	case 2:
		return !Text::StrEqualsICase(sbuff, this->value);
	case 1:
		return Text::StrStartsWithICase(sbuff, this->value);
	case 0:
	default:
		return !Text::StrStartsWithICase(sbuff, this->value);
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
		buff = Text::StrConcat(buff, this->value);
		return buff;
	case 2:
		buff = Text::StrConcatC(buff, UTF8STRC(" not equal to "));
		buff = Text::StrConcat(buff, this->value);
		return buff;
	case 1:
		buff = Text::StrConcatC(buff, UTF8STRC(" starts with "));
		buff = Text::StrConcat(buff, this->value);
		return buff;
	case 0:
	default:
		buff = Text::StrConcatC(buff, UTF8STRC(" not starts with "));
		buff = Text::StrConcat(buff, this->value);
		return buff;
	}
}

SSWR::SHPConv::MapFilter *SSWR::SHPConv::ValueFilter::Clone()
{
	MapFilter *filter;
	NEW_CLASS(filter, ValueFilter(this->colIndex, this->value, this->compareType));
	return filter;
}
