#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRAccelerometerForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRAccelerometerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRAccelerometerForm *me = (SSWR::AVIRead::AVIRAccelerometerForm*)userObj;
	UTF8Char sbuff[64];
	Double x;
	Double y;
	Double z;
	if (me->motion->UpdateStatus())
	{
		me->motion->GetValues(&x, &y, &z);

		Text::StrDouble(sbuff, x);
		me->txtX->SetText(sbuff);
		Text::StrDouble(sbuff, y);
		me->txtY->SetText(sbuff);
		Text::StrDouble(sbuff, z);
		me->txtZ->SetText(sbuff);

		if (me->currMoving != me->motion->IsMovving())
		{
			me->currMoving = !me->currMoving;
			if (me->currMoving)
			{
				me->pnlMoving->SetBGColor(0xffff0000);
			}
			else
			{
				me->pnlMoving->SetBGColor(0xff40ff40);
			}
			me->pnlMoving->Redraw();
		}
	}
	else
	{
		me->txtX->SetText((const UTF8Char*)"-");
		me->txtY->SetText((const UTF8Char*)"-");
		me->txtZ->SetText((const UTF8Char*)"-");
	}
}

SSWR::AVIRead::AVIRAccelerometerForm::AVIRAccelerometerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::SensorAccelerometer *acc) : UI::GUIForm(parent, 240, 160, ui)
{
	this->SetText((const UTF8Char*)"Accelerometer");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	NEW_CLASS(this->motion, IO::MotionDetectorAccelerometer(acc, true));
	this->currMoving = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblX, UI::GUILabel(ui, this, (const UTF8Char*)"X"));
	this->lblX->SetRect(4, 4, 50, 23, false);
	NEW_CLASS(this->txtX, UI::GUITextBox(ui, this, CSTR("")));
	this->txtX->SetRect(54, 4, 100, 23, false);
	this->txtX->SetReadOnly(true);
	NEW_CLASS(this->lblY, UI::GUILabel(ui, this, (const UTF8Char*)"Y"));
	this->lblY->SetRect(4, 28, 50, 23, false);
	NEW_CLASS(this->txtY, UI::GUITextBox(ui, this, CSTR("")));
	this->txtY->SetRect(54, 28, 100, 23, false);
	this->txtY->SetReadOnly(true);
	NEW_CLASS(this->lblZ, UI::GUILabel(ui, this, (const UTF8Char*)"Z"));
	this->lblZ->SetRect(4, 52, 50, 23, false);
	NEW_CLASS(this->txtZ, UI::GUITextBox(ui, this, CSTR("")));
	this->txtZ->SetRect(54, 52, 100, 23, false);
	this->txtZ->SetReadOnly(true);
	NEW_CLASS(this->pnlMoving, UI::GUIPanel(ui, this));
	this->pnlMoving->SetRect(0, 0, 100, 48, false);
	this->pnlMoving->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pnlMoving->SetBGColor(0xff40ff40);
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRAccelerometerForm::~AVIRAccelerometerForm()
{
	DEL_CLASS(this->motion);
}

void SSWR::AVIRead::AVIRAccelerometerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
