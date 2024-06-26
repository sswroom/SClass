#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"
#include "UI/GUILabel.h"
#include "UI/GUIButton.h"
#include "SSWR/AVIRead/AVIRCodePageForm.h"

void __stdcall SSWR::AVIRead::AVIRCodePageForm::OKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCodePageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCodePageForm>();
	UOSInt selInd = me->lbCodePages->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		me->core->SetCodePage((UInt32)(UOSInt)me->lbCodePages->GetItem(selInd).p);
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRCodePageForm::CancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCodePageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCodePageForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRCodePageForm::AVIRCodePageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 260, 664, ui)
{
	this->SetText(CSTR("Select Code Page"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<UI::GUILabel> lbl;
	
	lbl = ui->NewLabel(*this, CSTR("Code Pages"));
	lbl->SetRect(8, 8, 120, 23, false);

	this->lbCodePages = ui->NewListBox(*this, false);
	this->lbCodePages->SetRect(8, 32, 240, 560, false);

	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(40, 600, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(128, 600, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UOSInt i;
	UOSInt j;
	UOSInt ind;
	UInt32 codePage;
	UInt32 currCodePage = this->core->GetCurrCodePage();
	Data::ArrayListUInt32 codePages;
	Text::EncodingFactory::GetCodePages(codePages);
	i = 0;
	j = codePages.GetCount();
	while (i < j)
	{
		codePage = codePages.GetItem(i);
		sptr = Text::EncodingFactory::GetName(Text::StrConcatC(Text::StrUInt32(sbuff, codePage), UTF8STRC(" ")), codePage);
		ind = this->lbCodePages->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)codePage);
		if (codePage == currCodePage)
		{
			this->lbCodePages->SetSelectedIndex(ind);
		}
		i++;
	}
	this->lbCodePages->Focus();
}

SSWR::AVIRead::AVIRCodePageForm::~AVIRCodePageForm()
{
}

void SSWR::AVIRead::AVIRCodePageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
