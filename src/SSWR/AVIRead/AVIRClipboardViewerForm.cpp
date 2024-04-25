#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRClipboardViewerForm.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRClipboardViewerForm::OnTypeSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRClipboardViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRClipboardViewerForm>();
	UInt32 fmtId = (UInt32)(UOSInt)me->lbType->GetSelectedItem().p;
	if (fmtId)
	{
		Text::StringBuilderUTF8 sb;
		me->clipboard->GetDataText(fmtId, sb);
		me->txtMain->SetText(sb.ToCString());
	}
}

SSWR::AVIRead::AVIRClipboardViewerForm::AVIRClipboardViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Clipboard Viewer"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lbType = ui->NewListBox(*this, false);
	this->lbType->SetRect(0, 0, 200, 23, false);
	this->lbType->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbType->HandleSelectionChange(OnTypeSelChg, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->txtMain = ui->NewTextBox(*this, CSTR(""), true);
	this->txtMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtMain->SetReadOnly(true);

	NEW_CLASS(this->clipboard, UI::Clipboard(this->hwnd));

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt32 fmt;
	Data::ArrayList<UInt32> formats;
	this->clipboard->GetDataFormats(&formats);
	i = 0;
	j = formats.GetCount();
	while (i < j)
	{
		fmt = formats.GetItem(i);
		if ((sptr = UI::Clipboard::GetFormatName(fmt, sbuff, 256)) != 0)
		{
			this->lbType->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)fmt);
		}
		else
		{
			this->lbType->AddItem(CSTR("Unknown"), (void*)(OSInt)fmt);
		}
		i++;
	}

}

SSWR::AVIRead::AVIRClipboardViewerForm::~AVIRClipboardViewerForm()
{
	DEL_CLASS(this->clipboard);
}

void SSWR::AVIRead::AVIRClipboardViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
