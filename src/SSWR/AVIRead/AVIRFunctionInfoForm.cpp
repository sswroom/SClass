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

void __stdcall SSWR::AVIRead::AVIRFunctionInfoForm::OnMyStackDblClk(AnyType userObj, UOSInt index)
{
//	SSWR::AVIRead::AVIRFunctionInfoForm *me = (SSWR::AVIRead::AVIRFunctionInfoForm*)userObj;
}

SSWR::AVIRead::AVIRFunctionInfoForm::AVIRFunctionInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Manage::Process> proc, Optional<Manage::SymbolResolver> symbol, Int64 funcAddr) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Function Info"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lvMyStack = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvMyStack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMyStack->HandleDblClk(OnMyStackDblClk, this);
	this->lvMyStack->SetFullRowSelect(true);
	this->lvMyStack->SetShowGrid(true);
	this->lvMyStack->AddColumn(CSTR("Address"), 100);
	this->lvMyStack->AddColumn(CSTR("Code"), 900);

#if defined(CPU_X86_32)
	if (proc->GetContextType() == Manage::ThreadContext::ContextType::X86_32)
	{
		Manage::DasmX86_32 *dasm;
		Text::StringBuilderUTF8 sb;
		Text::PString sline[2];
		Text::PString  sarr[2];
		Bool hasNext;
		UInt32 addr = (UInt32)funcAddr;
		UInt32 blockStart;
		UInt32 blockEnd;
		UOSInt i;
		Data::ArrayListUInt32 callAddrs;
		Data::ArrayListUInt32 jmpAddrs;
		NEW_CLASS(dasm, Manage::DasmX86_32());
		dasm->Disasm32In(sb, symbol, &addr, &callAddrs, &jmpAddrs, &blockStart, &blockEnd, proc);
		DEL_CLASS(dasm);

		sline[1] = sb;
		while (true)
		{
			hasNext = Text::StrSplitP(sline, 2, sline[1], '\r') == 2;
			if (Text::StrSplitP(sarr, 2, sline[0], ' ') == 2)
			{
				i = this->lvMyStack->AddItem(sarr[0].ToCString(), 0);
				this->lvMyStack->SetSubItem(i, 1, sarr[1].ToCString());
			}
			if (!hasNext)
				break;
		}
	}
#elif defined(CPU_X86_64)
	if (proc->GetContextType() == Manage::ThreadContext::ContextType::X86_64)
	{
		Manage::DasmX86_64 *dasm;
		Text::StringBuilderUTF8 sb;
		Text::PString sline[2];
		Text::PString sarr[2];
		Bool hasNext;
		UInt64 addr = (UInt64)funcAddr;
		UInt64 blockStart;
		UInt64 blockEnd;
		UOSInt i;
		Data::ArrayListUInt64 callAddrs;
		Data::ArrayListUInt64 jmpAddrs;
		NEW_CLASS(dasm, Manage::DasmX86_64());
		dasm->Disasm64In(sb, symbol, &addr, &callAddrs, &jmpAddrs, &blockStart, &blockEnd, proc);
		DEL_CLASS(dasm);

		sline[1] = sb;
		while (true)
		{
			hasNext = Text::StrSplitP(sline, 2, sline[1], '\r') == 2;
			if (Text::StrSplitP(sarr, 2, sline[0], ' ') == 2)
			{
				i = this->lvMyStack->AddItem(sarr[0].ToCString(), 0);
				this->lvMyStack->SetSubItem(i, 1, sarr[1].ToCString());
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
