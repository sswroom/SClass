#include "Stdafx.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRImageColorForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnColorChg(void *userObj, UOSInt newPos)
{
	SSWR::AVIRead::AVIRImageColorForm *me = (SSWR::AVIRead::AVIRImageColorForm*)userObj;
	UTF8Char sbuff[256];
	UTF8Char *sptr;

	Double bvalue = UOSInt2Double(me->hsbBright->GetPos()) * 0.1;
	Double cvalue = UOSInt2Double(me->hsbContr->GetPos());
	Double gvalue = UOSInt2Double(me->hsbGamma->GetPos());
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, bvalue), UTF8STRC("%"));
	me->lblBrightV->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, cvalue), UTF8STRC("%"));
	me->lblContrV->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, gvalue), UTF8STRC("%"));
	me->lblGammaV->SetText(CSTRP(sbuff, sptr));

	Media::ColorProfile *color;
	color = me->srcImg->info->color;
	if (color->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefVProfile();
	}
	else if (color->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefPProfile();
	}
	me->currBVal = bvalue * 0.01 - 1.0;
	me->currCVal = cvalue * 0.01;
	me->currGVal = gvalue * 0.01;
	me->rgbFilter->SetParameter(me->currBVal, me->currCVal, me->currGVal, color, me->srcPrevImg->info->storeBPP, me->srcPrevImg->info->pf, 0);
	me->rgbFilter->ProcessImage(me->srcPrevImg->data, me->destPrevImg->data, me->srcPrevImg->info->dispWidth, me->srcPrevImg->info->dispHeight, (me->srcPrevImg->info->storeWidth * (me->srcPrevImg->info->storeBPP >> 3)), (me->srcPrevImg->info->storeWidth * (me->srcPrevImg->info->storeBPP >> 3)), false);
	me->previewCtrl->SetImage(me->destPrevImg, true);
}

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRImageColorForm *me = (SSWR::AVIRead::AVIRImageColorForm*)userObj;
	Media::ColorProfile *color;
	color = me->srcImg->info->color;
	if (color->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefVProfile();
	}
	else if (color->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefPProfile();
	}
	me->rgbFilter->SetParameter(me->currBVal, me->currCVal, me->currGVal, color, me->srcImg->info->storeBPP, me->srcImg->info->pf, 0);
	me->rgbFilter->ProcessImage(me->srcImg->data, me->destImg->data, me->srcImg->info->dispWidth, me->srcImg->info->dispHeight, (me->srcImg->info->storeWidth * (me->srcImg->info->storeBPP >> 3)), (me->srcImg->info->storeWidth * (me->srcImg->info->storeBPP >> 3)), false);
	me->previewCtrl->SetImage(me->destImg, true);

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
	if (reg)
	{
		reg->SetValue(L"LastImgColorBright", (Int32)me->hsbBright->GetPos());
		reg->SetValue(L"LastImgColorContr", (Int32)me->hsbContr->GetPos());
		reg->SetValue(L"LastImgColorGamma", (Int32)me->hsbGamma->GetPos());
		IO::Registry::CloseRegistry(reg);
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRImageColorForm *me = (SSWR::AVIRead::AVIRImageColorForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnLastValueClick(void *userObj)
{
	SSWR::AVIRead::AVIRImageColorForm *me = (SSWR::AVIRead::AVIRImageColorForm*)userObj;
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
	if (reg)
	{
		Int32 v;
		if (reg->GetValueI32(L"LastImgColorBright", &v))
			me->hsbBright->SetPos((UInt32)v);
		if (reg->GetValueI32(L"LastImgColorContr", &v))
			me->hsbContr->SetPos((UInt32)v);
		if (reg->GetValueI32(L"LastImgColorGamma", &v))
			me->hsbGamma->SetPos((UInt32)v);
		IO::Registry::CloseRegistry(reg);
	}
}

SSWR::AVIRead::AVIRImageColorForm::AVIRImageColorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::StaticImage *srcImg, Media::StaticImage *destImg, UI::GUIPictureBoxDD *previewCtrl) : UI::GUIForm(parent, 640, 140, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Image Color"));
	this->SetNoResize(true);

	this->core = core;
	this->srcImg = srcImg;
	this->destImg = destImg;
	this->previewCtrl = previewCtrl;
	NEW_CLASS(this->rgbFilter, Media::RGBColorFilter(this->core->GetColorMgr()));
	this->srcPrevImg = 0;
	this->destPrevImg = 0;
	this->srcPrevImg = this->previewCtrl->CreatePreviewImage(this->srcImg);
	this->destPrevImg = this->srcPrevImg->CreateStaticImage();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lblBright, UI::GUILabel(ui, this, CSTR("Brightness")));
	this->lblBright->SetRect(0, 0, 100, 24, false);
	NEW_CLASS(this->hsbBright, UI::GUIHScrollBar(ui, this, 16));
	this->hsbBright->SetRect(100, 0, 450, 17, false);
	this->hsbBright->InitScrollBar(0, 10009, 1, 10);
	this->hsbBright->SetPos(1000);
	this->hsbBright->HandlePosChanged(OnColorChg, this);
	NEW_CLASS(this->lblBrightV, UI::GUILabel(ui, this, CSTR("")));
	this->lblBrightV->SetRect(550, 0, 50, 24, false);
	NEW_CLASS(this->lblContr, UI::GUILabel(ui, this, CSTR("Contrast")));
	this->lblContr->SetRect(0, 24, 100, 24, false);
	NEW_CLASS(this->hsbContr, UI::GUIHScrollBar(ui, this, 16));
	this->hsbContr->SetRect(100, 24, 450, 17, false);
	this->hsbContr->InitScrollBar(0, 1609, 1, 10);
	this->hsbContr->SetPos(100);
	this->hsbContr->HandlePosChanged(OnColorChg, this);
	NEW_CLASS(this->lblContrV, UI::GUILabel(ui, this, CSTR("")));
	this->lblContrV->SetRect(550, 24, 50, 24, false);
	NEW_CLASS(this->lblGamma, UI::GUILabel(ui, this, CSTR("Gamma")));
	this->lblGamma->SetRect(0, 48, 100, 24, false);
	NEW_CLASS(this->hsbGamma, UI::GUIHScrollBar(ui, this, 16));
	this->hsbGamma->SetRect(100, 48, 450, 17, false);
	this->hsbGamma->InitScrollBar(0, 1009, 1, 10);
	this->hsbGamma->SetPos(100);
	this->hsbGamma->HandlePosChanged(OnColorChg, this);
	NEW_CLASS(this->lblGammaV, UI::GUILabel(ui, this, CSTR("")));
	this->lblGammaV->SetRect(550, 48, 50, 24, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("&OK")));
	this->btnOK->SetRect(240, 72, 75, 24, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("&Cancel")));
	this->btnCancel->SetRect(325, 72, 75, 24, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	NEW_CLASS(this->btnLastValue, UI::GUIButton(ui, this, CSTR("&Last Value")));
	this->btnLastValue->SetRect(410, 72, 75, 24, false);
	this->btnLastValue->HandleButtonClick(OnLastValueClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	OnColorChg(this, 0);
}

SSWR::AVIRead::AVIRImageColorForm::~AVIRImageColorForm()
{
	DEL_CLASS(this->rgbFilter);
	SDEL_CLASS(this->srcPrevImg);
	SDEL_CLASS(this->destPrevImg);
}

void SSWR::AVIRead::AVIRImageColorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
