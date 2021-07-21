#include "Stdafx.h"
#if defined(CPU_X86_32)
#include "Manage/DasmX86_32.h"
#elif defined(CPU_X86_64)
#include "Manage/DasmX86_64.h"
#elif defined(CPU_X86_ARM)
#include "Manage/DasmARM.h"
#endif
#include "SSWR/AVIRead/AVIRFunctionInfoForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRFunctionInfoForm::OnMyStackDblClk(void *userObj, UOSInt index)
{
//	SSWR::AVIRead::AVIRFunctionInfoForm *me = (SSWR::AVIRead::AVIRFunctionInfoForm*)userObj;
}

SSWR::AVIRead::AVIRFunctionInfoForm::AVIRFunctionInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Manage::Process *proc, Manage::SymbolResolver *symbol, Int64 funcAddr) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Function Info");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lvMyStack, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvMyStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMyStack->HandleDblClk(OnMyStackDblClk, this);
	this->lvMyStack->SetFullRowSelect(true);
	this->lvMyStack->SetShowGrid(true);
	this->lvMyStack->AddColumn((const UTF8Char*)"Address", 100);
	this->lvMyStack->AddColumn((const UTF8Char*)"Code", 900);

#if defined(CPU_X86_32)
	if (proc->GetContextType() == Manage::ThreadContext::CT_X86_32)
	{
		Manage::DasmX86_32 *dasm;
		Text::StringBuilderUTF8 sb;
		UTF8Char *sbuff;
		UTF8Char *sline[2];
		UTF8Char *sarr[2];
		Bool hasNext;
		UInt32 addr = (UInt32)funcAddr;
		UInt32 blockStart;
		UInt32 blockEnd;
		UOSInt i;
		Data::ArrayListUInt32 callAddrs;
		Data::ArrayListUInt32 jmpAddrs;
		NEW_CLASS(dasm, Manage::DasmX86_32());
		dasm->Disasm32In(&sb, symbol, &addr, &callAddrs, &jmpAddrs, &blockStart, &blockEnd, proc);
		DEL_CLASS(dasm);

		sbuff = sb.ToString();
		sline[1] = sbuff;
		while (true)
		{
			hasNext = Text::StrSplit(sline, 2, sline[1], '\r') == 2;
			if (Text::StrSplit(sarr, 2, sline[0], ' ') == 2)
			{
				i = this->lvMyStack->AddItem(sarr[0], 0);
				this->lvMyStack->SetSubItem(i, 1, sarr[1]);
			}
			if (!hasNext)
				break;
		}
	}
#elif defined(CPU_X86_64)
	if (proc->GetContextType() == Manage::ThreadContext::CT_X86_64)
	{
		Manage::DasmX86_64 *dasm;
		Text::StringBuilderUTF8 sb;
		UTF8Char *sbuff;
		UTF8Char *sline[2];
		UTF8Char *sarr[2];
		Bool hasNext;
		UInt64 addr = (UInt64)funcAddr;
		UInt64 blockStart;
		UInt64 blockEnd;
		UOSInt i;
		Data::ArrayListUInt64 callAddrs;
		Data::ArrayListUInt64 jmpAddrs;
		NEW_CLASS(dasm, Manage::DasmX86_64());
		dasm->Disasm64In(&sb, symbol, &addr, &callAddrs, &jmpAddrs, &blockStart, &blockEnd, proc);
		DEL_CLASS(dasm);

		sbuff = sb.ToString();
		sline[1] = sbuff;
		while (true)
		{
			hasNext = Text::StrSplit(sline, 2, sline[1], '\r') == 2;
			if (Text::StrSplit(sarr, 2, sline[0], ' ') == 2)
			{
				i = this->lvMyStack->AddItem(sarr[0], 0);
				this->lvMyStack->SetSubItem(i, 1, sarr[1]);
			}
			if (!hasNext)
				break;
		}
	}
#endif
}

SSWR::AVIRead::AVIRFunctionInfoForm::~AVIRFunctionInfoForm()
{
}

void SSWR::AVIRead::AVIRFunctionInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
