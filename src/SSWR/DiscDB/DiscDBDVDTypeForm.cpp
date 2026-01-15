#include "Stdafx.h"
#include "SSWR/DiscDB/DiscDBDVDTypeForm.h"

void SSWR::DiscDB::DiscDBDVDTypeForm::ShowStatus()
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("You are viewing "));
	sptr = Text::StrUIntOS(sptr, this->currIndex + 1);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" of "));
	sptr = Text::StrUIntOS(sptr, this->env->GetDVDTypeCount());
	this->lblDisplay->SetText(CSTRP(sbuff, sptr));
}

void SSWR::DiscDB::DiscDBDVDTypeForm::UpdateDisplay()
{
	NN<const DiscDBEnv::DVDTypeInfo> currRec;
	if (this->currRec.SetTo(currRec))
	{
		this->txtID->SetText(currRec->discTypeID->ToCString());
		this->txtName->SetText(currRec->name->ToCString());
		this->txtDescription->SetText(currRec->description->ToCString());
	}
}

Bool SSWR::DiscDB::DiscDBDVDTypeForm::UpdateRow()
{
	if (this->currRec.NotNull())
	{
		return true;
	}
	Text::StringBuilderUTF8 sbID;
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbDesc;
	this->txtID->GetText(sbID);
	if (sbID.GetLength() <= 0)
	{
		this->ui->ShowMsgOK(CSTR("Please enter the id"), CSTR("DVDType"), this);
		this->txtID->Focus();
		return false;
	}
	this->txtName->GetText(sbName);
	if (sbName.GetLength() <= 0)
	{
		this->ui->ShowMsgOK(CSTR("Please enter the name"), CSTR("DVDType"), this);
		this->txtName->Focus();
		return false;
	}
	this->txtDescription->GetText(sbDesc);
	if (sbDesc.GetLength() <= 0)
	{
		this->ui->ShowMsgOK(CSTR("Please enter the description"), CSTR("DVDType"), this);
		this->txtDescription->Focus();
		return false;
	}

    if (this->newRec)
	{
		NN<const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo> newRec;
		if (this->env->NewDVDType(sbID.ToCString(), sbName.ToCString(), sbDesc.ToCString()).SetTo(newRec))
		{
			this->newRec = false;
			this->txtID->SetReadOnly(true);
			this->currIndex = (UIntOS)this->env->GetDVDTypeIndex(sbID.ToCString());
			this->currRec = newRec;
			this->btnCancel->SetVisible(false);
			this->ShowStatus();
			return true;
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("ID Already exist"), CSTR("DVDType"), this);
			return false;
		}
	}
	else
	{
		this->env->ModifyDVDType(sbID.ToCString(), sbName.ToCString(), sbDesc.ToCString());
		return true;
	}
}

SSWR::DiscDB::DiscDBDVDTypeForm::DiscDBDVDTypeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 358, 223, ui)
{
	this->SetText(CSTR("Maintain DVD Types"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));

	this->lblID = ui->NewLabel(*this, CSTR("ID"));
	this->lblID->SetRect(8, 9, 80, 25, false);
	this->txtID = ui->NewTextBox(*this, CSTR(""));
	this->txtID->SetRect(96, 9, 100, 20, false);
	this->txtID->SetReadOnly(true);
	this->btnNew = ui->NewButton(*this, CSTR("N&ew"));
	this->btnNew->SetRect(224, 9, 80, 25, false);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(224, 43, 80, 25, false);
	this->btnCancel->SetVisible(false);
	this->lblName = ui->NewLabel(*this, CSTR("Name"));
	this->lblName->SetRect(8, 43, 80, 25, false);
	this->txtName = ui->NewTextBox(*this, CSTR(""));
	this->txtName->SetRect(96, 43, 100, 20, false);
	this->lblDescription = ui->NewLabel(*this, CSTR("Description"));
	this->lblDescription->SetRect(8, 78, 80, 25, false);
	this->txtDescription = ui->NewTextBox(*this, CSTR(""));
	this->txtDescription->SetRect(96, 78, 216, 20, false);
	this->lblDisplay = ui->NewLabel(*this, CSTR(""));
	this->lblDisplay->SetRect(8, 113, 304, 26, false);
	this->btnPrev = ui->NewButton(*this, CSTR("&Prev"));
	this->btnPrev->SetRect(8, 147, 75, 25, false);
	this->btnSave = ui->NewButton(*this, CSTR("&Save"));
	this->btnSave->SetRect(120, 147, 75, 25, false);
	this->btnNext = ui->NewButton(*this, CSTR("&Next"));
	this->btnNext->SetRect(240, 147, 75, 25, false);

    if (this->env->GetDVDTypeCount() <= 0)
	{
        this->txtDescription->SetReadOnly(true);
        this->txtID->SetReadOnly(true);
        this->txtName->SetReadOnly(true);
        this->currIndex = 0;
        this->currRec = nullptr;
	}
    else
	{
		NN<const DiscDBEnv::DVDTypeInfo> currRec;
        this->currIndex = 0;
        this->currRec = this->env->GetDVDType(this->currIndex);
		if (this->currRec.SetTo(currRec))
		{
			this->txtID->SetText(currRec->discTypeID->ToCString());
			this->txtName->SetText(currRec->name->ToCString());
			this->txtDescription->SetText(currRec->description->ToCString());
		}
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
