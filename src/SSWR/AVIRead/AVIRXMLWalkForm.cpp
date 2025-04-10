#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRXMLWalkForm.h"
#include "Text/XMLReader.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRXMLWalkForm::OnBrowseClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRXMLWalkForm> me = userObj.GetNN<SSWR::AVIRead::AVIRXMLWalkForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"XMLWalk", false);
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter(CSTR("*.xml"), CSTR("XML File"));
	dlg->AddFilter(CSTR("*.html"), CSTR("HTML File"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRXMLWalkForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames)
{
	NN<SSWR::AVIRead::AVIRXMLWalkForm> me = userObj.GetNN<SSWR::AVIRead::AVIRXMLWalkForm>();
	if (fileNames.GetCount() > 0)
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
		this->lvXML->SetSubItem(i, 3, Text::String::OrEmpty(reader.GetNamespace()));
	}
	if (!reader.IsComplete())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in parsing the file, code = "));
		sb.AppendUOSInt(reader.GetErrorCode());
		this->ui->ShowMsgOK(sb.ToCString(), CSTR("XML Walk"), this);
	}
}

SSWR::AVIRead::AVIRXMLWalkForm::AVIRXMLWalkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("XML Walk"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("XML File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtFile->SetRect(104, 4, 500, 23, false);
	this->txtFile->SetReadOnly(true);
	this->btnBrowse = ui->NewButton(this->pnlFile, CSTR("Browse"));
	this->btnBrowse->SetRect(604, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClick, this);
	this->lvXML = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvXML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvXML->SetShowGrid(true);
	this->lvXML->SetFullRowSelect(true);
	this->lvXML->AddColumn(CSTR("Path"), 300);
	this->lvXML->AddColumn(CSTR("Type"), 100);
	this->lvXML->AddColumn(CSTR("Value"), 200);
	this->lvXML->AddColumn(CSTR("Namespace"), 200);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRXMLWalkForm::~AVIRXMLWalkForm()
{
}

void SSWR::AVIRead::AVIRXMLWalkForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
