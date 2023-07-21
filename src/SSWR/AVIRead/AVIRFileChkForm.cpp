#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRFileChkForm.h"

typedef enum
{
	MNU_FILE_SAVE = 100,
	MNU_FILE_VALIDATE
} MenuItems;

SSWR::AVIRead::AVIRFileChkForm::AVIRFileChkForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, IO::FileCheck *fileChk) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt8 *hash;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetFont(0, 0, 8.25, false);
	sptr = fileChk->GetSourceName(Text::StrConcatC(sbuff, UTF8STRC("File Check - ")));
	this->SetText(CSTRP(sbuff, sptr));
	
	NEW_CLASS(this->mnu, UI::GUIMainMenu());
	UI::GUIMenu *mnu = this->mnu->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Save"), MNU_FILE_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Validate"), MNU_FILE_VALIDATE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnu);

	NEW_CLASS(this->lvFileChk, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvFileChk->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFileChk->AddColumn(CSTR("File Name"), 600);
	this->lvFileChk->AddColumn(CSTR("Check Value"), 250);
	this->lvFileChk->AddColumn(CSTR("Current Value"), 250);
	this->lvFileChk->AddColumn(CSTR("Valid"), 50);

	this->fileChk = fileChk;

	UOSInt hashSize;
	UOSInt i;
	UOSInt j;
	hashSize = this->fileChk->GetHashSize();
	hash = MemAlloc(UInt8, hashSize);
	i = 0;
	j = this->fileChk->GetCount();
	while (i < j)
	{
		this->lvFileChk->AddItem(Text::String::OrEmpty(this->fileChk->GetEntryName(i)), 0);
		this->fileChk->GetEntryHash(i, hash);
		sptr = Text::StrHexBytes(sbuff, hash, hashSize, 0);
		this->lvFileChk->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		this->lvFileChk->SetSubItem(i, 2, CSTR("-"));
		this->lvFileChk->SetSubItem(i, 3, CSTR("-"));
		i++;
	}
	MemFree(hash);
}

SSWR::AVIRead::AVIRFileChkForm::~AVIRFileChkForm()
{
	DEL_CLASS(this->fileChk);
}

void SSWR::AVIRead::AVIRFileChkForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_FILE_SAVE:
		this->core->SaveData(this, this->fileChk, L"SaveFileChk");
		break;
	case MNU_FILE_VALIDATE:
		{
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			UOSInt hashSize;
			UInt8 *hash;
			UInt8 *hash2;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			Bool eq;
			hashSize = this->fileChk->GetHashSize();
			hash = MemAlloc(UInt8, hashSize);
			hash2 = MemAlloc(UInt8, hashSize);
			i = 0;
			j = this->fileChk->GetCount();
			while (i < j)
			{
				if (this->fileChk->CheckEntryHash(i, hash))
				{
					sptr = Text::StrHexBytes(sbuff, hash, hashSize, 0);
					this->lvFileChk->SetSubItem(i, 2, CSTRP(sbuff, sptr));
					this->fileChk->GetEntryHash(i, hash2);
					eq = true;
					k = hashSize;
					while (k-- > 0)
					{
						if (hash2[k] != hash[k])
						{
							eq = false;
							break;
						}
					}
					this->lvFileChk->SetSubItem(i, 3, eq?CSTR("Y"):CSTR("N"));
				}
				else
				{
					this->lvFileChk->SetSubItem(i, 3, CSTR("Err"));
				}
				i++;
			}
			MemFree(hash2);
			MemFree(hash);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRFileChkForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
