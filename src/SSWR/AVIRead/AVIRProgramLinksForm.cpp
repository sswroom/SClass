#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "SSWR/AVIRead/AVIRProgramLinksCreateForm.h"
#include "SSWR/AVIRead/AVIRProgramLinksForm.h"
#include "Text/StringComparatorFastNN.h"

void __stdcall SSWR::AVIRead::AVIRProgramLinksForm::OnItemsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProgramLinksForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProgramLinksForm>();
	NN<Text::String> s;
	if (me->lbItems->GetSelectedItemTextNew().SetTo(s))
	{
		IO::ProgramLink link;
		me->txtLinkName->SetText(s->ToCString());
		if (!me->progMgr.GetLinkDetail(s->ToCString(), &link))
		{
			me->txtType->SetText(CSTR(""));
			me->txtName->SetText(CSTR(""));
			me->txtGenericName->SetText(CSTR(""));
			me->txtVersion->SetText(CSTR(""));
			me->txtComment->SetText(CSTR(""));
			me->txtMimeTypes->SetText(CSTR(""));
			me->txtCategories->SetText(CSTR(""));
			me->txtKeywords->SetText(CSTR(""));
			me->txtCmdLine->SetText(CSTR(""));
			me->txtNoDisplay->SetText(CSTR(""));
			me->txtStartupNotify->SetText(CSTR(""));
			me->txtTerminal->SetText(CSTR(""));
			me->txtIcon->SetText(CSTR(""));
		}
		else
		{
			me->txtType->SetText(Text::String::OrEmpty(link.GetType())->ToCString());
			me->txtName->SetText(Text::String::OrEmpty(link.GetName())->ToCString());
			me->txtGenericName->SetText(Text::String::OrEmpty(link.GetGenericName())->ToCString());
			me->txtVersion->SetText(Text::String::OrEmpty(link.GetVersion())->ToCString());
			me->txtComment->SetText(Text::String::OrEmpty(link.GetComment())->ToCString());
			me->txtMimeTypes->SetText(Text::String::OrEmpty(link.GetMimeTypes())->ToCString());
			me->txtCategories->SetText(Text::String::OrEmpty(link.GetCategories())->ToCString());
			me->txtKeywords->SetText(Text::String::OrEmpty(link.GetKeywords())->ToCString());
			me->txtCmdLine->SetText(Text::String::OrEmpty(link.GetCmdLine())->ToCString());
			me->txtNoDisplay->SetText(link.IsNoDisplay()?CSTR("true"):CSTR("false"));
			me->txtStartupNotify->SetText(link.IsStartupNotify()?CSTR("true"):CSTR("false"));
			me->txtTerminal->SetText(link.HasTerminal()?CSTR("true"):CSTR("false"));
			me->txtIcon->SetText(Text::String::OrEmpty(link.GetIcon())->ToCString());
		}
		s->Release();
	}
	else
	{
		me->txtLinkName->SetText(CSTR(""));
		me->txtType->SetText(CSTR(""));
		me->txtName->SetText(CSTR(""));
		me->txtGenericName->SetText(CSTR(""));
		me->txtVersion->SetText(CSTR(""));
		me->txtComment->SetText(CSTR(""));
		me->txtMimeTypes->SetText(CSTR(""));
		me->txtCategories->SetText(CSTR(""));
		me->txtKeywords->SetText(CSTR(""));
		me->txtCmdLine->SetText(CSTR(""));
		me->txtNoDisplay->SetText(CSTR(""));
		me->txtStartupNotify->SetText(CSTR(""));
		me->txtTerminal->SetText(CSTR(""));
		me->txtIcon->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRProgramLinksForm::OnDeleteClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProgramLinksForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProgramLinksForm>();
	NN<Text::String> s;
	if (me->lbItems->GetSelectedItemTextNew().SetTo(s))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Are you sure to delete program link \""));
		sb.Append(s);
		sb.AppendUTF8Char('\"');
		if (me->ui->ShowMsgYesNo(sb.ToCString(), CSTR("Program Links"), me))
		{
			if (!me->progMgr.DeleteLink(s->ToCString()))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Error in deleting program link \""));
				sb.Append(s);
				sb.AppendUTF8Char('\"');
				me->ui->ShowMsgOK(sb.ToCString(), CSTR("Program Links"), me);
			}
			else
			{
				me->UpdateLinkList();
			}
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRProgramLinksForm::OnCreateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProgramLinksForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProgramLinksForm>();
	SSWR::AVIRead::AVIRProgramLinksCreateForm frm(0, me->ui, me->core, &me->progMgr);
	if (frm.ShowDialog(me))
	{
		me->UpdateLinkList();
	}
}

void SSWR::AVIRead::AVIRProgramLinksForm::UpdateLinkList()
{
	this->lbItems->ClearItems();
	Data::ArrayListStringNN nameList;
	Text::StringComparatorFastNN comparator;
	this->progMgr.GetLinkNames(&nameList, true, true);
	Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(nameList, comparator);
	NN<Text::String> s;
	UOSInt i = 0;
	UOSInt j = nameList.GetCount();
	while (i < j)
	{
		s = Text::String::OrEmpty(nameList.GetItem(i));
		this->lbItems->AddItem(s, 0);
		s->Release();
		i++;
	}
}

