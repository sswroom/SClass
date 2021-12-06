#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSMakeForm.h"

void __stdcall SSWR::AVIRead::AVIRSMakeForm::OnProgSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSMakeForm *me = (SSWR::AVIRead::AVIRSMakeForm*)userObj;
	Text::String *progName = me->lbProg->GetSelectedItemTextNew();
	me->lbProgHeader->ClearItems();
	me->lbProgObject->ClearItems();
	me->lbProgSource->ClearItems();
	if (progName)
	{
		Data::ArrayListStrUTF8 objList;
		Data::ArrayListStrUTF8 liblist;
		Data::ArrayListStrUTF8 procList;
		Data::ArrayListStrUTF8 headerList;
		Int64 latestTime;
		Bool progGroup;
		const UTF8Char *csptr;
		UOSInt i;
		UOSInt j;
		const IO::SMake::ProgramItem *prog;
		me->smake->ParseProg(&objList, &liblist, &procList, &headerList, &latestTime, &progGroup, progName);
		procList.Clear();
		i = 0;
		j = objList.GetCount();
		while (i < j)
		{
			csptr = objList.GetItem(i);
			me->lbProgObject->AddItem(csptr, 0);
			prog = me->smake->GetProgItem(csptr);
			if (prog && prog->srcFile)
			{
				if (prog->srcFile[0] == '@')
				{
					procList.SortedInsert(prog->srcFile + 1);
				}
				else
				{
					procList.SortedInsert(prog->srcFile);
				}
			}
			i++;
		}

		i = 0;
		j = procList.GetCount();
		while (i < j)
		{
			csptr = procList.GetItem(i);
			me->lbProgSource->AddItem(csptr, 0);
			i++;
		}

		i = 0;
		j = headerList.GetCount();
		while (i < j)
		{
			csptr = headerList.GetItem(i);
			me->lbProgHeader->AddItem(csptr, 0);
			i++;
		}

		progName->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRSMakeForm::OnProgGroupSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSMakeForm *me = (SSWR::AVIRead::AVIRSMakeForm*)userObj;
	Text::String *progName = me->lbProgGroup->GetSelectedItemTextNew();
	me->lbProgGroupItems->ClearItems();
	if (progName)
	{
		const IO::SMake::ProgramItem *prog = me->smake->GetProgItem(progName->v);
		if (prog)
		{
			UOSInt i = 0;
			UOSInt j = prog->subItems->GetCount();
			while (i < j)
			{
				me->lbProgGroupItems->AddItem(prog->subItems->GetItem(i), 0);
				i++;
			}
		}
		progName->Release();
	}
}

SSWR::AVIRead::AVIRSMakeForm::AVIRSMakeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::SMake *smake) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->smake = smake;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText((const UTF8Char*)"SMake Form");

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->pnlFile, UI::GUIPanel(ui, this));
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, this->smake->GetSourceNameObj()->v));
	this->txtFile->SetReadOnly(true);
	this->txtFile->SetRect(104, 4, 600, 23, false);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgGroup = this->tcMain->AddTabPage((const UTF8Char*)"Program Group");
	NEW_CLASS(this->lbProgGroup, UI::GUIListBox(ui, this->tpProgGroup, false));
	this->lbProgGroup->SetRect(0, 0, 150, 23, false);
	this->lbProgGroup->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProgGroup->HandleSelectionChange(OnProgGroupSelChg, this);
	NEW_CLASS(this->hspProgGroup, UI::GUIHSplitter(ui, this->tpProgGroup, 3, false));
	NEW_CLASS(this->lbProgGroupItems, UI::GUIListBox(ui, this->tpProgGroup, false));
	this->lbProgGroupItems->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProg = this->tcMain->AddTabPage((const UTF8Char*)"Program");
	NEW_CLASS(this->lbProg, UI::GUIListBox(ui, this->tpProg, false));
	this->lbProg->SetRect(0, 0, 150, 23, false);
	this->lbProg->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProg->HandleSelectionChange(OnProgSelChg, this);
	NEW_CLASS(this->hspProg, UI::GUIHSplitter(ui, this->tpProg, 3, false));
	NEW_CLASS(this->pnlProg, UI::GUIPanel(ui, this->tpProg));
	this->pnlProg->SetRect(0, 0, 100, 31, false);
	this->pnlProg->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->tcProg, UI::GUITabControl(ui, this->tpProg));
	this->tcProg->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgObject = this->tcProg->AddTabPage((const UTF8Char*)"Object");
	NEW_CLASS(this->lbProgObject, UI::GUIListBox(ui, this->tpProgObject, false));
	this->lbProgObject->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgHeader = this->tcProg->AddTabPage((const UTF8Char*)"Header");
	NEW_CLASS(this->lbProgHeader, UI::GUIListBox(ui, this->tpProgHeader, false));
	this->lbProgHeader->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgSource = this->tcProg->AddTabPage((const UTF8Char*)"Source");
	NEW_CLASS(this->lbProgSource, UI::GUIListBox(ui, this->tpProgSource, false));
	this->lbProgSource->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpConfig = this->tcMain->AddTabPage((const UTF8Char *)"Config");
	NEW_CLASS(this->lvConfig, UI::GUIListView(ui, this->tpConfig, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvConfig->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvConfig->SetShowGrid(true);
	this->lvConfig->SetFullRowSelect(true);
	this->lvConfig->AddColumn((const UTF8Char*)"Name", 150);
	this->lvConfig->AddColumn((const UTF8Char*)"Value", 450);

	Data::ArrayList<const UTF8Char*> progList;
	const UTF8Char *progName;
	UOSInt i = 0;
	UOSInt j = this->smake->GetProgList(&progList);
	while (i < j)
	{
		progName = progList.GetItem(i);
		if (this->smake->IsProgGroup(progName))
		{
			this->lbProgGroup->AddItem(progName, (void*)progName);
		}
		else
		{
			this->lbProg->AddItem(progName, (void*)progName);
		}
		i++;
	}

	Data::ArrayList<IO::SMake::ConfigItem*> *configList = this->smake->GetConfigList();
	IO::SMake::ConfigItem *config;
	i = 0;
	j = configList->GetCount();
	while (i < j)
	{
		config = configList->GetItem(i);
		this->lvConfig->AddItem(config->name, config);
		this->lvConfig->SetSubItem(i, 1, config->value);
		i++;
	}
}

SSWR::AVIRead::AVIRSMakeForm::~AVIRSMakeForm()
{
	DEL_CLASS(this->smake);
}

void SSWR::AVIRead::AVIRSMakeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
