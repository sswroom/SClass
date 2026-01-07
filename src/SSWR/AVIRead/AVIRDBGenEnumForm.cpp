#include "Stdafx.h"
#include "Data/FastStringMap.hpp"
#include "Data/Sort/ArtificialQuickSort.h"
#include "SSWR/AVIRead/AVIRDBGenEnumForm.h"
#include "Text/JSText.h"
#include "Text/StringCaseConverter.h"

#define ADDITEM(cbo, type) cbo->AddItem(Text::CaseTypeGetSample(type), (void*)type)

void SSWR::AVIRead::AVIRDBGenEnumForm::Generate()
{
	Text::CaseType srcCaseType = (Text::CaseType)this->cboValueCase->GetSelectedItem().GetUOSInt();
	Text::CaseType outCaseType = (Text::CaseType)this->cboOutputCase->GetSelectedItem().GetUOSInt();
	UOSInt outType = this->cboOutputFormat->GetSelectedIndex();
	Text::StringBuilderUTF8 sb;
	if (outType == 0)
	{
		UOSInt i = 0;
		UOSInt j = this->nameList.GetCount();
		sb.Append(CSTR("export enum "));
		Text::StringCaseConverter conv(srcCaseType);
		conv.Convert(this->colName->v, Text::CaseType::PascalCase, sb);
		sb.Append(CSTR(" {\r\n"));
		if (j > 0)
		{
			while (i < j)
			{
				sb.AppendUTF8Char('\t');
				conv.Convert(this->nameList.GetItemNoCheck(i)->v, outCaseType, sb);
				if (i + 1 < j) sb.AppendUTF8Char(',');
				sb.Append(CSTR("\r\n"));
				i++;
			}
		}
		sb.Append(CSTR("}"));
		this->txtEnum->SetText(sb.ToCString());
	}
	else
	{
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j = this->nameList.GetCount();
		sb.Append(CSTR("export const "));
		Text::StringCaseConverter conv(srcCaseType);
		conv.Convert(this->colName->v, Text::CaseType::PascalCase, sb);
		sb.Append(CSTR(" = {\r\n"));
		if (j > 0)
		{
			while (i < j)
			{
				sb.AppendUTF8Char('\t');
				conv.Convert(this->nameList.GetItemNoCheck(i)->v, outCaseType, sb);
				sb.Append(CSTR(": "));
				s = Text::JSText::ToNewJSTextDQuote(this->nameList.GetItemNoCheck(i)->v);
				sb.Append(s);
				s->Release();
				if (i + 1 < j) sb.AppendUTF8Char(',');
				sb.Append(CSTR("\r\n"));
				i++;
			}
		}
		sb.Append(CSTR("};"));
		this->txtEnum->SetText(sb.ToCString());
	}
	this->txtEnum->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRDBGenEnumForm::OnUpdate(AnyType userObj)
{
	userObj.GetNN<SSWR::AVIRead::AVIRDBGenEnumForm>()->Generate();
}

SSWR::AVIRead::AVIRDBGenEnumForm::AVIRDBGenEnumForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN table, UOSInt colIndex) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Database Generate Enum"));
	this->core = core;
	this->db = db;
	this->schema = schema;
	this->table = table;
	this->colIndex = colIndex;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 96, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtEnum = ui->NewTextBox(*this, CSTR(""), true);
	this->txtEnum->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbValues = ui->NewListBox(this->pnlMain, false);
	this->lbValues->SetRect(0, 0, 150, 24, false);
	this->lbValues->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspCtrl = ui->NewHSplitter(this->pnlMain, 3, false);
	this->pnlCtrl = ui->NewPanel(this->pnlMain);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblValueCase = ui->NewLabel(this->pnlCtrl, CSTR("Value Case"));
	this->lblValueCase->SetRect(4, 4, 100, 23, false);
	this->cboValueCase = ui->NewComboBox(this->pnlCtrl, false);
	this->cboValueCase->SetRect(104, 4, 200, 23, false);
	this->lblOutputCase = ui->NewLabel(this->pnlCtrl, CSTR("Output Case"));
	this->lblOutputCase->SetRect(4, 28, 100, 23, false);
	this->cboOutputCase = ui->NewComboBox(this->pnlCtrl, false);
	this->cboOutputCase->SetRect(104, 28, 200, 23, false);
	this->lblOutputFormat = ui->NewLabel(this->pnlCtrl, CSTR("Output Format"));
	this->lblOutputFormat->SetRect(4, 52, 100, 23, false);
	this->cboOutputFormat = ui->NewComboBox(this->pnlCtrl, false);
	this->cboOutputFormat->SetRect(104, 52, 200, 23, false);

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Column")), colIndex);

	NN<DB::DBReader> r;
	if (this->db->QueryTableData(schema, table, 0, 0, 0, 0, 0).SetTo(r))
	{
		NN<Text::String> s;
		Data::FastStringMap<Int32> nameMap;
		r->GetName(colIndex, sbuff).SetTo(sptr);
		while (r->ReadNext())
		{
			if (r->GetNewStr(colIndex).SetTo(s))
			{
				if (nameMap.GetNN(s) == 0)
				{
					nameMap.PutNN(s, 1);
				}
				s->Release();
			}
		}
		this->db->CloseReader(r);

		UOSInt i = nameMap.GetCount();
		while (i-- > 0)
		{
			if (nameMap.GetKey(i).SetTo(s))
			{
				s = s->Clone();
				s->Trim();
				this->nameList.Add(s);
			}
		}
		Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(this->nameList, this->nameList);
		UOSInt j = this->nameList.GetCount();
		i = 0;
		while (i < j)
		{
			this->lbValues->AddItem(this->nameList.GetItemNoCheck(i)->ToCString(), 0);
			i++;
		}
	}
	this->colName = Text::String::NewP(sbuff, sptr);

	ADDITEM(this->cboValueCase, Text::CaseType::CamelCase);
	ADDITEM(this->cboValueCase, Text::CaseType::PascalCase);
	ADDITEM(this->cboValueCase, Text::CaseType::SnakeCase);
	ADDITEM(this->cboValueCase, Text::CaseType::KebabCase);
	ADDITEM(this->cboValueCase, Text::CaseType::ScreamingSnakeCase);
	ADDITEM(this->cboValueCase, Text::CaseType::UpperCase);
	ADDITEM(this->cboValueCase, Text::CaseType::LowerCase);
	ADDITEM(this->cboValueCase, Text::CaseType::BlockCase);

	ADDITEM(this->cboOutputCase, Text::CaseType::CamelCase);
	ADDITEM(this->cboOutputCase, Text::CaseType::PascalCase);
	ADDITEM(this->cboOutputCase, Text::CaseType::SnakeCase);
	ADDITEM(this->cboOutputCase, Text::CaseType::KebabCase);
	ADDITEM(this->cboOutputCase, Text::CaseType::ScreamingSnakeCase);
	ADDITEM(this->cboOutputCase, Text::CaseType::UpperCase);
	ADDITEM(this->cboOutputCase, Text::CaseType::LowerCase);
	ADDITEM(this->cboOutputCase, Text::CaseType::BlockCase);

	this->cboOutputFormat->AddItem(CSTR("Typescript definition (.d.ts)"), 0);
	this->cboOutputFormat->AddItem(CSTR("Javascript module"), 0);
	this->cboValueCase->SetSelectedIndex(0);
	this->cboOutputCase->SetSelectedIndex(1);
	this->cboOutputFormat->SetSelectedIndex(0);
	this->Generate();
	this->cboValueCase->HandleSelectionChange(OnUpdate, this);
	this->cboOutputCase->HandleSelectionChange(OnUpdate, this);
	this->cboOutputFormat->HandleSelectionChange(OnUpdate, this);
}

SSWR::AVIRead::AVIRDBGenEnumForm::~AVIRDBGenEnumForm()
{
	this->colName->Release();
	this->nameList.FreeAll();
}

void SSWR::AVIRead::AVIRDBGenEnumForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
