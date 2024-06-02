#include "Stdafx.h"
#include "SSWR/SHPConv/RangeFilter.h"

SSWR::SHPConv::RangeFilter::RangeFilter(Double left, Double top, Double right, Double bottom, Int32 compareType)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->compareType = compareType;
}

SSWR::SHPConv::RangeFilter::~RangeFilter()
{
}

Bool SSWR::SHPConv::RangeFilter::IsValid(Double left, Double top, Double right, Double bottom, NN<DB::DBReader> dbf) const
{
	if (this->right > left && this->left < right && this->bottom > top && this->top < bottom)
	{
		return compareType == 1;
	}
	else
	{
		return compareType != 1;
	}
}

UnsafeArray<UTF8Char> SSWR::SHPConv::RangeFilter::ToString(UnsafeArray<UTF8Char> buff) const
{
	if (this->compareType == 1)
	{
		return Text::StrConcatC(buff, UTF8STRC("Compare range equal"));
	}
	else
	{
		return Text::StrConcatC(buff, UTF8STRC("Compare range not equal"));
	}
}

SSWR::SHPConv::MapFilter *SSWR::SHPConv::RangeFilter::Clone() const
{
	MapFilter *filter;
	NEW_CLASS(filter, RangeFilter(this->left, this->top, this->right, this->bottom, this->compareType));
	return filter;
}
