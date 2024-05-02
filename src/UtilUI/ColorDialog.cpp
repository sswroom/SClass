#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/CS/TransferFunc.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UtilUI/ColorDialog.h"

void __stdcall UtilUI::ColorDialog::OnOKClicked(AnyType userObj)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	me->StoreColor();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall UtilUI::ColorDialog::OnCancelClicked(AnyType userObj)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

Bool __stdcall UtilUI::ColorDialog::OnSubDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		Math::Size2D<UOSInt> sz;
		Double newV;
		me->subDowned = true;
		me->pbSub->SetCapture();
		sz = me->pbSub->GetSizeP();
		if (scnPos.y < 1)
		{
			scnPos.y = 1;
		}
		else if (scnPos.y > (OSInt)sz.y - 2)
		{
			scnPos.y = (OSInt)sz.y - 2;
		}
		newV = OSInt2Double(scnPos.y - 1) / (Double)(sz.y - 3);
		if (me->mainZ != newV)
		{
			me->mainZ = newV;
			me->GenMainImage();
			me->UpdateColor();
		}
	}
	return false;
}

Bool __stdcall UtilUI::ColorDialog::OnSubMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->subDowned)
	{
		Math::Size2D<UOSInt> sz;
		Double newV;
		sz = me->pbSub->GetSizeP();
		if (scnPos.y < 1)
		{
			scnPos.y = 1;
		}
		else if (scnPos.y > (OSInt)sz.y - 2)
		{
			scnPos.y = (OSInt)sz.y - 2;
		}
		newV = OSInt2Double(scnPos.y - 1) / (Double)(sz.y - 3);
		if (me->mainZ != newV)
		{
			me->mainZ = newV;
			me->GenMainImage();
			me->UpdateColor();
		}
	}
	return false;
}

Bool __stdcall UtilUI::ColorDialog::OnSubUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		me->pbSub->ReleaseCapture();
		if (me->subDowned)
		{
			me->subDowned = false;
			Math::Size2D<UOSInt> sz;
			Double newV;
			sz = me->pbSub->GetSizeP();
			if (scnPos.y < 1)
			{
				scnPos.y = 1;
			}
			else if (scnPos.y > (OSInt)sz.y - 2)
			{
				scnPos.y = (OSInt)sz.y - 2;
			}
			newV = OSInt2Double(scnPos.y - 1) / (Double)(sz.y - 3);
			if (me->mainZ != newV)
			{
				me->mainZ = newV;
				me->GenMainImage();
				me->UpdateColor();
			}
		}
	}
	return false;
}

Bool __stdcall UtilUI::ColorDialog::OnMainDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		me->subDowned = true;
		me->pbMain->SetCapture();
		Math::Size2D<UOSInt> sz;
		Double newV1;
		Double newV2;
		sz = me->pbMain->GetSizeP();

		if (scnPos.x < 1)
		{
			scnPos.x = 1;
		}
		else if (scnPos.x > (OSInt)sz.x - 2)
		{
			scnPos.x = (OSInt)sz.x - 2;
		}
		if (scnPos.y < 1)
		{
			scnPos.y = 1;
		}
		else if (scnPos.y > (OSInt)sz.y - 2)
		{
			scnPos.y = (OSInt)sz.y - 2;
		}
		newV1 = OSInt2Double(scnPos.x - 1) / (Double)(sz.x - 3);
		newV2 = OSInt2Double(scnPos.y - 1) / (Double)(sz.y - 3);
		if (me->mainX != newV1 || me->mainY != newV2)
		{
			me->mainX = newV1;
			me->mainY = newV2;
			me->GenSubImage();
			me->UpdateColor();
		}
	}
	return false;
}

Bool __stdcall UtilUI::ColorDialog::OnMainMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->subDowned)
	{
		Math::Size2D<UOSInt> sz;
		Double newV1;
		Double newV2;
		sz = me->pbMain->GetSizeP();
		if (scnPos.x < 1)
		{
			scnPos.x = 1;
		}
		else if (scnPos.x > (OSInt)sz.x - 2)
		{
			scnPos.x = (OSInt)sz.x - 2;
		}
		if (scnPos.y < 1)
		{
			scnPos.y = 1;
		}
		else if (scnPos.y > (OSInt)sz.y - 2)
		{
			scnPos.y = (OSInt)sz.y - 2;
		}
		newV1 = OSInt2Double(scnPos.x - 1) / (Double)(sz.x - 3);
		newV2 = OSInt2Double(scnPos.y - 1) / (Double)(sz.y - 3);
		if (me->mainX != newV1 || me->mainY != newV2)
		{
			me->mainX = newV1;
			me->mainY = newV2;
			me->GenSubImage();
			me->UpdateColor();
		}
	}
	return false;
}

Bool __stdcall UtilUI::ColorDialog::OnMainUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		me->pbMain->ReleaseCapture();
		if (me->subDowned)
		{
			me->subDowned = false;
			Math::Size2D<UOSInt> sz;
			Double newV1;
			Double newV2;
			sz = me->pbMain->GetSizeP();
			if (scnPos.x < 1)
			{
				scnPos.x = 1;
			}
			else if (scnPos.x > (OSInt)sz.x - 2)
			{
				scnPos.x = (OSInt)sz.x - 2;
			}
			if (scnPos.y < 1)
			{
				scnPos.y = 1;
			}
			else if (scnPos.y > (OSInt)sz.y - 2)
			{
				scnPos.y = (OSInt)sz.y - 2;
			}
			newV1 = OSInt2Double(scnPos.x - 1) / (Double)(sz.x - 3);
			newV2 = OSInt2Double(scnPos.y - 1) / (Double)(sz.y - 3);
			if (me->mainX != newV1 || me->mainY != newV2)
			{
				me->mainX = newV1;
				me->mainY = newV2;
				me->GenSubImage();
				me->UpdateColor();
			}
		}
	}
	return false;
}

void __stdcall UtilUI::ColorDialog::OnRedChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_RED;
		me->LoadColor();
	}
}


void __stdcall UtilUI::ColorDialog::OnGreenChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_GREEN;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnBlueChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_BLUE;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnYIQYChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_YIQY;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnYIQIChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_YIQI;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnYIQQChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_YIQQ;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnHSVHChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_HSVH;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnHSVSChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_HSVS;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnHSVVChange(AnyType userObj, Bool newState)
{
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (newState)
	{
		me->StoreColor();
		me->colorType = CT_HSVV;
		me->LoadColor();
	}
}

