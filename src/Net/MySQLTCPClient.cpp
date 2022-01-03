#include "Stdafx.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/DBUtil.h"
#include "IO/OS.h"
#include "Net/MySQLTCPClient.h"
#include "Net/MySQLUtil.h"
#include "Net/SocketUtil.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

#define CLIVERSION "1.0.0"
#define BUFFSIZE 65536

//#define VERBOSE
#if defined(VERBOSE)
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

#ifdef __WIN32
#include <windows.h>
#define printf(...) { Char dbgbuff[8192]; sprintf(dbgbuff, __VA_ARGS__ ); OutputDebugStringA(dbgbuff);}
#endif

#endif

class MySQLTCPReader : public DB::DBReader
{
private:
	typedef struct
	{
		Text::String *name;
		Text::String *defValues;
		UInt32 colLen;
		UInt16 charSet;
		UInt16 flags;
		Net::MySQLUtil::MySQLType colType;
		DB::DBUtil::ColType dbColType;
		UInt8 decimals;
	} ColumnDef;

	Data::ArrayList<ColumnDef*> *cols;
	UOSInt colCount;
	OSInt rowChanged;
	Text::String **currRow;
	Text::String **nextRow;
	Bool nextRowReady;
	Sync::Event *rowEvt;
	Sync::Event *nextRowEvt;
	Sync::MutexUsage *mutUsage;
public:
	MySQLTCPReader(Sync::MutexUsage *mutUsage)
	{
		this->mutUsage = mutUsage;
		NEW_CLASS(this->cols, Data::ArrayList<ColumnDef*>());
		this->rowChanged = -1;
		this->currRow = 0;
		this->nextRow = 0;
		this->colCount = 0;
		this->nextRowReady = false;
		NEW_CLASS(this->nextRowEvt, Sync::Event(true, (const UTF8Char*)"MySQLTCPReader.nextRowEvt"));
		NEW_CLASS(this->rowEvt, Sync::Event(true, (const UTF8Char*)"MySQLTCPReader.nextRowEvt"));
	}

	virtual ~MySQLTCPReader()
	{
		ColumnDef *col;
		while (this->ReadNext())
		{

		}
		UOSInt i = this->cols->GetCount();
		while (i-- > 0)
		{
			col = this->cols->GetItem(i);
			col->name->Release();
			SDEL_STRING(col->defValues);
			MemFree(col); 
		}
		DEL_CLASS(this->cols);
		DEL_CLASS(this->rowEvt);
		DEL_CLASS(this->nextRowEvt);
		DEL_CLASS(this->mutUsage);
	}

	static DB::DBUtil::ColType DBType2ColType(Net::MySQLUtil::MySQLType colType)
	{
		switch (colType)
		{
			case Net::MySQLUtil::MYSQL_TYPE_DECIMAL:
				return DB::DBUtil::CT_Double;
			case Net::MySQLUtil::MYSQL_TYPE_TINY:
				return DB::DBUtil::CT_Byte;
			case Net::MySQLUtil::MYSQL_TYPE_SHORT:
				return DB::DBUtil::CT_Int16;
			case Net::MySQLUtil::MYSQL_TYPE_LONG:
				return DB::DBUtil::CT_Int32;
			case Net::MySQLUtil::MYSQL_TYPE_FLOAT:
				return DB::DBUtil::CT_Float;
			case Net::MySQLUtil::MYSQL_TYPE_DOUBLE:
				return DB::DBUtil::CT_Double;
			case Net::MySQLUtil::MYSQL_TYPE_NULL:
				return DB::DBUtil::CT_Unknown;
			case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_LONGLONG:
				return DB::DBUtil::CT_Int64;
			case Net::MySQLUtil::MYSQL_TYPE_INT24:
				return DB::DBUtil::CT_Int32;
			case Net::MySQLUtil::MYSQL_TYPE_DATE:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_TIME:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_DATETIME:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_YEAR:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_NEWDATE:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_VARCHAR:
				return DB::DBUtil::CT_VarChar;
			case Net::MySQLUtil::MYSQL_TYPE_BIT:
				return DB::DBUtil::CT_Bool;
			case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP2:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_DATETIME2:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_TIME2:
				return DB::DBUtil::CT_DateTime;
			case Net::MySQLUtil::MYSQL_TYPE_NEWDECIMAL:
				return DB::DBUtil::CT_Double;
			case Net::MySQLUtil::MYSQL_TYPE_ENUM:
				return DB::DBUtil::CT_Int32;
			case Net::MySQLUtil::MYSQL_TYPE_SET:
				return DB::DBUtil::CT_Unknown;
			case Net::MySQLUtil::MYSQL_TYPE_TINY_BLOB:
				return DB::DBUtil::CT_Binary;
			case Net::MySQLUtil::MYSQL_TYPE_MEDIUM_BLOB:
				return DB::DBUtil::CT_Binary;
			case Net::MySQLUtil::MYSQL_TYPE_LONG_BLOB:
				return DB::DBUtil::CT_Binary;
			case Net::MySQLUtil::MYSQL_TYPE_BLOB:
				return DB::DBUtil::CT_Binary;
			case Net::MySQLUtil::MYSQL_TYPE_VAR_STRING:
				return DB::DBUtil::CT_VarChar;
			case Net::MySQLUtil::MYSQL_TYPE_STRING:
				return DB::DBUtil::CT_VarChar;
			case Net::MySQLUtil::MYSQL_TYPE_GEOMETRY:
				return DB::DBUtil::CT_Vector;
			default:
				return DB::DBUtil::CT_Unknown;
		}
	}

