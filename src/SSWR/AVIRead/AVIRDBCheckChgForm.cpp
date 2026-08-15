#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/CSVFile.h"
#include "DB/DBChangeChecker.h"
#include "DB/DBConn.h"
#include "DB/DBTool.h"
#include "DB/TableDef.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRDBAssignColumnForm.h"
#include "SSWR/AVIRead/AVIRDBCheckChgForm.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataFileClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	Text::StringBuilderUTF8 sb;
	me->txtDataFile->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBCheckChg", false);
	dlg->SetAllowMultiSel(false);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::ReadingDB);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->inited = false;
		me->LoadDataFile(dlg->GetFileName()->ToCString());
		me->inited = true;
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	UIntOS i = 0;
	UIntOS nFiles = files.GetCount();
	me->inited = false;
	while (i < nFiles)
	{
		if (me->LoadDataFile(files[i]->ToCString()))
		{
			return;
		}
		i++;
	}
	me->inited = true;
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataCheckClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	if (!me->CheckDataFile())
	{
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnSQLClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (me->dataConn.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Please input Data file first"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::SQLType sqlType = (DB::SQLType)me->cboDBType->GetSelectedItem().GetIntOS();
	Bool axisAware = me->chkAxisAware->IsChecked();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBCheckChgSQL", true);
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	sptr = sbuff;
	Text::CStringNN schema;
	if (me->schema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
	{
		sptr = schema.ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = me->table.ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".sql"));
	dlg->SetFileName(CSTRP(sbuff, sptr));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		Bool succ;
		SQLSession sess;
		sess.mode = 0;
		sess.totalCnt = 0;
		sess.startTime = Data::Timestamp::UtcNow();
		sess.lastUpdateTime = sess.startTime;
		sess.db = nullptr;
		
		{
			IO::FileStream fs(dlg->GetFileName()->ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			sess.stm = fs;
			succ = me->GenerateSQL(sqlType, axisAware, sess);
		}
		Double t = Data::Timestamp::UtcNow().DiffSecDbl(sess.startTime);
		sptr = Text::StrDouble(sbuff, t);
		me->txtStatTime->SetText(CSTRP(sbuff, sptr));
		if (succ)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Success, "));
			sb.AppendUIntOS(sess.totalCnt);
			sb.AppendC(UTF8STRC(" SQL generated"));
			me->txtStatus->SetText(sb.ToCString());
		}
		else
		{
			IO::Path::DeleteFile(dlg->GetFileName()->v);
			me->txtStatus->SetText(CSTR("Error in generating SQL"));
		}
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnExecuteClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	if (me->dataConn.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Please load data file first"), CSTR("Check Table Changes"), me);
		return;
	}
	NN<DB::DBConn> db;
	if (me->db->IsFullConn())
	{
		db = NN<DB::DBConn>::ConvertFrom(me->db);
	}
	else if (me->db->IsDBTool())
	{
		db = NN<DB::ReadingDBTool>::ConvertFrom(me->db)->GetDBConn();
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Connection does not support SQL Execution"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::SQLType sqlType = (DB::SQLType)me->cboDBType->GetSelectedItem().GetIntOS();
	Bool axisAware = me->chkAxisAware->IsChecked();
	Bool succ;
	SQLSession sess;
	sess.totalCnt = 0;
	sess.startTime = Data::Timestamp::UtcNow();
	sess.lastUpdateTime = sess.startTime;
	sess.stm = nullptr;
	sess.db = db;
	if (me->chkMultiRow->IsChecked())
	{
		Text::StringBuilderUTF8 sbInsert;
		sess.mode = 2;
		sess.nInsert = 0;
		sess.sbInsert = sbInsert;
		succ = me->GenerateSQL(sqlType, axisAware, sess);
		if (succ && sess.nInsert > 0)
		{
			if (db->ExecuteNonQuery(sess.sbInsert->ToCString()) >= 0)
			{
				sess.totalCnt += sess.nInsert;
				sess.nInsert = 0;
			}
			else
			{
				succ = false;
			}
		}
	}
	else
	{
		sess.mode = 1;
		succ = me->GenerateSQL(sqlType, axisAware, sess);
	}
	Double t = Data::Timestamp::UtcNow().DiffSecDbl(sess.startTime);
	Text::StringBuilderUTF8 sb;
	sb.AppendDouble(t);
	me->txtStatTime->SetText(sb.ToCString());
	if (succ)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Success, "));
		sb.AppendUIntOS(sess.totalCnt);
		sb.AppendC(UTF8STRC(" SQL executed"));
		me->txtStatus->SetText(sb.ToCString());
	}
	else
	{
		me->txtStatus->SetText(CSTR("Error in executing SQL"));
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataTableChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	NN<DB::ReadingDB> dataConn;
	if (!me->dataConn.SetTo(dataConn))
	{
		//me->ui->ShowMsgOK(CSTR("Please load data file first"), CSTR("Check Table Changes"), me);
		return;
	}
	Text::CString schema = nullptr;
	Text::StringBuilderUTF8 sbSchema;
	if (me->cboDataSchema->GetCount() > 0)
	{
		if (me->cboDataSchema->GetText(sbSchema))
		{
			schema = sbSchema.ToCString();
		}
	}
	Text::StringBuilderUTF8 sbTable;
	if (!me->cboDataTable->GetText(sbTable))
	{
		//me->ui->ShowMsgOK(CSTR("Error in getting table name"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::DBChangeChecker checker(me->db, me->schema, me->table);
	checker.SetDataTable(dataConn, schema, sbTable.ToCString(), 0, me->dataFileNoHeader);
	checker.InitColMapping(me->colInd);
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnAssignColClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	NN<DB::ReadingDB> dataFile;
	if (!me->dataConn.SetTo(dataFile))
	{
		//me->ui->ShowMsgOK(CSTR("Please load data file first"), CSTR("Check Table Changes"), me);
		return;
	}
	Text::StringBuilderUTF8 sbTable;
	if (!me->cboDataTable->GetText(sbTable))
	{
		//me->ui->ShowMsgOK(CSTR("Error in getting table name"), CSTR("Check Table Changes"), me);
		return;
	}
	Text::CString dataSchema = nullptr;
	Text::StringBuilderUTF8 sbSchema;
	if (me->cboDataSchema->GetCount() > 0)
	{
		if (me->cboDataSchema->GetText(sbSchema))
		{
			dataSchema = sbSchema.ToCString();
		}
	}
	NN<DB::TableDef> table;
	if (!me->db->GetTableDef(me->schema, me->table).SetTo(table))
	{
		return;
	}
	SSWR::AVIRead::AVIRDBAssignColumnForm frm(nullptr, me->ui, me->core, table, dataFile, dataSchema, sbTable.ToCString(), me->dataFileNoHeader, me->dataTz, me->colInd, me->colStr);
	frm.ShowDialog(me);
	table.Delete();
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataConnSelChg(AnyType userObj, Bool newState)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	if (me->inited)
	{
		if (newState)
		{
			me->relDataFile.Delete();
			me->cboNullCol->SetSelectedIndex(3);
			me->dataConn = me->dbMgr->OpenDataSource(me->cboDataConn->GetSelectedIndex());
			NN<DB::ReadingDB> conn;
			if (me->dataConn.SetTo(conn))
			{
				me->InitDataConn(conn, conn->GetTzQhr());
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			me->txtDataFile->GetText(sb);
			me->inited = false;
			me->LoadDataFile(sb.ToCString());
			me->inited = true;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataConnCboSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	if (me->inited)
	{
		me->relDataFile.Delete();
		me->cboNullCol->SetSelectedIndex(3);
		me->dataConn = me->dbMgr->OpenDataSource(me->cboDataConn->GetSelectedIndex());
		me->dataFileNoHeader = false;
		NN<DB::ReadingDB> conn;
		if (me->dataConn.SetTo(conn))
		{
			me->inited = false;
			me->radDataConn->Select();
			me->InitDataConn(conn, conn->GetTzQhr());
			me->inited = true;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataSchemaSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCheckChgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCheckChgForm>();
	if (me->inited)
	{
		Text::StringBuilderUTF8 sb;
		me->cboDataSchema->GetText(sb);
		me->InitSchema(sb.ToCString());
	}
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::LoadDataFile(Text::CStringNN fileName)
{
	NN<DB::TableDef> table;
	if (!this->db->GetTableDef(this->schema, this->table).SetTo(table))
	{
		this->ui->ShowMsgOK(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	table.Delete();
	Int8 csvTZ = this->db->GetTzQhr();
	Bool noHeader = this->chkNoHeader->IsChecked();
	NN<DB::ReadingDB> nndataFile;
	if (fileName.EndsWithICase(UTF8STRC(".csv")))
	{
		NN<DB::CSVFile> csv;
		NEW_CLASSNN(csv, DB::CSVFile(fileName, 65001))
		if (noHeader) csv->SetNoHeader(true);
		NN<DB::DBReader> r;
		if (!csv->QueryTableData(nullptr, CSTR(""), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
		{
			csv.Delete();
			this->ui->ShowMsgOK(CSTR("Error in reading CSV file"), CSTR("Check Table Changes"), this);
			this->dataConn = nullptr;
			return false;
		}
		csv->CloseReader(r);
		if (this->chkCSVUTCTime->IsChecked())
			csvTZ = 0;
		this->txtDataFile->SetText(fileName);
		this->radDataFile->Select();
		this->relDataFile.Delete();
		this->relDataFile = csv;
		this->dataFileNoHeader = noHeader;
		this->cboNullCol->SetSelectedIndex(0);
		return this->InitDataConn(csv, csvTZ);
	}
	else
	{
		IO::Path::PathType pt = IO::Path::GetPathType(fileName);
		Optional<DB::ReadingDB> db = nullptr;
		if (pt == IO::Path::PathType::File)
		{
			IO::StmData::FileData fd(fileName, false);
			db = Optional<DB::ReadingDB>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ReadingDB));
		}
		else if (pt == IO::Path::PathType::Directory)
		{
			IO::DirectoryPackage pkg(fileName);
			NN<Parser::ParserList> parsers = this->core->GetParserList();
			db = Optional<DB::ReadingDB>::ConvertFrom(parsers->ParseObjectType(pkg, IO::ParserType::ReadingDB));
		}
		if (db.SetTo(nndataFile))
		{
			this->txtDataFile->SetText(fileName);
			this->radDataFile->Select();
			this->relDataFile.Delete();
			this->relDataFile = nndataFile;
			this->dataFileNoHeader = false;
			this->cboNullCol->SetSelectedIndex(3);
			return this->InitDataConn(nndataFile, csvTZ);
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("Check Table Changes"), this);
			this->dataConn = nullptr;
			return false;
		}
	}
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::InitDataConn(NN<DB::ReadingDB> conn, Int8 connTz)
{
	this->dataTz = connTz;
	this->dataConn = conn;
	this->dataFileNoHeader = false;
	Data::ArrayListStringNN schemaNames;
	Optional<Text::String> schemaName = nullptr;
	conn->QuerySchemaNames(schemaNames);
	this->cboDataSchema->ClearItems();
	if (schemaNames.GetCount() == 0)
	{
		this->cboDataSchema->SetEnabled(false);
	}
	else
	{
		this->cboDataSchema->SetEnabled(true);
		UIntOS i = 0;
		UIntOS j = schemaNames.GetCount();
		while (i < j)
		{
			this->cboDataSchema->AddItem(schemaNames.GetItemNoCheck(i)->ToCString(), 0);
			i++;
		}
		schemaName = schemaNames.GetItemNoCheck(0);
		this->cboDataSchema->SetSelectedIndex(0);
	}
	Bool succ = InitSchema(OPTSTR_CSTR(schemaName));
	schemaNames.FreeAll();
	return succ;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::InitSchema(Text::CString schema)
{
	NN<DB::ReadingDB> conn;
	if (!this->dataConn.SetTo(conn))
	{
		return false;
	}
	Data::ArrayListStringNN tableNames;
	conn->QueryTableNames(schema, tableNames);
	this->cboDataTable->ClearItems();
	if (tableNames.GetCount() > 0)
	{
		UIntOS tableIndex = 0;
		NN<Text::String> name;
		Data::ArrayIterator<NN<Text::String>> it = tableNames.Iterator();
		UIntOS i = 0;
		while (it.HasNext())
		{
			name = it.Next();
			if (name->EqualsICase(this->table))
			{
				tableIndex = i;
			}
			this->cboDataTable->AddItem(name->ToCString(), 0);
			name->Release();
			i++;
		}
		this->cboDataTable->SetSelectedIndex(tableIndex);
	}
	return true;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::CheckDataFile()
{
	Text::CStringNN nullStr = this->GetNullText();
	DB::DBChangeChecker checker(this->db, this->schema, this->table);
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> dataConn;
	if (!this->dataConn.SetTo(dataConn))
	{
		this->ui->ShowMsgOK(CSTR("Please load data file first"), CSTR("Check Table Changes"), this);
		return false;
	}
	Text::StringBuilderUTF8 sbTable;
	if (!this->cboDataTable->GetText(sbTable))
	{
		this->ui->ShowMsgOK(CSTR("Error in getting table name"), CSTR("Check Table Changes"), this);
		return false;
	}
	Text::StringBuilderUTF8 sbSchema;
	Text::CString dataSchema = nullptr;
	if (this->cboDataSchema->GetCount() > 0)
	{
		if (this->cboDataSchema->GetText(sbSchema))
		{
			dataSchema = sbSchema.ToCString();
		}
	}
	checker.SetDataTable(dataConn, dataSchema, sbTable.ToCString(), this->dataTz, this->dataFileNoHeader);
	Optional<Data::QueryConditions> srcDBCond = nullptr;
	Optional<Data::QueryConditions> dataDBCond = nullptr;
	Text::StringBuilderUTF8 sbFilter;
	this->txtSrcFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		srcDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType(), this->db->GetTzQhr());
		if (srcDBCond.IsNull())
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing source filter"), CSTR("Check Table Changes"), this);
			return false;
		}
	}
	sbFilter.ClearStr();
	this->txtDataFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		dataDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType(), this->dataTz);
		if (dataDBCond.IsNull())
		{
			srcDBCond.Delete();
			this->ui->ShowMsgOK(CSTR("Error in parsing data filter"), CSTR("Check Table Changes"), this);
			return false;
		}
	}
	checker.SetNullStr(nullStr);
	checker.SetKeyCols(this->cboKeyCol1->GetSelectedItem().GetUIntOS(), this->cboKeyCol2->GetSelectedItem().GetUIntOS());
	checker.SetSrcCond(srcDBCond);
	checker.SetDataCond(dataDBCond);
	checker.SetColMapping(this->colInd, this->colStr);
	checker.SetSRConv(this->chkGeomConv->IsChecked());
	checker.SetSimpleShape(this->chkGeomSimpleShape->IsChecked());
	checker.SetFixError(this->chkGeomFixError->IsChecked());
	Bool succ = checker.CheckChange();
	if (succ)
	{
		sptr = Text::StrUIntOS(sbuff, checker.GetDataFileRowCnt());
		this->txtDataFileRow->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, checker.GetNoChgCnt());
		this->txtNoChg->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, checker.GetUpdateCnt());
		this->txtUpdated->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, checker.GetNewRowCnt());
		this->txtNewRow->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, checker.GetDelRowCnt());
		this->txtDeletedRow->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		Optional<Text::String> errMsg = checker.GetLastError();
		this->ui->ShowMsgOK(OPTSTR_CSTR(errMsg).Or(CSTR("Unknown Error")), CSTR("Check Table Changes"), this);
	}
	srcDBCond.Delete();
	dataDBCond.Delete();
	return succ;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::GenerateSQL(DB::SQLType sqlType, Bool axisAware, NN<SQLSession> sess)
{
	Text::CStringNN nullStr = this->GetNullText();
	DB::DBChangeChecker checker(this->db, this->schema, this->table);
	NN<DB::ReadingDB> dataConn;
	if (!this->dataConn.SetTo(dataConn))
	{
		this->ui->ShowMsgOK(CSTR("Please load data file first"), CSTR("Check Table Changes"), this);
		return false;
	}
	Text::StringBuilderUTF8 sbTable;
	if (!this->cboDataTable->GetText(sbTable))
	{
		this->ui->ShowMsgOK(CSTR("Error in getting table name"), CSTR("Check Table Changes"), this);
		return false;
	}
	Text::CString dataSchema = nullptr;
	Text::StringBuilderUTF8 sbSchema;
	if (this->cboDataSchema->GetCount() > 0)
	{
		if (this->cboDataSchema->GetText(sbSchema))
		{
			dataSchema = sbSchema.ToCString();
		}
	}
	checker.SetDataTable(dataConn, dataSchema, sbTable.ToCString(), this->dataTz, this->dataFileNoHeader);
	sess->me = *this;
	Optional<Data::QueryConditions> srcDBCond = nullptr;
	Optional<Data::QueryConditions> dataDBCond = nullptr;
	Text::StringBuilderUTF8 sbFilter;
	this->txtSrcFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		srcDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType(), this->db->GetTzQhr());
		if (srcDBCond.IsNull())
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing source filter"), CSTR("Check Table Changes"), this);
			return false;
		}
	}
	sbFilter.ClearStr();
	this->txtDataFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		dataDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType(), this->dataTz);
		if (dataDBCond.IsNull())
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing data filter"), CSTR("Check Table Changes"), this);
			srcDBCond.Delete();
			return false;
		}
	}
	checker.SetNullStr(nullStr);
	checker.SetKeyCols(this->cboKeyCol1->GetSelectedItem().GetUIntOS(), this->cboKeyCol2->GetSelectedItem().GetUIntOS());
	checker.SetSrcCond(srcDBCond);
	checker.SetDataCond(dataDBCond);
	checker.SetColMapping(this->colInd, this->colStr);
	checker.SetSRConv(this->chkGeomConv->IsChecked());
	checker.SetSimpleShape(this->chkGeomSimpleShape->IsChecked());
	checker.SetFixError(this->chkGeomFixError->IsChecked());
	Bool succ = checker.GenerateSQL(sqlType, axisAware, NextSQL, sess);
	if (!succ)
	{
		NN<Text::String> errMsg;
		if (checker.GetLastError().SetTo(errMsg))
		{
			this->ui->ShowMsgOK(errMsg->ToCString(), CSTR("Check Table Changes"), this);
		}
	}
	srcDBCond.Delete();
	dataDBCond.Delete();
	return succ;
}