void __stdcall UtilUI::ColorDialog::OnRedTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_RED;
	if (!me->radR->IsSelected())
	{
		me->radR->Select();
	}
	me->txtR->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnGreenTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_GREEN;
	if (!me->radG->IsSelected())
	{
		me->radG->Select();
	}
	me->txtG->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnBlueTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_BLUE;
	if (!me->radB->IsSelected())
	{
		me->radB->Select();
	}
	me->txtB->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnYIQYTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_YIQY;
	if (!me->radYIQY->IsSelected())
	{
		me->radYIQY->Select();
	}
	me->txtYIQY->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnYIQITChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_YIQI;
	if (!me->radYIQI->IsSelected())
	{
		me->radYIQI->Select();
	}
	me->txtYIQI->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnYIQQTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_YIQQ;
	if (!me->radYIQQ->IsSelected())
	{
		me->radYIQQ->Select();
	}
	me->txtYIQQ->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnHSVHTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_HSVH;
	if (!me->radHSVH->IsSelected())
	{
		me->radHSVH->Select();
	}
	me->txtHSVH->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnHSVSTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_HSVS;
	if (!me->radHSVS->IsSelected())
	{
		me->radHSVS->Select();
	}
	me->txtHSVS->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnHSVVTChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	if (me->autoTextUpdate)
		return;
	me->textUpdating = CT_HSVV;
	if (!me->radHSVV->IsSelected())
	{
		me->radHSVV->Select();
	}
	me->txtHSVV->GetText(sbuff);
	me->mainZ = Text::StrToDouble(sbuff) / 255.0;
	if (me->mainZ < 0)
		me->mainZ = 0;
	else if (me->mainZ > 1)
		me->mainZ = 1;
	me->GenMainImage();
	me->UpdateColor();
	me->textUpdating = CT_UNKNOWN;
}

void __stdcall UtilUI::ColorDialog::OnAlphaChange(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<UtilUI::ColorDialog> me = userObj.GetNN<UtilUI::ColorDialog>();
	me->txtAlpha->GetText(sbuff);
	me->aVal = Text::StrToDouble(sbuff) / 255.0;
}

UInt32 __stdcall UtilUI::ColorDialog::GenThread(AnyType userObj)
{
	NN<UtilUI::ColorDialog::ThreadStat> stat = userObj.GetNN<UtilUI::ColorDialog::ThreadStat>();
	stat->status = 1;
	stat->me->genEvt->Set();
	while (true)
	{
		stat->evt->Wait(1000);
		if (stat->status == 2)
		{
			stat->me->GenMainImageInner(stat->imgPtr, stat->startIndex, stat->endIndex, stat->w, stat->h);
			stat->status = 1;
			stat->me->genEvt->Set();
		}
		else if (stat->status == 4)
		{
			break;
		}
	}
	stat->status = 0;
	stat->me->genEvt->Set();
	return 0;
}

void UtilUI::ColorDialog::XYZ2RGB(Double x, Double y, Double z, Double *r, Double *g, Double *b)
{
	switch (colorType)
	{
	case CT_RED:
		*r = z;
		*g = x;
		*b = y;
		break;
	case CT_GREEN:
		*r = x;
		*g = z;
		*b = y;
		break;
	case CT_BLUE:
		*r = x;
		*g = y;
		*b = z;
		break;
	case CT_YIQY:
		YIQ2RGB(z, x, y, r, g, b);
		break;
	case CT_YIQI:
		YIQ2RGB(x, z, y, r, g, b);
		break;
	case CT_YIQQ:
		YIQ2RGB(x, y, z, r, g, b);
		break;
	case CT_HSVH:
		HSV2RGB(z, x, y, r, g, b);
		break;
	case CT_HSVS:
		HSV2RGB(x, z, y, r, g, b);
		break;
	case CT_HSVV:
		HSV2RGB(x, y, z, r, g, b);
		break;
	case CT_UNKNOWN:
	default:
		*r = x;
		*g = y;
		*b = z;
		break;
	}
}

void UtilUI::ColorDialog::YIQ2RGB(Double y, Double i, Double q, Double *r, Double *g, Double *b)
{
	i = i - 0.5;
	q = q - 0.5;
	*r = y + 0.956 * i + 0.621 * q;
	*g = y - 0.272 * i - 0.647 * q;
	*b = y - 1.105 * i + 1.702 * q;
}

void UtilUI::ColorDialog::HSV2RGB(Double h, Double s, Double v, Double *r, Double *g, Double *b)
{
	if (s == 0)
	{
		*r = v;
		*g = v;
		*b = v;

	}
	Int32 ih;
	Double p;
	Double q;
	Double t;
	Double f;
	while (h < 0)
	{
		h += 1;
	}
	while (h >= 1)
	{
		h -= 1;
	}
	ih = (Int32)(h * 6.0);
	f = h * 6.0 - ih;

    p = v * (1 - s);
    q = v * (1 - s * f);
    t = v * (1 - s * (1 - f));
	switch (ih)
	{
	case 0:
		*r = v;
		*g = t;
		*b = p;
		break;
	case 1:
		*r = q;
		*g = v;
		*b = p;
		break;
	case 2:
		*r = p;
		*g = v;
		*b = t;
		break;
	case 3:
		*r = p;
		*g = q;
		*b = v;
		break;
	case 4:
		*r = t;
		*g = p;
		*b = v;
		break;
	default:
		*r = v;
		*g = p;
		*b = q;
		break;
	}
}

void UtilUI::ColorDialog::RGB2XYZ(Double r, Double g, Double b, Double *x, Double *y, Double *z)
{
	switch (colorType)
	{
	case CT_RED:
		*x = g;
		*y = b;
		*z = r;
		break;
	case CT_GREEN:
		*x = r;
		*y = b;
		*z = g;
		break;
	case CT_BLUE:
		*x = r;
		*y = g;
		*z = b;
		break;
	case CT_YIQY:
		RGB2YIQ(r, g, b, z, x, y);
		break;
	case CT_YIQI:
		RGB2YIQ(r, g, b, x, z, y);
		break;
	case CT_YIQQ:
		RGB2YIQ(r, g, b, x, y, z);
		break;
	case CT_HSVH:
		RGB2HSV(r, g, b, z, x, y);
		break;
	case CT_HSVS:
		RGB2HSV(r, g, b, x, z, y);
		break;
	case CT_HSVV:
		RGB2HSV(r, g, b, x, y, z);
		break;
	case CT_UNKNOWN:
	default:
		*x = r;
		*y = g;
		*z = b;
		break;
	}
}

