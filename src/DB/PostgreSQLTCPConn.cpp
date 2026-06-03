#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/DBTool.h"
#include "DB/PostgreSQLTCPConn.h"
#include "Net/SocketUtil.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

namespace DB
{
	struct PostgreSQLTCPConn::ClassData
	{
		Optional<Net::TCPClient> connCli;
		UInt32 backendPID;
		Int32 cancelKey;
	};

	Int32 PostgreSQLTCPConn::readPacket(UnsafeArray<UInt8> buff, UIntOS buffSize)
	{		Optional<Net::TCPClient> cli;
		if (!clsData->connCli.SetTo(cli))
		{
			return -1;
		}
		NN<IO::Stream> s = cli->GetStream();
		Int32 totalRead = 0;
		while (totalRead < (Int32)buffSize)
		{
			UIntOS readSize = s->Read(Data::ByteArray(buff.Ptr() + totalRead, buffSize - totalRead));
			if (readSize == 0)
			{
				return -1;
			}
			totalRead += (Int32)readSize;
		}
		return totalRead;
	}

	Bool PostgreSQLTCPConn::sendPacket(UInt8 msgType, UnsafeArray<UInt8> data, UIntOS dataLen)
	{		Optional<Net::TCPClient> cli;
		if (!clsData->connCli.SetTo(cli))
		{
			return false;
		}
		NN<IO::Stream> s = cli->GetStream();
		
		UInt32 packetLen = dataLen + 4;
		UInt8 packet[512];
		packet[0] = msgType;
		WriteMUInt32(packet + 1, packetLen);
		if (dataLen > 0)
		{
			MemCopy(packet + 5, data, dataLen);
		}
		
		UIntOS written = s->Write(Data::ByteArray(packet, dataLen + 5));
		return written == dataLen + 5;
	}

	Bool PostgreSQLTCPConn::sendStartupPacket(Text::CStringNN user, Text::CStringNN database)
	{
		UInt8 packet[1024];
		UnsafeArray<UInt8> p = packet + 8;
		
		WriteMInt32(p.Ptr(), 80877103);
		p += 4;
		
		MemCopy(p.Ptr(), "user", 5);
		p += 5;
		MemCopy(p.Ptr(), user.v.Ptr(), user.leng + 1);
		p += user.leng + 1;
		
		MemCopy(p.Ptr(), "database", 9);
		p += 9;
		MemCopy(p.Ptr(), database.v.Ptr(), database.leng + 1);
		p += database.leng + 1;
		
		*p++ = 0;
		
		UInt32 packetLen = (UInt32)(p - packet);
		WriteMInt32(packet, packetLen);
		
		return this->sendPacket( 0, packet, packetLen);
	}

