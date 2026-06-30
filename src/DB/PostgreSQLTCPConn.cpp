#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/DBTool.h"
#include "DB/PostgreSQLTCPConn.h"
#include "Net/SocketUtil.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

UIntOS DB::PostgreSQLTCPConn::ReadPacket(NN<Net::TCPClient> cli, UnsafeArray<UInt8> buff, UIntOS buffSize)
{		
	UIntOS totalRead = 0;
	while (totalRead < buffSize)
	{
		UIntOS readSize = cli->Read(Data::ByteArray(buff.Ptr() + totalRead, buffSize - totalRead));
		if (readSize == 0)
		{
			return totalRead;
		}
		totalRead += readSize;
	}
	return totalRead;
}

Bool DB::PostgreSQLTCPConn::SendPacket(NN<Net::TCPClient> cli, UInt8 msgType, UnsafeArray<UInt8> data, UIntOS dataLen)
{
	UInt32 packetLen = (UInt32)(dataLen + 4);
	UInt8 packet[512];
	packet[0] = msgType;
	WriteMInt32(packet + 1, packetLen);
	if (dataLen > 0)
	{
		MemCopyO(packet + 5, data.Ptr(), dataLen);
	}
	
	UIntOS written = cli->Write(Data::ByteArray(packet, packetLen));
	return written == packetLen;
}

Bool DB::PostgreSQLTCPConn::SendStartupPacket(NN<Net::TCPClient> cli, Text::CString user, Text::CStringNN database)
{
	UInt8 packet[1024];
	UnsafeArray<UInt8> p = packet + 8;
	
	WriteMInt32(&p[0], 80877103);
	p += 4;
	
	Text::CStringNN nnuser;
	if (user.SetTo(nnuser))
	{
		p = CSTR("user").ConcatTo(p) + 1;
		p = nnuser.ConcatTo(p) + 1;
	}
	
	p = CSTR("database").ConcatTo(p) + 1;
	p = database.ConcatTo(p) + 1;
	*p++ = 0;
	
	UInt32 packetLen = (UInt32)(p - packet);
	WriteMInt32(packet, packetLen);
	
	return this->SendPacket(cli, 0, packet, packetLen);
}