void __inline UtilUI::ColorDialog::RGB2YIQ(Double r, Double g, Double b, Double *y, Double *i, Double *q)
{
	*y = 0.299 * r + 0.587 * g + 0.114 * b;
	*i = 0.596 * r - 0.275 * g - 0.321 * b + 0.5;
	*q = 0.212 * r - 0.523 * g + 0.311 * b + 0.5;
}

void __inline UtilUI::ColorDialog::RGB2HSV(Double r, Double g, Double b, Double *h, Double *s, Double *v)
{
	Double max;
	Double min;
	Double delta;
	if (r > g)
	{
		max = r;
		min = g;
	}
	else
	{
		max = g;
		min = r;
	}
	if (b > max)
		max = b;
	if (b < min)
		min = b;

	*v = max;
	delta = max - min;
	*s = delta;
	if (delta == 0)
		*h = 0;
	else
	{
		if (r == max)
		{
			*h = (g - b) / delta;
		}
		else if (g == max)
		{
			*h = 0.33333333333333333333333333333333 + (b - r) / delta;
		}
		else
		{
			*h = 0.66666666666666666666666666666667 + (r - g) / delta;
		}
		if (*h < 0)
			*h += 1;
	}
}

void UtilUI::ColorDialog::GenMainImageInner(UInt8 *imgPtr, UOSInt startIndex, UOSInt endIndex, UOSInt w, UOSInt h)
{
	UOSInt i;
	UOSInt j;
	Double v1;
	Double v2;
	Double v3;
	Math::Vector3 rgbv;
	Math::Vector3 rgbv2;
	Math::Vector3 rgbv3;

	NN<const Media::IColorHandler::RGBPARAM2> rgbParam = this->colorSess->GetRGBParam();
	Double rGammaVal;
	Double gGammaVal;
	Double bGammaVal;
	Double rBright;
	Double gBright;
	Double bBright;
	Double rContr;
	Double gContr;
	Double bContr;
	Double tMul;
	Double rMul;
	Double gMul;
	Double bMul;

	NN<Media::CS::TransferFunc> dispRTran;
	NN<Media::CS::TransferFunc> dispGTran;
	NN<Media::CS::TransferFunc> dispBTran;
	NN<Media::CS::TransferFunc> srcRTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetRTranParam());
	NN<Media::CS::TransferFunc> srcGTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetGTranParam());
	NN<Media::CS::TransferFunc> srcBTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetBTranParam());
	if (this->colorCorr == CCT_VIDEO)
	{
		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
	}
	else if (this->colorCorr == CCT_PHOTO)
	{
		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
	}
	else
	{
		rGammaVal = 1.0;
		gGammaVal = 1.0;
		bGammaVal = 1.0;
		rBright = 1.0;
		gBright = 1.0;
		bBright = 1.0;
		rContr = 1.0;
		gContr = 1.0;
		bContr = 1.0;
		tMul = 1.0;
		rMul = 1.0;
		gMul = 1.0;
		bMul = 1.0;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
	}
	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	this->colorProfile->GetPrimaries()->GetConvMatrix(mat1);
	rgbParam->monProfile.GetPrimariesRead()->GetConvMatrix(mat5);
	mat5.Inverse();

	rgbv2.Set(rgbParam->monProfile.GetPrimariesRead()->w, 1.0);
	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat2);
	mat3.Set(mat2);
	mat4.SetIdentity();
	rgbv.Set(this->colorProfile->GetPrimaries()->w, 1.0);
	rgbv3 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(rgbv2);
	rgbv2 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(rgbv);
	rgbv = mat2.Multiply(rgbv2);
	rgbv2 = mat2.Multiply(rgbv3);
	mat2.Inverse();
	mat4.vec[0].val[0] = rgbv2.val[0] / rgbv.val[0];
	mat4.vec[1].val[1] = rgbv2.val[1] / rgbv.val[1];
	mat4.vec[2].val[2] = rgbv2.val[2] / rgbv.val[2];
	mat2.Multiply(mat4);
	mat2.Multiply(mat3);
	mat1.MultiplyBA(mat2);

	mat1.MultiplyBA(mat5);

	Double v3R = 1 / (Double)(h - 1);
	Double v2R = 1 / (Double)(w - 1);
	v1 = this->mainZ;
	i = startIndex;
	while (i < endIndex)
	{
		v3 = UOSInt2Double(i) * v3R;
		j = 0;
		while (j < w)
		{
			v2 = UOSInt2Double(j) * v2R;
			XYZ2RGB(v2, v3, v1, &rgbv.val[0], &rgbv.val[1], &rgbv.val[2]);
			rgbv.val[0] = srcRTran->InverseTransfer(rgbv.val[0]);
			rgbv.val[1] = srcGTran->InverseTransfer(rgbv.val[1]);
			rgbv.val[2] = srcBTran->InverseTransfer(rgbv.val[2]);
			rgbv2 = mat1.Multiply(rgbv);
			rgbv.val[0] = dispRTran->ForwardTransfer(rgbv2.val[0] * rMul);
			rgbv.val[1] = dispGTran->ForwardTransfer(rgbv2.val[1] * gMul);
			rgbv.val[2] = dispBTran->ForwardTransfer(rgbv2.val[2] * bMul);
			imgPtr[0] = Math::SDouble2UInt8((bBright - 1.0 + Math_Pow(rgbv.val[2], bGammaVal) * bContr) * 255.0);
			imgPtr[1] = Math::SDouble2UInt8((gBright - 1.0 + Math_Pow(rgbv.val[1], gGammaVal) * gContr) * 255.0);
			imgPtr[2] = Math::SDouble2UInt8((rBright - 1.0 + Math_Pow(rgbv.val[0], rGammaVal) * rContr) * 255.0);
			imgPtr[3] = 255;

			imgPtr += 4;
			j++;
		}
		i++;
	}
	srcRTran.Delete();
	srcGTran.Delete();
	srcBTran.Delete();
	dispRTran.Delete();
	dispGTran.Delete();
	dispBTran.Delete();
}

