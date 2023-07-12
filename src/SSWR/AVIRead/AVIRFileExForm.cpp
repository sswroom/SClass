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
	pt = IO::Path::GetPathType(sb.ToCString());
	if (pt == IO::Path::PathType::File)
	{
		fileSize = IO::Path::GetFileSize(sb.ToString());
		sb.ClearStr();
		sb.AppendU64(fileSize);
		me->txtFileSize->SetText(sb.ToCString());
		me->txtEndOfst->SetText(sb.ToCString());
	}
	else
	{
		me->txtFileSize->SetText(CSTR("Unknown"));
	}
}

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnSrcClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::FileDialog ofd(L"SSWR", L"AVIRead", L"FileExSrc", false);
	me->txtSrc->GetText(&sb);
	ofd.SetFileName(sb.ToCString());
	if (ofd.ShowDialog(me->GetHandle()))
	{
		me->txtSrc->SetText(ofd.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnDestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::FileDialog ofd(L"SSWR", L"AVIRead", L"FileExDest", true);
	me->txtDest->GetText(&sb);
	ofd.SetFileName(sb.ToCString());
	if (ofd.ShowDialog(me->GetHandle()))
	{
		me->txtDest->SetText(ofd.GetFileName()->ToCString());
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
		UI::MessageDialog::ShowDialog(CSTR("Start Offset is not valid"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtEndOfst->GetText(&sb);
	if (!sb.ToUInt64(&endOfst))
	{
		UI::MessageDialog::ShowDialog(CSTR("End Offset is not valid"), CSTR("Error"), me);
		return;
	}
	if (startOfst >= endOfst)
	{
		UI::MessageDialog::ShowDialog(CSTR("Start Offset is out of range"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtSrc->GetText(&sb);
	NEW_CLASS(srcFS, IO::FileStream(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	if (srcFS->IsError())
	{
		DEL_CLASS(srcFS);
		UI::MessageDialog::ShowDialog(CSTR("Error in opening source file"), CSTR("Error"), me);
		return;
	}
	fileSize = srcFS->GetLength();
	if (endOfst > fileSize)
	{
		DEL_CLASS(srcFS);
		UI::MessageDialog::ShowDialog(CSTR("End Offset is out of range"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtDest->GetText(&sb);
	NEW_CLASS(destFS, IO::FileStream(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (destFS->IsError())
	{
		DEL_CLASS(destFS);
		DEL_CLASS(srcFS);
		UI::MessageDialog::ShowDialog(CSTR("Error in opening dest file"), CSTR("Error"), me);
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

void __stdcall SSWR::AVIRead::AVIRFileExForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt fileCnt)
{
	SSWR::AVIRead::AVIRFileExForm *me = (SSWR::AVIRead::AVIRFileExForm*)userObj;
	me->txtSrc->SetText(files[0]->ToCString());
}

SSWR::AVIRead::AVIRFileExForm::AVIRFileExForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("File Extractor"));
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblSrc, UI::GUILabel(ui, this, CSTR("Source File")));
	this->lblSrc->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSrc, UI::GUITextBox(ui, this, CSTR("")));
	this->txtSrc->SetRect(104, 4, 600, 23, false);
	this->txtSrc->HandleTextChanged(OnSrcChanged, this);
	NEW_CLASS(this->btnSrc, UI::GUIButton(ui, this, CSTR("B&rowse")));
	this->btnSrc->SetRect(704, 4, 75, 23, false);
	this->btnSrc->HandleButtonClick(OnSrcClicked, this);
	NEW_CLASS(this->lblDest, UI::GUILabel(ui, this, CSTR("Dest File")));
	this->lblDest->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDest, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDest->SetRect(104, 28, 600, 23, false);
	NEW_CLASS(this->btnDest, UI::GUIButton(ui, this, CSTR("&Browse")));
	this->btnDest->SetRect(704, 28, 75, 23, false);
	this->btnDest->HandleButtonClick(OnDestClicked, this);
	NEW_CLASS(this->lblFileSize, UI::GUILabel(ui, this, CSTR("File Size")));
	this->lblFileSize->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtFileSize, UI::GUITextBox(ui, this, CSTR("Unknown")));
	this->txtFileSize->SetRect(104, 52, 160, 23, false);
	this->txtFileSize->SetReadOnly(true);
	NEW_CLASS(this->lblStartOfst, UI::GUILabel(ui, this, CSTR("Start Offset")));
	this->lblStartOfst->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtStartOfst, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtStartOfst->SetRect(104, 76, 160, 23, false);
	NEW_CLASS(this->lblEndOfst, UI::GUILabel(ui, this, CSTR("End Offset")));
	this->lblEndOfst->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtEndOfst, UI::GUITextBox(ui, this, CSTR("")));
	this->txtEndOfst->SetRect(104, 100, 160, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("&Start")));
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
