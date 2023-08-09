#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRHexViewerGoToForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHexViewerGoToForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerGoToForm *me = (SSWR::AVIRead::AVIRHexViewerGoToForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtInput->GetText(sb);

	UInt64 destOfst;
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter offset to go to"), CSTR("Hex Viewer"), me);
		return;
	}
	if (sb.v[0] == '+' || sb.v[0] == '-')
	{
		if (!Text::StrToUInt64(&sb.v[1], destOfst))
		{
			UI::MessageDialog::ShowDialog(CSTR("The offset is not in valid format"), CSTR("Hex Viewer"), me);
			return;
		}
		if (sb.v[0] == '-')
		{
			if (destOfst > me->currOfst)
			{
				UI::MessageDialog::ShowDialog(CSTR("The offset is out of range"), CSTR("Hex Viewer"), me);
				return;
			}
			destOfst -= me->currOfst;
		}
		else
		{
			destOfst += me->currOfst;
		}
	}
	else
	{
		if (!sb.ToUInt64(destOfst))
		{
			UI::MessageDialog::ShowDialog(CSTR("The offset is not in valid format"), CSTR("Hex Viewer"), me);
			return;
		}
	}
	if (destOfst > me->fileLen)
	{
		UI::MessageDialog::ShowDialog(CSTR("The offset is out of range"), CSTR("Hex Viewer"), me);
		return;
	}
	me->currOfst = destOfst;
	me->SetDialogResult(DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRHexViewerGoToForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerGoToForm *me = (SSWR::AVIRead::AVIRHexViewerGoToForm*)userObj;
	me->SetDialogResult(DR_CANCEL);
}

SSWR::AVIRead::AVIRHexViewerGoToForm::AVIRHexViewerGoToForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, UInt64 currOfst, UInt64 fileSize) : UI::GUIForm(parent, 320, 160, ui)
{
	this->core = core;
	this->currOfst = currOfst;
	this->fileLen = fileSize;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Go To"));
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->lblMessage, UI::GUILabel(ui, this, CSTR("Input offset to go to: (Start with +/- for relative offset)")))
	this->lblMessage->SetRect(0, 24, 320, 23, false);
	NEW_CLASS(this->txtInput, UI::GUITextBox(ui, this, CSTR("")));
	this->txtInput->SetRect(0, 56, 320, 23, false);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(80, 88, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(160, 88, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);

	this->SetCancelButton(this->btnCancel);
	this->SetDefaultButton(this->btnOK);
}

SSWR::AVIRead::AVIRHexViewerGoToForm::~AVIRHexViewerGoToForm()
{

}

void SSWR::AVIRead::AVIRHexViewerGoToForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

UInt64 SSWR::AVIRead::AVIRHexViewerGoToForm::GetOffset()
{
	return this->currOfst;
}