Bool __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::NextSQL(AnyType userObj, Text::CStringNN sql)
{
	NN<SQLSession> sess = userObj.GetNN<SQLSession>();
	NN<DB::DBConn> db;
	NN<IO::Stream> stm;
	if (sess->mode == 0 && sess->stm.SetTo(stm))
	{
		if (stm->Write(sql.ToByteArray()) != sql.leng)
		{
			return false;
		}
		if (stm->Write(CSTR(";\r\n").ToByteArray()) != 3)
		{
			return false;
		}
		sess->totalCnt++;
		return true;
	}
	else if (sess->mode == 1 && sess->db.SetTo(db))
	{
		if (db->ExecuteNonQuery(sql) >= 0)
		{
			sess->totalCnt++;
			sess->me->UpdateStatus(sess);
			return true;
		}
		Text::StringBuilderUTF8 sb;
		db->GetLastErrorMsg(sb);
		sess->me->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), sess->me);
		return false;
	}
	else if (sess->mode == 2 && sess->db.SetTo(db))
	{
		if (sql.StartsWith(UTF8STRC("insert into ")))
		{
			UIntOS i = sql.IndexOf(UTF8STRC(") values ("));
			if (i == INVALID_INDEX)
			{
				if (db->ExecuteNonQuery(sql) >= 0)
				{
					sess->totalCnt++;
					sess->me->UpdateStatus(sess);
					return true;
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					db->GetLastErrorMsg(sb);
					sess->me->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), sess->me);
					return false;
				}
			}
			if (sess->nInsert == 0)
			{
				sess->sbInsert->Append(sql);
			}
			else
			{
				sess->sbInsert->AppendUTF8Char(',');
				sess->sbInsert->Append(sql.Substring(i + 9));
			}
			sess->nInsert++;
			if (sess->nInsert >= 250)
			{
				if (db->ExecuteNonQuery(sess->sbInsert->ToCString()) >= 0)
				{
					sess->totalCnt += sess->nInsert;
					sess->nInsert = 0;
					sess->sbInsert->ClearStr();
					sess->me->UpdateStatus(sess);
					return true;
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					db->GetLastErrorMsg(sb);
					sess->me->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), sess->me);
					return false;
				}
			}
			return true;
		}
		else if (db->ExecuteNonQuery(sql) >= 0)
		{
			sess->totalCnt++;
			sess->me->UpdateStatus(sess);
			return true;
		}
		Text::StringBuilderUTF8 sb;
		db->GetLastErrorMsg(sb);
		sess->me->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), sess->me);
		return false;
	}
	else
	{
		return false;
	}
}