	virtual Bool ReadNext()
	{
		UOSInt i;
		if (this->currRow)
		{
			i = this->colCount;
			while (i-- > 0)
			{
				SDEL_STRING(this->currRow[i]);
			}
			MemFree(this->currRow);
			this->currRow = 0;
		}
		while (!this->nextRowReady)
		{
			this->rowEvt->Wait(1000);
		}
		if (this->nextRow)
		{
			this->currRow = this->nextRow;
			this->nextRowReady = false;
			this->nextRowEvt->Set();
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual UOSInt ColCount()
	{
		return this->colCount;
	}

	virtual OSInt GetRowChanged()
	{
		return this->rowChanged;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrToInt32(this->currRow[colIndex]->v);
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrToInt64(this->currRow[colIndex]->v);
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrUTF8_WChar(buff, this->currRow[colIndex]->v, 0);
	}

	virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
	{
		if (this->currRow == 0)
		{
			return false;
		}
		if (colIndex >= this->colCount)
		{
			return false;
		}
		if (this->currRow[colIndex] == 0)
		{
			return false;
		}
		sb->AppendC(this->currRow[colIndex]->v, this->currRow[colIndex]->leng);
		return true;
	}

	virtual Text::String *GetNewStr(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return this->currRow[colIndex]->Clone();
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrConcatS(buff, this->currRow[colIndex]->v, buffSize);
	}

	virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal)
	{
		if (this->currRow == 0)
		{
			return DET_NULL;
		}
		if (colIndex >= this->colCount)
		{
			return DET_NULL;
		}
		if (this->currRow[colIndex] == 0)
		{
			return DET_NULL;
		}
		if (outVal->SetValue(this->currRow[colIndex]->v))
		{
			return DET_OK;
		}
		else
		{
			return DET_ERROR;
		}
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrToDouble(this->currRow[colIndex]->v);
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return false;
		}
		if (colIndex >= this->colCount)
		{
			return false;
		}
		if (this->currRow[colIndex] == 0)
		{
			return false;
		}
		return Text::StrToInt32(this->currRow[colIndex]->v) != 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		/////////////////////////////
		return 0;
	}

	virtual UOSInt GetBinary(UOSInt UOSInt, UInt8 *buff)
	{
		/////////////////////////////
		return 0;
	}

	virtual Math::Vector2D *GetVector(UOSInt colIndex)
	{
		/////////////////////////////
		return 0;
	}

	virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid)
	{
		/////////////////////////////
		return false;
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return true;
		}
		if (colIndex >= this->colCount)
		{
			return true;
		}
		return this->currRow[colIndex] == 0;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			return Text::StrConcat(buff, col->name->v);
		}
		return 0;
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			*colSize = col->colLen;
			return col->dbColType;
		}
		return DB::DBUtil::CT_Unknown;
	}

	virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			colDef->SetColName(col->name);
			colDef->SetColDP(col->decimals);
			colDef->SetColSize(col->colLen);
			colDef->SetColType(DBType2ColType(col->colType));
			return true;
		}
		return false;
	}

	void SetRowChanged(Int64 val)
	{
		this->rowChanged = (OSInt)val;
		this->nextRowReady = true;
	}

	void AddColumnDef41(const UInt8 *colDef, UOSInt buffSize)
	{
		UInt64 v;
		ColumnDef *col = MemAlloc(ColumnDef, 1);
		const UInt8 *colEnd = colDef + buffSize;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //catalog
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //schema
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //table
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //org_table
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //name
		col->name = Text::String::New(colDef, (UOSInt)v);
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //org_name
		colDef += v;
		if (colDef[0] != 12)
		{
			col->name->Release();
			MemFree(col);
			return;
		}
		colDef += 1;
		col->charSet = ReadUInt16(&colDef[0]);
		col->colLen = ReadUInt32(&colDef[2]);
		col->colType = (Net::MySQLUtil::MySQLType)colDef[6];
		col->dbColType = DBType2ColType(col->colType);
		col->flags = ReadUInt16(&colDef[7]);
		col->decimals = colDef[9];
		colDef += 12;
		col->defValues = 0;
		if (colDef < colEnd)
		{
			colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //catalog
			if ((colDef + v) <= colEnd)
			{
				col->defValues = Text::String::New(colDef, (UOSInt)v);
			}
		}
		this->cols->Add(col);
		this->colCount++;
	}

	void AddRowData(const UInt8 *rowData, UOSInt dataSize)
	{
		Text::String **row = MemAlloc(Text::String *, this->colCount);
		UOSInt i = 0;
		UOSInt j = this->colCount;
		UInt64 v;
		while (i < j)
		{
			if (rowData[0] == 0xfb)
			{
				row[i] = 0;
				rowData++;
			}
			else
			{
				rowData = Net::MySQLUtil::ReadLenencInt(rowData, &v);
				row[i] = Text::String::New(rowData, (UOSInt)v);
				rowData += v;
			}
			i++;
		}
		while (this->nextRowReady)
		{
			this->nextRowEvt->Wait(1000);
		}
		this->nextRow = row;
		this->nextRowReady = true;
		this->rowEvt->Set();
	}

	void EndData()
	{
		if (this->nextRow == 0 && this->nextRowReady)
		{
			return;
		}
		while (this->nextRowReady)
		{
			this->nextRowEvt->Wait(1000);
		}
		this->nextRow = 0;
		this->nextRowReady = true;
		this->rowEvt->Set();
	}
};

