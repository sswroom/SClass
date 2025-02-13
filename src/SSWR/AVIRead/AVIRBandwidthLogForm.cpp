#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "DB/TableDef.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRBandwidthLogForm.h"
#include "Text/UTF8Reader.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRBandwidthLogForm::JMeterThreadFree(NN<JMeterThread> thread)
{
	thread->name->Release();
	MemFreeNN(thread);
}

void __stdcall SSWR::AVIRead::AVIRBandwidthLogForm::DropFilesHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRBandwidthLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBandwidthLogForm>();
	NN<Text::String> file;
	UOSInt i = 0;
	UOSInt j = files.GetSize();
	UOSInt k;
	while (i < j)
	{
		file = files[i];
		k = file->LastIndexOf(IO::Path::PATH_SEPERATOR);
		if (file->Substring(k + 1).StartsWith(CSTR("jmeter")) && file->Substring(k + 1).IndexOf(CSTR(".log")) != INVALID_INDEX)
		{
			me->LoadJMeterLog(file->ToCString());
		}
		else if (file->EndsWith(CSTR(".csv")))
		{
			me->LoadBandwidthLog(file->ToCString());
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRBandwidthLogForm::OnExportClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBandwidthLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBandwidthLogForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<BandwidthIP> ipItem;
	if (me->startTime != 0 && me->endTime != 0 && me->cboIP->GetSelectedItemText(sbuff).SetTo(sptr) && me->bandwidthMap.GetC(CSTRP(sbuff, sptr)).SetTo(ipItem))
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"BandwidthLogExport", true);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in creating file"), CSTR("Bandwidth Log"), me);
			}
			else
			{
				NN<BandwidthItem> item;
				Data::ArrayListNN<JMeterThread> threads;
				IO::BufferedOutputStream stm(fs, 8192);
				stm.Write(CSTR("Time,RecvBytes,SendBytes,Mbps,Concurr\r\n").ToByteArray());
				Text::StringBuilderUTF8 sb;
				Int64 currTime = me->startTime;
				Int64 endTime = me->endTime;
				NN<JMeterThread> thread;
				UOSInt jmeterI = 0;
				UOSInt jmeterJ = me->jmeterList.GetCount();
				while (jmeterI < jmeterJ)
				{
					thread = me->jmeterList.GetItemNoCheck(jmeterI);
					if ((thread->startTime / 1000) >= currTime)
					{
						break;
					}
					if ((thread->endTime / 1000) >= currTime)
						threads.Add(thread);
					jmeterI++;
				}
				UOSInt k;
				UOSInt i = 0;
				UOSInt j = ipItem->items.GetCount();
				while (currTime <= endTime)
				{
					sb.ClearStr();
					sb.AppendTS(Data::Timestamp(currTime * 1000, Data::DateTimeUtil::GetLocalTzQhr()), "HH:mm:ss");
					sb.AppendUTF8Char(',');
					if (i < j && (item = ipItem->items.GetItemNoCheck(i))->time <= currTime)
					{
						UInt64 recvBytes = item->recvBytes;
						UInt64 sendBytes = item->sendBytes;
						while (item->time < currTime && i + 1 < j)
						{
							item = ipItem->items.GetItemNoCheck(i + 1);
							if (item->time > currTime)
								break;
							i++;
							recvBytes += item->recvBytes;
							sendBytes += item->sendBytes;
						}
						sb.AppendU64(recvBytes);
						sb.AppendUTF8Char(',');
						sb.AppendU64(sendBytes);
						sb.AppendUTF8Char(',');
						sb.AppendDouble(UInt64_Double(recvBytes + sendBytes) * 0.000008);
						sb.AppendUTF8Char(',');
						i++;
					}
					else
					{
						sb.Append(CSTR("0,0,0,"));
					}
					k = threads.GetCount();
					while (k-- > 0)
					{
						thread = threads.GetItemNoCheck(k);
						if ((thread->endTime / 1000) < currTime)
						{
							threads.RemoveAt(k);
						}
					}
					while (jmeterI < jmeterJ)
					{
						thread = me->jmeterList.GetItemNoCheck(jmeterI);
						if ((thread->startTime / 1000) > currTime)
						{
							break;
						}
						threads.Add(thread);
						jmeterI++;
					}
					sb.AppendUOSInt(threads.GetCount());
					sb.Append(CSTR("\r\n"));
					stm.Write(sb.ToByteArray());
					currTime++;
				}
			}
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRBandwidthLogForm::OnJMeterExportClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBandwidthLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBandwidthLogForm>();
	if (me->jmeterList.GetCount() > 0)
	{
		Text::StringBuilderUTF8 sb;
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"BandwidthLogJMeter", true);
		me->txtJMeterLog->GetText(sb);
		sb.Append(CSTR(".csv"));
		dlg->SetFileName(sb.ToCString());
		dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (fs.IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in creating file"), CSTR("Bandwidth Log"), me);
			}
			else
			{
				NN<JMeterThread> thread;
				IO::BufferedOutputStream stm(fs, 8192);
				stm.Write(CSTR("Name,Start Time,End Time,Dur(second)\r\n").ToByteArray());
				NN<Text::String> s;
				UOSInt i = 0;
				UOSInt j = me->jmeterList.GetCount();
				while (i < j)
				{
					thread = me->jmeterList.GetItemNoCheck(i);
					sb.ClearStr();
					s = Text::String::NewCSVRec(thread->name->v);
					sb.Append(s);
					s->Release();
					sb.AppendUTF8Char(',');
					sb.AppendTS(Data::Timestamp(thread->startTime, Data::DateTimeUtil::GetLocalTzQhr()), "HH:mm:ss.fff");
					sb.AppendUTF8Char(',');
					sb.AppendTS(Data::Timestamp(thread->endTime, Data::DateTimeUtil::GetLocalTzQhr()), "HH:mm:ss.fff");
					sb.AppendUTF8Char(',');
					sb.AppendDouble(Int64_Double(thread->endTime - thread->startTime) * 0.001);
					sb.Append(CSTR("\r\n"));
					stm.Write(sb.ToByteArray());
					i++;
				}
			}
		}
		dlg.Delete();
	}

}

