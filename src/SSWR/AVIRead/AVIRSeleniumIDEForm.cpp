#include "Stdafx.h"
#include "IO/SeleniumIDERunner.h"
#include "SSWR/AVIRead/AVIRSeleniumIDEForm.h"
#include "UI/GUIComboBoxUtil.h"

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
	Text::StringBuilderUTF8 sb;
	UInt16 port;
	if (me->lbTest->GetSelectedItem().GetOpt<IO::SeleniumTest>().SetTo(test))
	{
		me->txtTestPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Selenium IDE"), me);
			me->txtTestPort->Focus();
			return ;
		}
		me->statusList.MemFreeAll();
		Text::CString mobile = 0;
		IO::SeleniumIDERunner::RunOptions options;
		IO::SeleniumIDERunner runner(me->core->GetTCPClientFactory(), port);
		if (me->chkTestUserDataDir->IsChecked())
		{
			sb.ClearStr();
			me->txtTestUserDataDir->GetText(sb);
			runner.SetUserDataDir(sb.ToCString());
		}
		if (me->chkTestMobile->IsChecked())
		{
			sb.ClearStr();
			me->cboTestMobile->GetText(sb);
			mobile = sb.ToCString();
		}
		if (me->chkTestNoPause->IsChecked())
		{
			runner.SetNoPause(true);
		}
		options.headless = me->chkTestHeadless->IsChecked();
		options.disableGPU = me->chkTestDisableGPU->IsChecked();
		options.noSandbox = me->chkTestNoSandbox->IsChecked();
		NN<Net::WebDriverSession> sess;
		if (runner.BeginTest((IO::SeleniumIDERunner::BrowserType)me->cboTestBrowser->GetSelectedItem().GetOSInt(), mobile, 0, Text::String::OrEmpty(me->side->GetURL())->ToCString(), options).SetTo(sess))
		{
			if (runner.RunTest(sess, test, Text::String::OrEmpty(me->side->GetURL())->ToCString(), OnStepStatus, me))
			{
				me->DisplayStatus();
				me->ui->ShowMsgOK(CSTR("Test Run successfully"), CSTR("Selenium IDE"), me);
			}
			else
			{
				me->DisplayStatus();
				Text::StringBuilderUTF8 sb;
				sb.Append(CSTR("Test Run failed: Error Index = "));
				sb.AppendOSInt((OSInt)runner.GetLastErrorIndex());
				sb.Append(CSTR("\r\n"));
				sb.AppendOpt(runner.GetLastErrorMsg());
				me->ui->ShowMsgOK(sb.ToCString(), CSTR("Selenium IDE"), me);
			}
			sess.Delete();
		}
		else
		{
			me->DisplayStatus();
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Failed in creating session\r\n"));
			sb.AppendOpt(runner.GetLastErrorMsg());
			me->ui->ShowMsgOK(sb.ToCString(), CSTR("Selenium IDE"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSeleniumIDEForm::OnStepStatus(AnyType userObj, UOSInt index, Data::Duration dur)
{
	NN<SSWR::AVIRead::AVIRSeleniumIDEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSeleniumIDEForm>();
	NN<RunStepStatus> status = MemAllocNN(RunStepStatus);
	status->ts = Data::Timestamp::Now();
	status->index = index;
	status->dur = dur;
	me->statusList.Add(status);
}

void SSWR::AVIRead::AVIRSeleniumIDEForm::DisplayTest()
{
	NN<IO::SeleniumTest> test;
	this->lvCommand->ClearItems();
	if (this->lbTest->GetSelectedItem().GetOpt<IO::SeleniumTest>().SetTo(test))
	{
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		NN<IO::SeleniumCommand> command;
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j;
		while (test->GetCommand(i).SetTo(command))
		{
			sptr = Text::StrUOSInt(sbuff, i);
			j = this->lvCommand->AddItem(CSTRP(sbuff, sptr), command);
			if (command->GetCommand().SetTo(s)) this->lvCommand->SetSubItem(j, 1, s);
			if (command->GetTarget().SetTo(s)) this->lvCommand->SetSubItem(j, 2, s);
			if (command->GetValue().SetTo(s)) this->lvCommand->SetSubItem(j, 3, s);
			i++;
		}
	}
}

void SSWR::AVIRead::AVIRSeleniumIDEForm::DisplayStatus()
{
	this->lvRunLog->ClearItems();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<RunStepStatus> status;
	UOSInt i = 0;
	UOSInt j = this->statusList.GetCount();
	while (i < j)
	{
		status = this->statusList.GetItemNoCheck(i);
		sptr = status->ts.ToStringNoZone(sbuff);
		this->lvRunLog->AddItem(CSTRP(sbuff, sptr), 0);
		sptr = Text::StrUOSInt(sbuff, status->index);
		this->lvRunLog->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, status->dur.GetTotalSec());
		this->lvRunLog->SetSubItem(i, 2, CSTRP(sbuff, sptr));
		i++;
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
	this->pnlTestCtrl->SetRect(0, 0, 100, 151, false);
	this->pnlTestCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblTestPort = ui->NewLabel(this->pnlTestCtrl, CSTR("Port"));
	this->lblTestPort->SetRect(4, 4, 100, 23, false);
	this->txtTestPort = ui->NewTextBox(this->pnlTestCtrl, CSTR("4444"));
	this->txtTestPort->SetRect(104, 4, 60, 23, false);
	this->lblTestBrowser = ui->NewLabel(this->pnlTestCtrl, CSTR("Browser"));
	this->lblTestBrowser->SetRect(4, 28, 100, 23, false);
	this->cboTestBrowser = ui->NewComboBox(this->pnlTestCtrl, false);
	this->cboTestBrowser->SetRect(104, 28, 150, 23, false);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, Chrome);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, MSEdge);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, Firefox);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, HtmlUnit);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, InternetExplorer);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, IPad);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, IPhone);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, Opera);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, Safari);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, WebKitGTK);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, Mock);
	CBOADDENUM(this->cboTestBrowser, IO::SeleniumIDERunner::BrowserType, PhantomJS);
	this->cboTestBrowser->SetSelectedIndex(0);
	this->chkTestMobile = ui->NewCheckBox(this->pnlTestCtrl, CSTR("Mobile"), false);
	this->chkTestMobile->SetRect(4, 52, 100, 23, false);
	this->cboTestMobile = ui->NewComboBox(this->pnlTestCtrl, true);
	this->cboTestMobile->SetRect(104, 52, 150, 23, false);
	IO::SeleniumIDERunner::FillMobileItemSelector(this->cboTestMobile);
	this->cboTestMobile->SetText(CSTR("iPhone 14 Pro Max"));
	this->btnTestRun = ui->NewButton(this->pnlTestCtrl, CSTR("Run Test"));
	this->btnTestRun->SetRect(254, 52, 75, 23, false);
	this->btnTestRun->HandleButtonClick(OnTestRunClicked, this);
	this->chkTestUserDataDir = ui->NewCheckBox(this->pnlTestCtrl, CSTR("UserDataDir"), false);
	this->chkTestUserDataDir->SetRect(4, 76, 100, 23, false);
	this->txtTestUserDataDir = ui->NewTextBox(this->pnlTestCtrl, CSTR(""));
	this->txtTestUserDataDir->SetRect(104, 76, 300, 23, false);
	this->chkTestHeadless = ui->NewCheckBox(this->pnlTestCtrl, CSTR("Headless"), false);
	this->chkTestHeadless->SetRect(4, 100, 120, 23, false);
	this->chkTestDisableGPU = ui->NewCheckBox(this->pnlTestCtrl, CSTR("Disable GPU"), false);
	this->chkTestDisableGPU->SetRect(124, 100, 120, 23, false);
	this->chkTestNoSandbox = ui->NewCheckBox(this->pnlTestCtrl, CSTR("No Sandbox"), false);
	this->chkTestNoSandbox->SetRect(244, 100, 120, 23, false);
	this->chkTestNoPause = ui->NewCheckBox(this->pnlTestCtrl, CSTR("No Pause"), false);
	this->chkTestNoPause->SetRect(4, 124, 120, 23, false);
	this->tcTest = ui->NewTabControl(*this);
	this->tcTest->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpCommand = this->tcTest->AddTabPage(CSTR("Commands"));
	this->pnlCommand = ui->NewPanel(this->tpCommand);
	this->pnlCommand->SetRect(0, 0, 100, 103, false);
	this->pnlCommand->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvCommand = ui->NewListView(this->tpCommand, UI::ListViewStyle::Table, 4);
	this->lvCommand->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCommand->SetShowGrid(true);
	this->lvCommand->SetFullRowSelect(true);
	this->lvCommand->AddColumn(CSTR("Index"), 30);
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
	this->tpRunLog = this->tcTest->AddTabPage(CSTR("Run Log"));
	this->lvRunLog = ui->NewListView(this->tpRunLog, UI::ListViewStyle::Table, 3);
	this->lvRunLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRunLog->SetShowGrid(true);
	this->lvRunLog->SetFullRowSelect(true);
	this->lvRunLog->AddColumn(CSTR("Time"), 150);
	this->lvRunLog->AddColumn(CSTR("Index"), 120);
	this->lvRunLog->AddColumn(CSTR("Duration"), 100);

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
	this->statusList.MemFreeAll();
}

void SSWR::AVIRead::AVIRSeleniumIDEForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
