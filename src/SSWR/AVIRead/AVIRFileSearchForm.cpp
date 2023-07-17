#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRFileSearchForm.h"
#include "UtilUI/TextViewerForm.h"
#include "UI/MessageDialog.h"

#define FILEBUFFSIZE 65536

void __stdcall SSWR::AVIRead::AVIRFileSearchForm::OnSearchClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileSearchForm *me = (SSWR::AVIRead::AVIRFileSearchForm*)userObj;
	Text::TextBinEnc::ITextBinEnc *enc = (Text::TextBinEnc::ITextBinEnc*)me->cboEncoding->GetSelectedItem();
	Text::StringBuilderUTF8 sbText;
	Text::StringBuilderUTF8 sbDir;
	UInt8 dataBuff[256];
	UOSInt dataSize;
	me->txtText->GetText(&sbText);
	if (sbText.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter text to search"), CSTR("File Search"), me);
		return;
	}
	me->txtDir->GetText(&sbDir);
	if (sbDir.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter Directory to search"), CSTR("File Search"), me);
		return;
	}
	if (sbDir.GetLength() >= 512)
	{
		UI::MessageDialog::ShowDialog(CSTR("Directory is too long"), CSTR("File Search"), me);
		return;
	}
	if (IO::Path::GetPathType(sbDir.ToCString()) != IO::Path::PathType::Directory)
	{
		UI::MessageDialog::ShowDialog(CSTR("Directory is not valid"), CSTR("File Search"), me);
		return;
	}
	dataSize = enc->DecodeBin(sbText.v, sbText.leng, dataBuff);
	if (dataSize == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in decoding text"), CSTR("File Search"), me);
		return;
	}
	me->ClearFiles();
	me->lvFiles->ClearItems();
	UTF8Char sbuff[512];
	UTF8Char *sptr = sbDir.ConcatTo(sbuff);
	me->FindDir(sbuff, sptr, dataBuff, dataSize);
}

