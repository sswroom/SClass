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
		me->txtDetProcId->SetText((const UTF8Char*)"");
		me->txtDetParentId->SetText((const UTF8Char*)"");
		me->txtDetName->SetText((const UTF8Char*)"");
		me->txtDetPath->SetText((const UTF8Char*)"");
		me->txtDetPriority->SetText((const UTF8Char*)"");
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
		me->txtDetProcId->SetText(sb.ToString());
		sb.ClearStr();
		sb.AppendU32(procInfo->parentProcId);
		me->txtDetParentId->SetText(sb.ToString());
		me->txtDetName->SetText(procInfo->procName);
		sb.ClearStr();
		proc.GetFilename(&sb);
		me->txtDetPath->SetText(sb.ToString());
		me->txtDetPriority->SetText(Manage::Process::GetPriorityName(proc.GetPriority()).v);
		me->UpdateProcHeaps();
		me->UpdateProcModules();
		me->UpdateProcThreads();

		me->rlcDetChartCPU->ClearChart();
		me->rlcDetChartCount->ClearChart();
		me->rlcDetChartWS->ClearChart();
		me->rlcDetChartPage->ClearChart();
		proc.GetTimeInfo(0, me->lastKernelTime, me->lastUserTime);
		me->clk->Start();
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
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess((const UTF8Char*)0);
	if (sess)
	{
		i = me->procList->GetCount();
		while (i-- > 0)
		{
			procInfo = me->procList->GetItem(i);
			procInfo->found = false;
		}

		while (Manage::Process::FindProcessNext(sbuff, sess, &proc))
		{
			si = me->procIds->SortedIndexOf(proc.processId);
			if (si >= 0)
			{
				procInfo = me->procList->GetItem((UOSInt)si);
				procInfo->found = true;
			}
			else
			{
				i = (UOSInt)~si;
				procInfo = MemAlloc(ProcessInfo, 1);
				procInfo->found = true;
				procInfo->procId = proc.processId;
				procInfo->parentProcId = proc.parentId;
				procInfo->procName = Text::StrCopyNew(sbuff);
				me->procIds->Insert(i, procInfo->procId);
				me->procList->Insert(i, procInfo);
				Text::StrUInt32(sbuff2, procInfo->procId);
				me->lvSummary->InsertItem(i, sbuff2, procInfo);
				me->lvSummary->SetSubItem(i, 1, sbuff);
				sptr = Text::StrConcat(Text::StrConcatC(Text::StrUInt32(sbuff, procInfo->procId), UTF8STRC(" ")), procInfo->procName);
				me->lbDetail->InsertItem(i, {sbuff, (UOSInt)(sptr - sbuff)}, procInfo);
			}

			Manage::Process proc(procInfo->procId, false);
			UOSInt ws;
			UOSInt pageFault;
			UOSInt pagedPool;
			UOSInt nonPagedPool;
			UOSInt pageFile;
			if (proc.GetMemoryInfo(&pageFault, &ws, &pagedPool, &nonPagedPool, &pageFile))
			{
				Text::StrUOSIntS(sbuff, ws, ',', 3);
				me->lvSummary->SetSubItem(i, 2, sbuff);
				Text::StrUOSIntS(sbuff, pageFault, ',', 3);
				me->lvSummary->SetSubItem(i, 3, sbuff);
				Text::StrUOSIntS(sbuff, pagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 4, sbuff);
				Text::StrUOSIntS(sbuff, nonPagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 5, sbuff);
				Text::StrUOSIntS(sbuff, pageFile, ',', 3);
				me->lvSummary->SetSubItem(i, 6, sbuff);

				Text::StrUInt32(sbuff, proc.GetGDIObjCount());
				me->lvSummary->SetSubItem(i, 7, sbuff);
				Text::StrUInt32(sbuff, proc.GetUserObjCount());
				me->lvSummary->SetSubItem(i, 8, sbuff);
				Text::StrUInt32(sbuff, proc.GetHandleCount());
				me->lvSummary->SetSubItem(i, 9, sbuff);
			}
			else
			{
				me->lvSummary->SetSubItem(i, 2, (const UTF8Char*)"-");
				me->lvSummary->SetSubItem(i, 3, (const UTF8Char*)"-");
				me->lvSummary->SetSubItem(i, 4, (const UTF8Char*)"-");
				me->lvSummary->SetSubItem(i, 5, (const UTF8Char*)"-");
				me->lvSummary->SetSubItem(i, 6, (const UTF8Char*)"-");
				me->lvSummary->SetSubItem(i, 7, (const UTF8Char*)"-");
				me->lvSummary->SetSubItem(i, 8, (const UTF8Char*)"-");
				me->lvSummary->SetSubItem(i, 9, (const UTF8Char*)"-");
			}
		}
		Manage::Process::FindProcessClose(sess);

		i = me->procList->GetCount();
		while (i-- > 0)
		{
			procInfo = me->procList->GetItem(i);
			if (!procInfo->found)
			{
				Text::StrDelNew(procInfo->procName);
				MemFree(procInfo);
				me->lvSummary->RemoveItem(i);
				me->lbDetail->RemoveItem(i);
				me->procList->RemoveAt(i);
				me->procIds->RemoveAt(i);
			}
		}
	}

}

