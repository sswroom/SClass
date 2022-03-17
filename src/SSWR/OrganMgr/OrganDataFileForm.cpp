#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganDataFileForm.h"
#include "SSWR/OrganMgr/OrganTimeAdjForm.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles)
{
	OrganDataFileForm *me = (OrganDataFileForm*)userObj;
	Bool chg = false;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->env->AddDataFile(files[i]->ToCString()))
		{
			chg = true;
		}
		i++;
	}
	if (chg)
	{
		me->UpdateFileList();
	}
}

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnFilesDblClk(void *userObj, UOSInt itemIndex)
{
	OrganDataFileForm *me = (OrganDataFileForm*)userObj;
	DataFileInfo *dataFile = (DataFileInfo*)me->lvFiles->GetItem(itemIndex);
	if (dataFile == 0)
		return;
	if (dataFile->fileType == 1)
	{
		OrganTimeAdjForm *frm;
		NEW_CLASS(frm, OrganTimeAdjForm(0, me->ui, me->env, dataFile));
		frm->ShowDialog(me);
		DEL_CLASS(frm);
	}
}

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnDeleteClicked(void *userObj)
{
	OrganDataFileForm *me = (OrganDataFileForm*)userObj;
	Text::StringBuilderUTF8 sb;
	DataFileInfo *dataFile = (DataFileInfo*)me->lvFiles->GetSelectedItem();
	if (dataFile == 0)
		return;
	sb.AppendC(UTF8STRC("Are you sure to delete "));
	sb.AppendC(dataFile->fileName->v, dataFile->fileName->leng);
	sb.AppendC(UTF8STRC("?"));
	if (UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("Question"), me))
	{
		if (me->env->DelDataFile(dataFile))
		{
			me->UpdateFileList();
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnStartTimeClicked(void *userObj)
{
	OrganDataFileForm *me = (OrganDataFileForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	DataFileInfo *dataFile = (DataFileInfo*)me->lvFiles->GetSelectedItem();
	if (dataFile == 0)
		return;

	Data::DateTime dt;
	dt.SetTicks(dataFile->startTimeTicks);
	dt.ToLocalTime();
	sptr = dt.ToString(sbuff, "dd/MM/yyyy HH:mm:ss");
	Win32::Clipboard::SetString(me->GetHandle(), CSTRP(sbuff, sptr));
}

void SSWR::OrganMgr::OrganDataFileForm::UpdateFileList()
{
	this->lvFiles->ClearItems();

	Data::ArrayList<DataFileInfo*> *dataFiles = this->env->GetDataFiles();
	DataFileInfo *dataFile;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Data::DateTime dt;
	i = 0;
	j = dataFiles->GetCount();
	while (i < j)
	{
		dataFile = dataFiles->GetItem(i);
		k = this->lvFiles->AddItem(dataFile->oriFileName, dataFile);
		dt.SetTicks(dataFile->startTimeTicks);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->lvFiles->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		dt.SetTicks(dataFile->endTimeTicks);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->lvFiles->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		i++;
	}
}

SSWR::OrganMgr::OrganDataFileForm::OrganDataFileForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Data File Form"));
	this->SetFont(0, 0, 10.5, false);

	this->env = env;

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 32, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnDelete, UI::GUIButton(ui, this->pnlCtrl, this->env->GetLang(UTF8STRC("DataFileDelete"))));
	this->btnDelete->SetRect(4, 4, 75, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	NEW_CLASS(this->btnStartTime, UI::GUIButton(ui, this->pnlCtrl, this->env->GetLang(UTF8STRC("DataFileStartTime"))));
	this->btnStartTime->SetRect(104, 4, 75, 23, false);
	this->btnStartTime->HandleButtonClick(OnStartTimeClicked, this);
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->SetShowGrid(true);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->AddColumn(this->env->GetLang(UTF8STRC("DataFileColFileName")), 200);
	this->lvFiles->AddColumn(this->env->GetLang(UTF8STRC("DataFileColStartTime")), 120);
	this->lvFiles->AddColumn(this->env->GetLang(UTF8STRC("DataFileColEndTime")), 120);
	this->lvFiles->HandleDblClk(OnFilesDblClk, this);

	this->UpdateFileList();
	this->HandleDropFiles(OnFileDrop, this);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganDataFileForm::~OrganDataFileForm()
{
}

void SSWR::OrganMgr::OrganDataFileForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

