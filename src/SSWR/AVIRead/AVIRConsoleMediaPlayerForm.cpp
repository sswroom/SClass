#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "SSWR/AVIRead/AVIRConsoleMediaPlayerForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	me->player->PBStop();
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnCaptureDevClicked(void *userObj)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	SSWR::AVIRead::AVIRCaptureDevForm dlg(0, me->ui, me->core);
	if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		Media::MediaFile *mf;
		sptr = dlg.capture->GetSourceName(sbuff);
		NEW_CLASS(mf, Media::MediaFile(CSTRP(sbuff, sptr)));
		mf->AddSource(dlg.capture, 0);
		me->player->OpenVideo(mf);
	}
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->player->OpenFile(files[i]->ToCString()))
		{
			return;
		}
		i++;
	}
	UI::MessageDialog::ShowDialog(CSTR("Error in loading files"), CSTR("Console Media Player"), me);
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnRotateChg(void *userObj)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	me->player->SetRotateType((Media::RotateType)(OSInt)me->cboRotate->GetSelectedItem());
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnSurfaceBugChg(void *userObj, Bool newVal)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	me->player->SetSurfaceBugMode(me->chkSurfaceBug->IsChecked());
}

SSWR::AVIRead::AVIRConsoleMediaPlayerForm::AVIRConsoleMediaPlayerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 240, ui)
{
	this->SetText(CSTR("Console Media Player"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->listener = 0;
	this->webIface = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, CSTR("Control Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPort->SetRect(104, 4, 60, 23, false);
	this->txtPort->SetReadOnly(true);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this, CSTR("Stop")));
	this->btnStop->SetRect(4, 28, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);
	NEW_CLASS(this->btnCaptureDev, UI::GUIButton(ui, this, CSTR("Capture Device")));
	this->btnCaptureDev->SetRect(4, 52, 150, 23, false);
	this->btnCaptureDev->HandleButtonClick(OnCaptureDevClicked, this);
	NEW_CLASS(this->lblRotate, UI::GUILabel(ui, this, CSTR("Rotate")));
	this->lblRotate->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->cboRotate, UI::GUIComboBox(ui, this, false));
	this->cboRotate->SetRect(104, 76, 100, 23, false);
	this->cboRotate->AddItem(CSTR("No Rotate"), (void*)Media::RotateType::None);
	this->cboRotate->AddItem(CSTR("CW 90"), (void*)Media::RotateType::CW_90);
	this->cboRotate->AddItem(CSTR("CW 180"), (void*)Media::RotateType::CW_180);
	this->cboRotate->AddItem(CSTR("CW 270"), (void*)Media::RotateType::CW_270);
	this->cboRotate->SetSelectedIndex(0);
	this->cboRotate->HandleSelectionChange(OnRotateChg, this);
	NEW_CLASS(this->chkSurfaceBug, UI::GUICheckBox(ui, this, CSTR("Surface Bug"), false));
	this->chkSurfaceBug->SetRect(4, 100, 200, 23, false);
	this->chkSurfaceBug->HandleCheckedChange(OnSurfaceBugChg, this);

	NEW_CLASS(this->player, Media::ConsoleMediaPlayer(this->core->GetMonitorMgr(), this->core->GetColorMgr(), this->core->GetParserList(), this->core->GetAudioDevice()));
	if (this->player->IsError())
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in initialize player"), CSTR("Console Media Player"), this);
	}
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt16 port = 8080;
	NEW_CLASS(this->webIface, Media::MediaPlayerWebInterface(this->player, false));
	while (port < 8090)
	{
		NEW_CLASS(this->listener, Net::WebServer::WebListener(this->core->GetSocketFactory(), 0, this->webIface, port, 10, 2, CSTR("ConsoleMediaPlayer/1.0"), false, true));
		if (this->listener->IsError())
		{
			DEL_CLASS(this->listener);
			this->listener = 0;
			port++;
		}
		else
		{
			sptr = Text::StrUInt16(sbuff, port);
			this->txtPort->SetText(CSTRP(sbuff, sptr));
			break;
		}
	}
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRConsoleMediaPlayerForm::~AVIRConsoleMediaPlayerForm()
{
	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->webIface);
	DEL_CLASS(this->player);
}

void SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