void __stdcall SSWR::AVIRead::AVIRProcInfoForm::OnTimerCPUTick(void *userObj)
{
	SSWR::AVIRead::AVIRProcInfoForm *me = (SSWR::AVIRead::AVIRProcInfoForm*)userObj;
	if (me->currProc != 0)
	{
		Data::DateTime kernelTime;
		Data::DateTime userTime;
		Double t;
		Double v[3];
		Manage::Process proc(me->currProc, false);
		if (proc.GetTimeInfo(0, &kernelTime, &userTime))
		{
			t = me->clk->GetAndRestart();
			if (t > 0)
			{
				v[0] = (Double)(kernelTime.DiffMS(me->lastKernelTime) + userTime.DiffMS(me->lastUserTime)) / t / 10.0 / UOSInt2Double(me->threadCnt);
				me->rlcDetChartCPU->AddSample(v);
				me->lastKernelTime->SetValue(&kernelTime);
				me->lastUserTime->SetValue(&userTime);
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
	SSWR::AVIRead::AVIRThreadInfoForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRThreadInfoForm(0, me->ui, me->core, me->currProcObj, me->currProcRes, threadId));
	frm->ShowDialog(me);
	DEL_CLASS(frm);
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
		me->txtDetHeap->SetText(sb.ToString());
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
		me->txtDetHeap->SetText(sb.ToString());
	}
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
			module->GetModuleFileName(sbuff);
			k = this->lvDetModule->AddItem(sbuff, 0);
			if (module->GetModuleAddress(&addr, &size))
			{
				Text::StrHexValOS(sbuff, addr);
				this->lvDetModule->SetSubItem(k, 1, sbuff);
				Text::StrHexValOS(sbuff, size);
				this->lvDetModule->SetSubItem(k, 2, sbuff);
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
			Text::StrUOSInt(sbuff, t->GetThreadId());
			k = this->lvDetThread->AddItem(sbuff, (void*)(OSInt)t->GetThreadId(), 0);
			addr = t->GetStartAddress();
			Text::StrHexVal64(sbuff, addr);
			this->lvDetThread->SetSubItem(k, 1, sbuff);

			if (this->currProcRes)
			{
				sptr = this->currProcRes->ResolveName(sbuff, addr);
				l = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
				this->lvDetThread->SetSubItem(k, 2, &sbuff[l + 1]);
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
			this->lbDetHeap->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, (void*)(UOSInt)heapList.GetItem(i));
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
		this->lvDetHeap->AddItem((const UTF8Char*)"Showing heap of current process is not allowed", 0, 0);
	}
	else
	{
		Manage::Process proc(this->currProc, false);
		Data::ArrayList<Manage::Process::HeapInfo*> heapList;
		Manage::Process::HeapInfo *heap;
		UTF8Char sbuff[20];
		UOSInt i;
		UOSInt j;
		UOSInt k;
		const UTF8Char *tStr;

		proc.GetHeaps(&heapList, heapId, 257);

		this->lvDetHeap->ClearItems();
		i = 0;
		j = heapList.GetCount();
		while (i < j)
		{
			heap = heapList.GetItem(i);
			Text::StrHexValOS(Text::StrConcatC(sbuff, UTF8STRC("0x")), heap->startAddr);
			k = this->lvDetHeap->AddItem(sbuff, (void*)heap->startAddr, 0);
			Text::StrUOSInt(sbuff, heap->size);
			this->lvDetHeap->SetSubItem(k, 1, sbuff);
			switch (heap->heapType)
			{
			case Manage::Process::HT_FIXED:
				tStr = (const UTF8Char*)"Fixed";
				break;
			case Manage::Process::HT_MOVABLE:
				tStr = (const UTF8Char*)"Movable";
				break;
			case Manage::Process::HT_FREE:
				tStr = (const UTF8Char*)"Free";
				break;
			case Manage::Process::HT_UNKNOWN:
			default:
				tStr = (const UTF8Char*)"Unknown";
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

SSWR::AVIRead::AVIRProcInfoForm::AVIRProcInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"Process Info");

	this->core = core;
	NEW_CLASS(this->clk, Manage::HiResClock());
	NEW_CLASS(this->lastUserTime, Data::DateTime());
	NEW_CLASS(this->lastKernelTime, Data::DateTime());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->procList, Data::ArrayList<ProcessInfo*>());
	NEW_CLASS(this->procIds, Data::ArrayListUInt32());
	this->currProc = 0;
	this->currProcObj = 0;
	this->currProcRes = 0;
	this->threadCnt = Sync::Thread::GetThreadCnt();

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpSummary = this->tcMain->AddTabPage((const UTF8Char*)"Summary");
	this->tpDetail = this->tcMain->AddTabPage((const UTF8Char*)"Detail");

	NEW_CLASS(this->pnlSummary, UI::GUIPanel(ui, this->tpSummary));
	this->pnlSummary->SetRect(0, 0, 100, 48, false);
	this->pnlSummary->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lvSummary, UI::GUIListView(ui, this->tpSummary, UI::GUIListView::LVSTYLE_TABLE, 10));
	this->lvSummary->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSummary->SetFullRowSelect(true);
	this->lvSummary->SetShowGrid(true);
	this->lvSummary->HandleDblClk(OnSumDblClicked, this);
	this->lvSummary->AddColumn((const UTF8Char*)"Id", 60);
	this->lvSummary->AddColumn((const UTF8Char*)"Process Name", 120);
	this->lvSummary->AddColumn((const UTF8Char*)"WS Size", 80);
	this->lvSummary->AddColumn((const UTF8Char*)"Page Fault", 80);
	this->lvSummary->AddColumn((const UTF8Char*)"Paged Pool", 60);
	this->lvSummary->AddColumn((const UTF8Char*)"Non-Paged Pool", 60);
	this->lvSummary->AddColumn((const UTF8Char*)"Page File", 80);
	this->lvSummary->AddColumn((const UTF8Char*)"GDI Objects", 50);
	this->lvSummary->AddColumn((const UTF8Char*)"User Objects", 50);
	this->lvSummary->AddColumn((const UTF8Char*)"Handles", 50);

	NEW_CLASS(this->lbDetail, UI::GUIListBox(ui, this->tpDetail, false));
	this->lbDetail->SetRect(0, 0, 200, 100, false);
	this->lbDetail->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDetail->HandleSelectionChange(OnProcSelChg, this);
	NEW_CLASS(this->hspDetail, UI::GUIHSplitter(ui, this->tpDetail, 3, false));
	NEW_CLASS(this->tcDetail, UI::GUITabControl(ui, this->tpDetail));
	this->tcDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDetInfo = this->tcDetail->AddTabPage((const UTF8Char*)"Info");
	this->tpDetModule = this->tcDetail->AddTabPage((const UTF8Char*)"Module");
	this->tpDetThread = this->tcDetail->AddTabPage((const UTF8Char*)"Thread");
	this->tpDetHeap = this->tcDetail->AddTabPage((const UTF8Char*)"Heap");
	this->tpDetChart = this->tcDetail->AddTabPage((const UTF8Char*)"Chart");

	NEW_CLASS(this->lblDetProcId, UI::GUILabel(ui, this->tpDetInfo, (const UTF8Char*)"Process Id"));
	this->lblDetProcId->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtDetProcId, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetProcId->SetRect(100, 0, 50, 23, false);
	this->txtDetProcId->SetReadOnly(true);
	NEW_CLASS(this->lblDetParentId, UI::GUILabel(ui, this->tpDetInfo, (const UTF8Char*)"Parent Id"));
	this->lblDetParentId->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtDetParentId, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetParentId->SetRect(100, 24, 50, 23, false);
	this->txtDetParentId->SetReadOnly(true);
	NEW_CLASS(this->lblDetName, UI::GUILabel(ui, this->tpDetInfo, (const UTF8Char*)"Process Name"));
	this->lblDetName->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtDetName, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetName->SetRect(100, 48, 200, 23, false);
	this->txtDetName->SetReadOnly(true);
	NEW_CLASS(this->lblDetPath, UI::GUILabel(ui, this->tpDetInfo, (const UTF8Char*)"Process Path"));
	this->lblDetPath->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtDetPath, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetPath->SetRect(100, 72, 700, 23, false);
	this->txtDetPath->SetReadOnly(true);
	NEW_CLASS(this->lblDetPriority, UI::GUILabel(ui, this->tpDetInfo, (const UTF8Char*)"Priority"));
	this->lblDetPriority->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtDetPriority, UI::GUITextBox(ui, this->tpDetInfo, CSTR(""), false));
	this->txtDetPriority->SetRect(100, 96, 100, 23, false);
	this->txtDetPriority->SetReadOnly(true);

	NEW_CLASS(this->pnlDetModule, UI::GUIPanel(ui, this->tpDetModule));
	this->pnlDetModule->SetRect(0, 0, 100, 31, false);
	this->pnlDetModule->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDetModule, UI::GUIButton(ui, this->pnlDetModule, (const UTF8Char*)"Refresh"));
	this->btnDetModule->SetRect(4, 4, 75, 23, false);
	this->btnDetModule->HandleButtonClick(OnDetModuleRefClicked, this);
	NEW_CLASS(this->lvDetModule, UI::GUIListView(ui, this->tpDetModule, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvDetModule->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetModule->SetFullRowSelect(true);
	this->lvDetModule->SetShowGrid(true);
	this->lvDetModule->AddColumn((const UTF8Char*)"Name", 600);
	this->lvDetModule->AddColumn((const UTF8Char*)"Address", 80);
	this->lvDetModule->AddColumn((const UTF8Char*)"Size", 80);

	NEW_CLASS(this->pnlDetThread, UI::GUIPanel(ui, this->tpDetThread));
	this->pnlDetThread->SetRect(0, 0, 100, 31, false);
	this->pnlDetThread->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDetThread, UI::GUIButton(ui, this->pnlDetThread, (const UTF8Char*)"Refresh"));
	this->btnDetThread->SetRect(4, 4, 75, 23, false);
	this->btnDetThread->HandleButtonClick(OnDetThreadRefClicked, this);
	NEW_CLASS(this->lvDetThread, UI::GUIListView(ui, this->tpDetThread, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvDetThread->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetThread->HandleDblClk(OnDetThreadDblClicked, this);
	this->lvDetThread->SetFullRowSelect(true);
	this->lvDetThread->SetShowGrid(true);
	this->lvDetThread->AddColumn((const UTF8Char*)"Id", 60);
	this->lvDetThread->AddColumn((const UTF8Char*)"Start Address", 120);
	this->lvDetThread->AddColumn((const UTF8Char*)"Start Address(Name)", 600);

	NEW_CLASS(this->pnlDetHeap, UI::GUIPanel(ui, this->tpDetHeap));
	this->pnlDetHeap->SetRect(0, 0, 100, 31, false);
	this->pnlDetHeap->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDetHeap, UI::GUIButton(ui, this->pnlDetHeap, (const UTF8Char*)"Refresh"));
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
	this->lvDetHeap->AddColumn((const UTF8Char*)"Start Address", 160);
	this->lvDetHeap->AddColumn((const UTF8Char*)"Size", 60);
	this->lvDetHeap->AddColumn((const UTF8Char*)"Type", 80);
	this->lvDetHeap->HandleSelChg(OnDetHeapItemSelChg, this);

	NEW_CLASS(this->grpDetChartCPU, UI::GUIGroupBox(ui, this->tpDetChart, (const UTF8Char*)"CPU"));
	this->grpDetChartCPU->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCPU->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartCPU, UI::GUIRealtimeLineChart(ui, this->grpDetChartCPU, this->core->GetDrawEngine(), 1, 600, 300));
	this->rlcDetChartCPU->SetDockType(UI::GUIControl::DOCK_FILL);
	this->rlcDetChartCPU->SetUnit((const UTF8Char*)"%");
	NEW_CLASS(this->vspDetChartCPU, UI::GUIVSplitter(ui, this->tpDetChart, 3, false));
	NEW_CLASS(this->grpDetChartPage, UI::GUIGroupBox(ui, this->tpDetChart, (const UTF8Char*)"Paged(R)/Non-Paged(B) Pool"));
	this->grpDetChartPage->SetRect(0, 0, 100, 200, false);
	this->grpDetChartPage->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartPage, UI::GUIRealtimeLineChart(ui, this->grpDetChartPage, this->core->GetDrawEngine(), 2, 600, 300));
	this->rlcDetChartPage->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->vspDetChartPage, UI::GUIVSplitter(ui, this->tpDetChart, 3, false));
	NEW_CLASS(this->grpDetChartCount, UI::GUIGroupBox(ui, this->tpDetChart, (const UTF8Char*)"GDI(R)/User(B)/Handle(G) Count"));
	this->grpDetChartCount->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCount->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartCount, UI::GUIRealtimeLineChart(ui, this->grpDetChartCount, this->core->GetDrawEngine(), 3, 600, 300));
	this->rlcDetChartCount->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->vspDetChartCount, UI::GUIVSplitter(ui, this->tpDetChart, 3, false));
	NEW_CLASS(this->grpDetChartWS, UI::GUIGroupBox(ui, this->tpDetChart, (const UTF8Char*)"WS(R)/Page File(B)"));
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
	i = this->procList->GetCount();
	while (i-- > 0)
	{
		procInfo = this->procList->GetItem(i);
		Text::StrDelNew(procInfo->procName);
		MemFree(procInfo);
	}
	DEL_CLASS(this->procList);
	DEL_CLASS(this->procIds);
	SDEL_CLASS(this->currProcRes);
	SDEL_CLASS(this->currProcObj);
	DEL_CLASS(this->lastKernelTime);
	DEL_CLASS(this->lastUserTime);
	DEL_CLASS(this->clk);
}

void SSWR::AVIRead::AVIRProcInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