void UtilUI::ColorDialog::GenMainImage()
{
	UOSInt i;
	Bool found;
	UInt8 *imgPtr = this->mainImg->data;
	UOSInt w = this->mainImg->info.dispSize.x;
	UOSInt h = this->mainImg->info.dispSize.y;

	UOSInt lastIndex = h;
	UOSInt thisIndex;
	i = this->genThreadCnt;
	while (i-- > 0)
	{
		thisIndex = h * i / this->genThreadCnt;
		this->genStats[i].imgPtr = imgPtr + thisIndex * w * 4;
		this->genStats[i].startIndex = thisIndex;
		this->genStats[i].endIndex = lastIndex;
		this->genStats[i].w = w;
		this->genStats[i].h = h;
		this->genStats[i].status = 2;
		this->genStats[i].evt->Set();
		lastIndex = thisIndex;
	}
	while (true)
	{
		this->genEvt->Wait(1000);
		found = false;
		i = this->genThreadCnt;
		while (i-- > 0)
		{
			if (this->genStats[i].status != 1)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}
//	GenMainImageInner(this->mainImg->data, 0, 512);
	this->pbMain->SetImage(this->mainImg);
}

void UtilUI::ColorDialog::GenSubImage()
{
	UOSInt i;
	UOSInt j;
	Double v1;
	Double v2;
	Double v3;
	Math::Vector3 rgbv;
	Math::Vector3 rgbv2;
	Math::Vector3 rgbv3;

	UInt8 c[4];
	UInt8 *imgPtr = this->subImg->data;
	UOSInt w = this->subImg->info.dispSize.x;
	UOSInt h = this->subImg->info.dispSize.y;
	NN<const Media::IColorHandler::RGBPARAM2> rgbParam = this->colorSess->GetRGBParam();
	Double rGammaVal;
	Double gGammaVal;
	Double bGammaVal;
	Double rBright;
	Double gBright;
	Double bBright;
	Double rContr;
	Double gContr;
	Double bContr;
	Double tMul;
	Double rMul;
	Double gMul;
	Double bMul;

	NN<Media::CS::TransferFunc> dispRTran;
	NN<Media::CS::TransferFunc> dispGTran;
	NN<Media::CS::TransferFunc> dispBTran;
	NN<Media::CS::TransferFunc> srcRTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetRTranParam());
	NN<Media::CS::TransferFunc> srcGTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetGTranParam());
	NN<Media::CS::TransferFunc> srcBTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetBTranParam());
	if (this->colorCorr == CCT_VIDEO)
	{
		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetGTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetBTranParamRead());
	}
	else if (this->colorCorr == CCT_PHOTO)
	{
		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetGTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetBTranParamRead());
	}
	else
	{
		rGammaVal = 1.0;
		gGammaVal = 1.0;
		bGammaVal = 1.0;
		rBright = 1.0;
		gBright = 1.0;
		bBright = 1.0;
		rContr = 1.0;
		gContr = 1.0;
		bContr = 1.0;
		tMul = 1.0;
		rMul = 1.0;
		gMul = 1.0;
		bMul = 1.0;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetGTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetBTranParamRead());
	}
	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	this->colorProfile->GetPrimaries()->GetConvMatrix(mat1);
	rgbParam->monProfile.GetPrimariesRead()->GetConvMatrix(mat5);
	mat5.Inverse();

	rgbv2.Set(rgbParam->monProfile.GetPrimariesRead()->w, 1.0);
	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat2);
	mat3.Set(mat2);
	mat4.SetIdentity();
	rgbv.Set(this->colorProfile->GetPrimaries()->w, 1.0);
	rgbv3 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(rgbv2);
	rgbv2 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(rgbv);
	rgbv = mat2.Multiply(rgbv2);
	rgbv2 = mat2.Multiply(rgbv3);
	mat2.Inverse();
	mat4.vec[0].val[0] = rgbv2.val[0] / rgbv.val[0];
	mat4.vec[1].val[1] = rgbv2.val[1] / rgbv.val[1];
	mat4.vec[2].val[2] = rgbv2.val[2] / rgbv.val[2];
	mat2.Multiply(mat4);
	mat2.Multiply(mat3);
	mat1.MultiplyBA(mat2);

	mat1.MultiplyBA(mat5);

	v2 = this->mainX;
	v3 = this->mainY;
	Double v1R = 1 / (Double)(h - 1);
	i = 0;
	while (i < h)
	{
		v1 = UOSInt2Double(i) * v1R;
		this->XYZ2RGB(v2, v3, v1, &rgbv.val[0], &rgbv.val[1], &rgbv.val[2]);
		rgbv.val[0] = srcRTran->InverseTransfer(rgbv.val[0]);
		rgbv.val[1] = srcGTran->InverseTransfer(rgbv.val[1]);
		rgbv.val[2] = srcBTran->InverseTransfer(rgbv.val[2]);
		rgbv2 = mat1.Multiply(rgbv);
		rgbv.val[0] = dispRTran->ForwardTransfer(rgbv2.val[0] * rMul);
		rgbv.val[1] = dispGTran->ForwardTransfer(rgbv2.val[1] * gMul);
		rgbv.val[2] = dispBTran->ForwardTransfer(rgbv2.val[2] * bMul);
		Int32 rV;
		Int32 gV;
		Int32 bV;
		rV = Double2Int32((rBright - 1.0 + Math_Pow(rgbv.val[0], rGammaVal) * rContr) * 255.0);
		gV = Double2Int32((gBright - 1.0 + Math_Pow(rgbv.val[1], gGammaVal) * gContr) * 255.0);
		bV = Double2Int32((bBright - 1.0 + Math_Pow(rgbv.val[2], bGammaVal) * bContr) * 255.0);
		if (bV > 255)
			c[0] = 255;
		else if (bV < 0)
			c[0] = 0;
		else
			c[0] = (UInt8)(bV & 255);
		if (gV > 255)
			c[1] = 255;
		else if (gV < 0)
			c[1] = 0;
		else
			c[1] = (UInt8)(gV & 255);
		if (rV > 255)
			c[2] = 255;
		else if (rV < 0)
			c[2] = 0;
		else
			c[2] = (UInt8)(rV & 255);
		c[3] = 255;

		j = w;
		while (j-- > 0)
		{
			*(Int32*)imgPtr = *(Int32*)c;

			imgPtr += 4;
		}
		i++;
	}
	dispRTran.Delete();
	dispGTran.Delete();
	dispBTran.Delete();
	srcRTran.Delete();
	srcGTran.Delete();
	srcBTran.Delete();
	this->pbSub->SetImage(this->subImg);
}

void UtilUI::ColorDialog::StoreColor()
{
	XYZ2RGB(this->mainX, this->mainY, this->mainZ, &this->rVal, &this->gVal, &this->bVal);
}

