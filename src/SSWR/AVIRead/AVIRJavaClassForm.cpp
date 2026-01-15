#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRJavaClassForm.h"

void __stdcall SSWR::AVIRead::AVIRJavaClassForm::OnMethodsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJavaClassForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJavaClassForm>();
	Text::StringBuilderUTF8 sb;
	me->clsFile->MethodsGetDetail((UIntOS)me->lbMethods->GetSelectedIndex(), 0, true, sb);
	me->txtMethods->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRJavaClassForm::AVIRJavaClassForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::JavaClass> clsFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	this->SetFont(nullptr, 8.25, false);
	this->clsFile = clsFile;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	sb.AppendC(UTF8STRC("Java Class - "));
	sb.Append(clsFile->GetSourceNameObj());
	this->SetText(sb.ToCString());

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFileStruct = this->tcMain->AddTabPage(CSTR("File Structure"));
	this->txtFileStruct = ui->NewTextBox(this->tpFileStruct, CSTR(""), true);
	this->txtFileStruct->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtFileStruct->SetReadOnly(true);
	sb.ClearStr();
	this->clsFile->FileStructDetail(sb);
	this->txtFileStruct->SetText(sb.ToCString());

	this->tpFields = this->tcMain->AddTabPage(CSTR("Fields"));
	this->lbFields = ui->NewListBox(this->tpFields, false);
	this->lbFields->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpMethods = this->tcMain->AddTabPage(CSTR("Methods"));
	this->lbMethods = ui->NewListBox(this->tpMethods, false);
	this->lbMethods->SetRect(0, 0, 100, 300, false);
	this->lbMethods->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lbMethods->HandleSelectionChange(OnMethodsSelChg, this);
	this->vspMethods = ui->NewVSplitter(this->tpMethods, 3, false);
	this->txtMethods = ui->NewTextBox(this->tpMethods, CSTR(""), true);
	this->txtMethods->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtMethods->SetReadOnly(true);

	this->tpDecompile = this->tcMain->AddTabPage(CSTR("Decompile"));
	this->txtDecompile = ui->NewTextBox(this->tpDecompile, CSTR(""), true);
	this->txtDecompile->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDecompile->SetReadOnly(true);

	UIntOS i = 0;
	UIntOS j = this->clsFile->FieldsGetCount();
	while (i < j)
	{
		sb.ClearStr();
		this->clsFile->FieldsGetDecl(i, sb);
		this->lbFields->AddItem(sb.ToCString(), (void*)i);
		i++;
	}

	i = 0;
	j = this->clsFile->MethodsGetCount();
	while (i < j)
	{
		sb.ClearStr();
		this->clsFile->MethodsGetDecl(i, sb);
		this->lbMethods->AddItem(sb.ToCString(), (void*)i);
		i++;
	}

	sb.ClearStr();
	this->clsFile->DecompileFile(sb);
	this->txtDecompile->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRJavaClassForm::~AVIRJavaClassForm()
{
	this->clsFile.Delete();
}

void SSWR::AVIRead::AVIRJavaClassForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
