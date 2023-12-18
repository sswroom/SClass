#include "Stdafx.h"
#include "SSWR/DiscDB/DiscDBSearchDiscForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::DiscDB::DiscDBSearchDiscForm::OnSearchClicked(void *userObj)
{
	SSWR::DiscDB::DiscDBSearchDiscForm *me = (SSWR::DiscDB::DiscDBSearchDiscForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDiscId->GetText(sb);
	if (sb.GetLength() <= 0)
		return;

	const SSWR::DiscDB::DiscDBEnv::BurntDiscInfo *disc = me->env->GetBurntDisc(sb.ToCString());
	SSWR::DiscDB::DiscDBEnv::DiscFileInfo *file;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (disc)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		if (disc->burntDate == 0)
		{
			me->txtBurntDate->SetText(CSTR(""));
		}
		else
		{
			sptr = disc->burntDate.ToString(sbuff, "yyyy-MM-dd");
			me->txtBurntDate->SetText(CSTRP(sbuff, sptr));
		}
		me->txtDiscType->SetText(disc->discTypeId->ToCString());
		me->txtDiscIdOut->SetText(disc->discId->ToCString());

		me->lvFiles->ClearItems();
		Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DiscFileInfo*> fileList;
		me->env->GetBurntFiles(disc->discId->ToCString(), &fileList);
		i = 0;
		j = fileList.GetCount();
		while (i < j)
		{
			file = fileList.GetItem(i);
			k = me->lvFiles->AddItem(file->fileName, 0);
			sptr = Text::StrUInt64(sbuff, file->fileSize);
			me->lvFiles->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			me->lvFiles->SetSubItem(k, 2, Text::CStringNN::FromPtr(file->category));
			i++;
		}
		me->env->FreeBurntFiles(&fileList);
	}
}

SSWR::DiscDB::DiscDBSearchDiscForm::DiscDBSearchDiscForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 300, 322, ui)
{
	this->SetText(CSTR("DVDType"));
	this->SetFont(0, 0, 8.25, false);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlDiscId, UI::GUIPanel(ui, *this));
	this->pnlDiscId->SetRect(0, 0, 292, 43, false);
	this->pnlDiscId->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDiscId = ui->NewLabel(this->pnlDiscId, CSTR("Disc ID"));
	this->lblDiscId->SetRect(8, 9, 56, 25, false);
	NEW_CLASS(this->txtDiscId, UI::GUITextBox(ui, this->pnlDiscId, CSTR("")));
	this->txtDiscId->SetRect(64, 9, 120, 20, false);
	this->btnSearch = ui->NewButton(this->pnlDiscId, CSTR("&Search"));
	this->btnSearch->SetRect(208, 9, 75, 25, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	NEW_CLASSNN(this->pnlOut, UI::GUIPanel(ui, *this));
	this->pnlOut->SetRect(0, 0, 292, 108, false);
	this->pnlOut->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDiscIdOut = ui->NewLabel(this->pnlOut, CSTR("Disc ID"));
	this->lblDiscIdOut->SetRect(8, 9, 64, 25, false);
	NEW_CLASS(this->txtDiscIdOut, UI::GUITextBox(ui, this->pnlOut, CSTR("")));
	this->txtDiscIdOut->SetRect(72, 9, 100, 20, false);
	this->txtDiscIdOut->SetReadOnly(true);
	this->lblDiscType = ui->NewLabel(this->pnlOut, CSTR("Type"));
	this->lblDiscType->SetRect(8, 35, 64, 25, false);
	NEW_CLASS(this->txtDiscType, UI::GUITextBox(ui, this->pnlOut, CSTR("")));
	this->txtDiscType->SetRect(72, 35, 100, 20, false);
	this->txtDiscType->SetReadOnly(true);
	this->lblBurntDate = ui->NewLabel(this->pnlOut, CSTR("Date"));
	this->lblBurntDate->SetRect(8, 61, 64, 25, false);
	NEW_CLASS(this->txtBurntDate, UI::GUITextBox(ui, this->pnlOut, CSTR("")));
	this->txtBurntDate->SetRect(72, 61, 100, 20, false);
	this->txtBurntDate->SetReadOnly(true);
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->SetShowGrid(true);
	this->lvFiles->AddColumn(CSTR("File Name"), 150);
	this->lvFiles->AddColumn(CSTR("Size"), 80);
	this->lvFiles->AddColumn(CSTR("Category"), 80);

	this->txtDiscId->Focus();
	this->SetDefaultButton(this->btnSearch);
}

SSWR::DiscDB::DiscDBSearchDiscForm::~DiscDBSearchDiscForm()
{
}

void SSWR::DiscDB::DiscDBSearchDiscForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
