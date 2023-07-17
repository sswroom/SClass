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
#include "UI/FileDialog.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif
void SSWR::AVIRead::AVIRWellFormatForm::AddFilters(IO::FileSelector *selector)
{
	selector->AddFilter(CSTR("*.json"), CSTR("JSON File"));
	selector->AddFilter(CSTR("*.html"), CSTR("HTML File"));
	selector->AddFilter(CSTR("*.htm"), CSTR("HTML File"));
	selector->AddFilter(CSTR("*.xml"), CSTR("XML File"));
	selector->AddFilter(CSTR("*.gml"), CSTR("GML File"));
	selector->AddFilter(CSTR("*.kml"), CSTR("KML File"));
}

Bool SSWR::AVIRead::AVIRWellFormatForm::ParseFile(Text::CString fileName, Text::StringBuilderUTF8 *output)
{
	Bool succ = false;
	UInt64 fileLen;
	Bool reqSSL = false;
	if (fileName.StartsWith(UTF8STRC("http://")) || (reqSSL = fileName.StartsWith(UTF8STRC("https://"))))
	{
		Net::SSLEngine *ssl = 0;
		if (reqSSL)
		{
			ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), false);
		}
		NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->core->GetSocketFactory(), ssl, fileName, Net::WebUtil::RequestMethod::HTTP_GET, true);
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
		SDEL_CLASS(ssl);
	}
	else if (fileName.EndsWithICase(UTF8STRC(".json")))
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fileLen = fs.GetLength();
		if (fileLen > 0 && fileLen < 1048576)
		{
			Data::ByteBuffer buff((UOSInt)fileLen);
			if (fs.Read(buff) == fileLen)
			{
				succ = Text::JSText::JSONWellFormat(buff.Ptr(), (UOSInt)fileLen, 0, output);
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

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWellFormatForm *me = (SSWR::AVIRead::AVIRWellFormatForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"WellFormat", false);
	AddFilters(&dlg);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->txtFile->SetText(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRWellFormatForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFile)
{
	SSWR::AVIRead::AVIRWellFormatForm *me = (SSWR::AVIRead::AVIRWellFormatForm*)userObj;
	me->txtFile->SetText(files[0]->ToCString());
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
	if (me->ParseFile(sbFile.ToCString(), &sbOutput))
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
	if (me->ParseFile(sbFile.ToCString(), &sbOutput))
	{
		UI::FileDialog dlg(L"SSWR", L"AVIRead", L"WellFormatParse", true);
		AddFilters(&dlg);
		dlg.SetFileName(sbFile.ToCString());
		if (dlg.ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(sbOutput.ToString(), sbOutput.GetLength());
		}
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
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, CSTR("JS Text")));
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
