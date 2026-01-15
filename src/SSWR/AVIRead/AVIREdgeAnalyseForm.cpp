#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "SSWR/AVIRead/AVIREdgeAnalyseForm.h"
#define TITLE CSTR("Edge Analyse Device")
#define MAX_ITEMS 300

class EdgeAnalyseHandler : public Net::WebServer::WebHandler
{
private:
	NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me;
	
	virtual void DoWebRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
	{
		if (req->GetReqMethod() != Net::WebUtil::RequestMethod::HTTP_POST)
		{
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}

		UnsafeArray<const UInt8> data;
		UOSInt dataSize;
		if (!req->GetReqData(dataSize).SetTo(data))
		{
			resp->ResponseError(req, Net::WebStatus::SC_NOT_ACCEPTABLE);
			return;
		}

		NN<Text::JSONBase> json;
		if (!Text::JSONBase::ParseJSONBytes(Data::ByteArrayR(data, dataSize)).SetTo(json))
		{
			resp->ResponseError(req, Net::WebStatus::SC_UNPROCESSABLE_ENTITY);
			return;
		}
		NN<Text::String> type;
		if (json->GetValueString(CSTR("type")).SetTo(type))
		{
			this->me->AddRecord(Data::Timestamp::Now(), type, json);
			json->EndUse();
			resp->SetStatusCode(Net::WebStatus::SC_NO_CONTENT);
			resp->AddDefHeaders(req);
			resp->AddContentLength(0);
			return;
		}
		json->EndUse();
		resp->ResponseError(req, Net::WebStatus::SC_UNPROCESSABLE_ENTITY);
		return;
	}

public:
	EdgeAnalyseHandler(NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me)
	{
		this->me = me;
	}

	virtual ~EdgeAnalyseHandler()
	{
	}
};

void __stdcall SSWR::AVIRead::AVIREdgeAnalyseForm::OnWebHookClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me = userObj.GetNN<SSWR::AVIRead::AVIREdgeAnalyseForm>();
	NN<Net::WebServer::WebListener> listener;
	if (me->webHookListener.NotNull())
	{
		me->webHookListener.Delete();
		me->txtWebHookPort->SetReadOnly(false);
		me->btnWebHook->SetText(CSTR("Start"));
		return;
	}
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtWebHookPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid port"), TITLE, me);
		return;
	}
	NEW_CLASSNN(listener, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), nullptr, me->webHookHdlr, port, 60, 1, 4, CSTR("EdgeAnalyse/1.0"), false, Net::WebServer::KeepAlive::Always, true));
	if (listener->IsError())
	{
		me->ui->ShowMsgOK(CSTR("Error in listening port"), TITLE, me);
		listener.Delete();
		return;
	}
	listener->SetAccessLog(me->log, IO::LogHandler::LogLevel::Action);
	me->webHookListener = listener;
	me->txtWebHookPort->SetReadOnly(true);
	me->btnWebHook->SetText(CSTR("Stop"));
}

