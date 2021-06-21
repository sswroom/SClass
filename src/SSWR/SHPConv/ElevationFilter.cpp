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

Bool SSWR::SHPConv::ElevationFilter::IsValid(Double left, Double top, Double right, Double bottom, DB::DBReader *dbf)
{
	Int32 v = Math::Double2Int32(dbf->GetDbl(this->colIndex));
	return v != 0 && (v % this->value) == 0;
}

UTF8Char *SSWR::SHPConv::ElevationFilter::ToString(UTF8Char *buff)
{
	buff = Text::StrConcat(buff, (const UTF8Char*)"Compare column ");
	buff = Text::StrUOSInt(buff, this->colIndex);
	buff = Text::StrConcat(buff, (const UTF8Char*)" every ");
	buff = Text::StrInt32(buff, this->value);
	return buff;
}

SSWR::SHPConv::MapFilter *SSWR::SHPConv::ElevationFilter::Clone()
{
	MapFilter *filter;
	NEW_CLASS(filter, ElevationFilter(this->colIndex, this->value));
	return filter;
}