UInt32 __stdcall Net::MySQLTCPClient::RecvThread(void *userObj)
{
	Net::MySQLTCPClient *me = (Net::MySQLTCPClient*)userObj;
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt readSize;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char sbuff2[128];
	UInt8 *ptrCurr;
	UInt8 *ptrEnd;
	OSInt i;
#if defined(VERBOSE)
	Text::StringBuilderUTF8 *sb;
#endif
	me->recvStarted = true;
	me->recvRunning = true;
	buffSize = 0;
#if defined(VERBOSE)
	NEW_CLASS(sb, Text::StringBuilderUTF8());
#endif
	buff = MemAlloc(UInt8, BUFFSIZE);
	while (true)
	{
		readSize = me->cli->Read(&buff[buffSize], BUFFSIZE - buffSize);
		if (readSize <= 0)
			break;
#if defined(VERBOSE)
		sb->ClearStr();
		sb->AppendHexBuff(&buff[buffSize], readSize, ' ', Text::LineBreakType::CRLF);
		printf("MySQLTCP Received Buff:\r\n%s\r\n", sb->ToString());
#endif
		buffSize += readSize;

		if (me->mode == 0)
		{
			readSize = buffSize;
			if (buffSize >= 10)
			{
				if (buff[3] != 0)
				{
#if defined(VERBOSE)
					printf("MySQLTCP Seq Id Invalid\r\n");
#endif
					me->cli->Close();
					readSize = 0;
				}
				else
				{
					UOSInt packetSize = ReadUInt32(buff);
					if (packetSize < 10 || packetSize > 1024)
					{
#if defined(VERBOSE)
						printf("MySQLTCP packet size Invalid\r\n");
#endif
						me->cli->Close();
						readSize = 0;
					}
					else if (packetSize + 4 <= buffSize)
					{
						if (buff[4] == 9)
						{
							if (buff[packetSize + 3] != 0 || buff[packetSize - 10] != 0)
							{
#if defined(VERBOSE)
								printf("MySQLTCP protocol ver 9 invalid\r\n");
#endif
								me->cli->Close();
								readSize = 0;
							}
							else
							{
								me->svrVer = Text::String::NewNotNull(&buff[5]);
								me->connId = ReadUInt32(&buff[packetSize - 9]);
								MemCopyNO(me->authPluginData, &buff[packetSize - 5], 8);
								me->authPluginDataSize = 8;
								me->mode = 1;
								////////////////////////////////
#if defined(VERBOSE)
								printf("MySQLTCP Server ver = %s\r\n", me->svrVer);
								printf("MySQLTCP Conn Id = %d\r\n", me->connId);
								sb->ClearStr();
								sb->AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
								printf("MySQLTCP Auth Plugin Data = %s\r\n", sb->ToString());
#endif
								readSize = 0;
							}
						}
						else if (buff[4] == 10)
						{
							ptrEnd = &buff[packetSize + 4];
							sptr = Text::StrConcatS(sbuff, &buff[5], packetSize - 1);
							ptrCurr = &buff[6] + (sptr - sbuff);
							me->svrVer = Text::String::NewNotNull(sbuff);
#if defined(VERBOSE)
							printf("MySQLTCP Server ver = %s\r\n", me->svrVer);
#endif
							if (ptrEnd - ptrCurr >= 15)
							{
								me->connId = ReadUInt32(&ptrCurr[0]);
								MemCopyNO(me->authPluginData, &ptrCurr[4], 8);
								me->authPluginDataSize = 8;
								me->svrCap = ReadUInt16(&ptrCurr[13]);
								ptrCurr += 15;
								if (ptrEnd - ptrCurr >= 16)
								{
									me->svrCS = ptrCurr[0];
									me->connStatus = ReadUInt16(&ptrCurr[1]);
									me->svrCap |= ((UInt32)ReadUInt16(&ptrCurr[3])) << 16;
									UInt8 len = ptrCurr[5];
									ptrCurr += 16;
									if (me->svrCap & Net::MySQLUtil::CLIENT_SECURE_CONNECTION)
									{
										if (len == 21 && (ptrEnd - ptrCurr) >= 13)
										{
											MemCopyNO(&me->authPluginData[8], ptrCurr, 12);
											me->authPluginDataSize = 20;
											ptrCurr += 13;
										}
										else if (len > 21)
										{
											ptrCurr += 13;
										}
										else
										{
											ptrCurr += len - 8;
										}
									}
									if (me->svrCap & Net::MySQLUtil::CLIENT_PLUGIN_AUTH)
									{
										Text::StrConcatS(sbuff, ptrCurr, (UOSInt)(ptrEnd - ptrCurr));
									}
									else
									{
										sbuff[0] = 0;
									}
#if defined(VERBOSE)
									printf("MySQLTCP Conn Id = %d\r\n", me->connId);
									printf("MySQLTCP Server Cap = 0x%x\r\n", me->svrCap);
									printf("MySQLTCP character set = %d\r\n", me->svrCS);
									printf("MySQLTCP status = 0x%x\r\n", me->connStatus);
									sb->ClearStr();
									sb->AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
									printf("MySQLTCP auth plugin data = %s\r\n", sb->ToString());
									printf("MySQLTCP auth plugin name = %s\r\n", sbuff);
#endif
								}
								else
								{
#if defined(VERBOSE)
									printf("MySQLTCP Conn Id = %d\r\n", me->connId);
									printf("MySQLTCP Server Cap = 0x%x\r\n", me->svrCap);
									sb->ClearStr();
									sb->AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
									printf("MySQLTCP auth plugin data = %s\r\n", sb->ToString());
#endif
								}
								me->mode = 1;

								if (me->authPluginDataSize == 20)
								{
									UInt32 cliCap = Net::MySQLUtil::CLIENT_LONG_PASSWORD | 
										Net::MySQLUtil::CLIENT_FOUND_ROWS |
										Net::MySQLUtil::CLIENT_LONG_FLAG |
										Net::MySQLUtil::CLIENT_PROTOCOL_41 |
										Net::MySQLUtil::CLIENT_TRANSACTIONS |
										Net::MySQLUtil::CLIENT_SECURE_CONNECTION |
										Net::MySQLUtil::CLIENT_MULTI_RESULTS |
										Net::MySQLUtil::CLIENT_PLUGIN_AUTH |
										Net::MySQLUtil::CLIENT_CONNECT_ATTRS |
										Net::MySQLUtil::CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA;
									if (me->database)
									{
										cliCap |= Net::MySQLUtil::CLIENT_CONNECT_WITH_DB;
									}
									WriteUInt32(&buff[4], cliCap);
									WriteInt32(&buff[8], 16777215);
									buff[12] = 45;
									MemClear(&buff[13], 23);
									ptrCurr = me->userName->ConcatTo(&buff[36]) + 1;
									ptrCurr[0] = 20;
									Crypto::Hash::SHA1 sha1;
									sha1.Calc(me->password->v, me->password->leng);
									sha1.GetValue(sbuff);
									sha1.Clear();
									sha1.Calc(sbuff, 20);
									sha1.GetValue(&ptrCurr[1]);
									sha1.Clear();
									sha1.Calc(me->authPluginData, 20);
									sha1.Calc(&ptrCurr[1], 20);
									sha1.GetValue(&ptrCurr[1]);
									i = 0;
									while (i < 20)
									{
										ptrCurr[i + 1] ^= sbuff[i];
										i++;
									}
									ptrCurr += 21;
									if (me->database)
									{
										ptrCurr = me->database->ConcatTo(ptrCurr) + 1;
									}
									if (cliCap & Net::MySQLUtil::CLIENT_PLUGIN_AUTH)
									{
										ptrCurr = Text::StrConcat(ptrCurr, (const UTF8Char*)"mysql_native_password") + 1;
									}

									if (cliCap & Net::MySQLUtil::CLIENT_CONNECT_ATTRS)
									{
										sptr = sbuff;
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, (const UTF8Char*)"_client_name");
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, (const UTF8Char*)"MySQL TCP Client/SSWR");
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, (const UTF8Char*)"_client_version");
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, (const UTF8Char*)CLIVERSION);
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, (const UTF8Char*)"_os");
										IO::OS::GetDistro(sbuff2);
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, sbuff2);
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, (const UTF8Char*)"_os_version");
										IO::OS::GetVersion(sbuff2);
										sptr = Net::MySQLUtil::AppendLenencStr(sptr, sbuff2);
										ptrCurr = Net::MySQLUtil::AppendLenencInt(ptrCurr, (UOSInt)(sptr - sbuff));
										MemCopyNO(ptrCurr, sbuff, (UOSInt)(sptr - sbuff));
										ptrCurr += sptr - sbuff;
									}
									WriteInt24(buff, (ptrCurr - buff - 4));
									buff[3] = 1;
									me->cli->Write(buff, (UOSInt)(ptrCurr - buff));
#if defined(VERBOSE)
									printf("MySQLTCP handshake response sent\r\n");
#endif
								}

								readSize = 0;
							}
							else
							{
#if defined(VERBOSE)
								printf("MySQLTCP protocol version 10 invalid\r\n");
#endif
								me->mode = 1;
								////////////////////////////////
								readSize = 0;
								me->cli->Close();
							}
						}
						else
						{
#if defined(VERBOSE)
							printf("MySQLTCP protocol version unsupported (%d)\r\n", buff[4]);
#endif
							readSize = 0;
							me->cli->Close();
						}
					}
				}
			}
		}
		else if (me->mode == 1)
		{
			if (buffSize < 4)
			{
				readSize = buffSize;
			}
			else
			{
				readSize = ReadUInt24(&buff[buffSize - readSize]);
				if (readSize + 4 <= buffSize)
				{
					if (buff[4] == 0xff)
					{
						me->SetLastError(&buff[7], readSize - 3);
						me->cli->Close();
					}
					else if (buff[3] != 2)
					{
						me->SetLastError((const UTF8Char*)"Invalid login reply");
						me->cli->Close();
					}
					else if (buff[4] == 0)
					{
						me->mode = 2;
#if defined(VERBOSE)
						printf("MySQLTCP login success\r\n");
#endif
					}
					else if (buff[4] == 0xFE)
					{
						Text::StringBuilderUTF8 sb;
						sb.Append((const UTF8Char*)"AuthSwitchRequest received: plugin name = ");
						sb.Append(&buff[5]);
						me->SetLastError(sb.ToString());
						me->cli->Close();
					}
					else
					{
						me->SetLastError((const UTF8Char*)"Invalid reply on login");
						me->cli->Close();
					}
					readSize = 0;
				}
				else
				{
					readSize = buffSize;
				}
			}
		}
		else if (me->mode == 2)
		{
			UInt32 packetSize;
			UInt64 val;
			readSize = 0;
			while (readSize + 4 <= buffSize)
			{
				packetSize = ReadUInt24(&buff[readSize]);
				if (readSize + 4 + packetSize > buffSize)
				{
					break;
				}
				if (buff[readSize + 3] == (me->cmdSeqNum & 0xff) && me->cmdReader)
				{
					if (me->cmdSeqNum == 1)
					{
						if (buff[readSize + 4] == 0) //OK
						{
							Net::MySQLUtil::ReadLenencInt(&buff[readSize + 5], &val);
							((MySQLTCPReader*)me->cmdReader)->SetRowChanged((Int64)val);
							me->cmdResultType = 3;
							me->cmdEvt->Set();
#if defined(VERBOSE)
							printf("MySQLTCP Command OK, row changed = %lld\r\n", val);
#endif
						}
						else if (buff[readSize + 4] == 0xFF) //ERR
						{
							me->SetLastError(&buff[readSize + 7], packetSize - 3);
							if (me->cmdReader)
							{
								if (me->cmdResultType == 0)
								{
									me->cmdResultType = 2;
									me->cmdEvt->Set();
								}
								((MySQLTCPReader*)me->cmdReader)->EndData();
							}
							else
							{
								me->cmdResultType = 2;
								me->cmdEvt->Set();
							}
//							me->cli->Close();
						}
						else
						{
							Net::MySQLUtil::ReadLenencInt(&buff[readSize + 4], &val);
#if defined(VERBOSE)
							printf("MySQLTCP Command executed, column cnt = %lld\r\n", val);
#endif
						}
					}
					else if (buff[readSize + 4] == 0xFE) //EOF
					{
#if defined(VERBOSE)
						printf("MySQLTCP EOF found, curr result type = %d\r\n", (int)me->cmdResultType);
#endif
						if (me->cmdResultType == 0)
						{
							me->cmdResultType = 1;
						}
						else
						{
							me->cmdResultType = 3;
							((MySQLTCPReader*)me->cmdReader)->EndData();
						}
						me->cmdEvt->Set();
					}
					else if (me->cmdResultType == 0) //ColumnDef
					{
#if defined(VERBOSE)
						printf("MySQLTCP Seq %d Column found\r\n", buff[readSize + 3]);
#endif
						((MySQLTCPReader*)me->cmdReader)->AddColumnDef41(&buff[readSize + 4], packetSize);
					}
					else
					{
#if defined(VERBOSE)
						printf("MySQLTCP Seq %d Row found\r\n", buff[readSize + 3]);
#endif
						((MySQLTCPReader*)me->cmdReader)->AddRowData(&buff[readSize + 4], packetSize);
					}
					me->cmdSeqNum++;
				}
				else
				{
#if defined(VERBOSE)
					printf("MySQLTCP Seq num mismatch: %d != %d\r\n", buff[readSize + 3], (int)me->cmdSeqNum);
#endif
				}
				readSize += packetSize + 4;
			}
			readSize = buffSize - readSize;
		}

		if (readSize == 0)
		{
			buffSize = 0;
		}
		else if (readSize < buffSize)
		{
			MemCopyO(buff, &buff[buffSize - readSize], readSize);
			buffSize = readSize;
		}
	}

	if (me->cmdReader)
	{
		if (me->cmdResultType == 0)
		{
#if defined(VERBOSE)	
			printf("MySQLTCP End Conn: signal waiting\r\n");
#endif
			me->cmdResultType = 2;
			me->cmdEvt->Set();
		}
		else
		{
#if defined(VERBOSE)	
			printf("MySQLTCP End Conn: end data\r\n");
#endif
			((MySQLTCPReader*)me->cmdReader)->EndData();
		}
	}
