#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ImageList.h"
#include "SSWR/AVIRead/AVIRSNBHandlerForm.h"
#include "Text/StringBuilder.h"

void __stdcall SSWR::AVIRead::AVIRSNBHandlerForm::OnOkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBHandlerForm *me = (SSWR::AVIRead::AVIRSNBHandlerForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSNBHandlerForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBHandlerForm *me = (SSWR::AVIRead::AVIRSNBHandlerForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSNBHandlerForm::OnTypeSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSNBHandlerForm *me = (SSWR::AVIRead::AVIRSNBHandlerForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	OSInt val = (OSInt)me->cboType->GetSelectedItem();
	me->handType = (IO::SNBDongle::HandleType)val;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SNBImg"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrOSInt(sptr, val);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));

	me->pbImage->SetImage(0);
	SDEL_CLASS(me->simg);

	NotNullPtr<Parser::ParserList> parsers = me->core->GetParserList();
	Media::ImageList *imgList;
	{
		IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
		imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParserType::ImageList);
	}
	if (imgList)
	{
		imgList->ToStaticImage(0);
		me->simg = imgList;
		me->pbImage->SetImage((Media::StaticImage*)imgList->GetImage(0, 0));
	}
}

SSWR::AVIRead::AVIRSNBHandlerForm::AVIRSNBHandlerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SNBDongle::HandleType handType) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SnB Handle Type"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->simg = 0;
	this->handType = handType;

	NEW_CLASSNN(this->pnlControl, UI::GUIPanel(ui, *this));
	this->pnlControl->SetRect(0, 0, 200, 100, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblType = ui->NewLabel(this->pnlControl, CSTR("Type"));
	this->lblType->SetRect(0, 0, 100, 23, false);
	this->cboType = ui->NewComboBox(this->pnlControl, false);
	this->cboType->SetRect(0, 24, 180, 23, false);
	this->cboType->HandleSelectionChange(OnTypeSelChg, this);
	this->btnCancel = ui->NewButton(this->pnlControl, CSTR("Cancel"));
	this->btnCancel->SetRect(20, 64, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOk = ui->NewButton(this->pnlControl, CSTR("Ok"));
	this->btnOk->SetRect(100, 64, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	NEW_CLASS(this->pbImage, UI::GUIPictureBox(ui, *this, this->core->GetDrawEngine(), false, true));
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);

	UOSInt i = 0;
	UOSInt j = IO::SNBDongle::HT_LAST;
	UOSInt k;
	while (i <= j)
	{
		k = this->cboType->AddItem(IO::SNBDongle::GetHandleName((IO::SNBDongle::HandleType)i), (void*)i);
		if (i == (UOSInt)handType)
		{
			this->cboType->SetSelectedIndex(k);
		}
		i++;
	}
	this->SetDefaultButton(this->btnOk);
	this->SetCancelButton(this->btnCancel);
	this->cboType->Focus();
}

SSWR::AVIRead::AVIRSNBHandlerForm::~AVIRSNBHandlerForm()
{
	this->ClearChildren();
	SDEL_CLASS(this->simg);
}

void SSWR::AVIRead::AVIRSNBHandlerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

IO::SNBDongle::HandleType SSWR::AVIRead::AVIRSNBHandlerForm::GetHandleType()
{
	return this->handType;
}
