#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRFileSizePackForm.h"

SSWR::AVIRead::AVIRFileSizePackForm::MyFile::MyFile(Text::CString fileName, UInt64 fileSize)
{
	this->fileName = Text::String::New(fileName);
	this->fileSize = fileSize;
}

SSWR::AVIRead::AVIRFileSizePackForm::MyFile::~MyFile()
{
	this->fileName->Release();
}

Bool SSWR::AVIRead::AVIRFileSizePackForm::MyFile::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	sb->Append(this->fileName);
	sb->AppendC(UTF8STRC(" size="));
	sb->AppendU64(this->fileSize);
	return true;
}

NotNullPtr<Text::String> SSWR::AVIRead::AVIRFileSizePackForm::MyFile::GetName() const
{
	return this->fileName;
}

UInt64 SSWR::AVIRead::AVIRFileSizePackForm::MyFile::GetSize() const
{
	return this->fileSize;
}

OSInt SSWR::AVIRead::AVIRFileSizePackForm::MyFile::CompareTo(Data::IComparable *obj) const
{
	UInt64 size = ((MyFile*)obj)->fileSize;
	if (this->fileSize > size)
		return -1;
	else if (this->fileSize < size)
		return 1;
	else
		return 0;
}

void __stdcall SSWR::AVIRead::AVIRFileSizePackForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileSizePackForm *me = (SSWR::AVIRead::AVIRFileSizePackForm*)userObj;
	me->GenList();
}

void __stdcall SSWR::AVIRead::AVIRFileSizePackForm::OnMoveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileSizePackForm *me = (SSWR::AVIRead::AVIRFileSizePackForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char *sptr2;
	UTF8Char *sptr2End;
	if (me->filePath)
	{
		sptr = Text::StrConcat(sbuff, me->filePath);
		sptr2 = me->txtDirName->GetText(sptr);
		if (sptr2 == sptr)
			return;
		if (sptr2 != sptr)
		{
			SSWR::AVIRead::AVIRFileSizePackForm::MyFile *file;
			UOSInt i;
			IO::Path::CreateDirectory(CSTRP(sbuff, sptr2));
			sptr = sptr2;
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}

			sptr2 = Text::StrConcat(sbuff2, me->filePath);
			i = me->packList.GetCount();
			while (i-- > 0)
			{
				file = me->packList.GetItem(i);
				sptrEnd = file->GetName()->ConcatTo(sptr);
				sptr2End = file->GetName()->ConcatTo(sptr2);
				IO::FileUtil::MoveFile(CSTRP(sbuff2, sptr2End), CSTRP(sbuff, sptrEnd), IO::FileUtil::FileExistAction::Fail, 0, 0);
			}

			sptrEnd = sptr = me->txtDirName->GetText(sbuff);
			while (sptr > sbuff)
			{
				if (++*--sptr == 0x3A)
				{
					*sptr = 0x30;
				}
				else
				{
					break;
				}
			}
			me->txtDirName->SetText(CSTRP(sbuff, sptrEnd));
		}
	}
	me->GenList();
}

void SSWR::AVIRead::AVIRFileSizePackForm::ReleaseObjects()
{
	SSWR::AVIRead::AVIRFileSizePackForm::MyFile *file;
	UOSInt i;
	i = this->fileList.GetCount();
	while (i-- > 0)
	{
		file = this->fileList.GetItem(i);
		DEL_CLASS(file);
	}
	this->fileList.Clear();
	this->packList.Clear();
}

