#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRFileExForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnSrcChanged(void *userObj)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UInt64 fileSize;
	IO::Path::PathType pt;
	me->txtSrc->GetText(&sb);
	pt = IO::Path::GetPathType(sb.ToString());
	if (pt == IO::Path::PathType::File)
	{
		fileSize = IO::Path::GetFileSize(sb.ToString());
		sb.ClearStr();
		sb.AppendU64(fileSize);
		me->txtFileSize->SetText(sb.ToString());
		me->txtEndOfst->SetText(sb.ToString());
	}
	else
	{
		me->txtFileSize->SetText((const UTF8Char*)"Unknown");
	}
}

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnSrcClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	UI::FileDialog *ofd;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(ofd, UI::FileDialog(L"SSWR", L"AVIRead", L"FileExSrc", false));
	me->txtSrc->GetText(&sb);
	ofd->SetFileName(sb.ToString());
	if (ofd->ShowDialog(me->GetHandle()))
	{
		me->txtSrc->SetText(ofd->GetFileName());
	}
}

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnDestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	UI::FileDialog *ofd;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(ofd, UI::FileDialog(L"SSWR", L"AVIRead", L"FileExDest", true));
	me->txtDest->GetText(&sb);
	ofd->SetFileName(sb.ToString());
	if (ofd->ShowDialog(me->GetHandle()))
	{
		me->txtDest->SetText(ofd->GetFileName());
	}
}

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UInt64 startOfst;
	UInt64 endOfst;
	UInt64 fileSize;
	UInt64 sizeLeft;
	UOSInt thisSize;
	UOSInt readSize;
	UInt8 *buff;
	IO::FileStream *srcFS;
	IO::FileStream *destFS;
	me->txtStartOfst->GetText(&sb);
	if (!sb.ToUInt64(&startOfst))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Start Offset is not valid", (const UTF8Char*)"Error", me);
		return;
	}
	sb.ClearStr();
	me->txtEndOfst->GetText(&sb);
	if (!sb.ToUInt64(&endOfst))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"End Offset is not valid", (const UTF8Char*)"Error", me);
		return;
	}
	if (startOfst >= endOfst)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Start Offset is out of range", (const UTF8Char*)"Error", me);
		return;
	}
	sb.ClearStr();
	me->txtSrc->GetText(&sb);
	NEW_CLASS(srcFS, IO::FileStream(sb.ToString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	if (srcFS->IsError())
	{
		DEL_CLASS(srcFS);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening source file", (const UTF8Char*)"Error", me);
		return;
	}
	fileSize = srcFS->GetLength();
	if (endOfst > fileSize)
	{
		DEL_CLASS(srcFS);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"End Offset is out of range", (const UTF8Char*)"Error", me);
		return;
	}
	sb.ClearStr();
	me->txtDest->GetText(&sb);
	NEW_CLASS(destFS, IO::FileStream(sb.ToString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (destFS->IsError())
	{
		DEL_CLASS(destFS);
		DEL_CLASS(srcFS);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening dest file", (const UTF8Char*)"Error", me);
		return;
	}
	srcFS->SeekFromBeginning(startOfst);
	sizeLeft = endOfst - startOfst;
	buff = MemAlloc(UInt8, 1048576);
	destFS->SetLength(sizeLeft);
	while (sizeLeft > 0)
	{
		thisSize = 1048576;
		if (thisSize > sizeLeft)
			thisSize = (UOSInt)sizeLeft;
		readSize = srcFS->Read(buff, thisSize);
		if (readSize == 0)
			break;
		destFS->Write(buff, readSize);
		sizeLeft -= readSize;
	}
	MemFree(buff);

	DEL_CLASS(destFS);
	DEL_CLASS(srcFS);
}

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt fileCnt)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	me->txtSrc->SetText(files[0]);
}

SSWR::AVIRead::AVIRFileExForm::AVIRFileExForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"File Extractor");
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblSrc, UI::GUILabel(ui, this, (const UTF8Char*)"Source File"));
	this->lblSrc->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSrc, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtSrc->SetRect(104, 4, 600, 23, false);
	this->txtSrc->HandleTextChanged(OnSrcChanged, this);
	NEW_CLASS(this->btnSrc, UI::GUIButton(ui, this, (const UTF8Char*)"B&rowse"));
	this->btnSrc->SetRect(704, 4, 75, 23, false);
	this->btnSrc->HandleButtonClick(OnSrcClicked, this);
	NEW_CLASS(this->lblDest, UI::GUILabel(ui, this, (const UTF8Char*)"Dest File"));
	this->lblDest->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDest, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtDest->SetRect(104, 28, 600, 23, false);
	NEW_CLASS(this->btnDest, UI::GUIButton(ui, this, (const UTF8Char*)"&Browse"));
	this->btnDest->SetRect(704, 28, 75, 23, false);
	this->btnDest->HandleButtonClick(OnDestClicked, this);
	NEW_CLASS(this->lblFileSize, UI::GUILabel(ui, this, (const UTF8Char*)"File Size"));
	this->lblFileSize->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtFileSize, UI::GUITextBox(ui, this, (const UTF8Char*)"Unknown"));
	this->txtFileSize->SetRect(104, 52, 160, 23, false);
	this->txtFileSize->SetReadOnly(true);
	NEW_CLASS(this->lblStartOfst, UI::GUILabel(ui, this, (const UTF8Char*)"Start Offset"));
	this->lblStartOfst->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtStartOfst, UI::GUITextBox(ui, this, (const UTF8Char*)"0"));
	this->txtStartOfst->SetRect(104, 76, 160, 23, false);
	NEW_CLASS(this->lblEndOfst, UI::GUILabel(ui, this, (const UTF8Char*)"End Offset"));
	this->lblEndOfst->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtEndOfst, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtEndOfst->SetRect(104, 100, 160, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, (const UTF8Char*)"&Start"));
	this->btnStart->SetRect(304, 76, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRFileExForm::~AVIRFileExForm()
{
}

void SSWR::AVIRead::AVIRFileExForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