#if defined(VERBOSE)	
	printf("MySQLTCP End connection\r\n");
	DEL_CLASS(sb);
#endif
	MemFree(buff);
	me->recvRunning = false;
	return 0;
}

void Net::MySQLTCPClient::SetLastError(const UTF8Char *errMsg, UOSInt msgLen)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(errMsg, msgLen);
#if defined(VERBOSE)
	Text::StringBuilderUTF8 sb;
	this->GetErrorMsg(&sb);
	printf("MySQLTCP Error: %s\r\n", sb.ToString());
#endif
}

void Net::MySQLTCPClient::SetLastError(const UTF8Char *errMsg)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::NewOrNull(errMsg);
#if defined(VERBOSE)
	Text::StringBuilderUTF8 sb;
	this->GetErrorMsg(&sb);
	printf("MySQLTCP Error: %s\r\n", sb.ToString());
#endif
}

Net::MySQLTCPClient::MySQLTCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Text::String *userName, Text::String *password, Text::String *database) : DB::DBConn((const UTF8Char*)"MySQLTCPClient")
{
	this->sockf = sockf;
	this->recvRunning = false;
	this->recvStarted = false;
	this->addr = *addr;
	this->port = port;
	this->mode = 0;
	this->svrVer = 0;
	this->connId = 0;
	this->authPluginDataSize = 0;
	this->svrCap = 0;
	this->tableNames = 0;
	this->lastError = 0;
	this->userName = userName->Clone();
	this->password = password->Clone();
	this->database = SCOPY_STRING(database);
	NEW_CLASS(this->cmdMut, Sync::Mutex());
	NEW_CLASS(this->cmdEvt, Sync::Event(true, (const UTF8Char*)"Net.MySQLTCPCLient.cmdEvt"));
	this->cmdSeqNum = 0;
	this->cmdReader = 0;
	this->cmdResultType = 0;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	this->cli = 0;
	this->Reconnect();
}

