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

Bool SSWR::AVIRead::AVIRWellFormatForm::ParseFile(const UTF8Char *fileName, UOSInt fileNameLen, Text::StringBuilderUTF8 *output)
{
	Bool succ = false;
	IO::FileStream *fs;
	UInt64 fileLen;
	UInt8 *buff;

	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".json")))
	{
		NEW_CLASS(fs, IO::FileStream({fileName, fileNameLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
	else if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".html")) || Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".htm")))
	{
		NEW_CLASS(fs, IO::FileStream({fileName, fileNameLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fileLen = fs->GetLength();
		if (fileLen > 0 && fileLen < 1048576)
		{
			succ = Text::HTMLUtil::HTMLWellFormat(this->core->GetEncFactory(), fs, 0, output);
		}
		DEL_CLASS(fs);
	}
	else if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".xml")))
	{
		NEW_CLASS(fs, IO::FileStream({fileName, fileNameLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fileLen = fs->GetLength();
		if (fileLen > 0 && fileLen < 1048576)
		{
			succ = Text::XMLReader::XMLWellFormat(this->core->GetEncFactory(), fs, 0, output);
		}
		DEL_CLASS(fs);
	}
	else if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".js")))
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
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile->SetText(dlg->GetFileName()->ToCString());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFile)
{
	SSWR::AVIRead::AVIRWellFormatForm *me = (SSWR::AVIRead::AVIRWellFormatForm*)userObj;
	me->txtFile->SetText({files[0], Text::StrCharCnt(files[0])});
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
	if (me->ParseFile(sbFile.ToString(), sbFile.GetLength(), &sbOutput))
	{
		me->txtOutput->SetText(sbOutput.ToCString());
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
	if (me->ParseFile(sbFile.ToString(), sbFile.GetLength(), &sbOutput))
	{
		UI::FileDialog *dlg;
		NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"WellFormatParse", true));
		AddFilters(dlg);
		dlg->SetFileName(sbFile.ToString());
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			fs->Write(sbOutput.ToString(), sbOutput.GetLength());
			DEL_CLASS(fs);
		}
		DEL_CLASS(dlg);
	}
}


SSWR::AVIRead::AVIRWellFormatForm::AVIRWellFormatForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Well Format"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlFile, UI::GUIPanel(ui, this));
	this->pnlFile->SetRect(0, 0, 100, 55, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"JS Text"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtFile->SetRect(104, 4, 600, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this->pnlFile, CSTR("Browse")));
	this->btnBrowse->SetRect(704, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->btnParseToText, UI::GUIButton(ui, this->pnlFile, CSTR("To Text")));
	this->btnParseToText->SetRect(104, 28, 75, 23, false);
	this->btnParseToText->HandleButtonClick(OnParseToTextClicked, this);
	NEW_CLASS(this->btnParseToFile, UI::GUIButton(ui, this->pnlFile, CSTR("To File")));
	this->btnParseToFile->SetRect(184, 28, 75, 23, false);
	this->btnParseToFile->HandleButtonClick(OnParseToFileClicked, this);
	NEW_CLASS(this->txtOutput, UI::GUITextBox(ui, this, CSTR(""), true));
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
