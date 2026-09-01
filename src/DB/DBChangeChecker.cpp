#include "Stdafx.h"
#include "Data/FastStringMapObj.hpp"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/DBChangeChecker.h"
#include "DB/DBReader.h"
#include "DB/ReadingDBTool.h"
#include "DB/TableDef.h"
#include "Map/ESRI/FileGDBUtil.h"
#include "Math/CoordinateSystemConverter.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/MSGeography.h"
#include "Math/WKBReader.h"
#include "Math/WKTReader.h"
#include "Math/WKTWriter.h"

Bool DB::DBChangeChecker::IsColIndexValid(NN<Data::ArrayListNative<UIntOS>> colInd, NN<DB::TableDef> destTable)
{
	UIntOS j = colInd->GetCount();
	if (j == 0)
	{
		this->SetLastError(CSTR("Error in reading database structure"));
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
			this->SetLastError(sb.ToCString());
			return false;
		}
		i++;
	}
	return true;
}

void DB::DBChangeChecker::SetLastError(Text::CStringNN msg)
{
	OPTSTR_DEL(this->lastError);
	this->lastError = Text::String::New(msg);
}

DB::SQLType DB::DBChangeChecker::GetDBSQLType() const
{
	if (this->srcConn->IsDBTool())
	{
		return NN<DB::ReadingDBTool>::ConvertFrom(this->srcConn)->GetSQLType();
	}
	else if (this->srcConn->IsFullConn())
	{
		return NN<DB::DBConn>::ConvertFrom(this->srcConn)->GetSQLType();
	}
	else
	{
		return DB::SQLType::Unknown;
	}
}

Optional<Text::String> DB::DBChangeChecker::GetNewText(UIntOS colIndex) const
{
	NN<Text::String> s;
	if (this->colStr.GetItem(colIndex).SetTo(s))
		return s->Clone();
	else
		return nullptr;
}

NN<Text::String> DB::DBChangeChecker::GetNewTextNN(UIntOS colIndex) const
{
	NN<Text::String> s;
	if (this->colStr.GetItem(colIndex).SetTo(s))
		return s;
	else
		return Text::String::NewEmpty();
}

void __stdcall DB::DBChangeChecker::AppendCol(NN<DB::SQLBuilder> sql, NN<DB::ColDef> col, Optional<Text::String> s, Int8 tzQhr, UInt32 srid)
{
	NN<Text::String> nns;
	if (!s.SetTo(nns))
	{
		sql->AppendNull();
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
			vec2 = String2Vector(nns, (srid == 0)?col->GetGeometrySRID():srid);
			sql->AppendVector(vec2);
			vec2.Delete();
		}
		break;
	case DB::DBUtil::CT_Binary:
		{
			UnsafeArray<UInt8> buff = MemAllocArr(UInt8, nns->leng >> 1);
			UIntOS buffSize = Text::StrHex2Bytes(nns->v, buff);
			sql->AppendBinary(UnsafeArray<const UInt8>(buff), buffSize);
			MemFreeArr(buff);
		}
		break;
	case DB::DBUtil::CT_UUID:
		break;
	case DB::DBUtil::CT_Unknown:
	default:
		break;
	}
}

Optional<Math::Geometry::Vector2D> DB::DBChangeChecker::String2Vector(NN<Text::String> str, UInt32 srid)
{
	Math::WKTReader reader(srid);
	NN<Math::Geometry::Vector2D> vec;
	if (reader.ParseWKT(str->v).SetTo(vec))
	{
		return vec;
	}
	UnsafeArray<UInt8> buff = MemAllocArr(UInt8, str->leng >> 1);
	UIntOS buffLen;
	if (str->StartsWith(CSTR("0x")))
	{
		buffLen = Text::StrHex2Bytes(&str->v[2], buff);
	}
	else
	{
		buffLen = str->Hex2Bytes(buff);
	}
	if (Map::ESRI::FileGDBUtil::ParseSDERecord(Data::ByteArrayR(buff, buffLen)).SetTo(vec))
	{
		MemFreeArr(buff);
		return vec;
	}
	Math::WKBReader wkb(srid);
	if (wkb.ParseWKB(buff, buffLen, 0).SetTo(vec))
	{
		MemFreeArr(buff);
		return vec;
	}
	MemFreeArr(buff);
	return nullptr;
}