void UtilUI::ColorDialog::LoadColor()
{
	RGB2XYZ(this->rVal, this->gVal, this->bVal, &this->mainX, &this->mainY, &this->mainZ);
	this->GenMainImage();
	this->GenSubImage();
}

void UtilUI::ColorDialog::UpdateColor()
{
	UTF8Char sbuff[10];
	UTF8Char *sptr;
	Math::Vector3 rgbv;
	Math::Vector3 rgbv2;
	Math::Vector3 rgbv3;

	UInt8 c[4];
	NN<const Media::IColorHandler::RGBPARAM2> rgbParam = this->colorSess->GetRGBParam();
	Double rGammaVal;
	Double gGammaVal;
	Double bGammaVal;
	Double rBright;
	Double gBright;
	Double bBright;
	Double rContr;
	Double gContr;
	Double bContr;
	Double tMul;
	Double rMul;
	Double gMul;
	Double bMul;

	NN<Media::CS::TransferFunc> dispRTran;
	NN<Media::CS::TransferFunc> dispGTran;
	NN<Media::CS::TransferFunc> dispBTran;
	NN<Media::CS::TransferFunc> srcRTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetRTranParam());
	NN<Media::CS::TransferFunc> srcGTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetGTranParam());
	NN<Media::CS::TransferFunc> srcBTran = Media::CS::TransferFunc::CreateFunc(this->colorProfile->GetBTranParam());
	if (this->colorCorr == CCT_VIDEO)
	{
		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetGTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetBTranParamRead());
	}
	else if (this->colorCorr == CCT_PHOTO)
	{
		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetGTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetBTranParamRead());
	}
	else
	{
		rGammaVal = 1.0;
		gGammaVal = 1.0;
		bGammaVal = 1.0;
		rBright = 1.0;
		gBright = 1.0;
		bBright = 1.0;
		rContr = 1.0;
		gContr = 1.0;
		bContr = 1.0;
		tMul = 1.0;
		rMul = 1.0;
		gMul = 1.0;
		bMul = 1.0;
		dispRTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetRTranParamRead());
		dispGTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetGTranParamRead());
		dispBTran = Media::CS::TransferFunc::CreateFunc(rgbParam->monProfile.GetBTranParamRead());
	}
	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	this->colorProfile->GetPrimaries()->GetConvMatrix(mat1);
	rgbParam->monProfile.GetPrimariesRead()->GetConvMatrix(mat5);
	mat5.Inverse();

	rgbv2.Set(rgbParam->monProfile.GetPrimariesRead()->w, 1.0);
	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat2);
	mat3.Set(mat2);
	mat4.SetIdentity();
	rgbv.Set(this->colorProfile->GetPrimaries()->w, 1.0);
	rgbv3 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(rgbv2);
	rgbv2 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(rgbv);
	rgbv = mat2.Multiply(rgbv2);
	rgbv2 = mat2.Multiply(rgbv3);
	mat2.Inverse();
	mat4.vec[0].val[0] = rgbv2.val[0] / rgbv.val[0];
	mat4.vec[1].val[1] = rgbv2.val[1] / rgbv.val[1];
	mat4.vec[2].val[2] = rgbv2.val[2] / rgbv.val[2];
	mat2.Multiply(mat4);
	mat2.Multiply(mat3);
	mat1.MultiplyBA(mat2);

	mat1.MultiplyBA(mat5);

	this->XYZ2RGB(this->mainX, this->mainY, this->mainZ, &rgbv.val[0], &rgbv.val[1], &rgbv.val[2]);
	rgbv.val[0] = srcRTran->InverseTransfer(rgbv.val[0]);
	rgbv.val[1] = srcGTran->InverseTransfer(rgbv.val[1]);
	rgbv.val[2] = srcBTran->InverseTransfer(rgbv.val[2]);
	rgbv2 = mat1.Multiply(rgbv);
	rgbv3.val[0] = dispRTran->ForwardTransfer(rgbv2.val[0] * rMul);
	rgbv3.val[1] = dispGTran->ForwardTransfer(rgbv2.val[1] * gMul);
	rgbv3.val[2] = dispBTran->ForwardTransfer(rgbv2.val[2] * bMul);
	Int32 rV;
	Int32 gV;
	Int32 bV;
	rV = Double2Int32((rBright - 1.0 + Math_Pow(rgbv3.val[0], rGammaVal) * rContr) * 255.0);
	gV = Double2Int32((gBright - 1.0 + Math_Pow(rgbv3.val[1], gGammaVal) * gContr) * 255.0);
	bV = Double2Int32((bBright - 1.0 + Math_Pow(rgbv3.val[2], bGammaVal) * bContr) * 255.0);
	if (bV > 255)
		c[0] = 255;
	else if (bV < 0)
		c[0] = 0;
	else
		c[0] = (UInt8)(bV & 255);
	if (gV > 255)
		c[1] = 255;
	else if (gV < 0)
		c[1] = 0;
	else
		c[1] = (UInt8)(gV & 255);
	if (rV > 255)
		c[2] = 255;
	else if (rV < 0)
		c[2] = 0;
	else
		c[2] = (UInt8)(rV & 255);
	c[3] = 255;
	this->pbColor->SetBGColor(ReadUInt32(c));
	this->pbColor->Redraw();

	this->autoTextUpdate = true;
	rgbv.val[0] = srcRTran->ForwardTransfer(rgbv.val[0]);
	rgbv.val[1] = srcGTran->ForwardTransfer(rgbv.val[1]);
	rgbv.val[2] = srcBTran->ForwardTransfer(rgbv.val[2]);

	rV = Double2Int32(rgbv.val[0] * 255.0);
	gV = Double2Int32(rgbv.val[1] * 255.0);
	bV = Double2Int32(rgbv.val[2] * 255.0);
	if (rV > 255)
		rV = 255;
	else if (rV < 0)
		rV = 0;
	if (gV > 255)
		gV = 255;
	else if (gV < 0)
		gV = 0;
	if (bV > 255)
		bV = 255;
	else if (bV < 0)
		bV = 0;
	if (this->textUpdating != CT_RED)
	{
		sptr = Text::StrInt32(sbuff, rV);
		this->txtR->SetText(CSTRP(sbuff, sptr));
	}
	if (this->textUpdating != CT_GREEN)
	{
		sptr = Text::StrInt32(sbuff, gV);
		this->txtG->SetText(CSTRP(sbuff, sptr));
	}
	if (this->textUpdating != CT_BLUE)
	{
		sptr = Text::StrInt32(sbuff, bV);
		this->txtB->SetText(CSTRP(sbuff, sptr));
	}

	RGB2YIQ(rgbv.val[0], rgbv.val[1], rgbv.val[2], &rgbv2.val[0], &rgbv2.val[1], &rgbv2.val[2]);
	rV = Double2Int32(rgbv2.val[0] * 255.0);
	gV = Double2Int32(rgbv2.val[1] * 255.0);
	bV = Double2Int32(rgbv2.val[2] * 255.0);
	if (rV > 255)
		rV = 255;
	else if (rV < 0)
		rV = 0;
	if (gV > 255)
		gV = 255;
	else if (gV < 0)
		gV = 0;
	if (bV > 255)
		bV = 255;
	else if (bV < 0)
		bV = 0;
	if (this->textUpdating != CT_YIQY)
	{
		sptr = Text::StrInt32(sbuff, rV);
		this->txtYIQY->SetText(CSTRP(sbuff, sptr));
	}
	if (this->textUpdating != CT_YIQI)
	{
		sptr = Text::StrInt32(sbuff, gV);
		this->txtYIQI->SetText(CSTRP(sbuff, sptr));
	}
	if (this->textUpdating != CT_YIQQ)
	{
		sptr = Text::StrInt32(sbuff, bV);
		this->txtYIQQ->SetText(CSTRP(sbuff, sptr));
	}

	RGB2HSV(rgbv.val[0], rgbv.val[1], rgbv.val[2], &rgbv2.val[0], &rgbv2.val[1], &rgbv2.val[2]);
	rV = Double2Int32(rgbv2.val[0] * 255.0);
	gV = Double2Int32(rgbv2.val[1] * 255.0);
	bV = Double2Int32(rgbv2.val[2] * 255.0);
	if (rV > 255)
		rV = 255;
	else if (rV < 0)
		rV = 0;
	if (gV > 255)
		gV = 255;
	else if (gV < 0)
		gV = 0;
	if (bV > 255)
		bV = 255;
	else if (bV < 0)
		bV = 0;
	if (this->textUpdating != CT_HSVH)
	{
		sptr = Text::StrInt32(sbuff, rV);
		this->txtHSVH->SetText(CSTRP(sbuff, sptr));
	}
	if (this->textUpdating != CT_HSVS)
	{
		sptr = Text::StrInt32(sbuff, gV);
		this->txtHSVS->SetText(CSTRP(sbuff, sptr));
	}
	if (this->textUpdating != CT_HSVV)
	{
		sptr = Text::StrInt32(sbuff, bV);
		this->txtHSVV->SetText(CSTRP(sbuff, sptr));
	}

	this->autoTextUpdate = false;

	dispRTran.Delete();
	dispGTran.Delete();
	dispBTran.Delete();
	srcRTran.Delete();
	srcGTran.Delete();
	srcBTran.Delete();
}

