#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRBYDC9RForm.h"
#include "SSWR/AVIRead/AVIRSelCANForm.h"

void __stdcall SSWR::AVIRead::AVIRBYDC9RForm::OnCANBusClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBYDC9RForm *me = (SSWR::AVIRead::AVIRBYDC9RForm*)userObj;
	if (me->listener)
	{
		DEL_CLASS(me->listener);
		me->listener = 0;
		me->txtCANBus->SetText(CSTR(""));
		me->btnCANBus->SetText(CSTR("Open"));
	}
	else
	{
		SSWR::AVIRead::AVIRSelCANForm dlg(0, me->ui, me->core, me->ssl, me->c9r.GetCANHandler());
		if (dlg.ShowDialog(me) == DR_OK)
		{
			me->listener = dlg.GetListener();
			Text::StringBuilderUTF8 sb;
			me->listener->ToString(sb);
			me->txtCANBus->SetText(sb.ToCString());
			me->btnCANBus->SetText(CSTR("Close"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRBYDC9RForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRBYDC9RForm *me = (SSWR::AVIRead::AVIRBYDC9RForm*)userObj;
	IO::Device::BYDC9R::DeviceStatus currStatus;
	me->c9r.GetStatus(&currStatus);
	UTF8Char sbuff[64];
	UTF8Char *sptr;

	if (currStatus.speedkmHr != me->dispStatus.speedkmHr)
	{
		me->dispStatus.speedkmHr = currStatus.speedkmHr;
		sptr = Text::StrDouble(sbuff, currStatus.speedkmHr);
		me->txtSpeedKmHr->SetText(CSTRP(sbuff, sptr));
	}
	if (currStatus.door1 != me->dispStatus.door1)
	{
		me->dispStatus.door1 = currStatus.door1;
		me->txtDoor1->SetText(IO::Device::BYDC9RHandler::DoorStatusGetName(currStatus.door1));
	}
	if (currStatus.door2 != me->dispStatus.door2)
	{
		me->dispStatus.door2 = currStatus.door2;
		me->txtDoor2->SetText(IO::Device::BYDC9RHandler::DoorStatusGetName(currStatus.door2));
	}
	if (currStatus.batteryLevelPercent != me->dispStatus.batteryLevelPercent)
	{
		me->dispStatus.batteryLevelPercent = currStatus.batteryLevelPercent;
		sptr = Text::StrDouble(sbuff, currStatus.batteryLevelPercent);
		me->txtBatteryLevel->SetText(CSTRP(sbuff, sptr));
	}
	if (currStatus.motorRPM != me->dispStatus.motorRPM)
	{
		me->dispStatus.motorRPM = currStatus.motorRPM;
		sptr = Text::StrInt32(sbuff, currStatus.motorRPM);
		me->txtMotorRPM->SetText(CSTRP(sbuff, sptr));
	}
	if (currStatus.batteryChargedStart != me->dispStatus.batteryChargedStart)
	{
		me->dispStatus.batteryChargedStart = currStatus.batteryChargedStart;
		sptr = currStatus.batteryChargedStart.ToStringNoZone(sbuff);
		me->txtBatteryStartCharging->SetText(CSTRP(sbuff, sptr));
	}
	if (currStatus.leftMotorMode != me->dispStatus.leftMotorMode)
	{
		me->dispStatus.leftMotorMode = currStatus.leftMotorMode;
		me->txtLeftMotorMode->SetText(IO::Device::BYDC9RHandler::MotorModeGetName(currStatus.leftMotorMode));
	}
	if (currStatus.rightMotorMode != me->dispStatus.rightMotorMode)
	{
		me->dispStatus.rightMotorMode = currStatus.rightMotorMode;
		me->txtRightMotorMode->SetText(IO::Device::BYDC9RHandler::MotorModeGetName(currStatus.rightMotorMode));
	}
	if (currStatus.carbinDoorBack != me->dispStatus.carbinDoorBack)
	{
		me->dispStatus.carbinDoorBack = currStatus.carbinDoorBack;
		me->txtCarbinDoorBack->SetText(currStatus.carbinDoorBack?CSTR("true"):CSTR("false"));
	}
	if (currStatus.carbinDoorLeft != me->dispStatus.carbinDoorLeft)
	{
		me->dispStatus.carbinDoorLeft = currStatus.carbinDoorLeft;
		me->txtCarbinDoorLeft->SetText(currStatus.carbinDoorLeft?CSTR("true"):CSTR("false"));
	}
	if (currStatus.carbinDoorRight != me->dispStatus.carbinDoorRight)
	{
		me->dispStatus.carbinDoorRight = currStatus.carbinDoorRight;
		me->txtCarbinDoorRight->SetText(currStatus.carbinDoorRight?CSTR("true"):CSTR("false"));
	}
	if (currStatus.powerMode != me->dispStatus.powerMode)
	{
		me->dispStatus.powerMode = currStatus.powerMode;
		me->txtPowerMode->SetText(IO::Device::BYDC9RHandler::PowerStatusGetName(currStatus.powerMode));
	}
	if (currStatus.batteryCharging != me->dispStatus.batteryCharging)
	{
		me->dispStatus.batteryCharging = currStatus.batteryCharging;
		me->txtBatteryCharging->SetText(IO::Device::BYDC9RHandler::ChargingStatusGetName(currStatus.batteryCharging));
	}
	if (currStatus.okLED != me->dispStatus.okLED)
	{
		me->dispStatus.okLED = currStatus.okLED;
		me->txtOkLED->SetText(currStatus.okLED?CSTR("true"):CSTR("false"));
	}
}

SSWR::AVIRead::AVIRBYDC9RForm::AVIRBYDC9RForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("BYD C9R"));

	this->core = core;
	this->listener = 0;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpCANBus = ui->NewGroupBox(*this, CSTR("CAN Bus"));
	this->grpCANBus->SetRect(0, 0, 400, 39, false);
	this->lblCANBus = ui->NewLabel(this->grpCANBus, CSTR("CAN Bus"));
	this->lblCANBus->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtCANBus, UI::GUITextBox(ui, this->grpCANBus, CSTR("")));
	this->txtCANBus->SetRect(100, 0, 100, 23, false);
	this->txtCANBus->SetReadOnly(true);
	this->btnCANBus = ui->NewButton(this->grpCANBus, CSTR("Open"));
	this->btnCANBus->SetRect(200, 0, 75, 23, false);
	this->btnCANBus->HandleButtonClick(OnCANBusClicked, this);

	UTF8Char sbuff[64];
	UTF8Char *sptr;
	this->c9r.GetStatus(&this->dispStatus);
	this->lblSpeedKmHr = ui->NewLabel(*this, CSTR("Speed (km/Hr)"));
	this->lblSpeedKmHr->SetRect(4, 64, 100, 23, false);
	NEW_CLASS(this->txtSpeedKmHr, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtSpeedKmHr->SetRect(104, 64, 100, 23, false);
	this->txtSpeedKmHr->SetReadOnly(true);
	this->lblDoor1 = ui->NewLabel(*this, CSTR("Door1"));
	this->lblDoor1->SetRect(4, 88, 100, 23, false);
	NEW_CLASS(this->txtDoor1, UI::GUITextBox(ui, *this, IO::Device::BYDC9RHandler::DoorStatusGetName(this->dispStatus.door1)));
	this->txtDoor1->SetRect(104, 88, 100, 23, false);
	this->txtDoor1->SetReadOnly(true);
	this->lblDoor2 = ui->NewLabel(*this, CSTR("Door2"));
	this->lblDoor2->SetRect(4, 112, 100, 23, false);
	NEW_CLASS(this->txtDoor2, UI::GUITextBox(ui, *this, IO::Device::BYDC9RHandler::DoorStatusGetName(this->dispStatus.door2)));
	this->txtDoor2->SetRect(104, 112, 100, 23, false);
	this->txtDoor2->SetReadOnly(true);
	this->lblBatteryLevel = ui->NewLabel(*this, CSTR("Battery Level (%)"));
	this->lblBatteryLevel->SetRect(4, 136, 100, 23, false);
	NEW_CLASS(this->txtBatteryLevel, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtBatteryLevel->SetRect(104, 136, 100, 23, false);
	this->txtBatteryLevel->SetReadOnly(true);
	this->lblMotorRPM = ui->NewLabel(*this, CSTR("Motor RPM"));
	this->lblMotorRPM->SetRect(4, 160, 100, 23, false);
	NEW_CLASS(this->txtMotorRPM, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtMotorRPM->SetRect(104, 160, 100, 23, false);
	this->txtMotorRPM->SetReadOnly(true);
	this->lblBatteryStartCharging = ui->NewLabel(*this, CSTR("Battery Start Charging"));
	this->lblBatteryStartCharging->SetRect(4, 184, 100, 23, false);
	sptr = this->dispStatus.batteryChargedStart.ToStringNoZone(sbuff);
	NEW_CLASS(this->txtBatteryStartCharging, UI::GUITextBox(ui, *this, CSTRP(sbuff, sptr)));
	this->txtBatteryStartCharging->SetRect(104, 184, 200, 23, false);
	this->txtBatteryStartCharging->SetReadOnly(true);
	this->lblLeftMotorMode = ui->NewLabel(*this, CSTR("Left Motor Mode"));
	this->lblLeftMotorMode->SetRect(4, 208, 100, 23, false);
	NEW_CLASS(this->txtLeftMotorMode, UI::GUITextBox(ui, *this, IO::Device::BYDC9RHandler::MotorModeGetName(this->dispStatus.leftMotorMode)));
	this->txtLeftMotorMode->SetRect(104, 208, 100, 23, false);
	this->txtLeftMotorMode->SetReadOnly(true);
	this->lblRightotorMode = ui->NewLabel(*this, CSTR("Left Motor Mode"));
	this->lblRightotorMode->SetRect(4, 232, 100, 23, false);
	NEW_CLASS(this->txtRightMotorMode, UI::GUITextBox(ui, *this, IO::Device::BYDC9RHandler::MotorModeGetName(this->dispStatus.rightMotorMode)));
	this->txtRightMotorMode->SetRect(104, 232, 100, 23, false);
	this->txtRightMotorMode->SetReadOnly(true);
	this->lblCarbinDoorBack = ui->NewLabel(*this, CSTR("Carbin Door Back"));
	this->lblCarbinDoorBack->SetRect(4, 256, 100, 23, false);
	NEW_CLASS(this->txtCarbinDoorBack, UI::GUITextBox(ui, *this, CSTR("false")));
	this->txtCarbinDoorBack->SetRect(104, 256, 100, 23, false);
	this->txtCarbinDoorBack->SetReadOnly(true);
	this->lblCarbinDoorLeft = ui->NewLabel(*this, CSTR("Carbin Door Left"));
	this->lblCarbinDoorLeft->SetRect(4, 280, 100, 23, false);
	NEW_CLASS(this->txtCarbinDoorLeft, UI::GUITextBox(ui, *this, CSTR("false")));
	this->txtCarbinDoorLeft->SetRect(104, 280, 100, 23, false);
	this->txtCarbinDoorLeft->SetReadOnly(true);
	this->lblCarbinDoorRight = ui->NewLabel(*this, CSTR("Carbin Door Right"));
	this->lblCarbinDoorRight->SetRect(4, 304, 100, 23, false);
	NEW_CLASS(this->txtCarbinDoorRight, UI::GUITextBox(ui, *this, CSTR("false")));
	this->txtCarbinDoorRight->SetRect(104, 304, 100, 23, false);
	this->txtCarbinDoorRight->SetReadOnly(true);
	this->lblPowerMode = ui->NewLabel(*this, CSTR("Power Mode"));
	this->lblPowerMode->SetRect(4, 328, 100, 23, false);
	NEW_CLASS(this->txtPowerMode, UI::GUITextBox(ui, *this, IO::Device::BYDC9RHandler::PowerStatusGetName(this->dispStatus.powerMode)));
	this->txtPowerMode->SetRect(104, 328, 100, 23, false);
	this->txtPowerMode->SetReadOnly(true);
	this->lblBatteryCharging = ui->NewLabel(*this, CSTR("Battery Charging"));
	this->lblBatteryCharging->SetRect(4, 352, 100, 23, false);
	NEW_CLASS(this->txtBatteryCharging, UI::GUITextBox(ui, *this, IO::Device::BYDC9RHandler::ChargingStatusGetName(this->dispStatus.batteryCharging)));
	this->txtBatteryCharging->SetRect(104, 352, 100, 23, false);
	this->txtBatteryCharging->SetReadOnly(true);
	this->lblOkLED = ui->NewLabel(*this, CSTR("Ok LED"));
	this->lblOkLED->SetRect(4, 376, 100, 23, false);
	NEW_CLASS(this->txtOkLED, UI::GUITextBox(ui, *this, CSTR("false")));
	this->txtOkLED->SetRect(104, 376, 100, 23, false);
	this->txtOkLED->SetReadOnly(true);

	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRBYDC9RForm::~AVIRBYDC9RForm()
{
	SDEL_CLASS(this->listener);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRBYDC9RForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
