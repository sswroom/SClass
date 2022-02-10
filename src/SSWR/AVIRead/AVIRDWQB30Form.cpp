#include "Stdafx.h"
#include "IO/Device/DensoWaveQB30.h"
#include "IO/Device/DensoWaveQK30U.h"
#include "SSWR/AVIRead/AVIRDWQB30Form.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnPortClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		SDEL_CLASS(me->scanner);
		me->txtPort->SetText((const UTF8Char*)"");
		me->btnPort->SetText((const UTF8Char*)"Open");
		me->txtMode->SetText((const UTF8Char*)"");
	}
	else
	{
		IO::Stream *stm = 0;
		SSWR::AVIRead::AVIRSelStreamForm *frm;
		NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, me->ui, me->core, false));
		if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			stm = frm->stm;
			UOSInt i = me->cboDevType->GetSelectedIndex();
			if (i == 0)
			{
				NEW_CLASS(me->scanner, IO::Device::DensoWaveQB30(stm));
			}
			else
			{
				NEW_CLASS(me->scanner, IO::Device::DensoWaveQK30U(stm));
			}
			me->scanner->HandleCodeScanned(OnCodeScanned, me);
		}
		DEL_CLASS(frm);

		if (me->scanner)
		{
			me->txtPort->SetText(stm->GetSourceNameObj()->v);
			me->btnPort->SetText((const UTF8Char*)"Close");
			me->txtMode->SetText((const UTF8Char*)"Idle");
			me->tpSetting->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnModeScanClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		me->scanner->SetCurrMode(IO::CodeScanner::MT_SCAN);
		me->txtMode->SetText((const UTF8Char*)"Scan");
		me->tpSetting->SetEnabled(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnModeSettingClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		me->scanner->SetCurrMode(IO::CodeScanner::MT_SETTING);
		me->txtMode->SetText((const UTF8Char*)"Setting");
		me->tpSetting->SetEnabled(true);

		Data::ArrayList<IO::CodeScanner::DeviceCommand> cmdList;
		UOSInt i;
		UOSInt j;
		me->scanner->GetCommandList(&cmdList);
		me->cboSetCmd->ClearItems();
		i = 0;
		j = cmdList.GetCount();
		while (i < j)
		{
			me->cboSetCmd->AddItem(me->scanner->GetCommandName(cmdList.GetItem(i)), (void*)(OSInt)cmdList.GetItem(i));
			i++;
		}
		if (j > 0)
		{
			me->cboSetCmd->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnModeIdleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		me->scanner->SetCurrMode(IO::CodeScanner::MT_IDLE);
		me->txtMode->SetText((const UTF8Char*)"Idle");
		me->tpSetting->SetEnabled(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnResetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		if (me->scanner->SoftReset())
		{
			me->txtMode->SetText((const UTF8Char*)"Idle");
			me->tpSetting->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnDefaultClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		if (me->scanner->ResetDefault())
		{
			me->txtMode->SetText((const UTF8Char*)"Idle");
			me->tpSetting->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnSetCmdSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		UOSInt i = me->cboSetCmd->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			IO::CodeScanner::DeviceCommand dcmd = (IO::CodeScanner::DeviceCommand)(OSInt)me->cboSetCmd->GetItem(i);
			me->cmdCurr = dcmd;
			me->cmdType = me->scanner->GetCommandParamType(dcmd, &me->cmdMin, &me->cmdMax);
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
					me->cboSetParam->AddItem(me->scanner->GetCommandParamName(dcmd, i), (void*)(OSInt)i); 
					i++;
				}
			}
			else if (me->cmdType == IO::CodeScanner::CT_SET_COMMAND)
			{
				me->cboSetParam->SetEnabled(true);
				me->cboSetParam->ClearItems();
				me->cboSetParam->SetText((const UTF8Char*)"");
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

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnSetCmdClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->scanner)
	{
		UTF8Char sbuff[12];
		if (me->cmdType == IO::CodeScanner::CT_GET_COMMAND)
		{
			Int32 val = me->scanner->GetCommand(me->cmdCurr);
			if (val == -1)
			{
				me->txtSetCmd->SetText((const UTF8Char*)"Failed");
			}
			else
			{
				Text::StrInt32(sbuff, val);
				me->txtSetCmd->SetText(sbuff);
			}
		}
		else if (me->cmdType == IO::CodeScanner::CT_GET_COMMAND_NAME)
		{
			Int32 val = me->scanner->GetCommand(me->cmdCurr);
			if (val == -1)
			{
				me->txtSetCmd->SetText((const UTF8Char*)"Failed");
			}
			else
			{
				me->txtSetCmd->SetText(me->scanner->GetCommandParamName(me->cmdCurr, val).v);
			}
		}
		else if (me->cmdType == IO::CodeScanner::CT_SELECT_COMMAND)
		{
			Int32 val;
			UOSInt i = me->cboSetParam->GetSelectedIndex();
			if (i != INVALID_INDEX)
			{
				val = (Int32)(OSInt)me->cboSetParam->GetItem(i);
				if (me->scanner->SetCommand(me->cmdCurr, val))
				{
					me->txtSetCmd->SetText((const UTF8Char*)"Success");
				}
				else
				{
					me->txtSetCmd->SetText((const UTF8Char*)"Failed");
				}
			}
			else
			{
				me->txtSetCmd->SetText((const UTF8Char*)"No selection");
			}
		}
		else if (me->cmdType == IO::CodeScanner::CT_SET_COMMAND)
		{
			Text::StringBuilderUTF8 sb;
			Int32 val;
			me->cboSetParam->GetText(&sb);
			if (sb.ToInt32(&val))
			{
				if (val >= me->cmdMin && val <= me->cmdMax)
				{
					if (me->scanner->SetCommand(me->cmdCurr, val))
					{
						me->txtSetCmd->SetText((const UTF8Char*)"Success");
					}
					else
					{
						me->txtSetCmd->SetText((const UTF8Char*)"Failed");
					}
				}
				else
				{
					me->txtSetCmd->SetText((const UTF8Char*)"Out of range");
				}
			}
			else
			{
				me->txtSetCmd->SetText((const UTF8Char*)"Non integer");
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnCodeScanned(void *userObj, const UTF8Char *code)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	Sync::MutexUsage mutUsage(me->codeMut);
	SDEL_STRING(me->newCode);
	me->newCode = Text::String::NewNotNull(code);
	me->codeUpdate = true;
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRDWQB30Form::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRDWQB30Form *me = (SSWR::AVIRead::AVIRDWQB30Form*)userObj;
	if (me->codeUpdate)
	{
		me->codeUpdate = false;
		Sync::MutexUsage mutUsage(me->codeMut);
		if (me->newCode)
		{
			me->txtScan->SetText(me->newCode->v);
			me->lbScan->AddItem(me->newCode, 0);
			me->newCode->Release();
			me->newCode = 0;
		}
		mutUsage.EndUse();
	}
}

SSWR::AVIRead::AVIRDWQB30Form::AVIRDWQB30Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 360, ui)
{
	this->SetText((const UTF8Char*)"Denso Wave QB-30");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->scanner = 0;
	NEW_CLASS(this->codeMut, Sync::Mutex());
	this->newCode = 0;
	this->codeUpdate = false;
	this->cmdCurr = IO::CodeScanner::DC_GET_READ_MODE;
	this->cmdType = IO::CodeScanner::CT_UNKNOWN;
	this->cmdMin = 0;
	this->cmdMax = 0;

	NEW_CLASS(this->grpConn, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Connect"));
	this->grpConn->SetRect(0, 0, 100, 68, false);
	this->grpConn->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDevType, UI::GUILabel(ui, this->grpConn, (const UTF8Char*)"Type"));
	this->lblDevType->SetRect(160, 0, 100, 23, false);
	NEW_CLASS(this->cboDevType, UI::GUIComboBox(ui, this->grpConn, false));
	this->cboDevType->SetRect(260, 0, 150, 23, false);
	this->cboDevType->AddItem(CSTR("Denso Wave QB30"), 0);
	this->cboDevType->AddItem(CSTR("Denso Wave QK30-U"), 0);
	this->cboDevType->SetSelectedIndex(0);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpConn, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(160, 24, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpConn, CSTR(""), false));
	this->txtPort->SetRect(260, 24, 100, 23, false);
	this->txtPort->SetReadOnly(true);
	NEW_CLASS(this->btnPort, UI::GUIButton(ui, this->grpConn, (const UTF8Char*)"Open"));
	this->btnPort->SetRect(360, 24, 75, 23, false);
	this->btnPort->HandleButtonClick(OnPortClicked, this);
	NEW_CLASS(this->grpCtrl, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Control"));
	this->grpCtrl->SetRect(0, 0, 100, 68, false);
	this->grpCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblMode, UI::GUILabel(ui, this->grpCtrl, (const UTF8Char*)"Mode"));
	this->lblMode->SetRect(160, 0, 100, 23, false);
	NEW_CLASS(this->txtMode, UI::GUITextBox(ui, this->grpCtrl, CSTR("")));
	this->txtMode->SetRect(260, 0, 100, 23, false);
	this->txtMode->SetReadOnly(true);
	NEW_CLASS(this->btnModeScan, UI::GUIButton(ui, this->grpCtrl, (const UTF8Char*)"Scan Mode"));
	this->btnModeScan->SetRect(360, 0, 75, 23, false);
	this->btnModeScan->HandleButtonClick(OnModeScanClicked, this);
	NEW_CLASS(this->btnModeSetting, UI::GUIButton(ui, this->grpCtrl, (const UTF8Char*)"Setting Mode"));
	this->btnModeSetting->SetRect(440, 0, 75, 23, false);
	this->btnModeSetting->HandleButtonClick(OnModeSettingClicked, this);
	NEW_CLASS(this->btnModeIdle, UI::GUIButton(ui, this->grpCtrl, (const UTF8Char*)"Idle Mode"));
	this->btnModeIdle->SetRect(520, 0, 75, 23, false);
	this->btnModeIdle->HandleButtonClick(OnModeIdleClicked, this);
	NEW_CLASS(this->btnReset, UI::GUIButton(ui, this->grpCtrl, (const UTF8Char*)"Reset"));
	this->btnReset->SetRect(360, 24, 75, 23, false);
	this->btnReset->HandleButtonClick(OnResetClicked, this);
	NEW_CLASS(this->btnDefault, UI::GUIButton(ui, this->grpCtrl, (const UTF8Char*)"Factory Default"));
	this->btnDefault->SetRect(440, 24, 75, 23, false);
	this->btnDefault->HandleButtonClick(OnDefaultClicked, this);	

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpScan = this->tcMain->AddTabPage(CSTR("Scan"));
	NEW_CLASS(this->pnlScan, UI::GUIPanel(ui, this->tpScan));
	this->pnlScan->SetRect(0, 0, 100, 32, false);
	this->pnlScan->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblScan, UI::GUILabel(ui, this->pnlScan, (const UTF8Char*)"Last Result"));
	this->lblScan->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtScan, UI::GUITextBox(ui, this->pnlScan, CSTR(""), false));
	this->txtScan->SetReadOnly(true);
	this->txtScan->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lbScan, UI::GUIListBox(ui, this->tpScan, false));
	this->lbScan->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSetting = this->tcMain->AddTabPage(CSTR("Setting"));
	NEW_CLASS(this->lblSetCmd, UI::GUILabel(ui, this->tpSetting, (const UTF8Char*)"Command"));
	this->lblSetCmd->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboSetCmd, UI::GUIComboBox(ui, this->tpSetting, false));
	this->cboSetCmd->SetRect(104, 4, 150, 23, false);
	this->cboSetCmd->HandleSelectionChange(OnSetCmdSelChg, this);
	NEW_CLASS(this->cboSetParam, UI::GUIComboBox(ui, this->tpSetting, true));
	this->cboSetParam->SetRect(264, 4, 150, 23, false);
	NEW_CLASS(this->btnSetCmd, UI::GUIButton(ui, this->tpSetting, (const UTF8Char*)"Send"));
	this->btnSetCmd->SetRect(414, 4, 75, 23, false);
	this->btnSetCmd->HandleButtonClick(OnSetCmdClicked, this);
	NEW_CLASS(this->txtSetCmd, UI::GUITextBox(ui, this->tpSetting, CSTR(""), false));
	this->txtSetCmd->SetRect(494, 4, 100, 23, false);
	this->txtSetCmd->SetReadOnly(true);

	this->tpSetting->SetEnabled(false);

	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRDWQB30Form::~AVIRDWQB30Form()
{
	SDEL_CLASS(this->scanner);
	SDEL_STRING(this->newCode);
	DEL_CLASS(this->codeMut);
}

void SSWR::AVIRead::AVIRDWQB30Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
