#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRFileSearchForm.h"
#include "UtilUI/TextViewerForm.h"

#define FILEBUFFSIZE 65536

void __stdcall SSWR::AVIRead::AVIRFileSearchForm::OnSearchClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRFileSearchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileSearchForm>();
	NN<Text::TextBinEnc::TextBinEnc> enc;
	Text::StringBuilderUTF8 sbText;
	Text::StringBuilderUTF8 sbDir;
	UInt8 dataBuff[256];
	UIntOS dataSize;
	if (!me->cboEncoding->GetSelectedItem().GetOpt<Text::TextBinEnc::TextBinEnc>().SetTo(enc))
	{
		me->ui->ShowMsgOK(CSTR("Please select an encoding first"), CSTR("File Search"), me);
		return;
	}
	me->txtText->GetText(sbText);
	if (sbText.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter text to search"), CSTR("File Search"), me);
		return;
	}
	me->txtDir->GetText(sbDir);
	if (sbDir.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Directory to search"), CSTR("File Search"), me);
		return;
	}
	if (sbDir.GetLength() >= 512)
	{
		me->ui->ShowMsgOK(CSTR("Directory is too long"), CSTR("File Search"), me);
		return;
	}
	if (IO::Path::GetPathType(sbDir.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Directory is not valid"), CSTR("File Search"), me);
		return;
	}
	dataSize = enc->DecodeBin(sbText.ToCString(), dataBuff);
	if (dataSize == 0)
	{
		me->ui->ShowMsgOK(CSTR("Error in decoding text"), CSTR("File Search"), me);
		return;
	}
	me->ClearFiles();
	me->lvFiles->ClearItems();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr = sbDir.ConcatTo(sbuff);
	me->FindDir(sbuff, sptr, dataBuff, dataSize);
}

void __stdcall SSWR::AVIRead::AVIRFileSearchForm::OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRFileSearchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileSearchForm>();
	UIntOS i = 0;
	UIntOS nFiles = files.GetCount();
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

void __stdcall SSWR::AVIRead::AVIRFileSearchForm::OnFilesDblClk(AnyType userObj, UIntOS itemIndex)
{
	NN<SSWR::AVIRead::AVIRFileSearchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileSearchForm>();
	NN<Text::String> filePath;
	if (!me->lvFiles->GetItem(itemIndex).GetOpt<Text::String>().SetTo(filePath))
		return;
	NN<UtilUI::TextViewerForm> frm;
	NEW_CLASSNN(frm, UtilUI::TextViewerForm(nullptr, me->ui, me->core->GetMonitorMgr(), me->core->GetDrawEngine(), me->core->GetCurrCodePage()));
	me->core->ShowForm(frm);
	if (frm->LoadFile(filePath))
	{
		Text::StringBuilderUTF8 sb;
		me->txtText->GetText(sb);
		frm->OpenSearch(sb.ToCString());
		frm->SearchText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRFileSearchForm::ClearFiles()
{
	UIntOS i = this->fileList.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->fileList.GetItem(i));
	}
	this->fileList.Clear();
}

void SSWR::AVIRead::AVIRFileSearchForm::FindDir(UnsafeArray<UTF8Char> dir, UnsafeArray<UTF8Char> dirEnd, UnsafeArray<const UInt8> searchBuff, UIntOS searchLen)
{
	UnsafeArray<UTF8Char> sptr;
	if (dirEnd[-1] != IO::Path::PATH_SEPERATOR)
		*dirEnd++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(dirEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	if (!IO::Path::FindFile(CSTRP(dir, sptr)).SetTo(sess))
		return;

	UInt8 *fileBuff = 0;
	UIntOS fileBuffSize;
	UIntOS readSize;
	UIntOS i;
	UIntOS j;
	UIntOS matchCount;

	IO::Path::PathType pt;
	while (IO::Path::FindNextFile(dirEnd, sess, 0, pt, 0).SetTo(sptr))
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
				NN<Text::String> s = Text::String::NewP(dir, sptr);
				i = this->lvFiles->AddItem(s, s.Ptr());
				sptr = Text::StrUIntOS(dirEnd, matchCount);
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

SSWR::AVIRead::AVIRFileSearchForm::AVIRFileSearchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 750, 344, ui)
{
	this->SetText(CSTR("File Search"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 103, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDir = ui->NewLabel(this->pnlControl, CSTR("Directory"));
	this->lblDir->SetRect(4, 4, 100, 23, false);
	this->txtDir = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtDir->SetRect(104, 4, 600, 23, false);
	this->lblEncoding = ui->NewLabel(this->pnlControl, CSTR("Encoding"));
	this->lblEncoding->SetRect(4, 28, 100, 23, false);
	this->cboEncoding = ui->NewComboBox(this->pnlControl, false);
	this->cboEncoding->SetRect(104, 28, 200, 23, false);
	NN<Data::ArrayListNN<Text::TextBinEnc::TextBinEnc>> encs = this->encList.GetEncList();
	NN<Text::TextBinEnc::TextBinEnc> enc;
	UIntOS i;
	UIntOS j;
	i = 0;
	j = encs->GetCount();
	while (i < j)
	{
		enc = encs->GetItemNoCheck(i);
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
	this->lblText = ui->NewLabel(this->pnlControl, CSTR("Text"));
	this->lblText->SetRect(4, 52, 100, 23, false);
	this->txtText = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtText->SetRect(104, 52, 600, 23, false);
	this->btnSearch = ui->NewButton(this->pnlControl, CSTR("Search"));
	this->btnSearch->SetRect(104, 76, 75, 23, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	this->lvFiles = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
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
