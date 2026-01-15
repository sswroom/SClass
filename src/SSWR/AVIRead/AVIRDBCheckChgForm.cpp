#include "Stdafx.h"
#include "Data/FastStringMapObj.hpp"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "DB/CSVFile.h"
#include "DB/DBConn.h"
#include "DB/DBTool.h"
#include "DB/TableDef.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/MSGeography.h"
#include "Math/WKTReader.h"
#include "SSWR/AVIRead/AVIRDBAssignColumnForm.h"
#include "SSWR/AVIRead/AVIRDBCheckChgForm.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

#define SRID 4326
#define TEXT_COL (UIntOS)-3
#define UNKNOWN_COL (UIntOS)-2

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
	if (me->dataConn.IsNull())
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
	NN<DB::TableDef> table;
	if (!me->db->GetTableDef(me->schema, me->table).SetTo(table))
	{
		return;
	}
	me->GetColIndex(me->colInd, table, nullptr, sbTable.ToCString());
	table.Delete();
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
	NN<DB::TableDef> table;
	if (!me->db->GetTableDef(me->schema, me->table).SetTo(table))
	{
		return;
	}
	SSWR::AVIRead::AVIRDBAssignColumnForm frm(nullptr, me->ui, me->core, table, dataFile, nullptr, sbTable.ToCString(), me->dataFileNoHeader, me->connTz, me->colInd, me->colStr);
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
				me->InitConn(conn, conn->GetTzQhr());
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
		NN<DB::ReadingDB> conn;
		if (me->dataConn.SetTo(conn))
		{
			me->inited = false;
			me->radDataConn->Select();
			me->InitConn(conn, conn->GetTzQhr());
			me->inited = true;
		}
	}
}
			
Optional<Text::String> SSWR::AVIRead::AVIRDBCheckChgForm::GetNewText(UIntOS colIndex)
{
	NN<Text::String> s;
	if (this->colStr.GetItem(colIndex).SetTo(s))
		return s->Clone();
	else
		return nullptr;
}

