#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRWellFormatForm.h"
#include "Text/HTMLUtil.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"
#include "UI/FileDialog.h"

void SSWR::AVIRead::AVIRWellFormatForm::AddFilters(IO::IFileSelector *selector)
{
	selector->AddFilter((const UTF8Char*)"*.json", (const UTF8Char*)"JSON File");
	selector->AddFilter((const UTF8Char*)"*.html", (const UTF8Char*)"HTML File");
	selector->AddFilter((const UTF8Char*)"*.htm", (const UTF8Char*)"HTML File");
	selector->AddFilter((const UTF8Char*)"*.xml", (const UTF8Char*)"XML File");
}

Bool SSWR::AVIRead::AVIRWellFormatForm::ParseFile(const UTF8Char *fileName, Text::StringBuilderUTF *output)
{
	Bool succ = false;
	IO::FileStream *fs;
	UInt64 fileLen;
	UInt8 *buff;

	if (Text::StrEndsWithICase(fileName, (const UTF8Char*)".json"))
	{
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		fileLen = fs->GetLength();
		if (fileLen > 0 && fileLen < 1048576)
		{
			buff = MemAlloc(UInt8, (UOSInt)fileLen);
			if (fs->Read(buff, (UOSInt)fileLen) == fileLen)
			{
				succ = Text::JSText::JSONWellFormat(buff, (UOSInt)fileLen, 0, output);
			}
			MemFree(buff);
		}
		DEL_CLASS(fs);
	}
	else if (Text::StrEndsWithICase(fileName, (const UTF8Char*)".html") || Text::StrEndsWithICase(fileName, (const UTF8Char*)".htm"))
	{
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		fileLen = fs->GetLength();
		if (fileLen > 0 && fileLen < 1048576)
		{
			succ = Text::HTMLUtil::HTMLWellFormat(this->core->GetEncFactory(), fs, 0, output);
		}
		DEL_CLASS(fs);
	}
	else if (Text::StrEndsWithICase(fileName, (const UTF8Char*)".xml"))
	{
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		fileLen = fs->GetLength();
		if (fileLen > 0 && fileLen < 1048576)
		{
			succ = Text::XMLReader::XMLWellFormat(this->core->GetEncFactory(), fs, 0, output);
		}
		DEL_CLASS(fs);
	}
	else if (Text::StrEndsWithICase(fileName, (const UTF8Char*)".js"))
	{

	}

	return succ;
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWellFormatForm *me = (SSWR::AVIRead::AVIRWellFormatForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"WellFormat", false));
	AddFilters(dlg);
	if (dlg->ShowDialog(me))
	{
		me->txtFile->SetText(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFile)
{
	SSWR::AVIRead::AVIRWellFormatForm *me = (SSWR::AVIRead::AVIRWellFormatForm*)userObj;
	me->txtFile->SetText(files[0]);
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnParseToTextClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWellFormatForm *me = (SSWR::AVIRead::AVIRWellFormatForm*)userObj;
	Text::StringBuilderUTF8 sbFile;
	Text::StringBuilderUTF8 sbOutput;
	me->txtFile->GetText(&sbFile);
	if (sbFile.GetLength() <= 0)
	{
		return;
	}
	if (me->ParseFile(sbFile.ToString(), &sbOutput))
	{
		me->txtOutput->SetText(sbOutput.ToString());
	}
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnParseToFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWellFormatForm *me = (SSWR::AVIRead::AVIRWellFormatForm*)userObj;
	Text::StringBuilderUTF8 sbFile;
	Text::StringBuilderUTF8 sbOutput;
	me->txtFile->GetText(&sbFile);
	if (sbFile.GetLength() <= 0)
	{
		return;
	}
	if (me->ParseFile(sbFile.ToString(), &sbOutput))
	{
		UI::FileDialog *dlg;
		NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"WellFormatParse", true));
		AddFilters(dlg);
		dlg->SetFileName(sbFile.ToString());
		if (dlg->ShowDialog(me))
		{
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			fs->Write(sbOutput.ToString(), sbOutput.GetLength());
			DEL_CLASS(fs);
		}
		DEL_CLASS(dlg);
	}
}


SSWR::AVIRead::AVIRWellFormatForm::AVIRWellFormatForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Well Format");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlFile, UI::GUIPanel(ui, this));
	this->pnlFile->SetRect(0, 0, 100, 55, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"JS Text"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, (const UTF8Char*)""));
	this->txtFile->SetRect(104, 4, 600, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"Browse"));
	this->btnBrowse->SetRect(704, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->btnParseToText, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"To Text"));
	this->btnParseToText->SetRect(104, 28, 75, 23, false);
	this->btnParseToText->HandleButtonClick(OnParseToTextClicked, this);
	NEW_CLASS(this->btnParseToFile, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"To File"));
	this->btnParseToFile->SetRect(184, 28, 75, 23, false);
	this->btnParseToFile->HandleButtonClick(OnParseToFileClicked, this);
	NEW_CLASS(this->txtOutput, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtOutput->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtOutput->SetReadOnly(true);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRWellFormatForm::~AVIRWellFormatForm()
{
}

void SSWR::AVIRead::AVIRWellFormatForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
