#include "Stdafx.h"
#include "Data/Comparator.hpp"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/CSVFile.h"
#include "DB/TableDef.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRBandwidthLogForm.h"
#include "Text/UTF8Reader.h"
#include "UI/GUIFileDialog.h"

class AVIRBandwidthLogFormComparator : public Data::Comparator<NN<SSWR::AVIRead::AVIRBandwidthLogForm::ThreadStatus>>
{
public:
	AVIRBandwidthLogFormComparator(){}
	virtual ~AVIRBandwidthLogFormComparator(){}

	virtual OSInt Compare(NN<SSWR::AVIRead::AVIRBandwidthLogForm::ThreadStatus> a, NN<SSWR::AVIRead::AVIRBandwidthLogForm::ThreadStatus> b) const
	{
		if (a->startTime > b->startTime)
			return 1;
		else if (a->startTime < b->startTime)
			return -1;
		return 0;
	}
};

void __stdcall SSWR::AVIRead::AVIRBandwidthLogForm::ThreadStatusFree(NN<ThreadStatus> thread)
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
		if (IO::Path::GetPathType(file->ToCString()) == IO::Path::PathType::Directory)
		{
			me->LoadSIDELog(file->ToCString());
		}
		else if (file->Substring(k + 1).StartsWith(CSTR("jmeter")) && file->Substring(k + 1).IndexOf(CSTR(".log")) != INVALID_INDEX)
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
				Data::ArrayListNN<ThreadStatus> allThreads;
				Data::ArrayListNN<ThreadStatus> threads;
				IO::BufferedOutputStream stm(fs, 8192);
				stm.Write(CSTR("Time,RecvBytes,SendBytes,Mbps,Concurr\r\n").ToByteArray());
				Text::StringBuilderUTF8 sb;
				Int64 currTime = me->startTime;
				Int64 endTime = me->endTime;
				NN<ThreadStatus> thread;
				UOSInt threadI;
				UOSInt threadJ;
				allThreads.AddAll(me->jmeterThreadList);
				allThreads.AddAll(me->sideThreadList);
				AVIRBandwidthLogFormComparator comparator;
				Data::Sort::ArtificialQuickSort::Sort<NN<ThreadStatus>>(allThreads, comparator);

				threadI = 0;
				threadJ = allThreads.GetCount();
				while (threadI < threadJ)
				{
					thread = allThreads.GetItemNoCheck(threadI);
					if ((thread->startTime / 1000) >= currTime)
					{
						break;
					}
					if ((thread->endTime / 1000) >= currTime)
						threads.Add(thread);
					threadI++;
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
					while (threadI < threadJ)
					{
						thread = allThreads.GetItemNoCheck(threadI);
						if ((thread->startTime / 1000) > currTime)
						{
							break;
						}
						threads.Add(thread);
						threadI++;
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

void __stdcall SSWR::AVIRead::AVIRBandwidthLogForm::OnThreadsExportClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBandwidthLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBandwidthLogForm>();
	if (me->jmeterThreadList.GetCount() > 0 || me->sideThreadList.GetCount() > 0)
	{
		Text::StringBuilderUTF8 sb;
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"BandwidthLogThreads", true);
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
				NN<ThreadStatus> thread;
				IO::BufferedOutputStream stm(fs, 8192);
				stm.Write(CSTR("Name,Start Time,End Time,Dur(second)\r\n").ToByteArray());
				NN<Text::String> s;
				UOSInt i = 0;
				UOSInt j = me->jmeterThreadList.GetCount();
				while (i < j)
				{
					thread = me->jmeterThreadList.GetItemNoCheck(i);
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
				i = 0;
				j = me->sideThreadList.GetCount();
				while (i < j)
				{
					thread = me->sideThreadList.GetItemNoCheck(i);
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
			if (valid && csv.QueryTableData(0, fileName, nullptr, 0, 0, 0, nullptr).SetTo(r))
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
	Data::ArrayListNN<ThreadStatus> logList;
	Data::FastStringMapNN<ThreadStatus> threadMap;
	Text::StringBuilderUTF8 sb;
	Text::CStringNN logContent;
	Text::CStringNN threadName;
	Data::Timestamp ts;
	NN<ThreadStatus> log;
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
						log = MemAllocNN(ThreadStatus);
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
		this->jmeterThreadList.AddAll(logList);
		sb.ClearStr();
		sb.AppendUOSInt(logList.GetCount() + this->sideThreadList.GetCount());
		this->txtThreadsCount->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRBandwidthLogForm::LoadSIDELog(Text::CStringNN filePath)
{
	Data::ArrayListNN<ThreadStatus> logList;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr3;
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;
	NN<DB::DBReader> r;
	Data::Timestamp startTime;
	Data::Timestamp endTime;
	Data::Timestamp currTime;
	NN<ThreadStatus> thread;
	Double dur;
	sptr = filePath.ConcatTo(sbuff);
	if (filePath.leng > 0 && sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.csv"));
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2) && pt == IO::Path::PathType::File)
		{
			DB::CSVFile csv(CSTRP(sbuff, sptr2), 65001);
			if (csv.QueryTableData(0, CSTR(""), nullptr, 0, 0, 0, nullptr).SetTo(r))
			{
				startTime = 0;
				endTime = 0;
				Bool valid = true;
				if (!r->GetName(0, sbuff2).SetTo(sptr3) || !CSTRP(sbuff2, sptr3).Equals(CSTR("Time"))) valid = false;
				if (!r->GetName(1, sbuff2).SetTo(sptr3) || !CSTRP(sbuff2, sptr3).Equals(CSTR("TestId"))) valid = false;
				if (!r->GetName(2, sbuff2).SetTo(sptr3) || !CSTRP(sbuff2, sptr3).Equals(CSTR("Step"))) valid = false;
				if (!r->GetName(3, sbuff2).SetTo(sptr3) || !CSTRP(sbuff2, sptr3).Equals(CSTR("Duration"))) valid = false;
				if (valid)
				{
					while (r->ReadNext())
					{
						currTime = r->GetTimestamp(0);
						dur = r->GetDblOrNAN(3);
						if (!currTime.IsNull() && !Math::IsNAN(dur))
						{
							if (startTime.IsNull())
							{
								startTime = currTime;
								endTime = startTime.AddSecondDbl(dur);
							}
							else
							{
								if (startTime > currTime) startTime = currTime;
								if (endTime.DiffSecDbl(currTime) < dur) endTime = currTime.AddSecondDbl(dur);
							}
						}
					}
				}
				csv.CloseReader(r);
				if (!startTime.IsNull())
				{
					thread = MemAllocNN(ThreadStatus);
					thread->name = Text::String::NewP(sptr, sptr2);
					thread->startTime = startTime.ToTicks();
					thread->endTime = endTime.ToTicks();
					logList.Add(thread);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	if (logList.GetCount() > 0)
	{
		this->ClearSIDELog();
		this->txtSIDELog->SetText(filePath);
		this->sideThreadList.AddAll(logList);
		sb.ClearStr();
		sb.AppendUOSInt(logList.GetCount() + this->jmeterThreadList.GetCount());
		this->txtThreadsCount->SetText(sb.ToCString());
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
	this->jmeterThreadList.FreeAll(ThreadStatusFree);
}

void SSWR::AVIRead::AVIRBandwidthLogForm::ClearSIDELog()
{
	this->sideThreadList.FreeAll(ThreadStatusFree);
}

SSWR::AVIRead::AVIRBandwidthLogForm::AVIRBandwidthLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Bandwidth Log Analyst"));

	this->core = core;
	this->startTime = 0;
	this->endTime = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpFile = ui->NewGroupBox(*this, CSTR("File"));
	this->grpFile->SetRect(0, 0, 100, 91, false);
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
	this->lblSIDELog = ui->NewLabel(this->grpFile, CSTR("SIDERunner Log"));
	this->lblSIDELog->SetRect(0, 48, 100, 23, false);
	this->txtSIDELog = ui->NewTextBox(this->grpFile, CSTR(""));
	this->txtSIDELog->SetRect(100, 48, 700, 23, false);
	this->txtSIDELog->SetReadOnly(true);

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
	this->lblThreadsCount = ui->NewLabel(this->grpDetail, CSTR("Threads Count"));
	this->lblThreadsCount->SetRect(0, 48, 100, 23, false);
	this->txtThreadsCount = ui->NewTextBox(this->grpDetail, CSTR(""));
	this->txtThreadsCount->SetRect(100, 48, 150, 23, false);
	this->txtThreadsCount->SetReadOnly(true);
	this->btnThreadsExport = ui->NewButton(this->grpDetail, CSTR("Save"));
	this->btnThreadsExport->SetRect(250, 48, 75, 23, false);
	this->btnThreadsExport->HandleButtonClick(OnThreadsExportClicked, this);
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
	this->ClearSIDELog();
}

void SSWR::AVIRead::AVIRBandwidthLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