void SSWR::AVIRead::AVIRFileSizePackForm::GenList()
{
	UInt64 maxSize;
	UInt64 minSize;
	UInt64 fileSize;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	SSWR::AVIRead::AVIRFileSizePackForm::MyFile *file;
	IO::Path::FindFileSession *sess;
	UOSInt i;
	UOSInt j;
	IO::Path::PathType pt;
	Text::StringBuilderUTF8 sb;
	this->cboMaxSize->GetText(sb);
	maxSize = sb.ToUInt64();
	if (maxSize <= 0)
	{
		this->ui->ShowMsgOK(CSTR("Error in parsing the size"), CSTR("Error"), this);
		this->cboMaxSize->Focus();
		return;
	}
    minSize = maxSize * 999 / 1000;

	sb.ClearStr();
	this->txtDir->GetText(sb);
	if (sb.GetLength() == 0)
	{
		this->ui->ShowMsgOK(CSTR("Please input directory"), CSTR("Error"), this);
		this->txtDir->Focus();
		return;
	}
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		this->ui->ShowMsgOK(CSTR("Please input directory"), CSTR("Error"), this);
		this->txtDir->Focus();
		return;
	}
	sptr = Text::StrConcatC(sbuff, sb.ToString(), sb.GetLength());
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
		*sptr = 0;
	}
	this->ReleaseObjects();
	this->lbFileDir->ClearItems();
	this->lbFilePack->ClearItems();
	SDEL_TEXT(this->filePath);

	UInt64 totalFileSize = 0;
	this->filePath = Text::StrCopyNew(sbuff).Ptr();
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				NEW_CLASS(file, MyFile(CSTRP(sptr, sptr2), fileSize));
				this->fileList.Add(file);
				totalFileSize += fileSize;
			}
		}
		IO::Path::FindFileClose(sess);

		UOSInt arrSize;
		Data::IComparable **arr = (Data::IComparable**)this->fileList.GetPtr(arrSize);
		ArtificialQuickSort_SortCmpO(arr, 0, (OSInt)arrSize - 1);
		i = 0;
		while (i < arrSize)
		{
			file = this->fileList.GetItem(i);
			sb.ClearStr();
			file->ToString(sb);
			this->lbFileDir->AddItem(sb.ToCString(), file);
			i++;
		}
	}

	if (totalFileSize < minSize)
	{
		return;
	}
	if (totalFileSize <= maxSize)
	{
		this->lbFilePack->ClearItems();
		this->packList.AddAll(this->fileList);
		i = 0;
		j = this->fileList.GetCount();
		while (i < j)
		{
			file = this->fileList.GetItem(i);
			sb.ClearStr();
			file->ToString(sb);
			this->lbFilePack->AddItem(sb.ToCString(), file);
			i++;
		}
		sb.ClearStr();
		sb.AppendU64(totalFileSize);
		this->txtTotalSize->SetText(sb.ToCString());
		return;
	}

	UInt64 dirSize;
	this->packList.Clear();
	dirSize = NewCalc(&this->fileList, &this->packList, maxSize, minSize);

	if (this->packList.GetCount() > 0)
	{
		this->lbFilePack->ClearItems();
		i = 0;
		j = this->packList.GetCount();
		while (i < j)
		{
			file = this->packList.GetItem(i);
			sb.ClearStr();
			file->ToString(sb);
			this->lbFilePack->AddItem(sb.ToCString(), file);
			i++;
		}
		sb.ClearStr();
		sb.AppendU64(dirSize);
		this->txtTotalSize->SetText(sb.ToCString());
	}
	else
	{
		this->lbFilePack->ClearItems();
	}
}

UInt64 SSWR::AVIRead::AVIRFileSizePackForm::NewCalc(Data::ArrayList<SSWR::AVIRead::AVIRFileSizePackForm::MyFile *> *fileList, Data::ArrayList<SSWR::AVIRead::AVIRFileSizePackForm::MyFile *> *packList, UInt64 maxSize, UInt64 minSize)
{
	UInt64 currMaxSize = 0;
	UOSInt leng = fileList->GetCount();
	UInt32 *list = MemAlloc(UInt32, leng);
	UOSInt listCount = 0;
	UOSInt j;
	UInt64 currSize = 0;
	MyFile *currItem;
    list[0] = 0;
    listCount = 1;
	while (listCount >= 1)
	{
		currItem = fileList->GetItem(list[listCount - 1]);
        currSize += currItem->GetSize();
        if (currSize > maxSize)
		{
            currSize -= currItem->GetSize();
            list[listCount - 1] += 1;
            while (list[listCount - 1] >= leng)
			{
                listCount -= 1;
                if (listCount == 0)
				{
                    break;
				}
                currItem = fileList->GetItem(list[listCount - 1]);
                currSize -= currItem->GetSize();
                list[listCount - 1] += 1;
			}
		}
        else
		{
            if (currMaxSize < currSize)
			{
                currMaxSize = currSize;
				packList->Clear();
                j = 0;
                while (j < listCount)
				{
					packList->Add(fileList->GetItem(list[j]));
                    j++;
				}

                if (currMaxSize >= minSize)
				{
					MemFree(list);
                    return currMaxSize;
				}
			}
            if (listCount == leng)
			{
                currItem = fileList->GetItem(list[listCount - 1]);
                currSize -= currItem->GetSize();
                list[listCount - 1] += 1;
                while (list[listCount - 1] >= leng)
				{
                    listCount -= 1;
                    if (listCount == 0)
					{
                        break;
					}
                    currItem = fileList->GetItem(list[listCount - 1]);
                    currSize -= currItem->GetSize();
                    list[listCount - 1] += 1;
				}
			}
            else
			{
                list[listCount] = list[listCount - 1] + 1;
                if (list[listCount] >= leng)
				{
                    while (list[listCount] >= leng)
					{
                        listCount -= 1;
                        if (listCount < 0)
						{
                            break;
						}
                        currItem = fileList->GetItem(list[listCount]);
                        currSize -= currItem->GetSize();
                        list[listCount] += 1;
					}
				}
                listCount += 1;
			}
		}
	}
	MemFree(list);
	return 0;
}