void SSWR::AVIRead::AVIRBandwidthLogForm::LoadBandwidthLog(Text::CStringNN fileName)
{
	NN<BandwidthIP> ipItem;
	NN<BandwidthItem> item;
	NN<DB::TableDef> table;
	NN<DB::ColDef> col;
	NN<DB::DBReader> r;
	DB::CSVFile csv(fileName, 0);
	if (csv.GetTableDef(0, fileName).SetTo(table))
	{
		if (table->GetColCnt() >= 6)
		{
			Bool valid = true;
			if (!table->GetCol(0).SetTo(col) || !col->GetColName()->Equals(CSTR("IP")))
				valid = false;
			if (!table->GetCol(1).SetTo(col) || !col->GetColName()->Equals(CSTR("Time")))
				valid = false;
			if (!table->GetCol(2).SetTo(col) || !col->GetColName()->Equals(CSTR("RecvCnt")))
				valid = false;
			if (!table->GetCol(3).SetTo(col) || !col->GetColName()->Equals(CSTR("RecvBytes")))
				valid = false;
			if (!table->GetCol(4).SetTo(col) || !col->GetColName()->Equals(CSTR("SendCnt")))
				valid = false;
			if (!table->GetCol(5).SetTo(col) || !col->GetColName()->Equals(CSTR("SendBytes")))
				valid = false;
			if (valid && csv.QueryTableData(0, fileName, 0, 0, 0, 0, 0).SetTo(r))
			{
				this->ClearBandwidthLog();
				Text::StringBuilderUTF8 sb;
				Int64 startTime = 0;
				Int64 endTime = 0;
				Int64 time;
				while (r->ReadNext())
				{
					sb.ClearStr();
					r->GetStr(0, sb);
					time = r->GetInt64(1);
					if (!this->bandwidthMap.GetC(sb.ToCString()).SetTo(ipItem))
					{
						NEW_CLASSNN(ipItem, BandwidthIP());
						ipItem->ip = Text::String::New(sb.ToCString());
						this->bandwidthMap.PutNN(ipItem->ip, ipItem);
					}
					item = MemAllocNN(BandwidthItem);
					item->time = time;
					item->recvCnt = (UInt32)r->GetInt32(2);
					item->recvBytes = (UInt64)r->GetInt64(3);
					item->sendCnt = (UInt32)r->GetInt32(4);
					item->sendBytes = (UInt64)r->GetInt64(5);
					ipItem->items.Add(item);
					if (startTime == 0)
					{
						startTime = time;
						endTime = time;
					}
					else if (startTime > time)
					{
						startTime = time;
					}
					else if (endTime < time)
					{
						endTime = time;
					}
				}

				this->txtBandwidthLog->SetText(fileName);
				sb.ClearStr();
				sb.AppendTSNoZone(Data::Timestamp(startTime * 1000, Data::DateTimeUtil::GetLocalTzQhr()));
				this->txtStartTime->SetText(sb.ToCString());
				sb.ClearStr();
				sb.AppendTSNoZone(Data::Timestamp(endTime * 1000, Data::DateTimeUtil::GetLocalTzQhr()));
				this->txtEndTime->SetText(sb.ToCString());
				csv.CloseReader(r);
				this->startTime = startTime;
				this->endTime = endTime;
				this->cboIP->ClearItems();
				UOSInt i = 0;
				UOSInt j = this->bandwidthMap.GetCount();
				while (i < j)
				{
					this->cboIP->AddItem(this->bandwidthMap.GetItemNoCheck(i)->ip, 0);
					i++;
				}
				if (j > 0)
				{
					this->cboIP->SetSelectedIndex(0);
				}
			}
		}
		table.Delete();
	}
}

