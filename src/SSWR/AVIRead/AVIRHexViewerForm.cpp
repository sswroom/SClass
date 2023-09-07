#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRHexViewerForm.h"
#include "SSWR/AVIRead/AVIRHexViewerGoToForm.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "UI/FileDialog.h"
#include "UI/FontDialog.h"
#include "UI/MessageDialog.h"

typedef enum
{
	MNU_GOTO = 101,
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnFilesDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	Bool dynamicSize = me->chkDynamicSize->IsChecked();
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->LoadFile(files[i]->ToCString(), dynamicSize))
		{
			break;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnEndianChg(void *userObj, Bool newState)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	OnOffsetChg(userObj, me->hexView->GetCurrOfst());
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnOffsetChg(void *userObj, UInt64 ofst)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UInt8 buff[8];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt readSize = me->hexView->GetFileData(ofst, 8, BYTEARR(buff));
	Bool bigEndian = me->radEndianBig->IsSelected();
	if (readSize >= 1)
	{
		sptr = Text::StrInt16(sbuff, (Int8)buff[0]);
		me->txtInt8->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, buff[0]);
		me->txtUInt8->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		me->txtInt8->SetText(CSTR("-"));
		me->txtUInt8->SetText(CSTR("-"));
	}
	if (readSize >= 2)
	{
		if (bigEndian)
		{
			sptr = Text::StrInt16(sbuff, ReadMInt16(buff));
			me->txtInt16->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, ReadMUInt16(buff));
			me->txtUInt16->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			sptr = Text::StrInt16(sbuff, ReadInt16(buff));
			me->txtInt16->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, ReadUInt16(buff));
			me->txtUInt16->SetText(CSTRP(sbuff, sptr));
		}
	}
	else
	{
		me->txtInt16->SetText(CSTR("-"));
		me->txtUInt16->SetText(CSTR("-"));
	}
	if (readSize >= 4)
	{
		if (bigEndian)
		{
			sptr = Text::StrInt32(sbuff, ReadMInt32(buff));
			me->txtInt32->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ReadMUInt32(buff));
			me->txtUInt32->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ReadMFloat(buff));
			me->txtFloat32->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			sptr = Text::StrInt32(sbuff, ReadInt32(buff));
			me->txtInt32->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ReadUInt32(buff));
			me->txtUInt32->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ReadFloat(buff));
			me->txtFloat32->SetText(CSTRP(sbuff, sptr));
		}
	}
	else
	{
		me->txtInt32->SetText(CSTR("-"));
		me->txtUInt32->SetText(CSTR("-"));
		me->txtFloat32->SetText(CSTR("-"));
	}
	if (readSize >= 8)
	{
		if (bigEndian)
		{
			sptr = Text::StrInt64(sbuff, ReadMInt64(buff));
			me->txtInt64->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, ReadMUInt64(buff));
			me->txtUInt64->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ReadMDouble(buff));
			me->txtFloat64->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			sptr = Text::StrInt64(sbuff, ReadInt64(buff));
			me->txtInt64->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, ReadUInt64(buff));
			me->txtUInt64->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ReadDouble(buff));
			me->txtFloat64->SetText(CSTRP(sbuff, sptr));
		}
	}
	else
	{
		me->txtInt64->SetText(CSTR("-"));
		me->txtUInt64->SetText(CSTR("-"));
		me->txtFloat64->SetText(CSTR("-"));
	}
	if (Text::CharUtil::UTF8CharValid(buff))
	{
		UTF32Char c;
		Text::StrReadChar(buff, &c);
		sptr = Text::StrHexVal32V(Text::StrConcatC(sbuff, UTF8STRC("0x")), (UInt32)c);
		me->txtUTF8CharCode->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		me->txtUTF8CharCode->SetText(CSTR("-"));
	}

	Text::StringBuilderUTF8 sb;
	if (me->hexView->GetFrameName(sb))
	{
		me->txtFrameName->SetText(sb.ToCString());
		Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo*> fieldList;
		me->hexView->GetFieldInfos(&fieldList);
		const IO::FileAnalyse::FrameDetail::FieldInfo *field;
		if (fieldList.GetCount() == 0)
		{
			me->txtFieldDetail->SetText(CSTR("-"));
		}
		else
		{
			sb.ClearStr();
			UOSInt i = 0;
			UOSInt j = fieldList.GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("\r\n"));
				}
				field = fieldList.GetItem(i);
				sb.Append(field->name);
				sb.AppendUTF8Char('=');
				sb.Append(field->value);
				i++;
			}
			me->txtFieldDetail->SetText(sb.ToCString());
		}
	}
	else
	{
		me->txtFrameName->SetText(CSTR("-"));
		me->txtFieldDetail->SetText(CSTR("-"));
	}
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnFontClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UI::FontDialog dlg(me->fontName, me->fontHeightPt, me->fontIsBold, false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		Text::String *fontName = dlg.GetFontName();
		me->SetFont(fontName->v, fontName->leng, dlg.GetFontSizePt(), dlg.IsBold());
		me->hexView->UpdateFont();
	}
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnNextUnkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	if (!me->hexView->GoToNextUnkField())
	{
		UI::MessageDialog::ShowDialog(CSTR("No unknown field found"), CSTR("Hex Viewer"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnOpenFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"HexViewerOpen", false);
	dlg.SetAllowMultiSel(false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg.GetFileName()->ToCString(), me->chkDynamicSize->IsChecked());
	}
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnExtractBeginClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UTF8Char sbuff[17];
	UTF8Char *sptr;
	sptr = Text::StrHexVal64V(sbuff, me->hexView->GetCurrOfst());
	me->txtExtractBegin->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnExtractEndClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UTF8Char sbuff[17];
	UTF8Char *sptr;
	sptr = Text::StrHexVal64V(sbuff, me->hexView->GetCurrOfst());
	me->txtExtractEnd->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnExtractClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UTF8Char sbuff[17];
	UInt64 beginOfst;
	UInt64 endOfst;
	me->txtExtractBegin->GetText(sbuff);
	if (!Text::StrHex2UInt64V(sbuff, beginOfst))
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in parsing begin offset"), CSTR("Hex Viewer"), me);
		return;
	}
	me->txtExtractEnd->GetText(sbuff);
	if (!Text::StrHex2UInt64V(sbuff, endOfst))
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in parsing end offset"), CSTR("Hex Viewer"), me);
		return;
	}
	if (beginOfst >= endOfst)
	{
		UI::MessageDialog::ShowDialog(CSTR("Current Offsets are not valid to extract file"), CSTR("Hex Viewer"), me);
		return;
	}
	endOfst -= beginOfst;
	UOSInt buffSize;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"HexViewerExtract", true);
	dlg.AddFilter(CSTR("*.dat"), CSTR("Data File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (endOfst < 1048576)
		{
			Data::ByteBuffer buff((UOSInt)endOfst);
			buffSize = me->hexView->GetFileData(beginOfst, (UOSInt)endOfst, buff);
			fs.Write(buff.Ptr(), buffSize);
		}
		else
		{
			Bool hasError = false;
			Data::ByteBuffer buff(1048576);
			while (endOfst >= 1048576)
			{
				buffSize = me->hexView->GetFileData(beginOfst, 1048576, buff);
				fs.Write(buff.Ptr(), buffSize);
				beginOfst += 1048576;
				endOfst -= 1048576;
				if (buffSize != 1048576)
				{
					hasError = true;
					break;
				}
			}
			if (!hasError && endOfst > 0)
			{
				buffSize = me->hexView->GetFileData(beginOfst, (UOSInt)endOfst, buff);
				fs.Write(buff.Ptr(), buffSize);
				beginOfst += endOfst;
				if (buffSize != endOfst)
				{
					hasError = true;
				}
			}
			if (hasError)
			{
				UI::MessageDialog::ShowDialog(CSTR("Error in reading from source file"), CSTR("Hex Viewer"), me);
			}
		}
	}
}