Net::MySQLTCPClient::MySQLTCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UTF8Char *userName, const UTF8Char *password, const UTF8Char *database) : DB::DBConn((const UTF8Char*)"MySQLTCPClient")
{
	this->sockf = sockf;
	this->recvRunning = false;
	this->recvStarted = false;
	this->addr = *addr;
	this->port = port;
	this->mode = 0;
	this->svrVer = 0;
	this->connId = 0;
	this->authPluginDataSize = 0;
	this->svrCap = 0;
	this->tableNames = 0;
	this->lastError = 0;
	this->userName = Text::String::NewNotNull(userName);
	this->password = Text::String::NewNotNull(password);
	this->database = Text::String::NewOrNull(database);
	NEW_CLASS(this->cmdMut, Sync::Mutex());
	NEW_CLASS(this->cmdEvt, Sync::Event(true, (const UTF8Char*)"Net.MySQLTCPCLient.cmdEvt"));
	this->cmdSeqNum = 0;
	this->cmdReader = 0;
	this->cmdResultType = 0;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	this->cli = 0;
	this->Reconnect();
}

Net::MySQLTCPClient::~MySQLTCPClient()
{
	if (this->cli)
	{
		if (this->recvRunning)
		{
			this->cli->Close();
		}
		while (this->recvRunning)
		{
			Sync::Thread::Sleep(1);
		}
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	DEL_CLASS(this->cliMut);
	DEL_CLASS(this->cmdMut);
	DEL_CLASS(this->cmdEvt);
	this->userName->Release();
	this->password->Release();
	SDEL_STRING(this->database);
	SDEL_STRING(this->svrVer);
	SDEL_STRING(this->lastError);
	if (this->tableNames)
	{
		UOSInt i = this->tableNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->tableNames->GetItem(i));
		}
		DEL_CLASS(this->tableNames);
	}
}

