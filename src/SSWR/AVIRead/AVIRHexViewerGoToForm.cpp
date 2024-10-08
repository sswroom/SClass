#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRHexViewerGoToForm.h"

void __stdcall SSWR::AVIRead::AVIRHexViewerGoToForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHexViewerGoToForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHexViewerGoToForm>();
	Text::StringBuilderUTF8 sb;
	me->txtInput->GetText(sb);

	UInt64 destOfst;
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter offset to go to"), CSTR("Hex Viewer"), me);
		return;
	}
	if (sb.v[0] == '+' || sb.v[0] == '-')
	{
		if (!Text::StrToUInt64(&sb.v[1], destOfst))
		{
			me->ui->ShowMsgOK(CSTR("The offset is not in valid format"), CSTR("Hex Viewer"), me);
			return;
		}
		if (sb.v[0] == '-')
		{
			if (destOfst > me->currOfst)
			{
				me->ui->ShowMsgOK(CSTR("The offset is out of range"), CSTR("Hex Viewer"), me);
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
			me->ui->ShowMsgOK(CSTR("The offset is not in valid format"), CSTR("Hex Viewer"), me);
			return;
		}
	}
	if (destOfst > me->fileLen)
	{
		me->ui->ShowMsgOK(CSTR("The offset is out of range"), CSTR("Hex Viewer"), me);
		return;
	}
	me->currOfst = destOfst;
	me->SetDialogResult(DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRHexViewerGoToForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHexViewerGoToForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHexViewerGoToForm>();
	me->SetDialogResult(DR_CANCEL);
}

SSWR::AVIRead::AVIRHexViewerGoToForm::AVIRHexViewerGoToForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, UInt64 currOfst, UInt64 fileSize) : UI::GUIForm(parent, 320, 160, ui)
{
	this->core = core;
	this->currOfst = currOfst;
	this->fileLen = fileSize;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Go To"));
	this->SetFont(0, 0, 8.25, false);

	this->lblMessage = ui->NewLabel(*this, CSTR("Input offset to go to: (Start with +/- for relative offset)"));
	this->lblMessage->SetRect(0, 24, 320, 23, false);
	this->txtInput = ui->NewTextBox(*this, CSTR(""));
	this->txtInput->SetRect(0, 56, 320, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(80, 88, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
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