DB::DBChangeChecker::DBChangeChecker(NN<DB::ReadingDB> srcDB, Text::CString srcSchema, Text::CStringNN srcTable)
{
	this->lastError = nullptr;
	this->srcConn = srcDB;
	this->srcSchema = srcSchema;
	this->srcTable = srcTable;
	this->srcCond = nullptr;
	this->dataConn = nullptr;
	this->dataSchema = nullptr;
	this->dataTable = nullptr;
	this->dataTz = 0;
	this->dataFileNoHeader = false;
	this->nullStr = nullptr;
	this->keyCol1 = INVALID_INDEX;
	this->keyCol2 = INVALID_INDEX;
	this->srConv = false;
	this->simpleShape = false;
	this->fixError = false;
	this->dataFileRowCnt = 0;
	this->noChgCnt = 0;
	this->newRowCnt = 0;
	this->delRowCnt = 0;
	this->updateCnt = 0;
}

DB::DBChangeChecker::~DBChangeChecker()
{
	OPTSTR_DEL(this->lastError);
}

void DB::DBChangeChecker::SetDataTable(NN<DB::ReadingDB> dataDB, Text::CString dataSchema, Text::CStringNN dataTable, Int8 dataTz, Bool dataFileNoHeader)
{
	this->dataConn = dataDB;
	this->dataSchema = dataSchema;
	this->dataTable = dataTable;
	this->dataTz = dataTz;
	this->dataFileNoHeader = dataFileNoHeader;
}

void DB::DBChangeChecker::SetNullStr(Text::CStringNN nullStr)
{
	this->nullStr = nullStr;
}

void DB::DBChangeChecker::SetKeyCols(UIntOS keyCol1, UIntOS keyCol2)
{
	this->keyCol1 = keyCol1;
	this->keyCol2 = keyCol2;
}

void DB::DBChangeChecker::SetSrcCond(Optional<Data::QueryConditions> srcCond)
{
	this->srcCond = srcCond;
}

void DB::DBChangeChecker::SetDataCond(Optional<Data::QueryConditions> dataCond)
{
	this->dataCond = dataCond;
}

void DB::DBChangeChecker::SetColMapping(NN<Data::ArrayListNative<UIntOS>> colInd, NN<Data::ArrayListStringNN> colStr)
{
	this->colInd.Clear();
	this->colInd.AddAll(colInd);
	this->colStr.Clear();
	this->colStr.AddAll(colStr);
}

void DB::DBChangeChecker::SetSRConv(Bool srConv)
{
	this->srConv = srConv;
}

void DB::DBChangeChecker::SetSimpleShape(Bool simpleShape)
{
	this->simpleShape = simpleShape;
}

void DB::DBChangeChecker::SetFixError(Bool fixError)
{
	this->fixError = fixError;
}

Bool DB::DBChangeChecker::InitColMapping(NN<Data::ArrayListNative<UIntOS>> colInd)
{
	return this->InitColMapping(colInd, DB::DBChangeChecker::UNKNOWN_COL, nullptr);
}