Bool DB::PostgreSQLTCPConn::ParseAuthentication(NN<Net::TCPClient> cli)
{
	UInt8 buff[8];
	if (this->ReadPacket(cli, buff, 8) != 8)
	{
		return false;
	}
	
	UInt32 len = ReadMUInt32(buff + 4);
	if (len < 8)
	{
		return false;
	}
	
	Int32 authType = ReadMInt32(buff);
	switch (authType)
	{
		case 0:
			return true;
		case 5:
			if (len < 12)
			{
				return false;
			}
			UInt8 salt[4];
			if (this->ReadPacket(cli, salt, 4) != 4)
			{
				return false;
			}
			break;
		default:
			return false;
	}
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseBackendKeyData(NN<Net::TCPClient> cli)
{
	UInt8 buff[12];
	if (this->ReadPacket(cli, buff, 12) != 12)
	{
		return false;
	}
	
	this->backendPID = ReadMUInt32(buff + 4);
	this->cancelKey = ReadMInt32(buff + 8);
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseRowDescription(NN<Net::TCPClient> cli, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods)
{
	UInt8 buff[4];
	if (this->ReadPacket(cli, buff, 4) != 4)
	{
		return false;
	}
	
	UInt32 len = ReadMUInt32(buff);
	if (len < 4)
	{
		return false;
	}
	
	UInt8 cntBuff[2];
	if (this->ReadPacket(cli, cntBuff, 2) != 2)
	{
		return false;
	}
	
	Int16 colCount = ReadMInt16(cntBuff);
	for (Int16 i = 0; i < colCount; i++)
	{
		UTF8Char nameBuff[256];
		UnsafeArray<UTF8Char> p = nameBuff;
		while (true)
		{
			UInt8 c;
			if (this->ReadPacket(cli, &c, 1) != 1)
			{
				return false;
			}
			if (c == 0)
			{
				break;
			}
			*p++ = (UTF8Char)c;
		}
		colNames->Add(Text::String::New(nameBuff, (UIntOS)(p - nameBuff)));
		
		UInt8 typeInfo[12];
		if (this->ReadPacket(cli, typeInfo, 12) != 12)
		{
			return false;
		}
		types->Add(ReadMUInt32(typeInfo));
		typeMods->Add(ReadMInt32(typeInfo + 4));
		
		UInt16 fmtCode = ReadMUInt16(typeInfo + 8);
		if (fmtCode != 0)
		{
			return false;
		}
	}
	
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseDataRow(NN<Net::TCPClient> cli, UIntOS colCount, NN<Data::ArrayListObj<UnsafeArrayOpt<UInt8>>> values, NN<Data::ArrayListNative<UInt32>> lengths)
{
	UInt8 buff[4];
	if (this->ReadPacket(cli, buff, 4) != 4)
	{
		return false;
	}
	
	for (UIntOS i = 0; i < colCount; i++)
	{
		Int32 valLen;
		if (this->ReadPacket(cli, buff, 4) != 4)
		{
			return false;
		}
		
		valLen = ReadMInt32(buff);
		if (valLen < 0)
		{
			values->Add(nullptr);
			lengths->Add(0xFFFFFFFFu);
		}
		else if (valLen > 0)
		{
			UnsafeArray<UInt8> val = MemAllocArr(UInt8, (UIntOS)valLen);
			if (this->ReadPacket(cli, val, (UIntOS)valLen) != (UIntOS)valLen)
			{
				MemFreeArr(val);
				return false;
			}
			values->Add(val);
			lengths->Add((UInt32)valLen);
		}
		else
		{
			values->Add(nullptr);
			lengths->Add(0);
		}
	}
	
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseCommandComplete(NN<Net::TCPClient> cli, OutParam<IntOS> rowChanged)
{
	rowChanged.Set(0);
	
	UInt8 buff[4];
	if (this->ReadPacket(cli, buff, 4) != 4)
	{
		return false;
	}
	
	UInt32 len = ReadMUInt32(buff);
	if (len < 5)
	{
		return false;
	}
	
	UIntOS readSize = len - 4;
	if (this->ReadPacket(cli, buff, readSize) != readSize)
	{
		return false;
	}
	
	Text::StringBuilderUTF8 sb;
	sb.AppendSlow((const UTF8Char*)buff);
	
	if (sb.EndsWith(CSTR("1")))
	{
		rowChanged.Set(1);
	}
	else
	{
		Int32 rowCnt = 0;
		UIntOS len = sb.GetLength();
		for (UIntOS i = 0; i < len; i++)
		{
			if (sb.v[i] >= '0' && sb.v[i] <= '9')
			{
				rowCnt = rowCnt * 10 + (sb.v[i] - '0');
			}
		}
		rowChanged.Set(rowCnt);
	}
	
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseErrorResponse(NN<Net::TCPClient> cli, NN<Text::StringBuilderUTF8> errMsg)
{
	errMsg->ClearStr();
	
	UInt8 buff[4];
	if (this->ReadPacket(cli, buff, 4) != 4)
	{
		return false;
	}
	
	UInt32 len = ReadMUInt32(buff);
	if (len < 5)
	{
		return false;
	}
	
	UIntOS readSize = len - 4;
	UnsafeArray<UInt8> data = MemAllocArr(UInt8, readSize);
	if (this->ReadPacket(cli, data, readSize) != readSize)
	{
		MemFreeArr(data);
		return false;
	}
	
	UnsafeArray<UTF8Char> p = (UnsafeArray<UTF8Char>)data.Ptr();
	while (*p != 0)
	{
		switch (*p)
		{
			case 'M':
				p++;
				errMsg->AppendSlow(UnsafeArray<const UTF8Char>(p));
				break;
			case 'S':
				p++;
				if (errMsg->GetLength() > 0)
				{
					errMsg->AppendC(UTF8STRC("\n"));
				}
				errMsg->AppendSlow(UnsafeArray<const UTF8Char>(p));
				break;
		}
		while (*p != 0) p++;
		p++;
	}
	
	MemFreeArr(data);
	return errMsg->GetLength() > 0;
}

Bool DB::PostgreSQLTCPConn::Connect()
{
	if (this->connCli.NotNull())
	{
		return true;
	}
	
	NN<Net::TCPClient> cli;
	Net::SocketUtil::AddressInfo addr;
	if (!this->clif->GetSocketFactory()->DNSResolveIP(server->ToCString(), addr))
	{
		if (log->HasHandler())
		{
			log->LogMessage(CSTR("Failed to resolve PostgreSQL server address"), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	cli = this->clif->Create(addr, this->port, 60000);
	if (cli->IsConnectError())
	{
		cli.Delete();
		if (log->HasHandler())
		{
			log->LogMessage(CSTR("Failed to connect to PostgreSQL server"), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	
	this->connCli = cli;
	
	if (!this->SendStartupPacket(cli, OPTSTR_CSTR(this->uid), database->ToCString()))
	{
		this->Close();
		if (log->HasHandler())
		{
			log->LogMessage(CSTR("Failed to send startup packet"), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	
	if (!this->ParseAuthentication(cli))
	{
		this->Close();
		if (log->HasHandler())
		{
			log->LogMessage(CSTR("Authentication failed"), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	
	if (!this->ParseBackendKeyData(cli))
	{
		this->Close();
		if (log->HasHandler())
		{
			log->LogMessage(CSTR("Failed to parse backend key data"), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	
	isTran = false;
	
	if (log->HasHandler())
	{
		log->LogMessage(CSTR("PostgreSQL DB Connected"), IO::LogHandler::LogLevel::Raw);
	}
	
	return true;
}

void DB::PostgreSQLTCPConn::InitConnection()
{
	NN<DB::DBReader> r;
	if (ExecuteReader(CSTR("select now()")).SetTo(r))
	{
		if (r->ReadNext())
		{
			tzQhr = r->GetTimestamp(0).GetTimeZoneQHR();
		}
		CloseReader(r);
	}
}

DB::PostgreSQLTCPConn::PostgreSQLTCPConn(NN<Net::TCPClientFactory> clif, NN<Text::String> server, UInt16 port, Optional<Text::String> uid, Optional<Text::String> pwd, NN<Text::String> database, NN<IO::LogTool> log) : DBConn(CSTR("PostgreSQL"))
{
	this->clif = clif;
	this->server = server->Clone();
	this->port = port;
	this->database = database->Clone();
	this->uid = Text::String::CopyOrNull(uid);
	this->pwd = Text::String::CopyOrNull(pwd);
	this->log = log;
	this->tzQhr = 0;
	
	if (Connect())
	{
		InitConnection();
	}
}

DB::PostgreSQLTCPConn::PostgreSQLTCPConn(NN<Net::TCPClientFactory> clif, Text::CStringNN server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CStringNN database, NN<IO::LogTool> log) : DBConn(CSTR("PostgreSQL"))
{
	this->clif = clif;
	this->server = Text::String::New(server);
	this->port = port;
	this->database = Text::String::New(database);
	this->uid = Text::String::NewOrNull(uid);
	this->pwd = Text::String::NewOrNull(pwd);
	this->log = log;
	this->tzQhr = 0;
	
	if (this->Connect())
	{
		this->InitConnection();
	}
}

DB::PostgreSQLTCPConn::~PostgreSQLTCPConn()
{
	this->Close();
	this->server->Release();
	this->database->Release();
	OPTSTR_DEL(this->uid);
	OPTSTR_DEL(this->pwd);
}

DB::SQLType DB::PostgreSQLTCPConn::GetSQLType() const
{
	return DB::SQLType::PostgreSQL;
}

DB::DBConn::ConnType DB::PostgreSQLTCPConn::GetConnType() const
{
	return ConnType::PostgreSQLTCP;
}

void DB::PostgreSQLTCPConn::GetConnName(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("PostgreSQL"));
	sb->AppendC(UTF8STRC(" - "));
	sb->Append(database);
}

void DB::PostgreSQLTCPConn::Close()
{
	if (this->connCli.NotNull())
	{
		this->connCli.Delete();
		this->connCli = nullptr;
		
		if (this->log->HasHandler())
		{
			this->log->LogMessage(CSTR("PostgreSQL DB Disconnected"), IO::LogHandler::LogLevel::Raw);
		}
	}
}

void DB::PostgreSQLTCPConn::Dispose()
{
	DEL_CLASS(this);
}

	IntOS DB::PostgreSQLTCPConn::ExecuteNonQuery(Text::CStringNN sql)
{
	this->lastDataError = DataError::NoError;
	NN<Net::TCPClient> cli;
	if (!this->connCli.SetTo(cli))
	{
		return -2;
	}
	
	UInt32 sqlLen = (UInt32)sql.leng;
	UnsafeArray<UInt8> packet = MemAllocArr(UInt8, 6 + sqlLen);
	packet[0] = 'Q';
	WriteMInt32(&packet[1], 6 + sqlLen);
	packet[5] = '\0';  // unnamed statement
	sql.ConcatTo(&packet[6]);
	
	if (!this->SendPacket(cli, 'Q', packet, 6 + sqlLen))
	{
		MemFreeArr(packet);
		return -2;
	}
	MemFreeArr(packet);
	
	IntOS rowChanged = 0;
	while (true)
	{
		UInt8 buff[5];
		if (this->ReadPacket(cli, buff, 5) != 5)
		{
			return -2;
		}
		
		switch (buff[0])
		{
			case 'C':
				if (!this->ParseCommandComplete(cli, rowChanged))
				{
					return -2;
				}
				break;
			case 'Z':
				WriteMInt32(buff + 4, ReadMInt32(buff + 4));
				return rowChanged;
			case 'E':
				{
					Text::StringBuilderUTF8 errMsg;
					if (this->ParseErrorResponse(cli, errMsg))
					{
						this->lastDataError = DataError::ExecSQLError;
						if (this->log->HasHandler())
						{
							this->log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
						}
					}
				}
				return -2;
			case 'T':
				break;
			case 'D':
				break;
		}
	}
	
	return rowChanged;
}

	Optional<DB::DBReader> DB::PostgreSQLTCPConn::ExecuteReader(Text::CStringNN sql)
{
	this->lastDataError = DataError::NoError;
	NN<Net::TCPClient> cli;
	if (!this->connCli.SetTo(cli))
	{
		return nullptr;
	}
	
	UInt32 sqlLen = (UInt32)sql.leng;
	UnsafeArray<UInt8> packet = MemAllocArr(UInt8, 6 + sqlLen);
	packet[0] = 'Q';
	WriteMInt32(&packet[1], 6 + sqlLen);
	packet[5] = '\0';  // unnamed statement
	sql.ConcatTo(&packet[6]);
	
	if (!this->SendPacket(cli, 'Q', packet, 6 + sqlLen))
	{
		MemFreeArr(packet);
		return nullptr;
	}
	MemFreeArr(packet);
	
	Data::ArrayListStringNN colNames;
	Data::ArrayListObj<UnsafeArrayOpt<UInt8>> values;
	Data::ArrayListNative<UInt32> lengths;
	Data::ArrayListNative<UInt32> types;
	Data::ArrayListNative<Int32> typeMods;
	UnsafeArray<UInt8> val;
	
	IntOS rowChanged = 0;
	Bool hasResult = false;
	while (true)
	{
		UInt8 buff[5];
		if (this->ReadPacket(cli, buff, 5) != 5)
		{
			return nullptr;
		}
		
		switch (buff[0])
		{
			case 'C':
				if (!this->ParseCommandComplete(cli, rowChanged))
				{
					return nullptr;
				}
				break;
			case 'T':
				hasResult = true;
				if (!this->ParseRowDescription(cli, colNames, types, typeMods))
				{
					return nullptr;
				}
				break;
			case 'D':
				if (!hasResult)
				{
					return nullptr;
				}
				if (!this->ParseDataRow(cli, (UIntOS)colNames.GetCount(), values, lengths))
				{
					Data::ArrayIterator<UnsafeArrayOpt<UInt8>> it = values.Iterator();
					while (it.HasNext())
					{
						if (it.Next().SetTo(val))
						{
							MemFreeArr(val);
						}
					}
					return nullptr;
				}
				break;
			case 'Z':
				WriteMInt32(buff + 4, ReadMInt32(buff + 4));
				goto done_reading;
			case 'E':
				{
					Text::StringBuilderUTF8 errMsg;
					if (this->ParseErrorResponse(cli, errMsg))
					{
						this->lastDataError = DataError::ExecSQLError;
						if (this->log->HasHandler())
						{
							this->log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
						}
					}
				}
				return nullptr;
			default:
				break;
		}
	}
	
done_reading:
	
	if (!hasResult)
	{
		Data::ArrayListStringNN emptyNames;
		Data::ArrayListObj<UnsafeArrayOpt<UInt8>> emptyValues;
		Data::ArrayListNative<UInt32> emptyLengths;
		Data::ArrayListNative<UInt32> emptyTypes;
		Data::ArrayListNative<Int32> emptyTypeMods;
		
		return NEW_CLASS_D(PostgreSQLTCPReader(backendPID, cancelKey, emptyNames, emptyValues, emptyLengths, emptyTypes, emptyTypeMods));
	}
	
	return NEW_CLASS_D(PostgreSQLTCPReader(backendPID, cancelKey, colNames, values, lengths, types, typeMods));
}

void DB::PostgreSQLTCPConn::CloseReader(NN<DB::DBReader> r)
{
	NN<PostgreSQLTCPReader> reader = NN<PostgreSQLTCPReader>::ConvertFrom(r);
	reader.Delete();
}

void DB::PostgreSQLTCPConn::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	str->Append(CSTR("Error occurred during query execution"));
}

Bool DB::PostgreSQLTCPConn::IsLastDataError()
{
	return this->lastDataError == DataError::ExecSQLError;
}

void DB::PostgreSQLTCPConn::Reconnect()
{
	this->Close();
	if (this->Connect())
	{
		this->InitConnection();
	}
}

Int8 DB::PostgreSQLTCPConn::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::PostgreSQLTCPConn::ForceTzQhr(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

Optional<DB::DBTransaction> DB::PostgreSQLTCPConn::BeginTransaction()
{
	if (this->isTran)
	{
		return nullptr;
	}
	if (this->ExecuteNonQuery(CSTR("BEGIN")) < 0)
	{
		return nullptr;
	}
	this->isTran = true;
	return (DB::DBTransaction*)-1;
}

void DB::PostgreSQLTCPConn::Commit(NN<DB::DBTransaction> tran)
{
	if (this->isTran && this->ExecuteNonQuery(CSTR("COMMIT")) >= 0)
	{
		this->isTran = false;
	}
	else if (this->isTran)
	{
		this->Reconnect();
	}
}

void DB::PostgreSQLTCPConn::Rollback(NN<DB::DBTransaction> tran)
{
	if (this->isTran && this->ExecuteNonQuery(CSTR("ROLLBACK")) >= 0)
	{
		this->isTran = false;
	}
	else if (this->isTran)
	{
		this->Reconnect();
	}
}

UIntOS DB::PostgreSQLTCPConn::QuerySchemaNames(NN<Data::ArrayListStringNN> names)
{
	UIntOS initCnt = names->GetCount();
	NN<DB::DBReader> r;
	if (this->ExecuteReader(CSTR("SELECT nspname FROM pg_catalog.pg_namespace")).SetTo(r))
	{
		while (r->ReadNext())
		{
			names->Add(r->GetNewStrNN(0));
		}
		this->CloseReader(r);
	}
	return names->GetCount() - initCnt;
}

UIntOS DB::PostgreSQLTCPConn::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	Text::CStringNN nnschemaName;
	if (!schemaName.SetTo(nnschemaName) || nnschemaName.leng == 0)
	{
		nnschemaName = CSTR("public");
	}
	
	DB::SQLBuilder sql(DB::SQLType::PostgreSQL, false, this->tzQhr);
	sql.AppendCmdC(CSTR("select tablename from pg_catalog.pg_tables where schemaname = "));
	sql.AppendStrC(nnschemaName);
	
	UIntOS initCnt = names->GetCount();
	NN<DB::DBReader> r;
	if (this->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			names->Add(r->GetNewStrNN(0));
		}
		CloseReader(r);
	}
	
	return names->GetCount() - initCnt;
}

Optional<DB::DBReader> DB::PostgreSQLTCPConn::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	DB::SQLBuilder sql(DB::SQLType::PostgreSQL, false, this->tzQhr);
	sql.AppendCmdC(CSTR("select "));
	
	Text::CStringNN s;
	NN<Data::ArrayListStringNN> nncolumnNames;
	if (!columnNames.SetTo(nncolumnNames) || nncolumnNames->GetCount() == 0)
	{
		sql.AppendCmdC(CSTR("*"));
	}
	else
	{
		Data::ArrayIterator<NN<Text::String>> it = nncolumnNames->Iterator();
		Bool found = false;
		while (it.HasNext())
		{
			if (found)
			{
				sql.AppendCmdC(CSTR(","));
			}
			sql.AppendCol(it.Next()->v);
			found = true;
		}
	}
	
	sql.AppendCmdC(CSTR(" from "));
	if (schemaName.SetTo(s) && s.leng > 0)
	{
		sql.AppendCol(s.v);
		sql.AppendCmdC(CSTR("."));
	}
	sql.AppendCol(tableName.v);
	
	NN<Data::QueryConditions> nncondition;
	if (condition.SetTo(nncondition))
	{
		Data::ArrayListNN<Data::Conditions::BooleanObject> cliCond;
		sql.AppendCmdC(CSTR(" where "));
		nncondition->ToWhereClause(sql.GetStringBuilder(), DB::SQLType::PostgreSQL, 0, 100, cliCond);
	}
	
	if (ordering.SetTo(s) && s.leng > 0)
	{
		sql.AppendCmdC(CSTR(" order by "));
		sql.AppendCmdC(s);
	}
	
	if (maxCnt > 0)
	{
		sql.AppendCmdC(CSTR(" LIMIT "));
		sql.AppendInt32((Int32)maxCnt);
	}
	
	if (ofst > 0)
	{
		sql.AppendCmdC(CSTR(" OFFSET "));
		sql.AppendInt32((Int32)ofst);
	}
	
	return this->ExecuteReader(sql.ToCString());
}

Bool DB::PostgreSQLTCPConn::IsConnError()
{
	return this->connCli.IsNull();
}

NN<Text::String> DB::PostgreSQLTCPConn::GetConnServer() const
{
	return this->server;
}

UInt16 DB::PostgreSQLTCPConn::GetConnPort() const
{
	return this->port;
}

NN<Text::String> DB::PostgreSQLTCPConn::GetConnDB() const
{
	return this->database;
}

Optional<Text::String> DB::PostgreSQLTCPConn::GetConnUID() const
{
	return this->uid;
}

Optional<Text::String> DB::PostgreSQLTCPConn::GetConnPWD() const
{
	return this->pwd;
}

Bool DB::PostgreSQLTCPConn::ChangeDatabase(Text::CStringNN databaseName)
{
	NN<Text::String> oldDB = this->database;
	this->database = Text::String::New(databaseName);
	this->Reconnect();
	
	if (this->connCli.NotNull())
	{
		oldDB->Release();
		return true;
	}
	else
	{
		this->database->Release();
		this->database = oldDB;
		return false;
	}
}

UInt32 DB::PostgreSQLTCPConn::GetBackendPID() const
{
	return this->backendPID;
}

Int32 DB::PostgreSQLTCPConn::GetCancelKey() const
{
	return this->cancelKey;
}

Optional<DB::DBTool> DB::PostgreSQLTCPConn::CreateDBTool(NN<Net::TCPClientFactory> clif, NN<Text::String> serverName, UInt16 port, NN<Text::String> dbName, Optional<Text::String> uid, Optional<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<PostgreSQLTCPConn> conn;
	NEW_CLASSNN(conn, PostgreSQLTCPConn(clif, serverName, port, uid, pwd, dbName, log));
	
	if (conn->IsConnError())
	{
		conn.Delete();
		return nullptr;
	}
	
	NN<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
	return db;
}

Optional<DB::DBTool> DB::PostgreSQLTCPConn::CreateDBTool(NN<Net::TCPClientFactory> clif, Text::CStringNN serverName, UInt16 port, Text::CStringNN dbName, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<PostgreSQLTCPConn> conn;
	NEW_CLASSNN(conn, PostgreSQLTCPConn(clif, serverName, port, uid, pwd, dbName, log));
	
	if (conn->IsConnError())
	{
		conn.Delete();
		return nullptr;
	}
	
	NN<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
	return db;
}

DB::PostgreSQLTCPReader::PostgreSQLTCPReader(UInt32 backendPID, Int32 cancelKey, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListObj<UnsafeArrayOpt<UInt8>>> values, NN<Data::ArrayListNative<UInt32>> lengths, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods)
{
	this->backendPID = backendPID;
	this->cancelKey = cancelKey;
	currRow = (UIntOS)-1;
	
	Data::ArrayIterator<NN<Text::String>> nameIt = colNames->Iterator();
	while (nameIt.HasNext())
	{
		columnNames.Add(nameIt.Next()->Clone());
	}
	
	colCount = (UIntOS)columnNames.GetCount();
	rowCount = values->GetCount();
	
	Data::ArrayIterator<UnsafeArrayOpt<UInt8>> valIt = values->Iterator();
	Data::ArrayIterator<UInt32> lenIt = lengths->Iterator();
	while (valIt.HasNext())
	{
		rowValues.Add(valIt.Next());
	}
	while (lenIt.HasNext())
	{
		valueLengths.Add(lenIt.Next());
	}
	
	Data::ArrayIterator<UInt32> typeIt = types->Iterator();
	while (typeIt.HasNext())
	{
		columnTypes.Add(typeIt.Next());
	}
	Data::ArrayIterator<Int32> modIt = typeMods->Iterator();
	while (modIt.HasNext())
	{
		columnTypeMods.Add(modIt.Next());
	}
}

DB::PostgreSQLTCPReader::~PostgreSQLTCPReader()
{
	UnsafeArray<UInt8> rowVal;
	Data::ArrayIterator<UnsafeArrayOpt<UInt8>> it = this->rowValues.Iterator();
	while (it.HasNext())
	{
		if (it.Next().SetTo(rowVal))
		{
			MemFreeArr(rowVal);
		}
	}
	
	columnNames.FreeAll();
}

Bool DB::PostgreSQLTCPReader::ReadNext()
{
	currRow++;
	return currRow < rowCount;
}

UIntOS DB::PostgreSQLTCPReader::ColCount()
{
	return colCount;
}

IntOS DB::PostgreSQLTCPReader::GetRowChanged()
{
	return 0;
}

Int32 DB::PostgreSQLTCPReader::GetInt32(UIntOS colIndex)
{
	if (colIndex >= colCount || currRow >= rowCount)
	{
		return 0;
	}
	
	UnsafeArray<UInt8> val;
	UInt32 len = valueLengths.GetItem(colIndex + currRow * colCount);
	
	if (!rowValues.GetItem(colIndex + currRow * colCount).SetTo(val) || len != 4)
	{
		return 0;
	}
	
	return ReadMInt32(&val[0]);
}

Int64 DB::PostgreSQLTCPReader::GetInt64(UIntOS colIndex)
{
	if (colIndex >= colCount || currRow >= rowCount)
	{
		return 0;
	}
	
	UnsafeArray<UInt8> val;
	UInt32 len = valueLengths.GetItem(colIndex + currRow * colCount);
	if (!rowValues.GetItem(colIndex + currRow * colCount).SetTo(val) || len != 8)
	{
		return 0;
	}
	
	return ReadMInt64(&val[0]);
}

UnsafeArrayOpt<WChar> DB::PostgreSQLTCPReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return nullptr;
	}
	
	if (item.GetItemType() == Data::VariItem::ItemType::Null)
	{
		return nullptr;
	}
	
	Text::StringBuilderUTF8 sb;
	item.GetAsString(sb);
	
	return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
}

Bool DB::PostgreSQLTCPReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return false;
	}
	
	if (item.GetItemType() == Data::VariItem::ItemType::Null)
	{
		return false;
	}
	
	item.GetAsString(sb);
	return true;
}

Optional<Text::String> DB::PostgreSQLTCPReader::GetNewStr(UIntOS colIndex)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return nullptr;
	}
	
	return item.GetAsNewString();
}

UnsafeArrayOpt<UTF8Char> DB::PostgreSQLTCPReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return nullptr;
	}
	
	return item.GetAsStringS(buff, buffSize);
}

Data::Timestamp DB::PostgreSQLTCPReader::GetTimestamp(UIntOS colIndex)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return Data::Timestamp(nullptr);
	}
	
	if (item.GetItemType() == Data::VariItem::ItemType::Null)
	{
		return Data::Timestamp(nullptr);
	}
	
	return item.GetAsTimestamp();
}

Double DB::PostgreSQLTCPReader::GetDblOrNAN(UIntOS colIndex)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return 0.0;
	}
	
	return item.GetAsF64();
}

Bool DB::PostgreSQLTCPReader::GetBool(UIntOS colIndex)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return false;
	}
	
	return item.GetAsBool();
}

UIntOS DB::PostgreSQLTCPReader::GetBinarySize(UIntOS colIndex)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return 0;
	}
	
	NN<Data::ReadonlyArray<UInt8>> arr;
	if (item.GetAndRemoveByteArr().SetTo(arr))
	{
		UIntOS ret = arr->GetCount();
		arr.Delete();
		return ret;
	}
	return 0;
}

