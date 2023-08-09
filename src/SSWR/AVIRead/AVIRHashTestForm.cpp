#include "Stdafx.h"
#include "Crypto/Hash/HashCreator.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRHashTestForm.h"
#include "Text/MyStringFloat.h"

Double __stdcall SSWR::AVIRead::AVIRHashTestForm::HashTestSpeed(Crypto::Hash::IHash *hash)
{
	Manage::HiResClock clk;
	UInt8 hashVal[64];
	UInt8 *testBlock = MemAllocA(UInt8, 1048576);
	OSInt i;
	clk.Start();
	i = 1024;
	while (i-- > 0)
	{
		hash->Calc(testBlock, 1048576);
	}
	hash->GetValue(hashVal);
	Double t = 1024.0 * 1048576.0 / clk.GetTimeDiff();
	MemFreeA(testBlock);
	return t;
}

void __stdcall SSWR::AVIRead::AVIRHashTestForm::OnCompareClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHashTestForm *me = (SSWR::AVIRead::AVIRHashTestForm*)userObj;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Crypto::Hash::HashType i = Crypto::Hash::HashType::First;
	UOSInt j;
	Double speed;
	Crypto::Hash::IHash *hash;
	me->lvCompare->ClearItems();
	while (i <= Crypto::Hash::HashType::Last)
	{
		hash = Crypto::Hash::HashCreator::CreateHash(i);
		if (hash)
		{
			sbuff[0] = 0;
			speed = HashTestSpeed(hash);
			sptr = hash->GetName(sbuff);
			j = me->lvCompare->AddItem(CSTRP(sbuff, sptr), 0);
			sptr = Text::StrDouble(sbuff, speed);
			me->lvCompare->SetSubItem(j, 1, CSTRP(sbuff, sptr));
			DEL_CLASS(hash);
		}
		i = (Crypto::Hash::HashType)((OSInt)i + 1);
	}
}

void __stdcall SSWR::AVIRead::AVIRHashTestForm::OnSpeedClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHashTestForm *me = (SSWR::AVIRead::AVIRHashTestForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i = me->cboAlgorithm->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		Crypto::Hash::IHash *hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)(OSInt)me->cboAlgorithm->GetItem(i));
		if (hash)
		{
			sptr = Text::StrDouble(sbuff, HashTestSpeed(hash));
			me->txtSpeed->SetText(CSTRP(sbuff, sptr));
			DEL_CLASS(hash);
		}
		else
		{
			me->txtSpeed->SetText(CSTR("-"));
		}
	}
	else
	{
		me->txtSpeed->SetText(CSTR("-"));
	}
}

SSWR::AVIRead::AVIRHashTestForm::AVIRHashTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Hash Test"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpAlgorithm = this->tcMain->AddTabPage(CSTR("Algorithm"));
	NEW_CLASS(this->lblAlgorithm, UI::GUILabel(ui, this->tpAlgorithm, CSTR("Function")));
	this->lblAlgorithm->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboAlgorithm, UI::GUIComboBox(ui, this->tpAlgorithm, false));
	this->cboAlgorithm->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->btnSpeed, UI::GUIButton(ui, this->tpAlgorithm, CSTR("Test Speed")));
	this->btnSpeed->SetRect(304, 4, 75, 23, false);
	this->btnSpeed->HandleButtonClick(OnSpeedClicked, this);
	NEW_CLASS(this->lblSpeed, UI::GUILabel(ui, this->tpAlgorithm, CSTR("Speed")));
	this->lblSpeed->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSpeed, UI::GUITextBox(ui, this->tpAlgorithm, CSTR("")));
	this->txtSpeed->SetRect(104, 28, 200, 23, false);
	this->txtSpeed->SetReadOnly(true);

	this->tpCompare = this->tcMain->AddTabPage(CSTR("Compare"));
	NEW_CLASS(this->pnlCompare, UI::GUIPanel(ui, this->tpCompare));
	this->pnlCompare->SetRect(0, 0, 100, 31, false);
	this->pnlCompare->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnCompare, UI::GUIButton(ui, this->pnlCompare, CSTR("Compare")));
	this->btnCompare->SetRect(4, 4, 75, 23, false);
	this->btnCompare->HandleButtonClick(OnCompareClicked, this);
	NEW_CLASS(this->lvCompare, UI::GUIListView(ui, this->tpCompare, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvCompare->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCompare->AddColumn(CSTR("Hash Function"), 200);
	this->lvCompare->AddColumn(CSTR("Speed (Byte/s)"), 200);
	this->lvCompare->SetShowGrid(true);
	this->lvCompare->SetFullRowSelect(true);

	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Crypto::Hash::HashType i = Crypto::Hash::HashType::First;
	Crypto::Hash::IHash *hash;
	while (i <= Crypto::Hash::HashType::Last)
	{
		hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)i);
		if (hash)
		{
			sptr = hash->GetName(sbuff);
			this->cboAlgorithm->AddItem(CSTRP(sbuff, sptr), (void*)i);
			DEL_CLASS(hash);
		}
		i = (Crypto::Hash::HashType)((OSInt)i + 1);
	}
}

SSWR::AVIRead::AVIRHashTestForm::~AVIRHashTestForm()
{
}

void SSWR::AVIRead::AVIRHashTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
