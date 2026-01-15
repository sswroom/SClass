#include "Stdafx.h"
#include "Manage/Process.h"
#include "SSWR/AVIReadCE/AVIRCEProcInfoForm.h"
#include "SSWR/AVIReadCE/AVIRCEThreadInfoForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnSumDblClicked(AnyType userObj, UIntOS index)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	me->lbDetail->SetSelectedIndex(index);
	me->tcMain->SetSelectedIndex(1);
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnProcSelChg(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	NN<ProcessInfo> procInfo;
	me->currProcRes.Delete();
	me->currProcObj.Delete();
	if (!me->lbDetail->GetSelectedItem().GetOpt<ProcessInfo>().SetTo(procInfo))
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
		NN<Manage::Process> procress;

		NEW_CLASSNN(procress, Manage::Process(procInfo->procId, false));
		me->currProcObj = procress;
		NEW_CLASSOPT(me->currProcRes, Manage::SymbolResolver(procress));
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
		me->clk.Start();
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char sbuff2[12];
	UnsafeArray<UTF8Char> sptr2;
	NN<ProcessInfo> procInfo;
	Manage::Process::ProcessInfo proc;
	IntOS i;
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess(nullptr);
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
			i = me->procIds.SortedIndexOf(proc.processId);
			if (i >= 0)
			{
				procInfo = me->procList.GetItemNoCheck(i);
				procInfo->found = true;
			}
			else
			{
				i = ~i;
				procInfo = MemAllocNN(ProcessInfo);
				procInfo->found = true;
				procInfo->procId = proc.processId;
				procInfo->parentProcId = proc.parentId;
				procInfo->procName = Text::String::NewP(sbuff, sptr);
				me->procIds.Insert(i, procInfo->procId);
				me->procList.Insert(i, procInfo);
				sptr2 = Text::StrUInt32(sbuff2, procInfo->procId);
				me->lvSummary->InsertItem(i, CSTRP(sbuff2, sptr2), procInfo);
				me->lvSummary->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				me->lbDetail->InsertItem(i, procInfo->procName->ToCString(), procInfo);
			}

			Manage::Process proc(procInfo->procId, false);
			UIntOS ws;
			UIntOS pageFault;
			UIntOS pagedPool;
			UIntOS nonPagedPool;
			UIntOS pageFile;
			if (proc.GetMemoryInfo(&pageFault, &ws, &pagedPool, &nonPagedPool, &pageFile))
			{
				sptr = Text::StrIntOSS(sbuff, ws, ',', 3);
				me->lvSummary->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrIntOSS(sbuff, pageFault, ',', 3);
				me->lvSummary->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				sptr = Text::StrIntOSS(sbuff, pagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				sptr = Text::StrIntOSS(sbuff, nonPagedPool, ',', 3);
				me->lvSummary->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				sptr = Text::StrIntOSS(sbuff, pageFile, ',', 3);
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

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnTimerCPUTick(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
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
				v[0] = (kernelTime.DiffMS(me->lastKernelTime) + userTime.DiffMS(me->lastUserTime)) / t / 10.0 / me->threadCnt;
				me->rlcDetChartCPU->AddSample(v);
				me->lastKernelTime = kernelTime;
				me->lastUserTime = userTime;
			}
		}
		UIntOS workingSet;
		UIntOS pagePool;
		UIntOS nonPagePool;
		UIntOS pageFile;
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

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetModuleRefClicked(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	me->UpdateProcModules();
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetThreadRefClicked(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	me->UpdateProcThreads();
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetThreadDblClicked(AnyType userObj, UIntOS index)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	NN<Manage::Process> currProcObj;
	if (me->currProcObj.SetTo(currProcObj))
	{
		Int32 threadId = (Int32)me->lvDetThread->GetItem(index).GetIntOS();
		SSWR::AVIReadCE::AVIRCEThreadInfoForm frm(0, me->ui, currProcObj, me->currProcRes, threadId);
		frm.ShowDialog(me);
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetHeapRefClicked(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	me->UpdateProcHeaps();
}

void __stdcall SSWR::AVIReadCE::AVIRCEProcInfoForm::OnDetHeapSelChg(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEProcInfoForm>();
	IntOS id = me->lbDetHeap->GetSelectedItem().GetIntOS();
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
		NN<Manage::ModuleInfo> module;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i;
		UIntOS j;
		UIntOS k;
		UIntOS addr;
		UIntOS size;

		proc.GetModules(modList);

		this->lvDetModule->ClearItems();
		Data::ArrayIterator<NN<Manage::ModuleInfo>> it = modList.Iterator();
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
		Data::ArrayListNN<Manage::ThreadInfo> threadList;
		NN<Manage::ThreadInfo> t;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i;
		UIntOS j;
		UIntOS k;
		UIntOS l;
		Int64 addr;
		NN<Manage::SymbolResolver> currProcRes;

		proc.GetThreads(threadList);
		this->lvDetThread->ClearItems();
		i = 0;
		j = threadList.GetCount();
		while (i < j)
		{
			t = threadList.GetItemNoCheck(i);
			sptr = Text::StrUInt32(sbuff, t->GetThreadId());
			k = this->lvDetThread->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)t->GetThreadId(), 0);
			addr = t->GetStartAddress();
			sptr = Text::StrHexVal64(sbuff, addr);
			this->lvDetThread->SetSubItem(k, 1, CSTRP(sbuff, sptr));

			if (this->currProcRes.SetTo(currProcRes))
			{
				sbuff[0] = 0;
				sptr = currProcRes->ResolveName(sbuff, addr).Or(sbuff);
				l = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), '\\');
				this->lvDetThread->SetSubItem(k, 2, CSTRP(&sbuff[l + 1], sptr));
			}
			t.Delete();
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
		UnsafeArray<UTF8Char> sptr;
		UIntOS i;
		UIntOS j;

		proc.GetHeapLists(heapList);

		this->lbDetHeap->ClearItems();
		i = 0;
		j = heapList.GetCount();
		while (i < j)
		{
			sptr = Text::StrUInt32(sbuff, heapList.GetItem(i));
			this->lbDetHeap->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)heapList.GetItem(i));
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
		Data::ArrayListNN<Manage::Process::HeapInfo> heapList;
		NN<Manage::Process::HeapInfo> heap;
		UTF8Char sbuff[20];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i;
		UIntOS j;
		UIntOS k;
		Text::CStringNN tStr;

		proc.GetHeaps(heapList, heapId, 50);

		this->lvDetHeap->ClearItems();
		i = 0;
		j = heapList.GetCount();
		while (i < j)
		{
			heap = heapList.GetItemNoCheck(i);
			sptr = Text::StrHexValOS(Text::StrConcatC(sbuff, UTF8STRC("0x")), heap->startAddr);
			k = this->lvDetHeap->AddItem(CSTRP(sbuff, sptr), 0, 0);
			sptr = Text::StrUIntOS(sbuff, heap->size);
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
		proc.FreeHeaps(heapList);
	}
	
}

SSWR::AVIReadCE::AVIRCEProcInfoForm::AVIRCEProcInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 240, 320, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Process Info"));
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	this->core = core;
	this->lastUserTime = 0;
	this->lastKernelTime = 0;

	this->currProc = 0;
	this->currProcObj = 0;
	this->currProcRes = 0;
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpSummary = this->tcMain->AddTabPage(CSTR("Summary"));
	this->tpDetail = this->tcMain->AddTabPage(CSTR("Detail"));

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

	this->lbDetail = ui->NewListBox(this->tpDetail, false);
	this->lbDetail->SetRect(0, 0, 80, 100, false);
	this->lbDetail->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDetail->HandleSelectionChange(OnProcSelChg, this);
	this->hspDetail = ui->NewHSplitter(this->tpDetail, 3, false);
	this->tcDetail = ui->NewTabControl(this->tpDetail);
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

	this->pnlDetThread = ui->NewPanel(this->tpDetThread);
	this->pnlDetThread->SetRect(0, 0, 100, 31, false);
	this->pnlDetThread->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDetThread = ui->NewButton(this->pnlDetThread, CSTR("Refresh"));
	this->btnDetThread->SetRect(4, 4, 75, 23, false);
	this->btnDetThread->HandleButtonClick(OnDetThreadRefClicked, this);
	this->lvDetThread = ui->NewListView(this->tpDetThread, UI::ListViewStyle::Table, 3);
	this->lvDetThread->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetThread->HandleDblClk(OnDetThreadDblClicked, this);
	this->lvDetThread->SetFullRowSelect(true);
	this->lvDetThread->SetShowGrid(true);
	this->lvDetThread->AddColumn(CSTR("Id"), 60);
	this->lvDetThread->AddColumn(CSTR("Start Address"), 120);
	this->lvDetThread->AddColumn(CSTR("Start Address(Name)"), 600);

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
	this->lvDetHeap = ui->NewListView(this->tpDetHeap, UI::ListViewStyle::Table, 3);
	this->lvDetHeap->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetHeap->SetFullRowSelect(true);
	this->lvDetHeap->SetShowGrid(true);
	this->lvDetHeap->AddColumn(CSTR("Start Address"), 80);
	this->lvDetHeap->AddColumn(CSTR("Size"), 60);
	this->lvDetHeap->AddColumn(CSTR("Tyep"), 80);

	this->grpDetChartCPU = ui->NewGroupBox(this->tpDetChart, CSTR("CPU"));
	this->grpDetChartCPU->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCPU->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcDetChartCPU = ui->NewRealtimeLineChart(this->grpDetChartCPU, this->core->GetDrawEngine(), 1, 600, 300);
	this->rlcDetChartCPU->SetDockType(UI::GUIControl::DOCK_FILL);
	this->rlcDetChartCPU->SetUnit(CSTR("%"));
	this->vspDetChartCPU = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartPage = ui->NewGroupBox(this->tpDetChart, CSTR("Paged(R)/Non-Paged(B) Pool"));
	this->grpDetChartPage->SetRect(0, 0, 100, 200, false);
	this->grpDetChartPage->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcDetChartPage = ui->NewRealtimeLineChart(this->grpDetChartPage, this->core->GetDrawEngine(), 2, 600, 300);
	this->rlcDetChartPage->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspDetChartPage = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartCount = ui->NewGroupBox(this->tpDetChart, CSTR("GDI(R)/User(B)/Handle(G) Count"));
	this->grpDetChartCount->SetRect(0, 0, 100, 200, false);
	this->grpDetChartCount->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcDetChartCount = ui->NewRealtimeLineChart(this->grpDetChartCount, this->core->GetDrawEngine(), 3, 600, 300);
	this->rlcDetChartCount->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspDetChartCount = ui->NewVSplitter(this->tpDetChart, 3, false);
	this->grpDetChartWS = ui->NewGroupBox(this->tpDetChart, CSTR("WS(R)/Page File(B)"));
	this->grpDetChartWS->SetRect(0, 0, 100, 200, false);
	this->grpDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->rlcDetChartWS = ui->NewRealtimeLineChart(this->grpDetChartWS, this->core->GetDrawEngine(), 2, 600, 300);
	this->rlcDetChartWS->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(1000, OnTimerTick, this);
	this->AddTimer(300, OnTimerCPUTick, this);
	OnTimerTick(this);
}

SSWR::AVIReadCE::AVIRCEProcInfoForm::~AVIRCEProcInfoForm()
{
	NN<ProcessInfo> procInfo;
	IntOS i;
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
