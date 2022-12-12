#include "Stdafx.h"
#include "Manage/Process.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRProcInfoForm.h"
#include "SSWR/AVIRead/AVIRThreadInfoForm.h"
#include "Sync/Thread.h"

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnSumDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	me->lbDetail->SetSelectedIndex(index);
	me->tcMain->SetSelectedIndex(1);
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnProcSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	ProcessInfo *procInfo = (ProcessInfo*)me->lbDetail->GetSelectedItem();
	SDEL_CLASS(me->currProcRes);
	SDEL_CLASS(me->currProcObj);
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
		NEW_CLASS(me->currProcObj, Manage::Process(procInfo->procId, false));
		NEW_CLASS(me->currProcRes, Manage::SymbolResolver(me->currProcObj));
		Manage::Process proc(procInfo->procId, false);
		sb.AppendU32(procInfo->procId);
		me->txtDetProcId->SetText(sb.ToCString());
		sb.ClearStr();
		sb.AppendU32(procInfo->parentProcId);
		me->txtDetParentId->SetText(sb.ToCString());
		me->txtDetName->SetText(procInfo->procName->ToCString());
		sb.ClearStr();
		proc.GetFilename(&sb);
		me->txtDetPath->SetText(sb.ToCString());
		sb.ClearStr();
		proc.GetCommandLine(&sb);
		me->txtDetCmdLine->SetText(sb.ToCString());
		sb.ClearStr();
		proc.GetWorkingDir(&sb);
		me->txtDetWorkingDir->SetText(sb.ToCString());
		sb.ClearStr();
		proc.GetTrueProgramPath(&sb);
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

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[12];
	UTF8Char *sptr;
	ProcessInfo *procInfo;
	Manage::Process::ProcessInfo proc;
	UOSInt i;
	OSInt si;
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess(CSTR_NULL);
	if (sess)
	{
		i = me->procList.GetCount();
		while (i-- > 0)
		{
			procInfo = me->procList.GetItem(i);
			procInfo->found = false;
		}

		while ((sptr = Manage::Process::FindProcessNext(sbuff, sess, &proc)) != 0)
		{
			si = me->procIds.SortedIndexOf(proc.processId);
			if (si >= 0)
			{
				procInfo = me->procList.GetItem((UOSInt)si);
				procInfo->found = true;
			}
			else
			{
				i = (UOSInt)~si;
				procInfo = MemAlloc(ProcessInfo, 1);
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
			procInfo = me->procList.GetItem(i);
			if (!procInfo->found)
			{
				procInfo->procName->Release();
				MemFree(procInfo);
				me->lvSummary->RemoveItem(i);
				me->lbDetail->RemoveItem(i);
				me->procList.RemoveAt(i);
				me->procIds.RemoveAt(i);
			}
		}
	}

}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnTimerCPUTick(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
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

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetModuleRefClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	me->UpdateProcModules();
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetThreadRefClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	me->UpdateProcThreads();
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetThreadDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	UInt32 threadId = (UInt32)(UOSInt)me->lvDetThread->GetItem(index);
	SSWR::AVIRead::AVIRThreadInfoForm frm(0, me->ui, me->core, me->currProcObj, me->currProcRes, threadId);
	frm.ShowDialog(me);
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHeapRefClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	me->UpdateProcHeaps();
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHeapSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	UOSInt id = (UOSInt)me->lbDetHeap->GetSelectedItem();
	me->UpdateProcHeapDetail((UInt32)id);
}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHeapItemSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	UOSInt i = me->lvDetHeap->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	Text::StringBuilderUTF8 sb;
	UOSInt addr = (UOSInt)me->lvDetHeap->GetItem((UOSInt)i);
	UOSInt size;
	me->lvDetHeap->GetSubItem((UOSInt)i, 1, &sb);
	size = 0;
	sb.ToUOSInt(&size);
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

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnDetHandleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
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
		Data::ArrayList<Manage::ModuleInfo *> modList;
		Manage::ModuleInfo *module;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt addr;
		UOSInt size;

		proc.GetModules(&modList);

		this->lvDetModule->ClearItems();
		i = 0;
		j = modList.GetCount();
		while (i < j)
		{
			module = modList.GetItem(i);
			sptr = module->GetModuleFileName(sbuff);
			k = this->lvDetModule->AddItem(CSTRP(sbuff, sptr), 0);
			if (module->GetModuleAddress(&addr, &size))
			{
				sptr = Text::StrHexValOS(sbuff, addr);
				this->lvDetModule->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrHexValOS(sbuff, size);
				this->lvDetModule->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			}
			DEL_CLASS(module);
			i++;
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
		Data::ArrayList<Manage::ThreadInfo *> threadList;
		Manage::ThreadInfo *t;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		UInt64 addr;

		proc.GetThreads(&threadList);
		this->lvDetThread->ClearItems();
		i = 0;
		j = threadList.GetCount();
		while (i < j)
		{
			t = threadList.GetItem(i);
			sptr = Text::StrUOSInt(sbuff, t->GetThreadId());
			k = this->lvDetThread->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)t->GetThreadId(), 0);
			addr = t->GetStartAddress();
			sptr = Text::StrHexVal64(sbuff, addr);
			this->lvDetThread->SetSubItem(k, 1, CSTRP(sbuff, sptr));

			if (this->currProcRes)
			{
				sptr = this->currProcRes->ResolveName(sbuff, addr);
				if (sptr)
				{
					l = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
					this->lvDetThread->SetSubItem(k, 2, CSTRP(&sbuff[l + 1], sptr));
				}
			}
			DEL_CLASS(t);
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
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;

		proc.GetHeapLists(&heapList);

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
		Data::ArrayList<Manage::Process::HeapInfo*> heapList;
		Manage::Process::HeapInfo *heap;
		UTF8Char sbuff[20];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		Text::CString tStr;

		proc.GetHeaps(&heapList, heapId, 257);

		this->lvDetHeap->ClearItems();
		i = 0;
		j = heapList.GetCount();
		while (i < j)
		{
			heap = heapList.GetItem(i);
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
		proc.FreeHeaps(&heapList);
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
		UTF8Char *sptr;
		Manage::HandleType handleType;
		Text::StringBuilderUTF8 sb;
		UOSInt i;
		UOSInt j;

		proc.GetHandles(&handleList);

		this->lvDetHandle->ClearItems();
		i = 0;
		j = handleList.GetCount();
		while (i < j)
		{
			hinfo = handleList.GetItem(i);
			sptr = Text::StrInt32(sbuff, hinfo.id);
			this->lvDetHandle->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)hinfo.id);
			sptr = hinfo.createTime.ToLocalTime().ToStringNoZone(sbuff);
			this->lvDetHandle->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sb.ClearStr();
			if (proc.GetHandleDetail(hinfo.id, &handleType, &sb))
			{
				this->lvDetHandle->SetSubItem(i, 2, Manage::HandleTypeGetName(handleType));
				this->lvDetHandle->SetSubItem(i, 3, sb.ToCString());
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRProcInfoForm::AVIRProcInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Process Info"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->currProc = 0;
	this->currProcObj = 0;
	this->currProcRes = 0;
	this->threadCnt = Sync::Thread::GetThreadCnt();

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpSummary = this->tcMain->AddTabPage(CSTR("Summary"));
	this->tpDetail = this->tcMain->AddTabPage(CSTR("Detail"));

	NEW_CLASS(this->pnlSummary, UI::GUIPanel(ui, this->tpSummary));
	this->pnlSummary->SetRect(0, 0, 100, 48, false);
	this->pnlSummary->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lvSummary, UI::GUIListView(ui, this->tpSummary, UI::GUIListView::LVSTYLE_TABLE, 10));
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

	NEW_CLASS(this->lbDetail, UI::GUIListBox(ui, this->tpDetail, false));
	this->lbDetail->SetRect(0, 0, 200, 100, false);
	this->lbDetail->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDetail->HandleSelectionChange(OnProcSelChg, this);
	NEW_CLASS(this->hspDetail, UI::GUIHSplitter(ui, this->tpDetail, 3, false));
	NEW_CLASS(this->tcDetail, UI::GUITabControl(ui, this->tpDetail));
	this->tcDetail->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDetInfo = this->tcDetail->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lblDetProcId, UI::GUILabel(ui, this->tpDetInfo, CSTR("Process Id")));
	this->lblDetProcId->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtDetProcId, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetProcId->SetRect(100, 0, 50, 23, false);
	this->txtDetProcId->SetReadOnly(true);
	NEW_CLASS(this->lblDetParentId, UI::GUILabel(ui, this->tpDetInfo, CSTR("Parent Id")));
	this->lblDetParentId->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtDetParentId, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetParentId->SetRect(100, 24, 50, 23, false);
	this->txtDetParentId->SetReadOnly(true);
	NEW_CLASS(this->lblDetName, UI::GUILabel(ui, this->tpDetInfo, CSTR("Process Name")));
	this->lblDetName->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtDetName, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetName->SetRect(100, 48, 200, 23, false);
	this->txtDetName->SetReadOnly(true);
	NEW_CLASS(this->lblDetPath, UI::GUILabel(ui, this->tpDetInfo, CSTR("Process Path")));
	this->lblDetPath->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtDetPath, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetPath->SetRect(100, 72, 700, 23, false);
	this->txtDetPath->SetReadOnly(true);
	NEW_CLASS(this->lblDetCmdLine, UI::GUILabel(ui, this->tpDetInfo, CSTR("Command Line")));
	this->lblDetCmdLine->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtDetCmdLine, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetCmdLine->SetRect(100, 96, 700, 23, false);
	this->txtDetCmdLine->SetReadOnly(true);
	NEW_CLASS(this->lblDetWorkingDir, UI::GUILabel(ui, this->tpDetInfo, CSTR("Working Dir")));
	this->lblDetWorkingDir->SetRect(0, 120, 100, 23, false);
	NEW_CLASS(this->txtDetWorkingDir, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetWorkingDir->SetRect(100, 120, 700, 23, false);
	this->txtDetWorkingDir->SetReadOnly(true);
	NEW_CLASS(this->lblDetTrueProgPath, UI::GUILabel(ui, this->tpDetInfo, CSTR("True Prog Path")));
	this->lblDetTrueProgPath->SetRect(0, 144, 100, 23, false);
	NEW_CLASS(this->txtDetTrueProgPath, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetTrueProgPath->SetRect(100, 144, 700, 23, false);
	this->txtDetTrueProgPath->SetReadOnly(true);
	NEW_CLASS(this->lblDetPriority, UI::GUILabel(ui, this->tpDetInfo, CSTR("Priority")));
	this->lblDetPriority->SetRect(0, 168, 100, 23, false);
	NEW_CLASS(this->txtDetPriority, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetPriority->SetRect(100, 168, 100, 23, false);
	this->txtDetPriority->SetReadOnly(true);
	NEW_CLASS(this->lblDetArchitecture, UI::GUILabel(ui, this->tpDetInfo, CSTR("Architecture")));
	this->lblDetArchitecture->SetRect(0, 192, 100, 23, false);
	NEW_CLASS(this->txtDetArchitecture, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetArchitecture->SetRect(100, 192, 100, 23, false);
	this->txtDetArchitecture->SetReadOnly(true);

	this->tpDetModule = this->tcDetail->AddTabPage(CSTR("Module"));
	NEW_CLASS(this->pnlDetModule, UI::GUIPanel(ui, this->tpDetModule));
	this->pnlDetModule->SetRect(0, 0, 100, 31, false);
	this->pnlDetModule->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDetModule, UI::GUIButton(ui, this->pnlDetModule, CSTR("Refresh")));
	this->btnDetModule->SetRect(4, 4, 75, 23, false);
	this->btnDetModule->HandleButtonClick(OnDetModuleRefClicked, this);
	NEW_CLASS(this->lvDetModule, UI::GUIListView(ui, this->tpDetModule, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvDetModule->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetModule->SetFullRowSelect(true);
	this->lvDetModule->SetShowGrid(true);
	this->lvDetModule->AddColumn(CSTR("Name"), 600);
	this->lvDetModule->AddColumn(CSTR("Address"), 80);
	this->lvDetModule->AddColumn(CSTR("Size"), 80);

	this->tpDetThread = this->tcDetail->AddTabPage(CSTR("Thread"));
	NEW_CLASS(this->pnlDetThread, UI::GUIPanel(ui, this->tpDetThread));
	this->pnlDetThread->SetRect(0, 0, 100, 31, false);
	this->pnlDetThread->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDetThread, UI::GUIButton(ui, this->pnlDetThread, CSTR("Refresh")));
	this->btnDetThread->SetRect(4, 4, 75, 23, false);
	this->btnDetThread->HandleButtonClick(OnDetThreadRefClicked, this);
	NEW_CLASS(this->lvDetThread, UI::GUIListView(ui, this->tpDetThread, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvDetThread->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetThread->HandleDblClk(OnDetThreadDblClicked, this);
	this->lvDetThread->SetFullRowSelect(true);
	this->lvDetThread->SetShowGrid(true);
	this->lvDetThread->AddColumn(CSTR("Id"), 60);
	this->lvDetThread->AddColumn(CSTR("Start Address"), 120);
	this->lvDetThread->AddColumn(CSTR("Start Address(Name)"), 600);

	this->tpDetHeap = this->tcDetail->AddTabPage(CSTR("Heap"));
	NEW_CLASS(this->pnlDetHeap, UI::GUIPanel(ui, this->tpDetHeap));
	this->pnlDetHeap->SetRect(0, 0, 100, 31, false);
	this->pnlDetHeap->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDetHeap, UI::GUIButton(ui, this->pnlDetHeap, CSTR("Refresh")));
	this->btnDetHeap->SetRect(4, 4, 75, 23, false);
	this->btnDetHeap->HandleButtonClick(OnDetHeapRefClicked, this);
	NEW_CLASS(this->lbDetHeap, UI::GUIListBox(ui, this->tpDetHeap, false));
	this->lbDetHeap->SetRect(0, 0, 100, 23, false);
	this->lbDetHeap->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDetHeap->HandleSelectionChange(OnDetHeapSelChg, this);
	NEW_CLASS(this->hspDetHeap, UI::GUIHSplitter(ui, this->tpDetHeap, 3, false));
	NEW_CLASS(this->txtDetHeap, UI::GUITextBox(ui, this->tpDetHeap, CSTR(""), true));
	this->txtDetHeap->SetReadOnly(true);
	this->txtDetHeap->SetRect(0, 0, 100, 128, false);
	this->txtDetHeap->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lvDetHeap, UI::GUIListView(ui, this->tpDetHeap, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvDetHeap->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetHeap->SetFullRowSelect(true);
	this->lvDetHeap->SetShowGrid(true);
	this->lvDetHeap->AddColumn(CSTR("Start Address"), 160);
	this->lvDetHeap->AddColumn(CSTR("Size"), 60);
	this->lvDetHeap->AddColumn(CSTR("Type"), 80);
	this->lvDetHeap->HandleSelChg(OnDetHeapItemSelChg, this);

	this->tpDetHandle = this->tcDetail->AddTabPage(CSTR("Handles"));
	NEW_CLASS(this->pnlDetHandle, UI::GUIPanel(ui, this->tpDetHandle));
	this->pnlDetHandle->SetRect(0, 0, 100, 31, false);
	this->pnlDetHandle->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDetHandle, UI::GUIButton(ui, this->tpDetHandle, CSTR("Refresh")));
	this->btnDetHandle->SetRect(4, 4, 75, 23, false);
	this->btnDetHandle->HandleButtonClick(OnDetHandleClicked, this);
	NEW_CLASS(this->lvDetHandle, UI::GUIListView(ui, this->tpDetHandle, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvDetHandle->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetHandle->SetFullRowSelect(true);
	this->lvDetHandle->SetShowGrid(true);
	this->lvDetHandle->AddColumn(CSTR("Id"), 60);
	this->lvDetHandle->AddColumn(CSTR("Create Time"), 150);
	this->lvDetHandle->AddColumn(CSTR("Type"), 100);
	this->lvDetHandle->AddColumn(CSTR("Desc"), 300);

	this->tpDetChart = this->tcDetail->AddTabPage(CSTR("Chart"));
	NEW_CLASS(this->grpDetChartCPU, UI::GUIGroupBox(ui, this->tpDetChart, CSTR("CPU")));
	this->grpDetChartCPU->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCPU->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartCPU, UI::GUIRealtimeLineChart(ui, this->grpDetChartCPU, this->core->GetDrawEngine(), 1, 600, 300));
	this->rlcDetChartCPU->SetDockType(UI::GUIControl::DOCK_FILL);
	this->rlcDetChartCPU->SetUnit(CSTR("%"));
	NEW_CLASS(this->vspDetChartCPU, UI::GUIVSplitter(ui, this->tpDetChart, 3, false));
	NEW_CLASS(this->grpDetChartPage, UI::GUIGroupBox(ui, this->tpDetChart, CSTR("Paged(R)/Non-Paged(B) Pool")));
	this->grpDetChartPage->SetRect(0, 0, 100, 200, false);
	this->grpDetChartPage->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartPage, UI::GUIRealtimeLineChart(ui, this->grpDetChartPage, this->core->GetDrawEngine(), 2, 600, 300));
	this->rlcDetChartPage->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->vspDetChartPage, UI::GUIVSplitter(ui, this->tpDetChart, 3, false));
	NEW_CLASS(this->grpDetChartCount, UI::GUIGroupBox(ui, this->tpDetChart, CSTR("GDI(R)/User(B)/Handle(G) Count")));
	this->grpDetChartCount->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCount->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartCount, UI::GUIRealtimeLineChart(ui, this->grpDetChartCount, this->core->GetDrawEngine(), 3, 600, 300));
	this->rlcDetChartCount->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->vspDetChartCount, UI::GUIVSplitter(ui, this->tpDetChart, 3, false));
	NEW_CLASS(this->grpDetChartWS, UI::GUIGroupBox(ui, this->tpDetChart, CSTR("WS(R)/Page File(B)")));
	this->grpDetChartWS->SetRect(0, 0, 100, 200, false);
	this->grpDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->rlcDetChartWS, UI::GUIRealtimeLineChart(ui, this->grpDetChartWS, this->core->GetDrawEngine(), 2, 600, 300));
	this->rlcDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(1000, OnTimerTick, this);
	this->AddTimer(300, OnTimerCPUTick, this);
	OnTimerTick(this);
}

SSWR::AVIRead::AVIRProcInfoForm::~AVIRProcInfoForm()
{
	ProcessInfo *procInfo;
	UOSInt i;
	i = this->procList.GetCount();
	while (i-- > 0)
	{
		procInfo = this->procList.GetItem(i);
		procInfo->procName->Release();
		MemFree(procInfo);
	}
	SDEL_CLASS(this->currProcRes);
	SDEL_CLASS(this->currProcObj);
}

void SSWR::AVIRead::AVIRProcInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