DB::DBUtil::ServerType Net::MySQLTCPClient::GetSvrType()
{
	return DB::DBUtil::ServerType::MySQL;
}

DB::DBConn::ConnType Net::MySQLTCPClient::GetConnType()
{
	return DB::DBConn::CT_MYSQLTCP;
}

Int8 Net::MySQLTCPClient::GetTzQhr()
{
	return 0;
}

void Net::MySQLTCPClient::ForceTz(Int8 tzQhr)
{

}

void Net::MySQLTCPClient::GetConnName(Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	sb->Append((const UTF8Char*)"MySQLTCP:");
	Net::SocketUtil::GetAddrName(sbuff, &this->addr, this->port);
	sb->Append(sbuff);
	if (this->database)
	{
		sb->AppendChar('/', 1);
		sb->Append(this->database);
	}
}

void Net::MySQLTCPClient::Close()
{
	this->cli->Close();
}

void Net::MySQLTCPClient::Dispose()
{
	this->Close();
}

OSInt Net::MySQLTCPClient::ExecuteNonQuery(const UTF8Char *sql)
{
	return ExecuteNonQueryC(sql, Text::StrCharCnt(sql));
}

OSInt Net::MySQLTCPClient::ExecuteNonQueryC(const UTF8Char *sql, UOSInt sqlLen)
{
	DB::DBReader *reader = ExecuteReaderC(sql, sqlLen);
	if (reader == 0)
	{
		return -2;
	}
	else
	{
		OSInt cnt = reader->GetRowChanged();
		this->CloseReader(reader);
		return cnt;
	}
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReader(const UTF8Char *sql)
{
	return ExecuteReaderC(sql, Text::StrCharCnt(sql));
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReaderC(const UTF8Char *sql, UOSInt sqlLen)
{
	if (this->cli == 0 || !this->recvRunning)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	while (this->mode != 2)
	{
		if (this->cli == 0 || !this->recvRunning)
		{
			return 0;
		}
		Sync::Thread::Sleep(10);
	}
	MySQLTCPReader *reader;
	Sync::MutexUsage *mutUsage;
	NEW_CLASS(mutUsage, Sync::MutexUsage(this->cmdMut));
	this->cmdResultType = 0;
	this->cmdSeqNum = 1;
	NEW_CLASS(reader, MySQLTCPReader(mutUsage));
	this->cmdReader = reader;
	UInt8 *buff = MemAlloc(UInt8, sqlLen + 5);
	WriteInt32(buff, (Int32)(sqlLen + 1));
	buff[4] = 3;
	MemCopyNO(&buff[5], sql, sqlLen);
	if (this->cli->Write(buff, 5 + sqlLen) != 5 + sqlLen)
	{
		this->cmdReader = 0;
		DEL_CLASS(reader);
		MemFree(buff);
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	MemFree(buff);
#if defined(VERBOSE)
	printf("MySQLTCP Sent SQL: %s\r\n", sql);
#endif
//	dt.SetCurrTimeUTC();
//	startTime = dt.ToTicks();
	while (this->cmdResultType == 0)
	{
		this->cmdEvt->Wait(10000);
	}
	if (this->cmdResultType == 2)
	{
		this->cmdReader = 0;
		reader->EndData();
		DEL_CLASS(reader);
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	this->lastDataError = DE_NO_ERROR;
	return reader;
}

void Net::MySQLTCPClient::CloseReader(DB::DBReader *r)
{
	while (r->ReadNext())
	{
	}
	while (this->cmdResultType != 3)
	{
		this->cmdEvt->Wait(10000);
	}
	this->cmdReader = 0;
	MySQLTCPReader *reader = (MySQLTCPReader*)r;
	DEL_CLASS(reader);
}

void Net::MySQLTCPClient::GetErrorMsg(Text::StringBuilderUTF *str)
{
	if (this->lastError)
	{
		if (this->lastError->v[0] == '#')
		{
			str->AppendChar('[', 1);
			str->AppendC(&this->lastError->v[1], 5);
			str->AppendChar(']', 1);
			str->AppendChar(' ', 1);
			str->Append(&this->lastError->v[6]);
		}
		else
		{
			str->Append(this->lastError);
		}
	}
}

Bool Net::MySQLTCPClient::IsLastDataError()
{
	return this->lastDataError == DE_EXEC_SQL_ERROR;
}

void Net::MySQLTCPClient::Reconnect()
{
	if (this->cli && this->recvRunning)
	{
		return;
	}
	if (this->cli)
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	this->recvRunning = false;
	this->recvStarted = false;
	this->mode = 0;
	SDEL_STRING(this->svrVer);
	NEW_CLASS(this->cli, Net::TCPClient(this->sockf, &this->addr, this->port));
	if (this->cli->IsConnectError())
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
		this->SetLastError((const UTF8Char*)"Cannot connect to server");
	}
	else
	{
		this->cli->SetNoDelay(true);
		Sync::Thread::Create(RecvThread, this);
		while (!this->recvStarted)
		{
			Sync::Thread::Sleep(1);
		}
	}
}

void *Net::MySQLTCPClient::BeginTransaction()
{
	if (this->ExecuteNonQueryC(UTF8STRC("START TRANSACTION")) != -2)
	{
		return (void*)-1;
	}
	return 0;
}
void Net::MySQLTCPClient::Commit(void *tran)
{
	this->ExecuteNonQueryC(UTF8STRC("COMMIT"));
}

void Net::MySQLTCPClient::Rollback(void *tran)
{
	this->ExecuteNonQueryC(UTF8STRC("ROLLBACK"));
}

UOSInt Net::MySQLTCPClient::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->tableNames)
	{
		names->AddAll(this->tableNames);
		return this->tableNames->GetCount();
	}
	else
	{
		UTF8Char sbuff[256];
		DB::DBReader *rdr = this->ExecuteReaderC(UTF8STRC("show tables"));
		NEW_CLASS(this->tableNames, Data::ArrayList<const UTF8Char*>());
		if (rdr)
		{
			while (rdr->ReadNext())
			{
				rdr->GetStr(0, sbuff, sizeof(sbuff));
				this->tableNames->Add(Text::StrCopyNew(sbuff));
			}
			this->CloseReader(rdr);
		}
		names->AddAll(this->tableNames);
		return this->tableNames->GetCount();
	}
}

DB::DBReader *Net::MySQLTCPClient::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	sb.Append((const UTF8Char*)"select ");
	if (columnNames == 0 || columnNames->GetCount() == 0)
	{
		sb.Append((const UTF8Char*)"*");
	}
	else
	{
		i = 0;
		j = columnNames->GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb.AppendChar(',', 1);
			}
			DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i), DB::DBUtil::ServerType::MySQL);
			sb.Append(sbuff);
			i++;
		}
	}
	sb.Append((const UTF8Char*)" from ");
	DB::DBUtil::SDBColUTF8(sbuff, tableName, DB::DBUtil::ServerType::MySQL);
	sb.Append(sbuff);
	if (maxCnt > 0)
	{
		sb.Append((const UTF8Char*)" LIMIT ");
		sb.AppendUOSInt(maxCnt);
	}
	return this->ExecuteReaderC(sb.ToString(), sb.GetLength());
}

