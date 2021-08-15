#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRConsoleMediaPlayerForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	me->player->Stop();
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRConsoleMediaPlayerForm *me = (SSWR::AVIRead::AVIRConsoleMediaPlayerForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->player->OpenFile(files[i]))
		{
			return;
		}
		i++;
	}
	UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in loading files", (const UTF8Char*)"Console Media Player", me);
}

SSWR::AVIRead::AVIRConsoleMediaPlayerForm::AVIRConsoleMediaPlayerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 240, ui)
{
	this->SetText((const UTF8Char*)"Console Media Player");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this, (const UTF8Char*)"Stop"));
	this->btnStop->SetRect(4, 4, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);

	NEW_CLASS(this->player, Media::ConsoleMediaPlayer(this->core->GetMonitorMgr(), this->core->GetColorMgr(), this->core->GetParserList()));
	if (this->player->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in initialize player", (const UTF8Char*)"Console Media Player", this);
	}
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRConsoleMediaPlayerForm::~AVIRConsoleMediaPlayerForm()
{
	DEL_CLASS(this->player);
}

void SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
