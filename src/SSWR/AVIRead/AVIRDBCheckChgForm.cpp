#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
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
#include "SSWR/AVIRead/AVIRDBCheckChgForm.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"

#define SRID 4326

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataFileClk(void *userObj)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDataFile->GetText(sb);
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBCheckChg", false);
	dlg.SetAllowMultiSel(false);
	if (sb.GetLength() > 0)
	{
		dlg.SetFileName(sb.ToCString());
	}
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::ReadingDB);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->LoadDataFile(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->LoadDataFile(files[i]->ToCString()))
		{
			return;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnDataCheckClk(void *userObj)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	if (!me->CheckDataFile())
	{
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnSQLClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (me->dataFile == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input Data file first"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::SQLType sqlType = (DB::SQLType)(OSInt)me->cboDBType->GetSelectedItem();
	Bool axisAware = me->chkAxisAware->IsChecked();
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBCheckChgSQL", true);
	dlg.SetAllowMultiSel(false);
	dlg.AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	sptr = sbuff;
	if (me->schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
	{
		sptr = me->schema.ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = me->table.ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".sql"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		Bool succ;
		SQLSession sess;
		sess.mode = 0;
		sess.totalCnt = 0;
		sess.startTime = Data::Timestamp::UtcNow();
		sess.lastUpdateTime = sess.startTime;
		sess.db = 0;
		
		{
			IO::FileStream fs(dlg.GetFileName()->ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			sess.stm = &fs;
			succ = me->GenerateSQL(sqlType, axisAware, &sess);
		}
		Double t = Data::Timestamp::UtcNow().DiffSecDbl(sess.startTime);
		sptr = Text::StrDouble(sbuff, t);
		me->txtStatTime->SetText(CSTRP(sbuff, sptr));
		if (succ)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Success, "));
			sb.AppendUOSInt(sess.totalCnt);
			sb.AppendC(UTF8STRC(" SQL generated"));
			me->txtStatus->SetText(sb.ToCString());
		}
		else
		{
			IO::Path::DeleteFile(dlg.GetFileName()->v);
			me->txtStatus->SetText(CSTR("Error in generating SQL"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnExecuteClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	if (me->dataFile == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please load data file first"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::DBConn *db;
	if (me->db->IsFullConn())
	{
		db = (DB::DBConn*)me->db;
	}
	else if (me->db->IsDBTool())
	{
		db = ((DB::ReadingDBTool*)me->db)->GetDBConn().Ptr();
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Connection does not support SQL Execution"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::SQLType sqlType = (DB::SQLType)(OSInt)me->cboDBType->GetSelectedItem();
	Bool axisAware = me->chkAxisAware->IsChecked();
	Bool succ;
	SQLSession sess;
	sess.totalCnt = 0;
	sess.startTime = Data::Timestamp::UtcNow();
	sess.lastUpdateTime = sess.startTime;
	sess.stm = 0;
	sess.db = db;
	if (me->chkMultiRow->IsChecked())
	{
		Text::StringBuilderUTF8 sbInsert;
		sess.mode = 2;
		sess.nInsert = 0;
		sess.sbInsert = sbInsert;
		succ = me->GenerateSQL(sqlType, axisAware, &sess);
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
		succ = me->GenerateSQL(sqlType, axisAware, &sess);
	}
	Double t = Data::Timestamp::UtcNow().DiffSecDbl(sess.startTime);
	Text::StringBuilderUTF8 sb;
	sb.AppendDouble(t);
	me->txtStatTime->SetText(sb.ToCString());
	if (succ)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Success, "));
		sb.AppendUOSInt(sess.totalCnt);
		sb.AppendC(UTF8STRC(" SQL executed"));
		me->txtStatus->SetText(sb.ToCString());
	}
	else
	{
		me->txtStatus->SetText(CSTR("Error in executing SQL"));
	}
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::LoadDataFile(Text::CStringNN fileName)
{
	DB::TableDef *table = this->db->GetTableDef(this->schema, this->table);
	if (table == 0)
	{
		this->ui->ShowMsgOK(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	DEL_CLASS(table);
	Int8 csvTZ = this->db->GetTzQhr();
	Bool noHeader = this->chkNoHeader->IsChecked();
	if (fileName.EndsWithICase(UTF8STRC(".csv")))
	{
		DB::CSVFile *csv;
		NEW_CLASS(csv, DB::CSVFile(fileName, 65001))
		if (noHeader) csv->SetNoHeader(true);
		NotNullPtr<DB::DBReader> r;
		if (!r.Set(csv->QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0)))
		{
			DEL_CLASS(csv);
			this->ui->ShowMsgOK(CSTR("Error in reading CSV file"), CSTR("Check Table Changes"), this);
			return false;
		}
		csv->CloseReader(r);
		this->txtDataFile->SetText(fileName);
		SDEL_CLASS(this->dataFile);
		this->dataFile = csv;
		this->dataFileNoHeader = noHeader;
		this->dataFileTz = csvTZ;
	}
	else
	{
		IO::Path::PathType pt = IO::Path::GetPathType(fileName);
		DB::ReadingDB *db = 0;
		if (pt == IO::Path::PathType::File)
		{
			IO::StmData::FileData fd(fileName, false);
			db = (DB::ReadingDB*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ReadingDB);
		}
		else if (pt == IO::Path::PathType::Directory)
		{
			IO::DirectoryPackage pkg(fileName);
			NotNullPtr<Parser::ParserList> parsers = this->core->GetParserList();
			db = (DB::ReadingDB*)parsers->ParseObjectType(pkg, IO::ParserType::ReadingDB);
		}
		if (db)
		{
			this->txtDataFile->SetText(fileName);
			SDEL_CLASS(this->dataFile);
			this->dataFile = db;
			this->dataFileNoHeader = false;
			this->dataFileTz = csvTZ;
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("Check Table Changes"), this);
			return false;
		}
	}
	Data::ArrayListStringNN tableNames;
	this->dataFile->QueryTableNames(CSTR_NULL, tableNames);
	this->cboDataTable->ClearItems();
	if (tableNames.GetCount() > 0)
	{
		UOSInt tableIndex = 0;
		NotNullPtr<Text::String> name;
		Data::ArrayIterator<NotNullPtr<Text::String>> it = tableNames.Iterator();
		UOSInt i = 0;
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
	Text::CString nullStr = this->GetNullText();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (this->dataFile == 0)
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
	Data::QueryConditions *srcDBCond = 0;
	Data::QueryConditions *dataDBCond = 0;
	Text::StringBuilderUTF8 sbFilter;
	this->txtSrcFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		srcDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType());
		if (srcDBCond == 0)
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
		if (dataDBCond == 0)
		{
			SDEL_CLASS(srcDBCond);
			this->ui->ShowMsgOK(CSTR("Error in parsing data filter"), CSTR("Check Table Changes"), this);
			return false;
		}
	}
	Int8 csvTZ = this->db->GetTzQhr();
	this->dataFileTz = csvTZ;
	Text::StringBuilderUTF8 sbIgnore;
	UOSInt nIgnore;
	Text::PString ignoreArr[10];
	this->txtIgnoreCol->GetText(sbIgnore);
	if (sbIgnore.GetLength() > 0)
	{
		nIgnore = Text::StrSplitTrimP(ignoreArr, 10, sbIgnore, ',');
	}
	else
	{
		nIgnore = 0;
	}
	DB::TableDef *table = this->db->GetTableDef(this->schema, this->table);
	if (table == 0)
	{
		SDEL_CLASS(srcDBCond);
		SDEL_CLASS(dataDBCond);
		this->ui->ShowMsgOK(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	Data::ArrayList<UOSInt> colInd;
	UOSInt keyCol = (UOSInt)this->cboKeyCol->GetSelectedItem();
	UOSInt keyDCol;
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		colInd.Add(this->dataFileNoHeader?i:INVALID_INDEX);
		i++;
	}
	UOSInt k;
	UOSInt l;
	Data::FastStringMap<Text::String**> csvData;
	Text::String** rowData;
	NotNullPtr<DB::ColDef> col;
	Bool found;
	NotNullPtr<DB::DBReader> r;
	if (!r.Set(this->dataFile->QueryTableData(CSTR_NULL, sbTable.ToCString(), 0, 0, 0, CSTR_NULL, dataDBCond)))
	{
		SDEL_CLASS(srcDBCond);
		SDEL_CLASS(dataDBCond);
		DEL_CLASS(table);
		this->ui->ShowMsgOK(CSTR("Error in reading data file"), CSTR("Check Table Changes"), this);
		return false;
	}
	l = r->ColCount();
	if (!this->dataFileNoHeader)
	{
		k = 0;
		while (k < l)
		{
			sptr = r->GetName(k, sbuff);
			found = false;
			if (sptr)
			{
				i = nIgnore;
				while (i-- > 0)
				{
					if (ignoreArr[i].EqualsICase(sbuff, (UOSInt)(sptr - sbuff)))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					i = 0;
					while (i < j)
					{
						if (table->GetCol(i).SetTo(col) && col->GetColName()->EqualsICase(sbuff, (UOSInt)(sptr - sbuff)))
						{
							found = true;
							colInd.SetItem(i, k);
							break;
						}
						i++;
					}
				}
			}
			if (!found)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Data File Column "));
				sb.AppendUOSInt(k);
				if (sptr)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.Append(CSTRP(sbuff, sptr));
					sb.AppendUTF8Char(')');
				}
				sb.AppendC(UTF8STRC(" not found"));
				this->dataFile->CloseReader(r);
				DEL_CLASS(table);
				SDEL_CLASS(srcDBCond);
				SDEL_CLASS(dataDBCond);
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
				return false;
			}
			k++;
		}
	}
	keyDCol = colInd.GetItem(keyCol);

	if (this->dataFileNoHeader)
	{
		if (j != l)
		{
			this->ui->ShowMsgOK(CSTR("Column Count does not match"), CSTR("Check Table Changes"), this);
			this->dataFile->CloseReader(r);
			DEL_CLASS(table);
			SDEL_CLASS(srcDBCond);
			SDEL_CLASS(dataDBCond);
			return false;
		}
	}
	else
	{
		if (keyCol != INVALID_INDEX && keyDCol == INVALID_INDEX)
		{
			this->ui->ShowMsgOK(CSTR("Key Column not found in data file"), CSTR("Check Table Changes"), this);
			this->dataFile->CloseReader(r);
			DEL_CLASS(table);
			SDEL_CLASS(srcDBCond);
			SDEL_CLASS(dataDBCond);
			return false;
		}
/*		i = 0;
		while (i < j)
		{
			if (colInd.GetItem(i) == INVALID_INDEX)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Column "));
				sptr = table->GetCol(i)->GetColName()->ConcatTo(sptr);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" not found in Data File"));
				this->ui->ShowMsgOK(CSTRP(sbuff, sptr), CSTR("Check Table Changes"), this);
				this->dataFile->CloseReader(r);
				DEL_CLASS(table);
				SDEL_CLASS(dbCond);
				SDEL_CLASS(dataDBCond);
				return false;
			}
			i++;
		}*/
	}
	UOSInt dataFileRowCnt = 0;
	UOSInt noChgCnt = 0;
	UOSInt updateCnt = 0;
	UOSInt newRowCnt = 0;
	UOSInt delRowCnt = 0;
	Bool succ = true;
	Text::String *s;
	Text::String *id;
	while (r->ReadNext())
	{
		dataFileRowCnt++;
		if (r->ColCount() >= l)
		{
			if (keyCol == INVALID_INDEX)
			{
				newRowCnt++;
			}
			else
			{
				s = r->GetNewStr(keyDCol);
				if (s->leng == 0 || s->Equals(UTF8STRC("0")))
				{
					s->Release();
					newRowCnt++;
				}
				else
				{
					rowData = MemAlloc(Text::String*, j);
					i = 0;
					while (i < j)
					{
						k = colInd.GetItem(i);
						rowData[i] = 0;
						if (k != INVALID_INDEX)
						{
							rowData[i] = r->GetNewStr(k);
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
					rowData = csvData.Put(s, rowData);
					s->Release();
					if (rowData)
					{
						i = j;
						while (i-- > 0)
						{
							SDEL_STRING(rowData[i]);
						}
						MemFree(rowData);

						this->ui->ShowMsgOK(CSTR("Data File Key duplicate"), CSTR("Check Table Changes"), this);
						succ = false;
						break;
					}
				}
			}
		}
	}
	this->dataFile->CloseReader(r);

	if (succ && keyCol != INVALID_INDEX)
	{
		if (!r.Set(this->db->QueryTableData(this->schema, this->table, 0, 0, 0, CSTR_NULL, srcDBCond)))
		{
			this->ui->ShowMsgOK(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
			succ = false;
		}
		else
		{
			Data::ArrayListString idList;
			while (r->ReadNext())
			{
				id = r->GetNewStr(keyCol);
				if (id == 0)
				{
					this->ui->ShowMsgOK(CSTR("Source Key is null"), CSTR("Check Table Changes"), this);
					succ = false;
					break;
				}
				if (idList.SortedIndexOf(id) >= 0)
				{
					sbFilter.ClearStr();
					sbFilter.AppendC(UTF8STRC("Source Key duplicate ("));
					sbFilter.Append(id);
					sbFilter.AppendUTF8Char(')');
					this->ui->ShowMsgOK(sbFilter.ToCString(), CSTR("Check Table Changes"), this);
					succ = false;
					break;
				}
				rowData = csvData.Get(id);
				idList.SortedInsert(id);
				if (rowData)
				{
					Bool diff = false;
					i = 0;
					while (i < j)
					{
						if (colInd.GetItem(i) == INVALID_INDEX)
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
								s = r->GetNewStr(i);
								if (!s->Equals(rowData[i]->v, rowData[i]->leng))
								{
									diff = true;
								}
								s->Release();
								break;
							case DB::DBUtil::CT_Date:
							case DB::DBUtil::CT_DateTime:
							case DB::DBUtil::CT_DateTimeTZ:
								{
									Data::Timestamp ts1 = r->GetTimestamp(i);
									Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->dataFileTz);
									if (ts1.DiffSec(ts2) != 0)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Float:
								{
									Double v1 = r->GetDbl(i);
									Double v2 = rowData[i]->ToDouble();
									if (!Math::NearlyEquals(v1, v2, 0.000001))
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Decimal:
							case DB::DBUtil::CT_Double:
								{
									Double v1 = r->GetDbl(i);
									Double v2 = rowData[i]->ToDouble();
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
									NotNullPtr<Math::Geometry::Vector2D> vec1;
									if (vec1.Set(r->GetVector(i)))
									{
										vec1->SetSRID(col->GetGeometrySRID());
										NotNullPtr<Math::Geometry::Vector2D> vec2;
										Math::WKTReader reader(vec1->GetSRID());
										if (!vec2.Set(reader.ParseWKT(rowData[i]->v)))
										{
											diff = true;
										}
										else
										{
											if (!vec1->Equals(vec2, false, true))
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
			LIST_FREE_STRING(&idList);
		}
	}

	k = csvData.GetCount();
	while (k-- > 0)
	{
		rowData = csvData.GetItem(k);
		i = j;
		while (i-- > 0)
		{
			SDEL_STRING(rowData[i]);
		}
		MemFree(rowData);
	}
	DEL_CLASS(table);
	SDEL_CLASS(srcDBCond);
	SDEL_CLASS(dataDBCond);
	if (succ)
	{
		sptr = Text::StrUOSInt(sbuff, dataFileRowCnt);
		this->txtDataFileRow->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, noChgCnt);
		this->txtNoChg->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, updateCnt);
		this->txtUpdated->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, newRowCnt);
		this->txtNewRow->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, delRowCnt);
		this->txtDeletedRow->SetText(CSTRP(sbuff, sptr));
		return true;
	}
	else
	{
		return false;
	}
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::GenerateSQL(DB::SQLType sqlType, Bool axisAware, SQLSession *sess)
{
	Text::CString nullStr = this->GetNullText();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (this->dataFile == 0)
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
	Data::QueryConditions *srcDBCond = 0;
	Data::QueryConditions *dataDBCond = 0;
	Text::StringBuilderUTF8 sbFilter;
	this->txtSrcFilter->GetText(sbFilter);
	if (sbFilter.GetLength() > 0)
	{
		srcDBCond = Data::QueryConditions::ParseStr(sbFilter.ToCString(), this->GetDBSQLType());
		if (srcDBCond == 0)
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
		if (dataDBCond == 0)
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing data filter"), CSTR("Check Table Changes"), this);
			SDEL_CLASS(srcDBCond);
			return false;
		}
	}
	Text::StringBuilderUTF8 sbIgnore;
	UOSInt nIgnore;
	Text::PString ignoreArr[10];
	this->txtIgnoreCol->GetText(sbIgnore);
	if (sbIgnore.GetLength() > 0)
	{
		nIgnore = Text::StrSplitTrimP(ignoreArr, 10, sbIgnore, ',');
	}
	else
	{
		nIgnore = 0;
	}
	DB::TableDef *table = this->db->GetTableDef(this->schema, this->table);
	if (table == 0)
	{
		this->ui->ShowMsgOK(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		SDEL_CLASS(srcDBCond);
		SDEL_CLASS(dataDBCond);
		return false;
	}
	Bool succ = true;
	Data::ArrayList<UOSInt> colInd;
	Bool intKey = false;
	NotNullPtr<DB::ColDef> col;
	UOSInt keyCol = (UOSInt)this->cboKeyCol->GetSelectedItem();
	UOSInt keyDCol;
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		colInd.Add(this->dataFileNoHeader?i:INVALID_INDEX);
		if (i == keyCol && table->GetCol(i).SetTo(col))
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
			case DB::DBUtil::CT_Vector:
			case DB::DBUtil::CT_UUID:
				break;
			}
		}
		i++;
	}
	UOSInt k;
	UOSInt l;
	Bool found;
	Data::FastStringMap<Text::String**> csvData;
	Text::String** rowData;
	NotNullPtr<DB::DBReader> r;
	if (!r.Set(this->dataFile->QueryTableData(CSTR_NULL, sbTable.ToCString(), 0, 0, 0, CSTR_NULL, 0)))
	{
		DEL_CLASS(table);
		SDEL_CLASS(srcDBCond);
		SDEL_CLASS(dataDBCond);
		this->ui->ShowMsgOK(CSTR("Error in reading data file"), CSTR("Check Table Changes"), this);
		return false;
	}
	l = r->ColCount();
	if (!this->dataFileNoHeader)
	{
		k = 0;
		while (k < l)
		{
			found = false;
			sptr = r->GetName(k, sbuff);
			if (sptr)
			{
				i = nIgnore;
				while (i-- > 0)
				{
					if (ignoreArr[i].EqualsICase(sbuff, (UOSInt)(sptr - sbuff)))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					i = 0;
					while (i < j)
					{
						if (table->GetCol(i).SetTo(col) && col->GetColName()->EqualsICase(sbuff, (UOSInt)(sptr - sbuff)))
						{
							found = true;
							colInd.SetItem(i, k);
							break;
						}
						i++;
					}
				}
			}
			if (!found)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Data File Column "));
				sb.AppendUOSInt(k);
				if (sptr)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.Append(CSTRP(sbuff, sptr));
					sb.AppendUTF8Char(')');
				}
				sb.AppendC(UTF8STRC(" not found"));
				this->dataFile->CloseReader(r);
				DEL_CLASS(table);
				SDEL_CLASS(srcDBCond);
				SDEL_CLASS(dataDBCond);
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("Check Table Changes"), this);
				return false;
			}
			k++;
		}
	}
	keyDCol = colInd.GetItem(keyCol);

	if (this->dataFileNoHeader)
	{
		if (j != l)
		{
			this->ui->ShowMsgOK(CSTR("Column Count does not match"), CSTR("Check Table Changes"), this);
			this->dataFile->CloseReader(r);
			DEL_CLASS(table);
			SDEL_CLASS(srcDBCond);
			SDEL_CLASS(dataDBCond);
			return false;
		}
	}
	else
	{
		if (keyCol != INVALID_INDEX && keyDCol == INVALID_INDEX)
		{
			this->ui->ShowMsgOK(CSTR("Key Column not found in data file"), CSTR("Check Table Changes"), this);
			this->dataFile->CloseReader(r);
			DEL_CLASS(table);
			SDEL_CLASS(srcDBCond);
			SDEL_CLASS(dataDBCond);
			return false;
		}
/*		i = 0;
		while (i < j)
		{
			if (colInd.GetItem(i) == INVALID_INDEX)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Column "));
				sptr = table->GetCol(i)->GetColName()->ConcatTo(sptr);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" not found in Data File"));
				this->ui->ShowMsgOK(CSTRP(sbuff, sptr), CSTR("Check Table Changes"), this);
				this->dataFile->CloseReader(r);
				DEL_CLASS(table);
				SDEL_CLASS(srcDBCond);
				SDEL_CLASS(dataDBCond);
				return false;
			}
			i++;
		}*/
	}
	DB::SQLBuilder sql(sqlType, axisAware, Data::DateTimeUtil::GetLocalTzQhr());
	Bool genInsert;
	Bool colFound;
	Text::String *s;
	Text::String *id;
	while (r->ReadNext())
	{
		if (r->ColCount() >= l)
		{
			genInsert = false;
			if (keyCol == INVALID_INDEX)
			{
				genInsert = true;
			}
			else
			{
				s = r->GetNewStr(keyDCol);
				if (s->leng == 0 || s->Equals(UTF8STRC("0")))
				{
					s->Release();
					genInsert = true;
				}
				else
				{
					rowData = MemAlloc(Text::String*, j);
					i = 0;
					while (i < j)
					{
						k = colInd.GetItem(i);
						if (k == INVALID_INDEX)
						{
							rowData[i] = 0;
						}
						else
						{
							rowData[i] = r->GetNewStr(colInd.GetItem(i));
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
					rowData = csvData.Put(s, rowData);
					s->Release();
					if (rowData)
					{
						i = j;
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
				if (this->schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
				{
					sql.AppendCol(this->schema.v);
					sql.AppendCmdC(CSTR("."));
				}
				sql.AppendCol(this->table.v);
				sql.AppendCmdC(CSTR(" ("));
				colFound = false;
				i = 0;
				while (i < j)
				{
					if (i != keyCol && colInd.GetItem(i) != INVALID_INDEX && table->GetCol(i).SetTo(col))
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
				while (i < j)
				{
					if (i != keyCol && colInd.GetItem(i) != INVALID_INDEX && table->GetCol(i).SetTo(col))
					{
						if (colFound) sql.AppendCmdC(CSTR(", "));
						colFound = true;
						s = r->GetNewStr(colInd.GetItem(i));
						if (s && s->Equals(nullStr.v, nullStr.leng))
						{
							sql.AppendStr(0);
						}
						else
						{
							AppendCol(&sql, col->GetColType(), s, this->dataFileTz);
						}
						SDEL_STRING(s);
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
	this->dataFile->CloseReader(r);

	if (keyCol != INVALID_INDEX)
	{
		if (!r.Set(this->db->QueryTableData(this->schema, this->table, 0, 0, 0, CSTR_NULL, srcDBCond)))
		{
			this->ui->ShowMsgOK(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
		}
		else
		{
			Data::ArrayListString idList;
			while (r->ReadNext())
			{
				id = r->GetNewStr(keyCol);
				rowData = csvData.Get(id);
				idList.Add(id);
				if (rowData)
				{
					Bool diff = false;
					sql.Clear();
					sql.AppendCmdC(CSTR("update "));
					if (this->schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
					{
						sql.AppendCol(this->schema.v);
						sql.AppendCmdC(CSTR("."));
					}
					sql.AppendCol(this->table.v);
					sql.AppendCmdC(CSTR(" set "));

					i = 0;
					while (i < j)
					{
						if (colInd.GetItem(i) == INVALID_INDEX)
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
									case DB::DBUtil::CT_DateTime:
									case DB::DBUtil::CT_DateTimeTZ:
										{
											Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->dataFileTz);
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
											Double v2 = rowData[i]->ToDouble();
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
											sql.AppendDbl(v2);
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
											Math::Geometry::Vector2D *vec2;;
											Math::WKTReader reader(SRID);
											vec2 = reader.ParseWKT(rowData[i]->v);
											if (vec2 == 0)
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
												SDEL_CLASS(vec2);
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
								s = r->GetNewStr(i);
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
							case DB::DBUtil::CT_DateTime:
							case DB::DBUtil::CT_DateTimeTZ:
								{
									Data::Timestamp ts1 = r->GetTimestamp(i);
									Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), this->dataFileTz);
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
									Double v1 = r->GetDbl(i);
									Double v2 = rowData[i]->ToDouble();
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
										sql.AppendDbl(v2);
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
									NotNullPtr<Math::Geometry::Vector2D> vec1;
									NotNullPtr<Math::Geometry::Vector2D> vec2;
									if (vec1.Set(r->GetVector(i)))
									{
										vec1->SetSRID(col->GetGeometrySRID());
										Math::WKTReader reader(vec1->GetSRID());
										if (!vec2.Set(reader.ParseWKT(rowData[i]->v)))
										{
										}
										else
										{
											if (!vec1->Equals(vec2, false, true))
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
						if (table->GetCol(keyCol).SetTo(col))
							sql.AppendCol(col->GetColName()->v);
						sql.AppendCmdC(CSTR(" = "));
						if (intKey)
						{
							sql.AppendInt64(id->ToInt64());
						}
						else
						{
							sql.AppendStr(id);
						}
						if (!NextSQL(sql.ToCString(), sess))
						{
							succ = false;
							break;
						}
					}
				}
				else
				{
					sql.Clear();
					sql.AppendCmdC(CSTR("delete from "));
					if (this->schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
					{
						sql.AppendCol(this->schema.v);
						sql.AppendCmdC(CSTR("."));
					}
					sql.AppendCol(this->table.v);
					sql.AppendCmdC(CSTR(" where "));
					if (table->GetCol(keyCol).SetTo(col))
						sql.AppendCol(col->GetColName()->v);
					sql.AppendCmdC(CSTR(" = "));
					if (intKey)
					{
						sql.AppendInt64(id->ToInt64());
					}
					else
					{
						sql.AppendStr(id);
					}
					if (!NextSQL(sql.ToCString(), sess))
					{
						succ = false;
						break;
					}
				}
			}
			this->db->CloseReader(r);

			if (succ)
			{
				Data::Sort::ArtificialQuickSort::Sort(&idList, &idList);
				k = 0;
				l = csvData.GetCount();
				while (k < l)
				{
					if (idList.SortedIndexOf(csvData.GetKey(k)) < 0)
					{
						rowData = csvData.GetItem(k);
						sql.Clear();
						sql.AppendCmdC(CSTR("insert into "));
						if (this->schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
						{
							sql.AppendCol(this->schema.v);
							sql.AppendCmdC(CSTR("."));
						}
						sql.AppendCol(this->table.v);
						sql.AppendCmdC(CSTR(" ("));
						colFound = false;
						i = 0;
						while (i < j)
						{
							if (table->GetCol(i).SetTo(col))
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
						while (i < j)
						{
							if (table->GetCol(i).SetTo(col))
							{
								if (colFound) sql.AppendCmdC(CSTR(", "));
								colFound = true;
								AppendCol(&sql, col->GetColType(), rowData[i], this->dataFileTz);
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
			LIST_FREE_STRING(&idList);
		}
	}
	
	k = csvData.GetCount();
	while (k-- > 0)
	{
		rowData = csvData.GetItem(k);
		i = j;
		while (i-- > 0)
		{
			SDEL_STRING(rowData[i]);
		}
		MemFree(rowData);
	}
	SDEL_CLASS(srcDBCond);
	SDEL_CLASS(dataDBCond);
	DEL_CLASS(table);
	return succ;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::NextSQL(Text::CStringNN sql, SQLSession *sess)
{
	if (sess->mode == 0)
	{
		if (sess->stm->Write(sql.v, sql.leng) != sql.leng)
		{
			return false;
		}
		if (sess->stm->Write(UTF8STRC(";\r\n")) != 3)
		{
			return false;
		}
		sess->totalCnt++;
		return true;
	}
	else if (sess->mode == 1)
	{
		if (sess->db->ExecuteNonQuery(sql) >= 0)
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
	else if (sess->mode == 2)
	{
		if (sql.StartsWith(UTF8STRC("insert into ")))
		{
			UOSInt i = sql.IndexOf(UTF8STRC(") values ("));
			if (i == INVALID_INDEX)
			{
				if (sess->db->ExecuteNonQuery(sql) >= 0)
				{
					sess->totalCnt++;
					this->UpdateStatus(sess);
					return true;
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sess->db->GetLastErrorMsg(sb);
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
				if (sess->db->ExecuteNonQuery(sess->sbInsert->ToCString()) >= 0)
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
		else if (sess->db->ExecuteNonQuery(sql) >= 0)
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

void SSWR::AVIRead::AVIRDBCheckChgForm::UpdateStatus(SQLSession *sess)
{
	Data::Timestamp t = Data::Timestamp::UtcNow();
	if (t.DiffSecDbl(sess->lastUpdateTime) >= 1)
	{
		sess->lastUpdateTime = t;
		Text::StringBuilderUTF8 sb;
		sb.AppendUOSInt(sess->totalCnt);
		sb.AppendC(UTF8STRC(" SQL executed"));
		this->txtStatus->SetText(sb.ToCString());
		this->ui->ProcessMessages();
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::AppendCol(DB::SQLBuilder *sql, DB::DBUtil::ColType colType, Text::String *s, Int8 tzQhr)
{
	if (s == 0)
	{
		sql->AppendStr(0);
		return;
	}

	switch (colType)
	{
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
		sql->AppendStr(s);
		break;
	case DB::DBUtil::CT_Date:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
		sql->AppendTS(Data::Timestamp::FromStr(s->ToCString(), tzQhr));
		break;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
		sql->AppendDbl(s->ToDouble());
		break;
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
		sql->AppendInt32(s->ToInt32());
		break;
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Int64:
		sql->AppendInt64(s->ToInt64());
		break;
	case DB::DBUtil::CT_Bool:
		if (s->v[0] >= '0' && s->v[0] <= '9')
		{
			sql->AppendBool(s->ToInt32() != 0);
		}
		else
		{
			sql->AppendBool(s->v[0] == 't' || s->v[0] == 'T');
		}
		break;
	case DB::DBUtil::CT_Vector:
		if (s->StartsWith(UTF8STRC("0x")))
		{
			UInt8 *buff = MemAlloc(UInt8, s->leng >> 1);
			UOSInt buffSize = Text::StrHex2Bytes(&s->v[2], buff);
			Math::Geometry::Vector2D *vec2 = Math::MSGeography::ParseBinary(buff, buffSize, 0);
			MemFree(buff);
			sql->AppendVector(vec2);
			SDEL_CLASS(vec2);
		}
		else
		{
			Math::Geometry::Vector2D *vec2;
			Math::WKTReader reader(0);
			vec2 = reader.ParseWKT(s->v);
			sql->AppendVector(vec2);
			SDEL_CLASS(vec2);
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

Text::CString SSWR::AVIRead::AVIRDBCheckChgForm::GetNullText()
{
	switch (this->cboNullCol->GetSelectedIndex())
	{
	case 1:
		return CSTR("NULL");
	case 2:
		return CSTR("null");
	default:
		return CSTR("");
	}
}

DB::SQLType SSWR::AVIRead::AVIRDBCheckChgForm::GetDBSQLType()
{
	if (this->db->IsDBTool())
	{
		return ((DB::ReadingDBTool*)this->db)->GetSQLType();
	}
	else if (this->db->IsFullConn())
	{
		return ((DB::DBConn*)this->db)->GetSQLType();
	}
	else
	{
		return DB::SQLType::Unknown;
	}
}

SSWR::AVIRead::AVIRDBCheckChgForm::AVIRDBCheckChgForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, DB::ReadingDB *db, Text::CString schema, Text::CStringNN table) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Check Table Changes"));
	this->core = core;
	this->db = db;
	this->schema = schema;
	this->table = table;
	this->dataFile = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	if (schema.v == 0)
	{
		schema = CSTR("");
	}

	NEW_CLASSNN(this->lblSchema, UI::GUILabel(ui, *this, CSTR("Schema")));
	this->lblSchema->SetRect(0, 0, 100, 23, false);
	NEW_CLASSNN(this->txtSchema, UI::GUITextBox(ui, *this, schema.OrEmpty()));
	this->txtSchema->SetRect(100, 0, 200, 23, false);
	this->txtSchema->SetReadOnly(true);
	NEW_CLASSNN(this->lblTable, UI::GUILabel(ui, *this, CSTR("Table")));
	this->lblTable->SetRect(0, 24, 100, 23, false);
	NEW_CLASSNN(this->txtTable, UI::GUITextBox(ui, *this, table.OrEmpty()));
	this->txtTable->SetRect(100, 24, 200, 23, false);
	this->txtTable->SetReadOnly(true);
	NEW_CLASSNN(this->lblSrcFilter, UI::GUILabel(ui, *this, CSTR("Source Filter")));
	this->lblSrcFilter->SetRect(0, 48, 100, 23, false);
	NEW_CLASSNN(this->txtSrcFilter, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtSrcFilter->SetRect(100, 48, 200, 23, false);
	this->grpData = ui->NewGroupBox(*this, CSTR("Data Source"));
	this->grpData->SetRect(0, 72, 800, 216, false);
	NEW_CLASSNN(this->chkNoHeader, UI::GUICheckBox(ui, this->grpData, CSTR("CSV No Header"), false));
	this->chkNoHeader->SetRect(100, 0, 200, 23, false);
	NEW_CLASSNN(this->lblDataFile, UI::GUILabel(ui, this->grpData, CSTR("Data File")));
	this->lblDataFile->SetRect(0, 24, 100, 23, false);
	NEW_CLASSNN(this->txtDataFile, UI::GUITextBox(ui, this->grpData, CSTR("")));
	this->txtDataFile->SetRect(100, 24, 600, 23, false);
	this->txtDataFile->SetReadOnly(true);
	this->btnDataFile = ui->NewButton(this->grpData, CSTR("Browse"));
	this->btnDataFile->SetRect(700, 24, 75, 23, false);
	this->btnDataFile->HandleButtonClick(OnDataFileClk, this);
	NEW_CLASSNN(this->lblDataTable, UI::GUILabel(ui, this->grpData, CSTR("Table")));
	this->lblDataTable->SetRect(0, 48, 100, 23, false);
	this->cboDataTable = ui->NewComboBox(this->grpData, false);
	this->cboDataTable->SetRect(100, 48, 200, 23, false);
	NEW_CLASSNN(this->lblKeyCol, UI::GUILabel(ui, this->grpData, CSTR("Key Column")));
	this->lblKeyCol->SetRect(0, 72, 100, 23, false);
	this->cboKeyCol = ui->NewComboBox(this->grpData, false);
	this->cboKeyCol->SetRect(100, 72, 200, 23, false);
	NEW_CLASSNN(this->lblNullCol, UI::GUILabel(ui, this->grpData, CSTR("Null Column")));
	this->lblNullCol->SetRect(0, 96, 100, 23, false);
	this->cboNullCol = ui->NewComboBox(this->grpData, false);
	this->cboNullCol->SetRect(100, 96, 200, 23, false);
	this->cboNullCol->AddItem(CSTR("Empty"), 0);
	this->cboNullCol->AddItem(CSTR("\"NULL\""), 0);
	this->cboNullCol->AddItem(CSTR("\"null\""), 0);
	this->cboNullCol->SetSelectedIndex(0);
	NEW_CLASSNN(this->lblIgnoreCol, UI::GUILabel(ui, this->grpData, CSTR("Ignore Columns")));
	this->lblIgnoreCol->SetRect(0, 120, 100, 23, false);
	NEW_CLASSNN(this->txtIgnoreCol, UI::GUITextBox(ui, this->grpData, CSTR("")));
	this->txtIgnoreCol->SetRect(100, 120, 300, 23, false);
	NEW_CLASSNN(this->lblDataFilter, UI::GUILabel(ui, this->grpData, CSTR("Data Filter")));
	this->lblDataFilter->SetRect(0, 144, 100, 23, false);
	NEW_CLASSNN(this->txtDataFilter, UI::GUITextBox(ui, this->grpData, CSTR("")));
	this->txtDataFilter->SetRect(100, 144, 200, 23, false);
	this->btnDataCheck = ui->NewButton(this->grpData, CSTR("Check"));
	this->btnDataCheck->SetRect(100, 168, 75, 23, false);
	this->btnDataCheck->HandleButtonClick(OnDataCheckClk, this);
	NEW_CLASSNN(this->lblDataFileRow, UI::GUILabel(ui, *this, CSTR("Data Rows")));
	this->lblDataFileRow->SetRect(0, 288, 100, 23, false);
	NEW_CLASSNN(this->txtDataFileRow, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtDataFileRow->SetRect(100, 288, 200, 23, false);
	this->txtDataFileRow->SetReadOnly(true);
	NEW_CLASSNN(this->lblNoChg, UI::GUILabel(ui, *this, CSTR("No Changes")));
	this->lblNoChg->SetRect(0, 312, 100, 23, false);
	NEW_CLASSNN(this->txtNoChg, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtNoChg->SetRect(100, 312, 200, 23, false);
	this->txtNoChg->SetReadOnly(true);
	NEW_CLASSNN(this->lblUpdated, UI::GUILabel(ui, *this, CSTR("Updated rows")));
	this->lblUpdated->SetRect(0, 336, 100, 23, false);
	NEW_CLASSNN(this->txtUpdated, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtUpdated->SetRect(100, 336, 200, 23, false);
	this->txtUpdated->SetReadOnly(true);
	NEW_CLASSNN(this->lblNewRow, UI::GUILabel(ui, *this, CSTR("New rows")));
	this->lblNewRow->SetRect(0, 360, 100, 23, false);
	NEW_CLASSNN(this->txtNewRow, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtNewRow->SetRect(100, 360, 200, 23, false);
	this->txtNewRow->SetReadOnly(true);
	NEW_CLASSNN(this->lblDeletedRow, UI::GUILabel(ui, *this, CSTR("Deleted rows")));
	this->lblDeletedRow->SetRect(0, 384, 100, 23, false);
	NEW_CLASSNN(this->txtDeletedRow, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtDeletedRow->SetRect(100, 384, 200, 23, false);
	this->txtDeletedRow->SetReadOnly(true);

	NEW_CLASSNN(this->lblDBType, UI::GUILabel(ui, *this, CSTR("SQL Type")));
	this->lblDBType->SetRect(0, 432, 100, 23, false);
	this->cboDBType = ui->NewComboBox(*this, false);
	this->cboDBType->SetRect(100, 432, 200, 23, false);
	this->cboDBType->AddItem(CSTR("MySQL"), (void*)DB::SQLType::MySQL);
	this->cboDBType->AddItem(CSTR("SQL Server"), (void*)DB::SQLType::MSSQL);
	this->cboDBType->AddItem(CSTR("PostgreSQL"), (void*)DB::SQLType::PostgreSQL);
	DB::SQLType sqlType = this->GetDBSQLType();
	if (sqlType == DB::SQLType::MSSQL)
		this->cboDBType->SetSelectedIndex(1);
	else if (sqlType == DB::SQLType::PostgreSQL)
		this->cboDBType->SetSelectedIndex(2);
	else
		this->cboDBType->SetSelectedIndex(0);
	NEW_CLASSNN(this->chkAxisAware, UI::GUICheckBox(ui, *this, CSTR("Axis-Aware (MySQL >=8)"), false));
	this->chkAxisAware->SetRect(300, 432, 150, 23, false);
	NEW_CLASSNN(this->chkMultiRow, UI::GUICheckBox(ui, *this, CSTR("Multi-Row Insert"), true));
	this->chkMultiRow->SetRect(100, 456, 150, 23, false);
	this->btnSQL = ui->NewButton(*this, CSTR("Generate SQL"));
	this->btnSQL->SetRect(100, 480, 75, 23, false);
	this->btnSQL->HandleButtonClick(OnSQLClicked, this);
	this->btnExecute = ui->NewButton(*this, CSTR("Execute SQL"));
	this->btnExecute->SetRect(180, 480, 75, 23, false);
	this->btnExecute->HandleButtonClick(OnExecuteClicked, this);
	NEW_CLASSNN(this->lblStatTime, UI::GUILabel(ui, *this, CSTR("Time Used")));
	this->lblStatTime->SetRect(0, 504, 100, 23, false);
	NEW_CLASSNN(this->txtStatTime, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtStatTime->SetRect(100, 504, 150, 23 ,false);
	this->txtStatTime->SetReadOnly(true);
	NEW_CLASSNN(this->lblStatus, UI::GUILabel(ui, *this, CSTR("Status")));
	this->lblStatus->SetRect(0, 528, 100, 23, false);
	NEW_CLASSNN(this->txtStatus, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtStatus->SetRect(100, 528, 300, 23 ,false);
	this->txtStatus->SetReadOnly(true);

	this->HandleDropFiles(OnFiles, this);

	DB::TableDef *tableDef = this->db->GetTableDef(this->schema, this->table);
	if (tableDef)
	{
		Bool hasKey = false;
		Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = tableDef->ColIterator();
		NotNullPtr<DB::ColDef> col;
		UOSInt i = 0;
		while (it.HasNext())
		{
			col = it.Next();
			this->cboKeyCol->AddItem(col->GetColName(), (void*)i);
			if (col->IsPK())
			{
				hasKey = true;
				this->cboKeyCol->SetSelectedIndex(i);
			}
			i++;
		}
		this->cboKeyCol->AddItem(CSTR("No Key, All inserts"), (void*)INVALID_INDEX);
		if (!hasKey)
		{
			this->cboKeyCol->SetSelectedIndex(0);
		}
		DEL_CLASS(tableDef);
	}
}

SSWR::AVIRead::AVIRDBCheckChgForm::~AVIRDBCheckChgForm()
{
	SDEL_CLASS(this->dataFile);
}

void SSWR::AVIRead::AVIRDBCheckChgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
