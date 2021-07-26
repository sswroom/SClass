#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRGISFontEditForm.h"
#include "UI/FontDialog.h"
#include "UtilUI/ColorDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::FontNameClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontEditForm *me = (SSWR::AVIRead::AVIRGISFontEditForm *)userObj;
	UI::FontDialog *dlg;
	if (me->currFontName == 0)
	{
		NEW_CLASS(dlg, UI::FontDialog());
	}
	else
	{
		NEW_CLASS(dlg, UI::FontDialog(me->currFontName, me->currFontSizePt, me->isBold, false));
	}
	if (dlg->ShowDialog(me->hwnd))
	{
		if (me->currFontName)
		{
			Text::StrDelNew(me->currFontName);
		}
		me->currFontName = Text::StrCopyNew(dlg->GetFontName());
		me->currFontSizePt = dlg->GetFontSizePt();
		me->isBold = dlg->IsBold();
		me->txtFontName->SetText(me->currFontName);
		me->UpdateFontPreview();
	}
	DEL_CLASS(dlg);
}

Bool __stdcall SSWR::AVIRead::AVIRGISFontEditForm::FontColorClicked(void *userObj, OSInt x, OSInt y, MouseButton mouseBtn)
{
	SSWR::AVIRead::AVIRGISFontEditForm *me = (SSWR::AVIRead::AVIRGISFontEditForm *)userObj;
	UtilUI::ColorDialog *dlg;
	if (mouseBtn == UI::GUIControl::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		NEW_CLASS(dlg, UtilUI::ColorDialog(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, &color, me->core->GetMonitorMgr()));
		dlg->SetColor32(me->currColor);
		if (dlg->ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->currColor = dlg->GetColor32();
			me->pbFontColor->SetBGColor(me->colorConv->ConvRGB8(me->currColor));
			me->pbFontColor->Redraw();
			me->UpdateFontPreview();
		}
		DEL_CLASS(dlg);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::BufferSizeChanged(void *userObj, UOSInt scrollPos)
{
	SSWR::AVIRead::AVIRGISFontEditForm *me = (SSWR::AVIRead::AVIRGISFontEditForm *)userObj;
	me->currBuffSize = scrollPos;
	me->UpdateFontPreview();
}

Bool __stdcall SSWR::AVIRead::AVIRGISFontEditForm::BufferColorClicked(void *userObj, OSInt x, OSInt y, MouseButton mouseBtn)
{
	SSWR::AVIRead::AVIRGISFontEditForm *me = (SSWR::AVIRead::AVIRGISFontEditForm *)userObj;
	UtilUI::ColorDialog *dlg;
	if (mouseBtn == UI::GUIControl::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		NEW_CLASS(dlg, UtilUI::ColorDialog(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, &color, me->core->GetMonitorMgr()));
		dlg->SetColor32(me->currBuffColor);
		if (dlg->ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->currBuffColor = dlg->GetColor32();
			me->pbBufferColor->SetBGColor(me->colorConv->ConvRGB8(me->currBuffColor));
			me->pbBufferColor->Redraw();
			me->UpdateFontPreview();
		}
		DEL_CLASS(dlg);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::FontsDoubleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontEditForm *me = (SSWR::AVIRead::AVIRGISFontEditForm *)userObj;
	if (me->fontStyle < 0)
		return;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::OKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontEditForm *me = (SSWR::AVIRead::AVIRGISFontEditForm *)userObj;
	UTF8Char sbuff[256];
	if (me->fontStyle < 0)
		return;
	if (me->txtStyleName->GetText(sbuff) && sbuff[0] != 0)
	{
		me->env->SetFontStyleName(me->fontStyle, sbuff);
	}

	me->env->ChgFontStyle(me->fontStyle, me->currFontName, me->currFontSizePt, me->isBold, me->currColor, me->currBuffSize, me->currBuffColor);
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::CancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontEditForm *me = (SSWR::AVIRead::AVIRGISFontEditForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISFontEditForm::UpdateFontPreview()
{
	UTF8Char sbuff[256];
	Double sz[2];
	UOSInt w;
	UOSInt h;
	Media::DrawImage *dimg;
	Media::DrawFont *f;
	Media::DrawBrush *b;
	this->pbFontPreview->GetSizeP(&w, &h);
	dimg = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
	dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);

	if (this->env->GetFontStyleName(this->fontStyle, sbuff) == 0 || sbuff[0] == 0)
	{
		Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"Style "), (Int32)this->fontStyle);
	}

	b = dimg->NewBrushARGB(this->colorConv->ConvRGB8(0xffc0c0c0));
	dimg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
	dimg->DelBrush(b);

	if (this->currFontName)
	{
		f = dimg->NewFontPt(this->currFontName, this->currFontSizePt, this->isBold?((Media::DrawEngine::DrawFontStyle)(Media::DrawEngine::DFS_BOLD | Media::DrawEngine::DFS_ANTIALIAS)):Media::DrawEngine::DFS_ANTIALIAS, this->core->GetCurrCodePage());
		dimg->GetTextSize(f, sbuff, sz);
		if (this->currBuffSize > 0)
		{
			b = dimg->NewBrushARGB(this->colorConv->ConvRGB8(this->currBuffColor));
			dimg->DrawStringB((Math::UOSInt2Double(w) - sz[0]) * 0.5, (Math::UOSInt2Double(h) - sz[1]) * 0.5, sbuff, f, b, (UOSInt)Math::Double2Int32(Math::UOSInt2Double(this->currBuffSize) * this->GetHDPI() / this->GetDDPI()));
			dimg->DelBrush(b);
		}
		b = dimg->NewBrushARGB(this->colorConv->ConvRGB8(this->currColor));
		dimg->DrawString((Math::UOSInt2Double(w) - sz[0]) * 0.5, (Math::UOSInt2Double(h) - sz[1]) * 0.5, sbuff, f, b);
		dimg->DelBrush(b);
		dimg->DelFont(f);
	}

	if (this->previewImage)
	{
		DEL_CLASS(this->previewImage);
	}
	this->previewImage = dimg->ToStaticImage();
	this->eng->DeleteImage(dimg);
	this->pbFontPreview->SetImage(this->previewImage);
	this->pbFontPreview->SetNoBGColor(true);
}

void SSWR::AVIRead::AVIRGISFontEditForm::UpdateDisplay()
{
	UTF8Char sbuff[256];
	const UTF8Char *fontName;
	if (env->GetFontStyleName(this->fontStyle, sbuff))
	{
		this->txtStyleName->SetText(sbuff);
	}
	else
	{
		this->txtStyleName->SetText((const UTF8Char*)"");
	}
	env->GetFontStyle(this->fontStyle, &fontName, &this->currFontSizePt, &this->isBold, &this->currColor, &this->currBuffSize, &this->currBuffColor);
	if (this->currFontName)
	{
		Text::StrDelNew(this->currFontName);
	}
	if (fontName)
	{
		this->currFontName = Text::StrCopyNew(fontName);
		this->txtFontName->SetText(this->currFontName);
	}
	else
	{
		this->currFontName = 0;
		this->txtFontName->SetText((const UTF8Char*)"");
	}
	this->pbFontColor->SetBGColor(this->colorConv->ConvRGB8(this->currColor));
	this->hsbBufferSize->SetPos(this->currBuffSize);
	this->pbBufferColor->SetBGColor(this->colorConv->ConvRGB8(this->currBuffColor));
}

SSWR::AVIRead::AVIRGISFontEditForm::AVIRGISFontEditForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, Media::DrawEngine *eng, UOSInt fontStyle) : UI::GUIForm(parent, 480, 306, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->fontStyle = fontStyle;
	this->currFontName = 0;
	this->currColor = 0xff000000;
	this->currBuffColor = 0xff000000;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(this);
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->colorConv, Media::ColorConv(&srcProfile, &destProfile, this->colorSess));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText((const UTF8Char*)"Edit Font Style");
	this->SetFont(0, 8.25, false);

	NEW_CLASS(this->pbFontPreview, UI::GUIPictureBox(ui, this, eng, false, false));
	this->pbFontPreview->SetRect(0, 0, 288, 64, false);
	this->pbFontPreview->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStyleName, UI::GUILabel(ui, this, (const UTF8Char*)"Style Name"));
	this->lblStyleName->SetRect(0, 68, 100, 23, false);
	NEW_CLASS(this->txtStyleName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtStyleName->SetRect(104, 68, 100, 23, false);
	NEW_CLASS(this->lblFontName, UI::GUILabel(ui, this, (const UTF8Char*)"Font Name"));
	this->lblFontName->SetRect(0, 92, 100, 23, false);
	NEW_CLASS(this->txtFontName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtFontName->SetReadOnly(true);
	this->txtFontName->SetRect(104, 92, 100, 23, false);
	NEW_CLASS(this->btnFontName, UI::GUIButton(ui, this, (const UTF8Char*)"S&elect"));
	this->btnFontName->SetRect(208, 92, 75, 23, false);
	this->btnFontName->HandleButtonClick(FontNameClicked, this);
	NEW_CLASS(this->lblFontColor, UI::GUILabel(ui, this, (const UTF8Char*)"Color"));
	this->lblFontColor->SetRect(0, 116, 100, 23, false);
	NEW_CLASS(this->pbFontColor, UI::GUIPictureBox(ui, this, eng, false, false));
	this->pbFontColor->SetRect(104, 116, 144, 23, false);
	this->pbFontColor->HandleMouseDown(FontColorClicked, this);
	NEW_CLASS(this->lblBufferSize, UI::GUILabel(ui, this, (const UTF8Char*)"Buffer"));
	this->lblBufferSize->SetRect(0, 140, 100, 23, false);
	NEW_CLASS(this->hsbBufferSize, UI::GUIHScrollBar(ui, this, 152));
	this->hsbBufferSize->SetRect(104, 140, 200, 23, false);
	this->hsbBufferSize->InitScrollBar(0, 17, 0, 1);
	this->hsbBufferSize->HandlePosChanged(BufferSizeChanged, this);
	NEW_CLASS(this->lblBufferColor, UI::GUILabel(ui, this, (const UTF8Char*)"Buffer Color"));
	this->lblBufferColor->SetRect(0, 164, 100, 23, false);
	NEW_CLASS(this->pbBufferColor, UI::GUIPictureBox(ui, this, eng, false, false));
	this->pbBufferColor->SetRect(104, 164, 144, 23, false);
	this->pbBufferColor->HandleMouseDown(BufferColorClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(104, 196, 75, 23, false);;
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->SetDefaultButton(this->btnOK);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(184, 196, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetCancelButton(this->btnCancel);
	this->previewImage = 0;

	this->changed = false;
	this->UpdateDisplay();
	this->UpdateFontPreview();
}

SSWR::AVIRead::AVIRGISFontEditForm::~AVIRGISFontEditForm()
{
	if (this->previewImage)
	{
		DEL_CLASS(this->previewImage);
		this->previewImage = 0;
	}
	if (this->currFontName)
	{
		Text::StrDelNew(this->currFontName);
		this->currFontName = 0;
	}
	DEL_CLASS(this->colorConv);
	this->colorSess->RemoveHandler(this);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISFontEditForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISFontEditForm::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void SSWR::AVIRead::AVIRGISFontEditForm::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	this->colorConv->RGBParamChanged(rgbParam);
	this->pbFontColor->SetBGColor(this->colorConv->ConvRGB8(this->currColor));
	this->pbBufferColor->SetBGColor(this->colorConv->ConvRGB8(this->currBuffColor));
	this->pbFontColor->Redraw();
	this->pbBufferColor->Redraw();
	this->UpdateFontPreview();
}