void __stdcall SSWR::AVIRead::AVIREdgeAnalyseForm::OnWebHookDataSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me = userObj.GetNN<SSWR::AVIRead::AVIREdgeAnalyseForm>();
	NN<EdgeRecord> record;
	if (me->lvWebHookData->GetSelectedItem().GetOpt<EdgeRecord>().SetTo(record))
	{
		Text::StringBuilderUTF8 sb;
		record->json->ToJSONStringWF(sb, 0);
		me->txtWebHookData->SetText(sb.ToCString());
	}
	else
	{
		me->txtWebHookData->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIREdgeAnalyseForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me = userObj.GetNN<SSWR::AVIRead::AVIREdgeAnalyseForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
	else
	{
		me->txtLog->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIREdgeAnalyseForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me = userObj.GetNN<SSWR::AVIRead::AVIREdgeAnalyseForm>();
	NN<EdgeRecord> record;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<Text::String> s;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Sync::MutexUsage mutUsage(me->newRecordsMut);
	if (me->newRecords.GetCount() > 0)
	{
		Text::StringBuilderUTF8 sb;
		Data::ArrayListNN<EdgeRecord> records;
		records.AddAll(me->newRecords);
		me->newRecords.Clear();
		mutUsage.EndUse();

		if (records.GetCount() >= MAX_ITEMS)
		{
			me->records.Clear();
			me->lvWebHookData->ClearItems();
			while (records.GetCount() > MAX_ITEMS)
			{
				if (records.RemoveAt(0).SetTo(record))
				{
					FreeRecord(record);
				}
			}
		}
		else if (me->records.GetCount() > (i = MAX_ITEMS - records.GetCount()))
		{
			while (me->records.GetCount() > i)
			{
				if (me->records.RemoveAt(0).SetTo(record))
				{
					FreeRecord(record);
				}
				me->lvWebHookData->RemoveItem(0);
			}
		}
		i = 0;
		j = records.GetCount();
		while (i < j)
		{
			record = records.GetItemNoCheck(i);
			sptr = record->ts.ToStringNoZone(sbuff);
			k = me->lvWebHookData->AddItem(CSTRP(sbuff, sptr), record);
			if (record->json->GetValueString(CSTR("deviceSn")).SetTo(s))
			{
				me->lvWebHookData->SetSubItem(k, 1, s);
			}
			if (record->json->GetValueString(CSTR("cameraId")).SetTo(s))
			{
				me->lvWebHookData->SetSubItem(k, 2, s);
			}
			me->lvWebHookData->SetSubItem(k, 3, record->type);
			if (record->type->Equals(CSTR("passengerNumber")))
			{
				NN<Text::String> hour;
				Double leaveNumber;
				Double enterNumber;
				if (record->json->GetValueString(CSTR("data.hour")).SetTo(hour) && record->json->GetValueAsDouble(CSTR("data.enterNumber"), enterNumber) && record->json->GetValueAsDouble(CSTR("data.leaveNumber"), leaveNumber))
				{
					sb.ClearStr();
					sb.Append(CSTR("hour="));
					sb.Append(hour);
					sb.Append(CSTR(", enterNumber="));
					sb.AppendI32(Double2Int32(enterNumber));
					sb.Append(CSTR(", leaveNumber="));
					sb.AppendI32(Double2Int32(leaveNumber));
					me->lvWebHookData->SetSubItem(k, 4, sb.ToCString());
				}
			}
			else if (record->type->Equals(CSTR("peopleCountByArea")))
			{
				NN<Text::String> areaName;
				Double countNumber;
				if (record->json->GetValueString(CSTR("data.areaName")).SetTo(areaName) && record->json->GetValueAsDouble(CSTR("data.countNumber"), countNumber))
				{
					sb.ClearStr();
					sb.Append(CSTR("areaName="));
					sb.Append(areaName);
					sb.Append(CSTR(", countNumber="));
					sb.AppendI32(Double2Int32(countNumber));
					me->lvWebHookData->SetSubItem(k, 4, sb.ToCString());
				}
			}
			me->records.Add(record);
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIREdgeAnalyseForm::FreeRecord(NN<EdgeRecord> record)
{
	record->type->Release();
	record->json->EndUse();
	MemFreeNN(record);
}

SSWR::AVIRead::AVIREdgeAnalyseForm::AVIREdgeAnalyseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->webHookListener = nullptr;
	NEW_CLASSNN(this->webHookHdlr, EdgeAnalyseHandler(*this));
	this->SetText(TITLE);
	this->SetFont(nullptr, 8.25, false);
	this->OnMonitorChanged();

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpWebHook = this->tcMain->AddTabPage(CSTR("Web Hook"));
	this->pnlWebHook = ui->NewPanel(this->tpWebHook);
	this->pnlWebHook->SetRect(0, 0, 100, 55, false);
	this->pnlWebHook->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblWebHookPort = ui->NewLabel(this->pnlWebHook, CSTR("Port"));
	this->lblWebHookPort->SetRect(4, 4, 100, 23, false);
	this->txtWebHookPort = ui->NewTextBox(this->pnlWebHook, CSTR("8080"));
	this->txtWebHookPort->SetRect(104, 4, 100, 23, false);
	this->btnWebHook = ui->NewButton(this->pnlWebHook, CSTR("Start"));
	this->btnWebHook->SetRect(104, 28, 75, 23, false);
	this->btnWebHook->HandleButtonClick(OnWebHookClicked, this);
	this->txtWebHookData = ui->NewTextBox(this->tpWebHook, CSTR(""), true);
	this->txtWebHookData->SetRect(0, 0, 100, 120, false);
	this->txtWebHookData->SetReadOnly(true);
	this->txtWebHookData->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvWebHookData = ui->NewListView(this->tpWebHook, UI::ListViewStyle::Table, 5);
	this->lvWebHookData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWebHookData->SetFullRowSelect(true);
	this->lvWebHookData->SetShowGrid(true);
	this->lvWebHookData->AddColumn(CSTR("Time"), 120);
	this->lvWebHookData->AddColumn(CSTR("DeviceSN"), 150);
	this->lvWebHookData->AddColumn(CSTR("CameraId"), 150);
	this->lvWebHookData->AddColumn(CSTR("Type"), 100);
	this->lvWebHookData->AddColumn(CSTR("Desc"), 300);
	this->lvWebHookData->HandleSelChg(OnWebHookDataSelChg, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);
	
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 300, false));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIREdgeAnalyseForm::~AVIREdgeAnalyseForm()
{
	this->ClearChildren();
	this->webHookListener.Delete();
	this->webHookHdlr.Delete();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->records.FreeAll(FreeRecord);
	this->newRecords.FreeAll(FreeRecord);
}

void SSWR::AVIRead::AVIREdgeAnalyseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIREdgeAnalyseForm::AddRecord(Data::Timestamp ts, NN<Text::String> type, NN<Text::JSONBase> json)
{
	NN<EdgeRecord> record = MemAllocNN(EdgeRecord);
	record->ts = ts;
	record->type = type->Clone();
	record->json = json;
	json->BeginUse();
	Sync::MutexUsage mutUsage(this->newRecordsMut);
	this->newRecords.Add(record);
}
