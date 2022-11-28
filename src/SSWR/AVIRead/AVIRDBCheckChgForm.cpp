#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "DB/TableDef.h"
#include "IO/FileStream.h"
#include "Math/WKTReader.h"
#include "SSWR/AVIRead/AVIRDBCheckChgForm.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnBrowseClk(void *userObj)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtCSV->GetText(&sb);
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBCheckChg", false);
	dlg.SetAllowMultiSel(false);
	if (sb.GetLength() > 0)
	{
		dlg.SetFileName(sb.ToCString());
	}
	dlg.AddFilter(CSTR("*.csv"), CSTR("Comma-Seperated-Value File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->LoadCSV(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnFiles(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->LoadCSV(files[i]->ToCString()))
		{
			return;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::OnSQLClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBCheckChgForm *me = (SSWR::AVIRead::AVIRDBCheckChgForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	me->txtCSV->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please input CSV file first"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::DBUtil::SQLType sqlType = (DB::DBUtil::SQLType)(OSInt)me->cboDBType->GetSelectedItem();
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
		me->GenerateSQL(sb.ToCString(), dlg.GetFileName()->ToCString(), sqlType);
	}
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::LoadCSV(Text::CString fileName)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	DB::TableDef *table = this->db->GetTableDef(this->schema, this->table);
	if (table == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	Data::ArrayList<UOSInt> colInd;
	UOSInt keyCol = INVALID_INDEX;
	UOSInt keyDCol;
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		colInd.Add(INVALID_INDEX);
		if (table->GetCol(i)->IsPK())
		{
			keyCol = i;
		}
		i++;
	}
	if (keyCol == INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog(CSTR("Key column not found"), CSTR("Check Table Changes"), this);
		DEL_CLASS(table);
		return false;
	}
	UOSInt k;
	UOSInt l;
	Data::Int64FastMap<Text::String**> csvData;
	Text::String** rowData;
	DB::CSVFile csv(fileName, 65001);
	DB::DBReader *r = csv.QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		DEL_CLASS(table);
		UI::MessageDialog::ShowDialog(CSTR("Error in reading CSV file"), CSTR("Check Table Changes"), this);
		return false;
	}
	k = 0;
	l = r->ColCount();
	while (k < l)
	{
		sptr = r->GetName(k, sbuff);
		if (sptr)
		{
			i = 0;
			while (i < j)
			{
				if (table->GetCol(i)->GetColName()->Equals(sbuff, (UOSInt)(sptr - sbuff)))
				{
					colInd.SetItem(i, k);
					break;
				}
				i++;
			}
		}
		k++;
	}
	keyDCol = colInd.GetItem(keyCol);

	i = 0;
	while (i < j)
	{
		if (colInd.GetItem(i) == INVALID_INDEX)
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Column "));
			sptr = table->GetCol(i)->GetColName()->ConcatTo(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" not found in CSV"));
			UI::MessageDialog::ShowDialog(CSTRP(sbuff, sptr), CSTR("Check Table Changes"), this);
			csv.CloseReader(r);
			DEL_CLASS(table);
			return false;
		}
		i++;
	}
	UOSInt noChgCnt = 0;
	UOSInt updateCnt = 0;
	UOSInt newRowCnt = 0;
	UOSInt delRowCnt = 0;
	Text::String *s;
	Int64 id;
	while (r->ReadNext())
	{
		if (r->ColCount() >= l)
		{
			s = r->GetNewStr(keyDCol);
			if (s->leng == 0 || s->Equals(UTF8STRC("0")))
			{
				s->Release();
				newRowCnt++;
			}
			else
			{
				id = s->ToInt64();
				s->Release();
				if (id == 0)
				{
					newRowCnt++;
				}
				else
				{
					rowData = MemAlloc(Text::String*, j);
					i = 0;
					while (i < j)
					{
						rowData[i] = r->GetNewStr(colInd.GetItem(i));
						i++;
					}
					rowData = csvData.Put(id, rowData);
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
		}
	}
	csv.CloseReader(r);

	r = this->db->QueryTableData(this->schema, this->table, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
	}
	else
	{
		while (r->ReadNext())
		{
			id = r->GetInt64(keyCol);
			rowData = csvData.Get(id);
			if (rowData)
			{
				Bool diff = false;
				i = 0;
				while (i < j)
				{
					if (r->IsNull(i))
					{
						if (rowData[i] == 0 || rowData[i]->leng == 0)
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
					else
					{
						switch (table->GetCol(i)->GetColType())
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
								Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
								if (ts1.DiffSec(ts2) != 0)
								{
									diff = true;
								}
							}
							break;
						case DB::DBUtil::CT_Double:
						case DB::DBUtil::CT_Float:
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
								Bool v2 = rowData[i]->v[0] == 't' || rowData[i]->v[0] == 'T';
								if (v1 != v2)
								{
									diff = true;
								}
							}
							break;
						case DB::DBUtil::CT_Vector:
							{
								Math::Geometry::Vector2D *vec1 = r->GetVector(i);
								Math::Geometry::Vector2D *vec2;;
								Math::WKTReader reader(vec1->GetSRID());
								vec2 = reader.ParseWKT(rowData[i]->v);
								if (vec2 == 0)
								{
									diff = true;
								}
								else if (!vec1->EqualsNearly(vec2))
								{
									diff = true;
								}
								SDEL_CLASS(vec1);
								SDEL_CLASS(vec2);
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
								r->GetUUID(i, &uuid1);
								uuid2.SetValue(rowData[i]->ToCString());
								if (!uuid1.Equals(&uuid2))
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
	this->txtCSV->SetText(fileName);
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

Bool SSWR::AVIRead::AVIRDBCheckChgForm::GenerateSQL(Text::CString csvFileName, Text::CString sqlFileName, DB::DBUtil::SQLType sqlType)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	DB::TableDef *table = this->db->GetTableDef(this->schema, this->table);
	if (table == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	Data::ArrayList<UOSInt> colInd;
	UOSInt keyCol = INVALID_INDEX;
	UOSInt keyDCol;
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		colInd.Add(INVALID_INDEX);
		if (table->GetCol(i)->IsPK())
		{
			keyCol = i;
		}
		i++;
	}
	if (keyCol == INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog(CSTR("Key column not found"), CSTR("Check Table Changes"), this);
		DEL_CLASS(table);
		return false;
	}
	UOSInt k;
	UOSInt l;
	Data::Int64FastMap<Text::String**> csvData;
	Text::String** rowData;
	DB::CSVFile csv(csvFileName, 65001);
	DB::DBReader *r = csv.QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		DEL_CLASS(table);
		UI::MessageDialog::ShowDialog(CSTR("Error in reading CSV file"), CSTR("Check Table Changes"), this);
		return false;
	}
	k = 0;
	l = r->ColCount();
	while (k < l)
	{
		sptr = r->GetName(k, sbuff);
		if (sptr)
		{
			i = 0;
			while (i < j)
			{
				if (table->GetCol(i)->GetColName()->Equals(sbuff, (UOSInt)(sptr - sbuff)))
				{
					colInd.SetItem(i, k);
					break;
				}
				i++;
			}
		}
		k++;
	}
	keyDCol = colInd.GetItem(keyCol);

	i = 0;
	while (i < j)
	{
		if (colInd.GetItem(i) == INVALID_INDEX)
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Column "));
			sptr = table->GetCol(i)->GetColName()->ConcatTo(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" not found in CSV"));
			UI::MessageDialog::ShowDialog(CSTRP(sbuff, sptr), CSTR("Check Table Changes"), this);
			csv.CloseReader(r);
			DEL_CLASS(table);
			return false;
		}
		i++;
	}
	IO::FileStream sqlFS(sqlFileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Writer writer(&sqlFS);
	DB::SQLBuilder sql(sqlType, Data::DateTimeUtil::GetLocalTzQhr());
	Bool genInsert;
	Bool colFound;
	Text::String *s;
	Int64 id;
	while (r->ReadNext())
	{
		if (r->ColCount() >= l)
		{
			genInsert = false;
			s = r->GetNewStr(keyDCol);
			if (s->leng == 0 || s->Equals(UTF8STRC("0")))
			{
				s->Release();
				genInsert = true;
			}
			else
			{
				id = s->ToInt64();
				s->Release();
				if (id == 0)
				{
					genInsert = true;
				}
				else
				{
					rowData = MemAlloc(Text::String*, j);
					i = 0;
					while (i < j)
					{
						rowData[i] = r->GetNewStr(colInd.GetItem(i));
						i++;
					}
					rowData = csvData.Put(id, rowData);
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
					if (i != keyCol)
					{
						if (colFound) sql.AppendCmdC(CSTR(", "));
						colFound = true;
						sql.AppendCol(table->GetCol(i)->GetColName()->v);
					}
					i++;
				}
				sql.AppendCmdC(CSTR(") values ("));
				colFound = false;
				i = 0;
				while (i < j)
				{
					if (i != keyCol)
					{
						if (colFound) sql.AppendCmdC(CSTR(", "));
						colFound = true;
						s = r->GetNewStr(colInd.GetItem(i));
						AppendCol(&sql, table->GetCol(i)->GetColType(), s);
						SDEL_STRING(s);
					}
					i++;
				}
				sql.AppendCmdC(CSTR(");"));
				writer.WriteLineCStr(sql.ToCString());
			}
		}
	}
	csv.CloseReader(r);

	r = this->db->QueryTableData(this->schema, this->table, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
	}
	else
	{
		while (r->ReadNext())
		{
			id = r->GetInt64(keyCol);
			rowData = csvData.Get(id);
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
					if (r->IsNull(i))
					{
						if (rowData[i] == 0 || rowData[i]->leng == 0)
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
					else
					{
						switch (table->GetCol(i)->GetColType())
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
								sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
								Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
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
									sql.AppendCol(table->GetCol(i)->GetColName()->v);
									sql.AppendCmdC(CSTR(" = "));
									sql.AppendTS(ts2);
								}
							}
							break;
						case DB::DBUtil::CT_Double:
						case DB::DBUtil::CT_Float:
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
									sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
									sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
									sql.AppendCol(table->GetCol(i)->GetColName()->v);
									sql.AppendCmdC(CSTR(" = "));
									sql.AppendInt64(v2);
								}
							}
							break;
						case DB::DBUtil::CT_Bool:
							{
								Bool v1 = r->GetBool(i);
								Bool v2 = rowData[i]->v[0] == 't' || rowData[i]->v[0] == 'T';
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
									sql.AppendCol(table->GetCol(i)->GetColName()->v);
									sql.AppendCmdC(CSTR(" = "));
									sql.AppendBool(v2);
								}
							}
							break;
						case DB::DBUtil::CT_Vector:
							{
								Math::Geometry::Vector2D *vec1 = r->GetVector(i);
								Math::Geometry::Vector2D *vec2;;
								Math::WKTReader reader(vec1->GetSRID());
								vec2 = reader.ParseWKT(rowData[i]->v);
								if (vec2 == 0)
								{
								}
								else if (!vec1->EqualsNearly(vec2))
								{
									if (diff)
									{
										sql.AppendCmdC(CSTR(", "));
									}
									else
									{
										diff = true;
									}
									sql.AppendCol(table->GetCol(i)->GetColName()->v);
									sql.AppendCmdC(CSTR(" = "));
									sql.AppendVector(vec2);
								}
								SDEL_CLASS(vec1);
								SDEL_CLASS(vec2);
							}
							break;
						case DB::DBUtil::CT_Binary:
							break;
						case DB::DBUtil::CT_UUID:
							{
								Data::UUID uuid1;
								Data::UUID uuid2;
								r->GetUUID(i, &uuid1);
								uuid2.SetValue(rowData[i]->ToCString());
								if (!uuid1.Equals(&uuid2))
								{
								}
							}
							break;
						case DB::DBUtil::CT_Unknown:
						default:
							break;
						}
						if (diff)
						{
							break;
						}
					}
					i++;
				}
				if (diff)
				{
					sql.AppendCmdC(CSTR(" where "));
					sql.AppendCol(table->GetCol(keyCol)->GetColName()->v);
					sql.AppendCmdC(CSTR(" = "));
					sql.AppendInt64(id);
					sql.AppendCmdC(CSTR(";"));
					writer.WriteLineCStr(sql.ToCString());
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
				sql.AppendCol(table->GetCol(keyCol)->GetColName()->v);
				sql.AppendCmdC(CSTR(" = "));
				sql.AppendInt64(id);
				sql.AppendCmdC(CSTR(";"));
				writer.WriteLineCStr(sql.ToCString());
			}
		}
		this->db->CloseReader(r);
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
	return true;
}

