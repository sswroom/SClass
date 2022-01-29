#include "Stdafx.h"
#include "MyMemory.h"
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

Text::CString Math::Unit::Count::GetUnitShortName(CountUnit unit)
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

Text::CString Math::Unit::Count::GetUnitName(CountUnit unit)
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

Double Math::Unit::Count::Convert(CountUnit fromUnit, CountUnit toUnit, Double fromValue)
{
	return fromValue * GetUnitRatio(fromUnit) / GetUnitRatio(toUnit);
}

UTF8Char *Math::Unit::Count::WellFormat(UTF8Char *sbuff, Double val)
{
	if (val < 1000)
	{
		return Text::StrDoubleFmt(sbuff, val, "0.00");
	}
	else if (val < 1000000)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1000, "0.00"), UTF8STRC("K"));
	}
	else if (val < 1000000000)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1000000, "0.00"), UTF8STRC("M"));
	}
	else if (val < 1000000000000)
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1000000000, "0.00"), UTF8STRC("G"));
	}
	else
	{
		return Text::StrConcatC(Text::StrDoubleFmt(sbuff, val / 1000000000000, "0.00"), UTF8STRC("T"));
	}
}

UTF8Char *Math::Unit::Count::WellFormatBin(UTF8Char *sbuff, Double val)
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
