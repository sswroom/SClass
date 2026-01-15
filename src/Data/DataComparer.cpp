
#include "Stdafx.h"
#include "Data/DataComparer.h"
#include "Data/ReadonlyArray.hpp"
#include "Text/MyString.h"

IntOS Data::DataComparer::Compare(UnsafeArray<const UTF8Char> val1, UnsafeArray<const UTF8Char> val2)
{
	return Text::StrCompare(val1, val2);
}

IntOS Data::DataComparer::Compare(Single val1, Single val2)
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

IntOS Data::DataComparer::Compare(Double val1, Double val2)
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

IntOS Data::DataComparer::Compare(Int32 val1, Int32 val2)
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

IntOS Data::DataComparer::Compare(UInt32 val1, UInt32 val2)
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

IntOS Data::DataComparer::Compare(Int64 val1, Int64 val2)
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

IntOS Data::DataComparer::Compare(UInt64 val1, UInt64 val2)
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

IntOS Data::DataComparer::Compare(Bool val1, Bool val2)
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

IntOS Data::DataComparer::Compare(const UInt8 *val1, UIntOS cnt1, const UInt8 *val2, UIntOS cnt2)
{
	UIntOS cnt = cnt1;
	if (cnt2 < cnt1)
	{
		cnt = cnt1;
	}
	UIntOS i = 0;
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

IntOS Data::DataComparer::Compare(Math::Geometry::Vector2D *val1, Math::Geometry::Vector2D *val2)
{
	IntOS v1 = (IntOS)val1;
	IntOS v2 = (IntOS)val2;
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

IntOS Data::DataComparer::Compare(NN<Math::Geometry::Vector2D> val1, NN<Math::Geometry::Vector2D> val2)
{
	IntOS v1 = (IntOS)val1.Ptr();
	IntOS v2 = (IntOS)val2.Ptr();
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

IntOS Data::DataComparer::Compare(Optional<Data::UUID> val1, Optional<Data::UUID> val2)
{
	if (val1 == val2)
		return 0;
	NN<Data::UUID> uuid1;
	NN<Data::UUID> uuid2;
	if (!val1.SetTo(uuid1))
		return -1;
	else if (!val2.SetTo(uuid2))
		return 1;
	return uuid1->CompareTo(uuid2);
}

IntOS Data::DataComparer::Compare(Text::CString val1, Text::CString val2)
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

IntOS Data::DataComparer::Compare(Text::String *val1, Text::String *val2)
{
	if (val1 == val2)
		return 0;
	else if (val1 == 0)
		return -1;
	else if (val2 == 0)
		return 1;
	return Text::StrCompare(val1->v, val2->v);
}

IntOS Data::DataComparer::Compare(NN<Text::String> val1, NN<Text::String> val2)
{
	return Text::StrCompare(val1->v, val2->v);
}

IntOS Data::DataComparer::Compare(Data::ReadonlyArray<UInt8> *val1, Data::ReadonlyArray<UInt8> *val2)
{
	return Text::StrCompareFastC(val1->GetArray(), val1->GetCount(), val2->GetArray(), val2->GetCount());
}
