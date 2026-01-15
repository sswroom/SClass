#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "SSWR/DiscDB/DiscDBBurntDiscForm.h"
#include "SSWR/DiscDB/DiscDBDVDTypeForm.h"
#include "SSWR/DiscDB/DiscDBMainForm.h"
#include "SSWR/DiscDB/DiscDBSearchDiscForm.h"
#include "Text/StringBuilderUTF8.h"

typedef enum
{
	MNU_DVDTYPE = 100,
	MNU_BURNT_DISC,
	MNU_SEARCH_DISC
} MenuItems;

void __stdcall SSWR::DiscDB::DiscDBMainForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::DiscDB::DiscDBMainForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBMainForm>();
	Bool succ;
	Bool failed = false;
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		succ = false;
		{
			IO::StmData::FileData fd(files[i], false);
			if (fd.GetDataSize() > 0)
			{
				succ = me->env->AddMD5(fd);
			}
		}
		if (!succ)
		{
			Text::StringBuilderUTF8 sb;
			if ((i + 1) >= nFiles)
			{
				sb.AppendC(UTF8STRC("Error in parsing "));
				sb.Append(files[i]);
				failed = true;
				me->ui->ShowMsgOK(sb.ToCString(), CSTR("DiscDB"), me);
			}
			else
			{
				sb.AppendC(UTF8STRC("Error in parsing "));
				sb.Append(files[i]);
				sb.AppendC(UTF8STRC(", do you want to continue?"));
				if (!me->ui->ShowMsgYesNo(sb.ToCString(), CSTR("DiscDB"), me))
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
		me->ui->ShowMsgOK(CSTR("Finished inputting to database"), CSTR("DiscDB"), me);
	}
}

SSWR::DiscDB::DiscDBMainForm::DiscDBMainForm(NN<UI::GUICore> ui, Optional<UI::GUIClientControl> parent, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 430, 344, ui)
{
	this->SetText(CSTR("Main Menu"));
	this->SetFont(nullptr, 8.25, false);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));

	NN<UI::GUIMenu> mnu;

	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
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
			SSWR::DiscDB::DiscDBDVDTypeForm frm(nullptr, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	case MNU_BURNT_DISC:
		{
			SSWR::DiscDB::DiscDBBurntDiscForm frm(nullptr, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	case MNU_SEARCH_DISC:
		{
			SSWR::DiscDB::DiscDBSearchDiscForm frm(nullptr, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	}
}

void SSWR::DiscDB::DiscDBMainForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
