#include "Stdafx.h"
#include "Math/Math_C.h"
#include "SSWR/AVIRead/AVIRMap3DForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRMap3DForm::OnMapUpdated(AnyType userObj, Math::Coord2DDbl center)
{
	NN<SSWR::AVIRead::AVIRMap3DForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMap3DForm>();
	me->UpdateCenterText(center);
}

void __stdcall SSWR::AVIRead::AVIRMap3DForm::OnVAngleScrolled(AnyType userObj, UIntOS newVal)
{
	NN<SSWR::AVIRead::AVIRMap3DForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMap3DForm>();
	me->mapCtrl->SetVAngle((Double)newVal * Math::PI / 180.0);
}

void __stdcall SSWR::AVIRead::AVIRMap3DForm::OnHAngleScrolled(AnyType userObj, UIntOS newVal)
{
	NN<SSWR::AVIRead::AVIRMap3DForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMap3DForm>();
	me->mapCtrl->SetHAngle((Double)newVal * Math::PI / 180.0);
}

void SSWR::AVIRead::AVIRMap3DForm::UpdateCenterText(Math::Coord2DDbl center)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr = Text::StrDouble(sbuff, center.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
	sptr = Text::StrDouble(sptr, center.y);
	this->txtCenter->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRMap3DForm::AVIRMap3DForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Bool releaseEnv) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->env = env;
	this->releaseEnv = releaseEnv;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());

	this->SetText(CSTR("Map 3D View"));
	this->SetFont(nullptr, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlCtrl->SetRect(0, 0, 100, 28, false);

	this->txtCenter = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtCenter->SetRect(4, 2, 300, 23, false);
	this->txtCenter->SetReadOnly(true);

	this->lblVAngle = ui->NewLabel(this->pnlCtrl, CSTR("VAngle"));
	this->lblVAngle->SetRect(320, 5, 50, 19, false);
	this->tbVAngle = ui->NewTrackBar(this->pnlCtrl, 3, 86, 45);
	this->tbVAngle->SetRect(372, 2, 120, 23, false);
	this->tbVAngle->HandleScrolled(OnVAngleScrolled, this);

	this->lblHAngle = ui->NewLabel(this->pnlCtrl, CSTR("HAngle"));
	this->lblHAngle->SetRect(500, 5, 50, 19, false);
	this->tbHAngle = ui->NewTrackBar(this->pnlCtrl, 0, 360, 0);
	this->tbHAngle->SetRect(552, 2, 120, 23, false);
	this->tbHAngle->HandleScrolled(OnHAngleScrolled, this);

	NEW_CLASSNN(this->mapCtrl, UI::GUIMap3DControl(ui, *this, this->core->GetDrawEngine(), this->env, this->colorSess));
	this->mapCtrl->SetDockType(UI::GUIControl::DOCK_FILL);
	this->mapCtrl->HandleMapUpdated(OnMapUpdated, this);
	this->UpdateCenterText(this->mapCtrl->GetMapCenter());
}

SSWR::AVIRead::AVIRMap3DForm::~AVIRMap3DForm()
{
	if (this->releaseEnv)
	{
		this->env.Delete();
	}
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRMap3DForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
