#include "Stdafx.h"
#include "Math/Unit/Pressure.h"
#include "SSWR/AVIRead/AVIRDHT22Form.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRDHT22Form::OnReadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDHT22Form *me = (SSWR::AVIRead::AVIRDHT22Form*)userObj;
	me->ReadData();
}

void __stdcall SSWR::AVIRead::AVIRDHT22Form::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRDHT22Form *me = (SSWR::AVIRead::AVIRDHT22Form*)userObj;
	if (me->chkAutoRead->IsChecked())
	{
		me->ReadData();
	}
}

void SSWR::AVIRead::AVIRDHT22Form::ReadData()
{
	Double temp;
	Double rh;
	Double humidity;
	UTF8Char sbuff[64];
	if (this->dht22->ReadData(&temp, &rh))
	{
		humidity = Math::Unit::Pressure::WaterVapourPressure(Math::Unit::Pressure::PU_KPASCAL, Math::Unit::Temperature::TU_CELSIUS, temp, rh);
		Text::StrDouble(sbuff, temp);
		this->txtTemp->SetText(sbuff);
		Text::StrDouble(sbuff, rh);
		this->txtRH->SetText(sbuff);
		Text::StrDouble(sbuff, humidity);
		this->txtHumidity->SetText(sbuff);
		this->txtStatus->SetText((const UTF8Char*)"Success");
	}
	else
	{
		this->txtStatus->SetText((const UTF8Char*)"Fail");
	}
}

SSWR::AVIRead::AVIRDHT22Form::AVIRDHT22Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::IOPin *pin) : UI::GUIForm(parent, 480, 160, ui)
{
	UTF8Char sbuff[256];
	this->SetFont(0, 0, 8.25, false);
	this->pin = pin;
	this->core = core;
	NEW_CLASS(this->dht22, IO::Device::DHT22(this->pin));
	this->pin->GetName(Text::StrConcatC(sbuff, UTF8STRC("DHT22 - ")));
	this->SetText(sbuff);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblTemp, UI::GUILabel(ui, this, (const UTF8Char*)"Temperature"));
	this->lblTemp->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtTemp, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtTemp->SetRect(104, 4, 100, 23, false);
	this->txtTemp->SetReadOnly(true);
	NEW_CLASS(this->lblRH, UI::GUILabel(ui, this, (const UTF8Char*)"RH"));
	this->lblRH->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtRH, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtRH->SetRect(104, 28, 100, 23, false);
	this->txtRH->SetReadOnly(true);
	NEW_CLASS(this->lblHumidity, UI::GUILabel(ui, this, (const UTF8Char*)"Absolute Humidity"));
	this->lblHumidity->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtHumidity, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtHumidity->SetRect(104, 52, 100, 23, false);
	this->txtHumidity->SetReadOnly(true);
	NEW_CLASS(this->btnRead, UI::GUIButton(ui, this, (const UTF8Char*)"Read"));
	this->btnRead->SetRect(104, 76, 75, 23, false);
	this->btnRead->HandleButtonClick(OnReadClicked, this);
	NEW_CLASS(this->chkAutoRead, UI::GUICheckBox(ui, this, (const UTF8Char*)"Auto Read", false));
	this->chkAutoRead->SetRect(184, 76, 100, 23, false);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, (const UTF8Char*)"Abs Humidity"));
	this->lblStatus->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtStatus->SetRect(104, 100, 100, 23, false);
	this->txtStatus->SetReadOnly(true);

	this->AddTimer(2000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRDHT22Form::~AVIRDHT22Form()
{
	DEL_CLASS(this->dht22);
	DEL_CLASS(this->pin);
}

void SSWR::AVIRead::AVIRDHT22Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