UtilUI::ColorDialog::ColorDialog(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::ColorManager> colorMgr, NN<Media::DrawEngine> eng, ColorCorrType colorCorr, NN<const Media::ColorProfile> colorProfile, Optional<Media::MonitorMgr> monMgr) : UI::GUIForm(parent, 756, 640, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Color Setting"));
	this->SetNoResize(true);
	this->aVal = 1.0;
	this->rVal = 0.0;
	this->gVal = 0.0;
	this->bVal = 0.0;
	this->colorMgr = colorMgr;
	this->monMgr = monMgr;
	this->colorSess = this->colorMgr->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(*this);
	this->subDowned = false;
	this->colorType = CT_RED;
	this->autoTextUpdate = false;
	this->textUpdating = CT_UNKNOWN;
	this->colorCorr = colorCorr;
	NEW_CLASS(this->colorProfile, Media::ColorProfile(colorProfile));
	NN<Media::MonitorMgr> nnmonMgr;
	if (this->monMgr.SetTo(nnmonMgr))
	{
		this->SetDPI(nnmonMgr->GetMonitorHDPI(this->GetHMonitor()), nnmonMgr->GetMonitorDDPI(this->GetHMonitor()));
	}

	this->pbMain = ui->NewPictureBox(*this, eng, true, false);
	this->pbMain->SetRect(16, 16, 514, 514, false);
	this->pbMain->HandleMouseDown(OnMainDown, this);
	this->pbMain->HandleMouseMove(OnMainMove, this);
	this->pbMain->HandleMouseUp(OnMainUp, this);
	this->pbMain->SetNoBGColor(true);
	this->pbSub = ui->NewPictureBox(*this, eng, true, false);
	this->pbSub->SetRect(546, 16, 34, 514, false);
	this->pbSub->HandleMouseDown(OnSubDown, this);
	this->pbSub->HandleMouseMove(OnSubMove, this);
	this->pbSub->HandleMouseUp(OnSubUp, this);
	this->pbSub->SetNoBGColor(true);
	this->pbColor = ui->NewPictureBox(*this, eng, true, false);
	this->pbColor->SetRect(604, 16, 112, 112, false);

	this->radR = ui->NewRadioButton(*this, CSTR("Red"), true);
	this->radR->SetRect(604, 164, 64, 23, false);
	this->radR->HandleSelectedChange(OnRedChange, this);
	this->radG = ui->NewRadioButton(*this, CSTR("Green"), false);
	this->radG->SetRect(604, 188, 64, 23, false);
	this->radG->HandleSelectedChange(OnGreenChange, this);
	this->radB = ui->NewRadioButton(*this, CSTR("Blue"), false);
	this->radB->SetRect(604, 212, 64, 23, false);
	this->radB->HandleSelectedChange(OnBlueChange, this);
	this->radYIQY = ui->NewRadioButton(*this, CSTR("Y"), false);
	this->radYIQY->SetRect(604, 260, 64, 23, false);
	this->radYIQY->HandleSelectedChange(OnYIQYChange, this);
	this->radYIQI = ui->NewRadioButton(*this, CSTR("I"), false);
	this->radYIQI->SetRect(604, 284, 64, 23, false);
	this->radYIQI->HandleSelectedChange(OnYIQIChange, this);
	this->radYIQQ = ui->NewRadioButton(*this, CSTR("Q"), false);
	this->radYIQQ->SetRect(604, 308, 64, 23, false);
	this->radYIQQ->HandleSelectedChange(OnYIQQChange, this);
	this->radHSVH = ui->NewRadioButton(*this, CSTR("H"), false);
	this->radHSVH->SetRect(604, 356, 64, 23, false);
	this->radHSVH->HandleSelectedChange(OnHSVHChange, this);
	this->radHSVS = ui->NewRadioButton(*this, CSTR("S"), false);
	this->radHSVS->SetRect(604, 380, 64, 23, false);
	this->radHSVS->HandleSelectedChange(OnHSVSChange, this);
	this->radHSVV = ui->NewRadioButton(*this, CSTR("V"), false);
	this->radHSVV->SetRect(604, 404, 64, 23, false);
	this->radHSVV->HandleSelectedChange(OnHSVVChange, this);

	this->txtR = ui->NewTextBox(*this, CSTR(""));
	this->txtR->SetRect(668, 164, 48, 23, false);
	this->txtR->HandleTextChanged(OnRedTChange, this);
	this->txtG = ui->NewTextBox(*this, CSTR(""));
	this->txtG->SetRect(668, 188, 48, 23, false);
	this->txtG->HandleTextChanged(OnGreenTChange, this);
	this->txtB = ui->NewTextBox(*this, CSTR(""));
	this->txtB->SetRect(668, 212, 48, 23, false);
	this->txtB->HandleTextChanged(OnBlueTChange, this);
	this->txtYIQY = ui->NewTextBox(*this, CSTR(""));
	this->txtYIQY->SetRect(668, 260, 48, 23, false);
	this->txtYIQY->HandleTextChanged(OnYIQYTChange, this);
	this->txtYIQI = ui->NewTextBox(*this, CSTR(""));
	this->txtYIQI->SetRect(668, 284, 48, 23, false);
	this->txtYIQI->HandleTextChanged(OnYIQITChange, this);
	this->txtYIQQ = ui->NewTextBox(*this, CSTR(""));
	this->txtYIQQ->SetRect(668, 308, 48, 23, false);
	this->txtYIQQ->HandleTextChanged(OnYIQQTChange, this);
	this->txtHSVH = ui->NewTextBox(*this, CSTR(""));
	this->txtHSVH->SetRect(668, 356, 48, 23, false);
	this->txtHSVH->HandleTextChanged(OnHSVHTChange, this);
	this->txtHSVS = ui->NewTextBox(*this, CSTR(""));
	this->txtHSVS->SetRect(668, 380, 48, 23, false);
	this->txtHSVS->HandleTextChanged(OnHSVSTChange, this);
	this->txtHSVV = ui->NewTextBox(*this, CSTR(""));
	this->txtHSVV->SetRect(668, 404, 48, 23, false);
	this->txtHSVV->HandleTextChanged(OnHSVVTChange, this);

	this->lblTrans = ui->NewLabel(*this, CSTR("RGB Transfer Func"));
	this->lblTrans->SetRect(16, 546, 150, 23, false);
	this->txtTrans = ui->NewTextBox(*this, Media::CS::TransferTypeGetName(this->colorProfile->GetRTranParam()->GetTranType()));
	this->txtTrans->SetRect(166, 546, 100, 23, false);
	this->txtTrans->SetReadOnly(true);
	this->lblPrimaries = ui->NewLabel(*this, CSTR("RGB Primaries"));
	this->lblPrimaries->SetRect(16, 570, 150, 23, false);
	this->txtPrimaries = ui->NewTextBox(*this, Media::ColorProfile::ColorTypeGetName(this->colorProfile->GetPrimaries()->colorType));
	this->txtPrimaries->SetRect(166, 570, 100, 23, false);
	this->txtPrimaries->SetReadOnly(true);
	this->btnOk = ui->NewButton(*this, CSTR("&Ok"));
	this->btnOk->SetRect(604, 452, 100, 23, false);
	this->btnOk->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(604, 500, 100, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOk);
	this->SetCancelButton(this->btnCancel);

	this->alphaShown = false;

	this->genThreadCnt = Sync::ThreadUtil::GetThreadCnt();
	NEW_CLASS(this->genEvt, Sync::Event(true));
	UOSInt i;
	this->genStats = MemAlloc(ThreadStat, this->genThreadCnt);
	Bool running;
	i = this->genThreadCnt;
	while (i-- > 0)
	{
		NEW_CLASS(this->genStats[i].evt, Sync::Event(true));
		this->genStats[i].me = this;
		this->genStats[i].status = 0;
		Sync::ThreadUtil::Create(GenThread, &this->genStats[i], 65536);
	}

	while (true)
	{
		this->genEvt->Wait(100);
		running = true;
		i = this->genThreadCnt;
		while (i-- > 0)
		{
			if (this->genStats[i].status == 0)
			{
				running = false;
				break;
			}
		}
		if (running)
			break;
	}

	Media::ColorProfile color;
	color.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
	Math::Size2D<UOSInt> sz;
	sz = this->pbMain->GetSizeP();
	NEW_CLASS(this->mainImg, Media::StaticImage(sz, 0, 32, Media::PF_B8G8R8A8, 0, color, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	sz = this->pbSub->GetSizeP();
	NEW_CLASS(this->subImg, Media::StaticImage(sz, 0, 32, Media::PF_B8G8R8A8, 0, color, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));

	this->GenMainImage();
	this->GenSubImage();
	sz = this->ui->GetDesktopSize();
	if (sz.y > sz.x)
	{
		this->OnDisplaySizeChange(sz.x, sz.y);
	}
}

UtilUI::ColorDialog::~ColorDialog()
{
	UOSInt i;
	Bool running;
	i = this->genThreadCnt;
	while (i-- > 0)
	{
		this->genStats[i].status = 4;
		this->genStats[i].evt->Set();
	}
	while (true)
	{
		this->genEvt->Wait(100);

		running = false;
		i = this->genThreadCnt;
		while (i-- > 0)
		{
			if (this->genStats[i].status != 0)
			{
				running = true;
				break;
			}
		}
		if (!running)
			break;
	}
	
	DEL_CLASS(this->genEvt);
	i = this->genThreadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->genStats[i].evt);
	}
	MemFree(this->genStats);

	DEL_CLASS(this->mainImg);
	DEL_CLASS(this->subImg);
	DEL_CLASS(this->colorProfile);
	this->ClearChildren();
	this->colorMgr->DeleteSess(this->colorSess);
}

