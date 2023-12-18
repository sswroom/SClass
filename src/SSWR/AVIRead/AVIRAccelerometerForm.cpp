#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRAccelerometerForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRAccelerometerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRAccelerometerForm *me = (SSWR::AVIRead::AVIRAccelerometerForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Math::Vector3 acc;
	if (me->motion->UpdateStatus())
	{
		acc = me->motion->GetValues();

		sptr = Text::StrDouble(sbuff, acc.val[0]);
		me->txtX->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, acc.val[1]);
		me->txtY->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, acc.val[2]);
		me->txtZ->SetText(CSTRP(sbuff, sptr));

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
		me->txtX->SetText(CSTR("-"));
		me->txtY->SetText(CSTR("-"));
		me->txtZ->SetText(CSTR("-"));
	}
}

SSWR::AVIRead::AVIRAccelerometerForm::AVIRAccelerometerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SensorAccelerometer *acc) : UI::GUIForm(parent, 240, 160, ui)
{
	this->SetText(CSTR("Accelerometer"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	NEW_CLASS(this->motion, IO::MotionDetectorAccelerometer(acc, true));
	this->currMoving = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblX = ui->NewLabel(*this, CSTR("X"));
	this->lblX->SetRect(4, 4, 50, 23, false);
	NEW_CLASS(this->txtX, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtX->SetRect(54, 4, 100, 23, false);
	this->txtX->SetReadOnly(true);
	this->lblY = ui->NewLabel(*this, CSTR("Y"));
	this->lblY->SetRect(4, 28, 50, 23, false);
	NEW_CLASS(this->txtY, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtY->SetRect(54, 28, 100, 23, false);
	this->txtY->SetReadOnly(true);
	this->lblZ = ui->NewLabel(*this, CSTR("Z"));
	this->lblZ->SetRect(4, 52, 50, 23, false);
	NEW_CLASS(this->txtZ, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtZ->SetRect(54, 52, 100, 23, false);
	this->txtZ->SetReadOnly(true);
	NEW_CLASSNN(this->pnlMoving, UI::GUIPanel(ui, *this));
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
