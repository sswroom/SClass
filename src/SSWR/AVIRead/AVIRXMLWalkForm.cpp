#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRXMLWalkForm.h"
#include "Text/XMLReader.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRXMLWalkForm::OnBrowseClick(void *userObj)
{
	SSWR::AVIRead::AVIRXMLWalkForm *me = (SSWR::AVIRead::AVIRXMLWalkForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"XMLWalk", false);
	dlg.SetAllowMultiSel(false);
	dlg.AddFilter(CSTR("*.xml"), CSTR("XML File"));
	dlg.AddFilter(CSTR("*.html"), CSTR("HTML File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRXMLWalkForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *fileNames, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRXMLWalkForm *me = (SSWR::AVIRead::AVIRXMLWalkForm*)userObj;
	if (nFiles > 0)
	{
		me->LoadFile(fileNames[0]->ToCString());
	}
}

void SSWR::AVIRead::AVIRXMLWalkForm::LoadFile(Text::CStringNN fileName)
{
	Text::StringBuilderUTF8 sb;
	Text::XMLReader::ParseMode mode;
	UOSInt i;
	this->lvXML->ClearItems();
	this->txtFile->SetText(fileName);
	i = fileName.LastIndexOf('.');
	if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileName.leng - i - 1, UTF8STRC("html")))
	{
		mode = Text::XMLReader::PM_HTML;
	}
	else if (Text::StrEqualsICaseC(&fileName.v[i + 1], fileName.leng - i - 1, UTF8STRC("htm")))
	{
		mode = Text::XMLReader::PM_HTML;
	}
	else
	{
		mode = Text::XMLReader::PM_XML;
	}
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::XMLReader reader(this->core->GetEncFactory(), fs, mode);
	while (reader.ReadNext())
	{
		sb.ClearStr();
		reader.GetCurrPath(sb);
		i = this->lvXML->AddItem(sb.ToCString(), 0);
		this->lvXML->SetSubItem(i, 1, Text::XMLNode::NodeTypeGetName(reader.GetNodeType()));
		this->lvXML->SetSubItem(i, 2, Text::String::OrEmpty(reader.GetNodeText()));
	}
	if (!reader.IsComplete())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in parsing the file, code = "));
		sb.AppendUOSInt(reader.GetErrorCode());
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("XML Walk"), this);
	}
}

SSWR::AVIRead::AVIRXMLWalkForm::AVIRXMLWalkForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("XML Walk"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlFile, UI::GUIPanel(ui, *this));
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, CSTR("XML File")));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtFile->SetRect(104, 4, 500, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this->pnlFile, CSTR("Browse")));
	this->btnBrowse->SetRect(604, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClick, this);
	NEW_CLASS(this->lvXML, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvXML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvXML->SetShowGrid(true);
	this->lvXML->SetFullRowSelect(true);
	this->lvXML->AddColumn(CSTR("Path"), 300);
	this->lvXML->AddColumn(CSTR("Type"), 100);
	this->lvXML->AddColumn(CSTR("Value"), 200);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRXMLWalkForm::~AVIRXMLWalkForm()
{
}

void SSWR::AVIRead::AVIRXMLWalkForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