	Bool PostgreSQLTCPConn::parseAuthentication()
	{		UInt8 buff[8];
		if (this->readPacket( buff, 8) != 8)
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
				if (this->readPacket( salt, 4) != 4)
				{
					return false;
				}
				break;
			default:
				return false;
		}
		return true;
	}

	Bool PostgreSQLTCPConn::parseBackendKeyData()
	{		UInt8 buff[12];
		if (this->readPacket( buff, 12) != 12)
		{
			return false;
		}
		
		clsData->backendPID = ReadMUInt32(buff + 4);
		clsData->cancelKey = ReadMInt32(buff + 8);
		return true;
	}

	Bool PostgreSQLTCPConn::parseRowDescription(NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods)
	{
		UInt8 buff[4];
		if (this->readPacket( buff, 4) != 4)
		{
			return false;
		}
		
		UInt32 len = ReadMUInt32(buff);
		if (len < 4)
		{
			return false;
		}
		
		UInt8 cntBuff[2];
		if (this->readPacket( cntBuff, 2) != 2)
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
				if (this->readPacket( &c, 1) != 1)
				{
					return false;
				}
				if (c == 0)
				{
					break;
				}
				*p++ = Data::SetCharA(p, (*c));
			}
			colNames->Add(Text::String::New(nameBuff, (UIntOS)(p - nameBuff)));
			
			UInt8 typeInfo[12];
			if (this->readPacket( typeInfo, 12) != 12)
			{
				return false;
			}
			types->Add(ReadMUInt32(typeInfo));
			typeMods->Add(ReadMInt32(typeInfo + 4));
			
			UInt16 fmtCode = ReadMInt16(typeInfo + 8);
			if (fmtCode != 0)
			{
				return false;
			}
		}
		
		return true;
	}

	Bool PostgreSQLTCPConn::parseDataRow(UIntOS colCount, NN<Data::ArrayList<UnsafeArray<UInt8>>> values, NN<Data::ArrayList<UInt32>> lengths)
	{
		UInt8 buff[4];
		if (this->readPacket( buff, 4) != 4)
		{
			return false;
		}
		
		for (UIntOS i = 0; i < colCount; i++)
		{
			Int32 valLen;
			if (this->readPacket( (UnsafeArray<UInt8>)&valLen, 4) != 4)
			{
				return false;
			}
			
			valLen = ReadMInt32((UnsafeArray<UInt8>)&valLen);
			if (valLen < 0)
			{
				values->Add(nullptr);
				lengths->Add(0xFFFFFFFFu);
			}
			else if (valLen > 0)
			{
				UnsafeArray<UInt8> val = MemAllocArr(UInt8, valLen);
				if (this->readPacket( val, valLen) != valLen)
				{
					MemFree(val);
					return false;
				}
				values->Add(val);
				lengths->Add(valLen);
			}
			else
			{
				values->Add(nullptr);
				lengths->Add(0);
			}
		}
		
		return true;
	}

	Bool PostgreSQLTCPConn::parseCommandComplete(OutParam<IntOS> rowChanged)
	{
		rowChanged.SetNoCheck(0);
		
		UInt8 buff[4];
		if (this->readPacket( buff, 4) != 4)
		{
			return false;
		}
		
		UInt32 len = ReadMUInt32(buff);
		if (len < 5)
		{
			return false;
		}
		
		UTF8Char cmdBuff[256];
		UIntOS readSize = len - 4;
		if (this->readPacket( buff, readSize) != readSize)
		{
			return false;
		}
		
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)buff);
		
		if (sb.EndsWith(CSTR("1")))
		{
			rowChanged.SetNoCheck(1);
		}
		else
		{
			Int32 rowCnt = 0;
			UIntOS len = sb.GetLength();
			for (UIntOS i = 0; i < len; i++)
			{
				if (sb.GetChar(i) >= '0' && sb.GetChar(i) <= '9')
				{
					rowCnt = rowCnt * 10 + (sb.GetChar(i) - '0');
				}
			}
			rowChanged.SetNoCheck(rowCnt);
		}
		
		return true;
	}

	Bool PostgreSQLTCPConn::parseErrorResponse(NN<Text::StringBuilderUTF8> errMsg)
	{
		errMsg->Clear();
		
		UInt8 buff[4];
		if (this->readPacket( buff, 4) != 4)
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
		if (this->readPacket( data, readSize) != readSize)
		{
			MemFree(data);
			return false;
		}
		
		UnsafeArray<UTF8Char> p = (UnsafeArray<UTF8Char>)data.Ptr();
		while (*p != 0)
		{
			switch (Data::GetCharA(p))
			{
				case 'M':
					p++;
					errMsg->Append((const UTF8Char*)p);
					break;
				case 'S':
					p++;
					if (errMsg->GetLength() > 0)
					{
						errMsg->AppendC(UTF8STRC("\n"));
					}
					errMsg->Append((const UTF8Char*)p);
					break;
			}
			while (*p != 0) p++;
			p++;
		}
		
		MemFree(data);
		return errMsg->GetLength() > 0;
	}

	Bool PostgreSQLTCPConn::Connect()
	{
		if (clsData->connCli.IsNotNull())
		{
			return true;
		}
		
		Optional<Net::TCPClient> cli;
		cli = Net::SocketUtil::CreateTCPClient(server, port, nullptr);
		
		if (cli.IsNull())
		{
			if (log->HasHandler())
			{
				log->LogMessage(CSTR("Failed to connect to PostgreSQL server"), IO::LogHandler::LogLevel::Error);
			}
			return false;
		}
		
		clsData->connCli = cli;
		
		if (!sendStartupPacket(*this, uid->ToCString(), database->ToCString()))
		{
			Close();
			if (log->HasHandler())
			{
				log->LogMessage(CSTR("Failed to send startup packet"), IO::LogHandler::LogLevel::Error);
			}
			return false;
		}
		
		if (!parseAuthentication(*this))
		{
			Close();
			if (log->HasHandler())
			{
				log->LogMessage(CSTR("Authentication failed"), IO::LogHandler::LogLevel::Error);
			}
			return false;
		}
		
		if (!parseBackendKeyData(*this))
		{
			Close();
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

	void PostgreSQLTCPConn::InitConnection()
	{
		Optional<DB::DBReader> r;
		if (ExecuteReader(CSTR("select now()")).SetTo(r))
		{
			if (r->ReadNext())
			{
				tzQhr = r->GetTimestamp(0).GetTimeZoneQHR();
			}
			CloseReader(r);
		}
	}

	PostgreSQLTCPConn::PostgreSQLTCPConn(NN<Text::String> server, UInt16 port, Optional<Text::String> uid, Optional<Text::String> pwd, NN<Text::String> database, NN<IO::LogTool> log) : DBConn(CSTR("PostgreSQL"))
	{
		clsData = MemAllocNN(ClassData);
		
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

	PostgreSQLTCPConn::PostgreSQLTCPConn(Text::CStringNN server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CStringNN database, NN<IO::LogTool> log) : DBConn(CSTR("PostgreSQL"))
	{
		clsData = MemAllocNN(ClassData);
		
		this->server = Text::String::New(server);
		this->port = port;
		this->database = Text::String::New(database);
		this->uid = Text::String::NewOrNull(uid);
		this->pwd = Text::String::NewOrNull(pwd);
		this->log = log;
		this->tzQhr = 0;
		
		if (Connect())
		{
			InitConnection();
		}
	}

	PostgreSQLTCPConn::~PostgreSQLTCPConn()
	{
		Close();
		server->Release();
		database->Release();
		OPTSTR_DEL(uid);
		OPTSTR_DEL(pwd);
		MemFreeNN(clsData);
	}

	DB::SQLType PostgreSQLTCPConn::GetSQLType() const
	{
		return DB::SQLType::PostgreSQL;
	}

	DB::DBConn::ConnType PostgreSQLTCPConn::GetConnType() const
	{
		return ConnType::PostgreSQL;
	}

	void PostgreSQLTCPConn::GetConnName(NN<Text::StringBuilderUTF8> sb)
	{
		sb->AppendC(UTF8STRC("PostgreSQL"));
		sb->AppendC(UTF8STRC(" - "));
		sb->Append(database);
	}

	void PostgreSQLTCPConn::Close()
	{
		if (clsData->connCli.IsNotNull())
		{
			clsData->connCli.Delete();
			clsData->connCli = nullptr;
			
			if (log->HasHandler())
			{
				log->LogMessage(CSTR("PostgreSQL DB Disconnected"), IO::LogHandler::LogLevel::Raw);
			}
		}
	}

	void PostgreSQLTCPConn::Dispose()
	{
		DEL_CLASS(this);
	}

	IntOS PostgreSQLTCPConn::ExecuteNonQuery(Text::CStringNN sql)
	{
		lastDataError = false;
		
		if (clsData->connCli.IsNull())
		{
			return -2;
		}
		
		UInt8 stmtName = 0;
		UInt8 formatCodes = 0;
		
		UInt32 sqlLen = (UInt32)sql.v.GetLength();
		UInt8* packet = MemAllocArr(UInt8, 5 + 1 + sqlLen + 4);
		packet[0] = 'Q';
		WriteMInt32(packet + 1, sqlLen + 5);
		packet[5] = stmtName;
		MemCopy(packet + 6, sql.v.Ptr(), sqlLen);
		WriteMInt32(packet + 6 + sqlLen, formatCodes);
		
		if (!sendPacket(*this, 'Q', packet, 5 + 1 + sqlLen + 4))
		{
			MemFree(packet);
			return -2;
		}
		MemFree(packet);
		
		IntOS rowChanged = 0;
		while (true)
		{
			UInt8 buff[5];
			if (readPacket(*this, buff, 5) != 5)
			{
				return -2;
			}
			
			switch (Data::GetCharA(buff))
			{
				case 'C':
					if (!this->parseCommandComplete(, rowChanged))
					{
						return -2;
					}
					break;
				case 'Z':
					WriteMInt32(buff + 4, ReadMInt32(buff + 4));
					return rowChanged;
				case 'E':
					Text::StringBuilderUTF8 errMsg;
					if (this->parseErrorResponse( errMsg))
					{
						lastDataError = true;
						if (log->HasHandler())
						{
							log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
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

	Optional<DBReader> PostgreSQLTCPConn::ExecuteReader(Text::CStringNN sql)
	{
		lastDataError = false;
		
		if (clsData->connCli.IsNull())
		{
			return nullptr;
		}
		
		UInt8 stmtName = 0;
		UInt8 formatCodes = 0;
		
		UInt32 sqlLen = (UInt32)sql.v.GetLength();
		UInt8* packet = MemAllocArr(UInt8, 5 + 1 + sqlLen + 4);
		packet[0] = 'Q';
		WriteMInt32(packet + 1, sqlLen + 5);
		packet[5] = stmtName;
		MemCopy(packet + 6, sql.v.Ptr(), sqlLen);
		WriteMInt32(packet + 6 + sqlLen, formatCodes);
		
		if (!sendPacket(*this, 'Q', packet, 5 + 1 + sqlLen + 4))
		{
			MemFree(packet);
			return nullptr;
		}
		MemFree(packet);
		
		Data::ArrayListStringNN colNames;
		Data::ArrayList<UnsafeArray<UInt8>> values;
		Data::ArrayList<UInt32> lengths;
		Data::ArrayList<UInt32> types;
		Data::ArrayList<Int32> typeMods;
		
		IntOS rowChanged = 0;
		while (true)
		{
			UInt8 buff[5];
			if (readPacket(*this, buff, 5) != 5)
			{
				return nullptr;
			}
			
			switch (Data::GetCharA(buff))
			{
				case 'C':
					if (!this->parseCommandComplete(, rowChanged))
					{
						return nullptr;
					}
					break;
				case 'T':
					if (!this->parseRowDescription( colNames, types, typeMods))
					{
						return nullptr;
					}
					break;
				case 'D':
					if (!this->parseDataRow( (UIntOS)colNames.GetCount(), values, lengths))
					{
						return nullptr;
					}
					break;
				case 'Z':
					WriteMInt32(buff + 4, ReadMInt32(buff + 4));
					break;
				case 'E':
					Text::StringBuilderUTF8 errMsg;
					if (this->parseErrorResponse( errMsg))
					{
						lastDataError = true;
						if (log->HasHandler())
						{
							log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
						}
					}
					return nullptr;
				default:
					break;
			}
		}
		
		return NEW_CLASS_D(PostgreSQLTCPReader(clsData->backendPID, clsData->cancelKey, colNames, values, lengths, types, typeMods));
	}

	void PostgreSQLTCPConn::CloseReader(NN<DB::DBReader> r)
	{
		PostgreSQLTCPReader* reader = (PostgreSQLTCPReader*)r.Ptr();
		DEL_CLASS(reader);
	}

	void PostgreSQLTCPConn::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
	{
		str->Append(CSTR("Error occurred during query execution"));
	}

	Bool PostgreSQLTCPConn::IsLastDataError()
	{
		return lastDataError;
	}

	void PostgreSQLTCPConn::Reconnect()
	{
		Close();
		if (Connect())
		{
			InitConnection();
		}
	}

	Int8 PostgreSQLTCPConn::GetTzQhr() const
	{
		return tzQhr;
	}

	void PostgreSQLTCPConn::ForceTzQhr(Int8 tzQhr)
	{
		this->tzQhr = tzQhr;
	}

	Optional<DB::DBTransaction> PostgreSQLTCPConn::BeginTransaction()
	{
		if (isTran)
		{
			return nullptr;
		}
		ExecuteNonQuery(CSTR("BEGIN"));
		isTran = true;
		return (DB::DBTransaction*)-1;
	}

	void PostgreSQLTCPConn::Commit(NN<DB::DBTransaction> tran)
	{
		if (isTran)
		{
			ExecuteNonQuery(CSTR("COMMIT"));
			isTran = false;
		}
	}

	void PostgreSQLTCPConn::Rollback(NN<DB::DBTransaction> tran)
	{
		if (isTran)
		{
			ExecuteNonQuery(CSTR("ROLLBACK"));
			isTran = false;
		}
	}

	UIntOS PostgreSQLTCPConn::QuerySchemaNames(NN<Data::ArrayListStringNN> names)
	{
		UIntOS initCnt = names->GetCount();
		Optional<DB::DBReader> r;
		if (ExecuteReader(CSTR("SELECT nspname FROM pg_catalog.pg_namespace")).SetTo(r))
		{
			while (r->ReadNext())
			{
				names->Add(r->GetNewStrNN(0));
			}
			CloseReader(r);
		}
		return names->GetCount() - initCnt;
	}

	UIntOS PostgreSQLTCPConn::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
	{
		if (schemaName.leng == 0)
		{
			schemaName = CSTR("public");
		}
		
		Text::StringBuilderUTF8 sql;
		sql.AppendC(UTF8STRC("select tablename from pg_catalog.pg_tables where schemaname = "));
		sql.AppendStrC(schemaName);
		
		UIntOS initCnt = names->GetCount();
		Optional<DB::DBReader> r;
		if (ExecuteReader(sql.ToCString()).SetTo(r))
		{
			while (r->ReadNext())
			{
				names->Add(r->GetNewStrNN(0));
			}
			CloseReader(r);
		}
		
		return names->GetCount() - initCnt;
	}

	Optional<DBReader> PostgreSQLTCPConn::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
	{
		UTF8Char sbuff[512];
		Text::StringBuilderUTF8 sql;
		sql.AppendC(UTF8STRC("select "));
		
		if (columnNames.IsNull() || columnNames.v.GetCount() == 0)
		{
			sql.AppendC(UTF8STRC("*"));
		}
		else
		{
			Data::ArrayIterator<NN<Text::String>> it = columnNames.v.Iterator();
			Bool found = false;
			while (it.HasNext())
			{
				if (found)
				{
					sql.AppendUTF8Char(',');
				}
				DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, DB::SQLType::PostgreSQL);
				sql.AppendC(sbuff, (UIntOS)(DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, DB::SQLType::PostgreSQL) - sbuff));
				found = true;
			}
		}
		
		sql.AppendC(UTF8STRC(" from "));
		if (schemaName.leng > 0)
		{
			DB::DBUtil::SDBColUTF8(sbuff, schemaName.v, DB::SQLType::PostgreSQL);
			sql.AppendC(sbuff, (UIntOS)(DB::DBUtil::SDBColUTF8(sbuff, schemaName.v, DB::SQLType::PostgreSQL) - sbuff));
			sql.AppendUTF8Char('.');
		}
		DB::DBUtil::SDBColUTF8(sbuff, tableName.v, DB::SQLType::PostgreSQL);
		sql.AppendC(sbuff, (UIntOS)(DB::DBUtil::SDBColUTF8(sbuff, tableName.v, DB::SQLType::PostgreSQL) - sbuff));
		
		if (condition.IsNotNull())
		{
			Data::ArrayListNN<Data::Conditions::BooleanObject> cliCond;
			sql.AppendC(UTF8STRC(" where "));
			condition.v.ToWhereClause(sql, DB::SQLType::PostgreSQL, 0, 100, cliCond);
		}
		
		if (ordering.leng > 0)
		{
			sql.AppendC(UTF8STRC(" order by "));
			sql.Append(ordering);
		}
		
		if (maxCnt > 0)
		{
			sql.AppendC(UTF8STRC(" LIMIT "));
			sql.AppendUIntOS(maxCnt);
		}
		
		if (ofst > 0)
		{
			sql.AppendC(UTF8STRC(" OFFSET "));
			sql.AppendUIntOS(ofst);
		}
		
		return ExecuteReader(sql.ToCString());
	}

	Bool PostgreSQLTCPConn::IsConnError()
	{
		return clsData->connCli.IsNull();
	}

	NN<Text::String> PostgreSQLTCPConn::GetConnServer() const
	{
		return server;
	}

	UInt16 PostgreSQLTCPConn::GetConnPort() const
	{
		return port;
	}

	NN<Text::String> PostgreSQLTCPConn::GetConnDB() const
	{
		return database;
	}

	Optional<Text::String> PostgreSQLTCPConn::GetConnUID() const
	{
		return uid;
	}

	Optional<Text::String> PostgreSQLTCPConn::GetConnPWD() const
	{
		return pwd;
	}

	Bool PostgreSQLTCPConn::ChangeDatabase(Text::CStringNN databaseName)
	{
		NN<Text::String> oldDB = database;
		database = Text::String::New(databaseName);
		Reconnect();
		
		if (clsData->connCli.IsNotNull())
		{
			oldDB->Release();
			return true;
		}
		else
		{
			database->Release();
			database = oldDB;
			Reconnect();
			return false;
		}
	}

	UInt32 PostgreSQLTCPConn::GetBackendPID() const
	{
		return clsData->backendPID;
	}

	Int32 PostgreSQLTCPConn::GetCancelKey() const
	{
		return clsData->cancelKey;
	}

	Optional<DBTool> PostgreSQLTCPConn::CreateDBTool(NN<Net::TCPClientFactory> clif, NN<Text::String> serverName, UInt16 port, Optional<Text::String> dbName, NN<Text::String> uid, NN<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix)
	{
		Optional<PostgreSQLTCPConn> conn;
		NEW_CLASSNN(conn, PostgreSQLTCPConn(serverName, port, uid, pwd, dbName, log));
		
		if (conn->IsConnError())
		{
			conn.Delete();
			return nullptr;
		}
		
		Optional<DBTool> db;
		NEW_CLASSNN(db, DBTool(conn, true, log, logPrefix));
		return db;
	}

	Optional<DBTool> PostgreSQLTCPConn::CreateDBTool(NN<Net::TCPClientFactory> clif, Text::CStringNN serverName, UInt16 port, Text::CString dbName, Text::CStringNN uid, Text::CStringNN pwd, NN<IO::LogTool> log, Text::CString logPrefix)
	{
		Optional<PostgreSQLTCPConn> conn;
		NEW_CLASSNN(conn, PostgreSQLTCPConn(serverName, port, uid, pwd, dbName, log));
		
		if (conn->IsConnError())
		{
			conn.Delete();
			return nullptr;
		}
		
		Optional<DBTool> db;
		NEW_CLASSNN(db, DBTool(conn, true, log, logPrefix));
		return db;
	}

	PostgreSQLTCPReader::PostgreSQLTCPReader(UInt32 backendPID, Int32 cancelKey, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayList<UnsafeArray<UInt8>>> values, NN<Data::ArrayList<UInt32>> lengths, NN<Data::ArrayList<UInt32>> types, NN<Data::ArrayList<Int32>> typeMods)
	{
		this->backendPID = backendPID;
		this->cancelKey = cancelKey;
		currRow = (UIntOS)-1;
		
		Data::ArrayIterator<NN<Text::String>> nameIt = colNames.Iterator();
		while (nameIt.HasNext())
		{
			Text::String* s = nameIt.Next();
			UTF8Char* name = MemAllocArr(UTF8Char, s->GetLength() + 1);
			MemCopy(name, s->v.Ptr(), s->GetLength());
			name[s->GetLength()] = 0;
			columnNames.Add(name);
		}
		
		colCount = (UIntOS)columnNames.GetCount();
		rowCount = values.GetCount();
		
		Data::ArrayIterator<UnsafeArray<UInt8>> valIt = values.Iterator();
		Data::ArrayIterator<UInt32> lenIt = lengths.Iterator();
		while (valIt.HasNext())
		{
			rowValues.Add(valIt.Next());
		}
		while (lenIt.HasNext())
		{
			valueLengths.Add(lenIt.Next());
		}
		
		Data::ArrayIterator<UInt32> typeIt = types.Iterator();
		while (typeIt.HasNext())
		{
			columnTypes.Add(typeIt.Next());
		}
		Data::ArrayIterator<Int32> modIt = typeMods.Iterator();
		while (modIt.HasNext())
		{
			columnTypeMods.Add(modIt.Next());
		}
	}

	PostgreSQLTCPReader::~PostgreSQLTCPReader()
	{
		Data::ArrayList<UnsafeArray<UInt8>> values;
		rowValues.Swap(values);
		
		Data::ArrayIterator<UnsafeArray<UInt8>> it = values.Iterator();
		while (it.HasNext())
		{
			MemFree(it.Next());
		}
		
		Data::ArrayList<UTF8Char*> names;
		columnNames.Swap(names);
		
		Data::ArrayIterator<UTF8Char*> nameIt = names.Iterator();
		while (nameIt.HasNext())
		{
			MemFree(nameIt.Next());
		}
	}

	Bool PostgreSQLTCPReader::ReadNext()
	{
		currRow++;
		return currRow < rowCount;
	}

	UIntOS PostgreSQLTCPReader::ColCount()
	{
		return colCount;
	}

	IntOS PostgreSQLTCPReader::GetRowChanged()
	{
		return 0;
	}

	Int32 PostgreSQLTCPReader::GetInt32(UIntOS colIndex)
	{
		if (colIndex >= colCount || currRow >= rowCount)
		{
			return 0;
		}
		
		UInt8* val = rowValues.GetItem(colIndex + currRow * colCount);
		UInt32 len = valueLengths.GetItem(colIndex + currRow * colCount);
		
		if (len != 4)
		{
			return 0;
		}
		
		return ReadMInt32(val);
	}

	Int64 PostgreSQLTCPReader::GetInt64(UIntOS colIndex)
	{
		if (colIndex >= colCount || currRow >= rowCount)
		{
			return 0;
		}
		
		UInt8* val = rowValues.GetItem(colIndex + currRow * colCount);
		UInt32 len = valueLengths.GetItem(colIndex + currRow * colCount);
		
		if (len != 8)
		{
			return 0;
		}
		
		return Data::GetInt64BE(val);
	}

	UnsafeArrayOpt<WChar> PostgreSQLTCPReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
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

	Bool PostgreSQLTCPReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
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

	Optional<Text::String> PostgreSQLTCPReader::GetNewStr(UIntOS colIndex)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return nullptr;
		}
		
		return item.GetAsNewString();
	}

	UnsafeArrayOpt<UTF8Char> PostgreSQLTCPReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return nullptr;
		}
		
		return item.GetAsStringS(buff, buffSize);
	}

	Data::Timestamp PostgreSQLTCPReader::GetTimestamp(UIntOS colIndex)
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

	Double PostgreSQLTCPReader::GetDblOrNAN(UIntOS colIndex)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return 0.0;
		}
		
		return item.GetAsF64();
	}

	Bool PostgreSQLTCPReader::GetBool(UIntOS colIndex)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return false;
		}
		
		return item.GetAsBool();
	}

	UIntOS PostgreSQLTCPReader::GetBinarySize(UIntOS colIndex)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return 0;
		}
		
		Data::ReadonlyArray<UInt8>* arr = item.GetAndRemoveByteArr();
		if (arr)
		{
			UIntOS ret = arr->GetCount();
			DEL_CLASS(arr);
			return ret;
		}
		return 0;
	}

	UIntOS PostgreSQLTCPReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return 0;
		}
		
		Data::ReadonlyArray<UInt8>* arr = item.GetAndRemoveByteArr();
		if (arr)
		{
			UIntOS ret = arr->GetCount();
			MemCopyNO(buff.Ptr(), arr->GetArray(), ret);
			DEL_CLASS(arr);
			return ret;
		}
		return 0;
	}

	Optional<Math::Geometry::Vector2D> PostgreSQLTCPReader::GetVector(UIntOS colIndex)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return nullptr;
		}
		
		return item.GetAndRemoveVector();
	}

	Bool PostgreSQLTCPReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return false;
		}
		
		return item.GetAndRemoveUUID();
	}

	UnsafeArrayOpt<UTF8Char> PostgreSQLTCPReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
	{
		if (colIndex >= colCount)
		{
			return nullptr;
		}
		
		return Text::StrConcat(buff, columnNames.GetItem(colIndex));
	}

	Bool PostgreSQLTCPReader::IsNull(UIntOS colIndex)
	{
		Data::VariItem item;
		if (!GetVariItem(colIndex, item))
		{
			return false;
		}
		
		return item.GetItemType() == Data::VariItem::ItemType::Null;
	}

	DB::DBUtil::ColType PostgreSQLTCPReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
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
				colSize.SetNoCheck(typeMod - 4);
			}
			else
			{
				colSize.SetNoCheck(65535);
			}
		}
		
		return colType;
	}

	Bool PostgreSQLTCPReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
	{
		if (colIndex >= colCount)
		{
			return false;
		}
		
		colDef->SetColName(Text::CStringNN(columnNames.GetItem(colIndex)));
		colDef->SetColType(DBType2ColType(columnTypes.GetItem(colIndex)));
		
		Int32 typeMod = columnTypeMods.GetItem(colIndex);
		if (typeMod >= 0)
		{
			colDef->SetColSize(typeMod - 4);
		}
		else
		{
			colDef->SetColSize(65535);
		}
		
		return true;
	}

	DB::DBUtil::ColType PostgreSQLTCPReader::DBType2ColType(UInt32 dbType)
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
			case 700:
				return DB::DBUtil::CT_Float;
			case 701:
				return DB::DBUtil::CT_Double;
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

	Bool PostgreSQLTCPReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
	{
		if (currRow < 0 || currRow >= rowCount || colIndex >= colCount)
		{
			return false;
		}
		
		UInt32 idx = colIndex + currRow * colCount;
		UInt8* val = rowValues.GetItem(idx);
		UInt32 len = valueLengths.GetItem(idx);
		
		if (val == nullptr)
		{
			item->SetNull();
			return true;
		}
		
		UInt32 dbType = columnTypes.GetItem(colIndex);
		
		switch (dbType)
		{
			case 16:
				item->SetBool(*val == 't');
				break;
			case 17:
				item->SetByteArr(val, len);
				break;
			case 18:
			case 19:
			case 25:
			case 1042:
			case 1043:
			case 1009:
				{
					UTF8Char* str = MemAllocArr(UTF8Char, len + 1);
					MemCopy(str, val, len);
					str[len] = 0;
					item->SetStrSlow(str);
					MemFree(str);
				}
				break;
			case 20:
				if (len == 8)
				{
					item->SetI64(Data::GetInt64BE(val));
				}
				else
				{
					item->SetI64(0);
				}
				break;
			case 21:
				if (len == 2)
				{
					item->SetI16((Int16)ReadMInt16(val));
				}
				else if (len == 4)
				{
					item->SetI16((Int16)ReadMInt32(val));
				}
				else
				{
					item->SetI16(0);
				}
				break;
			case 23:
				if (len == 4)
				{
					item->SetI32(ReadMInt32(val));
				}
				else
				{
					item->SetI32(0);
				}
				break;
			case 700:
				if (len == 4)
				{
					UInt32 intVal = ReadMInt32(val);
					Single floatVal = *(Single*)&intVal;
					item->SetF64((Double)floatVal);
				}
				else
				{
					item->SetF64(0.0);
				}
				break;
			case 701:
				if (len == 8)
				{
					UInt64 intVal = Data::GetInt64BE(val);
					Double floatVal = *(Double*)&intVal;
					item->SetF64(floatVal);
				}
				else
				{
					item->SetF64(0.0);
				}
				break;
			case 1082:
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)val, len);
					item->SetDate(Data::Date(sb.ToCString()));
				}
				break;
			case 1114:
			case 1184:
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)val, len);
					item->SetDate(Data::Timestamp(sb.ToCString(), tzQhr));
				}
				break;
			case 2950:
				{
					Data::UUID* uuid = MemAllocNN(Data::UUID);
					uuid->Parse(Text::CStringNN((const UTF8Char*)val, len));
					item->SetUUIDDirect(uuid);
				}
				break;
			default:
				{
					UTF8Char* str = MemAllocArr(UTF8Char, len + 1);
					MemCopy(str, val, len);
					str[len] = 0;
					item->SetStrSlow(str);
					MemFree(str);
				}
				break;
		}
		
		return true;
	}
}

