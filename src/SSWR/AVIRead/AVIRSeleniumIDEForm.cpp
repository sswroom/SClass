#include "Stdafx.h"
#include "IO/SeleniumIDERunner.h"
#include "SSWR/AVIRead/AVIRSeleniumIDEForm.h"

void __stdcall SSWR::AVIRead::AVIRSeleniumIDEForm::OnCommandSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSeleniumIDEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSeleniumIDEForm>();
	NN<IO::SeleniumCommand> command;
	if (me->lvCommand->GetSelectedItem().GetOpt<IO::SeleniumCommand>().SetTo(command))
	{
		me->txtCommand->SetText(Text::String::OrEmpty(command->GetCommand())->ToCString());
		me->txtCommandComment->SetText(Text::String::OrEmpty(command->GetComment())->ToCString());
		me->txtCommandTarget->SetText(Text::String::OrEmpty(command->GetTarget())->ToCString());
		me->txtCommandValue->SetText(Text::String::OrEmpty(command->GetValue())->ToCString());
	}
	else
	{
		me->txtCommand->SetText(CSTR(""));
		me->txtCommandComment->SetText(CSTR(""));
		me->txtCommandTarget->SetText(CSTR(""));
		me->txtCommandValue->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRSeleniumIDEForm::OnTestSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSeleniumIDEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSeleniumIDEForm>();
	me->DisplayTest();
}

void __stdcall SSWR::AVIRead::AVIRSeleniumIDEForm::OnTestRunClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSeleniumIDEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSeleniumIDEForm>();
	NN<IO::SeleniumTest> test;
	if (me->lbTest->GetSelectedItem().GetOpt<IO::SeleniumTest>().SetTo(test))
	{
		IO::SeleniumIDERunner runner;
		if (runner.Run(test))
		{
			me->ui->ShowMsgOK(CSTR("Test Run successfully"), CSTR("Selenium IDE"), me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Test Run failed"), CSTR("Selenium IDE"), me);
		}
	}
}

void SSWR::AVIRead::AVIRSeleniumIDEForm::DisplayTest()
{
	NN<IO::SeleniumTest> test;
	this->lvCommand->ClearItems();
	if (this->lbTest->GetSelectedItem().GetOpt<IO::SeleniumTest>().SetTo(test))
	{
		NN<IO::SeleniumCommand> command;
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j;
		while (test->GetCommand(i).SetTo(command))
		{
			j = this->lvCommand->AddItem(Text::String::OrEmpty(command->GetCommand()), command);
			if (command->GetTarget().SetTo(s)) this->lvCommand->SetSubItem(j, 1, s);
			if (command->GetValue().SetTo(s)) this->lvCommand->SetSubItem(j, 2, s);
			i++;
		}
	}
}

SSWR::AVIRead::AVIRSeleniumIDEForm::AVIRSeleniumIDEForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::SeleniumIDE> side) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("Selenium IDE"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->side = side;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlSIDE = ui->NewPanel(*this);
	this->pnlSIDE->SetRect(0, 0, 100, 71, false);
	this->pnlSIDE->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSIDEName = ui->NewLabel(this->pnlSIDE, CSTR("Name"));
	this->lblSIDEName->SetRect(0, 0, 100, 23, false);
	this->txtSIDEName = ui->NewTextBox(this->pnlSIDE, Text::String::OrEmpty(side->GetName())->ToCString());
	this->txtSIDEName->SetRect(100, 0, 150, 23, false);
	this->txtSIDEName->SetReadOnly(true);
	this->lblSIDEVersion = ui->NewLabel(this->pnlSIDE, CSTR("Version"));
	this->lblSIDEVersion->SetRect(0, 24, 100, 23, false);
	this->txtSIDEVersion = ui->NewTextBox(this->pnlSIDE, Text::String::OrEmpty(side->GetVersion())->ToCString());
	this->txtSIDEVersion->SetRect(100, 24, 150, 23, false);
	this->txtSIDEVersion->SetReadOnly(true);
	this->lblSIDEURL = ui->NewLabel(this->pnlSIDE, CSTR("URL"));
	this->lblSIDEURL->SetRect(0, 48, 100, 23, false);
	this->txtSIDEURL = ui->NewTextBox(this->pnlSIDE, Text::String::OrEmpty(side->GetURL())->ToCString());
	this->txtSIDEURL->SetRect(100, 48, 500, 23, false);
	this->txtSIDEURL->SetReadOnly(true);
	this->lbTest = ui->NewListBox(*this, false);
	this->lbTest->SetRect(0, 0, 150, 23, false);
	this->lbTest->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->pnlTestCtrl = ui->NewPanel(*this);
	this->pnlTestCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlTestCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnTestRun = ui->NewButton(this->pnlTestCtrl, CSTR("Run Test"));
	this->btnTestRun->SetRect(4, 4, 75, 23, false);
	this->btnTestRun->HandleButtonClick(OnTestRunClicked, this);
	this->pnlCommand = ui->NewPanel(*this);
	this->pnlCommand->SetRect(0, 0, 100, 103, false);
	this->pnlCommand->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvCommand = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvCommand->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCommand->SetShowGrid(true);
	this->lvCommand->SetFullRowSelect(true);
	this->lvCommand->AddColumn(CSTR("Command"), 120);
	this->lvCommand->AddColumn(CSTR("Target"), 300);
	this->lvCommand->AddColumn(CSTR("Value"), 120);
	this->lvCommand->HandleSelChg(OnCommandSelChg, this);
	this->lblCommand = ui->NewLabel(this->pnlCommand, CSTR("Command"));
	this->lblCommand->SetRect(4, 4, 100, 23, false);
	this->txtCommand = ui->NewTextBox(this->pnlCommand, CSTR(""));
	this->txtCommand->SetRect(104, 4, 400, 23, false);
	this->txtCommand->SetReadOnly(true);
	this->lblCommandTarget = ui->NewLabel(this->pnlCommand, CSTR("Target"));
	this->lblCommandTarget->SetRect(4, 28, 100, 23, false);
	this->txtCommandTarget = ui->NewTextBox(this->pnlCommand, CSTR(""));
	this->txtCommandTarget->SetRect(104, 28, 400, 23, false);
	this->txtCommandTarget->SetReadOnly(true);
	this->lblCommandValue = ui->NewLabel(this->pnlCommand, CSTR("Value"));
	this->lblCommandValue->SetRect(4, 52, 100, 23, false);
	this->txtCommandValue = ui->NewTextBox(this->pnlCommand, CSTR(""));
	this->txtCommandValue->SetRect(104, 52, 400, 23, false);
	this->txtCommandValue->SetReadOnly(true);
	this->lblCommandComment = ui->NewLabel(this->pnlCommand, CSTR("Comment"));
	this->lblCommandComment->SetRect(4, 76, 100, 23, false);
	this->txtCommandComment = ui->NewTextBox(this->pnlCommand, CSTR(""));
	this->txtCommandComment->SetRect(104, 76, 400, 23, false);
	this->txtCommandComment->SetReadOnly(true);

	NN<IO::SeleniumTest> test;
	UOSInt i = 0;
	while (side->GetTest(i).SetTo(test))
	{
		this->lbTest->AddItem(Text::String::OrEmpty(test->GetName()), test);
		i++;
	}
	if (this->lbTest->GetCount() > 0)
	{
		this->lbTest->SetSelectedIndex(0);
		this->DisplayTest();
	}
}

SSWR::AVIRead::AVIRSeleniumIDEForm::~AVIRSeleniumIDEForm()
{
	this->side.Delete();
}

void SSWR::AVIRead::AVIRSeleniumIDEForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
