#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/SHPConv/ElevationFilter.h"

SSWR::SHPConv::ElevationFilter::ElevationFilter(UOSInt colIndex, Int32 value)
{
	this->colIndex = colIndex;
	this->value = value;
}

SSWR::SHPConv::ElevationFilter::~ElevationFilter()
{
}

Bool SSWR::SHPConv::ElevationFilter::IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const
{
	Int32 v = Double2Int32(dbf->GetDblOr(this->colIndex, 0));
	return v != 0 && (v % this->value) == 0;
}

UnsafeArray<UTF8Char> SSWR::SHPConv::ElevationFilter::ToString(UnsafeArray<UTF8Char> buff) const
{
	buff = Text::StrConcatC(buff, UTF8STRC("Compare column "));
	buff = Text::StrUOSInt(buff, this->colIndex);
	buff = Text::StrConcatC(buff, UTF8STRC(" every "));
	buff = Text::StrInt32(buff, this->value);
	return buff;
}

NN<SSWR::SHPConv::MapFilter> SSWR::SHPConv::ElevationFilter::Clone() const
{
	NN<MapFilter> filter;
	NEW_CLASSNN(filter, ElevationFilter(this->colIndex, this->value));
	return filter;
}
