#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRHexViewerForm.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/FontDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnFilesDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	Bool dynamicSize = me->chkDynamicSize->IsChecked();
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->hexView->LoadFile(files[i], dynamicSize))
		{
			const UTF8Char *name = me->hexView->GetAnalyzerName();
			if (name)
			{
				me->txtFileFormat->SetText(name);
			}
			else if (dynamicSize)
			{
				me->txtFileFormat->SetText((const UTF8Char*)"Unknown (Dynamic Size cannot determine)");
			}
			else
			{
				me->txtFileFormat->SetText((const UTF8Char*)"Unknown");
			}

			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Hex Viewer - ");
			sb.Append(files[i]);
			me->SetText(sb.ToString());
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
	UOSInt readSize = me->hexView->GetFileData(ofst, 8, buff);
	Bool bigEndian = me->radEndianBig->IsSelected();
	if (readSize >= 1)
	{
		Text::StrInt16(sbuff, (Int8)buff[0]);
		me->txtInt8->SetText(sbuff);
		Text::StrUInt16(sbuff, buff[0]);
		me->txtUInt8->SetText(sbuff);
	}
	else
	{
		me->txtInt8->SetText((const UTF8Char*)"-");
		me->txtUInt8->SetText((const UTF8Char*)"-");
	}
	if (readSize >= 2)
	{
		if (bigEndian)
		{
			Text::StrInt16(sbuff, ReadMInt16(buff));
			me->txtInt16->SetText(sbuff);
			Text::StrUInt16(sbuff, ReadMUInt16(buff));
			me->txtUInt16->SetText(sbuff);
		}
		else
		{
			Text::StrInt16(sbuff, ReadInt16(buff));
			me->txtInt16->SetText(sbuff);
			Text::StrUInt16(sbuff, ReadUInt16(buff));
			me->txtUInt16->SetText(sbuff);
		}
	}
	else
	{
		me->txtInt16->SetText((const UTF8Char*)"-");
		me->txtUInt16->SetText((const UTF8Char*)"-");
	}
	if (readSize >= 4)
	{
		if (bigEndian)
		{
			Text::StrInt32(sbuff, ReadMInt32(buff));
			me->txtInt32->SetText(sbuff);
			Text::StrUInt32(sbuff, ReadMUInt32(buff));
			me->txtUInt32->SetText(sbuff);
			Text::StrDouble(sbuff, ReadMFloat(buff));
			me->txtFloat32->SetText(sbuff);
		}
		else
		{
			Text::StrInt32(sbuff, ReadInt32(buff));
			me->txtInt32->SetText(sbuff);
			Text::StrUInt32(sbuff, ReadUInt32(buff));
			me->txtUInt32->SetText(sbuff);
			Text::StrDouble(sbuff, ReadFloat(buff));
			me->txtFloat32->SetText(sbuff);
		}
	}
	else
	{
		me->txtInt32->SetText((const UTF8Char*)"-");
		me->txtUInt32->SetText((const UTF8Char*)"-");
		me->txtFloat32->SetText((const UTF8Char*)"-");
	}
	if (readSize >= 8)
	{
		if (bigEndian)
		{
			Text::StrInt64(sbuff, ReadMInt64(buff));
			me->txtInt64->SetText(sbuff);
			Text::StrUInt64(sbuff, ReadMUInt64(buff));
			me->txtUInt64->SetText(sbuff);
			Text::StrDouble(sbuff, ReadMDouble(buff));
			me->txtFloat64->SetText(sbuff);
		}
		else
		{
			Text::StrInt64(sbuff, ReadInt64(buff));
			me->txtInt64->SetText(sbuff);
			Text::StrUInt64(sbuff, ReadUInt64(buff));
			me->txtUInt64->SetText(sbuff);
			Text::StrDouble(sbuff, ReadDouble(buff));
			me->txtFloat64->SetText(sbuff);
		}
	}
	else
	{
		me->txtInt64->SetText((const UTF8Char*)"-");
		me->txtUInt64->SetText((const UTF8Char*)"-");
		me->txtFloat64->SetText((const UTF8Char*)"-");
	}
	if (Text::CharUtil::UTF8CharValid(buff))
	{
		UTF32Char c;
		Text::StrReadChar(buff, &c);
		Text::StrHexVal32V(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), (UInt32)c);
		me->txtUTF8CharCode->SetText(sbuff);
	}
	else
	{
		me->txtUTF8CharCode->SetText((const UTF8Char*)"-");
	}

	Text::StringBuilderUTF8 sb;
	if (me->hexView->GetFrameName(&sb))
	{
		me->txtFrameName->SetText(sb.ToString());
		Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo*> fieldList;
		me->hexView->GetFieldInfos(&fieldList);
		const IO::FileAnalyse::FrameDetail::FieldInfo *field;
		if (fieldList.GetCount() == 0)
		{
			me->txtFieldDetail->SetText((const UTF8Char*)"-");
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
					sb.Append((const UTF8Char*)"\r\n");
				}
				field = fieldList.GetItem(i);
				sb.Append(field->name);
				if (field->value)
				{
					sb.AppendChar('=', 1);
					sb.Append(field->value);
				}
				i++;
			}
			me->txtFieldDetail->SetText(sb.ToString());
		}
	}
	else
	{
		me->txtFrameName->SetText((const UTF8Char*)"-");
		me->txtFieldDetail->SetText((const UTF8Char*)"-");
	}
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnFontClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UI::FontDialog *dlg;
	NEW_CLASS(dlg, UI::FontDialog(me->fontName, me->fontHeightPt, me->fontIsBold, false));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->SetFont(dlg->GetFontName(), dlg->GetFontSizePt(), dlg->IsBold());
		me->hexView->UpdateFont();
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnNextUnkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	if (!me->hexView->GoToNextUnkField())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"No unknown field found", (const UTF8Char*)"Hex Viewer", me);
	}
}

