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
	const UTF8Char *s = me->stacks.GetItem(i);
	const UTF8Char *sMem = me->stacksMem.GetItem(i);
	UOSInt slen;
	UTF8Char *sbuff;
	Text::PString sline[2];
	Text::PString sarr[18];
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

		if (me->contextType == Manage::ThreadContext::CT_X86_32)
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
		else if (me->contextType == Manage::ThreadContext::CT_X86_64)
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

SSWR::AVIRead::AVIRThreadInfoForm::AVIRThreadInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Manage::Process *proc, Manage::SymbolResolver *symbol, UInt32 threadId) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Thread Info"));

	this->core = core;
	this->proc = proc;
	this->symbol = symbol;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->tpContext = this->tcMain->AddTabPage(CSTR("Context"));
	this->tpStack = this->tcMain->AddTabPage(CSTR("Stack"));
	this->tpMyStack = this->tcMain->AddTabPage(CSTR("My Stack"));

	NEW_CLASS(this->lblThreadId, UI::GUILabel(ui, this->tpInfo, CSTR("Thread Id")));
	this->lblThreadId->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtThreadId, UI::GUITextBox(ui, this->tpInfo, CSTR(""), false));
	this->txtThreadId->SetRect(100, 0, 100, 23, false);
	this->txtThreadId->SetReadOnly(true);
	NEW_CLASS(this->lblStartAddr, UI::GUILabel(ui, this->tpInfo, CSTR("Start Address")));
	this->lblStartAddr->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtStartAddr, UI::GUITextBox(ui, this->tpInfo, CSTR(""), false));
	this->txtStartAddr->SetRect(100, 24, 120, 23, false);
	this->txtStartAddr->SetReadOnly(true);
	NEW_CLASS(this->lblStartName, UI::GUILabel(ui, this->tpInfo, CSTR("Start Name")));
	this->lblStartName->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtStartName, UI::GUITextBox(ui, this->tpInfo, CSTR(""), false));
	this->txtStartName->SetRect(100, 48, 500, 23, false);
	this->txtStartName->SetReadOnly(true);

	NEW_CLASS(this->lvContext, UI::GUIListView(ui, this->tpContext, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvContext->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContext->SetFullRowSelect(true);
	this->lvContext->SetShowGrid(true);
	this->lvContext->AddColumn(CSTR("Name"), 100);
	this->lvContext->AddColumn(CSTR("Value"), 300);

	NEW_CLASS(this->lvStack, UI::GUIListView(ui, this->tpStack, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStack->SetFullRowSelect(true);
	this->lvStack->SetShowGrid(true);
	this->lvStack->AddColumn(CSTR("Address"), 120);
	this->lvStack->AddColumn(CSTR("Name"), 500);

	NEW_CLASS(this->lbMyStack, UI::GUIListBox(ui, this->tpMyStack, false));
	this->lbMyStack->SetRect(0, 0, 300, 23, false);
	this->lbMyStack->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbMyStack->HandleSelectionChange(OnMyStackChg, this);
	NEW_CLASS(this->hspMyStack, UI::GUIHSplitter(ui, this->tpMyStack, 3, false));
	NEW_CLASS(this->pnlMyStack, UI::GUIPanel(ui, this->tpMyStack));
	this->pnlMyStack->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->txtMyStackMem, UI::GUITextBox(ui, this->pnlMyStack, CSTR(""), true));
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
	UTF8Char *sptr;
	UInt64 startAddr;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(thread, Manage::ThreadInfo(proc->GetProcId(), threadId));

	startAddr = thread->GetStartAddress();
	sptr = Text::StrUInt32(sbuff, threadId);
	this->txtThreadId->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal64(sbuff, startAddr);
	this->txtStartAddr->SetText(CSTRP(sbuff, sptr));
	sptr = symbol->ResolveName(sbuff, startAddr);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
	this->txtStartName->SetText(CSTRP(&sbuff[i + 1], sptr));

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
			sptr = Text::StrHexVal64(sbuff, currAddr);
			i = this->lvStack->AddItem(CSTRP(sbuff, sptr), 0, 0);
			sptr = symbol->ResolveName(sbuff, currAddr);
			j = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
			this->lvStack->SetSubItem(i, 1, CSTRP(&sbuff[j + 1], sptr));
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
				ret = dasm->Disasm64(&sbWriter, symbol, &rip, &rsp, &rbp, callAddrs, jmpAddrs, &blockStart, &blockEnd, &regs, proc, true);
				this->stacks.Add(Text::StrCopyNew(sb.ToString()));
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

			DEL_CLASS(context);
		}
	}
	DEL_CLASS(thread);
}

SSWR::AVIRead::AVIRThreadInfoForm::~AVIRThreadInfoForm()
{
	UOSInt i;
	i = this->stacks.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->stacks.GetItem(i));
		Text::StrDelNew(this->stacksMem.GetItem(i));
	}
}

void SSWR::AVIRead::AVIRThreadInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
