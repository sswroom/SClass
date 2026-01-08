#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISFontForm.h"
#include "Text/MyStringFloat.h"
#include "UtilUI/ColorDialog.h"
#include "UI/GUIFontDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISFontForm::OnFontClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontForm>();
	NN<UI::GUIFontDialog> dlg = me->ui->NewFontDialog(me->fontName, me->fontSizePt, false, false);
	if (dlg->ShowDialog(me->hwnd))
	{
		NN<Text::String> s;
		if (dlg->GetFontName().SetTo(s))
		{
			me->fontName->Release();
			me->fontName = s->Clone();
			me->fontSizePt = dlg->GetFontSizePt();
			me->UpdateFontText();
			me->UpdateFontPreview();
		}
	}
	dlg.Delete();
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISFontForm::OnColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRGISFontForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontForm>();
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(0, me->ui, me->core->GetColorManager(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, color, me->core->GetMonitorMgr());
		dlg.SetColor32(me->fontColor);
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->fontColor = dlg.GetColor32();
			me->pbColor->SetBGColor(me->colorConv->ConvRGB8(me->fontColor));
			me->pbColor->Redraw();
			me->UpdateFontPreview();
		}
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRGISFontForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISFontForm::UpdateFontText()
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->fontName);
	sb.AppendC(UTF8STRC(", "));
	sb.AppendDouble(this->fontSizePt);
	sb.AppendC(UTF8STRC("pt"));
	this->txtFont->SetText(sb.ToCString());
}

void SSWR::AVIRead::AVIRGISFontForm::UpdateFontPreview()
{
	this->previewImage.Delete();
	Math::Size2D<UOSInt> sz = this->pbPreview->GetSizeP();
	NN<Media::DrawImage> dimg;
	if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
	{
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		this->core->GenFontPreview(dimg, this->eng, this->fontName->ToCString(), this->fontSizePt, this->fontColor, this->colorConv);
		this->previewImage = dimg->ToStaticImage();
		this->eng->DeleteImage(dimg);
		this->pbPreview->SetImage(this->previewImage);
	}
}

SSWR::AVIRead::AVIRGISFontForm::AVIRGISFontForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::DrawEngine> eng, Optional<Text::String> fontName, Double fontSizePt, UInt32 fontColor) : UI::GUIForm(parent, 480, 306, ui)
{
	this->core = core;
	this->eng = eng;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(*this);
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->colorConv, Media::ColorConv(srcProfile, destProfile, this->colorSess.Ptr()));
	NN<Text::String> s;
	if (fontName.SetTo(s))
	{
		this->fontName = s->Clone();
	}
	else
	{
		this->fontName = Text::String::New(UTF8STRC("Arial"));
	}
	this->fontSizePt = fontSizePt;
	this->fontColor = fontColor;
	this->previewImage = 0;

	this->SetText(CSTR("Font Modify"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->pbPreview = ui->NewPictureBox(*this, eng, true, false);
	this->pbPreview->SetRect(0, 0, 100, 60, false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblFont = ui->NewLabel(this->pnlMain, CSTR("Font"));
	this->lblFont->SetRect(4, 4, 100, 23, false);
	this->txtFont = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtFont->SetRect(104, 4, 200, 23, false);
	this->txtFont->SetReadOnly(true);
	this->btnFont = ui->NewButton(this->pnlMain, CSTR("Change"));
	this->btnFont->SetRect(304, 4, 75, 23, false);
	this->btnFont->HandleButtonClick(OnFontClicked, this);
	this->lblColor = ui->NewLabel(this->pnlMain, CSTR("Color"));
	this->lblColor->SetRect(4, 28, 100, 23, false);
	this->pbColor = ui->NewPictureBox(this->pnlMain, eng, true, false);
	this->pbColor->SetRect(104, 28, 200, 23, false);
	this->pbColor->HandleMouseDown(OnColorClicked, this);
	this->btnOK = ui->NewButton(this->pnlMain, CSTR("OK"));
	this->btnOK->SetRect(104, 80, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlMain, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 80, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	this->pbColor->SetBGColor(this->colorConv->ConvRGB8(this->fontColor));
	this->UpdateFontText();
	this->UpdateFontPreview();
}

SSWR::AVIRead::AVIRGISFontForm::~AVIRGISFontForm()
{
	this->previewImage.Delete();
	this->fontName->Release();
	this->colorConv.Delete();
	this->colorSess->RemoveHandler(*this);
	this->ClearChildren();
	this->eng->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISFontForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISFontForm::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRGISFontForm::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->colorConv->RGBParamChanged(rgbParam);
	this->pbColor->SetBGColor(this->colorConv->ConvRGB8(this->fontColor));
	this->pbColor->Redraw();
	this->UpdateFontPreview();
}

NN<Text::String> SSWR::AVIRead::AVIRGISFontForm::GetFontName() const
{
	return this->fontName;
}

Double SSWR::AVIRead::AVIRGISFontForm::GetFontSizePt()
{
	return this->fontSizePt;
}

UInt32 SSWR::AVIRead::AVIRGISFontForm::GetFontColor()
{
	return this->fontColor;
}

Bool SSWR::AVIRead::AVIRGISFontForm::IsChanged()
{
	return this->changed;
}
