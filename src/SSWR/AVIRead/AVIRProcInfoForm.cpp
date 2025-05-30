#include "Stdafx.h"
#include "Manage/Process.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRProcInfoForm.h"
#include "SSWR/AVIRead/AVIRThreadInfoForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnSumDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	me->lbDetail->SetSelectedIndex(index);
	me->tcMain->SetSelectedIndex(1);
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnProcSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	ProcessInfo *procInfo = (ProcessInfo*)me->lbDetail->GetSelectedItem().p;
	me->currProcRes.Delete();
	me->currProcObj.Delete();
	if (procInfo == 0)
	{
		me->txtDetProcId->SetText(CSTR(""));
		me->txtDetParentId->SetText(CSTR(""));
		me->txtDetName->SetText(CSTR(""));
		me->txtDetPath->SetText(CSTR(""));
		me->txtDetCmdLine->SetText(CSTR(""));
		me->txtDetWorkingDir->SetText(CSTR(""));
		me->txtDetTrueProgPath->SetText(CSTR(""));
		me->txtDetPriority->SetText(CSTR(""));
		me->txtDetArchitecture->SetText(CSTR(""));
		me->currProc = 0;
		me->rlcDetChartCPU->ClearChart();
		me->rlcDetChartCount->ClearChart();
		me->rlcDetChartWS->ClearChart();
		me->rlcDetChartPage->ClearChart();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->currProc = procInfo->procId;
		NN<Manage::Process> procObj;
		NEW_CLASSNN(procObj, Manage::Process(procInfo->procId, false));
		me->currProcObj = procObj;
		NEW_CLASSOPT(me->currProcRes, Manage::SymbolResolver(procObj));
		Manage::Process proc(procInfo->procId, false);
		sb.AppendU32(procInfo->procId);
		me->txtDetProcId->SetText(sb.ToCString());
		sb.ClearStr();
		sb.AppendU32(procInfo->parentProcId);
		me->txtDetParentId->SetText(sb.ToCString());
		me->txtDetName->SetText(procInfo->procName->ToCString());
		sb.ClearStr();
		proc.GetFilename(sb);
		me->txtDetPath->SetText(sb.ToCString());
		sb.ClearStr();
		proc.GetCommandLine(sb);
		me->txtDetCmdLine->SetText(sb.ToCString());
		sb.ClearStr();
		proc.GetWorkingDir(sb);
		me->txtDetWorkingDir->SetText(sb.ToCString());
		sb.ClearStr();
		proc.GetTrueProgramPath(sb);
		me->txtDetTrueProgPath->SetText(sb.ToCString());
		me->txtDetPriority->SetText(Manage::Process::GetPriorityName(proc.GetPriority()));
		me->txtDetArchitecture->SetText(Manage::ThreadContext::ContextTypeGetName(proc.GetContextType()));
		me->UpdateProcHeaps();
		me->UpdateProcModules();
		me->UpdateProcThreads();

		me->rlcDetChartCPU->ClearChart();
		me->rlcDetChartCount->ClearChart();
		me->rlcDetChartWS->ClearChart();
		me->rlcDetChartPage->ClearChart();
		proc.GetTimeInfo(0, &me->lastKernelTime, &me->lastUserTime);
		me->clk.Start();
	}
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	UTF8Char sbuff[512];
	UTF8Char sbuff2[12];
	UnsafeArray<UTF8Char> sptr;
	NN<ProcessInfo> procInfo;
	Manage::Process::ProcessInfo proc;
	UOSInt i;
	OSInt si;
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess(CSTR_NULL);
	if (sess)
	{
		i = me->procList.GetCount();
		while (i-- > 0)
		{
			procInfo = me->procList.GetItemNoCheck(i);
			procInfo->found = false;
		}

		while (Manage::Process::FindProcessNext(sbuff, sess, &proc).SetTo(sptr))
		{
			si = me->procIds.SortedIndexOf(proc.processId);
			if (si >= 0)
			{
				procInfo = me->procList.GetItemNoCheck((UOSInt)si);
				procInfo->found = true;
			}
			else
			{
				i = (UOSInt)~si;
				procInfo = MemAllocNN(ProcessInfo);
				procInfo->found = true;
				procInfo->procId = proc.processId;
				procInfo->parentProcId = proc.parentId;
				procInfo->procName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				me->procIds.Insert(i, procInfo->procId);
				me->procList.Insert(i, procInfo);
				sptr = Text::StrUInt32(sbuff2, procInfo->procId);
				me->lvSummary->InsertItem(i, CSTRP(sbuff2, sptr), procInfo);
				me->lvSummary->SetSubItem(i, 1, procInfo->procName);
				sptr = procInfo->procName->ConcatTo(Text::StrConcatC(Text::StrUInt32(sbuff, procInfo->procId), UTF8STRC(" ")));
				me->lbDetail->InsertItem(i, CSTRP(sbuff, sptr), procInfo);
			}

			Manage::Process proc(procInfo->procId, false);
			UOSInt ws;
			UOSInt pageFault;
			UOSInt pagedPool;
			UOSInt nonPagedPool;
			UOSInt pageFile;
			if (proc.GetMemoryInfo(&pageFault, &ws, &pagedPool, &nonPagedPool, &pageFile))
			{
				sptr = Text::StrUOSIntS(sbuff, ws, ',', 3);
				me->lvSummary->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSIntS(sbuff, pageFault, ',', 3);
				me->lvSummary->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSIntS(sbuff, pagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSIntS(sbuff, nonPagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSIntS(sbuff, pageFile, ',', 3);
				me->lvSummary->SetSubItem(i, 6, CSTRP(sbuff, sptr));

				sptr = Text::StrUInt32(sbuff, proc.GetGDIObjCount());
				me->lvSummary->SetSubItem(i, 7, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, proc.GetUserObjCount());
				me->lvSummary->SetSubItem(i, 8, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, proc.GetHandleCount());
				me->lvSummary->SetSubItem(i, 9, CSTRP(sbuff, sptr));
			}
			else
			{
				me->lvSummary->SetSubItem(i, 2, CSTR("-"));
				me->lvSummary->SetSubItem(i, 3, CSTR("-"));
				me->lvSummary->SetSubItem(i, 4, CSTR("-"));
				me->lvSummary->SetSubItem(i, 5, CSTR("-"));
				me->lvSummary->SetSubItem(i, 6, CSTR("-"));
				me->lvSummary->SetSubItem(i, 7, CSTR("-"));
				me->lvSummary->SetSubItem(i, 8, CSTR("-"));
				me->lvSummary->SetSubItem(i, 9, CSTR("-"));
			}
		}
		Manage::Process::FindProcessClose(sess);

		i = me->procList.GetCount();
		while (i-- > 0)
		{
			procInfo = me->procList.GetItemNoCheck(i);
			if (!procInfo->found)
			{
				procInfo->procName->Release();
				MemFreeNN(procInfo);
				me->lvSummary->RemoveItem(i);
				me->lbDetail->RemoveItem(i);
				me->procList.RemoveAt(i);
				me->procIds.RemoveAt(i);
			}
		}
	}

}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnTimerCPUTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	if (me->currProc != 0)
	{
		Data::Timestamp kernelTime;
		Data::Timestamp userTime;
		Double t;
		Double v[3];
		Manage::Process proc(me->currProc, false);
		if (proc.GetTimeInfo(0, &kernelTime, &userTime))
		{
			t = me->clk.GetAndRestart();
			if (t > 0)
			{
				v[0] = (Double)(kernelTime.DiffMS(me->lastKernelTime) + userTime.DiffMS(me->lastUserTime)) / t / 10.0 / UOSInt2Double(me->threadCnt);
				me->rlcDetChartCPU->AddSample(v);
				me->lastKernelTime = kernelTime;
				me->lastUserTime = userTime;
			}
		}
		UOSInt workingSet;
		UOSInt pagePool;
		UOSInt nonPagePool;
		UOSInt pageFile;
		if (proc.GetMemoryInfo(0, &workingSet, &pagePool, &nonPagePool, &pageFile))
		{
			v[0] = UOSInt2Double(pagePool);
			v[1] = UOSInt2Double(nonPagePool);
			me->rlcDetChartPage->AddSample(v);
			v[0] = UOSInt2Double(workingSet);
			v[1] = UOSInt2Double(pageFile);
			me->rlcDetChartWS->AddSample(v);
		}
		v[0] = proc.GetGDIObjCount();
		v[1] = proc.GetUserObjCount();
		v[2] = proc.GetHandleCount();
		me->rlcDetChartCount->AddSample(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetailCurrClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	OSInt i = me->procIds.SortedIndexOf((UInt32)Manage::Process::GetCurrProcId());
	if (i >= 0)
	{
		me->lbDetail->SetSelectedIndex((UOSInt)i);
	}
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetModuleRefClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	me->UpdateProcModules();
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetThreadRefClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	me->UpdateProcThreads();
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetThreadDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	NN<Manage::Process> currProcObj;
	NN<Manage::SymbolResolver> currProcRes;
	if (me->currProcObj.SetTo(currProcObj) && me->currProcRes.SetTo(currProcRes))
	{
		UInt32 threadId = (UInt32)me->lvDetThread->GetItem(index).GetUOSInt();
		SSWR::AVIRead::AVIRThreadInfoForm frm(0, me->ui, me->core, currProcObj, currProcRes, threadId);
		frm.ShowDialog(me);
	}
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHeapRefClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	me->UpdateProcHeaps();
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHeapSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	UOSInt id = (UOSInt)me->lbDetHeap->GetSelectedItem().p;
	me->UpdateProcHeapDetail((UInt32)id);
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHeapItemSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	UOSInt i = me->lvDetHeap->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	Text::StringBuilderUTF8 sb;
	UOSInt addr = me->lvDetHeap->GetItem((UOSInt)i).GetUOSInt();
	UOSInt size;
	me->lvDetHeap->GetSubItem((UOSInt)i, 1, sb);
	size = 0;
	sb.ToUOSInt(size);
	Manage::Process proc(me->currProc, false);
	UInt8 buff[512];
	if (size <= 512)
	{
		sb.ClearStr();
		size = proc.ReadMemory(addr, buff, size);
		sb.AppendHexBuff(buff, size, ' ', Text::LineBreakType::CRLF);
		me->txtDetHeap->SetText(sb.ToCString());
	}
	else
	{
		UOSInt size2;
		sb.ClearStr();
		size2 = proc.ReadMemory(addr, buff, 256);
		sb.AppendHexBuff(buff, size2, ' ', Text::LineBreakType::CRLF);
		sb.AppendC(UTF8STRC(".."));
		size2 = proc.ReadMemory(addr + size - 256, buff, 256);
		sb.AppendHexBuff(buff, size2, ' ', Text::LineBreakType::CRLF);
		me->txtDetHeap->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHandleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProcInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProcInfoForm>();
	me->UpdateProcHandles();
}

void SSWR::AVIRead::AVIRProcInfoForm::UpdateProcModules()
{
	if (this->currProc == 0)
	{
		this->lvDetModule->ClearItems();
	}
	else
	{
		Manage::Process proc(this->currProc, false);
		Data::ArrayListNN<Manage::ModuleInfo> modList;
		NN<Manage::ModuleInfo> module;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UOSInt k;
		UOSInt addr;
		UOSInt size;

		proc.GetModules(modList);

		this->lvDetModule->ClearItems();
		Data::ArrayIterator<NN<Manage::ModuleInfo>> it = modList.Iterator();
		while (it.HasNext())
		{
			module = it.Next();
			sptr = module->GetModuleFileName(sbuff);
			k = this->lvDetModule->AddItem(CSTRP(sbuff, sptr), 0);
			if (module->GetModuleAddress(addr, size))
			{
				sptr = Text::StrHexValOS(sbuff, addr);
				this->lvDetModule->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrHexValOS(sbuff, size);
				this->lvDetModule->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			}
			module.Delete();
		}
	}
}

void SSWR::AVIRead::AVIRProcInfoForm::UpdateProcThreads()
{
	if (this->currProc == 0)
	{
		this->lvDetModule->ClearItems();
	}
	else
	{
		Manage::Process proc(this->currProc, false);
		Data::ArrayListNN<Manage::ThreadInfo> threadList;
		NN<Manage::ThreadInfo> t;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		UInt64 addr;
		NN<Manage::SymbolResolver> currProcRes;

		proc.GetThreads(threadList);
		this->lvDetThread->ClearItems();
		i = 0;
		j = threadList.GetCount();
		while (i < j)
		{
			t = threadList.GetItemNoCheck(i);
			sptr = Text::StrUOSInt(sbuff, t->GetThreadId());
			k = this->lvDetThread->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)t->GetThreadId(), 0);
			if (t->GetName(sbuff).SetTo(sptr))
			{
				this->lvDetThread->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			}
			addr = t->GetStartAddress();
			sptr = Text::StrHexVal64(sbuff, addr);
			this->lvDetThread->SetSubItem(k, 2, CSTRP(sbuff, sptr));

			if (this->currProcRes.SetTo(currProcRes))
			{
				if (currProcRes->ResolveName(sbuff, addr).SetTo(sptr))
				{
					l = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
					this->lvDetThread->SetSubItem(k, 3, CSTRP(&sbuff[l + 1], sptr));
				}
			}
			t.Delete();
			i++;
		}
	}	
}

void SSWR::AVIRead::AVIRProcInfoForm::UpdateProcHeaps()
{
	this->lvDetHeap->ClearItems();
	if (this->currProc == 0)
	{
		this->lbDetHeap->ClearItems();
	}
	else
	{
		Manage::Process proc(this->currProc, false);
		Data::ArrayListUInt32 heapList;
		UTF8Char sbuff[20];
		UnsafeArray<UTF8Char> sptr;
		UOSInt i;
		UOSInt j;

		proc.GetHeapLists(heapList);

		this->lbDetHeap->ClearItems();
		i = 0;
		j = heapList.GetCount();
		while (i < j)
		{
			sptr = Text::StrUInt32(sbuff, heapList.GetItem(i));
			this->lbDetHeap->AddItem(CSTRP(sbuff, sptr), (void*)(UOSInt)heapList.GetItem(i));
			i++;
		}
	}
}

void SSWR::AVIRead::AVIRProcInfoForm::UpdateProcHeapDetail(UInt32 heapId)
{
	if (this->currProc == 0)
	{
		this->lvDetHeap->ClearItems();
	}
	else if (this->currProc == Manage::Process::GetCurrProcId())
	{
		this->lvDetHeap->ClearItems();
		this->lvDetHeap->AddItem(CSTR("Showing heap of current process is not allowed"), 0, 0);
	}
	else
	{
		Manage::Process proc(this->currProc, false);
		Data::ArrayListNN<Manage::Process::HeapInfo> heapList;
		NN<Manage::Process::HeapInfo> heap;
		UTF8Char sbuff[20];
		UnsafeArray<UTF8Char> sptr;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		Text::CStringNN tStr;

		proc.GetHeaps(heapList, heapId, 257);

		this->lvDetHeap->ClearItems();
		i = 0;
		j = heapList.GetCount();
		while (i < j)
		{
			heap = heapList.GetItemNoCheck(i);
			sptr = Text::StrHexValOS(Text::StrConcatC(sbuff, UTF8STRC("0x")), heap->startAddr);
			k = this->lvDetHeap->AddItem(CSTRP(sbuff, sptr), (void*)heap->startAddr, 0);
			sptr = Text::StrUOSInt(sbuff, heap->size);
			this->lvDetHeap->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			switch (heap->heapType)
			{
			case Manage::Process::HT_FIXED:
				tStr = CSTR("Fixed");
				break;
			case Manage::Process::HT_MOVABLE:
				tStr = CSTR("Movable");
				break;
			case Manage::Process::HT_FREE:
				tStr = CSTR("Free");
				break;
			case Manage::Process::HT_UNKNOWN:
			default:
				tStr = CSTR("Unknown");
				break;
			}
			this->lvDetHeap->SetSubItem(k, 2, tStr);
			i++;
			if (i > 256)
				break;
		}
		proc.FreeHeaps(heapList);
	}
	
}

void SSWR::AVIRead::AVIRProcInfoForm::UpdateProcHandles()
{
	if (this->currProc == 0)
	{
		this->lvDetHandle->ClearItems();
	}
	else
	{
		Manage::Process proc(this->currProc, false);
		Data::ArrayList<Manage::Process::HandleInfo> handleList;
		Manage::Process::HandleInfo hinfo;
		UTF8Char sbuff[20];
		UnsafeArray<UTF8Char> sptr;
		Manage::HandleType handleType;
		Text::StringBuilderUTF8 sb;
		UOSInt i;
		UOSInt j;

		proc.GetHandles(handleList);

		this->lvDetHandle->ClearItems();
		i = 0;
		j = handleList.GetCount();
		while (i < j)
		{
			hinfo = handleList.GetItem(i);
			sptr = Text::StrInt32(sbuff, hinfo.id);
			this->lvDetHandle->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)hinfo.id);
			if (!hinfo.createTime.IsNull())
			{
				sptr = hinfo.createTime.ToLocalTime().ToStringNoZone(sbuff);
				this->lvDetHandle->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			}
			sb.ClearStr();
			if (proc.GetHandleDetail(hinfo.id, handleType, sb))
			{
				this->lvDetHandle->SetSubItem(i, 2, Manage::HandleTypeGetName(handleType));
				this->lvDetHandle->SetSubItem(i, 3, sb.ToCString());
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRProcInfoForm::AVIRProcInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Process Info"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->currProc = 0;
	this->currProcObj = 0;
	this->currProcRes = 0;
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSummary = this->tcMain->AddTabPage(CSTR("Summary"));
	this->pnlSummary = ui->NewPanel(this->tpSummary);
	this->pnlSummary->SetRect(0, 0, 100, 48, false);
	this->pnlSummary->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvSummary = ui->NewListView(this->tpSummary, UI::ListViewStyle::Table, 10);
	this->lvSummary->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSummary->SetFullRowSelect(true);
	this->lvSummary->SetShowGrid(true);
	this->lvSummary->HandleDblClk(OnSumDblClicked, this);
	this->lvSummary->AddColumn(CSTR("Id"), 60);
	this->lvSummary->AddColumn(CSTR("Process Name"), 120);
	this->lvSummary->AddColumn(CSTR("WS Size"), 80);
	this->lvSummary->AddColumn(CSTR("Page Fault"), 80);
	this->lvSummary->AddColumn(CSTR("Paged Pool"), 60);
	this->lvSummary->AddColumn(CSTR("Non-Paged Pool"), 60);
	this->lvSummary->AddColumn(CSTR("Page File"), 80);
	this->lvSummary->AddColumn(CSTR("GDI Objects"), 50);
	this->lvSummary->AddColumn(CSTR("User Objects"), 50);
	this->lvSummary->AddColumn(CSTR("Handles"), 50);

	this->tpDetail = this->tcMain->AddTabPage(CSTR("Detail"));
	this->pnlDetail = ui->NewPanel(this->tpDetail);
	this->pnlDetail->SetRect(0, 0, 200, 100, false);
	this->pnlDetail->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnDetailCurr = ui->NewButton(this->pnlDetail, CSTR("Current Process"));
	this->btnDetailCurr->SetRect(0, 0, 100, 23, false);
	this->btnDetailCurr->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetailCurr->HandleButtonClick(OnDetailCurrClicked, this);
	this->lbDetail = ui->NewListBox(this->pnlDetail, false);
	this->lbDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbDetail->HandleSelectionChange(OnProcSelChg, this);
	this->hspDetail = ui->NewHSplitter(this->tpDetail, 3, false);
	this->tcDetail = ui->NewTabControl(this->tpDetail);
	this->tcDetail->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDetInfo = this->tcDetail->AddTabPage(CSTR("Info"));
	this->lblDetProcId = ui->NewLabel(this->tpDetInfo, CSTR("Process Id"));
	this->lblDetProcId->SetRect(0, 0, 100, 23, false);
	this->txtDetProcId = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetProcId->SetRect(100, 0, 50, 23, false);
	this->txtDetProcId->SetReadOnly(true);
	this->lblDetParentId = ui->NewLabel(this->tpDetInfo, CSTR("Parent Id"));
	this->lblDetParentId->SetRect(0, 24, 100, 23, false);
	this->txtDetParentId = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetParentId->SetRect(100, 24, 50, 23, false);
	this->txtDetParentId->SetReadOnly(true);
	this->lblDetName = ui->NewLabel(this->tpDetInfo, CSTR("Process Name"));
	this->lblDetName->SetRect(0, 48, 100, 23, false);
	this->txtDetName = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetName->SetRect(100, 48, 200, 23, false);
	this->txtDetName->SetReadOnly(true);
	this->lblDetPath = ui->NewLabel(this->tpDetInfo, CSTR("Process Path"));
	this->lblDetPath->SetRect(0, 72, 100, 23, false);
	this->txtDetPath = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetPath->SetRect(100, 72, 700, 23, false);
	this->txtDetPath->SetReadOnly(true);
	this->lblDetCmdLine = ui->NewLabel(this->tpDetInfo, CSTR("Command Line"));
	this->lblDetCmdLine->SetRect(0, 96, 100, 23, false);
	this->txtDetCmdLine = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetCmdLine->SetRect(100, 96, 700, 23, false);
	this->txtDetCmdLine->SetReadOnly(true);
	this->lblDetWorkingDir = ui->NewLabel(this->tpDetInfo, CSTR("Working Dir"));
	this->lblDetWorkingDir->SetRect(0, 120, 100, 23, false);
	this->txtDetWorkingDir = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetWorkingDir->SetRect(100, 120, 700, 23, false);
	this->txtDetWorkingDir->SetReadOnly(true);
	this->lblDetTrueProgPath = ui->NewLabel(this->tpDetInfo, CSTR("True Prog Path"));
	this->lblDetTrueProgPath->SetRect(0, 144, 100, 23, false);
	this->txtDetTrueProgPath = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetTrueProgPath->SetRect(100, 144, 700, 23, false);
	this->txtDetTrueProgPath->SetReadOnly(true);
	this->lblDetPriority = ui->NewLabel(this->tpDetInfo, CSTR("Priority"));
	this->lblDetPriority->SetRect(0, 168, 100, 23, false);
	this->txtDetPriority = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetPriority->SetRect(100, 168, 100, 23, false);
	this->txtDetPriority->SetReadOnly(true);
	this->lblDetArchitecture = ui->NewLabel(this->tpDetInfo, CSTR("Architecture"));
	this->lblDetArchitecture->SetRect(0, 192, 100, 23, false);
	this->txtDetArchitecture = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetArchitecture->SetRect(100, 192, 100, 23, false);
	this->txtDetArchitecture->SetReadOnly(true);

	this->tpDetModule = this->tcDetail->AddTabPage(CSTR("Module"));
	this->pnlDetModule = ui->NewPanel(this->tpDetModule);
	this->pnlDetModule->SetRect(0, 0, 100, 31, false);
	this->pnlDetModule->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetModule = ui->NewButton(this->pnlDetModule, CSTR("Refresh"));
	this->btnDetModule->SetRect(4, 4, 75, 23, false);
	this->btnDetModule->HandleButtonClick(OnDetModuleRefClicked, this);
	this->lvDetModule = ui->NewListView(this->tpDetModule, UI::ListViewStyle::Table, 3);
	this->lvDetModule->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetModule->SetFullRowSelect(true);
	this->lvDetModule->SetShowGrid(true);
	this->lvDetModule->AddColumn(CSTR("Name"), 600);
	this->lvDetModule->AddColumn(CSTR("Address"), 80);
	this->lvDetModule->AddColumn(CSTR("Size"), 80);

	this->tpDetThread = this->tcDetail->AddTabPage(CSTR("Thread"));
	this->pnlDetThread = ui->NewPanel(this->tpDetThread);
	this->pnlDetThread->SetRect(0, 0, 100, 31, false);
	this->pnlDetThread->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetThread = ui->NewButton(this->pnlDetThread, CSTR("Refresh"));
	this->btnDetThread->SetRect(4, 4, 75, 23, false);
	this->btnDetThread->HandleButtonClick(OnDetThreadRefClicked, this);
	this->lvDetThread = ui->NewListView(this->tpDetThread, UI::ListViewStyle::Table, 4);
	this->lvDetThread->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetThread->HandleDblClk(OnDetThreadDblClicked, this);
	this->lvDetThread->SetFullRowSelect(true);
	this->lvDetThread->SetShowGrid(true);
	this->lvDetThread->AddColumn(CSTR("Id"), 60);
	this->lvDetThread->AddColumn(CSTR("Name"), 100);
	this->lvDetThread->AddColumn(CSTR("Start Address"), 120);
	this->lvDetThread->AddColumn(CSTR("Start Address(Name)"), 600);

	this->tpDetHeap = this->tcDetail->AddTabPage(CSTR("Heap"));
	this->pnlDetHeap = ui->NewPanel(this->tpDetHeap);
	this->pnlDetHeap->SetRect(0, 0, 100, 31, false);
	this->pnlDetHeap->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetHeap = ui->NewButton(this->pnlDetHeap, CSTR("Refresh"));
	this->btnDetHeap->SetRect(4, 4, 75, 23, false);
	this->btnDetHeap->HandleButtonClick(OnDetHeapRefClicked, this);
	this->lbDetHeap = ui->NewListBox(this->tpDetHeap, false);
	this->lbDetHeap->SetRect(0, 0, 100, 23, false);
	this->lbDetHeap->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDetHeap->HandleSelectionChange(OnDetHeapSelChg, this);
	this->hspDetHeap = ui->NewHSplitter(this->tpDetHeap, 3, false);
	this->txtDetHeap = ui->NewTextBox(this->tpDetHeap, CSTR(""), true);
	this->txtDetHeap->SetReadOnly(true);
	this->txtDetHeap->SetRect(0, 0, 100, 128, false);
	this->txtDetHeap->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvDetHeap = ui->NewListView(this->tpDetHeap, UI::ListViewStyle::Table, 3);
	this->lvDetHeap->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetHeap->SetFullRowSelect(true);
	this->lvDetHeap->SetShowGrid(true);
	this->lvDetHeap->AddColumn(CSTR("Start Address"), 160);
	this->lvDetHeap->AddColumn(CSTR("Size"), 60);
	this->lvDetHeap->AddColumn(CSTR("Type"), 80);
	this->lvDetHeap->HandleSelChg(OnDetHeapItemSelChg, this);

	this->tpDetHandle = this->tcDetail->AddTabPage(CSTR("Handles"));
	this->pnlDetHandle = ui->NewPanel(this->tpDetHandle);
	this->pnlDetHandle->SetRect(0, 0, 100, 31, false);
	this->pnlDetHandle->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetHandle = ui->NewButton(this->pnlDetHandle, CSTR("Refresh"));
	this->btnDetHandle->SetRect(4, 4, 75, 23, false);
	this->btnDetHandle->HandleButtonClick(OnDetHandleClicked, this);
	this->lvDetHandle = ui->NewListView(this->tpDetHandle, UI::ListViewStyle::Table, 4);
	this->lvDetHandle->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetHandle->SetFullRowSelect(true);
	this->lvDetHandle->SetShowGrid(true);
	this->lvDetHandle->AddColumn(CSTR("Id"), 60);
	this->lvDetHandle->AddColumn(CSTR("Create Time"), 150);
	this->lvDetHandle->AddColumn(CSTR("Type"), 100);
	this->lvDetHandle->AddColumn(CSTR("Desc"), 300);

	this->tpDetChart = this->tcDetail->AddTabPage(CSTR("Chart"));
	this->grpDetChartCPU = ui->NewGroupBox(this->tpDetChart, CSTR("CPU"));
	this->grpDetChartCPU->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCPU->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcDetChartCPU = ui->NewRealtimeLineChart(this->grpDetChartCPU, this->core->GetDrawEngine(), 1, 600, 300, 0);
	this->rlcDetChartCPU->SetDockType(UI::GUIControl::DOCK_FILL);
	this->rlcDetChartCPU->SetUnit(CSTR("%"));
	this->vspDetChartCPU = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartPage = ui->NewGroupBox(this->tpDetChart, CSTR("Paged(R)/Non-Paged(B) Pool"));
	this->grpDetChartPage->SetRect(0, 0, 100, 200, false);
	this->grpDetChartPage->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcDetChartPage = ui->NewRealtimeLineChart(this->grpDetChartPage, this->core->GetDrawEngine(), 2, 600, 300, 0);
	this->rlcDetChartPage->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspDetChartPage = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartCount = ui->NewGroupBox(this->tpDetChart, CSTR("GDI(R)/User(B)/Handle(G) Count"));
	this->grpDetChartCount->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCount->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcDetChartCount = ui->NewRealtimeLineChart(this->grpDetChartCount, this->core->GetDrawEngine(), 3, 600, 300, 0);
	this->rlcDetChartCount->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspDetChartCount = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartWS = ui->NewGroupBox(this->tpDetChart, CSTR("WS(R)/Page File(B)"));
	this->grpDetChartWS->SetRect(0, 0, 100, 200, false);
	this->grpDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->rlcDetChartWS = ui->NewRealtimeLineChart(this->grpDetChartWS, this->core->GetDrawEngine(), 2, 600, 300, 0);
	this->rlcDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(1000, OnTimerTick, this);
	this->AddTimer(300, OnTimerCPUTick, this);
	OnTimerTick(this);
}

SSWR::AVIRead::AVIRProcInfoForm::~AVIRProcInfoForm()
{
	NN<ProcessInfo> procInfo;
	UOSInt i;
	i = this->procList.GetCount();
	while (i-- > 0)
	{
		procInfo = this->procList.GetItemNoCheck(i);
		procInfo->procName->Release();
		MemFreeNN(procInfo);
	}
	this->currProcRes.Delete();
	this->currProcObj.Delete();
}

void SSWR::AVIRead::AVIRProcInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
