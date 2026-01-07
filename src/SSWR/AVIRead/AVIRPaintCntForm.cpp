#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRPaintCntForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRPaintCntForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPaintCntForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPaintCntForm>();
	if (me->dispCnt != me->paintCnt)
	{
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		me->dispCnt = me->paintCnt;
		sptr = Text::StrInt64(sbuff, me->dispCnt);
		me->txtCnt->SetText(CSTRP(sbuff, sptr));
	}
}

SSWR::AVIRead::AVIRPaintCntForm::AVIRPaintCntForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Paint Counter"));
	
	this->core = core;
	this->paintCnt = 0;
	this->dispCnt = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblCnt = ui->NewLabel(*this, CSTR("Paint Cnt"));
	this->lblCnt->SetRect(4, 4, 100, 23, false);
	this->txtCnt = ui->NewTextBox(*this, CSTR("0"));
	this->txtCnt->SetReadOnly(true);
	this->txtCnt->SetRect(104, 4, 150, 23, false);

	this->tmrMain = this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRPaintCntForm::~AVIRPaintCntForm()
{
	this->RemoveTimer(this->tmrMain);
}

Bool SSWR::AVIRead::AVIRPaintCntForm::OnPaint()
{
	this->paintCnt++;
	return false;
}

void SSWR::AVIRead::AVIRPaintCntForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
