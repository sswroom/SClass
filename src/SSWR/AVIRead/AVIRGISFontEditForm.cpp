#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRGISFontEditForm.h"
#include "UI/GUIFontDialog.h"
#include "UtilUI/ColorDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::FontNameClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISFontEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontEditForm>();
	NotNullPtr<UI::GUIFontDialog> dlg;
	if (me->currFontName == 0)
	{
		dlg = me->ui->NewFontDialog(CSTR_NULL, 0, me->isBold, false);
	}
	else
	{
		dlg = me->ui->NewFontDialog(me->currFontName, me->currFontSizePt, me->isBold, false);
	}
	NotNullPtr<Text::String> s;
	if (dlg->ShowDialog(me->hwnd) && dlg->GetFontName().SetTo(s))
	{
		SDEL_STRING(me->currFontName);
		me->currFontName = s->Clone().Ptr();
		me->currFontSizePt = dlg->GetFontSizePt();
		me->isBold = dlg->IsBold();
		me->txtFontName->SetText(me->currFontName->ToCString());
		me->UpdateFontPreview();
	}
	dlg.Delete();
}

Bool __stdcall SSWR::AVIRead::AVIRGISFontEditForm::FontColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton mouseBtn)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISFontEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontEditForm>();
	if (mouseBtn == UI::GUIControl::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, color, me->core->GetMonitorMgr());
		dlg.SetColor32(me->currColor);
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->currColor = dlg.GetColor32();
			me->pbFontColor->SetBGColor(me->colorConv->ConvRGB8(me->currColor));
			me->pbFontColor->Redraw();
			me->UpdateFontPreview();
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::BufferSizeChanged(AnyType userObj, UOSInt scrollPos)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISFontEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontEditForm>();
	me->currBuffSize = scrollPos;
	me->UpdateFontPreview();
}

Bool __stdcall SSWR::AVIRead::AVIRGISFontEditForm::BufferColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton mouseBtn)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISFontEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontEditForm>();
	if (mouseBtn == UI::GUIControl::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, color, me->core->GetMonitorMgr());
		dlg.SetColor32(me->currBuffColor);
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->currBuffColor = dlg.GetColor32();
			me->pbBufferColor->SetBGColor(me->colorConv->ConvRGB8(me->currBuffColor));
			me->pbBufferColor->Redraw();
			me->UpdateFontPreview();
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::FontsDoubleClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISFontEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontEditForm>();
	if (me->fontStyle < 0)
		return;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::OKClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISFontEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontEditForm>();
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	NotNullPtr<Text::String> currFontName;
	if (me->fontStyle < 0 || !currFontName.Set(me->currFontName))
		return;
	if ((sptr = me->txtStyleName->GetText(sbuff)) != 0 && sbuff[0] != 0)
	{
		me->env->SetFontStyleName(me->fontStyle, CSTRP(sbuff, sptr));
	}

	me->env->ChgFontStyle(me->fontStyle, currFontName, me->currFontSizePt, me->isBold, me->currColor, me->currBuffSize, me->currBuffColor);
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontEditForm::CancelClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISFontEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontEditForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISFontEditForm::UpdateFontPreview()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Math::Size2DDbl sz;
	Math::Size2D<UOSInt> usz;
	NotNullPtr<Media::DrawImage> dimg;
	NotNullPtr<Media::DrawFont> f;
	NotNullPtr<Media::DrawBrush> b;
	usz = this->pbFontPreview->GetSizeP();
	if (dimg.Set(this->eng->CreateImage32(usz, Media::AT_NO_ALPHA)))
	{
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);

		if ((sptr = this->env->GetFontStyleName(this->fontStyle, sbuff)) == 0 || sbuff[0] == 0)
		{
			sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Style ")), (Int32)this->fontStyle);
		}

		b = dimg->NewBrushARGB(this->colorConv->ConvRGB8(0xffc0c0c0));
		dimg->DrawRect(Math::Coord2DDbl(0, 0), usz.ToDouble(), 0, b);
		dimg->DelBrush(b);

		if (this->currFontName)
		{
			f = dimg->NewFontPt(this->currFontName->ToCString(), this->currFontSizePt, this->isBold?((Media::DrawEngine::DrawFontStyle)(Media::DrawEngine::DFS_BOLD | Media::DrawEngine::DFS_ANTIALIAS)):Media::DrawEngine::DFS_ANTIALIAS, this->core->GetCurrCodePage());
			sz = dimg->GetTextSize(f, CSTRP(sbuff, sptr));
			if (this->currBuffSize > 0)
			{
				b = dimg->NewBrushARGB(this->colorConv->ConvRGB8(this->currBuffColor));
				dimg->DrawStringB((usz.ToDouble() - sz) * 0.5, CSTRP(sbuff, sptr), f, b, (UOSInt)Double2Int32(UOSInt2Double(this->currBuffSize) * this->GetHDPI() / this->GetDDPI()));
				dimg->DelBrush(b);
			}
			b = dimg->NewBrushARGB(this->colorConv->ConvRGB8(this->currColor));
			dimg->DrawString((usz.ToDouble() - sz) * 0.5, CSTRP(sbuff, sptr), f, b);
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
}

