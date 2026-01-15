#include "Stdafx.h"
#include "SSWR/SHPConv/ValueFilter.h"

SSWR::SHPConv::ValueFilter::ValueFilter(UIntOS colIndex, Text::CStringNN val, Int32 compareType)
{
	this->colIndex = colIndex;
	this->value = Text::String::New(val);
	this->compareType = compareType;
}

SSWR::SHPConv::ValueFilter::~ValueFilter()
{
	this->value->Release();
}

Bool SSWR::SHPConv::ValueFilter::IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!dbf->GetStr(this->colIndex, sbuff, sizeof(sbuff)).SetTo(sptr))
		return false;
	switch (this->compareType)
	{
	case 3:
		return Text::StrEqualsICaseC(sbuff, (UIntOS)(sptr - sbuff), this->value->v, this->value->leng);
	case 2:
		return !Text::StrEqualsICaseC(sbuff, (UIntOS)(sptr - sbuff), this->value->v, this->value->leng);
	case 1:
		return Text::StrStartsWithICaseC(sbuff, (UIntOS)(sptr - sbuff), this->value->v, this->value->leng);
	case 0:
	default:
		return !Text::StrStartsWithICaseC(sbuff, (UIntOS)(sptr - sbuff), this->value->v, this->value->leng);
	}
}

UnsafeArray<UTF8Char> SSWR::SHPConv::ValueFilter::ToString(UnsafeArray<UTF8Char> buff) const
{
	buff = Text::StrConcatC(buff, UTF8STRC("Compare column "));
	buff = Text::StrUIntOS(buff, this->colIndex);
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

NN<SSWR::SHPConv::MapFilter> SSWR::SHPConv::ValueFilter::Clone() const
{
	NN<MapFilter> filter;
	NEW_CLASSNN(filter, ValueFilter(this->colIndex, this->value->ToCString(), this->compareType));
	return filter;
}
