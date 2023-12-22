#include "Stdafx.h"
#include "Manage/Process.h"
#include "SSWR/AVIReadCE/AVIRCEProcInfoForm.h"
#include "SSWR/AVIReadCE/AVIRCEThreadInfoForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnSumDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	me->lbDetail->SetSelectedIndex(index);
	me->tcMain->SetSelectedIndex(1);
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnProcSelChg(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	ProcessInfo *procInfo = (ProcessInfo*)me->lbDetail->GetSelectedItem();
	SDEL_CLASS(me->currProcRes);
	SDEL_CLASS(me->currProcObj);
	if (procInfo == 0)
	{
		me->txtDetProcId->SetText(CSTR(""));
		me->txtDetParentId->SetText(CSTR(""));
		me->txtDetName->SetText(CSTR(""));
		me->txtDetPath->SetText(CSTR(""));
		me->txtDetPriority->SetText(CSTR(""));
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
		NotNullPtr<Manage::Process> procress;

		NEW_CLASSNN(procress, Manage::Process(procInfo->procId, false));
		me->currProcObj = procress.Ptr();
		NEW_CLASS(me->currProcRes, Manage::SymbolResolver(procress));
		Manage::Process proc(procInfo->procId, false);
		sb.AppendI32((Int32)procInfo->procId);
		me->txtDetProcId->SetText(sb.ToCString());
		sb.ClearStr();
		sb.AppendI32((Int32)procInfo->parentProcId);
		me->txtDetParentId->SetText(sb.ToCString());
		me->txtDetName->SetText(procInfo->procName->ToCString());
		sb.ClearStr();
		proc.GetFilename(sb);
		me->txtDetPath->SetText(sb.ToCString());
		me->txtDetPriority->SetText(Manage::Process::GetPriorityName(proc.GetPriority()));
		me->UpdateProcHeaps();
		me->UpdateProcModules();
		me->UpdateProcThreads();

		me->rlcDetChartCPU->ClearChart();
		me->rlcDetChartCount->ClearChart();
		me->rlcDetChartWS->ClearChart();
		me->rlcDetChartPage->ClearChart();
		proc.GetTimeInfo(0, &me->lastKernelTime, &me->lastUserTime);
		me->clk->Start();
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnTimerTick(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char sbuff2[12];
	UTF8Char *sptr2;
	ProcessInfo *procInfo;
	Manage::Process::ProcessInfo proc;
	OSInt i;
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess(CSTR_NULL);
	if (sess)
	{
		i = me->procList->GetCount();
		while (i-- > 0)
		{
			procInfo = me->procList->GetItem(i);
			procInfo->found = false;
		}

		while ((sptr = Manage::Process::FindProcessNext(sbuff, sess, &proc)) != 0)
		{
			i = me->procIds->SortedIndexOf(proc.processId);
			if (i >= 0)
			{
				procInfo = me->procList->GetItem(i);
				procInfo->found = true;
			}
			else
			{
				i = ~i;
				procInfo = MemAlloc(ProcessInfo, 1);
				procInfo->found = true;
				procInfo->procId = proc.processId;
				procInfo->parentProcId = proc.parentId;
				procInfo->procName = Text::String::NewP(sbuff, sptr);
				me->procIds->Insert(i, procInfo->procId);
				me->procList->Insert(i, procInfo);
				sptr2 = Text::StrUInt32(sbuff2, procInfo->procId);
				me->lvSummary->InsertItem(i, CSTRP(sbuff2, sptr2), procInfo);
				me->lvSummary->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				me->lbDetail->InsertItem(i, procInfo->procName->ToCString(), procInfo);
			}

			Manage::Process proc(procInfo->procId, false);
			UOSInt ws;
			UOSInt pageFault;
			UOSInt pagedPool;
			UOSInt nonPagedPool;
			UOSInt pageFile;
			if (proc.GetMemoryInfo(&pageFault, &ws, &pagedPool, &nonPagedPool, &pageFile))
			{
				sptr = Text::StrOSIntS(sbuff, ws, ',', 3);
				me->lvSummary->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrOSIntS(sbuff, pageFault, ',', 3);
				me->lvSummary->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				sptr = Text::StrOSIntS(sbuff, pagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				sptr = Text::StrOSIntS(sbuff, nonPagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				sptr = Text::StrOSIntS(sbuff, pageFile, ',', 3);
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
				me->lvSummary->SetSubItem(i, 2, L"-");
				me->lvSummary->SetSubItem(i, 3, L"-");
				me->lvSummary->SetSubItem(i, 4, L"-");
				me->lvSummary->SetSubItem(i, 5, L"-");
				me->lvSummary->SetSubItem(i, 6, L"-");
				me->lvSummary->SetSubItem(i, 7, L"-");
				me->lvSummary->SetSubItem(i, 8, L"-");
				me->lvSummary->SetSubItem(i, 9, L"-");
			}
		}
		Manage::Process::FindProcessClose(sess);

		i = me->procList->GetCount();
		while (i-- > 0)
		{
			procInfo = me->procList->GetItem(i);
			if (!procInfo->found)
			{
				procInfo->procName->Release();
				MemFree(procInfo);
				me->lvSummary->RemoveItem(i);
				me->lbDetail->RemoveItem(i);
				me->procList->RemoveAt(i);
				me->procIds->RemoveAt(i);
			}
		}
	}

}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnTimerCPUTick(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	if (me->currProc != 0)
	{
		Data::Timestamp kernelTime;
		Data::Timestamp userTime;
		Double t;
		Double v[3];
		Manage::Process proc(me->currProc, false);
		if (proc.GetTimeInfo(0, &kernelTime, &userTime))
		{
			t = me->clk->GetAndRestart();
			if (t > 0)
			{
				v[0] = (kernelTime.DiffMS(me->lastKernelTime) + userTime.DiffMS(me->lastUserTime)) / t / 10.0 / me->threadCnt;
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
			v[0] = pagePool;
			v[1] = nonPagePool;
			me->rlcDetChartPage->AddSample(v);
			v[0] = workingSet;
			v[1] = pageFile;
			me->rlcDetChartWS->AddSample(v);
		}
		v[0] = proc.GetGDIObjCount();
		v[1] = proc.GetUserObjCount();
		v[2] = proc.GetHandleCount();
		me->rlcDetChartCount->AddSample(v);
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetModuleRefClicked(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	me->UpdateProcModules();
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetThreadRefClicked(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	me->UpdateProcThreads();
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetThreadDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	Int32 threadId = (Int32)(OSInt)me->lvDetThread->GetItem(index);
	SSWR::AVIReadCE::AVIRCEThreadInfoForm frm(0, me->ui, me->currProcObj, me->currProcRes, threadId);
	frm.ShowDialog(me);
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetHeapRefClicked(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	me->UpdateProcHeaps();
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetHeapSelChg(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEProcInfoForm *me = (SSWR::AVIReadCE::AVIRCEProcInfoForm*)userObj;
	OSInt id = (OSInt)me->lbDetHeap->GetSelectedItem();
	me->UpdateProcHeapDetail((Int32)id);
}

void SSWR::AVIReadCE::AVIRCEProcInfoForm::UpdateProcModules()
{
	if (this->currProc == 0)
	{
		this->lvDetModule->ClearItems();
	}
	else
	{
		Manage::Process proc(this->currProc, false);
		Data::ArrayListNN<Manage::ModuleInfo> modList;
		NotNullPtr<Manage::ModuleInfo> module;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt addr;
		UOSInt size;

		proc.GetModules(modList);

		this->lvDetModule->ClearItems();
		Data::ArrayIterator<NotNullPtr<Manage::ModuleInfo>> it = modList.Iterator();
		while (it.HasNext())
		{
			module = it.Next();
			sptr = module->GetModuleFileName(sbuff);
			k = this->lvDetModule->AddItem(CSTRP(sbuff, sptr), 0, 0);
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

void SSWR::AVIReadCE::AVIRCEProcInfoForm::UpdateProcThreads()
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
		Int64 addr;

		proc.GetThreads(threadList);
		this->lvDetThread->ClearItems();
		i = 0;
		j = threadList.GetCount();
		while (i < j)
		{
			t = threadList.GetItem(i);
			sptr = Text::StrUInt32(sbuff, t->GetThreadId());
			k = this->lvDetThread->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)t->GetThreadId(), 0);
			addr = t->GetStartAddress();
			sptr = Text::StrHexVal64(sbuff, addr);
			this->lvDetThread->SetSubItem(k, 1, CSTRP(sbuff, sptr));

			if (this->currProcRes)
			{
				sptr = this->currProcRes->ResolveName(sbuff, addr);
				l = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
				this->lvDetThread->SetSubItem(k, 2, CSTRP(&sbuff[l + 1], sptr));
			}
			DEL_CLASS(t);
			i++;
		}
	}	
}

void SSWR::AVIReadCE::AVIRCEProcInfoForm::UpdateProcHeaps()
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
			this->lbDetHeap->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)heapList.GetItem(i));
			i++;
		}
	}
}

void SSWR::AVIReadCE::AVIRCEProcInfoForm::UpdateProcHeapDetail(Int32 heapId)
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
		Text::CStringNN tStr;

		proc.GetHeaps(&heapList, heapId, 50);

		this->lvDetHeap->ClearItems();
		i = 0;
		j = heapList.GetCount();
		while (i < j)
		{
			heap = heapList.GetItem(i);
			sptr = Text::StrHexValOS(Text::StrConcatC(sbuff, UTF8STRC("0x")), heap->startAddr);
			k = this->lvDetHeap->AddItem(CSTRP(sbuff, sptr), 0, 0);
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
			default:
				tStr = CSTR("Unknown");
				break;
			}
			this->lvDetHeap->SetSubItem(k, 2, tStr);
			i++;
		}
		proc.FreeHeaps(&heapList);
	}
	
}

SSWR::AVIReadCE::AVIRCEProcInfoForm::AVIRCEProcInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 240, 320, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Process Info"));
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	this->core = core;
	NEW_CLASS(this->clk, Manage::HiResClock());
	this->lastUserTime = 0;
	this->lastKernelTime = 0;

	NEW_CLASS(this->procList, Data::ArrayList<ProcessInfo*>());
	NEW_CLASS(this->procIds, Data::ArrayListInt32());
	this->currProc = 0;
	this->currProcObj = 0;
	this->currProcRes = 0;
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();

	NEW_CLASS(this->tcMain = ui->NewTabControl(*this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpSummary = this->tcMain->AddTabPage(CSTR("Summary"));
	this->tpDetail = this->tcMain->AddTabPage(CSTR("Detail"));

	NEW_CLASSNN(this->pnlSummary = ui->NewPanel(this->tpSummary));
	this->pnlSummary->SetRect(0, 0, 100, 48, false);
	this->pnlSummary->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lvSummary = ui->NewListView(this->tpSummary, UI::ListViewStyle::Table, 10));
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

	NEW_CLASS(this->lbDetail = ui->NewListBox(this->tpDetail, false));
	this->lbDetail->SetRect(0, 0, 80, 100, false);
	this->lbDetail->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDetail->HandleSelectionChange(OnProcSelChg, this);
	this->hspDetail = ui->NewHSplitter(this->tpDetail, 3, false);
	NEW_CLASS(this->tcDetail = ui->NewTabControl(this->tpDetail));
	this->tcDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDetInfo = this->tcDetail->AddTabPage(CSTR("Info"));
	this->tpDetModule = this->tcDetail->AddTabPage(CSTR("Module"));
	this->tpDetThread = this->tcDetail->AddTabPage(CSTR("Thread"));
	this->tpDetHeap = this->tcDetail->AddTabPage(CSTR("Heap"));
	this->tpDetChart = this->tcDetail->AddTabPage(CSTR("Chart"));

	this->lblDetProcId = ui->NewLabel(this->tpDetInfo, CSTR("Process Id"));
	this->lblDetProcId->SetRect(0, 0, 55, 23, false);
	this->txtDetProcId = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetProcId->SetRect(55, 0, 80, 23, false);
	this->txtDetProcId->SetReadOnly(true);
	this->lblDetParentId = ui->NewLabel(this->tpDetInfo, CSTR("Parent Id"));
	this->lblDetParentId->SetRect(0, 24, 55, 23, false);
	this->txtDetParentId = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetParentId->SetRect(55, 24, 50, 23, false);
	this->txtDetParentId->SetReadOnly(true);
	this->lblDetName = ui->NewLabel(this->tpDetInfo, CSTR("Name"));
	this->lblDetName->SetRect(0, 48, 55, 23, false);
	this->txtDetName = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetName->SetRect(55, 48, 200, 23, false);
	this->txtDetName->SetReadOnly(true);
	this->lblDetPath = ui->NewLabel(this->tpDetInfo, CSTR("Path"));
	this->lblDetPath->SetRect(0, 72, 55, 23, false);
	this->txtDetPath = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetPath->SetRect(55, 72, 700, 23, false);
	this->txtDetPath->SetReadOnly(true);
	this->lblDetPriority = ui->NewLabel(this->tpDetInfo, CSTR("Priority"));
	this->lblDetPriority->SetRect(0, 96, 55, 23, false);
	this->txtDetPriority = ui->NewTextBox(this->tpDetInfo, CSTR(""), false);
	this->txtDetPriority->SetRect(55, 96, 100, 23, false);
	this->txtDetPriority->SetReadOnly(true);

	NEW_CLASSNN(this->pnlDetModule = ui->NewPanel(this->tpDetModule));
	this->pnlDetModule->SetRect(0, 0, 100, 31, false);
	this->pnlDetModule->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetModule = ui->NewButton(this->pnlDetModule, CSTR("Refresh"));
	this->btnDetModule->SetRect(4, 4, 75, 23, false);
	this->btnDetModule->HandleButtonClick(OnDetModuleRefClicked, this);
	NEW_CLASS(this->lvDetModule = ui->NewListView(this->tpDetModule, UI::ListViewStyle::Table, 3));
	this->lvDetModule->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetModule->SetFullRowSelect(true);
	this->lvDetModule->SetShowGrid(true);
	this->lvDetModule->AddColumn(CSTR("Name"), 600);
	this->lvDetModule->AddColumn(CSTR("Address"), 80);
	this->lvDetModule->AddColumn(CSTR("Size"), 80);

	NEW_CLASSNN(this->pnlDetThread = ui->NewPanel(this->tpDetThread));
	this->pnlDetThread->SetRect(0, 0, 100, 31, false);
	this->pnlDetThread->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetThread = ui->NewButton(this->pnlDetThread, CSTR("Refresh"));
	this->btnDetThread->SetRect(4, 4, 75, 23, false);
	this->btnDetThread->HandleButtonClick(OnDetThreadRefClicked, this);
	NEW_CLASS(this->lvDetThread = ui->NewListView(this->tpDetThread, UI::ListViewStyle::Table, 3));
	this->lvDetThread->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetThread->HandleDblClk(OnDetThreadDblClicked, this);
	this->lvDetThread->SetFullRowSelect(true);
	this->lvDetThread->SetShowGrid(true);
	this->lvDetThread->AddColumn(CSTR("Id"), 60);
	this->lvDetThread->AddColumn(CSTR("Start Address"), 120);
	this->lvDetThread->AddColumn(CSTR("Start Address(Name)"), 600);

	NEW_CLASSNN(this->pnlDetHeap = ui->NewPanel(this->tpDetHeap));
	this->pnlDetHeap->SetRect(0, 0, 100, 31, false);
	this->pnlDetHeap->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetHeap = ui->NewButton(this->pnlDetHeap, CSTR("Refresh"));
	this->btnDetHeap->SetRect(4, 4, 75, 23, false);
	this->btnDetHeap->HandleButtonClick(OnDetHeapRefClicked, this);
	NEW_CLASS(this->lbDetHeap = ui->NewListBox(this->tpDetHeap, false));
	this->lbDetHeap->SetRect(0, 0, 100, 23, false);
	this->lbDetHeap->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDetHeap->HandleSelectionChange(OnDetHeapSelChg, this);
	this->hspDetHeap = ui->NewHSplitter(this->tpDetHeap, 3, false);
	NEW_CLASS(this->lvDetHeap = ui->NewListView(this->tpDetHeap, UI::ListViewStyle::Table, 3));
	this->lvDetHeap->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetHeap->SetFullRowSelect(true);
	this->lvDetHeap->SetShowGrid(true);
	this->lvDetHeap->AddColumn(CSTR("Start Address"), 80);
	this->lvDetHeap->AddColumn(CSTR("Size"), 60);
	this->lvDetHeap->AddColumn(CSTR("Tyep"), 80);

	this->grpDetChartCPU = ui->NewGroupBox(this->tpDetChart, CSTR("CPU"));
	this->grpDetChartCPU->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCPU->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartCPU = ui->NewRealtimeLineChart(this->grpDetChartCPU, this->core->GetDrawEngine(), 1, 600, 300));
	this->rlcDetChartCPU->SetDockType(UI::GUIControl::DOCK_FILL);
	this->rlcDetChartCPU->SetUnit(CSTR("%"));
	this->vspDetChartCPU = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartPage = ui->NewGroupBox(this->tpDetChart, CSTR("Paged(R)/Non-Paged(B) Pool"));
	this->grpDetChartPage->SetRect(0, 0, 100, 200, false);
	this->grpDetChartPage->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartPage = ui->NewRealtimeLineChart(this->grpDetChartPage, this->core->GetDrawEngine(), 2, 600, 300));
	this->rlcDetChartPage->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspDetChartPage = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartCount = ui->NewGroupBox(this->tpDetChart, CSTR("GDI(R)/User(B)/Handle(G) Count"));
	this->grpDetChartCount->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCount->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->rlcDetChartCount = ui->NewRealtimeLineChart(this->grpDetChartCount, this->core->GetDrawEngine(), 3, 600, 300));
	this->rlcDetChartCount->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspDetChartCount = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartWS = ui->NewGroupBox(this->tpDetChart, CSTR("WS(R)/Page File(B)"));
	this->grpDetChartWS->SetRect(0, 0, 100, 200, false);
	this->grpDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->rlcDetChartWS = ui->NewRealtimeLineChart(this->grpDetChartWS, this->core->GetDrawEngine(), 2, 600, 300));
	this->rlcDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(1000, OnTimerTick, this);
	this->AddTimer(300, OnTimerCPUTick, this);
	OnTimerTick(this);
}

SSWR::AVIReadCE::AVIRCEProcInfoForm::~AVIRCEProcInfoForm()
{
	ProcessInfo *procInfo;
	OSInt i;
	i = this->procList->GetCount();
	while (i-- > 0)
	{
		procInfo = this->procList->GetItem(i);
		procInfo->procName->Release();
		MemFree(procInfo);
	}
	DEL_CLASS(this->procList);
	DEL_CLASS(this->procIds);
	SDEL_CLASS(this->currProcRes);
	SDEL_CLASS(this->currProcObj);
	DEL_CLASS(this->clk);
}