SSWR::AVIRead::AVIRHexViewerForm::AVIRHexViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText((const UTF8Char*)"Hex Viewer");
	this->SetFont(0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetRect(0, 0, 100, 200, false);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->hexView, UI::GUIHexFileView(ui, this, this->core->GetDrawEngine()));
	this->hexView->SetDockType(UI::GUIControl::DOCK_FILL);
	this->hexView->HandleOffsetChg(OnOffsetChg, this);
	
	this->tpValues = this->tcMain->AddTabPage((const UTF8Char*)"Values");
	NEW_CLASS(this->lblEndian, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Endian"));
	this->lblEndian->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->radEndianLittle, UI::GUIRadioButton(ui, this->tpValues, (const UTF8Char*)"Little Endian", true));
	this->radEndianLittle->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->radEndianBig, UI::GUIRadioButton(ui, this->tpValues, (const UTF8Char*)"Big Endian", false));
	this->radEndianBig->SetRect(254, 4, 150, 23, false);
	this->radEndianBig->HandleSelectedChange(OnEndianChg, this);
	NEW_CLASS(this->lblInt8, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Int8"));
	this->lblInt8->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtInt8, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtInt8->SetRect(104, 28, 150, 23, false);
	this->txtInt8->SetReadOnly(true);
	NEW_CLASS(this->lblInt16, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Int16"));
	this->lblInt16->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtInt16, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtInt16->SetRect(104, 52, 150, 23, false);
	this->txtInt16->SetReadOnly(true);
	NEW_CLASS(this->lblInt32, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Int32"));
	this->lblInt32->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtInt32, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtInt32->SetRect(104, 76, 150, 23, false);
	this->txtInt32->SetReadOnly(true);
	NEW_CLASS(this->lblInt64, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Int64"));
	this->lblInt64->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtInt64, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtInt64->SetRect(104, 100, 150, 23, false);
	this->txtInt64->SetReadOnly(true);
	NEW_CLASS(this->lblUInt8, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"UInt8"));
	this->lblUInt8->SetRect(254, 28, 100, 23, false);
	NEW_CLASS(this->txtUInt8, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtUInt8->SetRect(354, 28, 150, 23, false);
	this->txtUInt8->SetReadOnly(true);
	NEW_CLASS(this->lblUInt16, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"UInt16"));
	this->lblUInt16->SetRect(254, 52, 100, 23, false);
	NEW_CLASS(this->txtUInt16, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtUInt16->SetRect(354, 52, 150, 23, false);
	this->txtUInt16->SetReadOnly(true);
	NEW_CLASS(this->lblUInt32, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"UInt32"));
	this->lblUInt32->SetRect(254, 76, 100, 23, false);
	NEW_CLASS(this->txtUInt32, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtUInt32->SetRect(354, 76, 150, 23, false);
	this->txtUInt32->SetReadOnly(true);
	NEW_CLASS(this->lblUInt64, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"UInt64"));
	this->lblUInt64->SetRect(254, 100, 100, 23, false);
	NEW_CLASS(this->txtUInt64, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtUInt64->SetRect(354, 100, 150, 23, false);
	this->txtUInt64->SetReadOnly(true);
	NEW_CLASS(this->lblFloat32, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Float32"));
	this->lblFloat32->SetRect(504, 28, 100, 23, false);
	NEW_CLASS(this->txtFloat32, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtFloat32->SetRect(604, 28, 150, 23, false);
	this->txtFloat32->SetReadOnly(true);
	NEW_CLASS(this->lblFloat64, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Float64"));
	this->lblFloat64->SetRect(504, 52, 100, 23, false);
	NEW_CLASS(this->txtFloat64, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtFloat64->SetRect(604, 52, 150, 23, false);
	this->txtFloat64->SetReadOnly(true);
	NEW_CLASS(this->lblUTF8CharCode, UI::GUILabel(ui, this->tpValues, (const UTF8Char*)"Char Code"));
	this->lblUTF8CharCode->SetRect(504, 76, 100, 23, false);
	NEW_CLASS(this->txtUTF8CharCode, UI::GUITextBox(ui, this->tpValues, (const UTF8Char*)""));
	this->txtUTF8CharCode->SetRect(604, 76, 150, 23, false);
	this->txtUTF8CharCode->SetReadOnly(true);
	NEW_CLASS(this->btnFont, UI::GUIButton(ui, this->tpValues, (const UTF8Char *)"Sel Font"));
	this->btnFont->SetRect(4, 124, 75, 23, false);
	this->btnFont->HandleButtonClick(OnFontClicked, this);
	NEW_CLASS(this->btnNextUnk, UI::GUIButton(ui, this->tpValues, (const UTF8Char*)"Next Unknown"));
	this->btnNextUnk->SetRect(84, 124, 75, 23, false);
	this->btnNextUnk->HandleButtonClick(OnNextUnkClicked, this);
	NEW_CLASS(this->chkDynamicSize, UI::GUICheckBox(ui, this->tpValues, (const UTF8Char*)"Dynamic Size", false));
	this->chkDynamicSize->SetRect(164, 124, 100, 23, false);

	this->tpAnalyse = this->tcMain->AddTabPage((const UTF8Char*)"Analyse");
	NEW_CLASS(this->lblFileFormat, UI::GUILabel(ui, this->tpAnalyse, (const UTF8Char*)"File Format"));
	this->lblFileFormat->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFileFormat, UI::GUITextBox(ui, this->tpAnalyse, (const UTF8Char*)""));
	this->txtFileFormat->SetRect(104, 4, 500, 23, false);
	this->txtFileFormat->SetReadOnly(true);
	NEW_CLASS(this->lblFrameName, UI::GUILabel(ui, this->tpAnalyse, (const UTF8Char*)"Frame Name"));
	this->lblFrameName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtFrameName, UI::GUITextBox(ui, this->tpAnalyse, (const UTF8Char*)""));
	this->txtFrameName->SetRect(104, 28, 500, 23, false);
	this->txtFrameName->SetReadOnly(true);
	NEW_CLASS(this->lblFieldDetail, UI::GUILabel(ui, this->tpAnalyse, (const UTF8Char*)"Field Detail"));
	this->lblFieldDetail->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtFieldDetail, UI::GUITextBox(ui, this->tpAnalyse, (const UTF8Char*)"", true));
	this->txtFieldDetail->SetRect(104, 52, 500, 128, false);
	this->txtFieldDetail->SetReadOnly(true);

	this->HandleDropFiles(OnFilesDrop, this);
}

SSWR::AVIRead::AVIRHexViewerForm::~AVIRHexViewerForm()
{
}

void SSWR::AVIRead::AVIRHexViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
