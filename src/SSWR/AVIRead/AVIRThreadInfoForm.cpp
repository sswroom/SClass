#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/DasmX86_32.h"
#include "Manage/DasmX86_64.h"
#include "Manage/DasmARM64.h"
#include "Manage/StackTracer.h"
#if defined(CPU_ARM64)
#include "Manage/ThreadContextARM64.h"
#endif
#if defined(CPU_X86_32) || (defined(CPU_X86_64) && defined(WIN32)) || (defined(CPU_ARM64) && defined(_WIN64))
#include "Manage/ThreadContextX86_32.h"
#endif
#if defined(CPU_X86_64) || (defined(CPU_ARM64) && defined(_WIN64))
#include "Manage/ThreadContextX86_64.h"
#endif
#include "SSWR/AVIRead/AVIRFunctionInfoForm.h"
#include "SSWR/AVIRead/AVIRThreadInfoForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderWriter.h"

void __stdcall SSWR::AVIRead::AVIRThreadInfoForm::OnMyStackChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRThreadInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRThreadInfoForm>();
	UOSInt i = me->lbMyStack->GetSelectedIndex();
	const UTF8Char *s = me->stacks.GetItem(i).OrNull();
	const UTF8Char *sMem = me->stacksMem.GetItem(i).OrNull();
	UOSInt slen;
	UTF8Char *sbuff;
	Text::PString sline[2];
	Text::PString sarr[34];
	Bool hasNext;

	if (sMem)
	{
		me->txtMyStackMem->SetText({sMem, Text::StrCharCnt(sMem)});
	}
	else
	{
		me->txtMyStackMem->SetText(CSTR(""));
	}
	me->lvMyStack->ClearItems();
	if (s)
	{
		slen = Text::StrCharCnt(s);
		sbuff = MemAlloc(UTF8Char, slen + 1);
		Text::StrConcatC(sbuff, s, slen);
		sline[1].v = sbuff;
		sline[1].leng = slen;

		if (me->contextType == Manage::ThreadContext::ContextType::X86_32)
		{
			while (true)
			{
				hasNext = Text::StrSplitP(sline, 2, sline[1], '\r') == 2;
				if (Text::StrSplitP(sarr, 10, sline[0], ' ') == 10)
				{
					i = me->lvMyStack->AddItem(sarr[0].ToCString(), 0);
					me->lvMyStack->SetSubItem(i, 1, sarr[1].ToCString());
					me->lvMyStack->SetSubItem(i, 2, sarr[2].ToCString());
					me->lvMyStack->SetSubItem(i, 3, sarr[9].ToCString());
					me->lvMyStack->SetSubItem(i, 4, sarr[3].ToCString());
					me->lvMyStack->SetSubItem(i, 5, sarr[4].ToCString());
					me->lvMyStack->SetSubItem(i, 6, sarr[5].ToCString());
					me->lvMyStack->SetSubItem(i, 7, sarr[6].ToCString());
					me->lvMyStack->SetSubItem(i, 8, sarr[7].ToCString());
					me->lvMyStack->SetSubItem(i, 9, sarr[8].ToCString());
				}
				if (!hasNext)
					break;
			}
		}
		else if (me->contextType == Manage::ThreadContext::ContextType::X86_64)
		{
			while (true)
			{
				hasNext = Text::StrSplitP(sline, 2, sline[1], '\r') == 2;
				if (Text::StrSplitP(sarr, 18, sline[0], ' ') == 18)
				{
					i = me->lvMyStack->AddItem(sarr[0].ToCString(), 0);
					me->lvMyStack->SetSubItem(i, 1, sarr[1].ToCString());
					me->lvMyStack->SetSubItem(i, 2, sarr[2].ToCString());
					me->lvMyStack->SetSubItem(i, 3, sarr[17].ToCString());
					me->lvMyStack->SetSubItem(i, 4, sarr[3].ToCString());
					me->lvMyStack->SetSubItem(i, 5, sarr[4].ToCString());
					me->lvMyStack->SetSubItem(i, 6, sarr[5].ToCString());
					me->lvMyStack->SetSubItem(i, 7, sarr[6].ToCString());
					me->lvMyStack->SetSubItem(i, 8, sarr[7].ToCString());
					me->lvMyStack->SetSubItem(i, 9, sarr[8].ToCString());
					me->lvMyStack->SetSubItem(i, 10, sarr[9].ToCString());
					me->lvMyStack->SetSubItem(i, 11, sarr[10].ToCString());
					me->lvMyStack->SetSubItem(i, 12, sarr[11].ToCString());
					me->lvMyStack->SetSubItem(i, 13, sarr[12].ToCString());
					me->lvMyStack->SetSubItem(i, 14, sarr[13].ToCString());
					me->lvMyStack->SetSubItem(i, 15, sarr[14].ToCString());
					me->lvMyStack->SetSubItem(i, 16, sarr[15].ToCString());
					me->lvMyStack->SetSubItem(i, 17, sarr[16].ToCString());
				}
				if (!hasNext)
					break;
			}
		}
		else if (me->contextType == Manage::ThreadContext::ContextType::ARM64)
		{
			while (true)
			{
				hasNext = Text::StrSplitP(sline, 2, sline[1], '\r') == 2;
				if (Text::StrSplitP(sarr, 34, sline[0], ' ') == 34)
				{
					i = me->lvMyStack->AddItem(sarr[0].ToCString(), 0);
					me->lvMyStack->SetSubItem(i, 1, sarr[1].ToCString());
					me->lvMyStack->SetSubItem(i, 2, sarr[2].ToCString());
					me->lvMyStack->SetSubItem(i, 3, sarr[33].ToCString());
					me->lvMyStack->SetSubItem(i, 4, sarr[3].ToCString());
					me->lvMyStack->SetSubItem(i, 5, sarr[4].ToCString());
					me->lvMyStack->SetSubItem(i, 6, sarr[5].ToCString());
					me->lvMyStack->SetSubItem(i, 7, sarr[6].ToCString());
					me->lvMyStack->SetSubItem(i, 8, sarr[7].ToCString());
					me->lvMyStack->SetSubItem(i, 9, sarr[8].ToCString());
					me->lvMyStack->SetSubItem(i, 10, sarr[9].ToCString());
					me->lvMyStack->SetSubItem(i, 11, sarr[10].ToCString());
					me->lvMyStack->SetSubItem(i, 12, sarr[11].ToCString());
					me->lvMyStack->SetSubItem(i, 13, sarr[12].ToCString());
					me->lvMyStack->SetSubItem(i, 14, sarr[13].ToCString());
					me->lvMyStack->SetSubItem(i, 15, sarr[14].ToCString());
					me->lvMyStack->SetSubItem(i, 16, sarr[15].ToCString());
					me->lvMyStack->SetSubItem(i, 17, sarr[16].ToCString());
					me->lvMyStack->SetSubItem(i, 18, sarr[17].ToCString());
					me->lvMyStack->SetSubItem(i, 19, sarr[18].ToCString());
					me->lvMyStack->SetSubItem(i, 20, sarr[19].ToCString());
					me->lvMyStack->SetSubItem(i, 21, sarr[20].ToCString());
					me->lvMyStack->SetSubItem(i, 22, sarr[21].ToCString());
					me->lvMyStack->SetSubItem(i, 23, sarr[22].ToCString());
					me->lvMyStack->SetSubItem(i, 24, sarr[23].ToCString());
					me->lvMyStack->SetSubItem(i, 25, sarr[24].ToCString());
					me->lvMyStack->SetSubItem(i, 26, sarr[25].ToCString());
					me->lvMyStack->SetSubItem(i, 27, sarr[26].ToCString());
					me->lvMyStack->SetSubItem(i, 28, sarr[27].ToCString());
					me->lvMyStack->SetSubItem(i, 29, sarr[28].ToCString());
					me->lvMyStack->SetSubItem(i, 30, sarr[29].ToCString());
					me->lvMyStack->SetSubItem(i, 31, sarr[30].ToCString());
					me->lvMyStack->SetSubItem(i, 32, sarr[31].ToCString());
					me->lvMyStack->SetSubItem(i, 33, sarr[32].ToCString());
				}
				if (!hasNext)
					break;
			}
		}
		MemFree(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRThreadInfoForm::OnMyStackDblClk(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRThreadInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRThreadInfoForm>();
	UTF8Char sbuff[18];
	UOSInt i;
	Int64 funcOfst;
	Text::StringBuilderUTF8 sb;
	me->lvMyStack->GetSubItem(index, 3, sb);
	if (sb.StartsWith(UTF8STRC("call 0x")))
	{
		Text::StrConcatS(sbuff, sb.ToString() + 7, 17);
		i = Text::StrIndexOfChar(sbuff, ' ');
		if (i != INVALID_INDEX && i > 0)
		{
			sbuff[i] = 0;
		}
		funcOfst = Text::StrHex2Int64C(sbuff);

		SSWR::AVIRead::AVIRFunctionInfoForm frm(0, me->ui, me->core, me->proc, me->symbol, funcOfst);
		frm.ShowDialog(me);
	}
}

SSWR::AVIRead::AVIRThreadInfoForm::AVIRThreadInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Manage::Process *proc, Manage::SymbolResolver *symbol, UInt32 threadId) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Thread Info"));

	this->core = core;
	this->proc = proc;
	this->symbol = symbol;
	this->contextType = Manage::ThreadContext::ContextType::Unknown;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	Manage::ThreadInfo thread(proc->GetProcId(), threadId);
	NN<Manage::ThreadContext> context;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt64 startAddr;
	UOSInt i;
	UOSInt j;
	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lblThreadId = ui->NewLabel(this->tpInfo, CSTR("Thread Id"));
	this->lblThreadId->SetRect(0, 0, 100, 23, false);
	this->txtThreadId = ui->NewTextBox(this->tpInfo, CSTR(""), false);
	this->txtThreadId->SetRect(100, 0, 100, 23, false);
	this->txtThreadId->SetReadOnly(true);
	this->lblStartAddr = ui->NewLabel(this->tpInfo, CSTR("Start Address"));
	this->lblStartAddr->SetRect(0, 24, 100, 23, false);
	this->txtStartAddr = ui->NewTextBox(this->tpInfo, CSTR(""), false);
	this->txtStartAddr->SetRect(100, 24, 120, 23, false);
	this->txtStartAddr->SetReadOnly(true);
	this->lblStartName = ui->NewLabel(this->tpInfo, CSTR("Start Name"));
	this->lblStartName->SetRect(0, 48, 100, 23, false);
	this->txtStartName = ui->NewTextBox(this->tpInfo, CSTR(""), false);
	this->txtStartName->SetRect(100, 48, 500, 23, false);
	this->txtStartName->SetReadOnly(true);
	this->lblThreadName = ui->NewLabel(this->tpInfo, CSTR("Thread Name"));
	this->lblThreadName->SetRect(0, 72, 100, 23, false);
	this->txtThreadName = ui->NewTextBox(this->tpInfo, CSTR(""), false);
	this->txtThreadName->SetRect(100, 72, 500, 23, false);
	this->txtThreadName->SetReadOnly(true);

	startAddr = thread.GetStartAddress();
	sptr = Text::StrUInt32(sbuff, threadId);
	this->txtThreadId->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal64(sbuff, startAddr);
	this->txtStartAddr->SetText(CSTRP(sbuff, sptr));
	sptr = symbol->ResolveName(sbuff, startAddr);
	if (sptr)
	{
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
		this->txtStartName->SetText(CSTRP(&sbuff[i + 1], sptr));
	}
	sptr = thread.GetName(sbuff);
	if (sptr)
	{
		this->txtThreadName->SetText(CSTRP(sbuff, sptr));
	}

	this->tpContext = this->tcMain->AddTabPage(CSTR("Context"));
	this->lvContext = ui->NewListView(this->tpContext, UI::ListViewStyle::Table, 2);
	this->lvContext->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContext->SetFullRowSelect(true);
	this->lvContext->SetShowGrid(true);
	this->lvContext->AddColumn(CSTR("Name"), 100);
	this->lvContext->AddColumn(CSTR("Value"), 300);

	this->tpStack = this->tcMain->AddTabPage(CSTR("Stack"));
	this->lvStack = ui->NewListView(this->tpStack, UI::ListViewStyle::Table, 2);
	this->lvStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStack->SetFullRowSelect(true);
	this->lvStack->SetShowGrid(true);
	this->lvStack->AddColumn(CSTR("Address"), 120);
	this->lvStack->AddColumn(CSTR("Name"), 500);

	this->tpMyStack = this->tcMain->AddTabPage(CSTR("My Stack"));
	this->lbMyStack = ui->NewListBox(this->tpMyStack, false);
	this->lbMyStack->SetRect(0, 0, 300, 23, false);
	this->lbMyStack->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbMyStack->HandleSelectionChange(OnMyStackChg, this);
	this->hspMyStack = ui->NewHSplitter(this->tpMyStack, 3, false);
	this->pnlMyStack = ui->NewPanel(this->tpMyStack);
	this->pnlMyStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtMyStackMem = ui->NewTextBox(this->pnlMyStack, CSTR(""), true);
	this->txtMyStackMem->SetRect(0, 0, 100, 200, false);
	this->txtMyStackMem->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtMyStackMem->SetReadOnly(true);
	this->vspMyStack = ui->NewVSplitter(this->pnlMyStack, 3, false);
	this->lvMyStack = ui->NewListView(this->pnlMyStack, UI::ListViewStyle::Table, 1);
	this->lvMyStack->SetShowGrid(true);
	this->lvMyStack->SetFullRowSelect(true);
	this->lvMyStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMyStack->HandleDblClk(OnMyStackDblClk, this);

	if (thread.IsCurrThread())
	{
	}
	else
	{
		UInt64 currAddr;
		UOSInt callLev;
		thread.Suspend();
		if (context.Set(thread.GetThreadContext()))
		{
			Manage::StackTracer tracer(context);
			callLev = 0;
			while (true)
			{
				currAddr = tracer.GetCurrentAddr();
				sptr = Text::StrHexVal64(sbuff, currAddr);
				i = this->lvStack->AddItem(CSTRP(sbuff, sptr), 0, 0);
				sptr = symbol->ResolveName(sbuff, currAddr);
				if (sptr)
				{
					j = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
					this->lvStack->SetSubItem(i, 1, CSTRP(&sbuff[j + 1], sptr));
				}
				if (!tracer.GoToNextLevel())
					break;
				if (++callLev > 50)
					break;
			}

#if defined(CPU_X86_32) || (defined(CPU_X86_64) && defined(WIN32)) || (defined(CPU_ARM64) && defined(_WIN64))
			if (context->GetType() == Manage::ThreadContext::ContextType::X86_32)
			{
				UInt32 eip;
				UInt32 esp;
				UInt32 ebp;
				UInt8 buff[256];
				UOSInt buffSize;
				Bool ret;
				UInt32 blockStart;
				UInt32 blockEnd;
				Text::StringBuilderUTF8 sb;

				this->lvMyStack->ChangeColumnCnt(10);
				this->lvMyStack->AddColumn(CSTR("Esp"), 70);
				this->lvMyStack->AddColumn(CSTR("Ebp"), 70);
				this->lvMyStack->AddColumn(CSTR("Eip"), 70);
				this->lvMyStack->AddColumn(CSTR("Code"), 500);
				this->lvMyStack->AddColumn(CSTR("Eax"), 70);
				this->lvMyStack->AddColumn(CSTR("Edx"), 70);
				this->lvMyStack->AddColumn(CSTR("Ecx"), 70);
				this->lvMyStack->AddColumn(CSTR("Ebx"), 70);
				this->lvMyStack->AddColumn(CSTR("Esi"), 70);
				this->lvMyStack->AddColumn(CSTR("Edi"), 70);
				this->contextType = Manage::ThreadContext::ContextType::X86_32;

				Data::ArrayListUInt32 callAddrs;
				Data::ArrayListUInt32 jmpAddrs;
				eip = (UInt32)context->GetInstAddr();
				esp = (UInt32)context->GetStackAddr();
				ebp = (UInt32)context->GetFrameAddr();
				Manage::DasmX86_32 dasm;

				Text::StringBuilderWriter sbWriter(sb);
				Manage::DasmX86_32::DasmX86_32_Regs regs;
				context->GetRegs(&regs);
				callLev = 0;
				while (true)
				{
					if (eip == 0)
						break;
					sb.ClearStr();
					sb.AppendHex32(eip);
					sb.AppendC(UTF8STRC(" "));
					sptr = symbol->ResolveName(sbuff, eip);
					i  = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
					sb.AppendP(&sbuff[i + 1], sptr);
					i = this->lbMyStack->AddItem(sb.ToCString(), 0);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("EIP = 0x"));
					sb.AppendHex32(eip);
					sb.AppendC(UTF8STRC("\r\n"));
					buffSize = proc->ReadMemory(eip, buff, 256);
					if (buffSize > 0)
					{
						sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
						sb.AppendC(UTF8STRC("\r\n"));
					}

					sb.AppendC(UTF8STRC("\r\n"));
					sb.AppendC(UTF8STRC("ESP = 0x"));
					sb.AppendHex32(esp);
					sb.AppendC(UTF8STRC("\r\n"));
					buffSize = proc->ReadMemory(esp, buff, 256);
					if (buffSize > 0)
					{
						sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
						sb.AppendC(UTF8STRC("\r\n"));
					}
					this->stacksMem.Add(Text::StrCopyNew(sb.ToString()));

					sb.ClearStr();
					ret = dasm.Disasm32(sbWriter, symbol, &eip, &esp, &ebp, &callAddrs, &jmpAddrs, &blockStart, &blockEnd, &regs, proc, true);
					this->stacks.Add(Text::StrCopyNew(sb.ToString()));
					if (!ret)
						break;
					if (++callLev > 50)
						break;
				}
			}
#endif
#if defined(CPU_X86_64) || (defined(CPU_ARM64) && defined(_WIN64))
			if (context->GetType() == Manage::ThreadContext::ContextType::X86_64)
			{
				UInt64 rip;
				UInt64 rsp;
				UInt64 rbp;
				UInt8 buff[256];
				UOSInt buffSize;
				Bool ret;
				UInt64 blockStart;
				UInt64 blockEnd;
				Text::StringBuilderUTF8 sb;

				this->lvMyStack->ChangeColumnCnt(18);
				this->lvMyStack->AddColumn(CSTR("Rsp"), 140);
				this->lvMyStack->AddColumn(CSTR("Rbp"), 140);
				this->lvMyStack->AddColumn(CSTR("Rip"), 140);
				this->lvMyStack->AddColumn(CSTR("Code"), 500);
				this->lvMyStack->AddColumn(CSTR("Rax"), 140);
				this->lvMyStack->AddColumn(CSTR("Rdx"), 140);
				this->lvMyStack->AddColumn(CSTR("Rcx"), 140);
				this->lvMyStack->AddColumn(CSTR("Rbx"), 140);
				this->lvMyStack->AddColumn(CSTR("Rsi"), 140);
				this->lvMyStack->AddColumn(CSTR("Rdi"), 140);
				this->lvMyStack->AddColumn(CSTR("R8"), 140);
				this->lvMyStack->AddColumn(CSTR("R9"), 140);
				this->lvMyStack->AddColumn(CSTR("R10"), 140);
				this->lvMyStack->AddColumn(CSTR("R11"), 140);
				this->lvMyStack->AddColumn(CSTR("R12"), 140);
				this->lvMyStack->AddColumn(CSTR("R13"), 140);
				this->lvMyStack->AddColumn(CSTR("R14"), 140);
				this->lvMyStack->AddColumn(CSTR("R15"), 140);
				this->contextType = Manage::ThreadContext::ContextType::X86_64;

				Data::ArrayListUInt64 callAddrs;
				Data::ArrayListUInt64 jmpAddrs;
				rip = context->GetInstAddr();
				rsp = context->GetStackAddr();
				rbp = context->GetFrameAddr();
				Manage::DasmX86_64 dasm;

				Text::StringBuilderWriter sbWriter(sb);
				Manage::DasmX86_64::DasmX86_64_Regs regs;
				context->GetRegs(&regs);
				callLev = 0;
				while (true)
				{
					if (rip == 0)
						break;
					sb.ClearStr();
					sb.AppendHex64(rip);
					sb.AppendC(UTF8STRC(" "));
					sptr = symbol->ResolveName(sbuff, rip);
					i  = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
					sb.AppendP(&sbuff[i + 1], sptr);
					i = this->lbMyStack->AddItem(sb.ToCString(), 0);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("RIP = 0x"));
					sb.AppendHex64(rip);
					sb.AppendC(UTF8STRC("\r\n"));
					buffSize = proc->ReadMemory(rip, buff, 256);
					if (buffSize > 0)
					{
						sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
						sb.AppendC(UTF8STRC("\r\n"));
					}

					sb.AppendC(UTF8STRC("\r\n"));
					sb.AppendC(UTF8STRC("RSP = 0x"));
					sb.AppendHex64(rsp);
					sb.AppendC(UTF8STRC("\r\n"));
					buffSize = proc->ReadMemory(rsp, buff, 256);
					if (buffSize > 0)
					{
						sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
						sb.AppendC(UTF8STRC("\r\n"));
					}
					this->stacksMem.Add(Text::StrCopyNew(sb.ToString()));

					sb.ClearStr();
					ret = dasm.Disasm64(sbWriter, symbol, &rip, &rsp, &rbp, &callAddrs, &jmpAddrs, &blockStart, &blockEnd, &regs, proc, true);
					this->stacks.Add(Text::StrCopyNew(sb.ToString()));
					if (!ret)
						break;
					if (++callLev > 50)
						break;
				}
			}
#endif
#if (defined(CPU_ARM64) && defined(_WIN64))
			if (context->GetType() == Manage::ThreadContext::ContextType::ARM64)
			{
				UInt64 pc;
				UInt64 sp;
				UInt64 lr;
				UInt8 buff[256];
				UOSInt buffSize;
				Bool ret;
				UInt64 blockStart;
				UInt64 blockEnd;
				Text::StringBuilderUTF8 sb;

				this->lvMyStack->ChangeColumnCnt(20);
				this->lvMyStack->AddColumn(CSTR("Sp"), 140);
				this->lvMyStack->AddColumn(CSTR("Lr"), 140);
				this->lvMyStack->AddColumn(CSTR("Pc"), 140);
				this->lvMyStack->AddColumn(CSTR("Code"), 500);
				this->lvMyStack->AddColumn(CSTR("X0"), 140);
				this->lvMyStack->AddColumn(CSTR("X1"), 140);
				this->lvMyStack->AddColumn(CSTR("X2"), 140);
				this->lvMyStack->AddColumn(CSTR("X3"), 140);
				this->lvMyStack->AddColumn(CSTR("X4"), 140);
				this->lvMyStack->AddColumn(CSTR("X5"), 140);
				this->lvMyStack->AddColumn(CSTR("X6"), 140);
				this->lvMyStack->AddColumn(CSTR("X7"), 140);
				this->lvMyStack->AddColumn(CSTR("X8"), 140);
				this->lvMyStack->AddColumn(CSTR("X9"), 140);
				this->lvMyStack->AddColumn(CSTR("X10"), 140);
				this->lvMyStack->AddColumn(CSTR("X11"), 140);
				this->lvMyStack->AddColumn(CSTR("X12"), 140);
				this->lvMyStack->AddColumn(CSTR("X13"), 140);
				this->lvMyStack->AddColumn(CSTR("X14"), 140);
				this->lvMyStack->AddColumn(CSTR("X15"), 140);
				this->lvMyStack->AddColumn(CSTR("X16"), 140);
				this->lvMyStack->AddColumn(CSTR("X17"), 140);
				this->lvMyStack->AddColumn(CSTR("X18"), 140);
				this->lvMyStack->AddColumn(CSTR("X19"), 140);
				this->lvMyStack->AddColumn(CSTR("X20"), 140);
				this->lvMyStack->AddColumn(CSTR("X21"), 140);
				this->lvMyStack->AddColumn(CSTR("X22"), 140);
				this->lvMyStack->AddColumn(CSTR("X23"), 140);
				this->lvMyStack->AddColumn(CSTR("X24"), 140);
				this->lvMyStack->AddColumn(CSTR("X25"), 140);
				this->lvMyStack->AddColumn(CSTR("X26"), 140);
				this->lvMyStack->AddColumn(CSTR("X27"), 140);
				this->lvMyStack->AddColumn(CSTR("X28"), 140);
				this->lvMyStack->AddColumn(CSTR("X29"), 140);
				this->contextType = Manage::ThreadContext::ContextType::ARM64;

				Data::ArrayListUInt64 callAddrs;
				Data::ArrayListUInt64 jmpAddrs;
				pc = context->GetInstAddr();
				sp = context->GetStackAddr();
				lr = context->GetFrameAddr();
				Manage::DasmARM64 dasm;

				Text::StringBuilderWriter sbWriter(sb);
				Manage::DasmARM64::DasmARM64_Regs regs;
				context->GetRegs(&regs);
				callLev = 0;
				while (true)
				{
					if (pc == 0)
						break;
					sb.ClearStr();
					sb.AppendHex64(pc);
					sb.AppendC(UTF8STRC(" "));
					sptr = symbol->ResolveName(sbuff, pc);
					i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
					sb.AppendP(&sbuff[i + 1], sptr);
					i = this->lbMyStack->AddItem(sb.ToCString(), 0);
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Pc = 0x"));
					sb.AppendHex64(pc);
					sb.AppendC(UTF8STRC("\r\n"));
					buffSize = proc->ReadMemory(pc, buff, 256);
					if (buffSize > 0)
					{
						sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
						sb.AppendC(UTF8STRC("\r\n"));
					}

					sb.AppendC(UTF8STRC("\r\n"));
					sb.AppendC(UTF8STRC("Sp = 0x"));
					sb.AppendHex64(sp);
					sb.AppendC(UTF8STRC("\r\n"));
					buffSize = proc->ReadMemory(sp, buff, 256);
					if (buffSize > 0)
					{
						sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
						sb.AppendC(UTF8STRC("\r\n"));
					}
					this->stacksMem.Add(Text::StrCopyNew(sb.ToString()));

					sb.ClearStr();
					ret = dasm.Disasm64(sbWriter, symbol, &pc, &sp, &lr, &callAddrs, &jmpAddrs, &blockStart, &blockEnd, &regs, proc, true);
					this->stacks.Add(Text::StrCopyNew(sb.ToString()));
					if (!ret)
						break;
					if (++callLev > 50)
						break;
				}
			}
#endif
			thread.Resume();

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
				sptr = context->GetRegister(i, sbuff, buff, &bitCnt);
				k = this->lvContext->AddItem(CSTRP(sbuff, sptr), 0);
				if (bitCnt == 8)
				{
					sptr = Text::StrHexByte(sbuff, buff[0]);
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 16)
				{
					sptr = Text::StrHexVal16(sbuff, ReadUInt16(buff));
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 32)
				{
					sptr = Text::StrHexVal32(sbuff, ReadUInt32(buff));
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 64)
				{
					sptr = Text::StrHexVal64(sbuff, ReadUInt64(buff));
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 128)
				{
					sptr = Text::StrHexVal64(Text::StrHexVal64(sbuff, ReadUInt64(buff)), ReadUInt64(&buff[8]));
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 80)
				{
					sptr = Text::StrDouble(sbuff, (Double)*(LDouble*)&buff[0]);
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else
				{
					bitCnt = 0;
				}
				i++;
			}

			context.Delete();
		}
		else
		{
			thread.Resume();
		}
	}
}

SSWR::AVIRead::AVIRThreadInfoForm::~AVIRThreadInfoForm()
{
	UOSInt i;
	i = this->stacks.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->stacks.GetItem(i).OrNull());
		Text::StrDelNew(this->stacksMem.GetItem(i).OrNull());
	}
}

void SSWR::AVIRead::AVIRThreadInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