Bool Net::MySQLTCPClient::ChangeSchema(const UTF8Char *schemaName)
{
	UTF8Char sbuff[128];
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"use ");
	UOSInt colLen = DB::DBUtil::SDBColUTF8Leng(schemaName, DB::DBUtil::ServerType::MySQL);
	if (colLen > 127)
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, colLen + 1);
		DB::DBUtil::SDBColUTF8(sptr, schemaName, DB::DBUtil::ServerType::MySQL);
		sb.Append(sptr);
		MemFree(sptr);
	}
	else
	{
		DB::DBUtil::SDBColUTF8(sbuff, schemaName, DB::DBUtil::ServerType::MySQL);
		sb.Append(sbuff);
	}
	if (this->ExecuteNonQueryC(sb.ToString(), sb.GetLength()) >= 0)
	{
		if (this->tableNames)
		{
			LIST_FREE_FUNC(this->tableNames, Text::StrDelNew);
			DEL_CLASS(this->tableNames);
			this->tableNames = 0;
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::MySQLTCPClient::IsError()
{
	return this->cli == 0 || !this->recvRunning;
}

Bool Net::MySQLTCPClient::ServerInfoRecv()
{
	return this->mode >= 1;
}

Text::String *Net::MySQLTCPClient::GetServerVer()
{
	return this->svrVer;
}

UInt32 Net::MySQLTCPClient::GetConnId()
{
	return this->connId;
}

UOSInt Net::MySQLTCPClient::GetAuthPluginData(UInt8 *buff)
{
	MemCopyNO(buff, this->authPluginData, this->authPluginDataSize);
	return this->authPluginDataSize;
}

UInt32 Net::MySQLTCPClient::GetServerCap()
{
	return this->svrCap;
}

UInt16 Net::MySQLTCPClient::GetServerCS()
{
	return this->svrCS;
}

const Net::SocketUtil::AddressInfo *Net::MySQLTCPClient::GetConnAddr()
{
	return &this->addr;
}

UInt16 Net::MySQLTCPClient::GetConnPort()
{
	return this->port;
}

Text::String *Net::MySQLTCPClient::GetConnDB()
{
	return this->database;
}

Text::String *Net::MySQLTCPClient::GetConnUID()
{
	return this->userName;
}

Text::String *Net::MySQLTCPClient::GetConnPWD()
{
	return this->password;
}

UInt16 Net::MySQLTCPClient::GetDefaultPort()
{
	return 3306;
}

DB::DBTool *Net::MySQLTCPClient::CreateDBTool(Net::SocketFactory *sockf, Text::String *serverName, Text::String *dbName, Text::String *uid, Text::String *pwd, IO::LogTool *log, const UTF8Char *logPrefix)
{
	Net::MySQLTCPClient *conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (sockf->DNSResolveIP(serverName->v, &addr))
	{
		NEW_CLASS(conn, Net::MySQLTCPClient(sockf, &addr, 3306, uid, pwd, dbName));
		if (conn->IsError() == 0)
		{
			NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
			return db;
		}
		else
		{
			DEL_CLASS(conn);
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

DB::DBTool *Net::MySQLTCPClient::CreateDBTool(Net::SocketFactory *sockf, const UTF8Char *serverName, const UTF8Char *dbName, const UTF8Char *uid, const UTF8Char *pwd, IO::LogTool *log, const UTF8Char *logPrefix)
{
	Net::MySQLTCPClient *conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (sockf->DNSResolveIP(serverName, &addr))
	{
		NEW_CLASS(conn, Net::MySQLTCPClient(sockf, &addr, 3306, uid, pwd, dbName));
		if (conn->IsError() == 0)
		{
			NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
			return db;
		}
		else
		{
			DEL_CLASS(conn);
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
