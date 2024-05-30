
#include "Stdafx.h"
#include "Data/DataComparer.h"
#include "Text/MyString.h"

OSInt Data::DataComparer::Compare(const UTF8Char *val1, const UTF8Char *val2)
{
	return Text::StrCompare(val1, val2);
}

OSInt Data::DataComparer::Compare(Single val1, Single val2)
{
	if (val1 > val2)
	{
		return 1;
	}
	else if (val1 < val2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(Double val1, Double val2)
{
	if (val1 > val2)
	{
		return 1;
	}
	else if (val1 < val2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(Int32 val1, Int32 val2)
{
	if (val1 > val2)
	{
		return 1;
	}
	else if (val1 < val2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(UInt32 val1, UInt32 val2)
{
	if (val1 > val2)
	{
		return 1;
	}
	else if (val1 < val2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(Int64 val1, Int64 val2)
{
	if (val1 > val2)
	{
		return 1;
	}
	else if (val1 < val2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(UInt64 val1, UInt64 val2)
{
	if (val1 > val2)
	{
		return 1;
	}
	else if (val1 < val2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(Bool val1, Bool val2)
{
	if (val1 == val2)
	{
		return 0;
	}
	else if (val1)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

OSInt Data::DataComparer::Compare(const UInt8 *val1, UOSInt cnt1, const UInt8 *val2, UOSInt cnt2)
{
	UOSInt cnt = cnt1;
	if (cnt2 < cnt1)
	{
		cnt = cnt1;
	}
	UOSInt i = 0;
	while (i < cnt)
	{
		if (val1[i] > val2[i])
		{
			return 1;
		}
		else if (val1[i] < val2[i])
		{
			return -1;
		}
		i++;
	}
	if (cnt1 > cnt2)
	{
		return 1;
	}
	else if (cnt1 < cnt2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(Math::Geometry::Vector2D *val1, Math::Geometry::Vector2D *val2)
{
	OSInt v1 = (OSInt)val1;
	OSInt v2 = (OSInt)val2;
	if (v1 > v2)
	{
		return 1;
	}
	else if (v1 < v2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

OSInt Data::DataComparer::Compare(Data::UUID* val1, Data::UUID *val2)
{
	if (val1 == val2)
		return 0;
	NN<Data::UUID> uuid1;
	NN<Data::UUID> uuid2;
	if (!uuid1.Set(val1))
		return -1;
	else if (!uuid2.Set(val2))
		return 1;
	return uuid1->CompareTo(uuid2);
}

OSInt Data::DataComparer::Compare(Text::CString val1, Text::CString val2)
{
	UnsafeArray<const UTF8Char> nnval1;
	UnsafeArray<const UTF8Char> nnval2;
	if (val1.v == val2.v)
		return 0;
	else if (!val1.v.SetTo(nnval1))
		return -1;
	else if (!val2.v.SetTo(nnval2))
		return 1;
	return Text::StrCompare(nnval1.Ptr(), nnval2.Ptr());
}

OSInt Data::DataComparer::Compare(Text::String *val1, Text::String *val2)
{
	if (val1 == val2)
		return 0;
	else if (val1 == 0)
		return -1;
	else if (val2 == 0)
		return 1;
	return Text::StrCompare(val1->v, val2->v);
}

OSInt Data::DataComparer::Compare(NN<Text::String> val1, NN<Text::String> val2)
{
	return Text::StrCompare(val1->v, val2->v);
}
