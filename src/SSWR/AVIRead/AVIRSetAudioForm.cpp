#include "Stdafx.h"
#include "Media/AudioDevice.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "Text/StringBuilder.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSetAudioForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRSetAudioForm *me = (SSWR::AVIRead::AVIRSetAudioForm*)userObj;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = me->lbDevice->GetSelectedItemText(sbuff);
	if (sptr == 0 || Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Default")))
	{
		me->core->SetAudioDeviceList(0);
	}
	else
	{
		Data::ArrayList<const UTF8Char *> audDevList;
		audDevList.Add(sbuff);
		me->core->SetAudioDeviceList(&audDevList);
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSetAudioForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRSetAudioForm *me = (SSWR::AVIRead::AVIRSetAudioForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRSetAudioForm::AVIRSetAudioForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 456, 272, ui)
{
	this->SetText((const UTF8Char*)"Select Audio Device");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblDevice, UI::GUILabel(ui, this, (const UTF8Char*)"Audio Device"));
	this->lblDevice->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, this, false));
	this->lbDevice->SetRect(112, 8, 256, 112, false);


	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"&OK"));
	this->btnOK->SetRect(112, 208, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"&Cancel"));
	this->btnCancel->SetRect(192, 208, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	Data::ArrayList<const UTF8Char *> *audDevList = this->core->GetAudioDeviceList();
	const UTF8Char *devName = audDevList->GetItem(0);
	Bool found = false;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[256];
	UTF8Char *sptr;

	this->lbDevice->AddItem(CSTR("Default"), 0);
	i = 0;
	j = Media::AudioDevice::GetDeviceCount();
	while (i < j)
	{
		if ((sptr = Media::AudioDevice::GetDeviceName(sbuff, i)) != 0)
		{
			k = this->lbDevice->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, 0);
			if (devName && Text::StrStartsWith(sbuff, devName))
			{
				this->lbDevice->SetSelectedIndex(k);
				found = true;
			}
		}
		i++;
	}
	if (!found)
	{
		this->lbDevice->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRSetAudioForm::~AVIRSetAudioForm()
{
}

void SSWR::AVIRead::AVIRSetAudioForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
