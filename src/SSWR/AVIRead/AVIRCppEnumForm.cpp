#include "Stdafx.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "SSWR/AVIRead/AVIRCppEnumForm.h"
#include "Text/CPPText.h"
#include "Text/MyString.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRCppEnumForm::OnConvClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCppEnumForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCppEnumForm>();
	me->ConvEnum();
}

void __stdcall SSWR::AVIRead::AVIRCppEnumForm::OnConv2Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCppEnumForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCppEnumForm>();
	Text::StringBuilderUTF8 sb;
	UI::Clipboard clipboard(me->GetHandle());
	if (clipboard.GetString(me->GetHandle(), sb))
	{
		me->txtSource->SetText(sb.ToCString());
	}
	else
	{
		return;
	}
	me->ConvEnum();
	sb.ClearStr();
	me->txtDest->GetText(sb);
	clipboard.SetString(me->GetHandle(), sb.ToCString());
}

void SSWR::AVIRead::AVIRCppEnumForm::ConvEnum()
{
	Text::StringBuilderUTF8 srcSb;
	Text::StringBuilderUTF8 destSb;
	Text::StringBuilderUTF8 sbPrefix;
	Data::ArrayListStringNN enumList;

	UOSInt type = this->cboType->GetSelectedIndex();
	this->txtPrefix->GetText(sbPrefix);
	this->txtSource->GetText(srcSb);
	if (Text::CPPText::ParseEnum(&enumList, srcSb.ToCString(), sbPrefix))
	{
		Data::ArrayIterator<NN<Text::String>> it = enumList.Iterator();
		NN<Text::String> s;
		if (type == 0)
		{
			while (it.HasNext())
			{
				s = it.Next();
				destSb.AppendC(UTF8STRC("case "));
				destSb.AppendC(sbPrefix.ToString(), sbPrefix.GetLength());
				destSb.Append(s);
				destSb.AppendC(UTF8STRC(":\r\n"));
			}
		}
		else if (type == 1)
		{
			while (it.HasNext())
			{
				s = it.Next();
				destSb.AppendC(UTF8STRC("case "));
				destSb.AppendC(sbPrefix.ToString(), sbPrefix.GetLength());
				destSb.Append(s);
				destSb.AppendC(UTF8STRC(":\r\n"));
				destSb.AppendC(UTF8STRC("\treturn CSTR(\""));
				destSb.Append(s);
				destSb.AppendC(UTF8STRC("\");\r\n"));
			}
		}
		else if (type == 2)
		{
			if (sbPrefix.EndsWith(UTF8STRC("::")))
			{
				sbPrefix.RemoveChars(2);
			}
			while (it.HasNext())
			{
				s = it.Next();
				destSb.AppendC(UTF8STRC("CBOADDENUM(cbo, "));
				destSb.AppendC(sbPrefix.ToString(), sbPrefix.GetLength());
				destSb.AppendC(UTF8STRC(", "));
				destSb.Append(s);
				destSb.AppendC(UTF8STRC(");\r\n"));
			}
		}
		else if (type == 3)
		{
			if (sbPrefix.EndsWith(UTF8STRC("::")))
			{
				destSb.AppendC(UTF8STRC("Text::CString "));
				destSb.Append(sbPrefix);
				destSb.RemoveChars(2);
				destSb.AppendC(UTF8STRC("GetName("));
				destSb.Append(sbPrefix);
				destSb.RemoveChars(2);
				destSb.AppendC(UTF8STRC(" val)\r\n"));
				destSb.AppendC(UTF8STRC("{\r\n"));
				destSb.AppendC(UTF8STRC("\tswitch (val)\r\n"));
				destSb.AppendC(UTF8STRC("\t{\r\n"));
				while (it.HasNext())
				{
					s = it.Next();
					destSb.AppendC(UTF8STRC("\tcase "));
					destSb.AppendC(sbPrefix.ToString(), sbPrefix.GetLength());
					destSb.Append(s);
					destSb.AppendC(UTF8STRC(":\r\n"));
					destSb.AppendC(UTF8STRC("\t\treturn CSTR(\""));
					destSb.Append(s);
					destSb.AppendC(UTF8STRC("\");\r\n"));
				}
				destSb.AppendC(UTF8STRC("\tdefault:\r\n"));
				destSb.AppendC(UTF8STRC("\t\treturn CSTR_NULL;\r\n"));
				destSb.AppendC(UTF8STRC("\t}\r\n"));
				destSb.AppendC(UTF8STRC("}\r\n"));
			}
		}
		this->txtDest->SetText(destSb.ToCString());
	}
	else
	{
		this->txtDest->SetText(CSTR(""));
	}
	enumList.FreeAll();
}

SSWR::AVIRead::AVIRCppEnumForm::AVIRCppEnumForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("C++ Enum to Switch case"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 55, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblType = ui->NewLabel(this->pnlCtrl, CSTR("Type"));
	this->lblType->SetRect(4, 4, 100, 23, false);
	this->cboType = ui->NewComboBox(this->pnlCtrl, false);
	this->cboType->SetRect(104, 4, 150, 23, false);
	this->cboType->AddItem(CSTR("Switch Cases"), 0);
	this->cboType->AddItem(CSTR("Switch Cases with return"), 0);
	this->cboType->AddItem(CSTR("ComboBox Add Enum"), 0);
	this->cboType->AddItem(CSTR("GetName Function"), 0);
	this->cboType->SetSelectedIndex(0);
	this->btnConv = ui->NewButton(this->pnlCtrl, CSTR("Convert"));
	this->btnConv->SetRect(4, 28, 75, 23, false);
	this->btnConv->HandleButtonClick(OnConvClicked, this);
	this->btnConv2 = ui->NewButton(this->pnlCtrl, CSTR("Paste-Conv-Copy"));
	this->btnConv2->SetRect(84, 28, 150, 23, false);
	this->btnConv2->HandleButtonClick(OnConv2Clicked, this);
	this->lblPrefix = ui->NewLabel(this->pnlCtrl, CSTR("Prefix"));
	this->lblPrefix->SetRect(234, 28, 100, 23, false);
	this->txtPrefix = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtPrefix->SetRect(334, 28, 100, 23, false);
	this->txtSource = ui->NewTextBox(*this, CSTR(""), true);
	this->txtSource->SetRect(0, 0, 512, 100, false);
	this->txtSource->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->txtDest = ui->NewTextBox(*this, CSTR(""), true);
	this->txtDest->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDest->SetReadOnly(true);
}

SSWR::AVIRead::AVIRCppEnumForm::~AVIRCppEnumForm()
{
}

void SSWR::AVIRead::AVIRCppEnumForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