void UtilUI::ColorDialog::SetColor32(UInt32 color)
{
	this->aVal = ((color >> 24) & 0xff) / 255.0;
	this->rVal = ((color >> 16) & 0xff) / 255.0;
	this->gVal = ((color >> 8) & 0xff) / 255.0;
	this->bVal = (color & 0xff) / 255.0;
	this->LoadColor();
	this->UpdateColor();
}

UInt32 UtilUI::ColorDialog::GetColor32()
{
	UInt32 a;
	UInt32 r;
	UInt32 g;
	UInt32 b;
	if (this->aVal > 1.0)
		a = 255;
	else if (this->aVal < 0)
		a = 0;
	else
		a = (UInt32)Double2Int32(this->aVal * 255.0);
	if (this->rVal > 1.0)
		r = 255;
	else if (this->rVal < 0)
		r = 0;
	else
		r = (UInt32)Double2Int32(this->rVal * 255.0);
	if (this->gVal > 1.0)
		g = 255;
	else if (this->gVal < 0)
		g = 0;
	else
		g = (UInt32)Double2Int32(this->gVal * 255.0);
	if (this->bVal > 1.0)
		b = 255;
	else if (this->bVal < 0)
		b = 0;
	else
		b = (UInt32)Double2Int32(this->bVal * 255.0);
	return (a << 24) | (r << 16) | (g << 8) | (b);
}

