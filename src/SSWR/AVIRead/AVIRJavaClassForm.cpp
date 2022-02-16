#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRJavaClassForm.h"

void __stdcall SSWR::AVIRead::AVIRJavaClassForm::OnMethodsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRJavaClassForm *me = (SSWR::AVIRead::AVIRJavaClassForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->clsFile->MethodsGetDetail((UOSInt)me->lbMethods->GetSelectedIndex(), 0, true, &sb);
	me->txtMethods->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRJavaClassForm::AVIRJavaClassForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::JavaClass *clsFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	this->SetFont(0, 0, 8.25, false);
	this->clsFile = clsFile;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	sb.AppendC(UTF8STRC("Java Class - "));
	sb.Append(clsFile->GetSourceNameObj());
	this->SetText(sb.ToCString());

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFileStruct = this->tcMain->AddTabPage(CSTR("File Structure"));
	NEW_CLASS(this->txtFileStruct, UI::GUITextBox(ui, this->tpFileStruct, CSTR(""), true));
	this->txtFileStruct->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtFileStruct->SetReadOnly(true);
	sb.ClearStr();
	this->clsFile->FileStructDetail(&sb);
	this->txtFileStruct->SetText(sb.ToCString());

	this->tpFields = this->tcMain->AddTabPage(CSTR("Fields"));
	NEW_CLASS(this->lbFields, UI::GUIListBox(ui, this->tpFields, false));
	this->lbFields->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpMethods = this->tcMain->AddTabPage(CSTR("Methods"));
	NEW_CLASS(this->lbMethods, UI::GUIListBox(ui, this->tpMethods, false));
	this->lbMethods->SetRect(0, 0, 100, 300, false);
	this->lbMethods->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lbMethods->HandleSelectionChange(OnMethodsSelChg, this);
	NEW_CLASS(this->vspMethods, UI::GUIVSplitter(ui, this->tpMethods, 3, false));
	NEW_CLASS(this->txtMethods, UI::GUITextBox(ui, this->tpMethods, CSTR(""), true));
	this->txtMethods->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtMethods->SetReadOnly(true);

	this->tpDecompile = this->tcMain->AddTabPage(CSTR("Decompile"));
	NEW_CLASS(this->txtDecompile, UI::GUITextBox(ui, this->tpDecompile, CSTR(""), true));
	this->txtDecompile->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDecompile->SetReadOnly(true);

	UOSInt i = 0;
	UOSInt j = this->clsFile->FieldsGetCount();
	while (i < j)
	{
		sb.ClearStr();
		this->clsFile->FieldsGetDecl(i, &sb);
		this->lbFields->AddItem(sb.ToCString(), (void*)i);
		i++;
	}

	i = 0;
	j = this->clsFile->MethodsGetCount();
	while (i < j)
	{
		sb.ClearStr();
		this->clsFile->MethodsGetDecl(i, &sb);
		this->lbMethods->AddItem(sb.ToCString(), (void*)i);
		i++;
	}

	sb.ClearStr();
	this->clsFile->DecompileFile(&sb);
	this->txtDecompile->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRJavaClassForm::~AVIRJavaClassForm()
{
	DEL_CLASS(this->clsFile);
}

void SSWR::AVIRead::AVIRJavaClassForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
