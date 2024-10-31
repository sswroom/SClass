#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ColDef.h"
#include "DB/DBExporter.h"
#include "DB/JavaDBUtil.h"
#include "DB/SQLGenerator.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRChartForm.h"
#include "SSWR/AVIRead/AVIRDBCheckChgForm.h"
#include "SSWR/AVIRead/AVIRDBExportForm.h"
#include "SSWR/AVIRead/AVIRDBForm.h"
#include "SSWR/AVIRead/AVIRDBGenEnumForm.h"
#include "SSWR/AVIRead/AVIRLineChartForm.h"
#include "Text/CharUtil.h"
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"

//#define VERBOSE
#if defined(VERBOSE)
#include "IO/ConsoleLogHandler.h"
#endif
#define MAX_ROW_CNT 1000

typedef enum
{
	MNU_FILE_SAVE = 100,
	MNU_TABLE_CPP_HEADER,
	MNU_TABLE_CPP_SOURCE,
	MNU_TABLE_JAVA,
	MNU_TABLE_CREATE_MYSQL,
	MNU_TABLE_CREATE_MYSQL8,
	MNU_TABLE_CREATE_MSSQL,
	MNU_TABLE_CREATE_POSTGRESQL,
	MNU_TABLE_EXPORT_MYSQL,
	MNU_TABLE_EXPORT_MYSQL8,
	MNU_TABLE_EXPORT_MSSQL,
	MNU_TABLE_EXPORT_POSTGRESQL,
	MNU_TABLE_EXPORT_OPTION,
	MNU_TABLE_EXPORT_CSV,
	MNU_TABLE_EXPORT_SQLITE,
	MNU_TABLE_EXPORT_HTML,
	MNU_TABLE_EXPORT_PLIST,
	MNU_TABLE_EXPORT_EXCELXML,
	MNU_TABLE_EXPORT_XLSX,
	MNU_TABLE_CHECK_CHANGE,
	MNU_COLUMN_GEN_ENUM,
	MNU_CHART_LINE,
	MNU_DATABASE_START = 1000
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRDBForm::OnSchemaSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBForm>();
	me->UpdateTables();
}