void SSWR::AVIRead::AVIRBandwidthLogForm::LoadJMeterLog(Text::CStringNN fileName)
{
	Data::ArrayListNN<JMeterThread> logList;
	Data::FastStringMapNN<JMeterThread> threadMap;
	Text::StringBuilderUTF8 sb;
	Text::CStringNN logContent;
	Text::CStringNN threadName;
	Data::Timestamp ts;
	NN<JMeterThread> log;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(fs);
		while (reader.ReadLine(sb, 4096))
		{
			if (sb.leng > 51 && sb.StartsWith(CSTR("20")) && sb.v[23] == ' ' && sb.v[19] == ',')
			{
				sb.v[19] = '.';
				sb.v[23] = 0;
				ts = Data::Timestamp::FromStr(Text::CStringNN(sb.v, 23), Data::DateTimeUtil::GetLocalTzQhr());
				logContent = sb.ToCString().Substring(24);
				if (logContent.StartsWith(CSTR("INFO o.a.j.t.JMeterThread: Thread started: ")))
				{
					threadName = logContent.Substring(43);
					if (threadMap.GetC(threadName).SetTo(log))
					{
					}
					else
					{
						log = MemAllocNN(JMeterThread);
						log->name = Text::String::New(threadName);
						log->startTime = ts.ToTicks();
						log->endTime = 0;
						logList.Add(log);
						threadMap.PutNN(log->name, log);
					}
				}
				else if (logContent.StartsWith(CSTR("INFO o.a.j.t.JMeterThread: Thread is done: ")))
				{
					threadName = logContent.Substring(43);
					if (threadMap.RemoveC(threadName).SetTo(log))
					{
						log->endTime = ts.ToTicks();
					}
				}
				else if (logContent.StartsWith(CSTR("INFO o.a.j.t.JMeterThread: Thread finished: ")))
				{
					threadName = logContent.Substring(44);
					if (threadMap.RemoveC(threadName).SetTo(log))
					{
						log->endTime = ts.ToTicks();
					}
				}
			}
			sb.ClearStr();
		}
	}

	if (logList.GetCount() > 0)
	{
		this->ClearJMeterLog();
		this->txtJMeterLog->SetText(fileName);
		this->jmeterList.AddAll(logList);
		sb.ClearStr();
		sb.AppendUOSInt(logList.GetCount());
		this->txtJMeterThreads->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRBandwidthLogForm::ClearBandwidthLog()
{
	NN<BandwidthIP> ip;
	UOSInt i = 0;
	UOSInt j = this->bandwidthMap.GetCount();
	while (i < j)
	{
		ip = this->bandwidthMap.GetItemNoCheck(i);
		ip->ip->Release();
		ip->items.MemFreeAll();
		ip.Delete();
		i++;
	}
	this->bandwidthMap.Clear();
}

void SSWR::AVIRead::AVIRBandwidthLogForm::ClearJMeterLog()
{
	this->jmeterList.FreeAll(JMeterThreadFree);
}

SSWR::AVIRead::AVIRBandwidthLogForm::AVIRBandwidthLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Bandwidth Log Analyst"));

	this->core = core;
	this->startTime = 0;
	this->endTime = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpFile = ui->NewGroupBox(*this, CSTR("File"));
	this->grpFile->SetRect(0, 0, 100, 63, false);
	this->grpFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblBandwidthLog = ui->NewLabel(this->grpFile, CSTR("Bandwidth Log"));
	this->lblBandwidthLog->SetRect(0, 0, 100, 23, false);
	this->txtBandwidthLog = ui->NewTextBox(this->grpFile, CSTR(""));
	this->txtBandwidthLog->SetRect(100, 0, 700, 23, false);
	this->txtBandwidthLog->SetReadOnly(true);
	this->lblJMeterLog = ui->NewLabel(this->grpFile, CSTR("JMeter Log"));
	this->lblJMeterLog->SetRect(0, 24, 100, 23, false);
	this->txtJMeterLog = ui->NewTextBox(this->grpFile, CSTR(""));
	this->txtJMeterLog->SetRect(100, 24, 700, 23, false);
	this->txtJMeterLog->SetReadOnly(true);

	this->grpDetail = ui->NewGroupBox(*this, CSTR("Detail"));
	this->grpDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblStartTime = ui->NewLabel(this->grpDetail, CSTR("Start Time"));
	this->lblStartTime->SetRect(0, 0, 100, 23, false);
	this->txtStartTime = ui->NewTextBox(this->grpDetail, CSTR(""));
	this->txtStartTime->SetRect(100, 0, 150, 23, false);
	this->txtStartTime->SetReadOnly(true);
	this->lblEndTime = ui->NewLabel(this->grpDetail, CSTR("End Time"));
	this->lblEndTime->SetRect(0, 24, 100, 23, false);
	this->txtEndTime = ui->NewTextBox(this->grpDetail, CSTR(""));
	this->txtEndTime->SetRect(100, 24, 150, 23, false);
	this->txtEndTime->SetReadOnly(true);
	this->lblJMeterThreads = ui->NewLabel(this->grpDetail, CSTR("JMeter Threads"));
	this->lblJMeterThreads->SetRect(0, 48, 100, 23, false);
	this->txtJMeterThreads = ui->NewTextBox(this->grpDetail, CSTR(""));
	this->txtJMeterThreads->SetRect(100, 48, 150, 23, false);
	this->txtJMeterThreads->SetReadOnly(true);
	this->btnJMeterExport = ui->NewButton(this->grpDetail, CSTR("Thread"));
	this->btnJMeterExport->SetRect(250, 48, 75, 23, false);
	this->btnJMeterExport->HandleButtonClick(OnJMeterExportClicked, this);
	this->lblIP = ui->NewLabel(this->grpDetail, CSTR("IP"));
	this->lblIP->SetRect(0, 72, 100, 23, false);
	this->cboIP = ui->NewComboBox(this->grpDetail, false);
	this->cboIP->SetRect(100, 72, 150, 23, false);
	this->btnExport = ui->NewButton(this->grpDetail, CSTR("Export"));
	this->btnExport->SetRect(100, 96, 75, 23, false);
	this->btnExport->HandleButtonClick(OnExportClicked, this);

	this->HandleDropFiles(DropFilesHandler, this);
}

SSWR::AVIRead::AVIRBandwidthLogForm::~AVIRBandwidthLogForm()
{
	this->ClearBandwidthLog();
	this->ClearJMeterLog();
}

void SSWR::AVIRead::AVIRBandwidthLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
