#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRIBuddyForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnHeadChanged(void *userObj)
{
	SSWR::AVIRead::AVIRIBuddyForm *me = (SSWR::AVIRead::AVIRIBuddyForm*)userObj;
	if (me->currBuddy)
	{
		me->currHead = (IO::Device::IBuddy::IBuddyHeadEffect)(OSInt)me->lbHead->GetSelectedItem();
		me->currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnHeartChanged(void *userObj, Bool newState)
{
	SSWR::AVIRead::AVIRIBuddyForm *me = (SSWR::AVIRead::AVIRIBuddyForm*)userObj;
	if (me->currBuddy)
	{
		if (newState)
		{
			me->currHeart = IO::Device::IBuddy::IBHRE_LIGHT;
		}
		else
		{
			me->currHeart = IO::Device::IBuddy::IBHRE_OFF;
		}
		me->currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnTurnLeftDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRIBuddyForm *me = (SSWR::AVIRead::AVIRIBuddyForm*)userObj;
	if (me->currBuddy)
	{
		if (isDown)
		{
			me->currTurn = IO::Device::IBuddy::IBBE_TURN_LEFT;
		}
		else
		{
			me->currTurn = IO::Device::IBuddy::IBBE_OFF;
		}
		me->currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnTurnRightDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRIBuddyForm *me = (SSWR::AVIRead::AVIRIBuddyForm*)userObj;
	if (me->currBuddy)
	{
		if (isDown)
		{
			me->currTurn = IO::Device::IBuddy::IBBE_TURN_RIGHT;
		}
		else
		{
			me->currTurn = IO::Device::IBuddy::IBBE_OFF;
		}
		me->currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnWingUpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRIBuddyForm *me = (SSWR::AVIRead::AVIRIBuddyForm*)userObj;
	if (me->currBuddy)
	{
		if (isDown)
		{
			me->currWing = IO::Device::IBuddy::IBWE_WING_UP;
		}
		else
		{
			me->currWing = IO::Device::IBuddy::IBWE_OFF;
		}
		me->currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnWingDownDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRIBuddyForm *me = (SSWR::AVIRead::AVIRIBuddyForm*)userObj;
	if (me->currBuddy)
	{
		if (isDown)
		{
			me->currWing = IO::Device::IBuddy::IBWE_WING_DOWN;
		}
		else
		{
			me->currWing = IO::Device::IBuddy::IBWE_OFF;
		}
		me->currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnDevChanged(void *userObj)
{
	SSWR::AVIRead::AVIRIBuddyForm *me = (SSWR::AVIRead::AVIRIBuddyForm*)userObj;
	if (me->currBuddy)
	{
		me->currBuddy->PlayEffect(IO::Device::IBuddy::IBBE_OFF, IO::Device::IBuddy::IBHDE_OFF, IO::Device::IBuddy::IBHRE_OFF, IO::Device::IBuddy::IBWE_OFF);
	}
	me->currBuddy = 0;
	me->lbHead->SetSelectedIndex(0);
	me->chkHeart->SetChecked(false);
	me->currHead = IO::Device::IBuddy::IBHDE_OFF;
	me->currHeart = IO::Device::IBuddy::IBHRE_OFF;
	me->currTurn = IO::Device::IBuddy::IBBE_OFF;
	me->currWing = IO::Device::IBuddy::IBWE_OFF;
	me->currBuddy = (IO::Device::IBuddy*)me->lbDevice->GetSelectedItem();
}

SSWR::AVIRead::AVIRIBuddyForm::AVIRIBuddyForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("i-Buddy"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	NEW_CLASS(this->buddyList, Data::ArrayList<IO::Device::IBuddy*>());
	this->currBuddy = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, this, false));
	this->lbDevice->SetRect(0, 0, 100, 23, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDevChanged, this);
	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblHead, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Head"));
	this->lblHead->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->lbHead, UI::GUIListBox(ui, this->pnlMain, false));
	this->lbHead->SetRect(104, 4, 200, 120, false);
	this->lbHead->HandleSelectionChange(OnHeadChanged, this);
	this->lbHead->AddItem(CSTR("Off"), (void*)IO::Device::IBuddy::IBHDE_OFF);
	this->lbHead->AddItem(CSTR("Red"), (void*)IO::Device::IBuddy::IBHDE_LIGHT_RED);
	this->lbHead->AddItem(CSTR("Green"), (void*)IO::Device::IBuddy::IBHDE_LIGHT_GREEN);
	this->lbHead->AddItem(CSTR("Blue"), (void*)IO::Device::IBuddy::IBHDE_LIGHT_BLUE);
	this->lbHead->AddItem(CSTR("Yellow"), (void*)IO::Device::IBuddy::IBHDE_LIGHT_YELLOW);
	this->lbHead->AddItem(CSTR("Magenta"), (void*)IO::Device::IBuddy::IBHDE_LIGHT_MAGENTA);
	this->lbHead->AddItem(CSTR("Cyan"), (void*)IO::Device::IBuddy::IBHDE_LIGHT_CYAN);
	this->lbHead->AddItem(CSTR("White"), (void*)IO::Device::IBuddy::IBHDE_LIGHT_WHITE);
	NEW_CLASS(this->lblHeart, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Heart"));
	this->lblHeart->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->chkHeart, UI::GUICheckBox(ui, this->pnlMain, (const UTF8Char*)"Turn On", false));
	this->chkHeart->SetRect(104, 124, 200, 23, false);
	this->chkHeart->HandleCheckedChange(OnHeartChanged, this);
	NEW_CLASS(this->lblTurn, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Body Turn"));
	this->lblTurn->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->btnTurnLeft, UI::GUIButton(ui, this->pnlMain, CSTR("Left")));
	this->btnTurnLeft->SetRect(104, 148, 75, 23, false);
	this->btnTurnLeft->HandleButtonUpDown(OnTurnLeftDown, this);
	NEW_CLASS(this->btnTurnRight, UI::GUIButton(ui, this->pnlMain, CSTR("Right")));
	this->btnTurnRight->SetRect(184, 148, 75, 23, false);
	this->btnTurnRight->HandleButtonUpDown(OnTurnRightDown, this);
	NEW_CLASS(this->lblWing, UI::GUILabel(ui, this->pnlMain, (const UTF8Char*)"Wing"));
	this->lblWing->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->btnWingUp, UI::GUIButton(ui, this->pnlMain, CSTR("Up")));
	this->btnWingUp->SetRect(104, 172, 75, 23, false);
	this->btnWingUp->HandleButtonUpDown(OnWingUpDown, this);
	NEW_CLASS(this->btnWingDown, UI::GUIButton(ui, this->pnlMain, CSTR("Down")));
	this->btnWingDown->SetRect(184, 172, 75, 23, false);
	this->btnWingDown->HandleButtonUpDown(OnWingDownDown, this);

	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k = 0;
	IO::Device::IBuddy *buddy;
	j = IO::Device::IBuddy::GetNumDevice();
	i = 0;
	while (i < j)
	{
		NEW_CLASS(buddy, IO::Device::IBuddy(i));
		if (buddy->IsError())
		{
			DEL_CLASS(buddy);
		}
		else
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Device ")), k);
			k++;
			this->buddyList->Add(buddy);
			this->lbDevice->AddItem(CSTRP(sbuff, sptr), buddy);
		}
		i++;
	}
}

SSWR::AVIRead::AVIRIBuddyForm::~AVIRIBuddyForm()
{
	UOSInt i = this->buddyList->GetCount();
	IO::Device::IBuddy *buddy;
	while (i-- > 0)
	{
		buddy = this->buddyList->GetItem(i);
		DEL_CLASS(buddy);
	}
	DEL_CLASS(this->buddyList);
}

void SSWR::AVIRead::AVIRIBuddyForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
