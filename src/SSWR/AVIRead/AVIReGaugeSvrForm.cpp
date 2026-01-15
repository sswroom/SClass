#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIReGaugeSvrForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIReGaugeSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIReGaugeSvrForm>();
	if (me->svr.NotNull())
	{
		me->svr.Delete();
		me->dirHdlr.Delete();
		me->log.Delete();
		me->logger.Delete();
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
			NN<Net::WebServer::EGaugeHandler> dirHdlr;
			NN<Net::WebServer::WebListener> svr;
			NEW_CLASSNN(dirHdlr, Net::WebServer::EGaugeHandler());
			dirHdlr->HandleEGaugeData(OnEGaugeData, me);
			NEW_CLASSNN(svr, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), nullptr, dirHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("eGauge/1.0"), false, Net::WebServer::KeepAlive::Default, false));
			if (svr->IsError())
			{
				valid = false;
				svr.Delete();
				dirHdlr.Delete();
			}
			else
			{
				NN<IO::LogTool> log;
				NN<UI::ListBoxLogger> logger;
				me->svr = svr;
				me->dirHdlr = dirHdlr;
				NEW_CLASSNN(log, IO::LogTool());
				svr->SetAccessLog(log, IO::LogHandler::LogLevel::Raw);
				me->log = log;
				NEW_CLASSNN(logger, UI::ListBoxLogger(me, me->lbLog, 500, true));
				me->logger = logger;
				log->AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
				if (!svr->Start())
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
			me->svr.Delete();
			me->dirHdlr.Delete();
			me->log.Delete();
			me->logger.Delete();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnLogSel(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIReGaugeSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIReGaugeSvrForm>();
	Optional<Text::String> s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIReGaugeSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIReGaugeSvrForm>();
	if (me->reqUpdated)
	{
		Sync::MutexUsage mutUsage(me->reqMut);
		me->reqUpdated = false;
		me->txtReqText->SetText(Text::String::OrEmpty(me->reqLast)->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIReGaugeSvrForm::OnEGaugeData(AnyType userObj, UnsafeArray<const UInt8> data, UOSInt dataSize)
{
	NN<SSWR::AVIRead::AVIReGaugeSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIReGaugeSvrForm>();
	Sync::MutexUsage mutUsage(me->reqMut);
	OPTSTR_DEL(me->reqLast);
	me->reqLast = Text::String::New(data, dataSize);
	me->reqUpdated = true;
}

SSWR::AVIRead::AVIReGaugeSvrForm::AVIReGaugeSvrForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("eGauge Server"));
	this->SetFont(nullptr, 8.25, false);
	this->svr = nullptr;
	this->log = nullptr;
	this->dirHdlr = nullptr;
	this->logger = nullptr;
	this->reqLast = nullptr;
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
	this->svr.Delete();
	this->dirHdlr.Delete();
	OPTSTR_DEL(this->reqLast);
	this->log.Delete();
	this->logger.Delete();
}

void SSWR::AVIRead::AVIReGaugeSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
