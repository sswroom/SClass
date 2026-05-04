#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "DB/TableDef.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "SSWR/AIDemo/AIDemoCore.h"
#include "SSWR/AIDemo/AIDemoHandler.h"
#include "Sync/MutexUsage.h"

Bool __stdcall SSWR::AIDemo::AIDemoHandler::BotFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<AIDemoHandler> me = NN<AIDemoHandler>::ConvertFrom(svcHdlr);
	NN<Text::String> question;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	req->ParseHTTPForm();
	if (!req->GetHTTPFormStr(CSTR("q")).SetTo(question))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
	NN<Net::OpenAIClient> cli;
	NN<Text::String> dbDDL;
	NN<Text::String> dbRule;
	NN<DB::DBTool> db;
	if (!me->core->GetAIClient().SetTo(cli) || !me->core->GetDBDDL().SetTo(dbDDL) || !me->core->GetDBRule().SetTo(dbRule) || !me->core->GetDB().SetTo(db))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("Database DDL:\n"));
	sb.Append(dbDDL);
	sb.Append(CSTR("\n\nDatabase Rule:\n"));
	sb.Append(dbRule);
	sb.Append(CSTR("\n\nCan you generate "));
	sb.Append(DB::SQLTypeGetName(db->GetSQLType()));
	sb.Append(CSTR(" query for this: \""));
	sb.Append(question);
	sb.AppendUTF8Char('\"');
	Net::OpenAIResponse aiResp(cli->GetCurrModel(), sb.ToCString());
	NN<Net::OpenAIResult> result = cli->SendResponses(aiResp);
	NN<Text::String> outputText = result->GetOutputText();
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddInt32(CSTR("status"), (Int32)result->GetStatusCode());
	json.ObjectAddStr(CSTR("answer"), outputText);
	UIntOS i = outputText->IndexOfICase(CSTR("```sql\n"));
	if (i != INVALID_INDEX)
	{
		i += 7;
		UIntOS j = outputText->IndexOf(UTF8STRC("```"), i);
		if (j != INVALID_INDEX)
		{
			sb.ClearStr();
			sb.AppendC(outputText->v + i, j - i);
			json.ObjectAddStr(CSTR("sql"), sb.ToCString());

			if (sb.StartsWithICase(UTF8STRC("SELECT ")))
			{
				NN<DB::DBReader> r;
				if (db->ExecuteReader(sb.ToCString()).SetTo(r))
				{
					UIntOS colCnt = r->ColCount();
					UnsafeArray<DB::DBUtil::ColType> colTypes = MemAllocArr(DB::DBUtil::ColType, colCnt);
					Bool hasGeometry = false;
					Bool singleNumCol = false;
					UIntOS pkCol = INVALID_INDEX;
					UIntOS geomColIndex = 0;
					DB::ColDef colDef(CSTR(""));
					i = 0;
					while (i < colCnt)
					{
						r->GetColDef(i, colDef);
						colTypes[i] = colDef.GetColType();
						if (colDef.IsPK() || colDef.GetColName()->Equals(CSTR("objectid")) || colDef.GetColName()->Equals(CSTR("burial_no_gen")) || (colDef.GetColName()->EndsWithICase(UTF8STRC("id")) && pkCol == INVALID_INDEX))
						{
							pkCol = i;
						}
						if (colDef.GetColType() == DB::DBUtil::CT_Vector)
						{
							hasGeometry = true;
							geomColIndex = i;
						}
						else if (colCnt == 1)
						{
							singleNumCol = colTypes[i] == DB::DBUtil::CT_Int32 || colTypes[i] == DB::DBUtil::CT_Int64 || colTypes[i] == DB::DBUtil::CT_UInt32 || colTypes[i] == DB::DBUtil::CT_UInt64;
						}
						i++;
					}
					if (singleNumCol)
					{
						if (r->ReadNext())
						{
							json.ObjectAddInt64(CSTR("resultValue"), r->GetInt64(0));
						}
					}
					else if (hasGeometry)
					{
						json.ObjectBeginObject(CSTR("resultGeoJSON"));
						json.ObjectAddStr(CSTR("type"), CSTR("FeatureCollection"));
						json.ObjectBeginArray(CSTR("features"));
						while (r->ReadNext())
						{
							json.ArrayBeginObject();
							json.ObjectAddStr(CSTR("type"), CSTR("Feature"));
							if (pkCol != INVALID_INDEX)
							{
								if (colTypes[pkCol] == DB::DBUtil::CT_Int32 || colTypes[pkCol] == DB::DBUtil::CT_UInt32)
								{
									json.ObjectAddInt32(CSTR("id"), r->GetInt32(pkCol));
								}
								else if (colTypes[pkCol] == DB::DBUtil::CT_Int64 || colTypes[pkCol] == DB::DBUtil::CT_UInt64)
								{
									json.ObjectAddInt64(CSTR("id"), r->GetInt64(pkCol));
								}
								else
								{
									sb.ClearStr();
									r->GetStr(pkCol, sb);
									json.ObjectAddStr(CSTR("id"), sb.ToCString());
								}
							}
							NN<Math::Geometry::Vector2D> vec;
							if (r->GetVector(geomColIndex).SetTo(vec))
							{
								json.ObjectAddGeometry(CSTR("geometry"), vec);
								vec.Delete();
							}
							else
							{
								json.ObjectAddNull(CSTR("geometry"));
							}
							json.ObjectBeginObject(CSTR("properties"));
							i = 0;
							while (i < colCnt)
							{
								if (i != geomColIndex)
								{
									sptr = r->GetName(i, sbuff).Or(sbuff);
									if (r->IsNull(i))
									{
										json.ObjectAddNull(CSTRP(sbuff, sptr));
									}
									else
									{
										switch (colTypes[i])
										{
										case DB::DBUtil::CT_Byte:
										case DB::DBUtil::CT_Int16:
										case DB::DBUtil::CT_UInt16:
										case DB::DBUtil::CT_Int32:
										case DB::DBUtil::CT_UInt32:
											json.ObjectAddInt32(CSTRP(sbuff, sptr), r->GetInt32(i));
											break;
										case DB::DBUtil::CT_Int64:
										case DB::DBUtil::CT_UInt64:
											json.ObjectAddInt64(CSTRP(sbuff, sptr), r->GetInt64(i));
											break;
										case DB::DBUtil::CT_Float:
										case DB::DBUtil::CT_Double:
										case DB::DBUtil::CT_Decimal:
											json.ObjectAddFloat64(CSTRP(sbuff, sptr), r->GetDblOrNAN(i));
											break;
										case DB::DBUtil::CT_Bool:
											json.ObjectAddBool(CSTRP(sbuff, sptr), r->GetBool(i));
											break;
										case DB::DBUtil::CT_Date:
										case DB::DBUtil::CT_DateTime:
										case DB::DBUtil::CT_DateTimeTZ:
										case DB::DBUtil::CT_UTF8Char:
										case DB::DBUtil::CT_UTF16Char:
										case DB::DBUtil::CT_UTF32Char:
										case DB::DBUtil::CT_VarUTF8Char:
										case DB::DBUtil::CT_VarUTF16Char:
										case DB::DBUtil::CT_VarUTF32Char:
										case DB::DBUtil::CT_Binary:
										case DB::DBUtil::CT_Vector:
										case DB::DBUtil::CT_UUID:
										case DB::DBUtil::CT_Unknown:
										default:
											sb.ClearStr();
											r->GetStr(i, sb);
											json.ObjectAddStr(CSTRP(sbuff, sptr), sb.ToCString());
											break;
										}
									}
								}
								i++;
							}
							json.ObjectEnd();
							json.ObjectEnd();
						}
						json.ArrayEnd();
						json.ObjectEnd();
					}
					else
					{
						json.ObjectBeginObject(CSTR("resultTable"));
						json.ObjectBeginArray(CSTR("header"));
						i = 0;
						while (i < colCnt)
						{
							sptr = r->GetName(i, sbuff).Or(sbuff);
							json.ArrayAddStr(CSTRP(sbuff, sptr));
							i++;
						}
						json.ArrayEnd();
						json.ObjectBeginArray(CSTR("data"));
						while (r->ReadNext())
						{
							json.ArrayBeginArray();
							i = 0;
							while (i < colCnt)
							{
								if (r->IsNull(i))
								{
									json.ArrayAddNull();
								}
								else
								{
									switch (colTypes[i])
									{
									case DB::DBUtil::CT_Byte:
									case DB::DBUtil::CT_Int16:
									case DB::DBUtil::CT_UInt16:
									case DB::DBUtil::CT_Int32:
									case DB::DBUtil::CT_UInt32:
										json.ArrayAddInt32(r->GetInt32(i));
										break;
									case DB::DBUtil::CT_Int64:
									case DB::DBUtil::CT_UInt64:
										json.ArrayAddInt64(r->GetInt64(i));
										break;
									case DB::DBUtil::CT_Float:
									case DB::DBUtil::CT_Double:
									case DB::DBUtil::CT_Decimal:
										json.ArrayAddFloat64(r->GetDblOrNAN(i));
										break;
									case DB::DBUtil::CT_Bool:
										json.ArrayAddBool(r->GetBool(i));
										break;
									case DB::DBUtil::CT_Date:
									case DB::DBUtil::CT_DateTime:
									case DB::DBUtil::CT_DateTimeTZ:
									case DB::DBUtil::CT_UTF8Char:
									case DB::DBUtil::CT_UTF16Char:
									case DB::DBUtil::CT_UTF32Char:
									case DB::DBUtil::CT_VarUTF8Char:
									case DB::DBUtil::CT_VarUTF16Char:
									case DB::DBUtil::CT_VarUTF32Char:
									case DB::DBUtil::CT_Binary:
									case DB::DBUtil::CT_Vector:
									case DB::DBUtil::CT_UUID:
									case DB::DBUtil::CT_Unknown:
									default:
										sb.ClearStr();
										r->GetStr(i, sb);
										json.ArrayAddStr(sb.ToCString());
										break;
									}
								}
								i++;
							}
							json.ArrayEnd();
						}
						json.ArrayEnd();
						json.ObjectEnd();
					}
					MemFreeArr(colTypes);
					db->CloseReader(r);
				}
			}
		}
	}
	outputText->Release();
	result.Delete();
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

SSWR::AIDemo::AIDemoHandler::AIDemoHandler(Text::CStringNN rootDir, NN<IO::LogTool> log, NN<SSWR::AIDemo::AIDemoCore> core) : WebServiceHandler(rootDir)
{
	this->log = log;
	this->core = core;
	this->AddService(CSTR("/api/bot"), Net::WebUtil::RequestMethod::HTTP_POST, BotFunc);
}

SSWR::AIDemo::AIDemoHandler::~AIDemoHandler()
{
}