Bool DB::DBChangeChecker::InitColMapping(NN<Data::ArrayListNative<UIntOS>> colInd, UIntOS defColInd, Optional<IO::LogTool> log)
{
	NN<IO::LogTool> nnlog;
	NN<DB::ReadingDB> dataConn;
	Text::CString dataSchema;
	Text::CStringNN dataTable;
	if (!this->dataConn.SetTo(dataConn))
	{
		return false;
	}
	dataSchema = this->dataSchema;
	if (!this->dataTable.SetTo(dataTable))
	{
		return false;
	}
	NN<DB::TableDef> srcTableDef;
	if (!this->srcConn->GetTableDef(this->srcSchema, this->srcTable).SetTo(srcTableDef))
	{
		return false;
	}
	colInd->Clear();
	UIntOS i = 0;
	UIntOS destCnt = srcTableDef->GetColCnt();
	UIntOS k;
	UIntOS srcCnt;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ColDef> col;
	NN<DB::DBReader> r;
	while (i < destCnt)
	{
		colInd->Add(this->dataFileNoHeader?i:defColInd);
		i++;
	}

	if (!dataConn->QueryTableData(dataSchema, dataTable, nullptr, 0, 1, nullptr, nullptr).SetTo(r))
	{
		srcTableDef.Delete();
		return false;
	}
	srcCnt = r->ColCount();
	if (!this->dataFileNoHeader)
	{
		k = 0;
		while (k < srcCnt)
		{
			Bool found = false;
			if (r->GetName(k, sbuff).SetTo(sptr))
			{
				i = 0;
				while (i < destCnt)
				{
					if (srcTableDef->GetCol(i).SetTo(col) && col->GetColName()->EqualsICase(sbuff, (UIntOS)(sptr - sbuff)))
					{
						colInd->SetItem(i, k);
						found = true;
						break;
					}
					i++;
				}
				if (log.SetTo(nnlog) && !found)
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Data File Column ["));
					sb.AppendP(sbuff, sptr);
					sb.AppendC(UTF8STRC("] not found"));
					nnlog->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
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
	srcTableDef.Delete();
	return true;
}

Bool DB::DBChangeChecker::CheckChange()
{
	NN<DB::ReadingDB> dataConn;
	Text::CString dataSchema;
	Text::CStringNN dataTable;
	Text::CStringNN nullStr;
	Bool srConv = this->srConv;
	Bool simpleShape = this->simpleShape;
	Bool fixError = this->fixError;
	if (!this->dataConn.SetTo(dataConn))
	{
		this->SetLastError(CSTR("Data connection not set"));
		return false;
	}
	dataSchema = this->dataSchema;
	if (!this->dataTable.SetTo(dataTable))
	{
		this->SetLastError(CSTR("Data table not set"));
		return false;
	}
	if (!this->nullStr.SetTo(nullStr))
	{
		this->SetLastError(CSTR("Null string not set"));
		return false;
	}

	DB::SQLType sqlType = this->GetDBSQLType();
	Bool no3DGeometry = DB::DBUtil::IsNo3DGeometry(sqlType);
	NN<DB::TableDef> table;
	if (!this->srcConn->GetTableDef(this->srcSchema, this->srcTable).SetTo(table))
	{
		this->SetLastError(CSTR("Error in getting table structure"));
		return false;
	}
	Optional<Math::CoordinateSystem> csysDB = nullptr;
	Optional<Math::CoordinateSystem> csysSrc = nullptr;
	NN<Math::CoordinateSystem> nncsysDB;
	NN<Math::CoordinateSystem> nncsysSrc;
	Text::StringBuilderUTF8 sb;
	UIntOS keyCol1 = this->keyCol1;
	UIntOS keyDCol1;
	UIntOS keyCol2 = this->keyCol2;
	UIntOS keyDCol2;
	UIntOS i;
	UIntOS k;
	UIntOS dbCnt;
	UIntOS srcCnt;
	Data::FastStringMapObj<Optional<Text::String>*> csvData;
	Optional<Text::String>* rowData;
	NN<DB::ColDef> col;
	NN<DB::DBReader> r;
	if (!this->IsColIndexValid(this->colInd, table))
	{
		table.Delete();
		return false;
	}
	if (!dataConn->QueryTableData(dataSchema, dataTable, nullptr, 0, 0, nullptr, this->dataCond).SetTo(r))
	{
		table.Delete();
		this->SetLastError(CSTR("Error in reading data file"));
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
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Column Count does not match, dbCnt = "));
			sb.AppendUIntOS(dbCnt);
			sb.AppendC(UTF8STRC(", srcCnt = "));
			sb.AppendUIntOS(srcCnt);
			this->SetLastError(sb.ToCString());
			dataConn->CloseReader(r);
			table.Delete();
			return false;
		}
	}
	else
	{
		if (keyCol1 != INVALID_INDEX && keyDCol1 == INVALID_INDEX)
		{
			this->SetLastError(CSTR("Key Column not found in data file"));
			dataConn->CloseReader(r);
			table.Delete();
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
					rowData = MemAlloc(Optional<Text::String>, dbCnt);
					i = 0;
					while (i < dbCnt)
					{
						k = this->colInd.GetItem(i);
						rowData[i] = nullptr;
						if (k != INVALID_INDEX)
						{
							if (k == TEXT_COL)
							{
								rowData[i] = this->GetNewText(i);
							}
							else
							{
								rowData[i] = r->GetNewStr(k);
							}
							if (rowData[i].SetTo(s))
							{
								if (s->Equals(nullStr.v, nullStr.leng))
								{
									s->Release();
									rowData[i] = nullptr;
								}
								else if (table->GetCol(i).SetTo(col))
								{
									if (s->leng == 0)
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
										case DB::DBUtil::CT_Vector:
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
										case DB::DBUtil::CT_UUID:
										case DB::DBUtil::CT_Unknown:
										default:
											s->Release();
											rowData[i] = nullptr;
											break;
										}
									}
									else if (col->GetColType() == DB::DBUtil::CT_Vector && (srConv || simpleShape || fixError))
									{
										NN<Math::Geometry::Vector2D> vec;
										if (r->GetVector(k).SetTo(vec))
										{
											Bool updated = false;
											if (vec->HasCurve() && simpleShape)
											{
												NN<Math::Geometry::Vector2D> vec2;
//												printf("Found curve\n");
												if (vec->ToSimpleShape().SetTo(vec2))
												{
//													printf("Converted to simple shape: %s\n", Math::Geometry::Vector2D::VectorTypeGetName(vec2->GetVectorType()).v.Ptr());
													vec.Delete();
													vec = vec2;
													updated = true;
												}
											}
											if (fixError)
											{
												if (vec->FixError())
												{
													updated = true;
												}
											}
											if (srConv)
											{
												UInt32 srcSRID = vec->GetSRID();
												UInt32 destSRID = col->GetGeometrySRID();
												if (srcSRID != 0 && srcSRID != destSRID)
												{
													if (!csysDB.SetTo(nncsysDB))
													{
														nncsysDB = Math::CoordinateSystemManager::SRCreateCSysOrDef(destSRID);
														csysDB = nncsysDB;
													}
													if (csysSrc.SetTo(nncsysSrc))
													{
														if (nncsysSrc->GetSRID() != srcSRID)
														{
															nncsysSrc.Delete();
															nncsysSrc = Math::CoordinateSystemManager::SRCreateCSysOrDef(srcSRID);
															csysSrc = nncsysSrc;
														}
													}
													else
													{
														nncsysSrc = Math::CoordinateSystemManager::SRCreateCSysOrDef(srcSRID);
														csysSrc = nncsysSrc;
													}
													Math::CoordinateSystemConverter csysConv(nncsysSrc, nncsysDB);
													vec->Convert(csysConv);
													updated = true;
												}
											}
											if (updated)
											{
												sb.ClearStr();
												Math::WKTWriter writer;
												writer.ToText(sb, vec);
												OPTSTR_DEL(rowData[i]);
												rowData[i] = Text::String::New(sb.ToCString());
											}
											vec.Delete();
										}
									}
									else if (col->GetColType() == DB::DBUtil::CT_Binary)
									{
										UIntOS binSize = r->GetBinarySize(k);
										UnsafeArray<UInt8> binBuff = MemAllocArr(UInt8, binSize);
										binSize = r->GetBinary(k, binBuff);
										sb.ClearStr();
										sb.AppendHexBuff(binBuff, binSize, 0, Text::LineBreakType::None);
										OPTSTR_DEL(rowData[i]);
										rowData[i] = Text::String::New(sb.ToCString());
										MemFreeArr(binBuff);
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
							OPTSTR_DEL(rowData[i]);
						}
						MemFree(rowData);

						Text::StringBuilderUTF8 sb;
						sb.Append(CSTR("Data File Key duplicate ("));
						sb.Append(sbId.ToCString());
						sb.AppendUTF8Char(')');
						this->SetLastError(sb.ToCString());
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
		if (!srcConn->QueryTableData(this->srcSchema, this->srcTable, nullptr, 0, 0, nullptr, this->srcCond).SetTo(r))
		{
			this->SetLastError(CSTR("Error in getting table data"));
			succ = false;
		}
		else
		{
			Data::ArrayListStringNN idList;
			while (r->ReadNext())
			{
				if (!r->GetNewStr(keyCol1).SetTo(id))
				{
					this->SetLastError(CSTR("Source Key is null"));
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
					Text::StringBuilderUTF8 sbFilter;
					sbFilter.AppendC(UTF8STRC("Source Key duplicate ("));
					sbFilter.Append(sbId);
					sbFilter.AppendUTF8Char(')');
					this->SetLastError(sbFilter.ToCString());
					succ = false;
					break;
				}
				rowData = csvData.GetC(sbId.ToCString());
				idList.SortedInsert(Text::String::New(sbId.ToCString()));
				id->Release();
				if (rowData)
				{
					NN<Text::String> rowDataStr;
					Bool diff = false;
					i = 0;
					while (i < dbCnt)
					{
						if (this->colInd.GetItem(i) == INVALID_INDEX)
						{

						}
						else if (r->IsNull(i))
						{
							if (rowData[i].IsNull())
							{

							}
							else
							{
								diff = true;
								break;
							}
						}
						else if (!rowData[i].SetTo(rowDataStr))
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
								if (!id->Equals(rowDataStr->v, rowDataStr->leng))
								{
									diff = true;
								}
								id->Release();
								break;
							case DB::DBUtil::CT_Date:
								{
									Data::Date ts1 = r->GetTimestamp(i).ToDate();
									Data::Date ts2 = Data::Timestamp::FromStr(rowDataStr->ToCString(), this->dataTz).ToDate();
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
									Data::Timestamp ts2 = Data::Timestamp::FromStr(rowDataStr->ToCString(), this->dataTz);
									if (ts1.DiffSec(ts2) != 0)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Float:
								{
									Double v1 = r->GetDblOrNAN(i);
									Double v2 = rowDataStr->ToDoubleOrNAN();
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
									Double v2 = rowDataStr->ToDoubleOrNAN();
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
									Int32 v2 = rowDataStr->ToInt32();
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
									Int64 v2 = rowDataStr->ToInt64();
									if (v1 != v2)
									{
										diff = true;
									}
								}
								break;
							case DB::DBUtil::CT_Bool:
								{
									Bool v1 = r->GetBool(i);
									Bool v2 = rowDataStr->v[0] == 't' || rowDataStr->v[0] == 'T' || rowDataStr->ToInt32() != 0;
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
										if (!String2Vector(rowDataStr, vec1->GetSRID()).SetTo(vec2))
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
									UIntOS binSize = r->GetBinarySize(i);
									UnsafeArray<UInt8> binBuff = MemAllocArr(UInt8, binSize);
									binSize = r->GetBinary(i, binBuff);
									UIntOS bin2Size = rowDataStr->leng >> 1;
									UnsafeArray<UInt8> bin2Buff = MemAllocArr(UInt8, bin2Size);
									bin2Size = Text::StrHex2Bytes(rowDataStr->v, bin2Buff);
									if (!Text::StrEqualsC(binBuff, binSize, bin2Buff, bin2Size))
									{
										diff = true;
									}
									MemFreeArr(binBuff);
									MemFreeArr(bin2Buff);
								}
								break;
							case DB::DBUtil::CT_UUID:
								{
									Data::UUID uuid1;
									Data::UUID uuid2;
									r->GetUUID(i, uuid1);
									uuid2.SetValue(rowDataStr->ToCString());
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
			this->srcConn->CloseReader(r);

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
			OPTSTR_DEL(rowData[i]);
		}
		MemFree(rowData);
	}
	table.Delete();
	csysSrc.Delete();
	csysDB.Delete();
	if (succ)
	{
		this->dataFileRowCnt = dataFileRowCnt;
		this->noChgCnt = noChgCnt;
		this->updateCnt = updateCnt;
		this->newRowCnt = newRowCnt;
		this->delRowCnt = delRowCnt;
		return true;
	}
	else
	{
		return false;
	}
}

Bool DB::DBChangeChecker::GenerateSQL(DB::SQLType sqlType, Bool axisAware, SQLHandler sqlHdlr, AnyType userObj)
{
	OPTSTR_DEL(this->lastError);
	NN<DB::ReadingDB> dataConn;
	Text::CString dataSchema;
	Text::CStringNN dataTable;
	Text::CStringNN nullStr;
	Bool srConv = this->srConv;
	Bool simpleShape = this->simpleShape;
	Bool fixError = this->fixError;
	if (sqlHdlr == nullptr)
	{
		this->SetLastError(CSTR("SQL handler not set"));
		return false;
	}
	if (!this->dataConn.SetTo(dataConn))
	{
		this->SetLastError(CSTR("Data connection not set"));
		return false;
	}
	dataSchema = this->dataSchema;
	if (!this->dataTable.SetTo(dataTable))
	{
		this->SetLastError(CSTR("Data table not set"));
		return false;
	}
	if (!this->nullStr.SetTo(nullStr))
	{
		this->SetLastError(CSTR("Null string not set"));
		return false;
	}

	NN<DB::TableDef> table;
	NN<DB::ColDef> col;
	UInt32 dbSrid = 0;
	Optional<Math::CoordinateSystem> csysDB = nullptr;
	Optional<Math::CoordinateSystem> csysSrc = nullptr;
	NN<Math::CoordinateSystem> nncsysDB;
	NN<Math::CoordinateSystem> nncsysSrc;
	Text::StringBuilderUTF8 sb;
	if (dataConn->GetTableDef(dataSchema, dataTable).SetTo(table))
	{
		UIntOS i = table->GetColCnt();
		while (i-- > 0)
		{
			if (table->GetCol(i).SetTo(col) && col->GetColType() == DB::DBUtil::CT_Vector)
			{
				dbSrid = col->GetGeometrySRID();
				break;
			}
		}
		table.Delete();
	}
	if (!this->srcConn->GetTableDef(this->srcSchema, this->srcTable).SetTo(table))
	{
		this->SetLastError(CSTR("Error in getting table structure"));
		return false;
	}
	Bool succ = true;
	Bool intKey = false;
	Bool no3DGeometry = DB::DBUtil::IsNo3DGeometry(this->GetDBSQLType());
	Text::StringBuilderUTF8 sbId;
	UIntOS keyCol1 = this->keyCol1;
	UIntOS keyDCol1;
	UIntOS keyCol2 = this->keyCol2;
	UIntOS keyDCol2;
	UIntOS i = 0;
	UIntOS dbCnt = table->GetColCnt();
	while (i < dbCnt)
	{
		if (table->GetCol(i).SetTo(col))
		{
			DB::DBUtil::ColType colType = col->GetColType();
			if (i == keyCol1 && keyCol2 == INVALID_INDEX)
			{
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
				case DB::DBUtil::CT_Vector:
					break;
				}
			}
			if (colType == DB::DBUtil::CT_Vector)
			{
				UInt32 srid = col->GetGeometrySRID();
				if (srid != 0)
				{
					dbSrid = srid;
				}
			}
		}
		i++;
	}
	if (dbSrid == 0)
	{
		dbSrid = 4326;
	}
	UIntOS k;
	UIntOS srcCnt;
	Data::FastStringMapObj<Optional<Text::String>*> csvData;
	Optional<Text::String>* rowData;
	NN<DB::DBReader> r;
	if (!this->IsColIndexValid(this->colInd, table))
	{
		table.Delete();
		return false;
	}
	if (!dataConn->QueryTableData(dataSchema, dataTable, nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		table.Delete();
		this->SetLastError(CSTR("Error in reading data file"));
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
			this->SetLastError(CSTR("Column Count does not match"));
			dataConn->CloseReader(r);
			table.Delete();
			return false;
		}
	}
	else
	{
		if (keyCol1 != INVALID_INDEX && keyDCol1 == INVALID_INDEX)
		{
			this->SetLastError(CSTR("Key Column not found in data file"));
			dataConn->CloseReader(r);
			table.Delete();
			return false;
		}
	}
	DB::SQLBuilder sql(sqlType, axisAware, Data::DateTimeUtil::GetLocalTzQhr());
	Bool genInsert;
	Bool colFound;
	NN<Text::String> s;
	NN<Text::String> s2;
	Optional<Text::String> ops;
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
				if (keyDCol1 == DB::DBChangeChecker::TEXT_COL)
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
						if (keyDCol2 == DB::DBChangeChecker::TEXT_COL)
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
					rowData = MemAlloc(Optional<Text::String>, dbCnt);
					i = 0;
					while (i < dbCnt)
					{
						k = this->colInd.GetItem(i);
						if (k == INVALID_INDEX)
						{
							rowData[i] = nullptr;
						}
						else
						{
							if (k == DB::DBChangeChecker::TEXT_COL)
							{
								rowData[i] = this->GetNewText(i).OrNull();
							}
							else
							{
								rowData[i] = r->GetNewStr(this->colInd.GetItem(i)).OrNull();
							}
							if (rowData[i].SetTo(s2))
							{
								if (s2->Equals(nullStr.v, nullStr.leng))
								{
									s2->Release();
									rowData[i] = nullptr;
								}
								else if (s2->leng == 0)
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
										s2->Release();
										rowData[i] = nullptr;
										break;
									}
								}
								else if (table->GetCol(i).SetTo(col))
								{
									if ((srConv || simpleShape || fixError) && col->GetColType() == DB::DBUtil::CT_Vector)
									{
										NN<Math::Geometry::Vector2D> vec;
										if (r->GetVector(this->colInd.GetItem(i)).SetTo(vec))
										{
											Bool updated = false;
											if (vec->HasCurve() && simpleShape)
											{
												NN<Math::Geometry::Vector2D> vec2;
												if (vec->ToSimpleShape().SetTo(vec2))
												{
													vec.Delete();
													vec = vec2;
													updated = true;
												}
											}
											if (fixError)
											{
												if (vec->FixError())
												{
													updated = true;
												}
											}
											if (srConv)
											{
												UInt32 srcSRID = vec->GetSRID();
												UInt32 destSRID = col->GetGeometrySRID();
												if (srcSRID != 0 && srcSRID != destSRID)
												{
													if (!csysDB.SetTo(nncsysDB))
													{
														nncsysDB = Math::CoordinateSystemManager::SRCreateCSysOrDef(destSRID);
														csysDB = nncsysDB;
													}
													if (csysSrc.SetTo(nncsysSrc))
													{
														if (nncsysSrc->GetSRID() != srcSRID)
														{
															nncsysSrc.Delete();
															nncsysSrc = Math::CoordinateSystemManager::SRCreateCSysOrDef(srcSRID);
															csysSrc = nncsysSrc;
														}
													}
													else
													{
														nncsysSrc = Math::CoordinateSystemManager::SRCreateCSysOrDef(srcSRID);
														csysSrc = nncsysSrc;
													}
													Math::CoordinateSystemConverter csysConv(nncsysSrc, nncsysDB);
													vec->Convert(csysConv);
													updated = true;
												}
											}
											if (updated)
											{
												sb.ClearStr();
												Math::WKTWriter writer;
												writer.ToText(sb, vec);
												s2->Release();
												rowData[i] = Text::String::New(sb.ToCString()).Ptr();
											}
											vec.Delete();
										}
									}
									else if (col->GetColType() == DB::DBUtil::CT_Binary)
									{
										UIntOS binSize = r->GetBinarySize(this->colInd.GetItem(i));
										if (binSize > 0)
										{
											UnsafeArray<UInt8> binBuff = MemAlloc(UInt8, binSize);
											binSize = r->GetBinary(this->colInd.GetItem(i), binBuff);
											sb.ClearStr();
											sb.AppendHexBuff(binBuff, binSize, 0, Text::LineBreakType::None);
											s2->Release();
											rowData[i] = Text::String::New(sb.ToCString());
											MemFreeArr(binBuff);
										}
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
							OPTSTR_DEL(rowData[i]);
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
				if (this->srcSchema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
				{
					sql.AppendCol(schema.v);
					sql.AppendCmdC(CSTR("."));
				}
				sql.AppendCol(this->srcTable.v);
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
							sql.AppendNull();
						}
						else if (col->GetColType() == DB::DBUtil::CT_Vector)
						{
							NN<Math::Geometry::Vector2D> vec;
							if (r->GetVector(this->colInd.GetItem(i)).SetTo(vec))
							{
								if (vec->HasCurve() && simpleShape)
								{
									NN<Math::Geometry::Vector2D> vec2;
									if (vec->ToSimpleShape().SetTo(vec2))
									{
										vec.Delete();
										vec = vec2;
									}
								}
								if (fixError)
								{
									vec->FixError();
								}
								if (srConv)
								{
									UInt32 srcSRID = vec->GetSRID();
									UInt32 destSRID = col->GetGeometrySRID();
									if (srcSRID != 0 && srcSRID != destSRID)
									{
										if (!csysDB.SetTo(nncsysDB))
										{
											nncsysDB = Math::CoordinateSystemManager::SRCreateCSysOrDef(destSRID);
											csysDB = nncsysDB;
										}
										if (csysSrc.SetTo(nncsysSrc))
										{
											if (nncsysSrc->GetSRID() != srcSRID)
											{
												nncsysSrc.Delete();
												nncsysSrc = Math::CoordinateSystemManager::SRCreateCSysOrDef(srcSRID);
												csysSrc = nncsysSrc;
											}
										}
										else
										{
											nncsysSrc = Math::CoordinateSystemManager::SRCreateCSysOrDef(srcSRID);
											csysSrc = nncsysSrc;
										}
										Math::CoordinateSystemConverter csysConv(nncsysSrc, nncsysDB);
										vec->Convert(csysConv);
									}
								}
								sql.AppendVector(vec);
								vec.Delete();
							}
							else
							{
								AppendCol(sql, col, ops, this->dataTz, dbSrid);
							}
						}
						else
						{
							AppendCol(sql, col, ops, this->dataTz, dbSrid);
						}
						OPTSTR_DEL(ops);
					}
					i++;
				}
				sql.AppendCmdC(CSTR(")"));
				if (!sqlHdlr(userObj, sql.ToCString()))
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
		if (!this->srcConn->QueryTableData(this->srcSchema, this->srcTable, nullptr, 0, 0, nullptr, srcCond).SetTo(r))
		{
			this->SetLastError(CSTR("Error in getting table data"));
		}
		else
		{
			NN<Text::String> id1;
			Optional<Text::String> id2;
			Data::ArrayListStringNN idList;
			Data::ArrayListStringNN sqlList;
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
					if (this->srcSchema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
					{
						sql.AppendCol(schema.v);
						sql.AppendCmdC(CSTR("."));
					}
					sql.AppendCol(this->srcTable.v);
					sql.AppendCmdC(CSTR(" set "));

					i = 0;
					while (i < dbCnt)
					{
						if (this->colInd.GetItem(i) == INVALID_INDEX)
						{

						}
						else if (r->IsNull(i))
						{
							if (!rowData[i].SetTo(s2))
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
											Data::Date ts2 = Data::Timestamp::FromStr(s2->ToCString(), this->dataTz).ToDate();
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
											Data::Timestamp ts2 = Data::Timestamp::FromStr(s2->ToCString(), this->dataTz);
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
											Double v2 = s2->ToDoubleOrNAN();
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
											Int32 v2 = s2->ToInt32();
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
											Int64 v2 = s2->ToInt64();
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
											Bool v2 = s2->v[0] == 't' || s2->v[0] == 'T' || s2->ToInt32() != 0;
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
											if (!String2Vector(s2, dbSrid).SetTo(vec2))
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
										{
											UnsafeArray<UInt8> binBuff = MemAllocArr(UInt8, s2->leng >> 1);
											UIntOS binLeng = s2->Hex2Bytes(binBuff);
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
											sql.AppendBinary(UnsafeArray<const UInt8>(binBuff), binLeng);
											MemFreeArr(binBuff);
										}
										break;
									case DB::DBUtil::CT_UUID:
										{
											Data::UUID uuid2;
											uuid2.SetValue(s2->ToCString());
										}
										break;
									case DB::DBUtil::CT_Unknown:
									default:
										break;
									}
								}
							}
						}
						else if (!rowData[i].SetTo(s2))
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
							sql.AppendNull();
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
								if (!s->Equals(s2->v, s2->leng))
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
									Data::Date ts2 = Data::Timestamp::FromStr(s2->ToCString(), this->dataTz).ToDate();
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
									Data::Timestamp ts2 = Data::Timestamp::FromStr(s2->ToCString(), this->dataTz);
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
									Double v2 = s2->ToDoubleOrNAN();
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
									Int32 v2 = s2->ToInt32();
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
									Int64 v2 = s2->ToInt64();
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
									Bool v2 = s2->v[0] == 't' || s2->v[0] == 'T' || s2->ToInt32() != 0;
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
										vec1->SetSRID(dbSrid);
										if (!String2Vector(s2, vec1->GetSRID()).SetTo(vec2))
										{
											printf("Error in parsing WKT: %s\r\n", s2->v.Ptr());
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
								{
									UIntOS binLeng1 = r->GetBinarySize(i);
									UnsafeArray<UInt8> binBuff1 = MemAllocArr(UInt8, binLeng1);
									binLeng1 = r->GetBinary(i, binBuff1);
									UIntOS binLeng2;
									UnsafeArray<UInt8> binBuff2 = MemAllocArr(UInt8, s2->leng >> 1);
									binLeng2 = s2->Hex2Bytes(binBuff2);
									if (Text::StrEqualsC(binBuff1, binLeng1, binBuff2, binLeng2))
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
										sql.AppendBinary(UnsafeArray<const UInt8>(binBuff2), binLeng2);
									}	
									MemFreeArr(binBuff1);
									MemFreeArr(binBuff2);
								}
								break;
							case DB::DBUtil::CT_UUID:
								{
									Data::UUID uuid1;
									Data::UUID uuid2;
									r->GetUUID(i, uuid1);
									uuid2.SetValue(s2->ToCString());
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
						sqlList.Add(Text::String::New(sql.ToCString()));
					}
				}
				else
				{
					sql.Clear();
					sql.AppendCmdC(CSTR("delete from "));
					Text::CStringNN schema;
					if (this->srcSchema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
					{
						sql.AppendCol(schema.v);
						sql.AppendCmdC(CSTR("."));
					}
					sql.AppendCol(this->srcTable.v);
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
					sqlList.Add(Text::String::New(sql.ToCString()));
				}
				id1->Release();
				OPTSTR_DEL(id2);
			}
			this->srcConn->CloseReader(r);
			if (succ)
			{
				UIntOS i = 0;
				UIntOS j = sqlList.GetCount();
				while (i < j)
				{
					if (!sqlHdlr(userObj, sqlList.GetItemNoCheck(i)->ToCString()))
					{
						succ = false;
						break;
					}
					i++;
				}
			}
			sqlList.FreeAll();

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
						if (this->srcSchema.SetTo(schema) && schema.leng > 0 && DB::DBUtil::HasSchema(sqlType))
						{
							sql.AppendCol(schema.v);
							sql.AppendCmdC(CSTR("."));
						}
						sql.AppendCol(this->srcTable.v);
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
								AppendCol(sql, col, rowData[i], this->dataTz, dbSrid);
							}
							i++;
						}
						sql.AppendCmdC(CSTR(")"));
						if (!sqlHdlr(userObj, sql.ToCString()))
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
			OPTSTR_DEL(rowData[i]);
		}
		MemFree(rowData);
	}
	table.Delete();
	csysSrc.Delete();
	csysDB.Delete();
	return succ;
}

Optional<Text::String> DB::DBChangeChecker::GetLastError() const
{
	return this->lastError;
}

UIntOS DB::DBChangeChecker::GetDataFileRowCnt() const
{
	return this->dataFileRowCnt;
}

UIntOS DB::DBChangeChecker::GetNoChgCnt() const
{
	return this->noChgCnt;
}

UIntOS DB::DBChangeChecker::GetNewRowCnt() const
{
	return this->newRowCnt;
}

UIntOS DB::DBChangeChecker::GetDelRowCnt() const
{
	return this->delRowCnt;
}

UIntOS DB::DBChangeChecker::GetUpdateCnt() const
{
	return this->updateCnt;
}