void SSWR::AVIRead::AVIRDBCheckChgForm::UpdateStatus(NN<SQLSession> sess)
{
	Data::Timestamp t = Data::Timestamp::UtcNow();
	if (t.DiffSecDbl(sess->lastUpdateTime) >= 1)
	{
		sess->lastUpdateTime = t;
		Text::StringBuilderUTF8 sb;
		sb.AppendUIntOS(sess->totalCnt);
		sb.AppendC(UTF8STRC(" SQL executed"));
		this->txtStatus->SetText(sb.ToCString());
		this->ui->ProcessMessages();
	}
}

Text::CStringNN SSWR::AVIRead::AVIRDBCheckChgForm::GetNullText()
{
	switch (this->cboNullCol->GetSelectedIndex())
	{
	case 1:
		return CSTR("NULL");
	case 2:
		return CSTR("null");
	case 3:
		return CSTR("#\"\"No Null");
	default:
		return CSTR("");
	}
}

DB::SQLType SSWR::AVIRead::AVIRDBCheckChgForm::GetDBSQLType()
{
	if (this->db->IsDBTool())
	{
		return NN<DB::ReadingDBTool>::ConvertFrom(this->db)->GetSQLType();
	}
	else if (this->db->IsFullConn())
	{
		return NN<DB::DBConn>::ConvertFrom(this->db)->GetSQLType();
	}
	else
	{
		return DB::SQLType::Unknown;
	}
}

