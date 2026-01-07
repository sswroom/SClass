#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRLogExtractForm.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRLogExtractForm::OnSFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogExtractForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogExtractForm>();
	Text::StringBuilderUTF8 sb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"LogExtractSFile", false);
	dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
	dlg->SetAllowMultiSel(false);
	me->txtSFile->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtSFile->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRLogExtractForm::OnOFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogExtractForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogExtractForm>();
	Text::StringBuilderUTF8 sb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"LogExtractOFile", true);
	dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
	dlg->AddFilter(CSTR("*.txt"), CSTR("Text File"));
	me->txtOFile->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtOFile->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRLogExtractForm::OnExtractClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogExtractForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogExtractForm>();
	UTF8Char sbuff[16];
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sbSuffix;
	UOSInt compareType = me->cboCompare->GetSelectedIndex();
	me->txtSFile->GetText(sb1);
	me->txtCompare->GetText(sb2);
	me->txtOFile->GetText(sb3);
	me->txtSuffix->GetText(sbSuffix);
	if (IO::Path::GetPathType(sb1.ToCString()) != IO::Path::PathType::File)
	{
		me->ui->ShowMsgOK(CSTR("Source File not found"), CSTR("Error"), me);
	}
	else if (sb2.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Prefix"), CSTR("Error"), me);
	}
	else if (sb3.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Output"), CSTR("Error"), me);
	}
	else
	{
		UOSInt outTyp = me->cboOutType->GetSelectedIndex();
		UOSInt i;
		Bool hasData;
		IO::FileStream fs1(sb1.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs1);
		IO::FileStream fs2(sb3.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs2.IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in creating Output file"), CSTR("Error"), me);
		}
		else
		{
			Text::UTF8Writer writer(fs2);
			writer.WriteSignature();

			sb1.ClearStr();
			hasData = reader.ReadLine(sb1, 1024);
			while (hasData)
			{
				reader.GetLastLineBreak(sbuff);
				while (hasData && sbuff[0] == 0)
				{
					hasData = reader.ReadLine(sb1, 1024);
					reader.GetLastLineBreak(sbuff);
				}

				sb1.Append(sbSuffix.ToCString());
				i = sb1.IndexOf('\t');
				if (i != INVALID_INDEX)
				{
					Bool match;
					if (compareType == 0)
					{
						match = sb1.Substring(i + 1).StartsWith(sb2.ToCString());
					}
					else if (compareType == 1)
					{
						match = sb1.IndexOf(sb2.ToCString()) != INVALID_INDEX;
					}
					else
					{
						match = sb1.EndsWith(sb2.ToCString());
					}
					if (match)
					{
						if (outTyp == 0)
						{
							writer.WriteLine(sb1.ToCString());
						}
						else
						{
							writer.WriteLine(sb1.ToCString().Substring(i + 1 + sb2.GetLength()));
						}
					}
				}

				sb1.ClearStr();
				hasData = reader.ReadLine(sb1, 1024);
			}

			me->ui->ShowMsgOK(CSTR("Complete log extract"), CSTR("Success"), me);
		}
	}
}

SSWR::AVIRead::AVIRLogExtractForm::AVIRLogExtractForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 184, ui)
{
	this->SetText(CSTR("Log Extract"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblSFile = ui->NewLabel(*this, CSTR("Source File"));
	this->lblSFile->SetRect(4, 4, 100, 23, false);
	this->txtSFile = ui->NewTextBox(*this, CSTR(""));
	this->txtSFile->SetRect(104, 4, 560, 23, false);
	this->btnSFile = ui->NewButton(*this, CSTR("Browse"));
	this->btnSFile->SetRect(660, 4, 75, 23, false);
	this->btnSFile->HandleButtonClick(OnSFileClicked, this);
	this->cboCompare = ui->NewComboBox(*this, false);
	this->cboCompare->SetRect(4, 28, 100, 23, false);
	this->cboCompare->AddItem(CSTR("Starts With"), 0);
	this->cboCompare->AddItem(CSTR("Contains"), 0);
	this->cboCompare->AddItem(CSTR("Ends With"), 0);
	this->cboCompare->SetSelectedIndex(0);
	this->txtCompare = ui->NewTextBox(*this, CSTR(""));
	this->txtCompare->SetRect(104, 28, 560, 23, false);
	this->lblOFile = ui->NewLabel(*this, CSTR("Output"));
	this->lblOFile->SetRect(4, 52, 100, 23, false);
	this->txtOFile = ui->NewTextBox(*this, CSTR(""));
	this->txtOFile->SetRect(104, 52, 560, 23, false);
	this->btnOFile = ui->NewButton(*this, CSTR("Browse"));
	this->btnOFile->SetRect(660, 52, 75, 23, false);
	this->btnOFile->HandleButtonClick(OnOFileClicked, this);
	this->lblOutType = ui->NewLabel(*this, CSTR("Out Type"));
	this->lblOutType->SetRect(4, 76, 100, 23, false);
	this->cboOutType = ui->NewComboBox(*this, false);
	this->cboOutType->SetRect(104, 76, 200, 23, false);
	this->cboOutType->AddItem(CSTR("Preserve line"), 0);
	this->cboOutType->AddItem(CSTR("No Date and prefix"), 0);
	this->cboOutType->SetSelectedIndex(0);
	this->lblSuffix = ui->NewLabel(*this, CSTR("Suffix to add"));
	this->lblSuffix->SetRect(4, 100, 100, 23, false);
	this->txtSuffix = ui->NewTextBox(*this, CSTR(""));
	this->txtSuffix->SetRect(104, 100, 560, 23, false);
	this->btnExtract = ui->NewButton(*this, CSTR("Extract"));
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