UIntOS DB::PostgreSQLTCPReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return 0;
	}
	
	NN<Data::ReadonlyArray<UInt8>> arr;
	if (item.GetAndRemoveByteArr().SetTo(arr))
	{
		UIntOS ret = arr->GetCount();
		MemCopyNO(buff.Ptr(), arr->GetArray().Ptr(), ret);
		arr.Delete();
		return ret;
	}
	return 0;
}

Optional<Math::Geometry::Vector2D> DB::PostgreSQLTCPReader::GetVector(UIntOS colIndex)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return nullptr;
	}
	
	return item.GetAndRemoveVector();
}

Bool DB::PostgreSQLTCPReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return false;
	}
	
	NN<Data::UUID> nnuuid;
	if (!item.GetAndRemoveUUID().SetTo(nnuuid))
	{
		return false;
	}
	uuid->SetValue(nnuuid);
	nnuuid.Delete();
	return true;
}

UnsafeArrayOpt<UTF8Char> DB::PostgreSQLTCPReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
{
	if (colIndex >= colCount)
	{
		return nullptr;
	}
	
	return columnNames.GetItemNoCheck(colIndex)->ConcatTo(buff);
}

Bool DB::PostgreSQLTCPReader::IsNull(UIntOS colIndex)
{
	Data::VariItem item;
	if (!GetVariItem(colIndex, item))
	{
		return false;
	}
	
	return item.GetItemType() == Data::VariItem::ItemType::Null;
}

