#include "Stdafx.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Media/ICCProfile.h"
#include "SSWR/AVIRead/AVIRColorCustomForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->r.x -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->r.x);
	me->txtRX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->r.x += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->r.x);
	me->txtRX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->r.y -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->r.y);
	me->txtRY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->r.y += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->r.y);
	me->txtRY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->g.x -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->g.x);
	me->txtGX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->g.x += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->g.x);
	me->txtGX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->g.y -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->g.y);
	me->txtGY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->g.y += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->g.y);
	me->txtGY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->b.x -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->b.x);
	me->txtBX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->b.x += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->b.x);
	me->txtBX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->b.y -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->b.y);
	me->txtBY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->b.y += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->b.y);
	me->txtBY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->w.x -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->w.x);
	me->txtWX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->w.x += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->w.x);
	me->txtWX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->w.y -= 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->w.y);
	me->txtWY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->w.y += 0.001;
	me->monColor->SetMonProfile(color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->w.y);
	me->txtWY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnCloseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	me->Close();
}

SSWR::AVIRead::AVIRColorCustomForm::AVIRColorCustomForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::MonitorColorManager> monColor) : UI::GUIForm(parent, 640, 480, ui)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	this->SetText(CSTR("Monitor Custom Color"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->monColor = monColor;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblRX = ui->NewLabel(*this, CSTR("Red x"));
	this->lblRX->SetRect(4, 4, 100, 23, false);
	this->txtRX = ui->NewTextBox(*this, CSTR(""));
	this->txtRX->SetRect(104, 4, 150, 23, false);
	this->txtRX->SetReadOnly(true);
	this->btnRXDown = ui->NewButton(*this, CSTR("-"));
	this->btnRXDown->SetRect(254, 4, 24, 23, false);
	this->btnRXDown->HandleButtonClick(OnRXDownClicked, this);
	this->btnRXUp = ui->NewButton(*this, CSTR("+"));
	this->btnRXUp->SetRect(278, 4, 24, 23, false);
	this->btnRXUp->HandleButtonClick(OnRXUpClicked, this);
	this->lblRY = ui->NewLabel(*this, CSTR("Red y"));
	this->lblRY->SetRect(4, 28, 100, 23, false);
	this->txtRY = ui->NewTextBox(*this, CSTR(""));
	this->txtRY->SetRect(104, 28, 150, 23, false);
	this->txtRY->SetReadOnly(true);
	this->btnRYDown = ui->NewButton(*this, CSTR("-"));
	this->btnRYDown->SetRect(254, 28, 24, 23, false);
	this->btnRYDown->HandleButtonClick(OnRYDownClicked, this);
	this->btnRYUp = ui->NewButton(*this, CSTR("+"));
	this->btnRYUp->SetRect(278, 28, 24, 23, false);
	this->btnRYUp->HandleButtonClick(OnRYUpClicked, this);
	this->lblGX = ui->NewLabel(*this, CSTR("Green x"));
	this->lblGX->SetRect(4, 52, 100, 23, false);
	this->txtGX = ui->NewTextBox(*this, CSTR(""));
	this->txtGX->SetRect(104, 52, 150, 23, false);
	this->txtGX->SetReadOnly(true);
	this->btnGXDown = ui->NewButton(*this, CSTR("-"));
	this->btnGXDown->SetRect(254, 52, 24, 23, false);
	this->btnGXDown->HandleButtonClick(OnGXDownClicked, this);
	this->btnGXUp = ui->NewButton(*this, CSTR("+"));
	this->btnGXUp->SetRect(278, 52, 24, 23, false);
	this->btnGXUp->HandleButtonClick(OnGXUpClicked, this);
	this->lblGY = ui->NewLabel(*this, CSTR("Green y"));
	this->lblGY->SetRect(4, 76, 100, 23, false);
	this->txtGY = ui->NewTextBox(*this, CSTR(""));
	this->txtGY->SetRect(104, 76, 150, 23, false);
	this->txtGY->SetReadOnly(true);
	this->btnGYDown = ui->NewButton(*this, CSTR("-"));
	this->btnGYDown->SetRect(254, 76, 24, 23, false);
	this->btnGYDown->HandleButtonClick(OnGYDownClicked, this);
	this->btnGYUp = ui->NewButton(*this, CSTR("+"));
	this->btnGYUp->SetRect(278, 76, 24, 23, false);
	this->btnGYUp->HandleButtonClick(OnGYUpClicked, this);
	this->lblBX = ui->NewLabel(*this, CSTR("Blue x"));
	this->lblBX->SetRect(4, 100, 100, 23, false);
	this->txtBX = ui->NewTextBox(*this, CSTR(""));
	this->txtBX->SetRect(104, 100, 150, 23, false);
	this->txtBX->SetReadOnly(true);
	this->btnBXDown = ui->NewButton(*this, CSTR("-"));
	this->btnBXDown->SetRect(254, 100, 24, 23, false);
	this->btnBXDown->HandleButtonClick(OnBXDownClicked, this);
	this->btnBXUp = ui->NewButton(*this, CSTR("+"));
	this->btnBXUp->SetRect(278, 100, 24, 23, false);
	this->btnBXUp->HandleButtonClick(OnBXUpClicked, this);
	this->lblBY = ui->NewLabel(*this, CSTR("Blue y"));
	this->lblBY->SetRect(4, 124, 100, 23, false);
	this->txtBY = ui->NewTextBox(*this, CSTR(""));
	this->txtBY->SetRect(104, 124, 150, 23, false);
	this->txtBY->SetReadOnly(true);
	this->btnBYDown = ui->NewButton(*this, CSTR("-"));
	this->btnBYDown->SetRect(254, 124, 24, 23, false);
	this->btnBYDown->HandleButtonClick(OnBYDownClicked, this);
	this->btnBYUp = ui->NewButton(*this, CSTR("+"));
	this->btnBYUp->SetRect(278, 124, 24, 23, false);
	this->btnBYUp->HandleButtonClick(OnBYUpClicked, this);
	this->lblWX = ui->NewLabel(*this, CSTR("White x"));
	this->lblWX->SetRect(4, 148, 100, 23, false);
	this->txtWX = ui->NewTextBox(*this, CSTR(""));
	this->txtWX->SetRect(104, 148, 150, 23, false);
	this->txtWX->SetReadOnly(true);
	this->btnWXDown = ui->NewButton(*this, CSTR("-"));
	this->btnWXDown->SetRect(254, 148, 24, 23, false);
	this->btnWXDown->HandleButtonClick(OnWXDownClicked, this);
	this->btnWXUp = ui->NewButton(*this, CSTR("+"));
	this->btnWXUp->SetRect(278, 148, 24, 23, false);
	this->btnWXUp->HandleButtonClick(OnWXUpClicked, this);
	this->lblWY = ui->NewLabel(*this, CSTR("White y"));
	this->lblWY->SetRect(4, 172, 100, 23, false);
	this->txtWY = ui->NewTextBox(*this, CSTR(""));
	this->txtWY->SetRect(104, 172, 150, 23, false);
	this->txtWY->SetReadOnly(true);
	this->btnWYDown = ui->NewButton(*this, CSTR("-"));
	this->btnWYDown->SetRect(254, 172, 24, 23, false);
	this->btnWYDown->HandleButtonClick(OnWYDownClicked, this);
	this->btnWYUp = ui->NewButton(*this, CSTR("+"));
	this->btnWYUp->SetRect(278, 172, 24, 23, false);
	this->btnWYUp->HandleButtonClick(OnWYUpClicked, this);
	this->btnClose = ui->NewButton(*this, CSTR("Close"));
	this->btnClose->SetRect(104, 196, 75, 23, false);
	this->btnClose->HandleButtonClick(OnCloseClicked, this);

	NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam = this->monColor->GetRGBParam();
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->r.x);
	this->txtRX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->r.y);
	this->txtRY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->g.x);
	this->txtGX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->g.y);
	this->txtGY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->b.x);
	this->txtBX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->b.y);
	this->txtBY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->w.x);
	this->txtWX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->w.y);
	this->txtWY->SetText(CSTRP(sbuff, sptr));
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->colorType = Media::ColorProfile::CT_CUSTOM;
	this->monColor->SetMonProfile(color);
}

SSWR::AVIRead::AVIRColorCustomForm::~AVIRColorCustomForm()
{
}

void SSWR::AVIRead::AVIRColorCustomForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
