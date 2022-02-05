#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRConsoleMediaPlayerForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	me->player->PBStop();
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->player->OpenFile({files[i], Text::StrCharCnt(files[i])}))
		{
			return;
		}
		i++;
	}
	UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in loading files", (const UTF8Char*)"Console Media Player", me);
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnRotateChg(void *userObj)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	me->player->SetRotateType((Media::RotateType)(OSInt)me->cboRotate->GetSelectedItem());
}

SSWR::AVIRead::AVIRConsoleMediaPlayerForm::AVIRConsoleMediaPlayerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 240, ui)
{
	this->SetText((const UTF8Char*)"Console Media Player");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->listener = 0;
	this->webIface = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, (const UTF8Char*)"Control Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtPort->SetRect(104, 4, 60, 23, false);
	this->txtPort->SetReadOnly(true);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this, (const UTF8Char*)"Stop"));
	this->btnStop->SetRect(4, 28, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);
	NEW_CLASS(this->lblRotate, UI::GUILabel(ui, this, (const UTF8Char*)"Rotate"));
	this->lblRotate->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboRotate, UI::GUIComboBox(ui, this, false));
	this->cboRotate->SetRect(104, 52, 100, 23, false);
	this->cboRotate->AddItem(CSTR("No Rotate"), (void*)Media::RotateType::None);
	this->cboRotate->AddItem(CSTR("CW 90"), (void*)Media::RotateType::CW_90);
	this->cboRotate->AddItem(CSTR("CW 180"), (void*)Media::RotateType::CW_180);
	this->cboRotate->AddItem(CSTR("CW 270"), (void*)Media::RotateType::CW_270);
	this->cboRotate->SetSelectedIndex(0);
	this->cboRotate->HandleSelectionChange(OnRotateChg, this);

	NEW_CLASS(this->player, Media::ConsoleMediaPlayer(this->core->GetMonitorMgr(), this->core->GetColorMgr(), this->core->GetParserList(), this->core->GetAudioDevice()));
	if (this->player->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in initialize player", (const UTF8Char*)"Console Media Player", this);
	}
	UTF8Char sbuff[32];
	UInt16 port = 8080;
	NEW_CLASS(this->webIface, Media::MediaPlayerWebInterface(this->player, false));
	while (port < 8090)
	{
		NEW_CLASS(this->listener, Net::WebServer::WebListener(this->core->GetSocketFactory(), 0, this->webIface, port, 10, 2, (const UTF8Char*)"ConsoleMediaPlayer/1.0", false, true));
		if (this->listener->IsError())
		{
			DEL_CLASS(this->listener);
			this->listener = 0;
			port++;
		}
		else
		{
			Text::StrUInt16(sbuff, port);
			this->txtPort->SetText(sbuff);
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