NN<Text::String> SSWR::AVIRead::AVIRDBCheckChgForm::GetNewTextNN(UIntOS colIndex)
{
	NN<Text::String> s;
	if (this->colStr.GetItem(colIndex).SetTo(s))
		return s->Clone();
	else
		return Text::String::NewEmpty();
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
		return this->InitConn(csv, csvTZ);
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
			return this->InitConn(nndataFile, csvTZ);
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("Check Table Changes"), this);
			this->dataConn = nullptr;
			return false;
		}
	}
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::InitConn(NN<DB::ReadingDB> conn, Int8 connTz)
{
	this->connTz = connTz;
	this->dataConn = conn;
	Data::ArrayListStringNN tableNames;
	conn->QueryTableNames(nullptr, tableNames);
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

Bool SSWR::AVIRead::AVIRDBCheckChgForm::GetColIndex(NN<Data::ArrayListNative<UIntOS>> colInd, NN<DB::TableDef> destTable, Text::CString srcSchema, Text::CStringNN srcTable)
{
	colInd->Clear();
	UIntOS i = 0;
	UIntOS destCnt = destTable->GetColCnt();
	UIntOS k;
	UIntOS srcCnt;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ColDef> col;
	NN<DB::DBReader> r;
	while (i < destCnt)
	{
		colInd->Add(this->dataFileNoHeader?i:UNKNOWN_COL);
		i++;
	}

	NN<DB::ReadingDB> dataConn;
	if (!this->dataConn.SetTo(dataConn) || !dataConn->QueryTableData(srcSchema, srcTable, nullptr, 0, 1, nullptr, nullptr).SetTo(r))
	{
		return false;
	}
	srcCnt = r->ColCount();
	if (!this->dataFileNoHeader)
	{
		k = 0;
		while (k < srcCnt)
		{
			if (r->GetName(k, sbuff).SetTo(sptr))
			{
				i = 0;
				while (i < destCnt)
				{
					if (destTable->GetCol(i).SetTo(col) && col->GetColName()->EqualsICase(sbuff, (UIntOS)(sptr - sbuff)))
					{
						colInd->SetItem(i, k);
						break;
					}
					i++;
				}
			}
/*			if (!found)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Data File Column "));
				sb.AppendUIntOS(k);
				if (sptr)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.Append(CSTRP(sbuff, sptr));
					sb.AppendUTF8Char(')');
				}
				sb.AppendC(UTF8STRC(" not found"));
				this->dataFile->CloseReader(r);
				table.Delete();
				SDEL_CLASS(srcDBCond);
				SDEL_CLASS(dataDBCond);
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
				return false;
			}*/
			k++;
		}
	}
	dataConn->CloseReader(r);
	return true;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::IsColIndexValid(NN<Data::ArrayListNative<UIntOS>> colInd, NN<DB::TableDef> destTable)
{
	UIntOS j = colInd->GetCount();
	if (j == 0)
	{
		this->ui->ShowMsgOK(CSTR("Error in reading database structure"), CSTR("DB Check Change"), this);
		return false;
	}
	NN<DB::ColDef> col;
	UIntOS i = 0;
	while (i < j)
	{
		if (colInd->GetItem(i) == UNKNOWN_COL)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Unknown column found: "));
			if (destTable->GetCol(i).SetTo(col))
			{
				sb.Append(col->GetColName());
			}
			else
			{
				sb.Append(CSTR("Column "));
				sb.AppendUIntOS(i);
			}
			this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Check Change"), this);
			return false;
		}
		i++;
	}
	return true;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::CheckDataFile()
{
	Text::CStringNN nullStr = this->GetNullText();
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
	Optional<Data::QueryConditions> srcDBCond = nullptr;
	Optional<Data::QueryConditions> dataDBCond = nullptr;
	Text::StringBuilderUTF8 sbFilter;
	this->txtSrcFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		srcDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType());
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
		dataDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType());
		if (dataDBCond.IsNull())
		{
			srcDBCond.Delete();
			this->ui->ShowMsgOK(CSTR("Error in parsing data filter"), CSTR("Check Table Changes"), this);
			return false;
		}
	}
	Int8 csvTZ = this->db->GetTzQhr();
	DB::SQLType sqlType = this->GetDBSQLType();
	Bool no3DGeometry = DB::DBUtil::IsNo3DGeometry(sqlType);
	this->connTz = csvTZ;
	NN<DB::TableDef> table;;
	if (!this->db->GetTableDef(this->schema, this->table).SetTo(table))
	{
		srcDBCond.Delete();
		dataDBCond.Delete();
		this->ui->ShowMsgOK(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	UIntOS keyCol1 = this->cboKeyCol1->GetSelectedItem().GetUIntOS();
	UIntOS keyDCol1;
	UIntOS keyCol2 = this->cboKeyCol2->GetSelectedItem().GetUIntOS();
	UIntOS keyDCol2;
	UIntOS i;
	UIntOS k;
	UIntOS dbCnt;
	UIntOS srcCnt;
	Data::FastStringMapObj<Text::String**> csvData;
	Text::String** rowData;
	NN<DB::ColDef> col;
	NN<DB::DBReader> r;
	if (!this->IsColIndexValid(this->colInd, table))
	{
		srcDBCond.Delete();
		dataDBCond.Delete();
		table.Delete();
		return false;
	}
	if (!dataConn->QueryTableData(nullptr, sbTable.ToCString(), nullptr, 0, 0, nullptr, dataDBCond).SetTo(r))
	{
		srcDBCond.Delete();
		dataDBCond.Delete();
		table.Delete();
		this->ui->ShowMsgOK(CSTR("Error in reading data file"), CSTR("Check Table Changes"), this);
		return false;
	}
	dbCnt = table->GetColCnt();
	srcCnt = r->ColCount();
	keyDCol1 = this->colInd.GetItem(keyCol1);
	if (keyCol2 == INVALID_INDEX)
	{
		keyDCol2 = INVALID_INDEX;
	}
	else
	{
		keyDCol2 = this->colInd.GetItem(keyCol2);
	}

	if (this->dataFileNoHeader)
	{
		if (dbCnt != srcCnt)
		{
			this->ui->ShowMsgOK(CSTR("Column Count does not match"), CSTR("Check Table Changes"), this);
			dataConn->CloseReader(r);
			table.Delete();
			srcDBCond.Delete();
			dataDBCond.Delete();
			return false;
		}
	}
	else
	{
		if (keyCol1 != INVALID_INDEX && keyDCol1 == INVALID_INDEX)
		{
			this->ui->ShowMsgOK(CSTR("Key Column not found in data file"), CSTR("Check Table Changes"), this);
			dataConn->CloseReader(r);
			table.Delete();
			srcDBCond.Delete();
			dataDBCond.Delete();
			return false;
		}
	}
	UIntOS dataFileRowCnt = 0;
	UIntOS noChgCnt = 0;
	UIntOS updateCnt = 0;
	UIntOS newRowCnt = 0;
	UIntOS delRowCnt = 0;
	Bool succ = true;
	NN<Text::String> s;
	NN<Text::String> id;
	Text::StringBuilderUTF8 sbId;
	while (r->ReadNext())
	{
		dataFileRowCnt++;
		if (r->ColCount() >= srcCnt)
		{
			if (keyCol1 == INVALID_INDEX)
			{
				newRowCnt++;
			}
			else
			{
				if (keyDCol1 == TEXT_COL)
				{
					id = this->GetNewTextNN(keyCol1);
				}
				else
				{
					id = r->GetNewStrNN(keyDCol1);
				}
				if (keyDCol2 == INVALID_INDEX && (id->leng == 0 || id->Equals(UTF8STRC("0"))))
				{
					id->Release();
					newRowCnt++;
				}
				else
				{
					sbId.ClearStr();
					sbId.Append(id);
					if (keyDCol2 != INVALID_INDEX)
					{
						sbId.AppendC(UTF8STRC("_ _"));
						if (keyDCol2 == TEXT_COL)
						{
							if (this->colStr.GetItem(keyCol2).SetTo(s))
								sbId.Append(s);
						}
						else
						{
							r->GetStr(keyDCol2, sbId);
						}
					}
					rowData = MemAlloc(Text::String*, dbCnt);
					i = 0;
					while (i < dbCnt)
					{
						k = this->colInd.GetItem(i);
						rowData[i] = 0;
						if (k != INVALID_INDEX)
						{
							if (k == TEXT_COL)
							{
								rowData[i] = this->GetNewText(i).OrNull();
							}
							else
							{
								rowData[i] = r->GetNewStr(k).OrNull();
							}
							if (rowData[i])
							{
								if (rowData[i]->Equals(nullStr.v, nullStr.leng))
								{
									rowData[i]->Release();
									rowData[i] = 0;
								}
								else if (rowData[i]->leng == 0 && table->GetCol(i).SetTo(col))
								{
									switch (col->GetColType())
																{
									case DB::DBUtil::CT_VarUTF8Char:
									case DB::DBUtil::CT_VarUTF16Char:
									case DB::DBUtil::CT_VarUTF32Char:
									case DB::DBUtil::CT_UTF8Char:
									case DB::DBUtil::CT_UTF16Char:
									case DB::DBUtil::CT_UTF32Char:
									case DB::DBUtil::CT_Binary:
										break;
									case DB::DBUtil::CT_Date:
									case DB::DBUtil::CT_DateTime:
									case DB::DBUtil::CT_DateTimeTZ:
									case DB::DBUtil::CT_Decimal:
									case DB::DBUtil::CT_Double:
									case DB::DBUtil::CT_Float:
									case DB::DBUtil::CT_UInt16:
									case DB::DBUtil::CT_Int16:
									case DB::DBUtil::CT_UInt32:
									case DB::DBUtil::CT_Int32:
									case DB::DBUtil::CT_Byte:
									case DB::DBUtil::CT_UInt64:
									case DB::DBUtil::CT_Int64:
									case DB::DBUtil::CT_Bool:
									case DB::DBUtil::CT_Vector:
									case DB::DBUtil::CT_UUID:
									case DB::DBUtil::CT_Unknown:
									default:
										rowData[i]->Release();
										rowData[i] = 0;
										break;
									}
								}
							}
						}
						i++;
					}
					rowData = csvData.PutC(sbId.ToCString(), rowData);
					if (rowData)
					{
						i = dbCnt;
						while (i-- > 0)
						{
							SDEL_STRING(rowData[i]);
						}
						MemFree(rowData);

						Text::StringBuilderUTF8 sb;
						sb.Append(CSTR("Data File Key duplicate ("));
						sb.Append(sbId.ToCString());
						sb.AppendUTF8Char(')');
						this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
						id->Release();
						succ = false;
						break;
					}
					id->Release();
				}
			}
		}
	}
	dataConn->CloseReader(r);

	if (succ && keyCol1 != INVALID_INDEX)
	{
		if (!this->db->QueryTableData(this->schema, this->table, nullptr, 0, 0, nullptr, srcDBCond).SetTo(r))
		{
			this->ui->ShowMsgOK(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
			succ = false;
		}
		else
		{
			Data::ArrayListStringNN idList;
			while (r->ReadNext())
			{
				if (!r->GetNewStr(keyCol1).SetTo(id))
				{
					this->ui->ShowMsgOK(CSTR("Source Key is null"), CSTR("Check Table Changes"), this);
					succ = false;
					break;
				}
				sbId.ClearStr();
				sbId.Append(id);
				if (keyCol2 != INVALID_INDEX)
				{
					sbId.AppendC(UTF8STRC("_ _"));
					r->GetStr(keyCol2, sbId);
				}
				if (idList.SortedIndexOfC(sbId.ToCString()) >= 0)
				{
					sbFilter.ClearStr();
					sbFilter.AppendC(UTF8STRC("Source Key duplicate ("));
					sbFilter.Append(sbId);
					sbFilter.AppendUTF8Char(')');
					this->ui->ShowMsgOK(sbFilter.ToCString(), CSTR("Check Table Changes"), this);
					succ = false;
					break;
				}
				rowData = csvData.GetC(sbId.ToCString());
				idList.SortedInsert(Text::String::New(sbId.ToCString()));
				id->Release();
				if (rowData)
				{
					Bool diff = false;
					i = 0;
					while (i < dbCnt)
					{
						if (this->colInd.GetItem(i) == INVALID_INDEX)
						{

						}
						else if (r->IsNull(i))
						{
							if (rowData[i] == 0)
							{

							}
							else
							{
								diff = true;
								break;
							}
						}
						else if (rowData[i] == 0)
						{
							diff = true;
							break;
						}
						else if (table->GetCol(i).SetTo(col))
						{
							switch (col->GetColType())
							{
							case DB::DBUtil::CT_VarUTF8Char:
							case DB::DBUtil::CT_VarUTF16Char:
							case DB::DBUtil::CT_VarUTF32Char:
							case DB::DBUtil::CT_UTF8Char:
							case DB::DBUtil::CT_UTF16Char:
							case DB::DBUtil::CT_UTF32Char:
								id = r->GetNewStrNN(i);
								if (!id->Equals(rowData[i]->v, rowData[i]->leng))
								{
									diff = true;
								}
								id->Release();
								break;
							case DB::DBUtil::CT_Date:
								{
									Data::Date ts1 = r->GetTimestamp(i).ToDate();
									Data::Date ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->connTz).ToDate();
									if (ts1.GetTotalDays() != ts2.GetTotalDays())
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_DateTime:
							case DB::DBUtil::CT_DateTimeTZ:
								{
									Data::Timestamp ts1 = r->GetTimestamp(i);
									Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->connTz);
									if (ts1.DiffSec(ts2) != 0)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Float:
								{
									Double v1 = r->GetDblOrNAN(i);
									Double v2 = rowData[i]->ToDoubleOrNAN();
									if (!Math::NearlyEquals(v1, v2, 0.000001))
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Decimal:
							case DB::DBUtil::CT_Double:
								{
									Double v1 = r->GetDblOrNAN(i);
									Double v2 = rowData[i]->ToDoubleOrNAN();
									if (!Math::NearlyEqualsDbl(v1, v2))
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_UInt16:
							case DB::DBUtil::CT_Int16:
							case DB::DBUtil::CT_UInt32:
							case DB::DBUtil::CT_Int32:
							case DB::DBUtil::CT_Byte:
								{
									Int32 v1 = r->GetInt32(i);
									Int32 v2 = rowData[i]->ToInt32();
									if (v1 != v2)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_UInt64:
							case DB::DBUtil::CT_Int64:
								{
									Int64 v1 = r->GetInt64(i);
									Int64 v2 = rowData[i]->ToInt64();
									if (v1 != v2)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Bool:
								{
									Bool v1 = r->GetBool(i);
									Bool v2 = rowData[i]->v[0] == 't' || rowData[i]->v[0] == 'T' || rowData[i]->ToInt32() != 0;
									if (v1 != v2)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Vector:
								{
									NN<Math::Geometry::Vector2D> vec1;
									if (r->GetVector(i).SetTo(vec1))
									{
										vec1->SetSRID(col->GetGeometrySRID());
										NN<Math::Geometry::Vector2D> vec2;
										Math::WKTReader reader(vec1->GetSRID());
										if (!reader.ParseWKT(rowData[i]->v).SetTo(vec2))
										{
											diff = true;
										}
										else
										{
											if (!vec1->Equals(vec2, false, true, no3DGeometry))
											{
												diff = true;
											}
											vec2.Delete();
										}
										vec1.Delete();
									}
									else
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Binary:
								{
									diff = true;
								}
								break;
							case DB::DBUtil::CT_UUID:
								{
									Data::UUID uuid1;
									Data::UUID uuid2;
									r->GetUUID(i, uuid1);
									uuid2.SetValue(rowData[i]->ToCString());
									if (!uuid1.Equals(uuid2))
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Unknown:
							default:
								diff = true;
								break;
							}
							if (diff)
							{
								break;
							}
						}
						else
						{
							diff = true;
							break;
						}
						i++;
					}
					if (diff)
					{
						updateCnt++;
					}
					else
					{
						noChgCnt++;
					}
				}
				else
				{
					delRowCnt++;
				}
			}
			this->db->CloseReader(r);

			newRowCnt += dataFileRowCnt + delRowCnt - idList.GetCount();
			idList.FreeAll();
		}
	}

	k = csvData.GetCount();
	while (k-- > 0)
	{
		rowData = csvData.GetItem(k);
		i = dbCnt;
		while (i-- > 0)
		{
			SDEL_STRING(rowData[i]);
		}
		MemFree(rowData);
	}
	table.Delete();
	srcDBCond.Delete();
	dataDBCond.Delete();
	if (succ)
	{
		sptr = Text::StrUIntOS(sbuff, dataFileRowCnt);
		this->txtDataFileRow->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, noChgCnt);
		this->txtNoChg->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, updateCnt);
		this->txtUpdated->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, newRowCnt);
		this->txtNewRow->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUIntOS(sbuff, delRowCnt);
		this->txtDeletedRow->SetText(CSTRP(sbuff, sptr));
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::GenerateSQL(DB::SQLType sqlType, Bool axisAware, NN<SQLSession> sess)
{
	Text::CStringNN nullStr = this->GetNullText();
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
	Optional<Data::QueryConditions> srcDBCond = nullptr;
	Optional<Data::QueryConditions> dataDBCond = nullptr;
	Text::StringBuilderUTF8 sbFilter;
	this->txtSrcFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		srcDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType());
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
		dataDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType());
		if (dataDBCond.IsNull())
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing data filter"), CSTR("Check Table Changes"), this);
			srcDBCond.Delete();
			return false;
		}
	}
	NN<DB::TableDef> table;
	NN<DB::ColDef> col;
	UInt32 srid = 0;
	if (dataConn->GetTableDef(nullptr, sbTable.ToCString()).SetTo(table))
	{
		UIntOS i = table->GetColCnt();
		while (i-- > 0)
		{
			if (table->GetCol(i).SetTo(col) && col->GetColType() == DB::DBUtil::CT_Vector)
			{
				srid = col->GetGeometrySRID();
				break;
			}
		}
		table.Delete();
	}
	if (!this->db->GetTableDef(this->schema, this->table).SetTo(table))
	{
		this->ui->ShowMsgOK(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		srcDBCond.Delete();
		dataDBCond.Delete();
		return false;
	}
	Bool succ = true;
	Bool intKey = false;
	Bool no3DGeometry = DB::DBUtil::IsNo3DGeometry(this->GetDBSQLType());
	Text::StringBuilderUTF8 sbId;
	UIntOS keyCol1 = this->cboKeyCol1->GetSelectedItem().GetUIntOS();
	UIntOS keyDCol1;
	UIntOS keyCol2 = this->cboKeyCol2->GetSelectedItem().GetUIntOS();
	UIntOS keyDCol2;
	UIntOS i = 0;
	UIntOS dbCnt = table->GetColCnt();
	while (i < dbCnt)
	{
		if (i == keyCol1 && keyCol2 == INVALID_INDEX && table->GetCol(i).SetTo(col))
		{
			DB::DBUtil::ColType colType = col->GetColType();
			switch (colType)
			{
			case DB::DBUtil::CT_Bool:
			case DB::DBUtil::CT_Byte:
			case DB::DBUtil::CT_Int16:
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_Int64:
			case DB::DBUtil::CT_UInt16:
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_UInt64:
				intKey = true;
				break;
			case DB::DBUtil::CT_Unknown:
			case DB::DBUtil::CT_UTF8Char:
			case DB::DBUtil::CT_UTF16Char:
			case DB::DBUtil::CT_UTF32Char:
			case DB::DBUtil::CT_VarUTF8Char:
			case DB::DBUtil::CT_VarUTF16Char:
			case DB::DBUtil::CT_VarUTF32Char:
			case DB::DBUtil::CT_Date:
			case DB::DBUtil::CT_DateTime:
			case DB::DBUtil::CT_DateTimeTZ:
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
			case DB::DBUtil::CT_Decimal:
			case DB::DBUtil::CT_Binary:
			case DB::DBUtil::CT_UUID:
				break;
			case DB::DBUtil::CT_Vector:
				if (srid == 0)
				{
					srid = col->GetGeometrySRID();
				}
				break;
			}
		}
		i++;
	}
	if (srid == 0)
	{
		srid = SRID;
	}
	UIntOS k;
	UIntOS srcCnt;
	Data::FastStringMapObj<Text::String**> csvData;
	Text::String** rowData;
	NN<DB::DBReader> r;
	if (!this->IsColIndexValid(this->colInd, table))
	{
		srcDBCond.Delete();
		dataDBCond.Delete();
		table.Delete();
		return false;
	}
	if (!dataConn->QueryTableData(nullptr, sbTable.ToCString(), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		table.Delete();
		srcDBCond.Delete();
		dataDBCond.Delete();
		this->ui->ShowMsgOK(CSTR("Error in reading data file"), CSTR("Check Table Changes"), this);
		return false;
	}
	srcCnt = r->ColCount();
	keyDCol1 = this->colInd.GetItem(keyCol1);
	if (keyCol2 == INVALID_INDEX)
	{
		keyDCol2 = INVALID_INDEX;
	}
	else
	{
		keyDCol2 = this->colInd.GetItem(keyCol2);
	}

	if (this->dataFileNoHeader)
	{
		if (dbCnt != srcCnt)
		{
			this->ui->ShowMsgOK(CSTR("Column Count does not match"), CSTR("Check Table Changes"), this);
			dataConn->CloseReader(r);
			table.Delete();
			srcDBCond.Delete();
			dataDBCond.Delete();
			return false;
		}
	}
	else
	{
		if (keyCol1 != INVALID_INDEX && keyDCol1 == INVALID_INDEX)
		{
			this->ui->ShowMsgOK(CSTR("Key Column not found in data file"), CSTR("Check Table Changes"), this);
			dataConn->CloseReader(r);
			table.Delete();
			srcDBCond.Delete();
			dataDBCond.Delete();
			return false;
		}
	}
	DB::SQLBuilder sql(sqlType, axisAware, Data::DateTimeUtil::GetLocalTzQhr());
	Bool genInsert;
	Bool colFound;
	NN<Text::String> s;
	NN<Text::String> s2;
	Optional<Text::String> ops;
//	NN<Text::String> id;
	while (r->ReadNext())
	{
		if (r->ColCount() >= srcCnt)
		{
			genInsert = false;
			if (keyCol1 == INVALID_INDEX)
			{
				genInsert = true;
			}
			else
			{
				if (keyDCol1 == TEXT_COL)
				{
					s = this->GetNewTextNN(keyCol1);
				}
				else
				{
					s = r->GetNewStrNN(keyDCol1);
				}
				if (keyDCol2 == INVALID_INDEX && (s->leng == 0 || s->Equals(UTF8STRC("0"))))
				{
					s->Release();
					genInsert = true;
				}
				else
				{
					sbId.ClearStr();
					sbId.Append(s);
					if (keyDCol2 != INVALID_INDEX)
					{
						sbId.AppendC(UTF8STRC("_ _"));
						if (keyDCol2 == TEXT_COL)
						{
							if (this->GetNewText(keyCol2).SetTo(s2))
							{
								sbId.Append(s2);
							}
						}
						else
						{
							r->GetStr(keyDCol2, sbId);
						}
					}
					rowData = MemAlloc(Text::String*, dbCnt);
					i = 0;
					while (i < dbCnt)
					{
						k = this->colInd.GetItem(i);
						if (k == INVALID_INDEX)
						{
							rowData[i] = 0;
						}
						else
						{
							if (k == TEXT_COL)
							{
								rowData[i] = this->GetNewText(i).OrNull();
							}
							else
							{
								rowData[i] = r->GetNewStr(this->colInd.GetItem(i)).OrNull();
							}
							if (rowData[i])
							{
								if (rowData[i]->Equals(nullStr.v, nullStr.leng))
								{
									rowData[i]->Release();
									rowData[i] = 0;
								}
								else if (rowData[i]->leng == 0)
								{
									switch (table->GetCol(i).SetTo(col)?col->GetColType():DB::DBUtil::CT_Unknown)
									{
									case DB::DBUtil::CT_VarUTF8Char:
									case DB::DBUtil::CT_VarUTF16Char:
									case DB::DBUtil::CT_VarUTF32Char:
									case DB::DBUtil::CT_UTF8Char:
									case DB::DBUtil::CT_UTF16Char:
									case DB::DBUtil::CT_UTF32Char:
									case DB::DBUtil::CT_Binary:
										break;
									case DB::DBUtil::CT_Date:
									case DB::DBUtil::CT_DateTime:
									case DB::DBUtil::CT_DateTimeTZ:
									case DB::DBUtil::CT_Decimal:
									case DB::DBUtil::CT_Double:
									case DB::DBUtil::CT_Float:
									case DB::DBUtil::CT_UInt16:
									case DB::DBUtil::CT_Int16:
									case DB::DBUtil::CT_UInt32:
									case DB::DBUtil::CT_Int32:
									case DB::DBUtil::CT_Byte:
									case DB::DBUtil::CT_UInt64:
									case DB::DBUtil::CT_Int64:
									case DB::DBUtil::CT_Bool:
									case DB::DBUtil::CT_Vector:
									case DB::DBUtil::CT_UUID:
									case DB::DBUtil::CT_Unknown:
									default:
										rowData[i]->Release();
										rowData[i] = 0;
										break;
									}
								}
							}
						}

						i++;
					}
					rowData = csvData.PutC(sbId.ToCString(), rowData);
					s->Release();
					if (rowData)
					{
						i = dbCnt;
						while (i-- > 0)
						{
							SDEL_STRING(rowData[i]);
						}
						MemFree(rowData);
					}
				}
			}
			if (genInsert)
			{
				sql.Clear();
				sql.AppendCmdC(CSTR("insert into "));
				Text::CStringNN schema;
				if (this->schema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
				{
					sql.AppendCol(schema.v);
					sql.AppendCmdC(CSTR("."));
				}
				sql.AppendCol(this->table.v);
				sql.AppendCmdC(CSTR(" ("));
				colFound = false;
				i = 0;
				while (i < dbCnt)
				{
					if ((i != keyCol1 || keyCol2 != INVALID_INDEX) && this->colInd.GetItem(i) != INVALID_INDEX && table->GetCol(i).SetTo(col))
					{
						if (colFound) sql.AppendCmdC(CSTR(", "));
						colFound = true;
						sql.AppendCol(col->GetColName()->v);
					}
					i++;
				}
				sql.AppendCmdC(CSTR(") values ("));
				colFound = false;
				i = 0;
				while (i < dbCnt)
				{
					if ((i != keyCol1 || keyCol2 != INVALID_INDEX) && this->colInd.GetItem(i) != INVALID_INDEX && table->GetCol(i).SetTo(col))
					{
						if (colFound) sql.AppendCmdC(CSTR(", "));
						colFound = true;
						ops = r->GetNewStr(this->colInd.GetItem(i));
						if (ops.SetTo(s) && s->Equals(nullStr.v, nullStr.leng))
						{
							sql.AppendStr(0);
						}
						else
						{
							AppendCol(sql, col, ops, this->connTz, srid);
						}
						OPTSTR_DEL(ops);
					}
					i++;
				}
				sql.AppendCmdC(CSTR(")"));
				if (!NextSQL(sql.ToCString(), sess))
				{
					succ = false;
					break;
				}
			}
		}
	}
	dataConn->CloseReader(r);

	if (keyCol1 != INVALID_INDEX)
	{
		if (!this->db->QueryTableData(this->schema, this->table, nullptr, 0, 0, nullptr, srcDBCond).SetTo(r))
		{
			this->ui->ShowMsgOK(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
		}
		else
		{
			NN<Text::String> id1;
			Optional<Text::String> id2;
			Data::ArrayListStringNN idList;
			while (r->ReadNext())
			{
				sbId.ClearStr();
				id1 = r->GetNewStrNN(keyCol1);
				sbId.Append(id1);
				id2 = nullptr;
				if (keyCol2 != INVALID_INDEX)
				{
					sbId.AppendC(UTF8STRC("_ _"));
					id2 = r->GetNewStrNN(keyCol2);
					sbId.AppendOpt(id2);
				}
				rowData = csvData.GetC(sbId.ToCString());
				idList.Add(Text::String::New(sbId.ToCString()));
				if (rowData)
				{
					Bool diff = false;
					sql.Clear();
					sql.AppendCmdC(CSTR("update "));
					Text::CStringNN schema;
					if (this->schema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
					{
						sql.AppendCol(schema.v);
						sql.AppendCmdC(CSTR("."));
					}
					sql.AppendCol(this->table.v);
					sql.AppendCmdC(CSTR(" set "));

					i = 0;
					while (i < dbCnt)
					{
						if (this->colInd.GetItem(i) == INVALID_INDEX)
						{

						}
						else if (r->IsNull(i))
						{
							if (rowData[i] == 0)
							{

							}
							else
							{
								if (table->GetCol(i).SetTo(col))
								{
									switch (col->GetColType())
									{
									case DB::DBUtil::CT_VarUTF8Char:
									case DB::DBUtil::CT_VarUTF16Char:
									case DB::DBUtil::CT_VarUTF32Char:
									case DB::DBUtil::CT_UTF8Char:
									case DB::DBUtil::CT_UTF16Char:
									case DB::DBUtil::CT_UTF32Char:
										if (diff)
										{
											sql.AppendCmdC(CSTR(", "));
										}
										else
										{
											diff = true;
										}
										sql.AppendCol(col->GetColName()->v);
										sql.AppendCmdC(CSTR(" = "));
										sql.AppendStr(rowData[i]);
										break;
									case DB::DBUtil::CT_Date:
										{
											Data::Date ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->connTz).ToDate();
											if (diff)
											{
												sql.AppendCmdC(CSTR(", "));
											}
											else
											{
												diff = true;
											}
											sql.AppendCol(col->GetColName()->v);
											sql.AppendCmdC(CSTR(" = "));
											sql.AppendDate(ts2);
										}
										break;
									case DB::DBUtil::CT_DateTime:
									case DB::DBUtil::CT_DateTimeTZ:
										{
											Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->connTz);
											if (diff)
											{
												sql.AppendCmdC(CSTR(", "));
											}
											else
											{
												diff = true;
											}
											sql.AppendCol(col->GetColName()->v);
											sql.AppendCmdC(CSTR(" = "));
											sql.AppendTS(ts2);
										}
										break;
									case DB::DBUtil::CT_Double:
									case DB::DBUtil::CT_Float:
									case DB::DBUtil::CT_Decimal:
										{
											Double v2 = rowData[i]->ToDoubleOrNAN();
											if (diff)
											{
												sql.AppendCmdC(CSTR(", "));
											}
											else
											{
												diff = true;
											}
											sql.AppendCol(col->GetColName()->v);
											sql.AppendCmdC(CSTR(" = "));
											sql.AppendNDbl(v2);
										}
										break;
									case DB::DBUtil::CT_UInt16:
									case DB::DBUtil::CT_Int16:
									case DB::DBUtil::CT_UInt32:
									case DB::DBUtil::CT_Int32:
									case DB::DBUtil::CT_Byte:
										{
											Int32 v2 = rowData[i]->ToInt32();
											if (diff)
											{
												sql.AppendCmdC(CSTR(", "));
											}
											else
											{
												diff = true;
											}
											sql.AppendCol(col->GetColName()->v);
											sql.AppendCmdC(CSTR(" = "));
											sql.AppendInt32(v2);
										}
										break;
									case DB::DBUtil::CT_UInt64:
									case DB::DBUtil::CT_Int64:
										{
											Int64 v2 = rowData[i]->ToInt64();
											if (diff)
											{
												sql.AppendCmdC(CSTR(", "));
											}
											else
											{
												diff = true;
											}
											sql.AppendCol(col->GetColName()->v);
											sql.AppendCmdC(CSTR(" = "));
											sql.AppendInt64(v2);
										}
										break;
									case DB::DBUtil::CT_Bool:
										{
											Bool v2 = rowData[i]->v[0] == 't' || rowData[i]->v[0] == 'T' || rowData[i]->ToInt32() != 0;
											if (diff)
											{
												sql.AppendCmdC(CSTR(", "));
											}
											else
											{
												diff = true;
											}
											sql.AppendCol(col->GetColName()->v);
											sql.AppendCmdC(CSTR(" = "));
											sql.AppendBool(v2);
										}
										break;
									case DB::DBUtil::CT_Vector:
										{
											NN<Math::Geometry::Vector2D> vec2;;
											Math::WKTReader reader(srid);
											if (!reader.ParseWKT(rowData[i]->v).SetTo(vec2))
											{
											}
											else
											{
												if (diff)
												{
													sql.AppendCmdC(CSTR(", "));
												}
												else
												{
													diff = true;
												}
												sql.AppendCol(col->GetColName()->v);
												sql.AppendCmdC(CSTR(" = "));
												sql.AppendVector(vec2);
												vec2.Delete();
											}
										}
										break;
									case DB::DBUtil::CT_Binary:
										break;
									case DB::DBUtil::CT_UUID:
										{
											Data::UUID uuid2;
											uuid2.SetValue(rowData[i]->ToCString());
										}
										break;
									case DB::DBUtil::CT_Unknown:
									default:
										break;
									}
								}
							}
						}
						else if (rowData[i] == 0)
						{
							if (diff)
							{
								sql.AppendCmdC(CSTR(", "));
							}
							else
							{
								diff = true;
							}
							if (table->GetCol(i).SetTo(col))
								sql.AppendCol(col->GetColName()->v);
							sql.AppendCmdC(CSTR(" = "));
							sql.AppendStr(0);
						}
						else if (table->GetCol(i).SetTo(col))
						{
							switch (col->GetColType())
							{
							case DB::DBUtil::CT_VarUTF8Char:
							case DB::DBUtil::CT_VarUTF16Char:
							case DB::DBUtil::CT_VarUTF32Char:
							case DB::DBUtil::CT_UTF8Char:
							case DB::DBUtil::CT_UTF16Char:
							case DB::DBUtil::CT_UTF32Char:
								s = r->GetNewStrNN(i);
								if (!s->Equals(rowData[i]->v, rowData[i]->leng))
								{
									if (diff)
									{
										sql.AppendCmdC(CSTR(", "));
									}
									else
									{
										diff = true;
									}
									sql.AppendCol(col->GetColName()->v);
									sql.AppendCmdC(CSTR(" = "));
									sql.AppendStr(rowData[i]);
								}
								s->Release();
								break;
							case DB::DBUtil::CT_Date:
								{
									Data::Date ts1 = r->GetTimestamp(i).ToDate();
									Data::Date ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->connTz).ToDate();
									if (ts1.GetTotalDays() != ts2.GetTotalDays())
									{
										if (diff)
										{
											sql.AppendCmdC(CSTR(", "));
										}
										else
										{
											diff = true;
										}
										sql.AppendCol(col->GetColName()->v);
										sql.AppendCmdC(CSTR(" = "));
										sql.AppendDate(ts2);
									}
								}
								break;
							case DB::DBUtil::CT_DateTime:
							case DB::DBUtil::CT_DateTimeTZ:
								{
									Data::Timestamp ts1 = r->GetTimestamp(i);
									Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->connTz);
									if (ts1.DiffSec(ts2) != 0)
									{
										if (diff)
										{
											sql.AppendCmdC(CSTR(", "));
										}
										else
										{
											diff = true;
										}
										sql.AppendCol(col->GetColName()->v);
										sql.AppendCmdC(CSTR(" = "));
										sql.AppendTS(ts2);
									}
								}
								break;
							case DB::DBUtil::CT_Double:
							case DB::DBUtil::CT_Float:
							case DB::DBUtil::CT_Decimal:
								{
									Double v1 = r->GetDblOrNAN(i);
									Double v2 = rowData[i]->ToDoubleOrNAN();
									if (!Math::NearlyEqualsDbl(v1, v2))
									{
										if (diff)
										{
											sql.AppendCmdC(CSTR(", "));
										}
										else
										{
											diff = true;
										}
										sql.AppendCol(col->GetColName()->v);
										sql.AppendCmdC(CSTR(" = "));
										sql.AppendNDbl(v2);
									}
								}
								break;
							case DB::DBUtil::CT_UInt16:
							case DB::DBUtil::CT_Int16:
							case DB::DBUtil::CT_UInt32:
							case DB::DBUtil::CT_Int32:
							case DB::DBUtil::CT_Byte:
								{
									Int32 v1 = r->GetInt32(i);
									Int32 v2 = rowData[i]->ToInt32();
									if (v1 != v2)
									{
										if (diff)
										{
											sql.AppendCmdC(CSTR(", "));
										}
										else
										{
											diff = true;
										}
										sql.AppendCol(col->GetColName()->v);
										sql.AppendCmdC(CSTR(" = "));
										sql.AppendInt32(v2);
									}
								}
								break;
							case DB::DBUtil::CT_UInt64:
							case DB::DBUtil::CT_Int64:
								{
									Int64 v1 = r->GetInt64(i);
									Int64 v2 = rowData[i]->ToInt64();
									if (v1 != v2)
									{
										if (diff)
										{
											sql.AppendCmdC(CSTR(", "));
										}
										else
										{
											diff = true;
										}
										sql.AppendCol(col->GetColName()->v);
										sql.AppendCmdC(CSTR(" = "));
										sql.AppendInt64(v2);
									}
								}
								break;
							case DB::DBUtil::CT_Bool:
								{
									Bool v1 = r->GetBool(i);
									Bool v2 = rowData[i]->v[0] == 't' || rowData[i]->v[0] == 'T' || rowData[i]->ToInt32() != 0;
									if (v1 != v2)
									{
										if (diff)
										{
											sql.AppendCmdC(CSTR(", "));
										}
										else
										{
											diff = true;
										}
										sql.AppendCol(col->GetColName()->v);
										sql.AppendCmdC(CSTR(" = "));
										sql.AppendBool(v2);
									}
								}
								break;
							case DB::DBUtil::CT_Vector:
								{
									NN<Math::Geometry::Vector2D> vec1;
									NN<Math::Geometry::Vector2D> vec2;
									if (r->GetVector(i).SetTo(vec1))
									{
										vec1->SetSRID(srid);
										Math::WKTReader reader(vec1->GetSRID());
										if (!reader.ParseWKT(rowData[i]->v).SetTo(vec2))
										{
										}
										else
										{
											if (!vec1->Equals(vec2, false, true, no3DGeometry))
											{
												if (diff)
												{
													sql.AppendCmdC(CSTR(", "));
												}
												else
												{
													diff = true;
												}
												sql.AppendCol(col->GetColName()->v);
												sql.AppendCmdC(CSTR(" = "));
												sql.AppendVector(vec2.Ptr());
											}
											vec2.Delete();
										}
										vec1.Delete();
									}
								}
								break;
							case DB::DBUtil::CT_Binary:
								break;
							case DB::DBUtil::CT_UUID:
								{
									Data::UUID uuid1;
									Data::UUID uuid2;
									r->GetUUID(i, uuid1);
									uuid2.SetValue(rowData[i]->ToCString());
									if (!uuid1.Equals(uuid2))
									{
									}
								}
								break;
							case DB::DBUtil::CT_Unknown:
							default:
								break;
							}
						}
						i++;
					}
					if (diff)
					{
						sql.AppendCmdC(CSTR(" where "));
						if (table->GetCol(keyCol1).SetTo(col))
							sql.AppendCol(col->GetColName()->v);
						sql.AppendCmdC(CSTR(" = "));
						if (intKey)
						{
							sql.AppendInt64(id1->ToInt64());
						}
						else
						{
							sql.AppendStr(id1);
						}
						if (keyCol2 != INVALID_INDEX)
						{
							sql.AppendCmdC(CSTR(" and "));
							if (table->GetCol(keyCol2).SetTo(col))
								sql.AppendCol(col->GetColName()->v);
							sql.AppendCmdC(CSTR(" = "));
							sql.AppendStr(id2);
						}
						if (!NextSQL(sql.ToCString(), sess))
						{
							id1->Release();
							OPTSTR_DEL(id2);
							succ = false;
							break;
						}
					}
				}
				else
				{
					sql.Clear();
					sql.AppendCmdC(CSTR("delete from "));
					Text::CStringNN schema;
					if (this->schema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
					{
						sql.AppendCol(schema.v);
						sql.AppendCmdC(CSTR("."));
					}
					sql.AppendCol(this->table.v);
					sql.AppendCmdC(CSTR(" where "));
					if (table->GetCol(keyCol1).SetTo(col))
						sql.AppendCol(col->GetColName()->v);
					sql.AppendCmdC(CSTR(" = "));
					if (intKey)
					{
						sql.AppendInt64(id1->ToInt64());
					}
					else
					{
						sql.AppendStr(id1);
					}
					if (keyCol2 != INVALID_INDEX)
					{
						sql.AppendCmdC(CSTR(" and "));
						if (table->GetCol(keyCol2).SetTo(col))
							sql.AppendCol(col->GetColName()->v);
						sql.AppendCmdC(CSTR(" = "));
						sql.AppendStr(id2);
					}
					if (!NextSQL(sql.ToCString(), sess))
					{
						id1->Release();
						OPTSTR_DEL(id2);
						succ = false;
						break;
					}
				}
				id1->Release();
				OPTSTR_DEL(id2);
			}
			this->db->CloseReader(r);

			if (succ)
			{
				Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(idList, idList);
				k = 0;
				srcCnt = csvData.GetCount();
				while (k < srcCnt)
				{
					if (csvData.GetKey(k).SetTo(s) && idList.SortedIndexOf(s) < 0)
					{
						rowData = csvData.GetItem(k);
						sql.Clear();
						sql.AppendCmdC(CSTR("insert into "));
						Text::CStringNN schema;
						if (this->schema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
						{
							sql.AppendCol(schema.v);
							sql.AppendCmdC(CSTR("."));
						}
						sql.AppendCol(this->table.v);
						sql.AppendCmdC(CSTR(" ("));
						colFound = false;
						i = 0;
						while (i < dbCnt)
						{
							if (this->colInd.GetItem(i) != INVALID_INDEX && table->GetCol(i).SetTo(col))
							{
								if (colFound) sql.AppendCmdC(CSTR(", "));
								colFound = true;
								sql.AppendCol(col->GetColName()->v);
							}
							i++;
						}
						sql.AppendCmdC(CSTR(") values ("));
						colFound = false;
						i = 0;
						while (i < dbCnt)
						{
							if (this->colInd.GetItem(i) != INVALID_INDEX && table->GetCol(i).SetTo(col))
							{
								if (colFound) sql.AppendCmdC(CSTR(", "));
								colFound = true;
								AppendCol(sql, col, rowData[i], this->connTz, srid);
							}
							i++;
						}
						sql.AppendCmdC(CSTR(")"));
						if (!NextSQL(sql.ToCString(), sess))
						{
							succ = false;
							break;
						}
					}
					k++;
				}
			}
			idList.FreeAll();
		}
	}
	
	k = csvData.GetCount();
	while (k-- > 0)
	{
		rowData = csvData.GetItem(k);
		i = dbCnt;
		while (i-- > 0)
		{
			SDEL_STRING(rowData[i]);
		}
		MemFree(rowData);
	}
	srcDBCond.Delete();
	dataDBCond.Delete();
	table.Delete();
	return succ;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::NextSQL(Text::CStringNN sql, NN<SQLSession> sess)
{
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
			this->UpdateStatus(sess);
			return true;
		}
		Text::StringBuilderUTF8 sb;
		db->GetLastErrorMsg(sb);
		this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
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
					this->UpdateStatus(sess);
					return true;
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					db->GetLastErrorMsg(sb);
					this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
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
					this->UpdateStatus(sess);
					return true;
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					db->GetLastErrorMsg(sb);
					this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
					return false;
				}
			}
			return true;
		}
		else if (db->ExecuteNonQuery(sql) >= 0)
		{
			sess->totalCnt++;
			this->UpdateStatus(sess);
			return true;
		}
		Text::StringBuilderUTF8 sb;
		db->GetLastErrorMsg(sb);
		this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
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

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::AppendCol(NN<DB::SQLBuilder> sql, NN<DB::ColDef> col, Optional<Text::String> s, Int8 tzQhr, UInt32 srid)
{
	NN<Text::String> nns;
	if (!s.SetTo(nns))
	{
		sql->AppendStr(0);
		return;
	}

	switch (col->GetColType())
	{
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
		sql->AppendStr(nns);
		break;
	case DB::DBUtil::CT_Date:
		sql->AppendDate(Data::Timestamp::FromStr(nns->ToCString(), tzQhr).ToDate());
		break;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
		sql->AppendTS(Data::Timestamp::FromStr(nns->ToCString(), tzQhr));
		break;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
		sql->AppendDbl(nns->ToDoubleOrNAN());
		break;
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
		sql->AppendInt32(nns->ToInt32());
		break;
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Int64:
		sql->AppendInt64(nns->ToInt64());
		break;
	case DB::DBUtil::CT_Bool:
		if (nns->v[0] >= '0' && nns->v[0] <= '9')
		{
			sql->AppendBool(nns->ToInt32() != 0);
		}
		else
		{
			sql->AppendBool(nns->v[0] == 't' || nns->v[0] == 'T');
		}
		break;
	case DB::DBUtil::CT_Vector:
		if (nns->StartsWith(UTF8STRC("0x")))
		{
			UInt8 *buff = MemAlloc(UInt8, nns->leng >> 1);
			UIntOS buffSize = Text::StrHex2Bytes(&nns->v[2], buff);
			Optional<Math::Geometry::Vector2D> vec2 = Math::MSGeography::ParseBinary(buff, buffSize, 0);
			MemFree(buff);
			sql->AppendVector(vec2);
			vec2.Delete();
		}
		else
		{
			Optional<Math::Geometry::Vector2D> vec2;
			Math::WKTReader reader((srid == 0)?col->GetGeometrySRID():srid);
			vec2 = reader.ParseWKT(nns->v);
			sql->AppendVector(vec2);
			vec2.Delete();
		}
		break;
	case DB::DBUtil::CT_Binary:
		break;
	case DB::DBUtil::CT_UUID:
		break;
	case DB::DBUtil::CT_Unknown:
	default:
		break;
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
	this->grpData->SetRect(0, 72, 800, 240, false);
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
	this->lblDataTable = ui->NewLabel(this->grpData, CSTR("Table"));
	this->lblDataTable->SetRect(0, 72, 100, 23, false);
	this->cboDataTable = ui->NewComboBox(this->grpData, false);
	this->cboDataTable->SetRect(100, 72, 200, 23, false);
	this->cboDataTable->HandleSelectionChange(OnDataTableChg, this);
	this->lblKeyCol = ui->NewLabel(this->grpData, CSTR("Key Column"));
	this->lblKeyCol->SetRect(0, 96, 100, 23, false);
	this->cboKeyCol1 = ui->NewComboBox(this->grpData, false);
	this->cboKeyCol1->SetRect(100, 96, 200, 23, false);
	this->cboKeyCol2 = ui->NewComboBox(this->grpData, false);
	this->cboKeyCol2->SetRect(300, 96, 200, 23, false);
	this->cboKeyCol2->AddItem(CSTR("No second key"), (void*)(INVALID_INDEX));
	this->lblNullCol = ui->NewLabel(this->grpData, CSTR("Null Column"));
	this->lblNullCol->SetRect(0, 120, 100, 23, false);
	this->cboNullCol = ui->NewComboBox(this->grpData, false);
	this->cboNullCol->SetRect(100, 120, 200, 23, false);
	this->cboNullCol->AddItem(CSTR("Empty"), 0);
	this->cboNullCol->AddItem(CSTR("\"NULL\""), 0);
	this->cboNullCol->AddItem(CSTR("\"null\""), 0);
	this->cboNullCol->AddItem(CSTR("No Null"), 0);
	this->cboNullCol->SetSelectedIndex(0);
	this->lblAssignCol = ui->NewLabel(this->grpData, CSTR("Assign Columns"));
	this->lblAssignCol->SetRect(0, 144, 100, 23, false);
	this->btnAssignCol = ui->NewButton(this->grpData, CSTR("Assign"));
	this->btnAssignCol->SetRect(100, 144, 75, 23, false);
	this->btnAssignCol->HandleButtonClick(OnAssignColClicked, this);
	this->lblDataFilter = ui->NewLabel(this->grpData, CSTR("Data Filter"));
	this->lblDataFilter->SetRect(0, 168, 100, 23, false);
	this->txtDataFilter = ui->NewTextBox(this->grpData, CSTR(""));
	this->txtDataFilter->SetRect(100, 168, 200, 23, false);
	this->btnDataCheck = ui->NewButton(this->grpData, CSTR("Check"));
	this->btnDataCheck->SetRect(100, 192, 75, 23, false);
	this->btnDataCheck->HandleButtonClick(OnDataCheckClk, this);
	this->lblDataFileRow = ui->NewLabel(*this, CSTR("Data Rows"));
	this->lblDataFileRow->SetRect(0, 312, 100, 23, false);
	this->txtDataFileRow = ui->NewTextBox(*this, CSTR("0"));
	this->txtDataFileRow->SetRect(100, 312, 200, 23, false);
	this->txtDataFileRow->SetReadOnly(true);
	this->lblNoChg = ui->NewLabel(*this, CSTR("No Changes"));
	this->lblNoChg->SetRect(0, 336, 100, 23, false);
	this->txtNoChg = ui->NewTextBox(*this, CSTR("0"));
	this->txtNoChg->SetRect(100, 336, 200, 23, false);
	this->txtNoChg->SetReadOnly(true);
	this->lblUpdated = ui->NewLabel(*this, CSTR("Updated rows"));
	this->lblUpdated->SetRect(0, 360, 100, 23, false);
	this->txtUpdated = ui->NewTextBox(*this, CSTR("0"));
	this->txtUpdated->SetRect(100, 360, 200, 23, false);
	this->txtUpdated->SetReadOnly(true);
	this->lblNewRow = ui->NewLabel(*this, CSTR("New rows"));
	this->lblNewRow->SetRect(0, 384, 100, 23, false);
	this->txtNewRow = ui->NewTextBox(*this, CSTR("0"));
	this->txtNewRow->SetRect(100, 384, 200, 23, false);
	this->txtNewRow->SetReadOnly(true);
	this->lblDeletedRow = ui->NewLabel(*this, CSTR("Deleted rows"));
	this->lblDeletedRow->SetRect(0, 408, 100, 23, false);
	this->txtDeletedRow = ui->NewTextBox(*this, CSTR("0"));
	this->txtDeletedRow->SetRect(100, 408, 200, 23, false);
	this->txtDeletedRow->SetReadOnly(true);

	this->lblDBType = ui->NewLabel(*this, CSTR("SQL Type"));
	this->lblDBType->SetRect(0, 456, 100, 23, false);
	this->cboDBType = ui->NewComboBox(*this, false);
	this->cboDBType->SetRect(100, 456, 200, 23, false);
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
	this->chkAxisAware->SetRect(300, 456, 150, 23, false);
	this->chkMultiRow = ui->NewCheckBox(*this, CSTR("Multi-Row Insert"), true);
	this->chkMultiRow->SetRect(100, 480, 150, 23, false);
	this->btnSQL = ui->NewButton(*this, CSTR("Generate SQL"));
	this->btnSQL->SetRect(100, 504, 75, 23, false);
	this->btnSQL->HandleButtonClick(OnSQLClicked, this);
	this->btnExecute = ui->NewButton(*this, CSTR("Execute SQL"));
	this->btnExecute->SetRect(180, 504, 75, 23, false);
	this->btnExecute->HandleButtonClick(OnExecuteClicked, this);
	this->lblStatTime = ui->NewLabel(*this, CSTR("Time Used"));
	this->lblStatTime->SetRect(0, 528, 100, 23, false);
	this->txtStatTime = ui->NewTextBox(*this, CSTR(""));
	this->txtStatTime->SetRect(100, 528, 150, 23 ,false);
	this->txtStatTime->SetReadOnly(true);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(0, 552, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(100, 552, 300, 23 ,false);
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
