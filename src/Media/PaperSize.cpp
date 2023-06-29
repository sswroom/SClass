#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Media/PaperSize.h"
#include "Text/MyString.h"

Media::PaperSize::PaperSize(PaperType pType)
{
	this->pType = pType;
	this->sizeMM = PaperTypeGetSizeMM(pType);
}

Media::PaperSize::~PaperSize()
{
}

Double Media::PaperSize::GetWidthMM()
{
	return this->sizeMM.GetWidth();
}

Double Media::PaperSize::GetHeightMM()
{
	return this->sizeMM.GetHeight();
}

Math::Size2DDbl Media::PaperSize::PaperTypeGetSizeMM(PaperType paperType)
{
	Double sqr2 = Math_Sqrt(2);
	Math::Size2DDbl ret;
	switch (paperType)
	{
	case PT_4A0:
		ret.x = Math_Sqrt(4000000 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_2A0:
		ret.x = Math_Sqrt(2000000 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A0:
		ret.x = Math_Sqrt(1000000 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A1:
		ret.x = Math_Sqrt(500000 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A2:
		ret.x = Math_Sqrt(250000 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A3:
		ret.x = Math_Sqrt(125000 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	default:
	case PT_DEFAULT:
	case PT_A4:
		ret.x = Math_Sqrt(62500 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A5:
		ret.x = Math_Sqrt(31250 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A6:
		ret.x = Math_Sqrt(15625 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A7:
		ret.x = Math_Sqrt(7812.5 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A8:
		ret.x = Math_Sqrt(3906.25 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A9:
		ret.x = Math_Sqrt(1953.125 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_A10:
		ret.x = Math_Sqrt(976.5625 / sqr2);
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B0:
		ret.x = 1000;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B1:
		ret.x = 500 * sqr2;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B2:
		ret.x = 500;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B3:
		ret.x = 250 * sqr2;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B4:
		ret.x = 250;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B5:
		ret.x = 125 * sqr2;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B6:
		ret.x = 125;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B7:
		ret.x = 62.5 * sqr2;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B8:
		ret.x = 62.5;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B9:
		ret.x = 31.25 * sqr2;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_B10:
		ret.x = 31.25;
		ret.y = ret.x * sqr2;
		return ret;
	case PT_C0:
		ret.y = 1000 / Math_Pow(2, -3.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C1:
		ret.y = 1000 / Math_Pow(2, 1.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C2:
		ret.y = 1000 / Math_Pow(2, 5.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C3:
		ret.y = 1000 / Math_Pow(2, 9.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C4:
		ret.y = 1000 / Math_Pow(2, 13.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C5:
		ret.y = 1000 / Math_Pow(2, 17.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C6:
		ret.y = 1000 / Math_Pow(2, 21.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C7:
		ret.y = 1000 / Math_Pow(2, 25.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C8:
		ret.y = 1000 / Math_Pow(2, 29.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C9:
		ret.y = 1000 / Math_Pow(2, 33.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_C10:
		ret.y = 1000 / Math_Pow(2, 37.0 / 8.0) + 0.2;
		ret.x = ret.y / sqr2;
		return ret;
	case PT_LETTER:
	case PT_ANSI_A:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.5) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		return ret;
	case PT_GOV_LETTER:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.0) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10.5) * 1000;
		return ret;
	case PT_LEGAL:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.5) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 14) * 1000;
		return ret;
	case PT_JUNIOR_LEGAL:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 5.0) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.0) * 1000;
		return ret;
	case PT_ANSI_B:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 17) * 1000;
		return ret;
	case PT_ANSI_C:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 17) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 22) * 1000;
		return ret;
	case PT_ANSI_D:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 22) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 34) * 1000;
		return ret;
	case PT_ANSI_E:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 34) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 44) * 1000;
		return ret;
	case PT_CREDIT_CARD:
		ret.x = 53.98;
		ret.y = 85.60;
		return ret;
	case PT_3R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 3) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 5) * 1000;
		return ret;
	case PT_4R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 4) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 6) * 1000;
		return ret;
	case PT_4D:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 4.5) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 6) * 1000;
		return ret;
	case PT_5R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 5) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 7) * 1000;
		return ret;
	case PT_6R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 6) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8) * 1000;
		return ret;
	case PT_8R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10) * 1000;
		return ret;
	case PT_S8R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		return ret;
	case PT_10R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		return ret;
	case PT_S10R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 15) * 1000;
		return ret;
	case PT_11R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 14) * 1000;
		return ret;
	case PT_S11R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 17) * 1000;
		return ret;
	case PT_12R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 15) * 1000;
		return ret;
	case PT_S12R:
		ret.x = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		ret.y = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 18) * 1000;
		return ret;
	}
}
