#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Media/PaperSize.h"
#include "Text/MyString.h"

Media::PaperSize::PaperSize(PaperType pType)
{
	Double sqr2 = Math::Sqrt(2);
	this->pType = pType;
	switch (pType)
	{
	case PT_4A0:
		this->widthMM = Math::Sqrt(4000000 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_2A0:
		this->widthMM = Math::Sqrt(2000000 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A0:
		this->widthMM = Math::Sqrt(1000000 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A1:
		this->widthMM = Math::Sqrt(500000 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A2:
		this->widthMM = Math::Sqrt(250000 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A3:
		this->widthMM = Math::Sqrt(125000 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	default:
	case PT_DEFAULT:
	case PT_A4:
		this->widthMM = Math::Sqrt(62500 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A5:
		this->widthMM = Math::Sqrt(31250 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A6:
		this->widthMM = Math::Sqrt(15625 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A7:
		this->widthMM = Math::Sqrt(7812.5 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A8:
		this->widthMM = Math::Sqrt(3906.25 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A9:
		this->widthMM = Math::Sqrt(1953.125 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_A10:
		this->widthMM = Math::Sqrt(976.5625 / sqr2);
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B0:
		this->widthMM = 1000;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B1:
		this->widthMM = 500 * sqr2;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B2:
		this->widthMM = 500;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B3:
		this->widthMM = 250 * sqr2;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B4:
		this->widthMM = 250;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B5:
		this->widthMM = 125 * sqr2;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B6:
		this->widthMM = 125;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B7:
		this->widthMM = 62.5 * sqr2;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B8:
		this->widthMM = 62.5;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B9:
		this->widthMM = 31.25 * sqr2;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_B10:
		this->widthMM = 31.25;
		this->heightMM = this->widthMM * sqr2;
		break;
	case PT_C0:
		this->heightMM = 1000 / Math::Pow(2, -3.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C1:
		this->heightMM = 1000 / Math::Pow(2, 1.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C2:
		this->heightMM = 1000 / Math::Pow(2, 5.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C3:
		this->heightMM = 1000 / Math::Pow(2, 9.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C4:
		this->heightMM = 1000 / Math::Pow(2, 13.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C5:
		this->heightMM = 1000 / Math::Pow(2, 17.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C6:
		this->heightMM = 1000 / Math::Pow(2, 21.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C7:
		this->heightMM = 1000 / Math::Pow(2, 25.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C8:
		this->heightMM = 1000 / Math::Pow(2, 29.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C9:
		this->heightMM = 1000 / Math::Pow(2, 33.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_C10:
		this->heightMM = 1000 / Math::Pow(2, 37.0 / 8.0) + 0.2;
		this->widthMM = this->heightMM / sqr2;
		break;
	case PT_LETTER:
	case PT_ANSI_A:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.5) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		break;
	case PT_GOV_LETTER:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.0) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10.5) * 1000;
		break;
	case PT_LEGAL:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.5) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 14) * 1000;
		break;
	case PT_JUNIOR_LEGAL:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 5.0) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8.0) * 1000;
		break;
	case PT_ANSI_B:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 17) * 1000;
		break;
	case PT_ANSI_C:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 17) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 22) * 1000;
		break;
	case PT_ANSI_D:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 22) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 34) * 1000;
		break;
	case PT_ANSI_E:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 34) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 44) * 1000;
		break;
	case PT_CREDIT_CARD:
		this->widthMM = 53.98;
		this->heightMM = 85.60;
		break;
	case PT_3R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 3) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 5) * 1000;
		break;
	case PT_4R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 4) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 6) * 1000;
		break;
	case PT_4D:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 4.5) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 6) * 1000;
		break;
	case PT_5R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 5) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 7) * 1000;
		break;
	case PT_6R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 6) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8) * 1000;
		break;
	case PT_8R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10) * 1000;
		break;
	case PT_S8R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 8) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		break;
	case PT_10R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		break;
	case PT_S10R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 10) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 15) * 1000;
		break;
	case PT_11R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 14) * 1000;
		break;
	case PT_S11R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 11) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 17) * 1000;
		break;
	case PT_12R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 15) * 1000;
		break;
	case PT_S12R:
		this->widthMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 12) * 1000;
		this->heightMM = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 18) * 1000;
		break;
	}
}

Media::PaperSize::~PaperSize()
{
}

Double Media::PaperSize::GetWidthMM()
{
	return this->widthMM;
}

Double Media::PaperSize::GetHeightMM()
{
	return this->heightMM;
}