void SSWR::AVIRead::AVIRGISFontEditForm::UpdateDisplay()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	NotNullPtr<Text::String> fontName;
	if ((sptr = env->GetFontStyleName(this->fontStyle, sbuff)) != 0)
	{
		this->txtStyleName->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtStyleName->SetText(CSTR(""));
	}
	SDEL_STRING(this->currFontName);
	if (env->GetFontStyle(this->fontStyle, fontName, this->currFontSizePt, this->isBold, this->currColor, this->currBuffSize, this->currBuffColor))
	{
		this->currFontName = fontName->Clone().Ptr();
		this->txtFontName->SetText(this->currFontName->ToCString());
	}
	else
	{
		this->currFontName = 0;
		this->txtFontName->SetText(CSTR(""));
	}
	this->pbFontColor->SetBGColor(this->colorConv->ConvRGB8(this->currColor));
	this->hsbBufferSize->SetPos(this->currBuffSize);
	this->pbBufferColor->SetBGColor(this->colorConv->ConvRGB8(this->currBuffColor));
}

SSWR::AVIRead::AVIRGISFontEditForm::AVIRGISFontEditForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, NotNullPtr<Media::DrawEngine> eng, UOSInt fontStyle) : UI::GUIForm(parent, 480, 306, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->fontStyle = fontStyle;
	this->currFontName = 0;
	this->currColor = 0xff000000;
	this->currBuffColor = 0xff000000;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(*this);
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->colorConv, Media::ColorConv(srcProfile, destProfile, this->colorSess.Ptr()));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText(CSTR("Edit Font Style"));
	this->SetFont(0, 0, 8.25, false);

	this->pbFontPreview = ui->NewPictureBox(*this, eng, false, false);
	this->pbFontPreview->SetRect(0, 0, 288, 64, false);
	this->pbFontPreview->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStyleName = ui->NewLabel(*this, CSTR("Style Name"));
	this->lblStyleName->SetRect(0, 68, 100, 23, false);
	this->txtStyleName = ui->NewTextBox(*this, CSTR(""));
	this->txtStyleName->SetRect(104, 68, 100, 23, false);
	this->lblFontName = ui->NewLabel(*this, CSTR("Font Name"));
	this->lblFontName->SetRect(0, 92, 100, 23, false);
	this->txtFontName = ui->NewTextBox(*this, CSTR(""));
	this->txtFontName->SetReadOnly(true);
	this->txtFontName->SetRect(104, 92, 100, 23, false);
	this->btnFontName = ui->NewButton(*this, CSTR("S&elect"));
	this->btnFontName->SetRect(208, 92, 75, 23, false);
	this->btnFontName->HandleButtonClick(FontNameClicked, this);
	this->lblFontColor = ui->NewLabel(*this, CSTR("Color"));
	this->lblFontColor->SetRect(0, 116, 100, 23, false);
	this->pbFontColor = ui->NewPictureBox(*this, eng, false, false);
	this->pbFontColor->SetRect(104, 116, 144, 23, false);
	this->pbFontColor->HandleMouseDown(FontColorClicked, this);
	this->lblBufferSize = ui->NewLabel(*this, CSTR("Buffer"));
	this->lblBufferSize->SetRect(0, 140, 100, 23, false);
	this->hsbBufferSize = ui->NewHScrollBar(*this, 152);
	this->hsbBufferSize->SetRect(104, 140, 200, 23, false);
	this->hsbBufferSize->InitScrollBar(0, 17, 0, 1);
	this->hsbBufferSize->HandlePosChanged(BufferSizeChanged, this);
	this->lblBufferColor = ui->NewLabel(*this, CSTR("Buffer Color"));
	this->lblBufferColor->SetRect(0, 164, 100, 23, false);
	this->pbBufferColor = ui->NewPictureBox(*this, eng, false, false);
	this->pbBufferColor->SetRect(104, 164, 144, 23, false);
	this->pbBufferColor->HandleMouseDown(BufferColorClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 196, 75, 23, false);;
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
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
	SDEL_STRING(this->currFontName);
	DEL_CLASS(this->colorConv);
	this->colorSess->RemoveHandler(*this);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISFontEditForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISFontEditForm::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRGISFontEditForm::RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam)
{
	this->colorConv->RGBParamChanged(rgbParam);
	this->pbFontColor->SetBGColor(this->colorConv->ConvRGB8(this->currColor));
	this->pbBufferColor->SetBGColor(this->colorConv->ConvRGB8(this->currBuffColor));
	this->pbFontColor->Redraw();
	this->pbBufferColor->Redraw();
	this->UpdateFontPreview();
}
