#include "Stdafx.h"
#include "Exporter/XLSXExporter.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRJMeterLogForm.h"
#include "Text/SpreadSheet/Workbook.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRJMeterLogForm::OnFileDropped(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRJMeterLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJMeterLogForm>();
	me->OpenFile(files[0]->ToCString());
}

void __stdcall SSWR::AVIRead::AVIRJMeterLogForm::OnBrowseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJMeterLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJMeterLogForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"JMeterLog", false);
	dlg->AddFilter(CSTR("*.csv"), CSTR("CSV Log file"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->OpenFile(dlg->GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRJMeterLogForm::OnConcurrClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJMeterLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJMeterLogForm>();
	Text::StringBuilderUTF8 sb;
	me->txtConcurr->GetText(sb);
	Int64 ts;
	if (sb.ToInt64(ts))
	{
		///////////////////////
	}
}

void __stdcall SSWR::AVIRead::AVIRJMeterLogForm::OnExportClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJMeterLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJMeterLogForm>();
	NN<IO::JMeterLog> log;
	Data::Timestamp startTime;
	Data::Timestamp endTime;
	Int64 iStartTime;
	Int64 iEndTime;
	Text::StringBuilderUTF8 sb;
	if (!me->log.SetTo(log))
	{
		return;
	}
	me->txtExportStart->GetText(sb);
	startTime = Data::Timestamp::FromStr(sb.ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	sb.ClearStr();
	me->txtExportEnd->GetText(sb);
	endTime = Data::Timestamp::FromStr(sb.ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	if (startTime.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Start Time is not valid"), CSTR("JMeter Log"), me);
		return;
	}
	if (endTime.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("End Time is not valid"), CSTR("JMeter Log"), me);
		return;
	}
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"JMeterLogExport", true);
	dlg->AddFilter(CSTR("*.xlsx"), CSTR("Excel 2007 file"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		iStartTime = startTime.ToTicks();
		iEndTime = endTime.ToTicks();
		Text::SpreadSheet::Workbook wb;
		NN<Text::SpreadSheet::Worksheet> sheet;
		sheet = wb.AddWorksheet(CSTR("Group"));
		sheet->SetCellString(0, 0, CSTR("Label"));
		sheet->SetCellString(0, 1, CSTR("Count"));
		sheet->SetCellString(0, 2, CSTR("ErrorCount"));
		sheet->SetCellString(0, 3, CSTR("ErrorRate"));
		sheet->SetCellString(0, 4, CSTR("Avg Dur"));
		sheet->SetCellString(0, 5, CSTR("Min Dur"));
		sheet->SetCellString(0, 6, CSTR("Max Dur"));
		sheet->SetCellString(0, 7, CSTR("URL"));


		Data::ArrayListNN<IO::JMeterGroup> groups;
		NN<IO::JMeterGroup> group;
		NN<IO::JMeterLogItem> item;
		log->GetGroups(groups);
		Int64 totalDur;
		Int64 minDur;
		Int64 maxDur;
		Int64 dur;
		UOSInt cnt;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		i = 0;
		j = groups.GetCount();
		while (i < j)
		{
			group = groups.GetItemNoCheck(i);
			sheet->SetCellString(i + 1, 0, group->label);
			totalDur = 0;
			minDur = 1000000000;
			maxDur = 0;
			cnt = 0;
			l = 0;
			k = group->logs.GetCount();
			while (k-- > 0)
			{
				item = group->logs.GetItemNoCheck(k);
				if (item->startTime >= iStartTime && item->startTime < iEndTime)
				{
					dur = item->endTime - item->startTime;
					totalDur += dur;
					if (dur < minDur) minDur = dur;
					if (dur > maxDur) maxDur = dur;
					if (!item->success) l++;
					cnt++;
				}
			}
			sheet->SetCellInt64(i + 1, 1, (Int64)cnt);
			if (cnt != 0)
			{
				sheet->SetCellInt64(i + 1, 2, (Int64)l);
				sheet->SetCellDouble(i + 1, 3, ((Double)l * 100.0)/(Double)cnt);
				sheet->SetCellInt64(i + 1, 4, totalDur /(Int64)group->logs.GetCount());
				sheet->SetCellInt64(i + 1, 5, minDur);
				sheet->SetCellInt64(i + 1, 6, maxDur);
			}
			sheet->SetCellString(i + 1, 7, group->url);
			i++;
		}

		Int64 t;
		Int64 currTime = iStartTime;
		Int64 maxTime = log->GetMaxTime();
		if (maxTime > iEndTime)
		{
			maxTime = iEndTime;
		}
		sheet = wb.AddWorksheet(CSTR("Concurr"));
		sheet->SetCellString(0, 0, CSTR("Time"));
		sheet->SetCellString(0, 1, CSTR("Count"));
		t = currTime % 1000;
		if (t != 0)
		{
			currTime += 1000 - t;
		}
		k = 1;
		while (currTime < maxTime)
		{
			l = log->GetConcurrCnt(currTime);
			sheet->SetCellTS(k, 0, Data::Timestamp(currTime, Data::DateTimeUtil::GetLocalTzQhr()));
			sheet->SetCellInt64(k, 1, (Int64)l);
			currTime += 1000;
			k++;
		}
		Exporter::XLSXExporter exporter;
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		exporter.ExportFile(fs, dlg->GetFileName()->ToCString(), wb, 0);
	}
}

void SSWR::AVIRead::AVIRJMeterLogForm::OpenFile(Text::CStringNN fileName)
{
	this->log.Delete();
	NN<IO::JMeterLog> log;
	NEW_CLASSNN(log, IO::JMeterLog(fileName));
	this->log = log;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	sptr = Data::Timestamp(log->GetMinTime(), Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
	this->txtStartTime->SetText(CSTRP(sbuff, sptr));
	sptr = Data::Timestamp(log->GetMaxTime(), Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
	this->txtEndTime->SetText(CSTRP(sbuff, sptr));
	this->txtLogFile->SetText(fileName);
	this->lvGroup->ClearItems();
	this->lvThread->ClearItems();
	Data::ArrayListNN<IO::JMeterThread> threads;
	Data::ArrayListNN<IO::JMeterGroup> groups;
	NN<IO::JMeterGroup> group;
	NN<IO::JMeterThread> thread;
	NN<IO::JMeterLogItem> item;
	log->GetGroups(groups);
	log->GetThreads(threads);
	Int64 totalDur;
	Int64 minDur;
	Int64 maxDur;
	Int64 dur;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	i = 0;
	j = groups.GetCount();
	while (i < j)
	{
		group = groups.GetItemNoCheck(i);
		this->lvGroup->AddItem(group->label, group);
		sptr = Text::StrUOSInt(sbuff, group->logs.GetCount());
		this->lvGroup->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		totalDur = 0;
		minDur = 1000000000;
		maxDur = 0;
		l = 0;
		k = group->logs.GetCount();
		while (k-- > 0)
		{
			item = group->logs.GetItemNoCheck(k);
			dur = item->endTime - item->startTime;
			totalDur += dur;
			if (dur < minDur) minDur = dur;
			if (dur > maxDur) maxDur = dur;
			if (!item->success) l++;
		}
		sptr = Text::StrUOSInt(sbuff, l);
		this->lvGroup->SetSubItem(i, 2, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, ((Double)l * 100.0)/(Double)group->logs.GetCount());
		this->lvGroup->SetSubItem(i, 3, CSTRP(sbuff, sptr));
		sptr = Text::StrInt64(sbuff, totalDur /(Int64)group->logs.GetCount());
		this->lvGroup->SetSubItem(i, 4, CSTRP(sbuff, sptr));
		sptr = Text::StrInt64(sbuff, minDur);
		this->lvGroup->SetSubItem(i, 5, CSTRP(sbuff, sptr));
		sptr = Text::StrInt64(sbuff, maxDur);
		this->lvGroup->SetSubItem(i, 6, CSTRP(sbuff, sptr));
		this->lvGroup->SetSubItem(i, 7, group->url);
		i++;
	}

	i = 0;
	j = threads.GetCount();
	while (i < j)
	{
		thread = threads.GetItemNoCheck(i);
		this->lvThread->AddItem(thread->name, thread);
		sptr = Text::StrUOSInt(sbuff, thread->logs.GetCount());
		this->lvThread->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		i++;
	}

	Int64 t;
	Int64 currTime = log->GetMinTime();
	Int64 maxTime = log->GetMaxTime();
	this->lvConcurr->ClearItems();
	t = currTime % 1000;
	if (t != 0)
	{
		currTime += 1000 - t;
	}
	while (currTime < maxTime)
	{
		l = log->GetConcurrCnt(currTime);
		sptr = Data::Timestamp(currTime, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
		k = this->lvConcurr->AddItem(CSTRP(sbuff, sptr), 0);
		sptr = Text::StrUOSInt(sbuff, l);
		this->lvConcurr->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		currTime += 1000;
	}
}

SSWR::AVIRead::AVIRJMeterLogForm::AVIRJMeterLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("JMeter Results"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->log = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 80, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblLogFile = ui->NewLabel(this->pnlFile, CSTR("Results File"));
	this->lblLogFile->SetRect(4, 4, 100, 23, false);
	this->txtLogFile = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtLogFile->SetRect(104, 4, 700, 23, false);
	this->btnBrowse = ui->NewButton(this->pnlFile, CSTR("Browse"));
	this->btnBrowse->SetRect(804, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->lblStartTime = ui->NewLabel(this->pnlFile, CSTR("StartTime"));
	this->lblStartTime->SetRect(4, 28, 100, 23, false);
	this->txtStartTime = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtStartTime->SetRect(104, 28, 200, 23, false);
	this->txtStartTime->SetReadOnly(true);
	this->lblEndTime = ui->NewLabel(this->pnlFile, CSTR("EndTime"));
	this->lblEndTime->SetRect(4, 52, 100, 23, false);
	this->txtEndTime = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtEndTime->SetRect(104, 52, 200, 23, false);
	this->txtEndTime->SetReadOnly(true);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpGroup = this->tcMain->AddTabPage(CSTR("Group"));
	this->lvGroup = ui->NewListView(this->tpGroup, UI::ListViewStyle::Table, 8);
	this->lvGroup->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvGroup->SetFullRowSelect(true);
	this->lvGroup->SetShowGrid(true);
	this->lvGroup->AddColumn(CSTR("Label"), 200);
	this->lvGroup->AddColumn(CSTR("Count"), 100);
	this->lvGroup->AddColumn(CSTR("ErrorCount"), 100);
	this->lvGroup->AddColumn(CSTR("ErrorRate"), 100);
	this->lvGroup->AddColumn(CSTR("Avg Dur"), 100);
	this->lvGroup->AddColumn(CSTR("Min Dur"), 100);
	this->lvGroup->AddColumn(CSTR("Max Dur"), 100);
	this->lvGroup->AddColumn(CSTR("URL"), 500);

	this->tpThread = this->tcMain->AddTabPage(CSTR("Thread"));
	this->lvThread = ui->NewListView(this->tpThread, UI::ListViewStyle::Table, 2);
	this->lvThread->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvThread->SetFullRowSelect(true);
	this->lvThread->SetShowGrid(true);
	this->lvThread->AddColumn(CSTR("Name"), 200);
	this->lvThread->AddColumn(CSTR("Count"), 100);

	this->tpConcurr = this->tcMain->AddTabPage(CSTR("Concurr"));
	this->pnlConcurr = ui->NewPanel(this->tpConcurr);
	this->pnlConcurr->SetRect(0, 0, 100, 31, false);
	this->pnlConcurr->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblConcurr = ui->NewLabel(this->pnlConcurr, CSTR("Time"));
	this->lblConcurr->SetRect(4, 4, 100, 23, false);
	this->txtConcurr = ui->NewTextBox(this->pnlConcurr, CSTR(""));
	this->txtConcurr->SetRect(104, 4, 150, 23, false);
	this->btnConcurr = ui->NewButton(this->pnlConcurr, CSTR("Calc"));
	this->btnConcurr->SetRect(254, 4, 75, 23, false);
	this->btnConcurr->HandleButtonClick(OnConcurrClicked, this);
	this->lvConcurr = ui->NewListView(this->tpConcurr, UI::ListViewStyle::Table, 2);
	this->lvConcurr->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvConcurr->SetFullRowSelect(true);
	this->lvConcurr->SetShowGrid(true);
	this->lvConcurr->AddColumn(CSTR("Time"), 150);
	this->lvConcurr->AddColumn(CSTR("Count"), 100);

	this->tpExport = this->tcMain->AddTabPage(CSTR("Export"));
	this->lblExportStart = ui->NewLabel(this->tpExport, CSTR("Start Time"));
	this->lblExportStart->SetRect(4, 4, 100, 23, false);
	this->txtExportStart = ui->NewTextBox(this->tpExport, CSTR(""));
	this->txtExportStart->SetRect(104, 4, 150, 23, false);
	this->lblExportEnd = ui->NewLabel(this->tpExport, CSTR("End Time"));
	this->lblExportEnd->SetRect(4, 28, 100, 23, false);
	this->txtExportEnd = ui->NewTextBox(this->tpExport, CSTR(""));
	this->txtExportEnd->SetRect(104, 28, 150, 23, false);
	this->btnExport = ui->NewButton(this->tpExport, CSTR("Export"));
	this->btnExport->SetRect(104, 52, 75, 23, false);
	this->btnExport->HandleButtonClick(OnExportClicked, this);

	this->HandleDropFiles(OnFileDropped, this);
}

SSWR::AVIRead::AVIRJMeterLogForm::~AVIRJMeterLogForm()
{
	this->log.Delete();
}

void SSWR::AVIRead::AVIRJMeterLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
