#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIReGaugeSvrForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	if (me->svr)
	{
		SDEL_CLASS(me->svr);
		SDEL_CLASS(me->dirHdlr);
		SDEL_CLASS(me->log);
		SDEL_CLASS(me->logger);
		me->txtPort->SetReadOnly(false);
	}
	else
	{
		UInt16 port = 0;
		Bool valid = true;
		Text::StringBuilderUTF8 sb;
		me->txtPort->GetText(sb);
		Text::StrToUInt16S(sb.ToString(), port, 0);

		if (port > 0 && port <= 65535)
		{
			NotNullPtr<Net::WebServer::EGaugeHandler> dirHdlr;
			NEW_CLASSNN(dirHdlr, Net::WebServer::EGaugeHandler());
			dirHdlr->HandleEGaugeData(OnEGaugeData, me);
			NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), 0, dirHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("eGauge/1.0"), false, Net::WebServer::KeepAlive::Default, false));
			if (me->svr->IsError())
			{
				valid = false;
				SDEL_CLASS(me->svr);
				dirHdlr.Delete();
			}
			else
			{
				me->dirHdlr = dirHdlr.Ptr();
				NEW_CLASS(me->log, IO::LogTool());
				me->svr->SetAccessLog(me->log, IO::LogHandler::LogLevel::Raw);
				NotNullPtr<UI::ListBoxLogger> logger;
				NEW_CLASSNN(logger, UI::ListBoxLogger(*me, me->lbLog, 500, true));
				me->logger = logger.Ptr();
				me->log->AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
				if (!me->svr->Start())
				{
					valid = false;
				}
			}
		}

		if (valid)
		{
			me->txtPort->SetReadOnly(true);
		}
		else
		{
			SDEL_CLASS(me->svr);
			SDEL_CLASS(me->dirHdlr);
			SDEL_CLASS(me->log);
			SDEL_CLASS(me->logger);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnLogSel(void *userObj)
{
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	Optional<Text::String> s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	if (me->reqUpdated)
	{
		Sync::MutexUsage mutUsage(me->reqMut);
		me->reqUpdated = false;
		me->txtReqText->SetText(me->reqLast->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnEGaugeData(void *userObj, const UInt8 *data, UOSInt dataSize)
{
	SSWR::AVIRead::AVIReGaugeSvrForm *me = (SSWR::AVIRead::AVIReGaugeSvrForm*)userObj;
	Sync::MutexUsage mutUsage(me->reqMut);
	if (me->reqLast)
	{
		me->reqLast->Release();
	}
	me->reqLast = Text::String::New(data, dataSize).Ptr();
	me->reqUpdated = true;
}

SSWR::AVIRead::AVIReGaugeSvrForm::AVIReGaugeSvrForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("eGauge Server"));
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->log = 0;
	this->dirHdlr = 0;
	this->logger = 0;
	this->reqLast = 0;
	this->reqUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->lblPort = ui->NewLabel(this->tpControl, CSTR("Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->tpControl, CSTR("12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(158, 8, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);

	this->tpReqText = this->tcMain->AddTabPage(CSTR("ReqText"));
	this->txtReqText = ui->NewTextBox(this->tpReqText, CSTR(""), true);
	this->txtReqText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtReqText->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIReGaugeSvrForm::~AVIReGaugeSvrForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->dirHdlr);
	if (this->reqLast)
	{
		this->reqLast->Release();
		this->reqLast = 0;
	}
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIReGaugeSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
