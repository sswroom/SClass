#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/CSVFile.h"
#include "DB/DBConn.h"
#include "DB/TableDef.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/MSGeography.h"
#include "Math/WKTReader.h"
#include "SSWR/AVIRead/AVIRDBCheckChgForm.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

#define SRID 4326

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
		Bool succ;
		SQLSession sess;
		sess.mode = 0;
		sess.totalCnt = 0;
		sess.startTime = Data::Timestamp::UtcNow();
		sess.lastUpdateTime = sess.startTime;
		
		{
			IO::FileStream fs(dlg.GetFileName()->ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			sess.stm = &fs;
			succ = me->GenerateSQL(sb.ToCString(), sqlType, &sess);
		}
		Double t = Data::Timestamp::UtcNow().DiffSecDbl(sess.startTime);
		sptr = Text::StrDouble(sbuff, t);
		me->txtStatTime->SetText(CSTRP(sbuff, sptr));
		if (succ)
		{
			sb.ClearStr();
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
	Text::StringBuilderUTF8 sb;
	me->txtCSV->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please input CSV file first"), CSTR("Check Table Changes"), me);
		return;
	}
	DB::DBUtil::SQLType sqlType = (DB::DBUtil::SQLType)(OSInt)me->cboDBType->GetSelectedItem();
	Bool succ;
	SQLSession sess;
	sess.totalCnt = 0;
	sess.startTime = Data::Timestamp::UtcNow();
	sess.lastUpdateTime = sess.startTime;
	sess.stm = 0;
	if (me->chkMultiRow->IsChecked())
	{
		Text::StringBuilderUTF8 sbInsert;
		sess.mode = 2;
		sess.nInsert = 0;
		sess.sbInsert = &sbInsert;
		succ = me->GenerateSQL(sb.ToCString(), sqlType, &sess);
		if (succ && sess.nInsert > 0)
		{
			if (((DB::DBConn*)me->db)->ExecuteNonQuery(sess.sbInsert->ToCString()) >= 0)
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
		succ = me->GenerateSQL(sb.ToCString(), sqlType, &sess);
	}
	Double t = Data::Timestamp::UtcNow().DiffSecDbl(sess.startTime);
	sb.ClearStr();
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

Bool SSWR::AVIRead::AVIRDBCheckChgForm::LoadCSV(Text::CString fileName)
{
	Int8 csvTZ = 0;
	if (this->chkLocalTZ->IsChecked())
	{
		csvTZ = Data::DateTimeUtil::GetLocalTzQhr();
	}
	Text::CString nullStr = this->GetNullText();
	Bool noHeader = this->chkNoHeader->IsChecked();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	DB::TableDef *table = this->db->GetTableDef(this->schema, this->table);
	if (table == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	Data::ArrayList<UOSInt> colInd;
	UOSInt keyCol = (UOSInt)this->cboKeyCol->GetSelectedItem();
	UOSInt keyDCol;
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		colInd.Add(noHeader?i:INVALID_INDEX);
		i++;
	}
	UOSInt k;
	UOSInt l;
	Data::FastStringMap<Text::String**> csvData;
	Text::String** rowData;
	DB::CSVFile csv(fileName, 65001);
	if (noHeader) csv.SetNoHeader(true);
	DB::DBReader *r = csv.QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		DEL_CLASS(table);
		UI::MessageDialog::ShowDialog(CSTR("Error in reading CSV file"), CSTR("Check Table Changes"), this);
		return false;
	}
	l = r->ColCount();
	if (!noHeader)
	{
		k = 0;
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
	}
	keyDCol = colInd.GetItem(keyCol);

	if (noHeader)
	{
		if (j != l)
		{
			UI::MessageDialog::ShowDialog(CSTR("Column Count does not match"), CSTR("Check Table Changes"), this);
			csv.CloseReader(r);
			DEL_CLASS(table);
			return false;
		}
	}
	else
	{
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
	}
	UOSInt csvRowCnt = 0;
	UOSInt noChgCnt = 0;
	UOSInt updateCnt = 0;
	UOSInt newRowCnt = 0;
	UOSInt delRowCnt = 0;
	Text::String *s;
	Text::String *id;
	while (r->ReadNext())
	{
		csvRowCnt++;
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
								switch (table->GetCol(i)->GetColType())
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
		}
	}
	csv.CloseReader(r);

	if (keyCol != INVALID_INDEX)
	{
		r = this->db->QueryTableData(this->schema, this->table, 0, 0, 0, CSTR_NULL, 0);
		if (r == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
		}
		else
		{
			Data::ArrayList<Text::String*> idList;
			while (r->ReadNext())
			{
				id = r->GetNewStr(keyCol);
				rowData = csvData.Get(id);
				idList.Add(id);
				if (rowData)
				{
					Bool diff = false;
					i = 0;
					while (i < j)
					{
						if (r->IsNull(i))
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
									Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), csvTZ);
									if (ts1.DiffSec(ts2) != 0)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Decimal:
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
									Bool v2 = rowData[i]->v[0] == 't' || rowData[i]->v[0] == 'T' || rowData[i]->ToInt32() != 0;
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

			newRowCnt += csvData.GetCount() - idList.GetCount() + delRowCnt;
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
	this->txtCSV->SetText(fileName);
	sptr = Text::StrUOSInt(sbuff, csvRowCnt);
	this->txtCSVRow->SetText(CSTRP(sbuff, sptr));
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

Bool SSWR::AVIRead::AVIRDBCheckChgForm::GenerateSQL(Text::CString csvFileName, DB::DBUtil::SQLType sqlType, SQLSession *sess)
{
	Int8 csvTZ = 0;
	if (this->chkLocalTZ->IsChecked())
	{
		csvTZ = Data::DateTimeUtil::GetLocalTzQhr();
	}
	Text::CString nullStr = this->GetNullText();
	Bool noHeader = this->chkNoHeader->IsChecked();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	DB::TableDef *table = this->db->GetTableDef(this->schema, this->table);
	if (table == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table structure"), CSTR("Check Table Changes"), this);
		return false;
	}
	Bool succ = true;
	Data::ArrayList<UOSInt> colInd;
	Bool intKey = false;
	UOSInt keyCol = (UOSInt)this->cboKeyCol->GetSelectedItem();
	UOSInt keyDCol;
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		colInd.Add(noHeader?i:INVALID_INDEX);
		if (i == keyCol)
		{
			DB::DBUtil::ColType colType = table->GetCol(i)->GetColType();
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
	Data::FastStringMap<Text::String**> csvData;
	Text::String** rowData;
	DB::CSVFile csv(csvFileName, 65001);
	if (noHeader)
	{
		csv.SetNoHeader(true);
	}
	DB::DBReader *r = csv.QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		DEL_CLASS(table);
		UI::MessageDialog::ShowDialog(CSTR("Error in reading CSV file"), CSTR("Check Table Changes"), this);
		return false;
	}
	l = r->ColCount();
	if (!noHeader)
	{
		k = 0;
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
	}
	keyDCol = colInd.GetItem(keyCol);

	if (noHeader)
	{
		if (j != l)
		{
			UI::MessageDialog::ShowDialog(CSTR("Column Count does not match"), CSTR("Check Table Changes"), this);
			csv.CloseReader(r);
			DEL_CLASS(table);
			return false;
		}
	}
	else
	{
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
	}
	DB::SQLBuilder sql(sqlType, Data::DateTimeUtil::GetLocalTzQhr());
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
								switch (table->GetCol(i)->GetColType())
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
						if (s && s->Equals(nullStr.v, nullStr.leng))
						{
							sql.AppendStr(0);
						}
						else
						{
							AppendCol(&sql, table->GetCol(i)->GetColType(), s, csvTZ);
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
	csv.CloseReader(r);

	if (keyCol != INVALID_INDEX)
	{
		r = this->db->QueryTableData(this->schema, this->table, 0, 0, 0, CSTR_NULL, 0);
		if (r == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in getting table data"), CSTR("Check Table Changes"), this);
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
						if (r->IsNull(i))
						{
							if (rowData[i] == 0)
							{

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
									break;
								case DB::DBUtil::CT_Date:
								case DB::DBUtil::CT_DateTime:
								case DB::DBUtil::CT_DateTimeTZ:
									{
										Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), csvTZ);
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
										sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
										sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
										sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
										sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
											sql.AppendCol(table->GetCol(i)->GetColName()->v);
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
							sql.AppendCol(table->GetCol(i)->GetColName()->v);
							sql.AppendCmdC(CSTR(" = "));
							sql.AppendStr(0);
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
									Data::Timestamp ts2 = Data::Timestamp::FromStr(rowData[i]->ToCString(), csvTZ);
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
						}
						i++;
					}
					if (diff)
					{
						sql.AppendCmdC(CSTR(" where "));
						sql.AppendCol(table->GetCol(keyCol)->GetColName()->v);
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
					sql.AppendCol(table->GetCol(keyCol)->GetColName()->v);
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
							if (colFound) sql.AppendCmdC(CSTR(", "));
							colFound = true;
							sql.AppendCol(table->GetCol(i)->GetColName()->v);
							i++;
						}
						sql.AppendCmdC(CSTR(") values ("));
						colFound = false;
						i = 0;
						while (i < j)
						{
							if (colFound) sql.AppendCmdC(CSTR(", "));
							colFound = true;
							AppendCol(&sql, table->GetCol(i)->GetColType(), rowData[i], csvTZ);
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
	DEL_CLASS(table);
	return succ;
}

Bool SSWR::AVIRead::AVIRDBCheckChgForm::NextSQL(Text::CString sql, SQLSession *sess)
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
		DB::DBConn *db = (DB::DBConn*)this->db;
		if (db->ExecuteNonQuery(sql) >= 0)
		{
			sess->totalCnt++;
			this->UpdateStatus(sess);
			return true;
		}
		Text::StringBuilderUTF8 sb;
		db->GetErrorMsg(&sb);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Check Table Changes"), this);
		return false;
	}
	else if (sess->mode == 2)
	{
		DB::DBConn *db = (DB::DBConn*)this->db;
		if (sql.StartsWith(UTF8STRC("insert into ")))
		{
			UOSInt i = sql.IndexOf(UTF8STRC(") values ("));
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
					db->GetErrorMsg(&sb);
					UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Check Table Changes"), this);
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
					db->GetErrorMsg(&sb);
					UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Check Table Changes"), this);
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
		db->GetErrorMsg(&sb);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Check Table Changes"), this);
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
		sql->AppendBool(s->v[0] == 't' || s->v[0] == 'T');
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
	default:
		return CSTR("");
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
	NEW_CLASS(this->lblKeyCol, UI::GUILabel(ui, this, CSTR("Key Column")));
	this->lblKeyCol->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->cboKeyCol, UI::GUIComboBox(ui, this, false));
	this->cboKeyCol->SetRect(100, 48, 200, 23, false);
	NEW_CLASS(this->lblNullCol, UI::GUILabel(ui, this, CSTR("Null Column")));
	this->lblNullCol->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->cboNullCol, UI::GUIComboBox(ui, this, false));
	this->cboNullCol->SetRect(100, 72, 200, 23, false);
	this->cboNullCol->AddItem(CSTR("Empty"), 0);
	this->cboNullCol->AddItem(CSTR("\"NULL\""), 0);
	this->cboNullCol->SetSelectedIndex(0);
	NEW_CLASS(this->chkNoHeader, UI::GUICheckBox(ui, this, CSTR("No Header"), false));
	this->chkNoHeader->SetRect(100, 96, 200, 23, false);
	NEW_CLASS(this->chkLocalTZ, UI::GUICheckBox(ui, this, CSTR("Local Timezone"), false));
	this->chkLocalTZ->SetRect(300, 96, 200, 23, false);
	NEW_CLASS(this->lblCSV, UI::GUILabel(ui, this, CSTR("CSV")));
	this->lblCSV->SetRect(0, 120, 100, 23, false);
	NEW_CLASS(this->txtCSV, UI::GUITextBox(ui, this, CSTR("")));
	this->txtCSV->SetRect(100, 120, 600, 23, false);
	this->txtCSV->SetReadOnly(true);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, CSTR("Browse")));
	this->btnBrowse->SetRect(700, 120, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClk, this);
	NEW_CLASS(this->lblCSVRow, UI::GUILabel(ui, this, CSTR("CSV Rows")));
	this->lblCSVRow->SetRect(0, 144, 100, 23, false);
	NEW_CLASS(this->txtCSVRow, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtCSVRow->SetRect(100, 144, 200, 23, false);
	this->txtCSVRow->SetReadOnly(true);
	NEW_CLASS(this->lblNoChg, UI::GUILabel(ui, this, CSTR("No Changes")));
	this->lblNoChg->SetRect(0, 168, 100, 23, false);
	NEW_CLASS(this->txtNoChg, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtNoChg->SetRect(100, 168, 200, 23, false);
	this->txtNoChg->SetReadOnly(true);
	NEW_CLASS(this->lblUpdated, UI::GUILabel(ui, this, CSTR("Updated rows")));
	this->lblUpdated->SetRect(0, 192, 100, 23, false);
	NEW_CLASS(this->txtUpdated, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtUpdated->SetRect(100, 192, 200, 23, false);
	this->txtUpdated->SetReadOnly(true);
	NEW_CLASS(this->lblNewRow, UI::GUILabel(ui, this, CSTR("New rows")));
	this->lblNewRow->SetRect(0, 216, 100, 23, false);
	NEW_CLASS(this->txtNewRow, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtNewRow->SetRect(100, 216, 200, 23, false);
	this->txtNewRow->SetReadOnly(true);
	NEW_CLASS(this->lblDeletedRow, UI::GUILabel(ui, this, CSTR("Deleted rows")));
	this->lblDeletedRow->SetRect(0, 240, 100, 23, false);
	NEW_CLASS(this->txtDeletedRow, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtDeletedRow->SetRect(100, 240, 200, 23, false);
	this->txtDeletedRow->SetReadOnly(true);

	NEW_CLASS(this->lblDBType, UI::GUILabel(ui, this, CSTR("SQL Type")));
	this->lblDBType->SetRect(0, 288, 100, 23, false);
	NEW_CLASS(this->cboDBType, UI::GUIComboBox(ui, this, false));
	this->cboDBType->SetRect(100, 288, 200, 23, false);
	this->cboDBType->AddItem(CSTR("MySQL"), (void*)DB::DBUtil::SQLType::MySQL);
	this->cboDBType->AddItem(CSTR("SQL Server"), (void*)DB::DBUtil::SQLType::MSSQL);
	this->cboDBType->AddItem(CSTR("PostgreSQL"), (void*)DB::DBUtil::SQLType::PostgreSQL);
	this->cboDBType->SetSelectedIndex(0);
	NEW_CLASS(this->chkMultiRow, UI::GUICheckBox(ui, this, CSTR("Multi-Row Insert"), true));
	this->chkMultiRow->SetRect(100, 312, 150, 23, false);
	NEW_CLASS(this->btnSQL, UI::GUIButton(ui, this, CSTR("Generate SQL")));
	this->btnSQL->SetRect(100, 336, 75, 23, false);
	this->btnSQL->HandleButtonClick(OnSQLClicked, this);
	NEW_CLASS(this->btnExecute, UI::GUIButton(ui, this, CSTR("Execute SQL")));
	this->btnExecute->SetRect(180, 336, 75, 23, false);
	this->btnExecute->HandleButtonClick(OnExecuteClicked, this);
	NEW_CLASS(this->lblStatTime, UI::GUILabel(ui, this, CSTR("Time Used")));
	this->lblStatTime->SetRect(0, 360, 100, 23, false);
	NEW_CLASS(this->txtStatTime, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatTime->SetRect(100, 360, 150, 23 ,false);
	this->txtStatTime->SetReadOnly(true);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(0, 384, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(100, 384, 300, 23 ,false);
	this->txtStatus->SetReadOnly(true);

	this->HandleDropFiles(OnFiles, this);

	DB::TableDef *tableDef = this->db->GetTableDef(this->schema, this->table);
	if (tableDef)
	{
		Bool hasKey = false;
		UOSInt i = 0;
		UOSInt j = tableDef->GetColCnt();
		while (i < j)
		{
			this->cboKeyCol->AddItem(tableDef->GetCol(i)->GetColName(), (void*)i);
			if (tableDef->GetCol(i)->IsPK())
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
}

void SSWR::AVIRead::AVIRDBCheckChgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
