#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRLogExtractForm.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRLogExtractForm::OnSFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogExtractForm *me = (SSWR::AVIRead::AVIRLogExtractForm *)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"LogExtractSFile", false));
	dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
	dlg->SetAllowMultiSel(false);
	me->txtSFile->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtSFile->SetText(dlg->GetFileName()->ToCString());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRLogExtractForm::OnOFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogExtractForm *me = (SSWR::AVIRead::AVIRLogExtractForm *)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"LogExtractOFile", true));
	dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
	dlg->AddFilter(CSTR("*.txt"), CSTR("Text File"));
	me->txtOFile->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtOFile->SetText(dlg->GetFileName()->ToCString());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRLogExtractForm::OnExtractClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogExtractForm *me = (SSWR::AVIRead::AVIRLogExtractForm *)userObj;
	UTF8Char sbuff[16];
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sbSuffix;
	me->txtSFile->GetText(&sb1);
	me->txtPrefix->GetText(&sb2);
	me->txtOFile->GetText(&sb3);
	me->txtSuffix->GetText(&sbSuffix);
	if (IO::Path::GetPathType(sb1.ToString(), sb1.GetLength()) != IO::Path::PathType::File)
	{
		UI::MessageDialog::ShowDialog(CSTR("Source File not found"), CSTR("Error"), me);
	}
	else if (sb2.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter Prefix"), CSTR("Error"), me);
	}
	else if (sb3.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter Output"), CSTR("Error"), me);
	}
	else
	{
		UOSInt typ = me->cboType->GetSelectedIndex();
		UOSInt i;
		Bool hasData;
		IO::FileStream *fs1;
		IO::FileStream *fs2;
		Text::UTF8Reader *reader;
		Text::UTF8Writer *writer;
		NEW_CLASS(fs1, IO::FileStream(sb1.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs1));
		NEW_CLASS(fs2, IO::FileStream(sb3.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs2->IsError())
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in creating Output file"), CSTR("Error"), me);
		}
		else
		{
			NEW_CLASS(writer, Text::UTF8Writer(fs2));
			writer->WriteSignature();

			sb1.ClearStr();
			hasData = reader->ReadLine(&sb1, 1024);
			while (hasData)
			{
				reader->GetLastLineBreak(sbuff);
				while (hasData && sbuff[0] == 0)
				{
					hasData = reader->ReadLine(&sb1, 1024);
					reader->GetLastLineBreak(sbuff);
				}

				sb1.AppendC(sbSuffix.ToString(), sbSuffix.GetLength());
				i = sb1.IndexOf('\t');
				if (i != INVALID_INDEX)
				{
					if (typ == 0)
					{
						if (Text::StrStartsWith(sb1.ToString() + i + 1, sb2.ToString()))
						{
							writer->WriteLineC(sb1.ToString(), sb1.GetLength());
						}
					}
					else if (typ == 1)
					{
						if (Text::StrStartsWith(sb1.ToString() + i + 1, sb2.ToString()))
						{
							writer->WriteLine(sb1.ToString() + i + 1 + sb2.GetLength());
						}
					}
				}

				sb1.ClearStr();
				hasData = reader->ReadLine(&sb1, 1024);
			}

			UI::MessageDialog::ShowDialog(CSTR("Complete log extract"), CSTR("Success"), me);
			DEL_CLASS(writer);
		}
		DEL_CLASS(fs2);
		DEL_CLASS(reader);
		DEL_CLASS(fs1);
	}
}

SSWR::AVIRead::AVIRLogExtractForm::AVIRLogExtractForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 184, ui)
{
	this->SetText(CSTR("Log Extract"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblSFile, UI::GUILabel(ui, this, CSTR("Source File")));
	this->lblSFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSFile, UI::GUITextBox(ui, this, CSTR("")));
	this->txtSFile->SetRect(104, 4, 560, 23, false);
	NEW_CLASS(this->btnSFile, UI::GUIButton(ui, this, CSTR("Browse")));
	this->btnSFile->SetRect(660, 4, 75, 23, false);
	this->btnSFile->HandleButtonClick(OnSFileClicked, this);
	NEW_CLASS(this->lblPrefix, UI::GUILabel(ui, this, CSTR("Prefix")));
	this->lblPrefix->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPrefix, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPrefix->SetRect(104, 28, 560, 23, false);
	NEW_CLASS(this->lblOFile, UI::GUILabel(ui, this, CSTR("Output")));
	this->lblOFile->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtOFile, UI::GUITextBox(ui, this, CSTR("")));
	this->txtOFile->SetRect(104, 52, 560, 23, false);
	NEW_CLASS(this->btnOFile, UI::GUIButton(ui, this, CSTR("Browse")));
	this->btnOFile->SetRect(660, 52, 75, 23, false);
	this->btnOFile->HandleButtonClick(OnOFileClicked, this);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this, CSTR("Prefix")));
	this->lblType->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->cboType, UI::GUIComboBox(ui, this, false));
	this->cboType->SetRect(104, 76, 200, 23, false);
	this->cboType->AddItem(CSTR("Preserve line"), 0);
	this->cboType->AddItem(CSTR("No Date and prefix"), 0);
	this->cboType->SetSelectedIndex(0);
	NEW_CLASS(this->lblSuffix, UI::GUILabel(ui, this, CSTR("Suffix to add")));
	this->lblSuffix->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtSuffix, UI::GUITextBox(ui, this, CSTR("")));
	this->txtSuffix->SetRect(104, 100, 560, 23, false);
	NEW_CLASS(this->btnExtract, UI::GUIButton(ui, this, CSTR("Extract")));
	this->btnExtract->SetRect(200, 124, 75, 23, false);
	this->btnExtract->HandleButtonClick(OnExtractClicked, this);
}

SSWR::AVIRead::AVIRLogExtractForm::~AVIRLogExtractForm()
{
}

void SSWR::AVIRead::AVIRLogExtractForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
