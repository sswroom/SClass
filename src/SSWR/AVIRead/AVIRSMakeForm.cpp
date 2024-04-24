#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSMakeForm.h"

void __stdcall SSWR::AVIRead::AVIRSMakeForm::OnProgSelChg(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRSMakeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSMakeForm>();
	NotNullPtr<Text::String> progName;
	me->lbProgHeader->ClearItems();
	me->lbProgObject->ClearItems();
	me->lbProgSource->ClearItems();
	if (me->lbProg->GetSelectedItemTextNew().SetTo(progName))
	{
		Data::FastStringMap<Int32> objList;
		Data::FastStringMap<Int32> liblist;
		Data::FastStringMap<Int32> procList;
		Data::ArrayListStringNN headerList;
		Int64 latestTime;
		Bool progGroup;
		NotNullPtr<Text::String> s;
		UOSInt i;
		UOSInt j;
		NN<const IO::SMake::ProgramItem> prog;
		me->smake->ParseProg(objList, liblist, procList, headerList, latestTime, progGroup, progName);
		procList.Clear();
		i = 0;
		j = objList.GetCount();
		while (i < j)
		{
			s = Text::String::OrEmpty(objList.GetKey(i));
			me->lbProgObject->AddItem(s, 0);
			if (me->smake->GetProgItem(s->ToCString()).SetTo(prog) && prog->srcFile)
			{
				procList.Put(prog->srcFile, 1);
			}
			i++;
		}

		i = 0;
		j = procList.GetCount();
		while (i < j)
		{
			s = Text::String::OrEmpty(procList.GetKey(i));
			if (s->v[0] == '@')
			{
				me->lbProgSource->AddItem({s->v + 1, s->leng - 1}, 0);
			}
			else
			{
				me->lbProgSource->AddItem(s, 0);
			}
			i++;
		}

		i = 0;
		j = headerList.GetCount();
		while (i < j)
		{
			s = Text::String::OrEmpty(headerList.GetItem(i));
			me->lbProgHeader->AddItem(s, 0);
			i++;
		}

		progName->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRSMakeForm::OnProgGroupSelChg(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRSMakeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSMakeForm>();
	NotNullPtr<Text::String> progName;
	me->lbProgGroupItems->ClearItems();
	if (me->lbProgGroup->GetSelectedItemTextNew().SetTo(progName))
	{
		NN<const IO::SMake::ProgramItem> prog;
		if (me->smake->GetProgItem(progName->ToCString()).SetTo(prog))
		{
			UOSInt i = 0;
			UOSInt j = prog->subItems.GetCount();
			while (i < j)
			{
				me->lbProgGroupItems->AddItem(Text::String::OrEmpty(prog->subItems.GetItem(i)), 0);
				i++;
			}
		}
		progName->Release();
	}
}

SSWR::AVIRead::AVIRSMakeForm::AVIRSMakeForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::SMake> smake) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->smake = smake;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("SMake Form"));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlFile, this->smake->GetSourceNameObj()->ToCString());
	this->txtFile->SetReadOnly(true);
	this->txtFile->SetRect(104, 4, 600, 23, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgGroup = this->tcMain->AddTabPage(CSTR("Program Group"));
	this->lbProgGroup = ui->NewListBox(this->tpProgGroup, false);
	this->lbProgGroup->SetRect(0, 0, 150, 23, false);
	this->lbProgGroup->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProgGroup->HandleSelectionChange(OnProgGroupSelChg, this);
	this->hspProgGroup = ui->NewHSplitter(this->tpProgGroup, 3, false);
	this->lbProgGroupItems = ui->NewListBox(this->tpProgGroup, false);
	this->lbProgGroupItems->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProg = this->tcMain->AddTabPage(CSTR("Program"));
	this->lbProg = ui->NewListBox(this->tpProg, false);
	this->lbProg->SetRect(0, 0, 150, 23, false);
	this->lbProg->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProg->HandleSelectionChange(OnProgSelChg, this);
	this->hspProg = ui->NewHSplitter(this->tpProg, 3, false);
	this->pnlProg = ui->NewPanel(this->tpProg);
	this->pnlProg->SetRect(0, 0, 100, 31, false);
	this->pnlProg->SetDockType(UI::GUIControl::DOCK_TOP);
	this->tcProg = ui->NewTabControl(this->tpProg);
	this->tcProg->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgObject = this->tcProg->AddTabPage(CSTR("Object"));
	this->lbProgObject = ui->NewListBox(this->tpProgObject, false);
	this->lbProgObject->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgHeader = this->tcProg->AddTabPage(CSTR("Header"));
	this->lbProgHeader = ui->NewListBox(this->tpProgHeader, false);
	this->lbProgHeader->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProgSource = this->tcProg->AddTabPage(CSTR("Source"));
	this->lbProgSource = ui->NewListBox(this->tpProgSource, false);
	this->lbProgSource->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpConfig = this->tcMain->AddTabPage(CSTR("Config"));
	this->lvConfig = ui->NewListView(this->tpConfig, UI::ListViewStyle::Table, 2);
	this->lvConfig->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvConfig->SetShowGrid(true);
	this->lvConfig->SetFullRowSelect(true);
	this->lvConfig->AddColumn(CSTR("Name"), 150);
	this->lvConfig->AddColumn(CSTR("Value"), 450);

	Data::ArrayListNN<Text::String> progList;
	NotNullPtr<Text::String> progName;
	UOSInt i = 0;
	UOSInt j = this->smake->GetProgList(progList);
	while (i < j)
	{
		progName = Text::String::OrEmpty(progList.GetItem(i));
		if (this->smake->IsProgGroup(progName->ToCString()))
		{
			this->lbProgGroup->AddItem(progName, (void*)progName.Ptr());
		}
		else
		{
			this->lbProg->AddItem(progName, (void*)progName.Ptr());
		}
		i++;
	}

	NotNullPtr<const Data::ArrayListNN<IO::SMake::ConfigItem>> configList = this->smake->GetConfigList();
	NN<IO::SMake::ConfigItem> config;
	i = 0;
	j = configList->GetCount();
	while (i < j)
	{
		config = configList->GetItemNoCheck(i);
		this->lvConfig->AddItem(config->name, config);
		this->lvConfig->SetSubItem(i, 1, config->value);
		i++;
	}
}

SSWR::AVIRead::AVIRSMakeForm::~AVIRSMakeForm()
{
	this->smake.Delete();
}

void SSWR::AVIRead::AVIRSMakeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
