#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRValgrindLogForm.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRValgrindLogForm::OnFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRValgrindLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRValgrindLogForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"ValgrindLog", false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRValgrindLogForm::FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRValgrindLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRValgrindLogForm>();
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	while (i < j)
	{
		if (me->LoadFile(files[i]))
		{
			break;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRValgrindLogForm::OnLeakSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRValgrindLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRValgrindLogForm>();
	NN<IO::ValgrindLog::LeakInfo> leak;
	me->lvLeakDetail->ClearItems();
	if (me->lvLeak->GetSelectedItem().GetOpt<IO::ValgrindLog::LeakInfo>().SetTo(leak))
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		NN<IO::ValgrindLog::StackEntry> stack;
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j = leak->stacks.GetCount();
		UOSInt k;
		while (i < j)
		{
			stack = leak->stacks.GetItemNoCheck(i);
			sptr = Text::StrHexVal64V(Text::StrConcatC(sbuff, UTF8STRC("0x")), stack->address);
			k = me->lvLeakDetail->AddItem(CSTRP(sbuff, sptr), stack);
			me->lvLeakDetail->SetSubItem(k, 1, stack->funcName);
			if (stack->source.SetTo(s))
			{
				me->lvLeakDetail->SetSubItem(k, 2, s);
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRValgrindLogForm::OnErrorSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRValgrindLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRValgrindLogForm>();
	NN<IO::ValgrindLog::ExceptionInfo> err;
	me->lvErrorDetail->ClearItems();
	if (me->lvError->GetSelectedItem().GetOpt<IO::ValgrindLog::ExceptionInfo>().SetTo(err))
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		NN<IO::ValgrindLog::StackEntry> stack;
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j = err->stacks.GetCount();
		UOSInt k;
		while (i < j)
		{
			stack = err->stacks.GetItemNoCheck(i);
			sptr = Text::StrHexVal64V(Text::StrConcatC(sbuff, UTF8STRC("0x")), stack->address);
			k = me->lvErrorDetail->AddItem(CSTRP(sbuff, sptr), stack);
			me->lvErrorDetail->SetSubItem(k, 1, stack->funcName);
			if (stack->source.SetTo(s))
			{
				me->lvErrorDetail->SetSubItem(k, 2, s);
			}
			i++;
		}

		Text::StringBuilderUTF8 sb;
		me->lvErrorDetail->AddItem(CSTR(""), 0);
		sptr = Text::StrHexVal64V(Text::StrConcatC(sbuff, UTF8STRC("0x")), err->accessAddress);
		k = me->lvErrorDetail->AddItem(CSTRP(sbuff, sptr), 0);
		switch (err->addrType)
		{
		case IO::ValgrindLog::AddressType::NoAddress:
			break;
		case IO::ValgrindLog::AddressType::Unknown:
			me->lvErrorDetail->SetSubItem(k, 1, CSTR("is not stack'd, malloc'd or (recently) free'd"));
			break;
		case IO::ValgrindLog::AddressType::Stack:
			sb.ClearStr();
			sb.Append(CSTR("is on thread "));
			sb.AppendUOSInt(err->threadId);
			sb.Append(CSTR("'s stack"));
			me->lvErrorDetail->SetSubItem(k, 1, sb.ToCString());
			break;
		case IO::ValgrindLog::AddressType::Alloc:
			sb.ClearStr();
			sb.Append(CSTR("is "));
			sb.AppendU32(err->blockOfst);
			sb.Append(CSTR(" bytes inside a block of size "));
			sb.AppendU32(err->stackSize);
			sb.Append(CSTR(" alloc'd"));
			me->lvErrorDetail->SetSubItem(k, 1, sb.ToCString());
			i = 0;
			j = err->allocStacks.GetCount();
			while (i < j)
			{
				stack = err->allocStacks.GetItemNoCheck(i);
				sptr = Text::StrHexVal64V(Text::StrConcatC(sbuff, UTF8STRC("0x")), stack->address);
				k = me->lvErrorDetail->AddItem(CSTRP(sbuff, sptr), stack);
				me->lvErrorDetail->SetSubItem(k, 1, stack->funcName);
				if (stack->source.SetTo(s))
				{
					me->lvErrorDetail->SetSubItem(k, 2, s);
				}
				i++;
			}
			break;
		}
	}
}

Bool SSWR::AVIRead::AVIRValgrindLogForm::LoadFile(NN<Text::String> file)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::ValgrindLog> log;
	UOSInt i;
	UOSInt j;
	if (IO::ValgrindLog::LoadFile(file->ToCString()).SetTo(log))
	{
		this->log.Delete();
		this->log = log;
		sptr = Text::StrUOSInt(sbuff, log->GetPPID());
		this->txtPPID->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, log->GetMainPID());
		this->txtPID->SetText(CSTRP(sbuff, sptr));
		this->txtVersion->SetText(Text::String::OrEmpty(log->GetVersion())->ToCString());
		this->txtCommandLine->SetText(Text::String::OrEmpty(log->GetCommandLine())->ToCString());
		sptr = Text::StrUInt32(sbuff, log->GetBlocksInUse());
		this->txtBlocksInUse->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt64(sbuff, log->GetBytesInUse());
		this->txtBytesInUse->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, log->GetBlocksAllocs());
		this->txtBlocksAllocs->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, log->GetBlocksFrees());
		this->txtBlocksFrees->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt64(sbuff, log->GetBytesAllocs());
		this->txtBytesAllocs->SetText(CSTRP(sbuff, sptr));

		this->lvError->ClearItems();
		this->lvErrorDetail->ClearItems();
		this->lvLeak->ClearItems();
		this->lvLeakDetail->ClearItems();
		NN<const Data::ArrayListNN<IO::ValgrindLog::ExceptionInfo>> errList = log->GetErrorList();
		NN<IO::ValgrindLog::ExceptionInfo> err;
		NN<IO::ValgrindLog::StackEntry> stack;
		NN<Text::String> s;
		i = 0;
		j = errList->GetCount();
		while (i < j)
		{
			err = errList->GetItemNoCheck(i);
			this->lvError->AddItem(err->message, err);
			if (GetOptimalStack(err->stacks).SetTo(stack))
			{
				this->lvError->SetSubItem(i, 1, stack->funcName);
				if (stack->source.SetTo(s))
				{
					this->lvError->SetSubItem(i, 2, s);
				}
			}
			i++;
		}

		NN<const Data::ArrayListNN<IO::ValgrindLog::LeakInfo>> leakList = log->GetLeakList();
		NN<IO::ValgrindLog::LeakInfo> leak;
		i = 0;
		j = leakList->GetCount();
		while (i < j)
		{
			leak = leakList->GetItemNoCheck(i);
			this->lvLeak->AddItem(leak->message, leak);
			if (GetOptimalStack(leak->stacks).SetTo(stack))
			{
				this->lvLeak->SetSubItem(i, 1, stack->funcName);
				if (stack->source.SetTo(s))
				{
					this->lvLeak->SetSubItem(i, 2, s);
				}
			}
			i++;
		}
		return true;
	}
	return false;
}

