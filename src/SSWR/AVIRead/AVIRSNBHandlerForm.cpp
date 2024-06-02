#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ImageList.h"
#include "SSWR/AVIRead/AVIRSNBHandlerForm.h"
#include "Text/StringBuilder.h"

void __stdcall SSWR::AVIRead::AVIRSNBHandlerForm::OnOkClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSNBHandlerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNBHandlerForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSNBHandlerForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSNBHandlerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNBHandlerForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSNBHandlerForm::OnTypeSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSNBHandlerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNBHandlerForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	OSInt val = me->cboType->GetSelectedItem().GetOSInt();
	me->handType = (IO::SNBDongle::HandleType)val;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SNBImg"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrOSInt(sptr, val);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));

	me->pbImage->SetImage(0);
	me->simg.Delete();

	NN<Parser::ParserList> parsers = me->core->GetParserList();
	NN<Media::ImageList> imgList;
	IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
	if (Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
	{
		imgList->ToStaticImage(0);
		me->simg = imgList;
		me->pbImage->SetImage(Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)));
	}
}

SSWR::AVIRead::AVIRSNBHandlerForm::AVIRSNBHandlerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::SNBDongle::HandleType handType) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SnB Handle Type"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->simg = 0;
	this->handType = handType;

	this->pnlControl = ui->NewPanel(*this);
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
	this->pbImage = ui->NewPictureBox(*this, this->core->GetDrawEngine(), false, true);
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
	this->simg.Delete();
}

void SSWR::AVIRead::AVIRSNBHandlerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

IO::SNBDongle::HandleType SSWR::AVIRead::AVIRSNBHandlerForm::GetHandleType()
{
	return this->handType;
}
