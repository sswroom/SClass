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
	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"SNBImg");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrOSInt(sptr, val);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".jpg");

	me->pbImage->SetImage(0);
	SDEL_CLASS(me->simg);

	Parser::ParserList *parsers = me->core->GetParserList();
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
	Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
	DEL_CLASS(fd);
	if (imgList)
	{
		Int32 delay;
		imgList->ToStaticImage(0);
		me->simg = imgList;
		me->pbImage->SetImage((Media::StaticImage*)imgList->GetImage(0, &delay));
	}
}

SSWR::AVIRead::AVIRSNBHandlerForm::AVIRSNBHandlerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::SNBDongle::HandleType handType) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"SnB Handle Type");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->simg = 0;
	this->handType = handType;

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 200, 100, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Type"));
	this->lblType->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->cboType, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboType->SetRect(0, 24, 180, 23, false);
	this->cboType->HandleSelectionChange(OnTypeSelChg, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(20, 64, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->btnOk, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Ok"));
	this->btnOk->SetRect(100, 64, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	NEW_CLASS(this->pbImage, UI::GUIPictureBox(ui, this, this->core->GetDrawEngine(), false, true));
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);

	OSInt i = 0;
	OSInt j = IO::SNBDongle::HT_LAST;
	OSInt k;
	while (i <= j)
	{
		k = this->cboType->AddItem(IO::SNBDongle::GetHandleName((IO::SNBDongle::HandleType)i), (void*)i);
		if (i == handType)
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