SSWR::AVIRead::AVIRDBCheckChgForm::AVIRDBCheckChgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN table, NN<SSWR::AVIRead::AVIRDBManager> dbMgr) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Check Table Changes"));
	this->core = core;
	this->dbMgr = dbMgr;
	this->db = db;
	this->schema = schema;
	this->table = table;
	this->relDataFile = nullptr;
	this->dataConn = nullptr;
	this->inited = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	if (schema.v.IsNull())
	{
		schema = CSTR("");
	}

	this->lblSchema = ui->NewLabel(*this, CSTR("Schema"));
	this->lblSchema->SetRect(0, 0, 100, 23, false);
	this->txtSchema = ui->NewTextBox(*this, schema.OrEmpty());
	this->txtSchema->SetRect(100, 0, 200, 23, false);
	this->txtSchema->SetReadOnly(true);
	this->lblTable = ui->NewLabel(*this, CSTR("Table"));
	this->lblTable->SetRect(0, 24, 100, 23, false);
	this->txtTable = ui->NewTextBox(*this, table);
	this->txtTable->SetRect(100, 24, 200, 23, false);
	this->txtTable->SetReadOnly(true);
	this->lblSrcFilter = ui->NewLabel(*this, CSTR("Source Filter"));
	this->lblSrcFilter->SetRect(0, 48, 100, 23, false);
	this->txtSrcFilter = ui->NewTextBox(*this, CSTR(""));
	this->txtSrcFilter->SetRect(100, 48, 200, 23, false);
	this->grpData = ui->NewGroupBox(*this, CSTR("Data Source"));
	this->grpData->SetRect(0, 72, 800, 288, false);
	this->chkNoHeader = ui->NewCheckBox(this->grpData, CSTR("CSV No Header"), false);
	this->chkNoHeader->SetRect(100, 0, 150, 23, false);
	this->chkCSVUTCTime = ui->NewCheckBox(this->grpData, CSTR("CSV UTC Time"), false);
	this->chkCSVUTCTime->SetRect(250, 0, 150, 23, false);
	this->radDataFile = ui->NewRadioButton(this->grpData, CSTR("Data File"), true);
	this->radDataFile->SetRect(0, 24, 100, 23, false);
	this->txtDataFile = ui->NewTextBox(this->grpData, CSTR(""));
	this->txtDataFile->SetRect(100, 24, 600, 23, false);
	this->txtDataFile->SetReadOnly(true);
	this->btnDataFile = ui->NewButton(this->grpData, CSTR("Browse"));
	this->btnDataFile->SetRect(700, 24, 75, 23, false);
	this->btnDataFile->HandleButtonClick(OnDataFileClk, this);
	this->radDataConn = ui->NewRadioButton(this->grpData, CSTR("Connection"), false);
	this->radDataConn->SetRect(0, 48, 100, 23, false);
	this->radDataConn->HandleSelectedChange(OnDataConnSelChg, this);
	this->cboDataConn = ui->NewComboBox(this->grpData, false);
	this->cboDataConn->SetRect(100, 48, 600, 23, false);
	this->cboDataConn->HandleSelectionChange(OnDataConnCboSelChg, this);
	this->lblDataSchema = ui->NewLabel(this->grpData, CSTR("Schema"));
	this->lblDataSchema->SetRect(0, 72, 100, 23, false);
	this->cboDataSchema = ui->NewComboBox(this->grpData, false);
	this->cboDataSchema->SetRect(100, 72, 200, 23, false);
	this->cboDataSchema->HandleSelectionChange(OnDataSchemaSelChg, this);
	this->lblDataTable = ui->NewLabel(this->grpData, CSTR("Table"));
	this->lblDataTable->SetRect(0, 96, 100, 23, false);
	this->cboDataTable = ui->NewComboBox(this->grpData, false);
	this->cboDataTable->SetRect(100, 96, 200, 23, false);
	this->cboDataTable->HandleSelectionChange(OnDataTableChg, this);
	this->lblKeyCol = ui->NewLabel(this->grpData, CSTR("Key Column"));
	this->lblKeyCol->SetRect(0, 120, 100, 23, false);
	this->cboKeyCol1 = ui->NewComboBox(this->grpData, false);
	this->cboKeyCol1->SetRect(100, 120, 200, 23, false);
	this->cboKeyCol2 = ui->NewComboBox(this->grpData, false);
	this->cboKeyCol2->SetRect(300, 120, 200, 23, false);
	this->cboKeyCol2->AddItem(CSTR("No second key"), (void*)(INVALID_INDEX));
	this->lblNullCol = ui->NewLabel(this->grpData, CSTR("Null Column"));
	this->lblNullCol->SetRect(0, 144, 100, 23, false);
	this->cboNullCol = ui->NewComboBox(this->grpData, false);
	this->cboNullCol->SetRect(100, 144, 200, 23, false);
	this->cboNullCol->AddItem(CSTR("Empty"), 0);
	this->cboNullCol->AddItem(CSTR("\"NULL\""), 0);
	this->cboNullCol->AddItem(CSTR("\"null\""), 0);
	this->cboNullCol->AddItem(CSTR("No Null"), 0);
	this->cboNullCol->SetSelectedIndex(0);
	this->lblGeomCol = ui->NewLabel(this->grpData, CSTR("Geom Column"));
	this->lblGeomCol->SetRect(0, 168, 100, 23, false);
	this->chkGeomConv = ui->NewCheckBox(this->grpData, CSTR("SR Convert"), false);
	this->chkGeomConv->SetRect(100, 168, 120, 23, false);
	this->chkGeomSimpleShape = ui->NewCheckBox(this->grpData, CSTR("Simple Shape"), false);
	this->chkGeomSimpleShape->SetRect(220, 168, 120, 23, false);
	this->chkGeomFixError = ui->NewCheckBox(this->grpData, CSTR("Fix Error"), false);
	this->chkGeomFixError->SetRect(340, 168, 120, 23, false);
	this->lblAssignCol = ui->NewLabel(this->grpData, CSTR("Assign Columns"));
	this->lblAssignCol->SetRect(0, 192, 100, 23, false);
	this->btnAssignCol = ui->NewButton(this->grpData, CSTR("Assign"));
	this->btnAssignCol->SetRect(100, 192, 75, 23, false);
	this->btnAssignCol->HandleButtonClick(OnAssignColClicked, this);
	this->lblDataFilter = ui->NewLabel(this->grpData, CSTR("Data Filter"));
	this->lblDataFilter->SetRect(0, 216, 100, 23, false);
	this->txtDataFilter = ui->NewTextBox(this->grpData, CSTR(""));
	this->txtDataFilter->SetRect(100, 216, 200, 23, false);
	this->btnDataCheck = ui->NewButton(this->grpData, CSTR("Check"));
	this->btnDataCheck->SetRect(100, 240, 75, 23, false);
	this->btnDataCheck->HandleButtonClick(OnDataCheckClk, this);
	this->lblDataFileRow = ui->NewLabel(*this, CSTR("Data Rows"));
	this->lblDataFileRow->SetRect(0, 360, 100, 23, false);
	this->txtDataFileRow = ui->NewTextBox(*this, CSTR("0"));
	this->txtDataFileRow->SetRect(100, 360, 200, 23, false);
	this->txtDataFileRow->SetReadOnly(true);
	this->lblNoChg = ui->NewLabel(*this, CSTR("No Changes"));
	this->lblNoChg->SetRect(0, 384, 100, 23, false);
	this->txtNoChg = ui->NewTextBox(*this, CSTR("0"));
	this->txtNoChg->SetRect(100, 384, 200, 23, false);
	this->txtNoChg->SetReadOnly(true);
	this->lblUpdated = ui->NewLabel(*this, CSTR("Updated rows"));
	this->lblUpdated->SetRect(0, 408, 100, 23, false);
	this->txtUpdated = ui->NewTextBox(*this, CSTR("0"));
	this->txtUpdated->SetRect(100, 408, 200, 23, false);
	this->txtUpdated->SetReadOnly(true);
	this->lblNewRow = ui->NewLabel(*this, CSTR("New rows"));
	this->lblNewRow->SetRect(0, 432, 100, 23, false);
	this->txtNewRow = ui->NewTextBox(*this, CSTR("0"));
	this->txtNewRow->SetRect(100, 432, 200, 23, false);
	this->txtNewRow->SetReadOnly(true);
	this->lblDeletedRow = ui->NewLabel(*this, CSTR("Deleted rows"));
	this->lblDeletedRow->SetRect(0, 456, 100, 23, false);
	this->txtDeletedRow = ui->NewTextBox(*this, CSTR("0"));
	this->txtDeletedRow->SetRect(100, 456, 200, 23, false);
	this->txtDeletedRow->SetReadOnly(true);

	this->lblDBType = ui->NewLabel(*this, CSTR("SQL Type"));
	this->lblDBType->SetRect(0, 504, 100, 23, false);
	this->cboDBType = ui->NewComboBox(*this, false);
	this->cboDBType->SetRect(100, 504, 200, 23, false);
	this->cboDBType->AddItem(CSTR("MySQL"), (void*)DB::SQLType::MySQL);
	this->cboDBType->AddItem(CSTR("SQL Server"), (void*)DB::SQLType::MSSQL);
	this->cboDBType->AddItem(CSTR("PostgreSQL"), (void*)DB::SQLType::PostgreSQL);
	this->cboDBType->AddItem(CSTR("SQLite"), (void*)DB::SQLType::SQLite);
	DB::SQLType sqlType = this->GetDBSQLType();
	if (sqlType == DB::SQLType::MSSQL)
		this->cboDBType->SetSelectedIndex(1);
	else if (sqlType == DB::SQLType::PostgreSQL)
		this->cboDBType->SetSelectedIndex(2);
	else if (sqlType == DB::SQLType::SQLite)
		this->cboDBType->SetSelectedIndex(3);
	else
		this->cboDBType->SetSelectedIndex(0);
	this->chkAxisAware = ui->NewCheckBox(*this, CSTR("Axis-Aware (MySQL >=8)"), false);
	this->chkAxisAware->SetRect(300, 504, 150, 23, false);
	this->chkMultiRow = ui->NewCheckBox(*this, CSTR("Multi-Row Insert"), true);
	this->chkMultiRow->SetRect(100, 528, 150, 23, false);
	this->btnSQL = ui->NewButton(*this, CSTR("Generate SQL"));
	this->btnSQL->SetRect(100, 552, 75, 23, false);
	this->btnSQL->HandleButtonClick(OnSQLClicked, this);
	this->btnExecute = ui->NewButton(*this, CSTR("Execute SQL"));
	this->btnExecute->SetRect(180, 552, 75, 23, false);
	this->btnExecute->HandleButtonClick(OnExecuteClicked, this);
	this->lblStatTime = ui->NewLabel(*this, CSTR("Time Used"));
	this->lblStatTime->SetRect(0, 576, 100, 23, false);
	this->txtStatTime = ui->NewTextBox(*this, CSTR(""));
	this->txtStatTime->SetRect(100, 576, 150, 23 ,false);
	this->txtStatTime->SetReadOnly(true);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(0, 600, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(100, 600, 300, 23 ,false);
	this->txtStatus->SetReadOnly(true);

	this->HandleDropFiles(OnFiles, this);

	this->cboKeyCol2->SetSelectedIndex(0);
	UIntOS i;
	UIntOS j;
	NN<DB::TableDef> tableDef;
	if (this->db->GetTableDef(this->schema, this->table).SetTo(tableDef))
	{
		Bool hasKey = false;
		Data::ArrayIterator<NN<DB::ColDef>> it = tableDef->ColIterator();
		NN<DB::ColDef> col;
		i = 0;
		while (it.HasNext())
		{
			col = it.Next();
			this->cboKeyCol1->AddItem(col->GetColName(), (void*)i);
			this->cboKeyCol2->AddItem(col->GetColName(), (void*)i);
			if (col->IsPK())
			{
				if (hasKey)
				{
					this->cboKeyCol2->SetSelectedIndex(i + 1);
				}
				else
				{
					hasKey = true;
					this->cboKeyCol1->SetSelectedIndex(i);
				}
			}
			i++;
		}
		this->cboKeyCol1->AddItem(CSTR("No Key, All inserts"), (void*)INVALID_INDEX);
		if (!hasKey)
		{
			this->cboKeyCol1->SetSelectedIndex(0);
		}
		tableDef.Delete();
	}
	Text::StringBuilderUTF8 sb;
	i = 0;
	j = this->dbMgr->GetDataSourceCount();
	while (i < j)
	{
		sb.ClearStr();
		this->dbMgr->GetDataSourceName(i, sb);
		this->cboDataConn->AddItem(sb.ToCString(), 0);
		i++;
	}
	if (j > 0)
	{
		this->cboDataConn->SetSelectedIndex(0);
	}
	this->inited = true;
}

SSWR::AVIRead::AVIRDBCheckChgForm::~AVIRDBCheckChgForm()
{
	this->relDataFile.Delete();
	this->colStr.FreeAll();
}

void SSWR::AVIRead::AVIRDBCheckChgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