SSWR::AVIRead::AVIRFileSizePackForm::AVIRFileSizePackForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 432, 344, ui)
{
	this->SetText(CSTR("File Size Pack"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->filePath = 0;

	NEW_CLASSNN(this->pnlFile, UI::GUIPanel(ui, *this));
	this->pnlFile->SetRect(0, 0, 424, 104, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDir, UI::GUILabel(ui, this->pnlFile, CSTR("Directory")));
	this->lblDir->SetRect(16, 8, 100, 23, false);
	NEW_CLASS(this->txtDir, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtDir->SetRect(120, 8, 192, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlFile, CSTR("Start")));
	this->btnStart->SetRect(328, 8, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblMaxSize, UI::GUILabel(ui, this->pnlFile, CSTR("Maximum Size")));
	this->lblMaxSize->SetRect(16, 42, 100, 23, false);
	NEW_CLASS(this->cboMaxSize, UI::GUIComboBox(ui, this->pnlFile, true));
	this->cboMaxSize->AddItem(CSTR("4697620480"), 0);
	this->cboMaxSize->AddItem(CSTR("25000000000"), 0);
	this->cboMaxSize->AddItem(CSTR("737126400"), 0);
	this->cboMaxSize->SetText(CSTR("25000000000"));
	this->cboMaxSize->SetRect(120, 42, 104, 23, false);
	NEW_CLASS(this->lblTotalSize, UI::GUILabel(ui, this->pnlFile, CSTR("Total Size")));
	this->lblTotalSize->SetRect(224, 42, 72, 23, false);
	NEW_CLASS(this->txtTotalSize, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtTotalSize->SetReadOnly(true);
	this->txtTotalSize->SetRect(296, 42, 100, 23, false);
	NEW_CLASS(this->lblDirName, UI::GUILabel(ui, this->pnlFile, CSTR("Dir Name")));
	this->lblDirName->SetRect(16, 76, 100, 23, false);
	NEW_CLASS(this->txtDirName, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtDirName->SetRect(120, 76, 104, 23, false);
	NEW_CLASS(this->btnMove, UI::GUIButton(ui, this->pnlFile, CSTR("Move")));
	this->btnMove->SetRect(232, 76, 75, 23, false);
	this->btnMove->HandleButtonClick(OnMoveClicked, this);
	NEW_CLASS(this->lbFilePack, UI::GUIListBox(ui, *this, false));
	this->lbFilePack->SetRect(0, 0, 120, 212, false);
	this->lbFilePack->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspFile, UI::GUIHSplitter(ui, *this, 3, false));
	NEW_CLASS(this->lbFileDir, UI::GUIListBox(ui, *this, false));
	this->lbFileDir->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::AVIRFileSizePackForm::~AVIRFileSizePackForm()
{
	this->ReleaseObjects();
	SDEL_TEXT(this->filePath);
}

void SSWR::AVIRead::AVIRFileSizePackForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