void __stdcall SSWR::AVIRead::AVIRDBCheckChgForm::AppendCol(DB::SQLBuilder *sql, DB::DBUtil::ColType colType, Text::String *s)
{
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
		sql->AppendTS(Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr()));
		break;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
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
		sql->AppendBool(s->v[0] == 't' || s->v[0] == 'T');
		break;
	case DB::DBUtil::CT_Vector:
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

SSWR::AVIRead::AVIRDBCheckChgForm::AVIRDBCheckChgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, DB::ReadingDB *db, Text::CString schema, Text::CString table) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Check Table Changes"));
	this->core = core;
	this->db = db;
	this->schema = schema;
	this->table = table;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	if (schema.v == 0)
	{
		schema = CSTR("");
	}

	NEW_CLASS(this->lblSchema, UI::GUILabel(ui, this, CSTR("Schema")));
	this->lblSchema->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtSchema, UI::GUITextBox(ui, this, schema));
	this->txtSchema->SetRect(100, 0, 200, 23, false);
	this->txtSchema->SetReadOnly(true);
	NEW_CLASS(this->lblTable, UI::GUILabel(ui, this, CSTR("Table")));
	this->lblTable->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtTable, UI::GUITextBox(ui, this, table));
	this->txtTable->SetRect(100, 24, 200, 23, false);
	this->txtTable->SetReadOnly(true);
	NEW_CLASS(this->lblCSV, UI::GUILabel(ui, this, CSTR("CSV")));
	this->lblCSV->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtCSV, UI::GUITextBox(ui, this, CSTR("")));
	this->txtCSV->SetRect(100, 48, 600, 23, false);
	this->txtCSV->SetReadOnly(true);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, CSTR("Browse")));
	this->btnBrowse->SetRect(700, 48, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClk, this);
	NEW_CLASS(this->lblNoChg, UI::GUILabel(ui, this, CSTR("No Changes")));
	this->lblNoChg->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtNoChg, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtNoChg->SetRect(100, 72, 200, 23, false);
	this->txtNoChg->SetReadOnly(true);
	NEW_CLASS(this->lblUpdated, UI::GUILabel(ui, this, CSTR("Updated rows")));
	this->lblUpdated->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtUpdated, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtUpdated->SetRect(100, 96, 200, 23, false);
	this->txtUpdated->SetReadOnly(true);
	NEW_CLASS(this->lblNewRow, UI::GUILabel(ui, this, CSTR("New rows")));
	this->lblNewRow->SetRect(0, 120, 100, 23, false);
	NEW_CLASS(this->txtNewRow, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtNewRow->SetRect(100, 120, 200, 23, false);
	this->txtNewRow->SetReadOnly(true);
	NEW_CLASS(this->lblDeletedRow, UI::GUILabel(ui, this, CSTR("Deleted rows")));
	this->lblDeletedRow->SetRect(0, 144, 100, 23, false);
	NEW_CLASS(this->txtDeletedRow, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtDeletedRow->SetRect(100, 144, 200, 23, false);
	this->txtDeletedRow->SetReadOnly(true);

	NEW_CLASS(this->lblDBType, UI::GUILabel(ui, this, CSTR("SQL Type")));
	this->lblDBType->SetRect(0, 192, 100, 23, false);
	NEW_CLASS(this->cboDBType, UI::GUIComboBox(ui, this, false));
	this->cboDBType->SetRect(100, 192, 200, 23, false);
	this->cboDBType->AddItem(CSTR("MySQL"), (void*)DB::DBUtil::SQLType::MySQL);
	this->cboDBType->AddItem(CSTR("SQL Server"), (void*)DB::DBUtil::SQLType::MSSQL);
	this->cboDBType->AddItem(CSTR("PostgreSQL"), (void*)DB::DBUtil::SQLType::PostgreSQL);
	this->cboDBType->SetSelectedIndex(0);
	NEW_CLASS(this->btnSQL, UI::GUIButton(ui, this, CSTR("Generate SQL")));
	this->btnSQL->SetRect(100, 216, 75, 23, false);
	this->btnSQL->HandleButtonClick(OnSQLClicked, this);

	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRDBCheckChgForm::~AVIRDBCheckChgForm()
{
}

void SSWR::AVIRead::AVIRDBCheckChgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
