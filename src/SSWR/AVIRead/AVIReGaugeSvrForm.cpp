#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIReGaugeSvrForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	if (me->svr)
	{
		SDEL_CLASS(me->svr);
		if (me->dirHdlr)
		{
			me->dirHdlr->Release();
			me->dirHdlr = 0;
		}
		SDEL_CLASS(me->log);
		SDEL_CLASS(me->logger);
		me->txtPort->SetReadOnly(false);
	}
	else
	{
		UInt16 port = 0;
		Bool valid = true;
		Text::StringBuilderUTF8 *sb;
		NEW_CLASS(sb, Text::StringBuilderUTF8());
		me->txtPort->GetText(sb);
		Text::StrToUInt16S(sb->ToString(), &port, 0);

		if (port > 0 && port <= 65535)
		{
			NEW_CLASS(me->dirHdlr, Net::WebServer::EGaugeHandler());
			me->dirHdlr->HandleEGaugeData(OnEGaugeData, me);
			NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), 0, me->dirHdlr, port, 120, Sync::Thread::GetThreadCnt(), (const UTF8Char*)"eGauge/1.0", false, false));
			if (me->svr->IsError())
			{
				valid = false;
				SDEL_CLASS(me->svr);
			}
			else
			{
				NEW_CLASS(me->log, IO::LogTool());
				me->svr->SetAccessLog(me->log, IO::ILogHandler::LOG_LEVEL_RAW);
				NEW_CLASS(me->logger, UI::ListBoxLogger(me, me->lbLog, 500, true));
				me->log->AddLogHandler(me->logger, IO::ILogHandler::LOG_LEVEL_RAW);
			}
		}

		if (valid)
		{
			me->txtPort->SetReadOnly(true);
		}
		else
		{
			SDEL_CLASS(me->svr);
			if (me->dirHdlr)
			{
				me->dirHdlr->Release();
				me->dirHdlr = 0;
			}
			SDEL_CLASS(me->log);
			SDEL_CLASS(me->logger);
		}
		DEL_CLASS(sb);
	}
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnLogSel(void *userObj)
{
	const UTF8Char *t;
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	t = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(t);
	me->lbLog->DelTextNew(t);
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	if (me->reqUpdated)
	{
		Sync::MutexUsage mutUsage(me->reqMut);
		me->reqUpdated = false;
		me->txtReqText->SetText(me->reqLast);
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnEGaugeData(void *userObj, const UInt8 *data, UOSInt dataSize)
{
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	Sync::MutexUsage mutUsage(me->reqMut);
	if (me->reqLast)
	{
		MemFree(me->reqLast);
	}
	me->reqLast = MemAlloc(UTF8Char, dataSize + 1);
	MemCopyNO(me->reqLast, data, dataSize);
	me->reqLast[dataSize] = 0;
	me->reqUpdated = true;
	mutUsage.EndUse();
}

SSWR::AVIRead::AVIReGaugeSvrForm::AVIReGaugeSvrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"eGauge Server");
	this->SetFont(0, 8.25, false);
	this->svr = 0;
	this->log = 0;
	this->dirHdlr = 0;
	this->logger = 0;
	NEW_CLASS(this->reqMut, Sync::Mutex());
	this->reqLast = 0;
	this->reqUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)"12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(158, 8, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);

	this->tpReqText = this->tcMain->AddTabPage((const UTF8Char*)"ReqText");
	NEW_CLASS(this->txtReqText, UI::GUITextBox(ui, this->tpReqText, (const UTF8Char*)"", true));
	this->txtReqText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtReqText->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIReGaugeSvrForm::~AVIReGaugeSvrForm()
{
	SDEL_CLASS(this->svr);
	if (this->dirHdlr)
	{
		this->dirHdlr->Release();
		this->dirHdlr = 0;
	}
	if (this->reqLast)
	{
		MemFree(this->reqLast);
		this->reqLast = 0;
	}
	DEL_CLASS(this->reqMut);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIReGaugeSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
