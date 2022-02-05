#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRXMLWalkForm.h"
#include "Text/XMLReader.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRXMLWalkForm::OnBrowseClick(void *userObj)
{
	SSWR::AVIRead::AVIRXMLWalkForm *me = (SSWR::AVIRead::AVIRXMLWalkForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"XMLWalk", false));
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter((const UTF8Char*)"*.xml", (const UTF8Char*)"XML File");
	dlg->AddFilter((const UTF8Char*)"*.html", (const UTF8Char*)"HTML File");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName()->v);
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRXMLWalkForm::OnFileDrop(void *userObj, const UTF8Char **fileNames, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRXMLWalkForm *me = (SSWR::AVIRead::AVIRXMLWalkForm*)userObj;
	if (nFiles > 0)
	{
		me->LoadFile(fileNames[0]);
	}
}

void SSWR::AVIRead::AVIRXMLWalkForm::LoadFile(const UTF8Char *fileName)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream *fs;
	Text::XMLReader *reader;
	Text::XMLReader::ParseMode mode;
	UOSInt fileNameLen = Text::StrCharCnt(fileName);
	UOSInt i;
	this->lvXML->ClearItems();
	this->txtFile->SetText(fileName);
	i = Text::StrLastIndexOfCharC(fileName, fileNameLen, '.');
	if (Text::StrEqualsICaseC(&fileName[i + 1], fileNameLen - i - 1, UTF8STRC("html")))
	{
		mode = Text::XMLReader::PM_HTML;
	}
	else if (Text::StrEqualsICaseC(&fileName[i + 1], fileNameLen - i - 1, UTF8STRC("htm")))
	{
		mode = Text::XMLReader::PM_HTML;
	}
	else
	{
		mode = Text::XMLReader::PM_XML;
	}
	NEW_CLASS(fs, IO::FileStream({fileName, fileNameLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::XMLReader(this->core->GetEncFactory(), fs, mode));
	while (reader->ReadNext())
	{
		sb.ClearStr();
		reader->GetCurrPath(&sb);
		i = this->lvXML->AddItem(sb.ToString(), 0);
		this->lvXML->SetSubItem(i, 1, Text::XMLNode::NodeTypeGetName(reader->GetNodeType()).v);
		this->lvXML->SetSubItem(i, 2, reader->GetNodeText());
	}
	if (!reader->IsComplete())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in parsing the file, code = "));
		sb.AppendOSInt(reader->GetErrorCode());
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"XML Walk", this);
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
}

SSWR::AVIRead::AVIRXMLWalkForm::AVIRXMLWalkForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"XML Walk");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlFile, UI::GUIPanel(ui, this));
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"XML File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtFile->SetRect(104, 4, 500, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"Browse"));
	this->btnBrowse->SetRect(604, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClick, this);
	NEW_CLASS(this->lvXML, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvXML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvXML->SetShowGrid(true);
	this->lvXML->SetFullRowSelect(true);
	this->lvXML->AddColumn((const UTF8Char*)"Path", 300);
	this->lvXML->AddColumn((const UTF8Char*)"Type", 100);
	this->lvXML->AddColumn((const UTF8Char*)"Value", 200);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRXMLWalkForm::~AVIRXMLWalkForm()
{
}

void SSWR::AVIRead::AVIRXMLWalkForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
