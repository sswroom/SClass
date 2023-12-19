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
	UTF8Char *sptr;
	if (me->ds18b20->ReadSensorID(buff))
	{
		sptr = Text::StrHexBytes(sbuff, buff, 7, ' ');
		me->txtSN->SetText(CSTRP(sbuff, sptr));
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
	UTF8Char *sptr;
	if (this->ds18b20->ConvTemp() && this->ds18b20->ReadTemp(&temp))
	{
		sptr = Text::StrDouble(sbuff, temp);
		this->txtTemp->SetText(CSTRP(sbuff, sptr));
		this->txtStatus->SetText(CSTR("Success"));
	}
	else
	{
		this->txtStatus->SetText(CSTR("Fail"));
	}
}

SSWR::AVIRead::AVIRDS18B20Form::AVIRDS18B20Form(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::IOPin> pin) : UI::GUIForm(parent, 480, 160, ui)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	this->SetFont(0, 0, 8.25, false);
	this->pin = pin;
	this->core = core;
	NEW_CLASS(this->oneWire, IO::OneWireGPIO(this->pin));
	NEW_CLASS(this->ds18b20, IO::Device::DS18B20(this->oneWire));
	sptr = this->pin->GetName(Text::StrConcatC(sbuff, UTF8STRC("DS18B20 - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblSN = ui->NewLabel(*this, CSTR("SN"));
	this->lblSN->SetRect(4, 4, 100, 23, false);
	this->txtSN = ui->NewTextBox(*this, CSTR(""));
	this->txtSN->SetRect(104, 4, 200, 23, false);
	this->txtSN->SetReadOnly(true);
	this->btnSN = ui->NewButton(*this, CSTR("Read SN"));
	this->btnSN->SetRect(304, 4, 75, 23, false);
	this->btnSN->HandleButtonClick(OnSNClicked, this);
	this->lblTemp = ui->NewLabel(*this, CSTR("Temperature"));
	this->lblTemp->SetRect(4, 28, 100, 23, false);
	this->txtTemp = ui->NewTextBox(*this, CSTR(""));
	this->txtTemp->SetRect(104, 28, 100, 23, false);
	this->txtTemp->SetReadOnly(true);
	this->btnRead = ui->NewButton(*this, CSTR("Read"));
	this->btnRead->SetRect(104, 52, 75, 23, false);
	this->btnRead->HandleButtonClick(OnReadClicked, this);
	NEW_CLASS(this->chkAutoRead, UI::GUICheckBox(ui, *this, CSTR("Auto Read"), false));
	this->chkAutoRead->SetRect(184, 52, 100, 23, false);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 76, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 76, 100, 23, false);
	this->txtStatus->SetReadOnly(true);

	this->AddTimer(2000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRDS18B20Form::~AVIRDS18B20Form()
{
	DEL_CLASS(this->ds18b20);
	DEL_CLASS(this->oneWire);
	this->pin.Delete();
}

void SSWR::AVIRead::AVIRDS18B20Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