Optional<IO::ValgrindLog::StackEntry> SSWR::AVIRead::AVIRValgrindLogForm::GetOptimalStack(NN<Data::ArrayListNN<IO::ValgrindLog::StackEntry>> stacks)
{
	NN<IO::ValgrindLog::StackEntry> stack;
	UOSInt i = 0;
	UOSInt j = stacks->GetCount();
	while (i < j)
	{
		stack = stacks->GetItemNoCheck(i);
		if (stack->source.IsNull())
		{
			return stack;
		}
		if (stack->funcName->Equals(CSTR("malloc")))
		{

		}
		else if (stack->funcName->StartsWith(CSTR("MAlloc")))
		{

		}
		else
		{
			return stack;
		}
		i++;
	}
	return nullptr;
}

SSWR::AVIRead::AVIRValgrindLogForm::AVIRValgrindLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Valgrind Log"));
	
	this->core = core;
	this->log = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("Version"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtFile->SetReadOnly(true);
	this->txtFile->SetRect(104, 4, 400, 23, false);
	this->btnFile = ui->NewButton(this->pnlFile, CSTR("Browse"));
	this->btnFile->SetRect(504, 0, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lblPPID = ui->NewLabel(this->tpInfo, CSTR("PPID"));
	this->lblPPID->SetRect(0, 0, 100, 23, false);
	this->txtPPID = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtPPID->SetRect(100, 0, 100, 23, false);
	this->txtPPID->SetReadOnly(true);
	this->lblPID = ui->NewLabel(this->tpInfo, CSTR("PID"));
	this->lblPID->SetRect(0, 24, 100, 23, false);
	this->txtPID = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtPID->SetRect(100, 24, 100, 23, false);
	this->txtPID->SetReadOnly(true);
	this->lblVersion = ui->NewLabel(this->tpInfo, CSTR("Version"));
	this->lblVersion->SetRect(0, 48, 100, 23, false);
	this->txtVersion = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtVersion->SetRect(100, 48, 100, 23, false);
	this->txtVersion->SetReadOnly(true);
	this->lblCommandLine = ui->NewLabel(this->tpInfo, CSTR("Command Line"));
	this->lblCommandLine->SetRect(0, 72, 100, 23, false);
	this->txtCommandLine = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtCommandLine->SetRect(100, 72, 400, 23, false);
	this->txtCommandLine->SetReadOnly(true);
	this->lblBlocksInUse = ui->NewLabel(this->tpInfo, CSTR("Blocks In Use"));
	this->lblBlocksInUse->SetRect(0, 96, 100, 23, false);
	this->txtBlocksInUse = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBlocksInUse->SetRect(100, 96, 100, 23, false);
	this->txtBlocksInUse->SetReadOnly(true);
	this->lblBytesInUse = ui->NewLabel(this->tpInfo, CSTR("Bytes In Use"));
	this->lblBytesInUse->SetRect(0, 120, 100, 23, false);
	this->txtBytesInUse = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBytesInUse->SetRect(100, 120, 100, 23, false);
	this->txtBytesInUse->SetReadOnly(true);
	this->lblBlocksAllocs = ui->NewLabel(this->tpInfo, CSTR("Blocks Allocs"));
	this->lblBlocksAllocs->SetRect(0, 144, 100, 23, false);
	this->txtBlocksAllocs = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBlocksAllocs->SetRect(100, 144, 100, 23, false);
	this->txtBlocksAllocs->SetReadOnly(true);
	this->lblBlocksFrees = ui->NewLabel(this->tpInfo, CSTR("Blocks Frees"));
	this->lblBlocksFrees->SetRect(0, 168, 100, 23, false);
	this->txtBlocksFrees = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBlocksFrees->SetRect(100, 168, 100, 23, false);
	this->txtBlocksFrees->SetReadOnly(true);
	this->lblBytesAllocs = ui->NewLabel(this->tpInfo, CSTR("Bytes Allocs"));
	this->lblBytesAllocs->SetRect(0, 192, 100, 23, false);
	this->txtBytesAllocs = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBytesAllocs->SetRect(100, 192, 100, 23, false);
	this->txtBytesAllocs->SetReadOnly(true);

	this->tpError = this->tcMain->AddTabPage(CSTR("Error"));
	this->lvErrorDetail = ui->NewListView(this->tpError, UI::ListViewStyle::Table, 3);
	this->lvErrorDetail->SetRect(0, 0, 120, 200, false);
	this->lvErrorDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvErrorDetail->AddColumn(CSTR("Address"), 80);
	this->lvErrorDetail->AddColumn(CSTR("Function"), 400);
	this->lvErrorDetail->AddColumn(CSTR("Source"), 200);
	this->vspError = ui->NewVSplitter(this->tpError, 3, true);
	this->lvError = ui->NewListView(this->tpError, UI::ListViewStyle::Table, 3);
	this->lvError->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvError->AddColumn(CSTR("Description"), 200);
	this->lvError->AddColumn(CSTR("Function"), 400);
	this->lvError->AddColumn(CSTR("Source"), 200);
	this->lvError->HandleSelChg(OnErrorSelChg, this);

	this->tpLeak = this->tcMain->AddTabPage(CSTR("Leak"));
	this->lvLeakDetail = ui->NewListView(this->tpLeak, UI::ListViewStyle::Table, 3);
	this->lvLeakDetail->SetRect(0, 0, 120, 200, false);
	this->lvLeakDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvLeakDetail->AddColumn(CSTR("Address"), 80);
	this->lvLeakDetail->AddColumn(CSTR("Function"), 400);
	this->lvLeakDetail->AddColumn(CSTR("Source"), 200);
	this->vspLeak = ui->NewVSplitter(this->tpLeak, 3, true);
	this->lvLeak = ui->NewListView(this->tpLeak, UI::ListViewStyle::Table, 3);
	this->lvLeak->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLeak->AddColumn(CSTR("Description"), 200);
	this->lvLeak->AddColumn(CSTR("Function"), 400);
	this->lvLeak->AddColumn(CSTR("Source"), 200);
	this->lvLeak->HandleSelChg(OnLeakSelChg, this);

	this->HandleDropFiles(FileHandler, this);
}

SSWR::AVIRead::AVIRValgrindLogForm::~AVIRValgrindLogForm()
{
	this->log.Delete();
}

void SSWR::AVIRead::AVIRValgrindLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
