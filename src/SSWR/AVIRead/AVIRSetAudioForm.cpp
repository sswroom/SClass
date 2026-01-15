#include "Stdafx.h"
#include "Media/AudioDevice.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "Text/StringBuilder.hpp"

void __stdcall SSWR::AVIRead::AVIRSetAudioForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetAudioForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetAudioForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!me->lbDevice->GetSelectedItemText(sbuff).SetTo(sptr) || Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Default")))
	{
		me->core->SetAudioDeviceList(nullptr);
	}
	else
	{
		Data::ArrayListStringNN audDevList;
		audDevList.Add(Text::String::NewP(sbuff, sptr));
		me->core->SetAudioDeviceList(&audDevList);
		OPTSTR_DEL(audDevList.RemoveAt(0));
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSetAudioForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetAudioForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetAudioForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRSetAudioForm::AVIRSetAudioForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 272, ui)
{
	this->SetText(CSTR("Select Audio Device"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblDevice = ui->NewLabel(*this, CSTR("Audio Device"));
	this->lblDevice->SetRect(8, 8, 100, 23, false);
	this->lbDevice = ui->NewListBox(*this, false);
	this->lbDevice->SetRect(112, 8, 256, 112, false);


	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(112, 208, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(192, 208, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	NN<Data::ArrayListStringNN> audDevList = this->core->GetAudioDeviceList();
	Optional<Text::String> devName = audDevList->GetItem(0);
	NN<Text::String> s;
	Bool found = false;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;

	this->lbDevice->AddItem(CSTR("Default"), 0);
	i = 0;
	j = Media::AudioDevice::GetDeviceCount();
	while (i < j)
	{
		if (Media::AudioDevice::GetDeviceName(sbuff, i).SetTo(sptr))
		{
			k = this->lbDevice->AddItem(CSTRP(sbuff, sptr), 0);
			if (devName.SetTo(s) && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), s->v, s->leng))
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