void __stdcall SSWR::AVIRead::AVIRDBForm::OnTableSelChg(AnyType userObj)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRDBForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBForm>();
	if (!me->lbTable->GetSelectedItemText(sbuff).SetTo(sptr))
	{
		me->lvTable->ClearItems();
		return;
	}
	Optional<Text::String> schemaName = me->lbSchema->GetSelectedItemTextNew();
	

	Optional<DB::TableDef> tabDef = 0;
	NN<DB::TableDef> nntabDef;
	Optional<DB::DBReader> tmpr;
	NN<DB::DBReader> r;
	if (me->dbt)
	{
		tabDef = me->dbt->GetTableDef(OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr));

		tmpr = me->db->QueryTableData(OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0);
	}
	else
	{
		tabDef = me->db->GetTableDef(OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr));

		tmpr = me->db->QueryTableData(OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0);
		if (tmpr.SetTo(r) && tabDef.IsNull())
		{
			tabDef = r->GenTableDef(OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr));
		}
	}
	OPTSTR_DEL(schemaName);
	if (tmpr.SetTo(r))
	{
		me->UpdateResult(r);

		me->lvTable->ClearItems();
		NN<Text::String> s;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		if (tabDef.SetTo(nntabDef))
		{
			NN<DB::ColDef> col;
			Data::ArrayIterator<NN<DB::ColDef>> it = nntabDef->ColIterator();
			while (it.HasNext())
			{
				col = it.Next();
				k = me->lvTable->AddItem(col->GetColName(), 0);
				sptr = col->ToColTypeStr(sbuff);
				me->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (col->GetNativeType().SetTo(s))
					me->lvTable->SetSubItem(k, 2, s);
				me->lvTable->SetSubItem(k, 3, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				me->lvTable->SetSubItem(k, 4, col->IsPK()?CSTR("PK"):CSTR(""));
				me->lvTable->SetSubItem(k, 5, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col->GetDefVal().SetTo(s))
					me->lvTable->SetSubItem(k, 6, s);
				if (col->GetAttr().SetTo(s))
					me->lvTable->SetSubItem(k, 7, s);
			}
			tabDef.Delete();
		}
		else
		{
			DB::ColDef col(Text::String::NewEmpty());
			j = r->ColCount();
			i = 0;
			while (i < j)
			{
				r->GetColDef(i, col);
				k = me->lvTable->AddItem(col.GetColName(), 0);
				sptr = col.ToColTypeStr(sbuff);
				me->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (col.GetNativeType().SetTo(s))
					me->lvTable->SetSubItem(k, 2, s);
				me->lvTable->SetSubItem(k, 3, col.IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				me->lvTable->SetSubItem(k, 4, col.IsPK()?CSTR("PK"):CSTR(""));
				me->lvTable->SetSubItem(k, 5, col.IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col.GetDefVal().SetTo(s))
					me->lvTable->SetSubItem(k, 6, s);
				if (col.GetAttr().SetTo(s))
					me->lvTable->SetSubItem(k, 7, s);

				i++;
			}
		}

		me->db->CloseReader(r);
	}
	else
	{
		tabDef.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRDBForm::OnSQLClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSQL->GetText(sb);
	if (sb.GetLength() > 0)
	{
		NN<DB::DBReader> r;
		if (me->dbt->ExecuteReader(sb.ToCString()).SetTo(r))
		{
			me->UpdateResult(r);
			me->dbt->CloseReader(r);
		}
		else
		{
			sb.ClearStr();
			me->dbt->GetLastErrorMsg(sb);
			me->ui->ShowMsgOK(sb.ToCString(), CSTR("Database"), me);
		}
	}
}

void SSWR::AVIRead::AVIRDBForm::UpdateResult(NN<DB::DBReader> r)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt *colSize;

	this->lvResult->ClearAll();

	Text::StringBuilderUTF8 sb;
	{
		DB::ColDef col(CSTR(""));
		j = r->ColCount();
		this->lvResult->ChangeColumnCnt(j);
		i = 0;
		colSize = MemAlloc(UOSInt, j);
		while (i < j)
		{
			if (r->GetColDef(i, col))
			{
				this->lvResult->AddColumn(col.GetColName(), 100);
			}
			else
			{
				this->lvResult->AddColumn(CSTR("Unnamed"), 100);
			}
			colSize[i] = 0;
			i++;
		}
	}

	OSInt rowCnt = 0;
	while (r->ReadNext())
	{
		i = 1;
		sb.ClearStr();
		r->GetStr(0, sb);
		if (sb.GetLength() > colSize[0])
			colSize[0] = sb.GetLength();
		k = this->lvResult->AddItem(sb.ToCString(), 0);
		while (i < j)
		{
			sb.ClearStr();
			r->GetStr(i, sb);
			this->lvResult->SetSubItem(k, i, sb.ToCString());

			if (sb.GetLength() > colSize[i])
				colSize[i] = sb.GetLength();
			i++;
		}
		rowCnt++;
		if (rowCnt > MAX_ROW_CNT)
			break;
	}

	k = 0;
	i = j;
	while (i-- > 0)
	{
		k += colSize[i];
	}
	if (k > 0)
	{
		Double w = this->lvResult->GetSize().x;
		w -= UOSInt2Double(20 + j * 6);
		if (w < 0)
			w = 0;
		i = 0;
		while (i < j)
		{
			this->lvResult->SetColumnWidth(i, (UOSInt2Double(colSize[i]) * w / UOSInt2Double(k) + 6));
			i++;
		}
	}
	MemFree(colSize);
}

void SSWR::AVIRead::AVIRDBForm::CopyTableCreate(DB::SQLType sqlType, Bool axisAware)
{
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		DB::SQLBuilder sql(sqlType, axisAware, 0);
		NN<DB::TableDef> tabDef;
		if (this->db->GetTableDef(OPTSTR_CSTR(schemaName), tableName->ToCString()).SetTo(tabDef))
		{
			if (!DB::SQLGenerator::GenCreateTableCmd(sql, OPTSTR_CSTR(schemaName), tableName->ToCString(), tabDef, true))
			{
				this->ui->ShowMsgOK(CSTR("Error in generating Create SQL command"), CSTR("DB Manager"), this);
			}
			else
			{
				UI::Clipboard::SetString(this->GetHandle(), sql.ToCString());
			}
			tabDef.Delete();
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in getting table definition"), CSTR("DB Manager"), this);
		}
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBForm::ExportTableData(DB::SQLType sqlType, Bool axisAware)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sql"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExportTable", true);
		dlg->AddFilter(CSTR("*.sql"), CSTR("SQL File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateInsertSQLs(this->db, sqlType, axisAware, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs))
			{
				this->ui->ShowMsgOK(CSTR("Error in reading table data"), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBForm::ExportTableCSV()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExportCSV", true);
		dlg->AddFilter(CSTR("*.csv"), CSTR("Comma-Seperated-Value File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateCSV(this->db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, CSTR("\"\""), fs, 65001))
			{
				this->ui->ShowMsgOK(CSTR("Error in reading table data"), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBForm::ExportTableSQLite()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sqlite"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExportSQLite", true);
		dlg->AddFilter(CSTR("*.sqlite"), CSTR("SQLite File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			Text::StringBuilderUTF8 sb;
			DB::SQLiteFile sqlite(dlg->GetFileName());
			if (!DB::DBExporter::GenerateSQLite(this->db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, sqlite, &sb))
			{
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBForm::ExportTableHTML()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".html"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExportHTML", true);
		dlg->AddFilter(CSTR("*.html"), CSTR("HTML File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateHTML(this->db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, 65001))
			{
				this->ui->ShowMsgOK(CSTR("Error in exporting as PList"), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBForm::ExportTablePList()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".plist"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExportPList", true);
		dlg->AddFilter(CSTR("*.plist"), CSTR("PList File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GeneratePList(this->db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, 65001))
			{
				this->ui->ShowMsgOK(CSTR("Error in exporting as PList"), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBForm::ExportTableXLSX()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".xlsx"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExportXLSX", true);
		dlg->AddFilter(CSTR("*.xlsx"), CSTR("Excel 2007 File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			Text::StringBuilderUTF8 sb;
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateXLSX(this->db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, &sb))
			{
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBForm::ExportTableExcelXML()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".xml"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExportXML", true);
		dlg->AddFilter(CSTR("*.xml"), CSTR("Excel XML File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			Text::StringBuilderUTF8 sb;
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateExcelXML(this->db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, &sb))
			{
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

SSWR::AVIRead::AVIRDBForm::AVIRDBForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Bool needRelease) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(0, 0, 8.25, false);
	sptr = db->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Database - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->db = db;
	this->needRelease = needRelease;
	this->dbt = 0;
	this->debugWriter = 0;
	this->logHdlr = 0;
	this->currCond = 0;
	if (db->IsFullConn())
	{
#if defined(VERBOSE)
		IO::ConsoleWriter *console;
		IO::ConsoleLogHandler *logHdlr;
		NEW_CLASS(console, IO::ConsoleWriter());
		NEW_CLASS(logHdlr, IO::ConsoleLogHandler(console));
		this->log.AddLogHandler(logHdlr, IO::LogHandler::LogLevel::Raw);
		this->logHdlr = logHdlr;
		this->debugWriter = console;
#endif
		NEW_CLASS(this->dbt, DB::ReadingDBTool(NN<DB::DBConn>::ConvertFrom(this->db), needRelease, this->log, CSTR("DB: ")));
	}

	this->tcDB = ui->NewTabControl(*this);
	this->tcDB->SetRect(0, 0, 100, 400, false);
	this->tcDB->SetDockType(UI::GUIControl::DOCK_TOP);
	this->vspDB = ui->NewVSplitter(*this, 3, false);
	this->lvResult = ui->NewListView(*this, UI::ListViewStyle::Table, 1);
	this->lvResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResult->SetFullRowSelect(true);
	this->lvResult->SetShowGrid(true);

	this->tpTable = this->tcDB->AddTabPage(CSTR("Tables"));
	this->lbSchema = ui->NewListBox(this->tpTable, false);
	this->lbSchema->SetRect(0, 0, 150, 100, false);
	this->lbSchema->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSchema->HandleSelectionChange(OnSchemaSelChg, this);
	this->hspSchema = ui->NewHSplitter(this->tpTable, 3, false);
	this->lbTable = ui->NewListBox(this->tpTable, false);
	this->lbTable->SetRect(0, 0, 150, 100, false);
	this->lbTable->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTable->HandleSelectionChange(OnTableSelChg, this);
	this->hspTable = ui->NewHSplitter(this->tpTable, 3, false);
	this->lvTable = ui->NewListView(this->tpTable, UI::ListViewStyle::Table, 8);
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->AddColumn(CSTR("Name"), 200);
	this->lvTable->AddColumn(CSTR("Type"), 100);
	this->lvTable->AddColumn(CSTR("NType"), 100);
	this->lvTable->AddColumn(CSTR("Null?"), 50);
	this->lvTable->AddColumn(CSTR("PK?"), 30);
	this->lvTable->AddColumn(CSTR("Auto_Inc"), 100);
	this->lvTable->AddColumn(CSTR("Default Val"), 100);
	this->lvTable->AddColumn(CSTR("Attribute"), 100);

	if (this->dbt)
	{
		this->tpSQL = this->tcDB->AddTabPage(CSTR("SQL"));
		this->pnlSQLCtrl = ui->NewPanel(this->tpSQL);
		this->pnlSQLCtrl->SetRect(0, 0, 100, 31, false);
		this->pnlSQLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		this->btnSQL = ui->NewButton(this->pnlSQLCtrl, CSTR("Execute"));
		this->btnSQL->SetRect(4, 4, 75, 23, false);
		this->btnSQL->HandleButtonClick(OnSQLClicked, this);
		this->txtSQL = ui->NewTextBox(this->tpSQL, CSTR(""), true);
		this->txtSQL->SetDockType(UI::GUIControl::DOCK_FILL);
	}

	NN<UI::GUIMenu> mnu;
	NN<UI::GUIMenu> mnu2;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Save as"), MNU_FILE_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Table"));
	mnu->AddItem(CSTR("Copy as CPP Header"), MNU_TABLE_CPP_HEADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Copy as CPP Source"), MNU_TABLE_CPP_SOURCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Copy as Java Entity"), MNU_TABLE_JAVA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Copy as Create SQL"));
	mnu2->AddItem(CSTR("MySQL"), MNU_TABLE_CREATE_MYSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("MySQL8"), MNU_TABLE_CREATE_MYSQL8, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SQL Server"), MNU_TABLE_CREATE_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("PostgreSQL"), MNU_TABLE_CREATE_POSTGRESQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Export Table Data as"));
	mnu2->AddItem(CSTR("SQL (MySQL)"), MNU_TABLE_EXPORT_MYSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SQL (MySQL8)"), MNU_TABLE_EXPORT_MYSQL8, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SQL (SQL Server)"), MNU_TABLE_EXPORT_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SQL (PostgreSQL)"), MNU_TABLE_EXPORT_POSTGRESQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SQL..."), MNU_TABLE_EXPORT_OPTION, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("CSV"), MNU_TABLE_EXPORT_CSV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SQLite"), MNU_TABLE_EXPORT_SQLITE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("HTML"), MNU_TABLE_EXPORT_HTML, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("PList"), MNU_TABLE_EXPORT_PLIST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Excel 2007"), MNU_TABLE_EXPORT_XLSX, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Excel XML"), MNU_TABLE_EXPORT_EXCELXML, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Check Table Changes"), MNU_TABLE_CHECK_CHANGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("C&olumn"));
	mnu->AddItem(CSTR("&Generate Enum"), MNU_COLUMN_GEN_ENUM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Chart"));
	mnu->AddItem(CSTR("&Line Chart"), MNU_CHART_LINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	if (this->dbt && this->dbt->GetDatabaseNames(this->dbNames) > 0)
	{
		mnu = this->mnuMain->AddSubMenu(CSTR("&Database"));
		Data::ArrayIterator<NN<Text::String>> it = this->dbNames.Iterator();
		UOSInt i = 0;
		while (it.HasNext())
		{
			NN<Text::String> dbName = it.Next();
			mnu->AddItem(dbName->ToCString(), (UInt16)(MNU_DATABASE_START + i), UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
			i++;
		}
	}
	this->SetMenu(this->mnuMain);

	this->UpdateSchemas();
}

SSWR::AVIRead::AVIRDBForm::~AVIRDBForm()
{
	if (this->dbt)
	{
		this->dbt->ReleaseDatabaseNames(this->dbNames);
		DEL_CLASS(this->dbt);
	}
	else if (this->needRelease)
	{
		this->db.Delete();
	}
	SDEL_CLASS(this->currCond);
	NN<IO::LogHandler> logHdlr;
	if (logHdlr.Set(this->logHdlr))
	{
		this->log.RemoveLogHandler(logHdlr);	
		SDEL_CLASS(this->logHdlr);
	}
	SDEL_CLASS(this->debugWriter);
}

void SSWR::AVIRead::AVIRDBForm::UpdateSchemas()
{
	Data::ArrayListStringNN schemaNames;
	UOSInt i;
	UOSInt j;

	this->lbSchema->ClearItems();
	if (this->dbt)
	{
		this->dbt->QuerySchemaNames(schemaNames);
	}
	else
	{
		this->db->QuerySchemaNames(schemaNames);
	}
	if (schemaNames.GetCount() == 0)
	{
		this->lbSchema->AddItem(CSTR(""), 0);
	}
	i = 0;
	j = schemaNames.GetCount();
	while (i < j)
	{
		this->lbSchema->AddItem(Text::String::OrEmpty(schemaNames.GetItem(i)), 0);
		i++;
	}

	schemaNames.FreeAll();
	this->lbSchema->SetSelectedIndex(0);
}

void SSWR::AVIRead::AVIRDBForm::UpdateTables()
{
	Text::StringBuilderUTF8 sb;
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	Data::ArrayListStringNN tableNames;
	UOSInt i;
	UOSInt j;

	this->lbTable->ClearItems();
	if (this->dbt)
	{
		this->dbt->QueryTableNames(OPTSTR_CSTR(schemaName), tableNames);
	}
	else
	{
		this->db->QueryTableNames(OPTSTR_CSTR(schemaName), tableNames);
	}
	OPTSTR_DEL(schemaName);
	i = 0;
	j = tableNames.GetCount();
	while (i < j)
	{
		this->lbTable->AddItem(Text::String::OrEmpty(tableNames.GetItem(i)), 0);
		i++;
	}

	tableNames.FreeAll();
}

void SSWR::AVIRead::AVIRDBForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	if (cmdId >= MNU_DATABASE_START)
	{
		if (this->dbt->ChangeDatabase(Text::String::OrEmpty(this->dbNames.GetItem((UOSInt)cmdId - MNU_DATABASE_START))->ToCString()))
		{
			this->UpdateTables();
		}
		return;
	}
	switch (cmdId)
	{
	case MNU_FILE_SAVE:
		this->core->SaveData(this, this->db, L"DBSave");
		break;
	case MNU_CHART_LINE:
		if (this->lbTable->GetSelectedItemText(sbuff).SetTo(sptr))
		{
			Data::Chart *chart = 0;
			{
				Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
				SSWR::AVIRead::AVIRLineChartForm frm(0, this->ui, this->core, this->db.Ptr(), OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr));
				OPTSTR_DEL(schemaName);
				if (frm.ShowDialog(this) == DR_OK)
				{
					chart = frm.GetChart();
				}
			}
			if (chart)
			{
				NN<SSWR::AVIRead::AVIRChartForm> chartFrm;
				NEW_CLASSNN(chartFrm, SSWR::AVIRead::AVIRChartForm(0, this->ui, this->core, chart));
				this->core->ShowForm(chartFrm);
			}
		}
		break;
	case MNU_TABLE_CPP_HEADER:
		if (this->lbTable->GetSelectedItemText(sbuff).SetTo(sptr))
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Data::Class> cls;
			if (DB::DBExporter::CreateTableClass(this->db, OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr)).SetTo(cls))
			{
				Text::PString hdr = {sbuff2, 0};
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
				hdr.leng = (UOSInt)(DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]) - sbuff2);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassHeader(&hdr, 0, sb);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
				cls.Delete();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_CPP_SOURCE:
		if (this->lbTable->GetSelectedItemText(sbuff).SetTo(sptr))
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Data::Class> cls;
			if (DB::DBExporter::CreateTableClass(this->db, OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr)).SetTo(cls))
			{
				Text::PString hdr = {sbuff2, 0};
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
				hdr.leng = (UOSInt)(DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]) - sbuff2);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassSource(0, &hdr, 0, sb);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
				cls.Delete();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_JAVA:
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Text::String> tableName;
			if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
			{
				Text::StringBuilderUTF8 sb;
				DB::JavaDBUtil::ToJavaEntity(sb, schemaName, tableName, 0, this->db);
				tableName->Release();
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_CREATE_MYSQL:
		this->CopyTableCreate(DB::SQLType::MySQL, false);
		break;
	case MNU_TABLE_CREATE_MYSQL8:
		this->CopyTableCreate(DB::SQLType::MySQL, true);
		break;
	case MNU_TABLE_CREATE_MSSQL:
		this->CopyTableCreate(DB::SQLType::MSSQL, false);
		break;
	case MNU_TABLE_CREATE_POSTGRESQL:
		this->CopyTableCreate(DB::SQLType::PostgreSQL, false);
		break;
	case MNU_TABLE_EXPORT_MYSQL:
		this->ExportTableData(DB::SQLType::MySQL, false);
		break;
	case MNU_TABLE_EXPORT_MYSQL8:
		this->ExportTableData(DB::SQLType::MySQL, true);
		break;
	case MNU_TABLE_EXPORT_MSSQL:
		this->ExportTableData(DB::SQLType::MSSQL, false);
		break;
	case MNU_TABLE_EXPORT_POSTGRESQL:
		this->ExportTableData(DB::SQLType::PostgreSQL, false);
		break;
	case MNU_TABLE_EXPORT_OPTION:
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Text::String> tableName;
			if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
			{
				SSWR::AVIRead::AVIRDBExportForm dlg(0, ui, this->core, this->db, OPTSTR_CSTR(schemaName), tableName->ToCString());
				dlg.ShowDialog(this);
				tableName->Release();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_EXPORT_CSV:
		this->ExportTableCSV();
		break;
	case MNU_TABLE_EXPORT_SQLITE:
		this->ExportTableSQLite();
		break;
	case MNU_TABLE_EXPORT_HTML:
		this->ExportTableHTML();
		break;
	case MNU_TABLE_EXPORT_PLIST:
		this->ExportTablePList();
		break;
	case MNU_TABLE_EXPORT_EXCELXML:
		this->ExportTableExcelXML();
		break;
	case MNU_TABLE_EXPORT_XLSX:
		this->ExportTableXLSX();
		break;
	case MNU_TABLE_CHECK_CHANGE:
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Text::String> tableName;
			if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
			{
				SSWR::AVIRead::AVIRDBCheckChgForm dlg(0, ui, this->core, this->db, OPTSTR_CSTR(schemaName), tableName->ToCString());
				dlg.ShowDialog(this);
				tableName->Release();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_COLUMN_GEN_ENUM:
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Text::String> tableName;
			if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
			{
				UOSInt colIndex = this->lvTable->GetSelectedIndex();
				if (colIndex != INVALID_INDEX)
				{
					SSWR::AVIRead::AVIRDBGenEnumForm dlg(0, ui, this->core, this->db, OPTSTR_CSTR(schemaName), tableName->ToCString(), colIndex);
					dlg.ShowDialog(this);
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("Please select column first"), CSTR("Database"), this);
				}
				tableName->Release();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRDBForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
