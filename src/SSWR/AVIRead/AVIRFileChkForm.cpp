#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRFileChkForm.h"
#include "Text/StringBuilderWriter.h"

typedef enum
{
	MNU_FILE_SAVE = 100,
	MNU_FILE_VALIDATE
} MenuItems;

SSWR::AVIRead::AVIRFileChkForm::AVIRFileChkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::FileCheck> fileChk) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt8 *hash;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetFont(0, 0, 8.25, false);
	sptr = fileChk->GetSourceName(Text::StrConcatC(sbuff, UTF8STRC("File Check - ")));
	this->SetText(CSTRP(sbuff, sptr));
	
	NEW_CLASSNN(this->mnu, UI::GUIMainMenu());
	NN<UI::GUIMenu> mnu = this->mnu->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Save"), MNU_FILE_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Validate"), MNU_FILE_VALIDATE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnu);

	this->pnlStatus = ui->NewPanel(*this);
	this->pnlStatus->SetRect(0, 0, 100, 31, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStatus = ui->NewLabel(this->pnlStatus, CSTR("Status"));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	this->txtStatus = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatus->SetRect(104, 4, 150, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->lblTotalFiles = ui->NewLabel(this->pnlStatus, CSTR("Total Files"));
	this->lblTotalFiles->SetRect(254, 4, 100, 23, false);
	this->txtTotalFiles = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtTotalFiles->SetRect(354, 4, 100, 23, false);
	this->txtTotalFiles->SetReadOnly(true);
	this->lblValidFiles = ui->NewLabel(this->pnlStatus, CSTR("Valid Files"));
	this->lblValidFiles->SetRect(454, 4, 100, 23, false);
	this->txtValidFiles = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtValidFiles->SetRect(554, 4, 100, 23, false);
	this->txtValidFiles->SetReadOnly(true);
	this->lvFileChk = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
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
	this->txtStatus->SetText(CSTR("Idle"));
	sptr = Text::StrUOSInt(sbuff, this->fileChk->GetCount());
	this->txtTotalFiles->SetText(CSTRP(sbuff, sptr));
	this->txtValidFiles->SetText(CSTR("0"));
	MemFree(hash);
}

SSWR::AVIRead::AVIRFileChkForm::~AVIRFileChkForm()
{
	this->fileChk.Delete();
}

void SSWR::AVIRead::AVIRFileChkForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_FILE_SAVE:
		this->core->SaveData(*this, this->fileChk, L"SaveFileChk");
		break;
	case MNU_FILE_VALIDATE:
		{
			this->txtStatus->SetText(CSTR("Checking"));
			UTF8Char sbuff[128];
			UnsafeArray<UTF8Char> sptr;
			UOSInt hashSize;
			UInt8 *hash;
			UInt8 *hash2;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			Bool eq;
			Data::Timestamp lastUpdateTime = Data::Timestamp::UtcNow();
			Data::Timestamp now;
			UOSInt validCnt = 0;
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderWriter writer(sb);
			this->ui->ProcessMessages();
			hashSize = this->fileChk->GetHashSize();
			hash = MemAlloc(UInt8, hashSize);
			hash2 = MemAlloc(UInt8, hashSize);
			i = 0;
			j = this->fileChk->GetCount();
			while (i < j)
			{
				sb.ClearStr();
				if (this->fileChk->CheckEntryHash(i, hash, writer))
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
					if (eq)
					{
						validCnt++;
					}
				}
				else
				{
					sb.TrimWSCRLF();
					this->lvFileChk->SetSubItem(i, 3, sb.ToCString());
				}
				now = Data::Timestamp::UtcNow();
				if (now.Diff(lastUpdateTime).GetTotalSec() >= 1)
				{
					lastUpdateTime = now;
					sptr = Text::StrUOSInt(sbuff, validCnt);
					this->txtValidFiles->SetText(CSTRP(sbuff, sptr));
					this->ui->ProcessMessages();
				}
				i++;
			}
			MemFree(hash2);
			MemFree(hash);
			sptr = Text::StrUOSInt(sbuff, validCnt);
			this->txtValidFiles->SetText(CSTRP(sbuff, sptr));
			this->txtStatus->SetText(CSTR("Idle"));
		}
		break;
	}
}

void SSWR::AVIRead::AVIRFileChkForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
