#include "Stdafx.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Media/ICCProfile.h"
#include "SSWR/AVIRead/AVIRColorCustomForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->rx -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->rx);
	me->txtRX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->rx += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->rx);
	me->txtRX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->ry -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->ry);
	me->txtRY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnRYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->ry += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->ry);
	me->txtRY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->gx -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->gx);
	me->txtGX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->gx += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->gx);
	me->txtGX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->gy -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->gy);
	me->txtGY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnGYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->gy += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->gy);
	me->txtGY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->bx -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->bx);
	me->txtBX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->bx += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->bx);
	me->txtBX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->by -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->by);
	me->txtBY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnBYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->by += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->by);
	me->txtBY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWXDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->wx -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->wx);
	me->txtWX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWXUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->wx += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->wx);
	me->txtWX->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWYDownClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->wy -= 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->wy);
	me->txtWY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnWYUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const Media::IColorHandler::RGBPARAM2 *rgbParam = me->monColor->GetRGBParam();
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->wy += 0.001;
	me->monColor->SetMonProfile(&color);
	sptr = Text::StrDouble(sbuff, color.GetPrimaries()->wy);
	me->txtWY->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRColorCustomForm::OnCloseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRColorCustomForm *me = (SSWR::AVIRead::AVIRColorCustomForm*)userObj;
	me->Close();
}