Bool SSWR::AVIRead::AVIRHexViewerForm::LoadFile(Text::CStringNN fileName, Bool dynamicSize)
{
	Bool succ = this->hexView->LoadFile(fileName, dynamicSize);
	if (succ)
	{
		Text::CString name = this->hexView->GetAnalyzerName();
		if (name.v)
		{
			this->txtFileFormat->SetText(name.OrEmpty());
		}
		else if (dynamicSize)
		{
			this->txtFileFormat->SetText(CSTR("Unknown (Dynamic Size cannot determine)"));
		}
		else
		{
			this->txtFileFormat->SetText(CSTR("Unknown"));
		}

		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Hex Viewer - "));
		sb.Append(fileName);
		this->SetText(sb.ToCString());
	}
	return succ;
}

SSWR::AVIRead::AVIRHexViewerForm::AVIRHexViewerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Hex Viewer"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetRect(0, 0, 100, 200, false);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->hexView, UI::GUIHexFileView(ui, this, this->core->GetDrawEngine()));
	this->hexView->SetDockType(UI::GUIControl::DOCK_FILL);
	this->hexView->HandleOffsetChg(OnOffsetChg, this);
	
	this->tpValues = this->tcMain->AddTabPage(CSTR("Values"));
	NEW_CLASS(this->lblEndian, UI::GUILabel(ui, this->tpValues, CSTR("Endian")));
	this->lblEndian->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->radEndianLittle, UI::GUIRadioButton(ui, this->tpValues, CSTR("Little Endian"), true));
	this->radEndianLittle->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->radEndianBig, UI::GUIRadioButton(ui, this->tpValues, CSTR("Big Endian"), false));
	this->radEndianBig->SetRect(254, 4, 150, 23, false);
	this->radEndianBig->HandleSelectedChange(OnEndianChg, this);
	NEW_CLASS(this->lblInt8, UI::GUILabel(ui, this->tpValues, CSTR("Int8")));
	this->lblInt8->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtInt8, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtInt8->SetRect(104, 28, 150, 23, false);
	this->txtInt8->SetReadOnly(true);
	NEW_CLASS(this->lblInt16, UI::GUILabel(ui, this->tpValues, CSTR("Int16")));
	this->lblInt16->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtInt16, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtInt16->SetRect(104, 52, 150, 23, false);
	this->txtInt16->SetReadOnly(true);
	NEW_CLASS(this->lblInt32, UI::GUILabel(ui, this->tpValues, CSTR("Int32")));
	this->lblInt32->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtInt32, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtInt32->SetRect(104, 76, 150, 23, false);
	this->txtInt32->SetReadOnly(true);
	NEW_CLASS(this->lblInt64, UI::GUILabel(ui, this->tpValues, CSTR("Int64")));
	this->lblInt64->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtInt64, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtInt64->SetRect(104, 100, 150, 23, false);
	this->txtInt64->SetReadOnly(true);
	NEW_CLASS(this->lblUInt8, UI::GUILabel(ui, this->tpValues, CSTR("UInt8")));
	this->lblUInt8->SetRect(254, 28, 100, 23, false);
	NEW_CLASS(this->txtUInt8, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtUInt8->SetRect(354, 28, 150, 23, false);
	this->txtUInt8->SetReadOnly(true);
	NEW_CLASS(this->lblUInt16, UI::GUILabel(ui, this->tpValues, CSTR("UInt16")));
	this->lblUInt16->SetRect(254, 52, 100, 23, false);
	NEW_CLASS(this->txtUInt16, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtUInt16->SetRect(354, 52, 150, 23, false);
	this->txtUInt16->SetReadOnly(true);
	NEW_CLASS(this->lblUInt32, UI::GUILabel(ui, this->tpValues, CSTR("UInt32")));
	this->lblUInt32->SetRect(254, 76, 100, 23, false);
	NEW_CLASS(this->txtUInt32, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtUInt32->SetRect(354, 76, 150, 23, false);
	this->txtUInt32->SetReadOnly(true);
	NEW_CLASS(this->lblUInt64, UI::GUILabel(ui, this->tpValues, CSTR("UInt64")));
	this->lblUInt64->SetRect(254, 100, 100, 23, false);
	NEW_CLASS(this->txtUInt64, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtUInt64->SetRect(354, 100, 150, 23, false);
	this->txtUInt64->SetReadOnly(true);
	NEW_CLASS(this->lblFloat32, UI::GUILabel(ui, this->tpValues, CSTR("Float32")));
	this->lblFloat32->SetRect(504, 28, 100, 23, false);
	NEW_CLASS(this->txtFloat32, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtFloat32->SetRect(604, 28, 150, 23, false);
	this->txtFloat32->SetReadOnly(true);
	NEW_CLASS(this->lblFloat64, UI::GUILabel(ui, this->tpValues, CSTR("Float64")));
	this->lblFloat64->SetRect(504, 52, 100, 23, false);
	NEW_CLASS(this->txtFloat64, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtFloat64->SetRect(604, 52, 150, 23, false);
	this->txtFloat64->SetReadOnly(true);
	NEW_CLASS(this->lblUTF8CharCode, UI::GUILabel(ui, this->tpValues, CSTR("Char Code")));
	this->lblUTF8CharCode->SetRect(504, 76, 100, 23, false);
	NEW_CLASS(this->txtUTF8CharCode, UI::GUITextBox(ui, this->tpValues, CSTR("")));
	this->txtUTF8CharCode->SetRect(604, 76, 150, 23, false);
	this->txtUTF8CharCode->SetReadOnly(true);
	NEW_CLASS(this->btnFont, UI::GUIButton(ui, this->tpValues, CSTR("Sel Font")));
	this->btnFont->SetRect(4, 124, 75, 23, false);
	this->btnFont->HandleButtonClick(OnFontClicked, this);
	NEW_CLASS(this->btnNextUnk, UI::GUIButton(ui, this->tpValues, CSTR("Next Unknown")));
	this->btnNextUnk->SetRect(84, 124, 75, 23, false);
	this->btnNextUnk->HandleButtonClick(OnNextUnkClicked, this);
	NEW_CLASS(this->chkDynamicSize, UI::GUICheckBox(ui, this->tpValues, CSTR("Dynamic Size"), false));
	this->chkDynamicSize->SetRect(164, 124, 100, 23, false);
	NEW_CLASS(this->btnOpenFile, UI::GUIButton(ui, this->tpValues, CSTR("Open")));
	this->btnOpenFile->SetRect(604, 124, 75, 23, false);
	this->btnOpenFile->HandleButtonClick(OnOpenFileClicked, this);

	this->tpAnalyse = this->tcMain->AddTabPage(CSTR("Analyse"));
	NEW_CLASS(this->lblFileFormat, UI::GUILabel(ui, this->tpAnalyse, CSTR("File Format")));
	this->lblFileFormat->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFileFormat, UI::GUITextBox(ui, this->tpAnalyse, CSTR("")));
	this->txtFileFormat->SetRect(104, 4, 500, 23, false);
	this->txtFileFormat->SetReadOnly(true);
	NEW_CLASS(this->lblFrameName, UI::GUILabel(ui, this->tpAnalyse, CSTR("Frame Name")));
	this->lblFrameName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtFrameName, UI::GUITextBox(ui, this->tpAnalyse, CSTR("")));
	this->txtFrameName->SetRect(104, 28, 500, 23, false);
	this->txtFrameName->SetReadOnly(true);
	NEW_CLASS(this->lblFieldDetail, UI::GUILabel(ui, this->tpAnalyse, CSTR("Field Detail")));
	this->lblFieldDetail->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtFieldDetail, UI::GUITextBox(ui, this->tpAnalyse, CSTR(""), true));
	this->txtFieldDetail->SetRect(104, 52, 500, 128, false);
	this->txtFieldDetail->SetReadOnly(true);


	this->tpExtract = this->tcMain->AddTabPage(CSTR("Extract"));
	NEW_CLASS(this->lblExtractBegin, UI::GUILabel(ui, this->tpExtract, CSTR("Begin")));
	this->lblExtractBegin->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtExtractBegin, UI::GUITextBox(ui, this->tpExtract, CSTR("0")));
	this->txtExtractBegin->SetRect(104, 4, 200, 23, false);
	this->txtExtractBegin->SetReadOnly(true);
	NEW_CLASS(this->btnExtractBegin, UI::GUIButton(ui, this->tpExtract, CSTR("Set")));
	this->btnExtractBegin->SetRect(304, 4, 75, 23, false);
	this->btnExtractBegin->HandleButtonClick(OnExtractBeginClicked, this);
	NEW_CLASS(this->lblExtractEnd, UI::GUILabel(ui, this->tpExtract, CSTR("End")));
	this->lblExtractEnd->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtExtractEnd, UI::GUITextBox(ui, this->tpExtract, CSTR("0")));
	this->txtExtractEnd->SetRect(104, 28, 200, 23, false);
	this->txtExtractEnd->SetReadOnly(true);
	NEW_CLASS(this->btnExtractEnd, UI::GUIButton(ui, this->tpExtract, CSTR("Set")));
	this->btnExtractEnd->SetRect(304, 28, 75, 23, false);
	this->btnExtractEnd->HandleButtonClick(OnExtractEndClicked, this);
	NEW_CLASS(this->btnExtract, UI::GUIButton(ui, this->tpExtract, CSTR("Extract")));
	this->btnExtract->SetRect(104, 52, 75, 23, false);
	this->btnExtract->HandleButtonClick(OnExtractClicked, this);
	
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	UI::GUIMenu *mnu = this->mnuMain->AddSubMenu(CSTR("Navigate"));
	mnu->AddItem(CSTR("Go To..."), MNU_GOTO, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_G);
	this->SetMenu(this->mnuMain);

	this->HandleDropFiles(OnFilesDrop, this);
}

SSWR::AVIRead::AVIRHexViewerForm::~AVIRHexViewerForm()
{
}

void SSWR::AVIRead::AVIRHexViewerForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_GOTO:
	{
		SSWR::AVIRead::AVIRHexViewerGoToForm frm(0, this->ui, this->core, this->hexView->GetCurrOfst(), this->hexView->GetFileSize());
		if (frm.ShowDialog(this))
		{
			this->hexView->GoToOffset(frm.GetOffset());
		}
		break;
	}
	}
}

void SSWR::AVIRead::AVIRHexViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRHexViewerForm::SetData(NotNullPtr<IO::StreamData> fd, IO::FileAnalyse::IFileAnalyse *fileAnalyse)
{
	if (this->hexView->LoadData(fd->GetPartialData(0, fd->GetDataSize()), fileAnalyse))
	{
		Text::CString name = this->hexView->GetAnalyzerName();
		if (name.v)
		{
			this->txtFileFormat->SetText(name.OrEmpty());
		}
		else
		{
			this->txtFileFormat->SetText(CSTR("Unknown"));
		}
	}
}
