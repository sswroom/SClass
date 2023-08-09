#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRPaintCntForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRPaintCntForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPaintCntForm *me = (SSWR::AVIRead::AVIRPaintCntForm*)userObj;
	if (me->dispCnt != me->paintCnt)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		me->dispCnt = me->paintCnt;
		sptr = Text::StrInt64(sbuff, me->dispCnt);
		me->txtCnt->SetText(CSTRP(sbuff, sptr));
	}
}

SSWR::AVIRead::AVIRPaintCntForm::AVIRPaintCntForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Paint Counter"));
	
	this->core = core;
	this->paintCnt = 0;
	this->dispCnt = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblCnt, UI::GUILabel(ui, this, CSTR("Paint Cnt")));
	this->lblCnt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCnt, UI::GUITextBox(ui, this, CSTR("0")));
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