SSWR::AVIRead::AVIRColorCustomForm::AVIRColorCustomForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::MonitorColorManager *monColor) : UI::GUIForm(parent, 640, 480, ui)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	this->SetText(CSTR("Monitor Custom Color"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->monColor = monColor;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblRX, UI::GUILabel(ui, this, CSTR("Red x")));
	this->lblRX->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtRX, UI::GUITextBox(ui, this, CSTR("")));
	this->txtRX->SetRect(104, 4, 150, 23, false);
	this->txtRX->SetReadOnly(true);
	NEW_CLASS(this->btnRXDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnRXDown->SetRect(254, 4, 24, 23, false);
	this->btnRXDown->HandleButtonClick(OnRXDownClicked, this);
	NEW_CLASS(this->btnRXUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnRXUp->SetRect(278, 4, 24, 23, false);
	this->btnRXUp->HandleButtonClick(OnRXUpClicked, this);
	NEW_CLASS(this->lblRY, UI::GUILabel(ui, this, CSTR("Red y")));
	this->lblRY->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtRY, UI::GUITextBox(ui, this, CSTR("")));
	this->txtRY->SetRect(104, 28, 150, 23, false);
	this->txtRY->SetReadOnly(true);
	NEW_CLASS(this->btnRYDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnRYDown->SetRect(254, 28, 24, 23, false);
	this->btnRYDown->HandleButtonClick(OnRYDownClicked, this);
	NEW_CLASS(this->btnRYUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnRYUp->SetRect(278, 28, 24, 23, false);
	this->btnRYUp->HandleButtonClick(OnRYUpClicked, this);
	NEW_CLASS(this->lblGX, UI::GUILabel(ui, this, CSTR("Green x")));
	this->lblGX->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtGX, UI::GUITextBox(ui, this, CSTR("")));
	this->txtGX->SetRect(104, 52, 150, 23, false);
	this->txtGX->SetReadOnly(true);
	NEW_CLASS(this->btnGXDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnGXDown->SetRect(254, 52, 24, 23, false);
	this->btnGXDown->HandleButtonClick(OnGXDownClicked, this);
	NEW_CLASS(this->btnGXUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnGXUp->SetRect(278, 52, 24, 23, false);
	this->btnGXUp->HandleButtonClick(OnGXUpClicked, this);
	NEW_CLASS(this->lblGY, UI::GUILabel(ui, this, CSTR("Green y")));
	this->lblGY->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtGY, UI::GUITextBox(ui, this, CSTR("")));
	this->txtGY->SetRect(104, 76, 150, 23, false);
	this->txtGY->SetReadOnly(true);
	NEW_CLASS(this->btnGYDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnGYDown->SetRect(254, 76, 24, 23, false);
	this->btnGYDown->HandleButtonClick(OnGYDownClicked, this);
	NEW_CLASS(this->btnGYUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnGYUp->SetRect(278, 76, 24, 23, false);
	this->btnGYUp->HandleButtonClick(OnGYUpClicked, this);
	NEW_CLASS(this->lblBX, UI::GUILabel(ui, this, CSTR("Blue x")));
	this->lblBX->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtBX, UI::GUITextBox(ui, this, CSTR("")));
	this->txtBX->SetRect(104, 100, 150, 23, false);
	this->txtBX->SetReadOnly(true);
	NEW_CLASS(this->btnBXDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnBXDown->SetRect(254, 100, 24, 23, false);
	this->btnBXDown->HandleButtonClick(OnBXDownClicked, this);
	NEW_CLASS(this->btnBXUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnBXUp->SetRect(278, 100, 24, 23, false);
	this->btnBXUp->HandleButtonClick(OnBXUpClicked, this);
	NEW_CLASS(this->lblBY, UI::GUILabel(ui, this, CSTR("Blue y")));
	this->lblBY->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtBY, UI::GUITextBox(ui, this, CSTR("")));
	this->txtBY->SetRect(104, 124, 150, 23, false);
	this->txtBY->SetReadOnly(true);
	NEW_CLASS(this->btnBYDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnBYDown->SetRect(254, 124, 24, 23, false);
	this->btnBYDown->HandleButtonClick(OnBYDownClicked, this);
	NEW_CLASS(this->btnBYUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnBYUp->SetRect(278, 124, 24, 23, false);
	this->btnBYUp->HandleButtonClick(OnBYUpClicked, this);
	NEW_CLASS(this->lblWX, UI::GUILabel(ui, this, CSTR("White x")));
	this->lblWX->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtWX, UI::GUITextBox(ui, this, CSTR("")));
	this->txtWX->SetRect(104, 148, 150, 23, false);
	this->txtWX->SetReadOnly(true);
	NEW_CLASS(this->btnWXDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnWXDown->SetRect(254, 148, 24, 23, false);
	this->btnWXDown->HandleButtonClick(OnWXDownClicked, this);
	NEW_CLASS(this->btnWXUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnWXUp->SetRect(278, 148, 24, 23, false);
	this->btnWXUp->HandleButtonClick(OnWXUpClicked, this);
	NEW_CLASS(this->lblWY, UI::GUILabel(ui, this, CSTR("White y")));
	this->lblWY->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtWY, UI::GUITextBox(ui, this, CSTR("")));
	this->txtWY->SetRect(104, 172, 150, 23, false);
	this->txtWY->SetReadOnly(true);
	NEW_CLASS(this->btnWYDown, UI::GUIButton(ui, this, CSTR("-")));
	this->btnWYDown->SetRect(254, 172, 24, 23, false);
	this->btnWYDown->HandleButtonClick(OnWYDownClicked, this);
	NEW_CLASS(this->btnWYUp, UI::GUIButton(ui, this, CSTR("+")));
	this->btnWYUp->SetRect(278, 172, 24, 23, false);
	this->btnWYUp->HandleButtonClick(OnWYUpClicked, this);
	NEW_CLASS(this->btnClose, UI::GUIButton(ui, this, CSTR("Close")));
	this->btnClose->SetRect(104, 196, 75, 23, false);
	this->btnClose->HandleButtonClick(OnCloseClicked, this);

	const Media::IColorHandler::RGBPARAM2 *rgbParam = this->monColor->GetRGBParam();
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->rx);
	this->txtRX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->ry);
	this->txtRY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->gx);
	this->txtGX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->gy);
	this->txtGY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->bx);
	this->txtBX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->by);
	this->txtBY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->wx);
	this->txtWX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetPrimariesRead()->wy);
	this->txtWY->SetText(CSTRP(sbuff, sptr));
	Media::ColorProfile color(rgbParam->monProfile);
	color.GetPrimaries()->colorType = Media::ColorProfile::CT_CUSTOM;
	this->monColor->SetMonProfile(&color);
}

SSWR::AVIRead::AVIRColorCustomForm::~AVIRColorCustomForm()
{
}

void SSWR::AVIRead::AVIRColorCustomForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
