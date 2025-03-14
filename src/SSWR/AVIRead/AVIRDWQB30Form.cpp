#include "Stdafx.h"
#include "IO/Device/DensoWaveQB30.h"
#include "IO/Device/DensoWaveQK30U.h"
#include "SSWR/AVIRead/AVIRDWQB30Form.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnPortClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		scanner.Delete();
		me->scanner = 0;
		me->txtPort->SetText(CSTR(""));
		me->btnPort->SetText(CSTR("Open"));
		me->txtMode->SetText(CSTR(""));
	}
	else
	{
		SSWR::AVIRead::AVIRSelStreamForm frm(0, me->ui, me->core, false, 0, me->core->GetLog());
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			NN<IO::Stream> stm = frm.GetStream();
			UOSInt i = me->cboDevType->GetSelectedIndex();
			if (i == 0)
			{
				NEW_CLASSNN(scanner, IO::Device::DensoWaveQB30(stm));
			}
			else
			{
				NEW_CLASSNN(scanner, IO::Device::DensoWaveQK30U(stm));
			}
			me->scanner = scanner;
			scanner->HandleCodeScanned(OnCodeScanned, me);

			me->txtPort->SetText(stm->GetSourceNameObj()->ToCString());
			me->btnPort->SetText(CSTR("Close"));
			me->txtMode->SetText(CSTR("Idle"));
			me->tpSetting->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnModeScanClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		scanner->SetCurrMode(IO::CodeScanner::MT_SCAN);
		me->txtMode->SetText(CSTR("Scan"));
		me->tpSetting->SetEnabled(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnModeSettingClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		scanner->SetCurrMode(IO::CodeScanner::MT_SETTING);
		me->txtMode->SetText(CSTR("Setting"));
		me->tpSetting->SetEnabled(true);

		Data::ArrayList<IO::CodeScanner::DeviceCommand> cmdList;
		UOSInt i;
		UOSInt j;
		scanner->GetCommandList(&cmdList);
		me->cboSetCmd->ClearItems();
		i = 0;
		j = cmdList.GetCount();
		while (i < j)
		{
			me->cboSetCmd->AddItem(scanner->GetCommandName(cmdList.GetItem(i)).OrEmpty(), (void*)(OSInt)cmdList.GetItem(i));
			i++;
		}
		if (j > 0)
		{
			me->cboSetCmd->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnModeIdleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		scanner->SetCurrMode(IO::CodeScanner::MT_IDLE);
		me->txtMode->SetText(CSTR("Idle"));
		me->tpSetting->SetEnabled(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnResetClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		if (scanner->SoftReset())
		{
			me->txtMode->SetText(CSTR("Idle"));
			me->tpSetting->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnDefaultClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		if (scanner->ResetDefault())
		{
			me->txtMode->SetText(CSTR("Idle"));
			me->tpSetting->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnSetCmdSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		UOSInt i = me->cboSetCmd->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			IO::CodeScanner::DeviceCommand dcmd = (IO::CodeScanner::DeviceCommand)me->cboSetCmd->GetItem(i).GetOSInt();
			me->cmdCurr = dcmd;
			me->cmdType = scanner->GetCommandParamType(dcmd, &me->cmdMin, &me->cmdMax);
			if (me->cmdType == IO::CodeScanner::CT_GET_COMMAND)
			{
				me->cboSetParam->SetEnabled(false);
			}
			else if (me->cmdType == IO::CodeScanner::CT_GET_COMMAND_NAME)
			{
				me->cboSetParam->SetEnabled(false);
			}
			else if (me->cmdType == IO::CodeScanner::CT_SELECT_COMMAND)
			{
				Int32 i;
				me->cboSetParam->SetEnabled(true);
				me->cboSetParam->ClearItems();
				i = me->cmdMin;
				while (i <= me->cmdMax)
				{
					me->cboSetParam->AddItem(scanner->GetCommandParamName(dcmd, i).OrEmpty(), (void*)(OSInt)i); 
					i++;
				}
			}
			else if (me->cmdType == IO::CodeScanner::CT_SET_COMMAND)
			{
				me->cboSetParam->SetEnabled(true);
				me->cboSetParam->ClearItems();
				me->cboSetParam->SetText(CSTR(""));
			}
			else
			{
			}
		}
		else
		{
			me->cmdType = IO::CodeScanner::CT_UNKNOWN;
			me->cboSetParam->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnSetCmdClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	NN<IO::CodeScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		UTF8Char sbuff[12];
		UnsafeArray<UTF8Char> sptr;
		if (me->cmdType == IO::CodeScanner::CT_GET_COMMAND)
		{
			Int32 val = scanner->GetCommand(me->cmdCurr);
			if (val == -1)
			{
				me->txtSetCmd->SetText(CSTR("Failed"));
			}
			else
			{
				sptr = Text::StrInt32(sbuff, val);
				me->txtSetCmd->SetText(CSTRP(sbuff, sptr));
			}
		}
		else if (me->cmdType == IO::CodeScanner::CT_GET_COMMAND_NAME)
		{
			Int32 val = scanner->GetCommand(me->cmdCurr);
			if (val == -1)
			{
				me->txtSetCmd->SetText(CSTR("Failed"));
			}
			else
			{
				me->txtSetCmd->SetText(scanner->GetCommandParamName(me->cmdCurr, val).OrEmpty());
			}
		}
		else if (me->cmdType == IO::CodeScanner::CT_SELECT_COMMAND)
		{
			Int32 val;
			UOSInt i = me->cboSetParam->GetSelectedIndex();
			if (i != INVALID_INDEX)
			{
				val = (Int32)me->cboSetParam->GetItem(i).GetOSInt();
				if (scanner->SetCommand(me->cmdCurr, val))
				{
					me->txtSetCmd->SetText(CSTR("Success"));
				}
				else
				{
					me->txtSetCmd->SetText(CSTR("Failed"));
				}
			}
			else
			{
				me->txtSetCmd->SetText(CSTR("No selection"));
			}
		}
		else if (me->cmdType == IO::CodeScanner::CT_SET_COMMAND)
		{
			Text::StringBuilderUTF8 sb;
			Int32 val;
			me->cboSetParam->GetText(sb);
			if (sb.ToInt32(val))
			{
				if (val >= me->cmdMin && val <= me->cmdMax)
				{
					if (scanner->SetCommand(me->cmdCurr, val))
					{
						me->txtSetCmd->SetText(CSTR("Success"));
					}
					else
					{
						me->txtSetCmd->SetText(CSTR("Failed"));
					}
				}
				else
				{
					me->txtSetCmd->SetText(CSTR("Out of range"));
				}
			}
			else
			{
				me->txtSetCmd->SetText(CSTR("Non integer"));
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnCodeScanned(AnyType userObj, Text::CStringNN code)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	Sync::MutexUsage mutUsage(me->codeMut);
	OPTSTR_DEL(me->newCode);
	me->newCode = Text::String::New(code);
	me->codeUpdate = true;
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDWQB30Form> me = userObj.GetNN<SSWR::AVIRead::AVIRDWQB30Form>();
	if (me->codeUpdate)
	{
		me->codeUpdate = false;
		NN<Text::String> s;
		Sync::MutexUsage mutUsage(me->codeMut);
		if (me->newCode.SetTo(s))
		{
			me->txtScan->SetText(s->ToCString());
			me->lbScan->AddItem(s, 0);
			s->Release();
			me->newCode = 0;
		}
	}
}

SSWR::AVIRead::AVIRDWQB30Form::AVIRDWQB30Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 360, ui)
{
	this->SetText(CSTR("Denso Wave QB-30"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->scanner = 0;
	this->newCode = 0;
	this->codeUpdate = false;
	this->cmdCurr = IO::CodeScanner::DC_GET_READ_MODE;
	this->cmdType = IO::CodeScanner::CT_UNKNOWN;
	this->cmdMin = 0;
	this->cmdMax = 0;

	this->grpConn = ui->NewGroupBox(*this, CSTR("Connect"));
	this->grpConn->SetRect(0, 0, 100, 68, false);
	this->grpConn->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDevType = ui->NewLabel(this->grpConn, CSTR("Type"));
	this->lblDevType->SetRect(160, 0, 100, 23, false);
	this->cboDevType = ui->NewComboBox(this->grpConn, false);
	this->cboDevType->SetRect(260, 0, 150, 23, false);
	this->cboDevType->AddItem(CSTR("Denso Wave QB30"), 0);
	this->cboDevType->AddItem(CSTR("Denso Wave QK30-U"), 0);
	this->cboDevType->SetSelectedIndex(0);
	this->lblPort = ui->NewLabel(this->grpConn, CSTR("Port"));
	this->lblPort->SetRect(160, 24, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->grpConn, CSTR(""), false);
	this->txtPort->SetRect(260, 24, 100, 23, false);
	this->txtPort->SetReadOnly(true);
	this->btnPort = ui->NewButton(this->grpConn, CSTR("Open"));
	this->btnPort->SetRect(360, 24, 75, 23, false);
	this->btnPort->HandleButtonClick(OnPortClicked, this);
	this->grpCtrl = ui->NewGroupBox(*this, CSTR("Control"));
	this->grpCtrl->SetRect(0, 0, 100, 68, false);
	this->grpCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblMode = ui->NewLabel(this->grpCtrl, CSTR("Mode"));
	this->lblMode->SetRect(160, 0, 100, 23, false);
	this->txtMode = ui->NewTextBox(this->grpCtrl, CSTR(""));
	this->txtMode->SetRect(260, 0, 100, 23, false);
	this->txtMode->SetReadOnly(true);
	this->btnModeScan = ui->NewButton(this->grpCtrl, CSTR("Scan Mode"));
	this->btnModeScan->SetRect(360, 0, 75, 23, false);
	this->btnModeScan->HandleButtonClick(OnModeScanClicked, this);
	this->btnModeSetting = ui->NewButton(this->grpCtrl, CSTR("Setting Mode"));
	this->btnModeSetting->SetRect(440, 0, 75, 23, false);
	this->btnModeSetting->HandleButtonClick(OnModeSettingClicked, this);
	this->btnModeIdle = ui->NewButton(this->grpCtrl, CSTR("Idle Mode"));
	this->btnModeIdle->SetRect(520, 0, 75, 23, false);
	this->btnModeIdle->HandleButtonClick(OnModeIdleClicked, this);
	this->btnReset = ui->NewButton(this->grpCtrl, CSTR("Reset"));
	this->btnReset->SetRect(360, 24, 75, 23, false);
	this->btnReset->HandleButtonClick(OnResetClicked, this);
	this->btnDefault = ui->NewButton(this->grpCtrl, CSTR("Factory Default"));
	this->btnDefault->SetRect(440, 24, 75, 23, false);
	this->btnDefault->HandleButtonClick(OnDefaultClicked, this);	

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpScan = this->tcMain->AddTabPage(CSTR("Scan"));
	this->pnlScan = ui->NewPanel(this->tpScan);
	this->pnlScan->SetRect(0, 0, 100, 32, false);
	this->pnlScan->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblScan = ui->NewLabel(this->pnlScan, CSTR("Last Result"));
	this->lblScan->SetRect(4, 4, 100, 23, false);
	this->txtScan = ui->NewTextBox(this->pnlScan, CSTR(""), false);
	this->txtScan->SetReadOnly(true);
	this->txtScan->SetRect(104, 4, 400, 23, false);
	this->lbScan = ui->NewListBox(this->tpScan, false);
	this->lbScan->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSetting = this->tcMain->AddTabPage(CSTR("Setting"));
	this->lblSetCmd = ui->NewLabel(this->tpSetting, CSTR("Command"));
	this->lblSetCmd->SetRect(4, 4, 100, 23, false);
	this->cboSetCmd = ui->NewComboBox(this->tpSetting, false);
	this->cboSetCmd->SetRect(104, 4, 150, 23, false);
	this->cboSetCmd->HandleSelectionChange(OnSetCmdSelChg, this);
	this->cboSetParam = ui->NewComboBox(this->tpSetting, true);
	this->cboSetParam->SetRect(264, 4, 150, 23, false);
	this->btnSetCmd = ui->NewButton(this->tpSetting, CSTR("Send"));
	this->btnSetCmd->SetRect(414, 4, 75, 23, false);
	this->btnSetCmd->HandleButtonClick(OnSetCmdClicked, this);
	this->txtSetCmd = ui->NewTextBox(this->tpSetting, CSTR(""), false);
	this->txtSetCmd->SetRect(494, 4, 100, 23, false);
	this->txtSetCmd->SetReadOnly(true);

	this->tpSetting->SetEnabled(false);

	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRDWQB30Form::~AVIRDWQB30Form()
{
	this->scanner.Delete();
	OPTSTR_DEL(this->newCode);
}

void SSWR::AVIRead::AVIRDWQB30Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
