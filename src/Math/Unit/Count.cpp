#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Count.h"

Double Math::Unit::Count::GetUnitRatio(CountUnit unit)
{
	switch (unit)
	{
	case CU_UNIT:
		return 1.0;
	case CU_KUNIT:
		return 1000.0;
	case CU_KIUNIT:
		return 1024.0;
	case CU_MUNIT:
		return 1000000.0;
	case CU_MIUNIT:
		return 1048576.0;
	case CU_GUNIT:
		return 1000000000.0;
	case CU_GIUNIT:
		return 1073741824.0;
	case CU_TUNIT:
		return 1000000000000.0;
	case CU_TIUNIT:
		return 1099511627776.0;
	}
	return 1;
}

Text::CStringNN Math::Unit::Count::GetUnitShortName(CountUnit unit)
{
	switch (unit)
	{
	case CU_UNIT:
		return CSTR("");
	case CU_KUNIT:
		return CSTR("K");
	case CU_KIUNIT:
		return CSTR("Ki");
	case CU_MUNIT:
		return CSTR("M");
	case CU_MIUNIT:
		return CSTR("Mi");
	case CU_GUNIT:
		return CSTR("G");
	case CU_GIUNIT:
		return CSTR("Gi");
	case CU_TUNIT:
		return CSTR("T");
	case CU_TIUNIT:
		return CSTR("Ti");
	}
	return CSTR("");
}

Text::CStringNN Math::Unit::Count::GetUnitName(CountUnit unit)
{
	switch (unit)
	{
	case CU_UNIT:
		return CSTR("");
	case CU_KUNIT:
		return CSTR("Kilo");
	case CU_KIUNIT:
		return CSTR("Binary Kilo");
	case CU_MUNIT:
		return CSTR("Mega");
	case CU_MIUNIT:
		return CSTR("Binary Mega");
	case CU_GUNIT:
		return CSTR("Giga");
	case CU_GIUNIT:
		return CSTR("Binary Giga");
	case CU_TUNIT:
		return CSTR("Tera");
	case CU_TIUNIT:
		return CSTR("Binary Tera");
	}
	return CSTR("");
}

Double Math::Unit::Count::GetConvertRatio(CountUnit fromUnit, CountUnit toUnit)
{
	return GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

Double Math::Unit::Count::Convert(CountUnit fromUnit, CountUnit toUnit, Double fromValue)
{
	return fromValue * GetConvertRatio(fromUnit, toUnit);
}

UnsafeArray<UTF8Char> Math::Unit::Count::WellFormat(UnsafeArray<UTF8Char> sbuff, Double val)
{
	return WellFormat(sbuff, val, "0.00");
}

UnsafeArray<UTF8Char> Math::Unit::Count::WellFormat(UnsafeArray<UTF8Char> sbuff, Double val, UnsafeArray<const Char> fmt)
{
	if (val == 0)
	{
		*sbuff++ = '0';
		*sbuff = 0;
		return sbuff;
	}
	Double lval = Math_Log10(val);
	if (lval < 0)
	{
		if (lval >= -2)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 100, fmt), UTF8STRC("c"));
		}
		else if (lval >= -3)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1000, fmt), UTF8STRC("m"));
		}
		else if (lval >= -6)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1000000, fmt), UTF8STRC("u"));
		}
		else if (lval >= -9)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1.0E9, fmt), UTF8STRC("n"));
		}
		else if (lval >= -12)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1.0E12, fmt), UTF8STRC("p"));
		}
		else if (lval >= -15)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1.0E15, fmt), UTF8STRC("f"));
		}
		else if (lval >= -18)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1.0E18, fmt), UTF8STRC("a"));
		}
		else if (lval >= -21)
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1.0E21, fmt), UTF8STRC("z"));
		}
		else
		{
			return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val * 1.0E24, fmt), UTF8STRC("y"));
		}
	}
	else if (lval < 3)
	{
		return Text::StrDoubleFmt(sbuff, val, fmt);
	}
	else if (lval < 6)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E3, fmt), UTF8STRC("k"));
	}
	else if (lval < 9)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E6, fmt), UTF8STRC("M"));
	}
	else if (lval < 12)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E9, fmt), UTF8STRC("G"));
	}
	else if (lval < 15)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E12, fmt), UTF8STRC("T"));
	}
	else if (lval < 18)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E15, fmt), UTF8STRC("P"));
	}
	else if (lval < 21)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E18, fmt), UTF8STRC("E"));
	}
	else if (lval < 24)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E21, fmt), UTF8STRC("Z"));
	}
	else
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1.0E24, fmt), UTF8STRC("Y"));
	}
}

UnsafeArray<UTF8Char> Math::Unit::Count::WellFormatBin(UnsafeArray<UTF8Char> sbuff, Double val)
{
	if (val < 1024)
	{
		return Text::StrDoubleFmt(sbuff, val, "0.00");
	}
	else if (val < 1048576)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1024, "0.00"), UTF8STRC("Ki"));
	}
	else if (val < 1073741824)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1048576, "0.00"), UTF8STRC("Mi"));
	}
	else if (val < 1099511627776)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1073741824, "0.00"), UTF8STRC("Gi"));
	}
	else
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1099511627776, "0.00"), UTF8STRC("Ti"));
	}
}
