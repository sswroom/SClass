#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRIBuddyForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnHeadChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIBuddyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIBuddyForm>();
	NN<IO::Device::IBuddy> currBuddy;
	if (me->currBuddy.SetTo(currBuddy))
	{
		me->currHead = (IO::Device::IBuddy::IBuddyHeadEffect)(OSInt)me->lbHead->GetSelectedItem().p;
		currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnHeartChanged(AnyType userObj, Bool newState)
{
	NN<SSWR::AVIRead::AVIRIBuddyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIBuddyForm>();
	NN<IO::Device::IBuddy> currBuddy;
	if (me->currBuddy.SetTo(currBuddy))
	{
		if (newState)
		{
			me->currHeart = IO::Device::IBuddy::IBHRE_LIGHT;
		}
		else
		{
			me->currHeart = IO::Device::IBuddy::IBHRE_OFF;
		}
		currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnTurnLeftDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRIBuddyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIBuddyForm>();
	NN<IO::Device::IBuddy> currBuddy;
	if (me->currBuddy.SetTo(currBuddy))
	{
		if (isDown)
		{
			me->currTurn = IO::Device::IBuddy::IBBE_TURN_LEFT;
		}
		else
		{
			me->currTurn = IO::Device::IBuddy::IBBE_OFF;
		}
		currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnTurnRightDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRIBuddyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIBuddyForm>();
	NN<IO::Device::IBuddy> currBuddy;
	if (me->currBuddy.SetTo(currBuddy))
	{
		if (isDown)
		{
			me->currTurn = IO::Device::IBuddy::IBBE_TURN_RIGHT;
		}
		else
		{
			me->currTurn = IO::Device::IBuddy::IBBE_OFF;
		}
		currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnWingUpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRIBuddyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIBuddyForm>();
	NN<IO::Device::IBuddy> currBuddy;
	if (me->currBuddy.SetTo(currBuddy))
	{
		if (isDown)
		{
			me->currWing = IO::Device::IBuddy::IBWE_WING_UP;
		}
		else
		{
			me->currWing = IO::Device::IBuddy::IBWE_OFF;
		}
		currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnWingDownDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRIBuddyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIBuddyForm>();
	NN<IO::Device::IBuddy> currBuddy;
	if (me->currBuddy.SetTo(currBuddy))
	{
		if (isDown)
		{
			me->currWing = IO::Device::IBuddy::IBWE_WING_DOWN;
		}
		else
		{
			me->currWing = IO::Device::IBuddy::IBWE_OFF;
		}
		currBuddy->PlayEffect(me->currTurn, me->currHead, me->currHeart, me->currWing);
	}
}

void __stdcall SSWR::AVIRead::AVIRIBuddyForm::OnDevChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIBuddyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIBuddyForm>();
	NN<IO::Device::IBuddy> currBuddy;
	if (me->currBuddy.SetTo(currBuddy))
	{
		currBuddy->PlayEffect(IO::Device::IBuddy::IBBE_OFF, IO::Device::IBuddy::IBHDE_OFF, IO::Device::IBuddy::IBHRE_OFF, IO::Device::IBuddy::IBWE_OFF);
	}
	me->currBuddy = 0;
	me->lbHead->SetSelectedIndex(0);
	me->chkHeart->SetChecked(false);
	me->currHead = IO::Device::IBuddy::IBHDE_OFF;
	me->currHeart = IO::Device::IBuddy::IBHRE_OFF;
	me->currTurn = IO::Device::IBuddy::IBBE_OFF;
	me->currWing = IO::Device::IBuddy::IBWE_OFF;
	me->currBuddy = me->lbDevice->GetSelectedItem().GetOpt<IO::Device::IBuddy>();
}

SSWR::AVIRead::AVIRIBuddyForm::AVIRIBuddyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("i-Buddy"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->currBuddy = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbDevice = ui->NewListBox(*this, false);
	this->lbDevice->SetRect(0, 0, 100, 23, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDevChanged, this);
	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblHead = ui->NewLabel(this->pnlMain, CSTR("Head"));
	this->lblHead->SetRect(4, 4, 100, 23, false);
	this->lbHead = ui->NewListBox(this->pnlMain, false);
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
	this->lblHeart = ui->NewLabel(this->pnlMain, CSTR("Heart"));
	this->lblHeart->SetRect(4, 124, 100, 23, false);
	this->chkHeart = ui->NewCheckBox(this->pnlMain, CSTR("Turn On"), false);
	this->chkHeart->SetRect(104, 124, 200, 23, false);
	this->chkHeart->HandleCheckedChange(OnHeartChanged, this);
	this->lblTurn = ui->NewLabel(this->pnlMain, CSTR("Body Turn"));
	this->lblTurn->SetRect(4, 148, 100, 23, false);
	this->btnTurnLeft = ui->NewButton(this->pnlMain, CSTR("Left"));
	this->btnTurnLeft->SetRect(104, 148, 75, 23, false);
	this->btnTurnLeft->HandleButtonUpDown(OnTurnLeftDown, this);
	this->btnTurnRight = ui->NewButton(this->pnlMain, CSTR("Right"));
	this->btnTurnRight->SetRect(184, 148, 75, 23, false);
	this->btnTurnRight->HandleButtonUpDown(OnTurnRightDown, this);
	this->lblWing = ui->NewLabel(this->pnlMain, CSTR("Wing"));
	this->lblWing->SetRect(4, 172, 100, 23, false);
	this->btnWingUp = ui->NewButton(this->pnlMain, CSTR("Up"));
	this->btnWingUp->SetRect(104, 172, 75, 23, false);
	this->btnWingUp->HandleButtonUpDown(OnWingUpDown, this);
	this->btnWingDown = ui->NewButton(this->pnlMain, CSTR("Down"));
	this->btnWingDown->SetRect(184, 172, 75, 23, false);
	this->btnWingDown->HandleButtonUpDown(OnWingDownDown, this);

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k = 0;
	NN<IO::Device::IBuddy> buddy;
	j = IO::Device::IBuddy::GetNumDevice();
	i = 0;
	while (i < j)
	{
		NEW_CLASSNN(buddy, IO::Device::IBuddy(i));
		if (buddy->IsError())
		{
			buddy.Delete();
		}
		else
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Device ")), k);
			k++;
			this->buddyList.Add(buddy);
			this->lbDevice->AddItem(CSTRP(sbuff, sptr), buddy);
		}
		i++;
	}
}

SSWR::AVIRead::AVIRIBuddyForm::~AVIRIBuddyForm()
{
	this->buddyList.DeleteAll();
}

void SSWR::AVIRead::AVIRIBuddyForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
