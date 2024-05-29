#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRWellFormatForm.h"
#include "Text/HTMLUtil.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"
#include "UI/GUIFileDialog.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif
void SSWR::AVIRead::AVIRWellFormatForm::AddFilters(NN<IO::FileSelector> selector)
{
	selector->AddFilter(CSTR("*.json"), CSTR("JSON File"));
	selector->AddFilter(CSTR("*.geojson"), CSTR("GeoJSON File"));
	selector->AddFilter(CSTR("*.html"), CSTR("HTML File"));
	selector->AddFilter(CSTR("*.htm"), CSTR("HTML File"));
	selector->AddFilter(CSTR("*.xml"), CSTR("XML File"));
	selector->AddFilter(CSTR("*.gml"), CSTR("GML File"));
	selector->AddFilter(CSTR("*.kml"), CSTR("KML File"));
}

Bool SSWR::AVIRead::AVIRWellFormatForm::ParseFile(Text::CStringNN fileName, NN<Text::StringBuilderUTF8> output)
{
	Bool succ = false;
	UInt64 fileLen;
	Bool reqSSL = false;
	if (fileName.StartsWith(UTF8STRC("http://")) || (reqSSL = fileName.StartsWith(UTF8STRC("https://"))))
	{
		Optional<Net::SSLEngine> ssl = 0;
		if (reqSSL)
		{
			ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), false);
		}
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->core->GetSocketFactory(), ssl, fileName, Net::WebUtil::RequestMethod::HTTP_GET, true);
		if (!cli->IsError() && cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			Text::CString contType = cli->GetContentType();
			if (contType.leng == 0)
			{

			}
			else if (contType.Equals(UTF8STRC("application/xml")))
			{
				succ = Text::XMLReader::XMLWellFormat(this->core->GetEncFactory(), cli, 0, output);
			}
//				else if (contType.Equals(UTF8STRC("text/")))
			else
			{
				printf("AVIRWellFormat: Unknown content-type: %s\r\n", contType.v);
			}
		}
		cli.Delete();
		ssl.Delete();
	}
	else if (fileName.EndsWithICase(UTF8STRC(".json")) || fileName.EndsWithICase(UTF8STRC(".geojson")))
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fileLen = fs.GetLength();
		if (fileLen > 0 && fileLen < 10485760)
		{
			Data::ByteBuffer buff((UOSInt)fileLen);
			if (fs.Read(buff) == fileLen)
			{
				succ = Text::JSText::JSONWellFormat(buff.Arr(), (UOSInt)fileLen, 0, output);
			}
		}
	}
	else if (fileName.EndsWithICase(UTF8STRC(".html")) || fileName.EndsWith(UTF8STRC(".htm")))
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fileLen = fs.GetLength();
		if (fileLen > 0 && fileLen < 1048576)
		{
			succ = Text::HTMLUtil::HTMLWellFormat(this->core->GetEncFactory(), fs, 0, output);
		}
	}
	else if (fileName.EndsWithICase(UTF8STRC(".xml")) || fileName.EndsWithICase(UTF8STRC(".gml")) || fileName.EndsWithICase(UTF8STRC(".kml")))
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fileLen = fs.GetLength();
		if (fileLen > 0 && fileLen < 10485760)
		{
			succ = Text::XMLReader::XMLWellFormat(this->core->GetEncFactory(), fs, 0, output);
		}
	}
	else if (fileName.EndsWithICase(UTF8STRC(".js")))
	{

	}

	return succ;
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnBrowseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWellFormatForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWellFormatForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"WellFormat", false);
	AddFilters(dlg);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRWellFormatForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWellFormatForm>();
	me->txtFile->SetText(files[0]->ToCString());
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnParseToTextClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWellFormatForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWellFormatForm>();
	Text::StringBuilderUTF8 sbFile;
	Text::StringBuilderUTF8 sbOutput;
	me->txtFile->GetText(sbFile);
	if (sbFile.GetLength() <= 0)
	{
		return;
	}
	if (me->ParseFile(sbFile.ToCString(), sbOutput))
	{
		me->txtOutput->SetText(sbOutput.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnParseToFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWellFormatForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWellFormatForm>();
	Text::StringBuilderUTF8 sbFile;
	Text::StringBuilderUTF8 sbOutput;
	me->txtFile->GetText(sbFile);
	if (sbFile.GetLength() <= 0)
	{
		return;
	}
	if (me->ParseFile(sbFile.ToCString(), sbOutput))
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"WellFormatParse", true);
		AddFilters(dlg);
		dlg->SetFileName(sbFile.ToCString());
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(sbOutput.ToString(), sbOutput.GetLength());
		}
		dlg.Delete();
	}
}


SSWR::AVIRead::AVIRWellFormatForm::AVIRWellFormatForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Well Format"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 55, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("JS Text"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtFile->SetRect(104, 4, 600, 23, false);
	this->btnBrowse = ui->NewButton(this->pnlFile, CSTR("Browse"));
	this->btnBrowse->SetRect(704, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->btnParseToText = ui->NewButton(this->pnlFile, CSTR("To Text"));
	this->btnParseToText->SetRect(104, 28, 75, 23, false);
	this->btnParseToText->HandleButtonClick(OnParseToTextClicked, this);
	this->btnParseToFile = ui->NewButton(this->pnlFile, CSTR("To File"));
	this->btnParseToFile->SetRect(184, 28, 75, 23, false);
	this->btnParseToFile->HandleButtonClick(OnParseToFileClicked, this);
	this->txtOutput = ui->NewTextBox(*this, CSTR(""), true);
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
