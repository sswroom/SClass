#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganDataFileForm.h"
#include "SSWR/OrganMgr/OrganTimeAdjForm.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files)
{
	NotNullPtr<OrganDataFileForm> me = userObj.GetNN<OrganDataFileForm>();
	Bool chg = false;
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
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

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnFilesDblClk(AnyType userObj, UOSInt itemIndex)
{
	NotNullPtr<OrganDataFileForm> me = userObj.GetNN<OrganDataFileForm>();
	NotNullPtr<DataFileInfo> dataFile;
	if (!me->lvFiles->GetItem(itemIndex).GetOpt<DataFileInfo>().SetTo(dataFile))
		return;
	if (dataFile->fileType == 1)
	{
		OrganTimeAdjForm frm(0, me->ui, me->env, dataFile);
		frm.ShowDialog(me);
	}
}

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnDeleteClicked(AnyType userObj)
{
	NotNullPtr<OrganDataFileForm> me = userObj.GetNN<OrganDataFileForm>();
	Text::StringBuilderUTF8 sb;
	NotNullPtr<DataFileInfo> dataFile;
	if (!me->lvFiles->GetSelectedItem().GetOpt<DataFileInfo>().SetTo(dataFile))
		return;
	sb.AppendC(UTF8STRC("Are you sure to delete "));
	sb.AppendC(dataFile->fileName->v, dataFile->fileName->leng);
	sb.AppendC(UTF8STRC("?"));
	if (me->ui->ShowMsgYesNo(sb.ToCString(), CSTR("Question"), me))
	{
		if (me->env->DelDataFile(dataFile))
		{
			me->UpdateFileList();
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganDataFileForm::OnStartTimeClicked(AnyType userObj)
{
	NotNullPtr<OrganDataFileForm> me = userObj.GetNN<OrganDataFileForm>();
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	NotNullPtr<DataFileInfo> dataFile;
	if (!me->lvFiles->GetSelectedItem().GetOpt<DataFileInfo>().SetTo(dataFile))
		return;

	sptr = dataFile->startTime.ToLocalTime().ToString(sbuff, "dd/MM/yyyy HH:mm:ss");
	UI::Clipboard::SetString(me->GetHandle(), CSTRP(sbuff, sptr));
}

void SSWR::OrganMgr::OrganDataFileForm::UpdateFileList()
{
	this->lvFiles->ClearItems();

	NotNullPtr<Data::ArrayListNN<DataFileInfo>> dataFiles = this->env->GetDataFiles();
	NotNullPtr<DataFileInfo> dataFile;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	i = 0;
	j = dataFiles->GetCount();
	while (i < j)
	{
		dataFile = dataFiles->GetItemNoCheck(i);
		k = this->lvFiles->AddItem(dataFile->oriFileName, dataFile);
		sptr = dataFile->startTime.ToLocalTime().ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->lvFiles->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		sptr = dataFile->endTime.ToLocalTime().ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->lvFiles->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		i++;
	}
}

SSWR::OrganMgr::OrganDataFileForm::OrganDataFileForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<OrganEnv> env) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Data File Form"));
	this->SetFont(0, 0, 10.5, false);

	this->env = env;

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 32, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnDelete = ui->NewButton(this->pnlCtrl, this->env->GetLang(CSTR("DataFileDelete")));
	this->btnDelete->SetRect(4, 4, 75, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	this->btnStartTime = ui->NewButton(this->pnlCtrl, this->env->GetLang(CSTR("DataFileStartTime")));
	this->btnStartTime->SetRect(104, 4, 75, 23, false);
	this->btnStartTime->HandleButtonClick(OnStartTimeClicked, this);
	this->lvFiles = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->SetShowGrid(true);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->AddColumn(this->env->GetLang(CSTR("DataFileColFileName")), 200);
	this->lvFiles->AddColumn(this->env->GetLang(CSTR("DataFileColStartTime")), 120);
	this->lvFiles->AddColumn(this->env->GetLang(CSTR("DataFileColEndTime")), 120);
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

