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

void __stdcall SSWR::DiscDB::DiscDBMainForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::DiscDB::DiscDBMainForm *me = (SSWR::DiscDB::DiscDBMainForm *)userObj;
	IO::StmData::FileData *fd;
	Bool succ;
	Bool failed = false;
	UOSInt i = 0;
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
				sb.AppendC(UTF8STRC("Error in parsing "));
				sb.Append(files[i]);
				failed = true;
				UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DiscDB"), me);
			}
			else
			{
				sb.AppendC(UTF8STRC("Error in parsing "));
				sb.Append(files[i]);
				sb.AppendC(UTF8STRC(", do you want to continue?"));
				if (!UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("DiscDB"), me))
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
		UI::MessageDialog::ShowDialog(CSTR("Finished inputting to database"), CSTR("DiscDB"), me);
	}
}

SSWR::DiscDB::DiscDBMainForm::DiscDBMainForm(UI::GUICore *ui, UI::GUIClientControl *parent, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 430, 344, ui)
{
	this->SetText(CSTR("Main Menu"));
	this->SetFont(0, 0, 8.25, false);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));

	UI::GUIMenu *mnu;

	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&Maintain"));
	mnu->AddItem(CSTR("&DVDType"), MNU_DVDTYPE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("&New"));
	mnu->AddItem(CSTR("&Burnt Disc"), MNU_BURNT_DISC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("&Search"));
	mnu->AddItem(CSTR("&Burnt Disc"), MNU_SEARCH_DISC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	this->SetMenu(this->mnuMain);
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::DiscDB::DiscDBMainForm::~DiscDBMainForm()
{
}

void SSWR::DiscDB::DiscDBMainForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_DVDTYPE:
		{
			SSWR::DiscDB::DiscDBDVDTypeForm frm(0, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	case MNU_BURNT_DISC:
		{
			SSWR::DiscDB::DiscDBBurntDiscForm frm(0, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	case MNU_SEARCH_DISC:
		{
			SSWR::DiscDB::DiscDBSearchDiscForm frm(0, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	}
}

void SSWR::DiscDB::DiscDBMainForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
