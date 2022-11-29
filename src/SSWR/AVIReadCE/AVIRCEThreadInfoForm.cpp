#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Manage/DasmX86_32.h"
#include "Manage/StackTracer.h"
//#include "SSWR/AVIReadCE/AVIRCEFunctionInfoForm.h"
#include "SSWR/AVIReadCE/AVIRCEThreadInfoForm.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringBuilderWriter.h"

void __stdcall SSWR::AVIReadCE::AVIRCEThreadInfoForm::OnMyStackChg(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEThreadInfoForm *me = (SSWR::AVIReadCE::AVIRCEThreadInfoForm*)userObj;
	OSInt i = me->lbMyStack->GetSelectedIndex();
	Text::String *s = me->stacks->GetItem(i);
	Text::String *sMem = me->stacksMem->GetItem(i);
	UOSInt slen;
	UTF8Char *sbuff;
	Text::PString sline[2];
	Text::PString sarr[10];
	Bool hasNext;

	if (sMem)
	{
		me->txtMyStackMem->SetText(sMem->ToCString());
	}
	else
	{
		me->txtMyStackMem->SetText(CSTR(""));
	}
	me->lvMyStack->ClearItems();
	if (s)
	{
		slen = s->leng;
		sbuff = MemAlloc(UTF8Char, slen + 1);
		s->ConcatTo(sbuff);
		sline[1].v = sbuff;
		sline[1].leng = slen;
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
		MemFree(sbuff);
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEThreadInfoForm::OnMyStackDblClk(void *userObj, UOSInt index)
{
	SSWR::AVIReadCE::AVIRCEThreadInfoForm *me = (SSWR::AVIReadCE::AVIRCEThreadInfoForm*)userObj;
	UTF8Char sbuff[18];
	UOSInt i;
	Int64 funcOfst;
	Text::StringBuilderUTF8 sb;
	me->lvMyStack->GetSubItem(index, 3, &sb);
	if (sb.StartsWith(UTF8STRC("call 0x")))
	{
		Text::StrConcatC(sbuff, sb.ToString() + 7, 17);
		i = Text::StrIndexOfChar(sbuff, ' ');
		if (i != INVALID_INDEX && i > 0)
		{
			sbuff[i] = 0;
		}
		funcOfst = Text::StrHex2Int32C(sbuff);

/*		SSWR::AVIRead::AVIRFunctionInfoForm frm(me->hInst, 0, me->ui, me->proc, me->symbol, funcOfst);
		frm.ShowDialog(me);*/
	}
}

SSWR::AVIReadCE::AVIRCEThreadInfoForm::AVIRCEThreadInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, Manage::Process *proc, Manage::SymbolResolver *symbol, Int32 threadId) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Thread Info"));
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	NEW_CLASS(this->stacks, Data::ArrayList<Text::String *>());
	NEW_CLASS(this->stacksMem, Data::ArrayList<Text::String *>());
	this->proc = proc;
	this->symbol = symbol;

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
	NEW_CLASS(this->lvMyStack, UI::GUIListView(ui, this->pnlMyStack, UI::GUIListView::LVSTYLE_TABLE, 10));
	this->lvMyStack->SetShowGrid(true);
	this->lvMyStack->SetFullRowSelect(true);
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
	sptr = Text::StrInt32(sbuff, threadId);
	this->txtThreadId->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal64(sbuff, startAddr);
	this->txtStartAddr->SetText(CSTRP(sbuff, sptr));
	sptr = symbol->ResolveName(sbuff, startAddr);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
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

#if defined(CPU_X86_32) || defined(CPU_X86_64)
		if (context->GetType() == Manage::ThreadContext::ContextType::X86_32)
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

			NEW_CLASS(callAddrs, Data::ArrayListUInt32());
			NEW_CLASS(jmpAddrs, Data::ArrayListUInt32());
			eip = (UInt32)context->GetInstAddr();
			esp = (UInt32)context->GetStackAddr();
			ebp = (UInt32)context->GetFrameAddr();
			NEW_CLASS(dasm, Manage::DasmX86_32());

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
				this->stacksMem->Add(Text::String::New(sb.ToCString()));

				sb.ClearStr();
				Text::StringBuilderWriter writer(&sb);
				Manage::DasmX86_32::Dasm_Regs regs;
				((Manage::ThreadContextX86_32*)context)->GetRegs(&regs);
				ret = dasm->Disasm32(&writer, symbol, &eip, &esp, &ebp, callAddrs, jmpAddrs, &blockStart, &blockEnd, &regs, proc, true);
				this->stacks->Add(Text::String::New(sb.ToCString()));
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
			UTF8Char *sptr;
			UInt32 bitCnt;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			i = 0;
			j = context->GetRegisterCnt();
			while (i < j)
			{
				sptr = context->GetRegister(i, sbuff, buff, &bitCnt);
				k = this->lvContext->AddItem(CSTRP(sbuff, sptr), 0, 0);
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
					sptr = Text::StrHexVal32(sbuff, ReadInt32(buff));
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 64)
				{
					sptr = Text::StrHexVal64(sbuff, ReadInt64(buff));
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 128)
				{
					sptr = Text::StrHexVal64(Text::StrHexVal64(sbuff, ReadInt64(buff)), ReadInt64(&buff[8]));
					this->lvContext->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				}
				else if (bitCnt == 80)
				{
					sptr = Text::StrDouble(sbuff, *(LDouble*)&buff[0]);
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

SSWR::AVIReadCE::AVIRCEThreadInfoForm::~AVIRCEThreadInfoForm()
{
	OSInt i;
	i = this->stacks->GetCount();
	while (i-- > 0)
	{
		this->stacks->GetItem(i)->Release();
		this->stacksMem->GetItem(i)->Release();
	}
	DEL_CLASS(this->stacks);
	DEL_CLASS(this->stacksMem);
}