void __stdcall SSWR::AVIRead::AVIRFileSearchForm::OnDirectoryDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRFileSearchForm *me = (SSWR::AVIRead::AVIRFileSearchForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (IO::Path::GetPathType(files[i]->ToCString()) == IO::Path::PathType::Directory)
		{
			me->txtDir->SetText(files[i]->ToCString());
			break;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRFileSearchForm::OnFilesDblClk(void *userObj, UOSInt itemIndex)
{
	SSWR::AVIRead::AVIRFileSearchForm *me = (SSWR::AVIRead::AVIRFileSearchForm*)userObj;
	NotNullPtr<Text::String> filePath;
	if (!filePath.Set((Text::String*)me->lvFiles->GetItem(itemIndex)))
		return;
	UtilUI::TextViewerForm *frm;
	NEW_CLASS(frm, UtilUI::TextViewerForm(0, me->ui, me->core->GetMonitorMgr(), me->core->GetDrawEngine(), me->core->GetCurrCodePage()));
	me->core->ShowForm(frm);
	if (frm->LoadFile(filePath))
	{
		Text::StringBuilderUTF8 sb;
		me->txtText->GetText(&sb);
		frm->OpenSearch(sb.ToCString());
		frm->SearchText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRFileSearchForm::ClearFiles()
{
	UOSInt i = this->fileList.GetCount();
	while (i-- > 0)
	{
		this->fileList.GetItem(i)->Release();
	}
	this->fileList.Clear();
}

void SSWR::AVIRead::AVIRFileSearchForm::FindDir(UTF8Char *dir, UTF8Char *dirEnd, const UInt8 *searchBuff, UOSInt searchLen)
{
	UTF8Char *sptr;
	if (dirEnd[-1] != IO::Path::PATH_SEPERATOR)
		*dirEnd++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(dirEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(dir, sptr));
	if (sess == 0)
		return;

	UInt8 *fileBuff = 0;
	UOSInt fileBuffSize;
	UOSInt readSize;
	UOSInt i;
	UOSInt j;
	UOSInt matchCount;

	IO::Path::PathType pt;
	while ((sptr = IO::Path::FindNextFile(dirEnd, sess, 0, &pt, 0)) != 0)
	{
		if (pt == IO::Path::PathType::Directory)
		{
			if (dirEnd[0] != '.')
				this->FindDir(dir, sptr, searchBuff, searchLen);
		}
		else if (pt == IO::Path::PathType::File)
		{
			if (fileBuff == 0)
				fileBuff = MemAlloc(UInt8, FILEBUFFSIZE);
			matchCount = 0;
			fileBuffSize = 0;
			IO::FileStream fs(CSTRP(dir, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			while (true)
			{
				readSize = fs.Read(Data::ByteArray(&fileBuff[fileBuffSize], FILEBUFFSIZE - fileBuffSize));
				if (readSize == 0)
					break;
				fileBuffSize += readSize;
				j = fileBuffSize - searchLen + 1;
				i = 0;
				while (i < j)
				{
					if (Text::StrEqualsC(&fileBuff[i], searchLen, searchBuff, searchLen))
					{
						matchCount++;
						i += searchLen;
					}
					else
						i++;
				}
				if (i >= fileBuffSize)
				{
					fileBuffSize = 0;
				}
				else if (i > 0)
				{
					MemCopyO(&fileBuff[i], fileBuff, fileBuffSize - i);
					fileBuffSize -= i;
				}
			}
			if (matchCount > 0)
			{
				NotNullPtr<Text::String> s = Text::String::NewP(dir, sptr);
				i = this->lvFiles->AddItem(s, s.Ptr());
				sptr = Text::StrUOSInt(dirEnd, matchCount);
				this->lvFiles->SetSubItem(i, 1, CSTRP(dirEnd, sptr));
				this->fileList.Add(s);
			}
		}
	}

	IO::Path::FindFileClose(sess);
	if (fileBuff)
	{
		MemFree(fileBuff);
	}
}

SSWR::AVIRead::AVIRFileSearchForm::AVIRFileSearchForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 750, 344, ui)
{
	this->SetText(CSTR("File Search"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 103, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDir, UI::GUILabel(ui, this->pnlControl, CSTR("Directory")));
	this->lblDir->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDir, UI::GUITextBox(ui, this->pnlControl, CSTR("")));
	this->txtDir->SetRect(104, 4, 600, 23, false);
	NEW_CLASS(this->lblEncoding, UI::GUILabel(ui, this->pnlControl, CSTR("Encoding")));
	this->lblEncoding->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboEncoding, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboEncoding->SetRect(104, 28, 200, 23, false);
	Data::ArrayList<Text::TextBinEnc::ITextBinEnc*> *encs = this->encList.GetEncList();
	Text::TextBinEnc::ITextBinEnc *enc;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = encs->GetCount();
	while (i < j)
	{
		enc = encs->GetItem(i);
		this->cboEncoding->AddItem(enc->GetName(), enc);
		i++;
	}
	if (j > 2)
	{
		this->cboEncoding->SetSelectedIndex(2);
	}
	else if (j > 0)
	{
		this->cboEncoding->SetSelectedIndex(0);
	}
	NEW_CLASS(this->lblText, UI::GUILabel(ui, this->pnlControl, CSTR("Text")));
	this->lblText->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtText, UI::GUITextBox(ui, this->pnlControl, CSTR("")));
	this->txtText->SetRect(104, 52, 600, 23, false);
	NEW_CLASS(this->btnSearch, UI::GUIButton(ui, this->pnlControl, CSTR("Search")));
	this->btnSearch->SetRect(104, 76, 75, 23, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->AddColumn(CSTR("File Name"), 400);
	this->lvFiles->AddColumn(CSTR("Match Count"), 200);
	this->lvFiles->HandleDblClk(OnFilesDblClk, this);
	
	this->HandleDropFiles(OnDirectoryDrop, this);
}

SSWR::AVIRead::AVIRFileSearchForm::~AVIRFileSearchForm()
{
	this->ClearFiles();
}

void SSWR::AVIRead::AVIRFileSearchForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
