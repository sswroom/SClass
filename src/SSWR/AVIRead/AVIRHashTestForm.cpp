#include "Stdafx.h"
#include "Crypto/Hash/HashCreator.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRHashTestForm.h"
#include "Text/MyStringFloat.h"

Double __stdcall SSWR::AVIRead::AVIRHashTestForm::HashTestSpeed(NN<Crypto::Hash::HashAlgorithm> hash)
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

void __stdcall SSWR::AVIRead::AVIRHashTestForm::OnCompareClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHashTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHashTestForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Crypto::Hash::HashType i = Crypto::Hash::HashType::First;
	UOSInt j;
	Double speed;
	NN<Crypto::Hash::HashAlgorithm> hash;
	me->lvCompare->ClearItems();
	while (i <= Crypto::Hash::HashType::Last)
	{
		if (Crypto::Hash::HashCreator::CreateHash(i).SetTo(hash))
		{
			sbuff[0] = 0;
			speed = HashTestSpeed(hash);
			sptr = hash->GetName(sbuff);
			j = me->lvCompare->AddItem(CSTRP(sbuff, sptr), 0);
			sptr = Text::StrDouble(sbuff, speed);
			me->lvCompare->SetSubItem(j, 1, CSTRP(sbuff, sptr));
			hash.Delete();
		}
		i = (Crypto::Hash::HashType)((OSInt)i + 1);
	}
}

void __stdcall SSWR::AVIRead::AVIRHashTestForm::OnSpeedClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHashTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHashTestForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = me->cboAlgorithm->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		NN<Crypto::Hash::HashAlgorithm> hash;
		if (Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)me->cboAlgorithm->GetItem(i).GetOSInt()).SetTo(hash))
		{
			sptr = Text::StrDouble(sbuff, HashTestSpeed(hash));
			me->txtSpeed->SetText(CSTRP(sbuff, sptr));
			hash.Delete();
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

SSWR::AVIRead::AVIRHashTestForm::AVIRHashTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Hash Test"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpAlgorithm = this->tcMain->AddTabPage(CSTR("Algorithm"));
	this->lblAlgorithm = ui->NewLabel(this->tpAlgorithm, CSTR("Function"));
	this->lblAlgorithm->SetRect(4, 4, 100, 23, false);
	this->cboAlgorithm = ui->NewComboBox(this->tpAlgorithm, false);
	this->cboAlgorithm->SetRect(104, 4, 200, 23, false);
	this->btnSpeed = ui->NewButton(this->tpAlgorithm, CSTR("Test Speed"));
	this->btnSpeed->SetRect(304, 4, 75, 23, false);
	this->btnSpeed->HandleButtonClick(OnSpeedClicked, this);
	this->lblSpeed = ui->NewLabel(this->tpAlgorithm, CSTR("Speed"));
	this->lblSpeed->SetRect(4, 28, 100, 23, false);
	this->txtSpeed = ui->NewTextBox(this->tpAlgorithm, CSTR(""));
	this->txtSpeed->SetRect(104, 28, 200, 23, false);
	this->txtSpeed->SetReadOnly(true);

	this->tpCompare = this->tcMain->AddTabPage(CSTR("Compare"));
	this->pnlCompare = ui->NewPanel(this->tpCompare);
	this->pnlCompare->SetRect(0, 0, 100, 31, false);
	this->pnlCompare->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnCompare = ui->NewButton(this->pnlCompare, CSTR("Compare"));
	this->btnCompare->SetRect(4, 4, 75, 23, false);
	this->btnCompare->HandleButtonClick(OnCompareClicked, this);
	this->lvCompare = ui->NewListView(this->tpCompare, UI::ListViewStyle::Table, 2);
	this->lvCompare->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCompare->AddColumn(CSTR("Hash Function"), 200);
	this->lvCompare->AddColumn(CSTR("Speed (Byte/s)"), 200);
	this->lvCompare->SetShowGrid(true);
	this->lvCompare->SetFullRowSelect(true);

	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Crypto::Hash::HashType i = Crypto::Hash::HashType::First;
	NN<Crypto::Hash::HashAlgorithm> hash;
	while (i <= Crypto::Hash::HashType::Last)
	{
		if (Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)i).SetTo(hash))
		{
			sptr = hash->GetName(sbuff);
			this->cboAlgorithm->AddItem(CSTRP(sbuff, sptr), (void*)i);
			hash.Delete();
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