DB::DBUtil::ColType DB::PostgreSQLTCPReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	if (colIndex >= colCount)
	{
		return DB::DBUtil::CT_Unknown;
	}
	
	UInt32 dbType = columnTypes.GetItem(colIndex);
	DB::DBUtil::ColType colType = DBType2ColType(dbType);
	
	if (colSize.IsNotNull())
	{
		Int32 typeMod = columnTypeMods.GetItem(colIndex);
		if (typeMod >= 0)
		{
			colSize.Set((UInt32)typeMod - 4);
		}
		else
		{
			colSize.Set(65535);
		}
	}
	
	return colType;
}

Bool DB::PostgreSQLTCPReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
{
	if (colIndex >= colCount)
	{
		return false;
	}
	
	colDef->SetColName(columnNames.GetItemNoCheck(colIndex));
	colDef->SetColType(DBType2ColType(columnTypes.GetItem(colIndex)));
	
	Int32 typeMod = columnTypeMods.GetItem(colIndex);
	if (typeMod >= 0)
	{
		colDef->SetColSize((UInt32)typeMod - 4);
	}
	else
	{
		colDef->SetColSize(65535);
	}
	
	return true;
}

DB::DBUtil::ColType DB::PostgreSQLTCPReader::DBType2ColType(UInt32 dbType)
{
	switch (dbType)
	{
	case 16:
		return DB::DBUtil::CT_Bool;
	case 17:
		return DB::DBUtil::CT_Binary;
	case 18:
		return DB::DBUtil::CT_UTF32Char;
	case 19:
		return DB::DBUtil::CT_VarUTF32Char;
	case 20:
		return DB::DBUtil::CT_Int64;
	case 21:
		return DB::DBUtil::CT_Int16;
	case 23:
		return DB::DBUtil::CT_Int32;
	case 25:
		return DB::DBUtil::CT_VarUTF32Char;
	case 1042:
		return DB::DBUtil::CT_UTF32Char;
	case 1043:
		return DB::DBUtil::CT_VarUTF32Char;
	case 1082:
		return DB::DBUtil::CT_Date;
	case 1114:
	case 1184:
		return DB::DBUtil::CT_DateTime;
	case 2950:
		return DB::DBUtil::CT_UUID;
	default:
		return DB::DBUtil::CT_Unknown;
	}
}

