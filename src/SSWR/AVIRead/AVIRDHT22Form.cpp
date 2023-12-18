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
	UTF8Char *sptr;
	if (this->dht22->ReadData(temp, rh))
	{
		humidity = Math::Unit::Pressure::WaterVapourPressure(Math::Unit::Pressure::PU_KPASCAL, Math::Unit::Temperature::TU_CELSIUS, temp, rh);
		sptr = Text::StrDouble(sbuff, temp);
		this->txtTemp->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, rh);
		this->txtRH->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, humidity);
		this->txtHumidity->SetText(CSTRP(sbuff, sptr));
		this->txtStatus->SetText(CSTR("Success"));
	}
	else
	{
		this->txtStatus->SetText(CSTR("Fail"));
	}
}

SSWR::AVIRead::AVIRDHT22Form::AVIRDHT22Form(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::IOPin> pin) : UI::GUIForm(parent, 480, 160, ui)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	this->SetFont(0, 0, 8.25, false);
	this->pin = pin;
	this->core = core;
	NEW_CLASS(this->dht22, IO::Device::DHT22(this->pin));
	sptr = this->pin->GetName(Text::StrConcatC(sbuff, UTF8STRC("DHT22 - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblTemp = ui->NewLabel(*this, CSTR("Temperature"));
	this->lblTemp->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtTemp, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtTemp->SetRect(104, 4, 100, 23, false);
	this->txtTemp->SetReadOnly(true);
	this->lblRH = ui->NewLabel(*this, CSTR("RH"));
	this->lblRH->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtRH, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtRH->SetRect(104, 28, 100, 23, false);
	this->txtRH->SetReadOnly(true);
	this->lblHumidity = ui->NewLabel(*this, CSTR("Absolute Humidity"));
	this->lblHumidity->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtHumidity, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtHumidity->SetRect(104, 52, 100, 23, false);
	this->txtHumidity->SetReadOnly(true);
	this->btnRead = ui->NewButton(*this, CSTR("Read"));
	this->btnRead->SetRect(104, 76, 75, 23, false);
	this->btnRead->HandleButtonClick(OnReadClicked, this);
	NEW_CLASS(this->chkAutoRead, UI::GUICheckBox(ui, *this, CSTR("Auto Read"), false));
	this->chkAutoRead->SetRect(184, 76, 100, 23, false);
	this->lblStatus = ui->NewLabel(*this, CSTR("Abs Humidity"));
	this->lblStatus->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtStatus->SetRect(104, 100, 100, 23, false);
	this->txtStatus->SetReadOnly(true);

	this->AddTimer(2000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRDHT22Form::~AVIRDHT22Form()
{
	DEL_CLASS(this->dht22);
	this->pin.Delete();
}

void SSWR::AVIRead::AVIRDHT22Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
