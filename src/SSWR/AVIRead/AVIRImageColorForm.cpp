#include "Stdafx.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRImageColorForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnColorChg(AnyType userObj, UOSInt newPos)
{
	NN<SSWR::AVIRead::AVIRImageColorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageColorForm>();
	NN<Media::StaticImage> srcPrevImg;
	NN<Media::StaticImage> destPrevImg;
	if (!me->srcPrevImg.SetTo(srcPrevImg) || !me->destPrevImg.SetTo(destPrevImg))
	{
		return;
	}

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

	NN<const Media::ColorProfile> color;
	color = me->srcImg->info.color;
	if (color->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefVProfile();
	}
	else if (color->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefPProfile();
	}
	me->currBVal = bvalue * 0.01 - 1.0;
	me->currCVal = cvalue * 0.01;
	me->currGVal = gvalue * 0.01;
	me->rgbFilter->SetParameter(me->currBVal, me->currCVal, me->currGVal, color, srcPrevImg->info.storeBPP, srcPrevImg->info.pf, 0);
	me->rgbFilter->ProcessImage(srcPrevImg->data, destPrevImg->data, srcPrevImg->info.dispSize.x, srcPrevImg->info.dispSize.y, (srcPrevImg->info.storeSize.x * (srcPrevImg->info.storeBPP >> 3)), (srcPrevImg->info.storeSize.x * (srcPrevImg->info.storeBPP >> 3)), false);
	me->previewCtrl->SetImage(destPrevImg, true);
}

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageColorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageColorForm>();
	NN<const Media::ColorProfile> color;
	color = me->srcImg->info.color;
	if (color->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefVProfile();
	}
	else if (color->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		color = me->core->GetColorMgr()->GetDefPProfile();
	}
	me->rgbFilter->SetParameter(me->currBVal, me->currCVal, me->currGVal, color, me->srcImg->info.storeBPP, me->srcImg->info.pf, 0);
	me->rgbFilter->ProcessImage(me->srcImg->data, me->destImg->data, me->srcImg->info.dispSize.x, me->srcImg->info.dispSize.y, (me->srcImg->info.storeSize.x * (me->srcImg->info.storeBPP >> 3)), (me->srcImg->info.storeSize.x * (me->srcImg->info.storeBPP >> 3)), false);
	me->previewCtrl->SetImage(me->destImg.Ptr(), true);

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

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageColorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageColorForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRImageColorForm::OnLastValueClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageColorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageColorForm>();
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead");
	if (reg)
	{
		Int32 v;
		if (reg->GetValueI32(L"LastImgColorBright", v))
			me->hsbBright->SetPos((UInt32)v);
		if (reg->GetValueI32(L"LastImgColorContr", v))
			me->hsbContr->SetPos((UInt32)v);
		if (reg->GetValueI32(L"LastImgColorGamma", v))
			me->hsbGamma->SetPos((UInt32)v);
		IO::Registry::CloseRegistry(reg);
	}
}

SSWR::AVIRead::AVIRImageColorForm::AVIRImageColorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg, NN<UI::GUIPictureBoxDD> previewCtrl) : UI::GUIForm(parent, 640, 140, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Image Color"));
	this->SetNoResize(true);

	this->core = core;
	this->srcImg = srcImg;
	this->destImg = destImg;
	this->previewCtrl = previewCtrl;
	NEW_CLASS(this->rgbFilter, Media::RGBColorFilter(this->core->GetColorMgr()));
	this->srcPrevImg = this->previewCtrl->CreatePreviewImage(this->srcImg);
	NN<Media::StaticImage> img;
	if (this->srcPrevImg.SetTo(img))
	{
		this->destPrevImg = img->CreateStaticImage();
	}
	else
	{
		this->destPrevImg = 0;
	}
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lblBright = ui->NewLabel(*this, CSTR("Brightness"));
	this->lblBright->SetRect(0, 0, 100, 24, false);
	this->hsbBright = ui->NewHScrollBar(*this, 16);
	this->hsbBright->SetRect(100, 0, 450, 17, false);
	this->hsbBright->InitScrollBar(0, 10009, 1, 10);
	this->hsbBright->SetPos(1000);
	this->hsbBright->HandlePosChanged(OnColorChg, this);
	this->lblBrightV = ui->NewLabel(*this, CSTR(""));
	this->lblBrightV->SetRect(550, 0, 50, 24, false);
	this->lblContr = ui->NewLabel(*this, CSTR("Contrast"));
	this->lblContr->SetRect(0, 24, 100, 24, false);
	this->hsbContr = ui->NewHScrollBar(*this, 16);
	this->hsbContr->SetRect(100, 24, 450, 17, false);
	this->hsbContr->InitScrollBar(0, 1609, 1, 10);
	this->hsbContr->SetPos(100);
	this->hsbContr->HandlePosChanged(OnColorChg, this);
	this->lblContrV = ui->NewLabel(*this, CSTR(""));
	this->lblContrV->SetRect(550, 24, 50, 24, false);
	this->lblGamma = ui->NewLabel(*this, CSTR("Gamma"));
	this->lblGamma->SetRect(0, 48, 100, 24, false);
	this->hsbGamma = ui->NewHScrollBar(*this, 16);
	this->hsbGamma->SetRect(100, 48, 450, 17, false);
	this->hsbGamma->InitScrollBar(0, 1009, 1, 10);
	this->hsbGamma->SetPos(100);
	this->hsbGamma->HandlePosChanged(OnColorChg, this);
	this->lblGammaV = ui->NewLabel(*this, CSTR(""));
	this->lblGammaV->SetRect(550, 48, 50, 24, false);
	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(240, 72, 75, 24, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(325, 72, 75, 24, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->btnLastValue = ui->NewButton(*this, CSTR("&Last Value"));
	this->btnLastValue->SetRect(410, 72, 75, 24, false);
	this->btnLastValue->HandleButtonClick(OnLastValueClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	OnColorChg(this, 0);
}

SSWR::AVIRead::AVIRImageColorForm::~AVIRImageColorForm()
{
	DEL_CLASS(this->rgbFilter);
	this->srcPrevImg.Delete();
	this->destPrevImg.Delete();
}

void SSWR::AVIRead::AVIRImageColorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
