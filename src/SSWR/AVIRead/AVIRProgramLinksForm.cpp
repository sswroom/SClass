#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "SSWR/AVIRead/AVIRProgramLinksCreateForm.h"
#include "SSWR/AVIRead/AVIRProgramLinksForm.h"
#include "Text/StringComparatorFastNN.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRProgramLinksForm::OnItemsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRProgramLinksForm *me = (SSWR::AVIRead::AVIRProgramLinksForm *)userObj;
	Text::String *s = me->lbItems->GetSelectedItemTextNew();
	if (s)
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

void __stdcall SSWR::AVIRead::AVIRProgramLinksForm::OnDeleteClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProgramLinksForm *me = (SSWR::AVIRead::AVIRProgramLinksForm *)userObj;
	Text::String *s = me->lbItems->GetSelectedItemTextNew();
	if (s)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Are you sure to delete program link \""));
		sb.Append(s);
		sb.AppendUTF8Char('\"');
		if (UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("Program Links"), me))
		{
			if (!me->progMgr.DeleteLink(s->ToCString()))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Error in deleting program link \""));
				sb.Append(s);
				sb.AppendUTF8Char('\"');
				UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Program Links"), me);
			}
			else
			{
				me->UpdateLinkList();
			}
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRProgramLinksForm::OnCreateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProgramLinksForm *me = (SSWR::AVIRead::AVIRProgramLinksForm *)userObj;
	SSWR::AVIRead::AVIRProgramLinksCreateForm frm(0, me->ui, me->core, &me->progMgr);
	if (frm.ShowDialog(me))
	{
		me->UpdateLinkList();
	}
}

void SSWR::AVIRead::AVIRProgramLinksForm::UpdateLinkList()
{
	this->lbItems->ClearItems();
	Data::ArrayListNN<Text::String> nameList;
	Text::StringComparatorFastNN comparator;
	this->progMgr.GetLinkNames(&nameList, true, true);
	Data::Sort::ArtificialQuickSort::Sort(&nameList, &comparator);
	NotNullPtr<Text::String> s;
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

SSWR::AVIRead::AVIRProgramLinksForm::AVIRProgramLinksForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Program Links"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbItems, UI::GUIListBox(ui, this, false));
	this->lbItems->SetRect(0, 0, 200, 23, false);
	this->lbItems->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbItems->HandleSelectionChange(OnItemsSelChg, this);
	NEW_CLASS(this->hspItems, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlItem, UI::GUIPanel(ui, this));
	this->pnlItem->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnDelete, UI::GUIButton(ui, this->pnlItem, CSTR("Delete")));
	this->btnDelete->SetRect(4, 4, 75, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	NEW_CLASS(this->btnCreate, UI::GUIButton(ui, this->pnlItem, CSTR("Create")));
	this->btnCreate->SetRect(84, 4, 75, 23, false);
	this->btnCreate->HandleButtonClick(OnCreateClicked, this);
	NEW_CLASS(this->lblLinkName, UI::GUILabel(ui, this->pnlItem, CSTR("Link Name")));
	this->lblLinkName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtLinkName, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtLinkName->SetRect(104, 28, 500, 23, false);
	this->txtLinkName->SetReadOnly(true);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this->pnlItem, CSTR("Type")));
	this->lblType->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtType, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtType->SetRect(104, 52, 500, 23, false);
	this->txtType->SetReadOnly(true);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this->pnlItem, CSTR("Name")));
	this->lblName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtName->SetRect(104, 76, 500, 23, false);
	this->txtName->SetReadOnly(true);
	NEW_CLASS(this->lblGenericName, UI::GUILabel(ui, this->pnlItem, CSTR("Generic Name")));
	this->lblGenericName->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtGenericName, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtGenericName->SetRect(104, 100, 500, 23, false);
	this->txtGenericName->SetReadOnly(true);
	NEW_CLASS(this->lblVersion, UI::GUILabel(ui, this->pnlItem, CSTR("Version")));
	this->lblVersion->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtVersion, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtVersion->SetRect(104, 124, 500, 23, false);
	this->txtVersion->SetReadOnly(true);
	NEW_CLASS(this->lblComment, UI::GUILabel(ui, this->pnlItem, CSTR("Comment")));
	this->lblComment->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtComment, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtComment->SetRect(104, 148, 500, 23, false);
	this->txtComment->SetReadOnly(true);
	NEW_CLASS(this->lblMimeTypes, UI::GUILabel(ui, this->pnlItem, CSTR("MIME Types")));
	this->lblMimeTypes->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtMimeTypes, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtMimeTypes->SetRect(104, 172, 500, 23, false);
	this->txtMimeTypes->SetReadOnly(true);
	NEW_CLASS(this->lblCategories, UI::GUILabel(ui, this->pnlItem, CSTR("Categories")));
	this->lblCategories->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtCategories, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtCategories->SetRect(104, 196, 500, 23, false);
	this->txtCategories->SetReadOnly(true);
	NEW_CLASS(this->lblKeywords, UI::GUILabel(ui, this->pnlItem, CSTR("Keywords")));
	this->lblKeywords->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtKeywords, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtKeywords->SetRect(104, 220, 500, 23, false);
	this->txtKeywords->SetReadOnly(true);
	NEW_CLASS(this->lblCmdLine, UI::GUILabel(ui, this->pnlItem, CSTR("Command Line")));
	this->lblCmdLine->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->txtCmdLine, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtCmdLine->SetRect(104, 244, 500, 23, false);
	this->txtCmdLine->SetReadOnly(true);
	NEW_CLASS(this->lblNoDisplay, UI::GUILabel(ui, this->pnlItem, CSTR("No Display")));
	this->lblNoDisplay->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtNoDisplay, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtNoDisplay->SetRect(104, 268, 500, 23, false);
	this->txtNoDisplay->SetReadOnly(true);
	NEW_CLASS(this->lblStartupNotify, UI::GUILabel(ui, this->pnlItem, CSTR("Startup Notify")));
	this->lblStartupNotify->SetRect(4, 292, 100, 23, false);
	NEW_CLASS(this->txtStartupNotify, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtStartupNotify->SetRect(104, 292, 500, 23, false);
	this->txtStartupNotify->SetReadOnly(true);
	NEW_CLASS(this->lblTerminal, UI::GUILabel(ui, this->pnlItem, CSTR("Terminal")));
	this->lblTerminal->SetRect(4, 316, 100, 23, false);
	NEW_CLASS(this->txtTerminal, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
	this->txtTerminal->SetRect(104, 316, 500, 23, false);
	this->txtTerminal->SetReadOnly(true);
	NEW_CLASS(this->lblIcon, UI::GUILabel(ui, this->pnlItem, CSTR("Icon")));
	this->lblIcon->SetRect(4, 340, 100, 23, false);
	NEW_CLASS(this->txtIcon, UI::GUITextBox(ui, this->pnlItem, CSTR("")));
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