Bool DB::PostgreSQLTCPReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
{
	if (currRow < 0 || currRow >= rowCount || colIndex >= colCount)
	{
		return false;
	}
	
	UIntOS idx = colIndex + currRow * colCount;
	UnsafeArray<UInt8> val;
	if (!rowValues.GetItem(idx).SetTo(val))
	{
		item->SetNull();
		return true;
	}
	UInt32 len = valueLengths.GetItem(idx);
	UInt32 dbType = columnTypes.GetItem(colIndex);
	
	switch (dbType)
	{
	case 16:
		if (len >= 1 && val[0] == 't')
		{
			item->SetBool(true);
		}
		else
		{
			item->SetBool(false);
		}
		break;
	case 17:
		item->SetByteArr(val, len);
		break;
	case 18:
	case 19:
	case 25:
	case 1042:
	case 1043:
		item->SetStrCopy(UnsafeArray<const UInt8>(val), len);
		break;
	case 20:
		if (len == 8)
		{
			item->SetI64(ReadMInt64(&val[0]));
		}
		else
		{
			item->SetI64(0);
		}
		break;
	case 21:
		if (len == 2)
		{
			item->SetI16(ReadMInt16(&val[0]));
		}
		else if (len == 4)
		{
			item->SetI16((Int16)ReadMInt32(&val[0]));
		}
		else
		{
			item->SetI16(0);
		}
		break;
	case 23:
		if (len == 4)
		{
			item->SetI32(ReadMInt32(&val[0]));
		}
		else
		{
			item->SetI32(0);
		}
		break;
	case 700:
		if (len == 4)
		{
			item->SetF64(ReadMFloat(&val[0]));
		}
		else
		{
			item->SetF64(0.0);
		}
		break;
	case 701:
		if (len == 8)
		{
			item->SetF64(ReadMDouble(&val[0]));
		}
		else
		{
			item->SetF64(0.0);
		}
		break;
	case 1082:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(val, len);
			item->SetDate(Data::Date(sb.ToCString()));
		}
		break;
	case 1114:
	case 1184:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(val, len);
			item->SetDate(Data::Timestamp(sb.ToCString(), tzQhr));
		}
		break;
	case 2950:
		{
			NN<Data::UUID> uuid;
			NEW_CLASSNN(uuid, Data::UUID(Text::CStringNN(val, len)));
			item->SetUUIDDirect(uuid);
		}
		break;
	default:
		{
			item->SetStrCopy(UnsafeArray<const UInt8>(val), len);
		}
		break;
	}
	
	return true;
}

