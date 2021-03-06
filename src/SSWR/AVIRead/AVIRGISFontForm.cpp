#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISFontForm.h"
#include "Text/MyStringFloat.h"
#include "UtilUI/ColorDialog.h"
#include "UI/FontDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISFontForm::OnFontClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontForm *me = (SSWR::AVIRead::AVIRGISFontForm *)userObj;
	UI::FontDialog *dlg;
	NEW_CLASS(dlg, UI::FontDialog(me->fontName, me->fontSizePt, false, false));
	if (dlg->ShowDialog(me->hwnd))
	{
		if (me->fontName)
		{
			Text::StrDelNew(me->fontName);
		}
		me->fontName = Text::StrCopyNew(dlg->GetFontName());
		me->fontSizePt = dlg->GetFontSizePt();
		me->UpdateFontText();
		me->UpdateFontPreview();
	}
	DEL_CLASS(dlg);
}

Bool __stdcall SSWR::AVIRead::AVIRGISFontForm::OnColorClicked(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn)
{
	SSWR::AVIRead::AVIRGISFontForm *me = (SSWR::AVIRead::AVIRGISFontForm *)userObj;
	UtilUI::ColorDialog *dlg;
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		NEW_CLASS(dlg, UtilUI::ColorDialog(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, &color, me->core->GetMonitorMgr()));
		dlg->SetColor32(me->fontColor);
		if (dlg->ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->fontColor = dlg->GetColor32();
			me->pbColor->SetBGColor(me->colorConv->ConvRGB8(me->fontColor));
			me->pbColor->Redraw();
			me->UpdateFontPreview();
		}
		DEL_CLASS(dlg);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISFontForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontForm *me = (SSWR::AVIRead::AVIRGISFontForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontForm *me = (SSWR::AVIRead::AVIRGISFontForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISFontForm::UpdateFontText()
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->fontName);
	sb.Append((const UTF8Char*)", ");
	Text::SBAppendF64(&sb, this->fontSizePt);
	sb.Append((const UTF8Char*)"pt");
	this->txtFont->SetText(sb.ToString());
}

void SSWR::AVIRead::AVIRGISFontForm::UpdateFontPreview()
{
	UOSInt w;
	UOSInt h;
	SDEL_CLASS(this->previewImage);
	this->pbPreview->GetSizeP(&w, &h);
	Media::DrawImage *dimg = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
	dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	this->core->GenFontPreview(dimg, this->eng, this->fontName, this->fontSizePt, this->fontColor, this->colorConv);
	this->previewImage = dimg->ToStaticImage();
	this->eng->DeleteImage(dimg);
	this->pbPreview->SetImage(this->previewImage);
}

SSWR::AVIRead::AVIRGISFontForm::AVIRGISFontForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::DrawEngine *eng, const UTF8Char *fontName, Double fontSizePt, UInt32 fontColor) : UI::GUIForm(parent, 480, 306, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(this);
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->colorConv, Media::ColorConv(&srcProfile, &destProfile, this->colorSess));
	if (fontName)
	{
		this->fontName = Text::StrCopyNew(fontName);
	}
	else
	{
		this->fontName = Text::StrCopyNew((const UTF8Char*)"Arial");
	}
	this->fontSizePt = fontSizePt;
	this->fontColor = fontColor;
	this->previewImage = 0;

	this->SetText((const UTF8Char*)"Font Modify");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	NEW_CLASS(this->pbPreview, UI::GUIPictureBox(ui, this, eng, true, false));
	this->pbPreview->SetRect(0, 0, 100, 60, false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblFont, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Font"));
	this->lblFont->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFont, UI::GUITextBox(ui, this->pnlMain, (const UTF8Char*)""));
	this->txtFont->SetRect(104, 4, 200, 23, false);
	this->txtFont->SetReadOnly(true);
	NEW_CLASS(this->btnFont, UI::GUIButton(ui, this->pnlMain, (const UTF8Char*)"Change"));
	this->btnFont->SetRect(304, 4, 75, 23, false);
	this->btnFont->HandleButtonClick(OnFontClicked, this);
	NEW_CLASS(this->lblColor, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Color"));
	this->lblColor->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->pbColor, UI::GUIPictureBox(ui, this->pnlMain, eng, true, false));
	this->pbColor->SetRect(104, 28, 200, 23, false);
	this->pbColor->HandleMouseDown(OnColorClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlMain, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(104, 80, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlMain, (const UTF8Char*)"Cancel"));
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
	if (this->previewImage)
	{
		DEL_CLASS(this->previewImage);
		this->previewImage = 0;
	}
	if (this->fontName)
	{
		Text::StrDelNew(this->fontName);
		this->fontName = 0;
	}
	DEL_CLASS(this->colorConv);
	this->colorSess->RemoveHandler(this);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISFontForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISFontForm::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void SSWR::AVIRead::AVIRGISFontForm::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	this->colorConv->RGBParamChanged(rgbParam);
	this->pbColor->SetBGColor(this->colorConv->ConvRGB8(this->fontColor));
	this->pbColor->Redraw();
	this->UpdateFontPreview();
}

const UTF8Char *SSWR::AVIRead::AVIRGISFontForm::GetFontName()
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