void UtilUI::ColorDialog::ShowAlpha()
{
	if (!this->alphaShown)
	{
		this->alphaShown = true;
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		this->lblAlpha = ui->NewLabel(*this, CSTR("Alpha"));
		this->lblAlpha->SetRect(504, 548, 100, 23, false);
		sptr = Text::StrDouble(sbuff, this->aVal * 255.0);
		this->txtAlpha = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
		this->txtAlpha->SetRect(604, 548, 60, 23, false);
		this->txtAlpha->HandleTextChanged(OnAlphaChange, this);
	}	
}

void UtilUI::ColorDialog::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	NN<Media::MonitorMgr> nnmonMgr;
	if (this->monMgr.SetTo(nnmonMgr))
	{
		this->SetDPI(nnmonMgr->GetMonitorHDPI(this->GetHMonitor()), nnmonMgr->GetMonitorDDPI(this->GetHMonitor()));

		Media::ColorProfile color;
		color.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
		Math::Size2D<UOSInt> sz;
		DEL_CLASS(this->mainImg);
		DEL_CLASS(this->subImg);
		sz = this->pbMain->GetSizeP();
		NEW_CLASS(this->mainImg, Media::StaticImage(sz - Math::Coord2D<UOSInt>(2, 2), 0, 32, Media::PF_B8G8R8A8, 0, color, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
		sz = this->pbSub->GetSizeP();
		NEW_CLASS(this->subImg, Media::StaticImage(sz - Math::Coord2D<UOSInt>(2, 2), 0, 32, Media::PF_B8G8R8A8, 0, color, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));

		this->GenMainImage();
		this->GenSubImage();
	}
}

void UtilUI::ColorDialog::OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight)
{
	if (dispWidth > dispHeight)
	{
		this->SetSize(756, 640);
		this->pbColor->SetRect(604, 16, 112, 112, true);
		this->radR->SetRect(604, 164, 64, 23, true);
		this->radG->SetRect(604, 188, 64, 23, true);
		this->radB->SetRect(604, 212, 64, 23, true);
		this->radYIQY->SetRect(604, 260, 64, 23, true);
		this->radYIQI->SetRect(604, 284, 64, 23, true);
		this->radYIQQ->SetRect(604, 308, 64, 23, true);
		this->radHSVH->SetRect(604, 356, 64, 23, true);
		this->radHSVS->SetRect(604, 380, 64, 23, true);
		this->radHSVV->SetRect(604, 404, 64, 23, true);
		this->txtR->SetRect(668, 164, 48, 23, true);
		this->txtG->SetRect(668, 188, 48, 23, true);
		this->txtB->SetRect(668, 212, 48, 23, true);
		this->txtYIQY->SetRect(668, 260, 48, 23, true);
		this->txtYIQI->SetRect(668, 284, 48, 23, true);
		this->txtYIQQ->SetRect(668, 308, 48, 23, true);
		this->txtHSVH->SetRect(668, 356, 48, 23, true);
		this->txtHSVS->SetRect(668, 380, 48, 23, true);
		this->txtHSVV->SetRect(668, 404, 48, 23, true);
		this->lblTrans->SetRect(16, 546, 150, 23, true);
		this->txtTrans->SetRect(166, 546, 100, 23, true);
		this->lblPrimaries->SetRect(16, 570, 150, 23, true);
		this->txtPrimaries->SetRect(166, 570, 100, 23, true);
		this->btnOk->SetRect(604, 452, 100, 23, true);
		this->btnCancel->SetRect(604, 500, 100, 23, true);
	}
	else
	{
		this->SetSize(600, 800);
		this->pbColor->SetRect(16, 546, 112, 112, true);
		this->radR->SetRect(144, 546, 64, 23, true);
		this->radG->SetRect(144, 570, 64, 23, true);
		this->radB->SetRect(144, 594, 64, 23, true);
		this->txtR->SetRect(208, 546, 48, 23, true);
		this->txtG->SetRect(208, 570, 48, 23, true);
		this->txtB->SetRect(208, 594, 48, 23, true);

		this->radYIQY->SetRect(296, 546, 64, 23, true);
		this->radYIQI->SetRect(296, 570, 64, 23, true);
		this->radYIQQ->SetRect(296, 594, 64, 23, true);
		this->txtYIQY->SetRect(360, 546, 48, 23, true);
		this->txtYIQI->SetRect(360, 570, 48, 23, true);
		this->txtYIQQ->SetRect(360, 594, 48, 23, true);

		this->radHSVH->SetRect(448, 546, 64, 23, true);
		this->radHSVS->SetRect(448, 570, 64, 23, true);
		this->radHSVV->SetRect(448, 594, 64, 23, true);
		this->txtHSVH->SetRect(512, 546, 48, 23, true);
		this->txtHSVS->SetRect(512, 570, 48, 23, true);
		this->txtHSVV->SetRect(512, 594, 48, 23, true);

		this->lblTrans->SetRect(16, 666, 150, 23, true);
		this->txtTrans->SetRect(166, 666, 100, 23, true);
		this->lblPrimaries->SetRect(16, 690, 150, 23, true);
		this->txtPrimaries->SetRect(166, 690, 100, 23, true);
		this->btnOk->SetRect(448, 666, 100, 23, true);
		this->btnCancel->SetRect(448, 690, 100, 23, true);
	}
}

void UtilUI::ColorDialog::YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam)
{
}

void UtilUI::ColorDialog::RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam)
{
	this->UpdateColor();
	this->GenMainImage();
	this->GenSubImage();
}
