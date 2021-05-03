#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "SSWR/DiscDB/DiscDBBurntDiscForm.h"
#include "SSWR/DiscDB/DiscDBDVDTypeForm.h"
#include "SSWR/DiscDB/DiscDBMainForm.h"
#include "SSWR/DiscDB/DiscDBSearchDiscForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

typedef enum
{
	MNU_DVDTYPE = 100,
	MNU_BURNT_DISC,
	MNU_SEARCH_DISC
} MenuItems;

void __stdcall SSWR::DiscDB::DiscDBMainForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::DiscDB::DiscDBMainForm *me = (SSWR::DiscDB::DiscDBMainForm *)userObj;
	IO::StmData::FileData *fd;
	Bool succ;
	Bool failed = false;
	OSInt i = 0;
	while (i < nFiles)
	{
		succ = false;
		NEW_CLASS(fd, IO::StmData::FileData(files[i], false));
		if (fd->GetDataSize() > 0)
		{
			succ = me->env->AddMD5(fd);
		}
		DEL_CLASS(fd);
		if (!succ)
		{
			Text::StringBuilderUTF8 sb;
			if ((i + 1) >= nFiles)
			{
				sb.Append((const UTF8Char*)"Error in parsing ");
				sb.Append(files[i]);
				failed = true;
				UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"DiscDB", me);
			}
			else
			{
				sb.Append((const UTF8Char*)"Error in parsing ");
				sb.Append(files[i]);
				sb.Append((const UTF8Char*)", do you want to continue?");
				if (!UI::MessageDialog::ShowYesNoDialog(sb.ToString(), (const UTF8Char*)"DiscDB", me))
				{
					failed = true;
					break;
				}
			}
		}
		i++;
	}
	if (!failed)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Finished inputting to database", (const UTF8Char*)"DiscDB", me);
	}
}

SSWR::DiscDB::DiscDBMainForm::DiscDBMainForm(UI::GUICore *ui, UI::GUIClientControl *parent, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 430, 344, ui)
{
	this->SetText((const UTF8Char*)"Main Menu");
	this->SetFont(0, 8.25, false);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));

	UI::GUIMenu *mnu;

	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Maintain");
	mnu->AddItem((const UTF8Char*)"&DVDType", MNU_DVDTYPE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&New");
	mnu->AddItem((const UTF8Char*)"&Burnt Disc", MNU_BURNT_DISC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Search");
	mnu->AddItem((const UTF8Char*)"&Burnt Disc", MNU_SEARCH_DISC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	this->SetMenu(this->mnuMain);
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::DiscDB::DiscDBMainForm::~DiscDBMainForm()
{
}

void SSWR::DiscDB::DiscDBMainForm::EventMenuClicked(UInt16 cmdId)
{
	UI::GUIForm *frm;
	switch (cmdId)
	{
	case MNU_DVDTYPE:
		{
			NEW_CLASS(frm, SSWR::DiscDB::DiscDBDVDTypeForm(0, this->ui, this->env));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_BURNT_DISC:
		{
			NEW_CLASS(frm, SSWR::DiscDB::DiscDBBurntDiscForm(0, this->ui, this->env));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_SEARCH_DISC:
		{
			NEW_CLASS(frm, SSWR::DiscDB::DiscDBSearchDiscForm(0, this->ui, this->env));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	}
}

void SSWR::DiscDB::DiscDBMainForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
