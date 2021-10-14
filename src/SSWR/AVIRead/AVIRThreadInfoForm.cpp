#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/DasmX86_32.h"
#include "Manage/DasmX86_64.h"
#include "Manage/StackTracer.h"
#if defined(CPU_X86_32) || (defined(CPU_X86_64) && defined(WIN32))
#include "Manage/ThreadContextX86_32.h"
#endif
#if defined(CPU_X86_64)
#include "Manage/ThreadContextX86_64.h"
#endif
#include "SSWR/AVIRead/AVIRFunctionInfoForm.h"
#include "SSWR/AVIRead/AVIRThreadInfoForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderWriter.h"

void __stdcall SSWR::AVIRead::AVIRThreadInfoForm::OnMyStackChg(void *userObj)
{
	SSWR::AVIRead::AVIRThreadInfoForm *me = (SSWR::AVIRead::AVIRThreadInfoForm*)userObj;
	UOSInt i = me->lbMyStack->GetSelectedIndex();
	const UTF8Char *s = me->stacks->GetItem(i);
	const UTF8Char *sMem = me->stacksMem->GetItem(i);
	UOSInt slen;
	UTF8Char *sbuff;
	UTF8Char *sline[2];
	UTF8Char *sarr[18];
	Bool hasNext;

	if (sMem)
	{
		me->txtMyStackMem->SetText(sMem);
	}
	else
	{
		me->txtMyStackMem->SetText((const UTF8Char*)"");
	}
	me->lvMyStack->ClearItems();
	if (s)
	{
		slen = Text::StrCharCnt(s);
		sbuff = MemAlloc(UTF8Char, slen + 1);
		Text::StrConcat(sbuff, s);
		sline[1] = sbuff;

		if (me->contextType == Manage::ThreadContext::CT_X86_32)
		{
			while (true)
			{
				hasNext = Text::StrSplit(sline, 2, sline[1], '\r') == 2;
				if (Text::StrSplit(sarr, 10, sline[0], ' ') == 10)
				{
					i = me->lvMyStack->AddItem(sarr[0], 0);
					me->lvMyStack->SetSubItem(i, 1, sarr[1]);
					me->lvMyStack->SetSubItem(i, 2, sarr[2]);
					me->lvMyStack->SetSubItem(i, 3, sarr[9]);
					me->lvMyStack->SetSubItem(i, 4, sarr[3]);
					me->lvMyStack->SetSubItem(i, 5, sarr[4]);
					me->lvMyStack->SetSubItem(i, 6, sarr[5]);
					me->lvMyStack->SetSubItem(i, 7, sarr[6]);
					me->lvMyStack->SetSubItem(i, 8, sarr[7]);
					me->lvMyStack->SetSubItem(i, 9, sarr[8]);
				}
				if (!hasNext)
					break;
			}
		}
		else if (me->contextType == Manage::ThreadContext::CT_X86_64)
		{
			while (true)
			{
				hasNext = Text::StrSplit(sline, 2, sline[1], '\r') == 2;
				if (Text::StrSplit(sarr, 18, sline[0], ' ') == 18)
				{
					i = me->lvMyStack->AddItem(sarr[0], 0);
					me->lvMyStack->SetSubItem(i, 1, sarr[1]);
					me->lvMyStack->SetSubItem(i, 2, sarr[2]);
					me->lvMyStack->SetSubItem(i, 3, sarr[17]);
					me->lvMyStack->SetSubItem(i, 4, sarr[3]);
					me->lvMyStack->SetSubItem(i, 5, sarr[4]);
					me->lvMyStack->SetSubItem(i, 6, sarr[5]);
					me->lvMyStack->SetSubItem(i, 7, sarr[6]);
					me->lvMyStack->SetSubItem(i, 8, sarr[7]);
					me->lvMyStack->SetSubItem(i, 9, sarr[8]);
					me->lvMyStack->SetSubItem(i, 10, sarr[9]);
					me->lvMyStack->SetSubItem(i, 11, sarr[10]);
					me->lvMyStack->SetSubItem(i, 12, sarr[11]);
					me->lvMyStack->SetSubItem(i, 13, sarr[12]);
					me->lvMyStack->SetSubItem(i, 14, sarr[13]);
					me->lvMyStack->SetSubItem(i, 15, sarr[14]);
					me->lvMyStack->SetSubItem(i, 16, sarr[15]);
					me->lvMyStack->SetSubItem(i, 17, sarr[16]);
				}
				if (!hasNext)
					break;
			}
		}
		MemFree(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRThreadInfoForm::OnMyStackDblClk(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRThreadInfoForm *me = (SSWR::AVIRead::AVIRThreadInfoForm*)userObj;
	UTF8Char sbuff[18];
	UOSInt i;
	Int64 funcOfst;
	Text::StringBuilderUTF8 sb;
	me->lvMyStack->GetSubItem(index, 3, &sb);
	if (sb.StartsWith((const UTF8Char*)"call 0x"))
	{
		Text::StrConcatS(sbuff, sb.ToString() + 7, 17);
		i = Text::StrIndexOf(sbuff, ' ');
		if (i != INVALID_INDEX && i > 0)
		{
			sbuff[i] = 0;
		}
		funcOfst = Text::StrHex2Int64C(sbuff);

		SSWR::AVIRead::AVIRFunctionInfoForm *frm;
		NEW_CLASS(frm, SSWR::AVIRead::AVIRFunctionInfoForm(0, me->ui, me->core, me->proc, me->symbol, funcOfst));
		frm->ShowDialog(me);
		DEL_CLASS(frm);
	}
}

SSWR::AVIRead::AVIRThreadInfoForm::AVIRThreadInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Manage::Process *proc, Manage::SymbolResolver *symbol, UInt32 threadId) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Thread Info");

	this->core = core;
	NEW_CLASS(this->stacks, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->stacksMem, Data::ArrayList<const UTF8Char *>());
	this->proc = proc;
	this->symbol = symbol;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpInfo = this->tcMain->AddTabPage((const UTF8Char*)"Info");
	this->tpContext = this->tcMain->AddTabPage((const UTF8Char*)"Context");
	this->tpStack = this->tcMain->AddTabPage((const UTF8Char*)"Stack");
	this->tpMyStack = this->tcMain->AddTabPage((const UTF8Char*)"My Stack");

	NEW_CLASS(this->lblThreadId, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Thread Id"));
	this->lblThreadId->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtThreadId, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)"", false));
	this->txtThreadId->SetRect(100, 0, 100, 23, false);
	this->txtThreadId->SetReadOnly(true);
	NEW_CLASS(this->lblStartAddr, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Start Address"));
	this->lblStartAddr->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtStartAddr, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)"", false));
	this->txtStartAddr->SetRect(100, 24, 120, 23, false);
	this->txtStartAddr->SetReadOnly(true);
	NEW_CLASS(this->lblStartName, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Start Name"));
	this->lblStartName->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtStartName, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)"", false));
	this->txtStartName->SetRect(100, 48, 500, 23, false);
	this->txtStartName->SetReadOnly(true);

	NEW_CLASS(this->lvContext, UI::GUIListView(ui, this->tpContext, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvContext->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContext->SetFullRowSelect(true);
	this->lvContext->SetShowGrid(true);
	this->lvContext->AddColumn((const UTF8Char*)"Name", 100);
	this->lvContext->AddColumn((const UTF8Char*)"Value", 300);

	NEW_CLASS(this->lvStack, UI::GUIListView(ui, this->tpStack, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStack->SetFullRowSelect(true);
	this->lvStack->SetShowGrid(true);
	this->lvStack->AddColumn((const UTF8Char*)"Address", 120);
	this->lvStack->AddColumn((const UTF8Char*)"Name", 500);

	NEW_CLASS(this->lbMyStack, UI::GUIListBox(ui, this->tpMyStack, false));
	this->lbMyStack->SetRect(0, 0, 300, 23, false);
	this->lbMyStack->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbMyStack->HandleSelectionChange(OnMyStackChg, this);
	NEW_CLASS(this->hspMyStack, UI::GUIHSplitter(ui, this->tpMyStack, 3, false));
	NEW_CLASS(this->pnlMyStack, UI::GUIPanel(ui, this->tpMyStack));
	this->pnlMyStack->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->txtMyStackMem, UI::GUITextBox(ui, this->pnlMyStack, (const UTF8Char*)"", true));
	this->txtMyStackMem->SetRect(0, 0, 100, 200, false);
	this->txtMyStackMem->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtMyStackMem->SetReadOnly(true);
	NEW_CLASS(this->vspMyStack, UI::GUIVSplitter(ui, this->pnlMyStack, 3, false));
	NEW_CLASS(this->lvMyStack, UI::GUIListView(ui, this->pnlMyStack, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvMyStack->SetShowGrid(true);
	this->lvMyStack->SetFullRowSelect(true);
	this->lvMyStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMyStack->HandleDblClk(OnMyStackDblClk, this);

	Manage::ThreadInfo *thread;
	Manage::ThreadContext *context;
	UTF8Char sbuff[512];
	UInt64 startAddr;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(thread, Manage::ThreadInfo(proc->GetProcId(), threadId));

	startAddr = thread->GetStartAddress();
	Text::StrUInt32(sbuff, threadId);
	this->txtThreadId->SetText(sbuff);
	Text::StrHexVal64(sbuff, startAddr);
	this->txtStartAddr->SetText(sbuff);
	symbol->ResolveName(sbuff, startAddr);
	i = Text::StrLastIndexOf(sbuff, '\\');
	this->txtStartName->SetText(&sbuff[i + 1]);

	if (thread->IsCurrThread())
	{
	}
	else
	{
		Manage::StackTracer *tracer;
		UInt64 currAddr;
		UOSInt callLev;
		thread->Suspend();
		context = thread->GetThreadContext();
		NEW_CLASS(tracer, Manage::StackTracer(context));
		callLev = 0;
		while (true)
		{
			currAddr = tracer->GetCurrentAddr();
			Text::StrHexVal64(sbuff, currAddr);
			i = this->lvStack->AddItem(sbuff, 0, 0);
			symbol->ResolveName(sbuff, currAddr);
			j = Text::StrLastIndexOf(sbuff, '\\');
			this->lvStack->SetSubItem(i, 1, &sbuff[j + 1]);
			if (!tracer->GoToNextLevel())
				break;
			if (++callLev > 50)
				break;
		}
		DEL_CLASS(tracer);

#if defined(CPU_X86_32) || (defined(CPU_X86_64) && defined(WIN32))
		if (context->GetType() == Manage::ThreadContext::CT_X86_32)
		{
			Manage::DasmX86_32 *dasm;
			UInt32 eip;
			UInt32 esp;
			UInt32 ebp;
			UInt8 buff[256];
			UOSInt buffSize;
			Bool ret;
			Data::ArrayListUInt32 *callAddrs;
			Data::ArrayListUInt32 *jmpAddrs;
			UInt32 blockStart;
			UInt32 blockEnd;
			Text::StringBuilderUTF8 sb;

			this->lvMyStack->ChangeColumnCnt(10);
			this->lvMyStack->AddColumn((const UTF8Char*)"Esp", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Ebp", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Eip", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Code", 500);
			this->lvMyStack->AddColumn((const UTF8Char*)"Eax", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Edx", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Ecx", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Ebx", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Esi", 70);
			this->lvMyStack->AddColumn((const UTF8Char*)"Edi", 70);
			this->contextType = Manage::ThreadContext::CT_X86_32;

			NEW_CLASS(callAddrs, Data::ArrayListUInt32());
			NEW_CLASS(jmpAddrs, Data::ArrayListUInt32());
			eip = (UInt32)context->GetInstAddr();
			esp = (UInt32)context->GetStackAddr();
			ebp = (UInt32)context->GetFrameAddr();
			NEW_CLASS(dasm, Manage::DasmX86_32());

			Text::StringBuilderWriter sbWriter(&sb);
			Manage::DasmX86_32::DasmX86_32_Regs regs;
			context->GetRegs(&regs);
			callLev = 0;
			while (true)
			{
				if (eip == 0)
					break;
				sb.ClearStr();
				sb.AppendHex32(eip);
				sb.Append((const UTF8Char*)" ");
				symbol->ResolveName(sbuff, eip);
				i  = Text::StrLastIndexOf(sbuff, '\\');
				sb.Append(&sbuff[i + 1]);
				i = this->lbMyStack->AddItem(sb.ToString(), 0);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"EIP = 0x");
				sb.AppendHex32(eip);
				sb.Append((const UTF8Char*)"\r\n");
				buffSize = proc->ReadMemory(eip, buff, 256);
				if (buffSize > 0)
				{
					sb.AppendHex(buff, buffSize, ' ', Text::LineBreakType::CRLF);
					sb.Append((const UTF8Char*)"\r\n");
				}

				sb.Append((const UTF8Char*)"\r\n");
				sb.Append((const UTF8Char*)"ESP = 0x");
				sb.AppendHex32(esp);
				sb.Append((const UTF8Char*)"\r\n");
				buffSize = proc->ReadMemory(esp, buff, 256);
				if (buffSize > 0)
				{
					sb.AppendHex(buff, buffSize, ' ', Text::LineBreakType::CRLF);
					sb.Append((const UTF8Char*)"\r\n");
				}
				this->stacksMem->Add(Text::StrCopyNew(sb.ToString()));

				sb.ClearStr();
				ret = dasm->Disasm32(&sbWriter, symbol, &eip, &esp, &ebp, callAddrs, jmpAddrs, &blockStart, &blockEnd, &regs, proc, true);
				this->stacks->Add(Text::StrCopyNew(sb.ToString()));
				if (!ret)
					break;
				if (++callLev > 50)
					break;
			}
			
			DEL_CLASS(dasm);
			DEL_CLASS(jmpAddrs);
			DEL_CLASS(callAddrs);
		}
#endif
#if defined(CPU_X86_64)
		if (context->GetType() == Manage::ThreadContext::CT_X86_64)
		{
			Manage::DasmX86_64 *dasm;
			UInt64 rip;
			UInt64 rsp;
			UInt64 rbp;
			UInt8 buff[256];
			UOSInt buffSize;
			Bool ret;
			Data::ArrayListUInt64 *callAddrs;
			Data::ArrayListUInt64 *jmpAddrs;
			UInt64 blockStart;
			UInt64 blockEnd;
			Text::StringBuilderUTF8 sb;

			this->lvMyStack->ChangeColumnCnt(18);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rsp", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rbp", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rip", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Code", 500);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rax", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rdx", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rcx", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rbx", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rsi", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"Rdi", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R8", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R9", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R10", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R11", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R12", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R13", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R14", 140);
			this->lvMyStack->AddColumn((const UTF8Char*)"R15", 140);
			this->contextType = Manage::ThreadContext::CT_X86_64;

			NEW_CLASS(callAddrs, Data::ArrayListUInt64());
			NEW_CLASS(jmpAddrs, Data::ArrayListUInt64());
			rip = context->GetInstAddr();
			rsp = context->GetStackAddr();
			rbp = context->GetFrameAddr();

			NEW_CLASS(dasm, Manage::DasmX86_64());

			Text::StringBuilderWriter sbWriter(&sb);
			Manage::DasmX86_64::DasmX86_64_Regs regs;
			context->GetRegs(&regs);
			callLev = 0;
			while (true)
			{
				if (rip == 0)
					break;
				sb.ClearStr();
				sb.AppendHex64(rip);
				sb.Append((const UTF8Char*)" ");
				symbol->ResolveName(sbuff, rip);
				i  = Text::StrLastIndexOf(sbuff, '\\');
				sb.Append(&sbuff[i + 1]);
				i = this->lbMyStack->AddItem(sb.ToString(), 0);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"RIP = 0x");
				sb.AppendHex64(rip);
				sb.Append((const UTF8Char*)"\r\n");
				buffSize = proc->ReadMemory(rip, buff, 256);
				if (buffSize > 0)
				{
					sb.AppendHex(buff, buffSize, ' ', Text::LineBreakType::CRLF);
					sb.Append((const UTF8Char*)"\r\n");
				}

				sb.Append((const UTF8Char*)"\r\n");
				sb.Append((const UTF8Char*)"RSP = 0x");
				sb.AppendHex64(rsp);
				sb.Append((const UTF8Char*)"\r\n");
				buffSize = proc->ReadMemory(rsp, buff, 256);
				if (buffSize > 0)
				{
					sb.AppendHex(buff, buffSize, ' ', Text::LineBreakType::CRLF);
					sb.Append((const UTF8Char*)"\r\n");
				}
				this->stacksMem->Add(Text::StrCopyNew(sb.ToString()));

				sb.ClearStr();
				ret = dasm->Disasm64(&sbWriter, symbol, &rip, &rsp, &rbp, callAddrs, jmpAddrs, &blockStart, &blockEnd, &regs, proc, true);
				this->stacks->Add(Text::StrCopyNew(sb.ToString()));
				if (!ret)
					break;
				if (++callLev > 50)
					break;
			}
			
			DEL_CLASS(dasm);
			DEL_CLASS(jmpAddrs);
			DEL_CLASS(callAddrs);
		}
#endif

		thread->Resume();
		if (context)
		{
			UInt8 buff[16];
			UTF8Char sbuff[64];
			UInt32 bitCnt;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			i = 0;
			j = context->GetRegisterCnt();
			while (i < j)
			{
				context->GetRegister(i, sbuff, buff, &bitCnt);
				k = this->lvContext->AddItem(sbuff, 0);
				if (bitCnt == 8)
				{
					Text::StrHexByte(sbuff, buff[0]);
					this->lvContext->SetSubItem(k, 1, sbuff);
				}
				else if (bitCnt == 16)
				{
					Text::StrHexVal16(sbuff, ReadUInt16(buff));
					this->lvContext->SetSubItem(k, 1, sbuff);
				}
				else if (bitCnt == 32)
				{
					Text::StrHexVal32(sbuff, ReadUInt32(buff));
					this->lvContext->SetSubItem(k, 1, sbuff);
				}
				else if (bitCnt == 64)
				{
					Text::StrHexVal64(sbuff, ReadUInt64(buff));
					this->lvContext->SetSubItem(k, 1, sbuff);
				}
				else if (bitCnt == 128)
				{
					Text::StrHexVal64(Text::StrHexVal64(sbuff, ReadUInt64(buff)), ReadUInt64(&buff[8]));
					this->lvContext->SetSubItem(k, 1, sbuff);
				}
				else if (bitCnt == 80)
				{
					Text::StrDouble(sbuff, (Double)*(LDouble*)&buff[0]);
					this->lvContext->SetSubItem(k, 1, sbuff);
				}
				else
				{
					bitCnt = 0;
				}
				i++;
			}

			DEL_CLASS(context);
		}
	}
	DEL_CLASS(thread);
}

SSWR::AVIRead::AVIRThreadInfoForm::~AVIRThreadInfoForm()
{
	UOSInt i;
	i = this->stacks->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->stacks->GetItem(i));
		Text::StrDelNew(this->stacksMem->GetItem(i));
	}
	DEL_CLASS(this->stacks);
	DEL_CLASS(this->stacksMem);
}

void SSWR::AVIRead::AVIRThreadInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
