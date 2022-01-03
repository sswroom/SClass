#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRBTScanLogDevForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"

void __stdcall SSWR::AVIRead::AVIRBTScanLogDevForm::OnCSVClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBTScanLogDevForm *me = (SSWR::AVIRead::AVIRBTScanLogDevForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"BTScanLogDev", true));
	dlg->AddFilter((const UTF8Char*)"*.csv", (const UTF8Char*)"CSV File");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[256];
		IO::FileStream *fs;
		IO::BufferedOutputStream *stm;
		NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(stm, IO::BufferedOutputStream(fs, 8192));
		Data::DateTime dt;
		IO::BTScanLog::LogEntry *log;
		Int64 lastTick;
		UOSInt i = 0;
		UOSInt j = me->entry->logs->GetCount();
		if (j > 0)
		{
			lastTick = me->entry->logs->GetItem(0)->timeTicks;
		}
		while (i < j)
		{
			log = me->entry->logs->GetItem(i);
			dt.SetTicks(log->timeTicks);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			sb.ClearStr();
			sb.AppendChar('\"', 1);
			sb.Append(sbuff);
			sb.AppendChar('\"', 1);
			sb.AppendChar(',', 1);
			Text::SBAppendF64(&sb, (Double)(log->timeTicks - lastTick) / 1000.0);
			lastTick = log->timeTicks;
			sb.AppendChar(',', 1);
			sb.AppendI16(log->rssi);
			sb.AppendChar(',', 1);
			sb.AppendI16(log->txPower);
			sb.AppendC(UTF8STRC("\r\n"));
			stm->Write(sb.ToString(), sb.GetLength());
			i++;
		}
		DEL_CLASS(stm);
		DEL_CLASS(fs);
	}
	DEL_CLASS(dlg);
}

SSWR::AVIRead::AVIRBTScanLogDevForm::AVIRBTScanLogDevForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const IO::BTScanLog::DevEntry *entry) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[256];
	UInt8 mac[8];
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Bluetooth Scan Log Entry");

	this->core = core;
	this->entry = entry;

	NEW_CLASS(this->pnlDevInfo, UI::GUIPanel(ui, this));
	this->pnlDevInfo->SetRect(0, 0, 100, 56, false);
	this->pnlDevInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblMAC, UI::GUILabel(ui, this->pnlDevInfo, (const UTF8Char*)"MAC"));
	this->lblMAC->SetRect(4, 4, 100, 23, false);
	WriteMUInt64(mac, entry->macInt);
	Text::StrHexBytes(sbuff, &mac[2], 6, ':');
	NEW_CLASS(this->txtMAC, UI::GUITextBox(ui, this->pnlDevInfo, sbuff));
	this->txtMAC->SetRect(104, 4, 200, 23, false);
	this->txtMAC->SetReadOnly(true);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this->pnlDevInfo, (const UTF8Char*)"Name"));
	this->lblName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this->pnlDevInfo, (entry->name?entry->name:(const UTF8Char*)"")));
	this->txtName->SetRect(104, 28, 200, 23, false);
	this->txtName->SetReadOnly(true);
	NEW_CLASS(this->btnCSV, UI::GUIButton(ui, this->pnlDevInfo, (const UTF8Char*)"Save CSV"));
	this->btnCSV->SetRect(304, 28, 75, 23, false);
	this->btnCSV->HandleButtonClick(OnCSVClicked, this);
	NEW_CLASS(this->lvContent, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->AddColumn((const UTF8Char*)"Time", 180);
	this->lvContent->AddColumn((const UTF8Char*)"Diff", 60);
	this->lvContent->AddColumn((const UTF8Char*)"RSSI", 80);
	this->lvContent->AddColumn((const UTF8Char*)"TXPower", 120);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	Data::DateTime dt;
	IO::BTScanLog::LogEntry *log;
	Int64 lastTick;
	UOSInt i = 0;
	UOSInt j = entry->logs->GetCount();
	UOSInt k;
	if (j > 0)
	{
		lastTick = entry->logs->GetItem(0)->timeTicks;
	}
	while (i < j)
	{
		log = entry->logs->GetItem(i);
		dt.SetTicks(log->timeTicks);
		dt.ToLocalTime();
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		k = this->lvContent->AddItem(sbuff, log);
		Text::StrDouble(sbuff, (Double)(log->timeTicks - lastTick) / 1000.0);
		this->lvContent->SetSubItem(k, 1, sbuff);
		lastTick = log->timeTicks;
		Text::StrInt16(sbuff, log->rssi);
		this->lvContent->SetSubItem(k, 2, sbuff);
		Text::StrInt16(sbuff, log->txPower);
		this->lvContent->SetSubItem(k, 3, sbuff);
		i++;
	}
}

SSWR::AVIRead::AVIRBTScanLogDevForm::~AVIRBTScanLogDevForm()
{
}

void SSWR::AVIRead::AVIRBTScanLogDevForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
