#include "Stdafx.h"
#include "Math/Unit/Pressure.h"
#include "SSWR/AVIRead/AVIRDS18B20Form.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRDS18B20Form::OnSNClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDS18B20Form *me = (SSWR::AVIRead::AVIRDS18B20Form*)userObj;
	UInt8 buff[8];
	UTF8Char sbuff[32];
	if (me->ds18b20->ReadSensorID(buff))
	{
		Text::StrHexBytes(sbuff, buff, 7, ' ');
		me->txtSN->SetText(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRDS18B20Form::OnReadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDS18B20Form *me = (SSWR::AVIRead::AVIRDS18B20Form*)userObj;
	me->ReadData();
}

void __stdcall SSWR::AVIRead::AVIRDS18B20Form::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRDS18B20Form *me = (SSWR::AVIRead::AVIRDS18B20Form*)userObj;
	if (me->chkAutoRead->IsChecked())
	{
		me->ReadData();
	}
}

void SSWR::AVIRead::AVIRDS18B20Form::ReadData()
{
	Double temp;
	UTF8Char sbuff[64];
	if (this->ds18b20->ConvTemp() && this->ds18b20->ReadTemp(&temp))
	{
		Text::StrDouble(sbuff, temp);
		this->txtTemp->SetText(sbuff);
		this->txtStatus->SetText((const UTF8Char*)"Success");
	}
	else
	{
		this->txtStatus->SetText((const UTF8Char*)"Fail");
	}
}

SSWR::AVIRead::AVIRDS18B20Form::AVIRDS18B20Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::IOPin *pin) : UI::GUIForm(parent, 480, 160, ui)
{
	UTF8Char sbuff[256];
	this->SetFont(0, 0, 8.25, false);
	this->pin = pin;
	this->core = core;
	NEW_CLASS(this->oneWire, IO::OneWireGPIO(this->pin));
	NEW_CLASS(this->ds18b20, IO::Device::DS18B20(this->oneWire));
	this->pin->GetName(Text::StrConcatC(sbuff, UTF8STRC("DS18B20 - ")));
	this->SetText(sbuff);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblSN, UI::GUILabel(ui, this, (const UTF8Char*)"SN"));
	this->lblSN->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSN, UI::GUITextBox(ui, this, CSTR("")));
	this->txtSN->SetRect(104, 4, 200, 23, false);
	this->txtSN->SetReadOnly(true);
	NEW_CLASS(this->btnSN, UI::GUIButton(ui, this, (const UTF8Char*)"Read SN"));
	this->btnSN->SetRect(304, 4, 75, 23, false);
	this->btnSN->HandleButtonClick(OnSNClicked, this);
	NEW_CLASS(this->lblTemp, UI::GUILabel(ui, this, (const UTF8Char*)"Temperature"));
	this->lblTemp->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTemp, UI::GUITextBox(ui, this, CSTR("")));
	this->txtTemp->SetRect(104, 28, 100, 23, false);
	this->txtTemp->SetReadOnly(true);
	NEW_CLASS(this->btnRead, UI::GUIButton(ui, this, (const UTF8Char*)"Read"));
	this->btnRead->SetRect(104, 52, 75, 23, false);
	this->btnRead->HandleButtonClick(OnReadClicked, this);
	NEW_CLASS(this->chkAutoRead, UI::GUICheckBox(ui, this, (const UTF8Char*)"Auto Read", false));
	this->chkAutoRead->SetRect(184, 52, 100, 23, false);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, (const UTF8Char*)"Status"));
	this->lblStatus->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 76, 100, 23, false);
	this->txtStatus->SetReadOnly(true);

	this->AddTimer(2000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRDS18B20Form::~AVIRDS18B20Form()
{
	DEL_CLASS(this->ds18b20);
	DEL_CLASS(this->oneWire);
	DEL_CLASS(this->pin);
}

void SSWR::AVIRead::AVIRDS18B20Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