SSWR::AVIRead::AVIRProgramLinksForm::AVIRProgramLinksForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Program Links"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbItems = ui->NewListBox(*this, false);
	this->lbItems->SetRect(0, 0, 200, 23, false);
	this->lbItems->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbItems->HandleSelectionChange(OnItemsSelChg, this);
	this->hspItems = ui->NewHSplitter(*this, 3, false);
	this->pnlItem = ui->NewPanel(*this);
	this->pnlItem->SetDockType(UI::GUIControl::DOCK_FILL);
	this->btnDelete = ui->NewButton(this->pnlItem, CSTR("Delete"));
	this->btnDelete->SetRect(4, 4, 75, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	this->btnCreate = ui->NewButton(this->pnlItem, CSTR("Create"));
	this->btnCreate->SetRect(84, 4, 75, 23, false);
	this->btnCreate->HandleButtonClick(OnCreateClicked, this);
	this->lblLinkName = ui->NewLabel(this->pnlItem, CSTR("Link Name"));
	this->lblLinkName->SetRect(4, 28, 100, 23, false);
	this->txtLinkName = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtLinkName->SetRect(104, 28, 500, 23, false);
	this->txtLinkName->SetReadOnly(true);
	this->lblType = ui->NewLabel(this->pnlItem, CSTR("Type"));
	this->lblType->SetRect(4, 52, 100, 23, false);
	this->txtType = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtType->SetRect(104, 52, 500, 23, false);
	this->txtType->SetReadOnly(true);
	this->lblName = ui->NewLabel(this->pnlItem, CSTR("Name"));
	this->lblName->SetRect(4, 76, 100, 23, false);
	this->txtName = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtName->SetRect(104, 76, 500, 23, false);
	this->txtName->SetReadOnly(true);
	this->lblGenericName = ui->NewLabel(this->pnlItem, CSTR("Generic Name"));
	this->lblGenericName->SetRect(4, 100, 100, 23, false);
	this->txtGenericName = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtGenericName->SetRect(104, 100, 500, 23, false);
	this->txtGenericName->SetReadOnly(true);
	this->lblVersion = ui->NewLabel(this->pnlItem, CSTR("Version"));
	this->lblVersion->SetRect(4, 124, 100, 23, false);
	this->txtVersion = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtVersion->SetRect(104, 124, 500, 23, false);
	this->txtVersion->SetReadOnly(true);
	this->lblComment = ui->NewLabel(this->pnlItem, CSTR("Comment"));
	this->lblComment->SetRect(4, 148, 100, 23, false);
	this->txtComment = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtComment->SetRect(104, 148, 500, 23, false);
	this->txtComment->SetReadOnly(true);
	this->lblMimeTypes = ui->NewLabel(this->pnlItem, CSTR("MIME Types"));
	this->lblMimeTypes->SetRect(4, 172, 100, 23, false);
	this->txtMimeTypes = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtMimeTypes->SetRect(104, 172, 500, 23, false);
	this->txtMimeTypes->SetReadOnly(true);
	this->lblCategories = ui->NewLabel(this->pnlItem, CSTR("Categories"));
	this->lblCategories->SetRect(4, 196, 100, 23, false);
	this->txtCategories = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtCategories->SetRect(104, 196, 500, 23, false);
	this->txtCategories->SetReadOnly(true);
	this->lblKeywords = ui->NewLabel(this->pnlItem, CSTR("Keywords"));
	this->lblKeywords->SetRect(4, 220, 100, 23, false);
	this->txtKeywords = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtKeywords->SetRect(104, 220, 500, 23, false);
	this->txtKeywords->SetReadOnly(true);
	this->lblCmdLine = ui->NewLabel(this->pnlItem, CSTR("Command Line"));
	this->lblCmdLine->SetRect(4, 244, 100, 23, false);
	this->txtCmdLine = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtCmdLine->SetRect(104, 244, 500, 23, false);
	this->txtCmdLine->SetReadOnly(true);
	this->lblNoDisplay = ui->NewLabel(this->pnlItem, CSTR("No Display"));
	this->lblNoDisplay->SetRect(4, 268, 100, 23, false);
	this->txtNoDisplay = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtNoDisplay->SetRect(104, 268, 500, 23, false);
	this->txtNoDisplay->SetReadOnly(true);
	this->lblStartupNotify = ui->NewLabel(this->pnlItem, CSTR("Startup Notify"));
	this->lblStartupNotify->SetRect(4, 292, 100, 23, false);
	this->txtStartupNotify = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtStartupNotify->SetRect(104, 292, 500, 23, false);
	this->txtStartupNotify->SetReadOnly(true);
	this->lblTerminal = ui->NewLabel(this->pnlItem, CSTR("Terminal"));
	this->lblTerminal->SetRect(4, 316, 100, 23, false);
	this->txtTerminal = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtTerminal->SetRect(104, 316, 500, 23, false);
	this->txtTerminal->SetReadOnly(true);
	this->lblIcon = ui->NewLabel(this->pnlItem, CSTR("Icon"));
	this->lblIcon->SetRect(4, 340, 100, 23, false);
	this->txtIcon = ui->NewTextBox(this->pnlItem, CSTR(""));
	this->txtIcon->SetRect(104, 340, 500, 23, false);
	this->txtIcon->SetReadOnly(true);

	this->UpdateLinkList();
}

SSWR::AVIRead::AVIRProgramLinksForm::~AVIRProgramLinksForm()
{

}

void SSWR::AVIRead::AVIRProgramLinksForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
