#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRBTScanLogDevForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRBTScanLogDevForm::OnCSVClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBTScanLogDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBTScanLogDevForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"BTScanLogDev", true);
	dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		{
			IO::BufferedOutputStream stm(fs, 8192);
			Data::DateTime dt;
			NN<IO::BTScanLog::LogEntry> log;
			Int64 lastTick;
			UOSInt i = 0;
			UOSInt j = me->entry->logs->GetCount();
			if (j > 0)
			{
				lastTick = me->entry->logs->GetItemNoCheck(0)->timeTicks;
			}
			while (i < j)
			{
				log = me->entry->logs->GetItemNoCheck(i);
				dt.SetTicks(log->timeTicks);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb.ClearStr();
				sb.AppendUTF8Char('\"');
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb.AppendUTF8Char('\"');
				sb.AppendUTF8Char(',');
				sb.AppendDouble((Double)(log->timeTicks - lastTick) / 1000.0);
				lastTick = log->timeTicks;
				sb.AppendUTF8Char(',');
				sb.AppendI16(log->rssi);
				sb.AppendUTF8Char(',');
				sb.AppendI16(log->txPower);
				sb.AppendC(UTF8STRC("\r\n"));
				stm.Write(sb.ToString(), sb.GetLength());
				i++;
			}
		}
	}
	dlg.Delete();
}

SSWR::AVIRead::AVIRBTScanLogDevForm::AVIRBTScanLogDevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<const IO::BTScanLog::DevEntry> entry) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt8 mac[8];
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Bluetooth Scan Log Entry"));

	this->core = core;
	this->entry = entry;

	this->pnlDevInfo = ui->NewPanel(*this);
	this->pnlDevInfo->SetRect(0, 0, 100, 56, false);
	this->pnlDevInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblMAC = ui->NewLabel(this->pnlDevInfo, CSTR("MAC"));
	this->lblMAC->SetRect(4, 4, 100, 23, false);
	WriteMUInt64(mac, entry->macInt);
	sptr = Text::StrHexBytes(sbuff, &mac[2], 6, ':');
	this->txtMAC = ui->NewTextBox(this->pnlDevInfo, CSTRP(sbuff, sptr));
	this->txtMAC->SetRect(104, 4, 200, 23, false);
	this->txtMAC->SetReadOnly(true);
	this->lblName = ui->NewLabel(this->pnlDevInfo, CSTR("Name"));
	this->lblName->SetRect(4, 28, 100, 23, false);
	if (entry->name)
	{
		this->txtName = ui->NewTextBox(this->pnlDevInfo, entry->name->ToCString());
	}
	else
	{
		this->txtName = ui->NewTextBox(this->pnlDevInfo, CSTR(""));
	}
	this->txtName->SetRect(104, 28, 200, 23, false);
	this->txtName->SetReadOnly(true);
	this->btnCSV = ui->NewButton(this->pnlDevInfo, CSTR("Save CSV"));
	this->btnCSV->SetRect(304, 28, 75, 23, false);
	this->btnCSV->HandleButtonClick(OnCSVClicked, this);
	this->lvContent = ui->NewListView(*this, UI::ListViewStyle::Table, 5);
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->AddColumn(CSTR("Time"), 180);
	this->lvContent->AddColumn(CSTR("Diff"), 60);
	this->lvContent->AddColumn(CSTR("RSSI"), 80);
	this->lvContent->AddColumn(CSTR("TXPower"), 120);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	Data::DateTime dt;
	NN<IO::BTScanLog::LogEntry> log;
	Int64 lastTick;
	UOSInt i = 0;
	UOSInt j = entry->logs->GetCount();
	UOSInt k;
	if (j > 0)
	{
		lastTick = entry->logs->GetItemNoCheck(0)->timeTicks;
	}
	while (i < j)
	{
		log = entry->logs->GetItemNoCheck(i);
		dt.SetTicks(log->timeTicks);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		k = this->lvContent->AddItem(CSTRP(sbuff, sptr), log);
		sptr = Text::StrDouble(sbuff, (Double)(log->timeTicks - lastTick) / 1000.0);
		this->lvContent->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		lastTick = log->timeTicks;
		sptr = Text::StrInt16(sbuff, log->rssi);
		this->lvContent->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		sptr = Text::StrInt16(sbuff, log->txPower);
		this->lvContent->SetSubItem(k, 3, CSTRP(sbuff, sptr));
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
