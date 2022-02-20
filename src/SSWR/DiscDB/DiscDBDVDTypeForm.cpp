#include "Stdafx.h"
#include "SSWR/DiscDB/DiscDBDVDTypeForm.h"
#include "UI/MessageDialog.h"

void SSWR::DiscDB::DiscDBDVDTypeForm::ShowStatus()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("You are viewing "));
	sptr = Text::StrUOSInt(sptr, this->currIndex + 1);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" of "));
	sptr = Text::StrUOSInt(sptr, this->env->GetDVDTypeCount());
	this->lblDisplay->SetText(CSTRP(sbuff, sptr));
}

void SSWR::DiscDB::DiscDBDVDTypeForm::UpdateDisplay()
{
	this->txtID->SetText(this->currRec->discTypeID->ToCString());
	this->txtName->SetText(this->currRec->name->ToCString());
	this->txtDescription->SetText(this->currRec->description->ToCString());
}

Bool SSWR::DiscDB::DiscDBDVDTypeForm::UpdateRow()
{
	if (this->currRec == 0)
	{
		return true;
	}
	Text::StringBuilderUTF8 sbID;
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbDesc;
	this->txtID->GetText(&sbID);
	if (sbID.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter the id"), CSTR("DVDType"), this);
		this->txtID->Focus();
		return false;
	}
	this->txtName->GetText(&sbName);
	if (sbName.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter the name"), CSTR("DVDType"), this);
		this->txtName->Focus();
		return false;
	}
	this->txtDescription->GetText(&sbDesc);
	if (sbDesc.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter the description"), CSTR("DVDType"), this);
		this->txtDescription->Focus();
		return false;
	}

    if (this->newRec)
	{
		const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo *newRec = this->env->NewDVDType(sbID.ToCString(), sbName.ToCString(), sbDesc.ToCString());
		if (newRec)
		{
			this->newRec = false;
			this->txtID->SetReadOnly(true);
			this->currIndex = (UOSInt)this->env->GetDVDTypeIndex(sbID.ToCString());
			this->currRec = newRec;
			this->btnCancel->SetVisible(false);
			this->ShowStatus();
			return true;
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("ID Already exist"), CSTR("DVDType"), this);
			return false;
		}
	}
	else
	{
		this->env->ModifyDVDType(sbID.ToCString(), sbName.ToCString(), sbDesc.ToCString());
		return true;
	}
}

SSWR::DiscDB::DiscDBDVDTypeForm::DiscDBDVDTypeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 358, 223, ui)
{
	this->SetText(CSTR("Maintain DVD Types"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblID, UI::GUILabel(ui, this, CSTR("ID")));
	this->lblID->SetRect(8, 9, 80, 25, false);
	NEW_CLASS(this->txtID, UI::GUITextBox(ui, this, CSTR("")));
	this->txtID->SetRect(96, 9, 100, 20, false);
	this->txtID->SetReadOnly(true);
	NEW_CLASS(this->btnNew, UI::GUIButton(ui, this, CSTR("N&ew")));
	this->btnNew->SetRect(224, 9, 80, 25, false);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("&Cancel")));
	this->btnCancel->SetRect(224, 43, 80, 25, false);
	this->btnCancel->SetVisible(false);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this, CSTR("Name")));
	this->lblName->SetRect(8, 43, 80, 25, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtName->SetRect(96, 43, 100, 20, false);
	NEW_CLASS(this->lblDescription, UI::GUILabel(ui, this, CSTR("Description")));
	this->lblDescription->SetRect(8, 78, 80, 25, false);
	NEW_CLASS(this->txtDescription, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDescription->SetRect(96, 78, 216, 20, false);
	NEW_CLASS(this->lblDisplay, UI::GUILabel(ui, this, CSTR("")));
	this->lblDisplay->SetRect(8, 113, 304, 26, false);
	NEW_CLASS(this->btnPrev, UI::GUIButton(ui, this, CSTR("&Prev")));
	this->btnPrev->SetRect(8, 147, 75, 25, false);
	NEW_CLASS(this->btnSave, UI::GUIButton(ui, this, CSTR("&Save")));
	this->btnSave->SetRect(120, 147, 75, 25, false);
	NEW_CLASS(this->btnNext, UI::GUIButton(ui, this, CSTR("&Next")));
	this->btnNext->SetRect(240, 147, 75, 25, false);

    if (this->env->GetDVDTypeCount() <= 0)
	{
        this->txtDescription->SetReadOnly(true);
        this->txtID->SetReadOnly(true);
        this->txtName->SetReadOnly(true);
        this->currIndex = 0;
        this->currRec = 0;
	}
    else
	{
        this->currIndex = 0;
        this->currRec = this->env->GetDVDType(this->currIndex);
        this->txtID->SetText(this->currRec->discTypeID->ToCString());
        this->txtName->SetText(this->currRec->name->ToCString());
        this->txtDescription->SetText(this->currRec->description->ToCString());
	}
    this->ShowStatus();
}

SSWR::DiscDB::DiscDBDVDTypeForm::~DiscDBDVDTypeForm()
{
}

void SSWR::DiscDB::DiscDBDVDTypeForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
