#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Data/RandomBytesGenerator.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/PBKDF2.h"
#include "DB/DBTool.h"
#include "DB/PostgreSQLTCPConn.h"
#include "Crypto/Hash/MD5.h"
#include "Math/Geometry/Point.h"
#include "Math/WKBReader.h"
#include "Net/SocketUtil.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/Base64Enc.h"

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
	UnsafeArray<UInt8> packet = MemAllocArr(UInt8, dataLen + 5);
	packet[0] = msgType;
	WriteMInt32(packet.Ptr() + 1, packetLen);
	if (dataLen > 0)
	{
		MemCopyO(packet.Ptr() + 5, data.Ptr(), dataLen);
	}
	
	UIntOS written = cli->Write(Data::ByteArray(packet, dataLen + 5));
	MemFreeArr(packet);
	return written == dataLen + 5;
}

Bool DB::PostgreSQLTCPConn::SendStartupPacket(NN<Net::TCPClient> cli, Text::CString user, Text::CStringNN database)
{
	UInt8 packet[1024];
	UnsafeArray<UInt8> p = packet + 4;
	
	WriteMInt32(&p[0], 196608); // Protocol version 3.0
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
	
	return cli->Write(Data::ByteArray(packet, packetLen)) == packetLen;
}

Bool DB::PostgreSQLTCPConn::ParseAuthentication(NN<Net::TCPClient> cli)
{
	UInt8 buff[5];
	while (true)
	{
		if (this->ReadPacket(cli, buff, 5) != 5 || buff[0] != 'R')
			{
				this->log->LogMessage(CSTR("Failed to read authentication packet"), IO::LogHandler::LogLevel::Error);
				return false;
			}
		
		UInt32 len = ReadMUInt32(buff + 1);
		if (len < 8 || this->ReadPacket(cli, buff, 4) != 4)
			{
				this->log->LogMessage(CSTR("Invalid authentication packet length"), IO::LogHandler::LogLevel::Error);
				return false;
			}
		
		switch (ReadMInt32(buff))
		{
		case 0:
			return true;
		case 10:
			if (!this->ParseSCRAMSHA256(cli, len - 8))
			{
				return false;
			}
			break;
		case 5:
			{
				UInt8 salt[4];
				if (len != 12 || this->ReadPacket(cli, salt, 4) != 4)
				{
					this->log->LogMessage(CSTR("Failed to read MD5 salt"), IO::LogHandler::LogLevel::Error);
					return false;
				}
				
				NN<Text::String> nnuid;
				NN<Text::String> nnpwd;
				if (!this->uid.SetTo(nnuid) || !this->pwd.SetTo(nnpwd))
				{
					this->log->LogMessage(CSTR("User name or password is missing"), IO::LogHandler::LogLevel::Error);
					return false;
				}
				
				static const UTF8Char hexChars[] = "0123456789abcdef";
				UInt8 hash[16];
				UTF8Char hashText[32];
				Crypto::Hash::MD5 md5;
				md5.Calc(nnpwd->v, nnpwd->leng);
				md5.Calc(nnuid->v, nnuid->leng);
				md5.GetValue(hash);
				for (UIntOS i = 0; i < 16; i++)
				{
					hashText[i * 2] = hexChars[hash[i] >> 4];
					hashText[i * 2 + 1] = hexChars[hash[i] & 15];
				}
				md5.Clear();
				md5.Calc((const UInt8*)hashText, 32);
				md5.Calc(salt, 4);
				md5.GetValue(hash);
				
				UInt8 response[36];
				response[0] = 'm';
				response[1] = 'd';
				response[2] = '5';
				for (UIntOS i = 0; i < 16; i++)
				{
					response[3 + i * 2] = (UInt8)hexChars[hash[i] >> 4];
					response[3 + i * 2 + 1] = (UInt8)hexChars[hash[i] & 15];
				}
				response[35] = 0;
				if (!this->SendPacket(cli, 'p', response, 36))
				{
					return false;
				}
			}
			break;
		default:
				
			this->log->LogMessage(Text::StringBuilderUTF8().Append(CSTR("Unsupported authentication type: "))->AppendI32(ReadMInt32(buff))->ToCString(), IO::LogHandler::LogLevel::Error);
			return false;
		}
	}
}

Bool DB::PostgreSQLTCPConn::ParseSCRAMSHA256(NN<Net::TCPClient> cli, UIntOS mechanismListLen)
{
	UnsafeArray<UInt8> mechanismList = MemAllocArr(UInt8, mechanismListLen);
	if (this->ReadPacket(cli, mechanismList, mechanismListLen) != mechanismListLen)
	{
		MemFreeArr(mechanismList);
		this->log->LogMessage(CSTR("Failed to read SASL mechanism list"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	Bool hasSCRAMSHA256 = false;
	UIntOS listOfst = 0;
	while (listOfst < mechanismListLen && mechanismList[listOfst] != 0)
	{
		UIntOS mechanismLen = 0;
		while (listOfst + mechanismLen < mechanismListLen && mechanismList[listOfst + mechanismLen] != 0)
		{
			mechanismLen++;
		}
		if (mechanismLen == 13 && Text::StrEqualsC((const UTF8Char*)&mechanismList[listOfst], mechanismLen, UTF8STRC("SCRAM-SHA-256")))
		{
			hasSCRAMSHA256 = true;
			break;
		}
		listOfst += mechanismLen + 1;
	}
	MemFreeArr(mechanismList);
	if (!hasSCRAMSHA256)
	{
		this->log->LogMessage(CSTR("PostgreSQL server does not offer SCRAM-SHA-256 authentication"), IO::LogHandler::LogLevel::Error);
		return false;
	}

	NN<Text::String> nnuid;
	NN<Text::String> nnpwd;
	if (!this->uid.SetTo(nnuid) || !this->pwd.SetTo(nnpwd))
	{
		this->log->LogMessage(CSTR("User name or password is missing"), IO::LogHandler::LogLevel::Error);
		return false;
	}

	UInt8 nonceBytes[18];
	Data::RandomBytesGenerator random;
	random.NextBytes(nonceBytes, sizeof(nonceBytes));
	Text::StringBuilderUTF8 nonce;
	Text::TextBinEnc::Base64Enc base64;
	base64.EncodeBin(nonce, nonceBytes, sizeof(nonceBytes));
	Text::StringBuilderUTF8 clientFirstBare;
	clientFirstBare.AppendC(UTF8STRC("n="));
	for (UIntOS i = 0; i < nnuid->leng; i++)
	{
		if (nnuid->v[i] == ',')
		{
			clientFirstBare.AppendC(UTF8STRC("=2C"));
		}
		else if (nnuid->v[i] == '=')
		{
			clientFirstBare.AppendC(UTF8STRC("=3D"));
		}
		else
		{
			clientFirstBare.AppendUTF8Char(nnuid->v[i]);
		}
	}
	clientFirstBare.AppendC(UTF8STRC(",r="));
	clientFirstBare.Append(nonce.ToCString());
	Text::StringBuilderUTF8 clientFirst;
	clientFirst.AppendC(UTF8STRC("n,,"));
	clientFirst.Append(clientFirstBare.ToCString());

	UIntOS initialDataLen = 13 + 1 + 4 + clientFirst.GetLength();
	UnsafeArray<UInt8> initialData = MemAllocArr(UInt8, initialDataLen);
	MemCopyNO(initialData.Ptr(), "SCRAM-SHA-256", 13);
	initialData[13] = 0;
	WriteMInt32(initialData.Ptr() + 14, (Int32)clientFirst.GetLength());
	MemCopyNO(initialData.Ptr() + 18, clientFirst.v.Ptr(), clientFirst.GetLength());
	Bool sent = this->SendPacket(cli, 'p', initialData, initialDataLen);
	MemFreeArr(initialData);
	if (!sent)
	{
		return false;
	}

	UInt8 packetHeader[5];
	if (this->ReadPacket(cli, packetHeader, 5) != 5)
	{
		this->log->LogMessage(CSTR("Failed to read SASL continuation"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	if (packetHeader[0] == 'E')
	{
		Text::StringBuilderUTF8 errMsg;
		if (this->ParseErrorResponse(cli, ReadMUInt32(packetHeader + 1) - 4, errMsg))
		{
			this->log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	if (packetHeader[0] != 'R')
	{
		this->log->LogMessage(CSTR("Unexpected PostgreSQL SASL response"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	UInt32 packetLen = ReadMUInt32(packetHeader + 1);
	if (packetLen < 8 || this->ReadPacket(cli, packetHeader, 4) != 4 || ReadMInt32(packetHeader) != 11)
	{
		this->log->LogMessage(CSTR("Invalid SASL continuation"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	UIntOS serverFirstLen = packetLen - 8;
	UnsafeArray<UTF8Char> serverFirst = MemAllocArr(UTF8Char, serverFirstLen);
	if (this->ReadPacket(cli, (UnsafeArray<UInt8>)serverFirst, serverFirstLen) != serverFirstLen)
	{
		MemFreeArr(serverFirst);
		this->log->LogMessage(CSTR("Failed to read SASL server-first message"), IO::LogHandler::LogLevel::Error);
		return false;
	}

	UnsafeArrayOpt<const UTF8Char> serverNonce = nullptr;
	UIntOS serverNonceLen = 0;
	UnsafeArrayOpt<const UTF8Char> saltText = nullptr;
	UIntOS saltTextLen = 0;
	UIntOS iterationCount = 0;
	for (UIntOS ofst = 0; ofst + 2 <= serverFirstLen; )
	{
		UIntOS valueOfst = ofst + 2;
		UIntOS valueLen = 0;
		while (valueOfst + valueLen < serverFirstLen && serverFirst[valueOfst + valueLen] != ',')
		{
			valueLen++;
		}
		if (serverFirst[ofst + 1] != '=')
		{
			MemFreeArr(serverFirst);
			return false;
		}
		if (serverFirst[ofst] == 'r')
		{
			serverNonce = UnsafeArray<const UTF8Char>(serverFirst + valueOfst);
			serverNonceLen = valueLen;
		}
		else if (serverFirst[ofst] == 's')
		{
			saltText = UnsafeArray<const UTF8Char>(serverFirst + valueOfst);
			saltTextLen = valueLen;
		}
		else if (serverFirst[ofst] == 'i')
		{
			for (UIntOS i = 0; i < valueLen; i++)
			{
				if (serverFirst[valueOfst + i] < '0' || serverFirst[valueOfst + i] > '9')
				{
					MemFreeArr(serverFirst);
					return false;
				}
				iterationCount = iterationCount * 10 + (UIntOS)(serverFirst[valueOfst + i] - '0');
			}
		}
		ofst = valueOfst + valueLen + 1;
	}
	UnsafeArray<const UTF8Char> nnServerNonce;
	UnsafeArray<const UTF8Char> nnSaltText;
	if (!serverNonce.SetTo(nnServerNonce) || !saltText.SetTo(nnSaltText) || iterationCount == 0 || serverNonceLen <= nonce.GetLength() || !Text::StrStartsWithC(nnServerNonce, serverNonceLen, nonce.v, nonce.GetLength()))
	{
		MemFreeArr(serverFirst);
		this->log->LogMessage(CSTR("Invalid SASL server-first message"), IO::LogHandler::LogLevel::Error);
		return false;
	}

	UnsafeArray<UInt8> salt = MemAllocArr(UInt8, saltTextLen * 3 / 4);
	UIntOS saltLen = base64.DecodeBin(Text::CStringNN(nnSaltText, saltTextLen), salt);
	UInt8 saltedPassword[32];
	Crypto::Hash::SHA256 sha256;
	Crypto::Hash::HMAC hmacPassword(sha256, nnpwd->v, nnpwd->leng);
	Crypto::PBKDF2::Calc(salt, saltLen, iterationCount, sizeof(saltedPassword), hmacPassword, saltedPassword);
	MemFreeArr(salt);

	UInt8 clientKey[32];
	UInt8 storedKey[32];
	UInt8 clientSignature[32];
	UInt8 clientProof[32];
	UInt8 serverKey[32];
	UInt8 expectedServerSignature[32];
	Crypto::Hash::HMAC hmacClientKey(sha256, saltedPassword, sizeof(saltedPassword));
	hmacClientKey.Calc((const UInt8*)"Client Key", 10);
	hmacClientKey.GetValue(clientKey);
	Crypto::Hash::HMAC hmacServerKey(sha256, saltedPassword, sizeof(saltedPassword));
	hmacServerKey.Calc((const UInt8*)"Server Key", 10);
	hmacServerKey.GetValue(serverKey);
	sha256.Clear();
	sha256.Calc(clientKey, sizeof(clientKey));
	sha256.GetValue(storedKey);
	Text::StringBuilderUTF8 clientFinalWithoutProof;
	clientFinalWithoutProof.AppendC(UTF8STRC("c=biws,r="));
	clientFinalWithoutProof.AppendC(nnServerNonce, serverNonceLen);
	Text::StringBuilderUTF8 authMessage;
	authMessage.Append(clientFirstBare.ToCString());
	authMessage.AppendUTF8Char(',');
	authMessage.AppendC(serverFirst, serverFirstLen);
	authMessage.AppendUTF8Char(',');
	authMessage.Append(clientFinalWithoutProof.ToCString());
	Crypto::Hash::HMAC hmacSignature(sha256, storedKey, sizeof(storedKey));
	hmacSignature.Calc(authMessage.v, authMessage.GetLength());
	hmacSignature.GetValue(clientSignature);
	Crypto::Hash::HMAC hmacServerSignature(sha256, serverKey, sizeof(serverKey));
	hmacServerSignature.Calc(authMessage.v, authMessage.GetLength());
	hmacServerSignature.GetValue(expectedServerSignature);
	for (UIntOS i = 0; i < sizeof(clientProof); i++)
	{
		clientProof[i] = clientKey[i] ^ clientSignature[i];
	}
	Text::StringBuilderUTF8 clientFinal;
	clientFinal.Append(clientFinalWithoutProof.ToCString());
	clientFinal.AppendC(UTF8STRC(",p="));
	base64.EncodeBin(clientFinal, clientProof, sizeof(clientProof));
	MemFreeArr(serverFirst);
	if (!this->SendPacket(cli, 'p', (UnsafeArray<UInt8>)clientFinal.v, clientFinal.GetLength()))
	{
		return false;
	}

	if (this->ReadPacket(cli, packetHeader, 5) != 5)
	{
		this->log->LogMessage(CSTR("Failed to read SASL final message"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	if (packetHeader[0] == 'E')
	{
		Text::StringBuilderUTF8 errMsg;
		if (this->ParseErrorResponse(cli, ReadMUInt32(packetHeader + 1) - 4, errMsg))
		{
			this->log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	UInt32 serverFinalPacketLen = ReadMUInt32(packetHeader + 1);
	if (packetHeader[0] != 'R' || serverFinalPacketLen < 8 || this->ReadPacket(cli, packetHeader, 4) != 4 || ReadMInt32(packetHeader) != 12)
	{
		this->log->LogMessage(CSTR("Invalid SASL final message"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	UIntOS serverFinalLen = serverFinalPacketLen - 8;
	UnsafeArray<UTF8Char> serverFinal = MemAllocArr(UTF8Char, serverFinalLen + 1);
	Bool readFinal = this->ReadPacket(cli, (UnsafeArray<UInt8>)serverFinal, serverFinalLen) == serverFinalLen;
	if (!readFinal)
	{
		MemFreeArr(serverFinal);
		this->log->LogMessage(CSTR("Failed to read SASL final data"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	serverFinal[serverFinalLen] = 0;
	if (serverFinalLen < 3 || serverFinal[0] != 'v' || serverFinal[1] != '=')
	{
		MemFreeArr(serverFinal);
		this->log->LogMessage(CSTR("PostgreSQL SASL authentication failed"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	UIntOS serverSignatureLen = base64.CalcBinSize(Text::CStringNN(serverFinal + 2, serverFinalLen - 2));
	UInt8 serverSignature[32];
	if (serverSignatureLen != sizeof(serverSignature))
	{
		MemFreeArr(serverFinal);
		this->log->LogMessage(CSTR("Invalid SASL server signature"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	base64.DecodeBin(Text::CStringNN(serverFinal + 2, serverFinalLen - 2), serverSignature);
	MemFreeArr(serverFinal);
	UInt8 signatureDiff = 0;
	for (UIntOS i = 0; i < sizeof(serverSignature); i++)
	{
		signatureDiff |= serverSignature[i] ^ expectedServerSignature[i];
	}
	if (signatureDiff != 0)
	{
		this->log->LogMessage(CSTR("Invalid SASL server signature"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseBackendKeyData(NN<Net::TCPClient> cli)
{
	UInt8 buff[8];
	if (this->ReadPacket(cli, buff, 8) != 8)
	{
		return false;
	}
	
	this->backendPID = ReadMUInt32(buff);
	this->cancelKey = ReadMInt32(buff + 4);
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseRowDescription(NN<Net::TCPClient> cli, UIntOS dataLen, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods)
{
	if (dataLen < 2)
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
		
		UInt8 typeInfo[18];
		if (this->ReadPacket(cli, typeInfo, 18) != 18)
		{
			return false;
		}
		types->Add(ReadMUInt32(typeInfo + 6));
		typeMods->Add(ReadMInt32(typeInfo + 12));
		
		UInt16 fmtCode = ReadMUInt16(typeInfo + 16);
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
	if (this->ReadPacket(cli, buff, 2) != 2 || ReadMUInt16(buff) != colCount)
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
			UnsafeArray<UInt8> val = MemAllocArr(UInt8, (UIntOS)valLen + 1);
			if (this->ReadPacket(cli, val, (UIntOS)valLen) != (UIntOS)valLen)
			{
				MemFreeArr(val);
				return false;
			}
			val[valLen] = 0;
			values->Add(val);
			lengths->Add((UInt32)valLen);
		}
		else
		{
			UnsafeArray<UInt8> val = MemAllocArr(UInt8, 1);
			val[0] = 0;
			values->Add(val);
			lengths->Add(0);
		}
	}
	
	return true;
}

Bool DB::PostgreSQLTCPConn::ParseCommandComplete(NN<Net::TCPClient> cli, UIntOS dataLen, OutParam<IntOS> rowChanged)
{
	rowChanged.Set(0);
	if (dataLen < 1)
	{
		return false;
	}
	UnsafeArray<UTF8Char> buff = MemAllocArr(UTF8Char, dataLen);
	if (this->ReadPacket(cli, (UnsafeArray<UInt8>)buff, dataLen) != dataLen)
	{
		MemFreeArr(buff);
		return false;
	}
	
	Text::StringBuilderUTF8 sb;
	sb.AppendC(buff, dataLen - 1);
	MemFreeArr(buff);
	
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

Bool DB::PostgreSQLTCPConn::ParseErrorResponse(NN<Net::TCPClient> cli, UIntOS dataLen, NN<Text::StringBuilderUTF8> errMsg)
{
	errMsg->ClearStr();
	if (dataLen == 0)
	{
		return false;
	}
	UnsafeArray<UInt8> data = MemAllocArr(UInt8, dataLen);
	if (this->ReadPacket(cli, data, dataLen) != dataLen)
	{
		MemFreeArr(data);
		return false;
	}
	
	UnsafeArray<UTF8Char> p = (UnsafeArray<UTF8Char>)data.Ptr();
	UnsafeArray<UTF8Char> dataEnd = p + dataLen;
	while (p < dataEnd && *p != 0)
	{
		UTF8Char fieldType = *p++;
		UnsafeArray<UTF8Char> fieldValue = p;
		while (p < dataEnd && *p != 0) p++;
		if (p == dataEnd)
		{
			MemFreeArr(data);
			return false;
		}
		switch (fieldType)
		{
			case 'M':
				errMsg->AppendC(fieldValue, (UIntOS)(p - fieldValue));
				break;
			case 'S':
				if (errMsg->GetLength() > 0)
				{
					errMsg->AppendC(UTF8STRC("\n"));
				}
				errMsg->AppendC(fieldValue, (UIntOS)(p - fieldValue));
				break;
		}
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
		log->LogMessage(CSTR("Failed to resolve PostgreSQL server address"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	cli = this->clif->Create(addr, this->port, 60000);
	if (cli->IsConnectError())
	{
		cli.Delete();
		log->LogMessage(CSTR("Failed to connect to PostgreSQL server"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	
	this->connCli = cli;
	
	if (!this->SendStartupPacket(cli, OPTSTR_CSTR(this->uid), database->ToCString()))
	{
		this->Close();
		log->LogMessage(CSTR("Failed to send startup packet"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	
	if (!this->ParseAuthentication(cli))
	{
		this->Close();
		log->LogMessage(CSTR("Authentication failed"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	
	Bool gotBackendKey = false;
	while (true)
	{
		UInt8 buff[5];
		if (this->ReadPacket(cli, buff, 5) != 5)
		{
			this->Close();
			log->LogMessage(CSTR("Failed to read PostgreSQL startup response"), IO::LogHandler::LogLevel::Error);
			return false;
		}
		UInt32 len = ReadMUInt32(buff + 1);
		if (len < 4)
		{
			this->Close();
			log->LogMessage(CSTR("Invalid PostgreSQL startup response length"), IO::LogHandler::LogLevel::Error);
			return false;
		}
		UIntOS dataLen = len - 4;
		switch (buff[0])
		{
		case 'K':
			if (dataLen != 8 || !this->ParseBackendKeyData(cli))
			{
				this->Close();
				log->LogMessage(CSTR("Failed to parse backend key data"), IO::LogHandler::LogLevel::Error);
				return false;
			}
			gotBackendKey = true;
			break;
		case 'Z':
			if (dataLen != 1 || this->ReadPacket(cli, buff, 1) != 1 || !gotBackendKey)
			{
				this->Close();
				log->LogMessage(CSTR("Invalid PostgreSQL startup completion"), IO::LogHandler::LogLevel::Error);
				return false;
			}
			isTran = false;
			log->LogMessage(CSTR("PostgreSQL DB Connected"), IO::LogHandler::LogLevel::Raw);
			return true;
		case 'E':
			{
				Text::StringBuilderUTF8 errMsg;
				if (this->ParseErrorResponse(cli, dataLen, errMsg))
				{
					log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
				}
			}
			this->Close();
			return false;
		default:
			{
				UnsafeArray<UInt8> data = MemAllocArr(UInt8, dataLen);
				Bool readSucc = this->ReadPacket(cli, data, dataLen) == dataLen;
				MemFreeArr(data);
				if (!readSucc)
				{
					this->Close();
					log->LogMessage(CSTR("Failed to read PostgreSQL startup response data"), IO::LogHandler::LogLevel::Error);
					return false;
				}
			}
			break;
		}
	}
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

	if (ExecuteReader(CSTR("SELECT COALESCE(to_regtype('geometry')::oid, 0::oid), COALESCE(to_regtype('geography')::oid, 0::oid)")).SetTo(r))
	{
		if (r->ReadNext())
		{
			this->geometryOid = (UInt32)r->GetInt32(0);
			this->geographyOid = (UInt32)r->GetInt32(1);
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
	this->connCli = nullptr;
	this->tzQhr = 0;
	this->readerActive = false;
	this->geometryOid = 0;
	this->geographyOid = 0;
	
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
	this->connCli = nullptr;
	this->tzQhr = 0;
	this->readerActive = false;
	this->geometryOid = 0;
	this->geographyOid = 0;
	
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
		
		this->log->LogMessage(CSTR("PostgreSQL DB Disconnected"), IO::LogHandler::LogLevel::Raw);
	}
}

void DB::PostgreSQLTCPConn::Dispose()
{
	DEL_CLASS(this);
}

	IntOS DB::PostgreSQLTCPConn::ExecuteNonQuery(Text::CStringNN sql)
{
	this->lastDataError = DataError::NoError;
	if (this->readerActive)
	{
		this->lastDataError = DataError::ConnError;
		return -2;
	}
	NN<Net::TCPClient> cli;
	if (!this->connCli.SetTo(cli))
	{
		return -2;
	}
	
	UIntOS packetLen = sql.leng + 1;
	UnsafeArray<UInt8> packet = MemAllocArr(UInt8, packetLen);
	sql.ConcatTo(packet);
	packet[sql.leng] = 0;
	
	if (!this->SendPacket(cli, 'Q', packet, packetLen))
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
		UIntOS dataLen = ReadMUInt32(buff + 1) - 4;
		
		switch (buff[0])
		{
			case 'C':
				if (!this->ParseCommandComplete(cli, dataLen, rowChanged))
				{
					return -2;
				}
				break;
			case 'Z':
				if (dataLen != 1 || this->ReadPacket(cli, buff, 1) != 1)
				{
					return -2;
				}
				return rowChanged;
			case 'E':
				{
					Text::StringBuilderUTF8 errMsg;
					if (this->ParseErrorResponse(cli, dataLen, errMsg))
					{
						this->lastDataError = DataError::ExecSQLError;
						this->log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
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
	if (this->readerActive)
	{
		this->lastDataError = DataError::ConnError;
		return nullptr;
	}
	NN<Net::TCPClient> cli;
	if (!this->connCli.SetTo(cli))
	{
		return nullptr;
	}
	
	UIntOS packetLen = sql.leng + 1;
	UnsafeArray<UInt8> packet = MemAllocArr(UInt8, packetLen);
	sql.ConcatTo(packet);
	packet[sql.leng] = 0;
	
	if (!this->SendPacket(cli, 'Q', packet, packetLen))
	{
		MemFreeArr(packet);
		return nullptr;
	}
	MemFreeArr(packet);
	
	Data::ArrayListStringNN colNames;
	Data::ArrayListNative<UInt32> types;
	Data::ArrayListNative<Int32> typeMods;
	IntOS rowChanged = 0;
	Bool queryError = false;
	while (true)
	{
		UInt8 buff[5];
		if (this->ReadPacket(cli, buff, 5) != 5)
		{
			return nullptr;
		}
		UInt32 packetLen = ReadMUInt32(buff + 1);
		if (packetLen < 4)
		{
			this->Close();
			return nullptr;
		}
		UIntOS dataLen = packetLen - 4;
		
		switch (buff[0])
		{
			case 'C':
				if (!this->ParseCommandComplete(cli, dataLen, rowChanged))
				{
					return nullptr;
				}
				break;
			case 'T':
				if (!this->ParseRowDescription(cli, dataLen, colNames, types, typeMods))
				{
					colNames.FreeAll();
					this->Close();
					return nullptr;
				}
				this->readerActive = true;
				return NEW_CLASS_D(PostgreSQLTCPReader(NNTHIS, cli->GetCliId(), this->backendPID, this->cancelKey, this->tzQhr, colNames, types, typeMods, false));
			case 'Z':
				if (dataLen != 1 || this->ReadPacket(cli, buff, 1) != 1)
				{
					this->Close();
					return nullptr;
				}
				if (queryError)
				{
					return nullptr;
				}
				return NEW_CLASS_D(PostgreSQLTCPReader(NNTHIS, cli->GetCliId(), this->backendPID, this->cancelKey, this->tzQhr, colNames, types, typeMods, true));
			case 'E':
				{
					Text::StringBuilderUTF8 errMsg;
					if (this->ParseErrorResponse(cli, dataLen, errMsg))
					{
						this->lastDataError = DataError::ExecSQLError;
						this->log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
					}
				}
				queryError = true;
				break;
			default:
				while (dataLen > 0)
				{
					UInt8 skipBuff[256];
					UIntOS skipSize = dataLen > sizeof(skipBuff) ? sizeof(skipBuff) : dataLen;
					if (this->ReadPacket(cli, skipBuff, skipSize) != skipSize)
					{
						this->Close();
						return nullptr;
					}
					dataLen -= skipSize;
				}
				break;
		}
	}
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

DB::PostgreSQLTCPReader::PostgreSQLTCPReader(NN<PostgreSQLTCPConn> conn, UInt64 clientId, UInt32 backendPID, Int32 cancelKey, Int8 tzQhr, NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListNative<UInt32>> types, NN<Data::ArrayListNative<Int32>> typeMods, Bool finished)
{
	this->conn = conn;
	this->clientId = clientId;
	this->backendPID = backendPID;
	this->cancelKey = cancelKey;
	this->tzQhr = tzQhr;
	this->rowChanged = 0;
	this->hasCurrentRow = false;
	this->finished = finished;
	
	Data::ArrayIterator<NN<Text::String>> nameIt = colNames->Iterator();
	while (nameIt.HasNext())
	{
		columnNames.Add(nameIt.Next()->Clone());
	}
	colNames->FreeAll();
	
	colCount = (UIntOS)columnNames.GetCount();
	
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
	while (this->ReadNext())
	{
	}
	this->ClearCurrentRow();
	columnNames.FreeAll();
}

void DB::PostgreSQLTCPReader::ClearCurrentRow()
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
	this->rowValues.Clear();
	this->valueLengths.Clear();
	this->hasCurrentRow = false;
}

Bool DB::PostgreSQLTCPReader::ReadNext()
{
	this->ClearCurrentRow();
	if (this->finished)
	{
		return false;
	}

	NN<Net::TCPClient> cli;
	if (!this->conn->connCli.SetTo(cli))
	{
		this->finished = true;
		this->conn->readerActive = false;
		return false;
	}
	if (cli->GetCliId() != this->clientId)
	{
		this->finished = true;
		this->conn->readerActive = false;
		return false;
	}

	while (true)
	{
		UInt8 buff[5];
		if (this->conn->ReadPacket(cli, buff, 5) != 5)
		{
			this->conn->lastDataError = DB::DBConn::DataError::ConnError;
			this->conn->Close();
			this->finished = true;
			this->conn->readerActive = false;
			return false;
		}
		UInt32 packetLen = ReadMUInt32(buff + 1);
		if (packetLen < 4)
		{
			this->conn->lastDataError = DB::DBConn::DataError::ConnError;
			this->conn->Close();
			this->finished = true;
			this->conn->readerActive = false;
			return false;
		}
		UIntOS dataLen = packetLen - 4;

		switch (buff[0])
		{
		case 'D':
			if (!this->conn->ParseDataRow(cli, this->colCount, this->rowValues, this->valueLengths))
			{
				this->ClearCurrentRow();
				this->conn->lastDataError = DB::DBConn::DataError::ConnError;
				this->conn->Close();
				this->finished = true;
				this->conn->readerActive = false;
				return false;
			}
			this->hasCurrentRow = true;
			return true;
		case 'C':
			if (!this->conn->ParseCommandComplete(cli, dataLen, this->rowChanged))
			{
				this->conn->lastDataError = DB::DBConn::DataError::ConnError;
				this->conn->Close();
				this->finished = true;
				this->conn->readerActive = false;
				return false;
			}
			break;
		case 'E':
			{
				Text::StringBuilderUTF8 errMsg;
				if (this->conn->ParseErrorResponse(cli, dataLen, errMsg))
				{
					this->conn->lastDataError = DB::DBConn::DataError::ExecSQLError;
					this->conn->log->LogMessage(errMsg.ToCString(), IO::LogHandler::LogLevel::Error);
				}
			}
			break;
		case 'Z':
			if (dataLen != 1 || this->conn->ReadPacket(cli, buff, 1) != 1)
			{
				this->conn->lastDataError = DB::DBConn::DataError::ConnError;
				this->conn->Close();
			}
			this->finished = true;
			this->conn->readerActive = false;
			return false;
		default:
			while (dataLen > 0)
			{
				UInt8 skipBuff[256];
				UIntOS skipSize = dataLen > sizeof(skipBuff) ? sizeof(skipBuff) : dataLen;
				if (this->conn->ReadPacket(cli, skipBuff, skipSize) != skipSize)
				{
					this->conn->lastDataError = DB::DBConn::DataError::ConnError;
					this->conn->Close();
					this->finished = true;
					this->conn->readerActive = false;
					return false;
				}
				dataLen -= skipSize;
			}
			break;
		}
	}
}

UIntOS DB::PostgreSQLTCPReader::ColCount()
{
	return colCount;
}

IntOS DB::PostgreSQLTCPReader::GetRowChanged()
{
	return this->rowChanged;
}

Int32 DB::PostgreSQLTCPReader::GetInt32(UIntOS colIndex)
{
	if (colIndex >= colCount || !this->hasCurrentRow)
	{
		return 0;
	}
	
	UnsafeArray<UInt8> val;
	if (!rowValues.GetItem(colIndex).SetTo(val))
	{
		return 0;
	}
	
	return Text::StrToInt32(val.Ptr());
}

Int64 DB::PostgreSQLTCPReader::GetInt64(UIntOS colIndex)
{
	if (colIndex >= colCount || !this->hasCurrentRow)
	{
		return 0;
	}
	
	UnsafeArray<UInt8> val;
	if (!rowValues.GetItem(colIndex).SetTo(val))
	{
		return 0;
	}
	
	return Text::StrToInt64(val.Ptr());
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
	Int32 typeMod = columnTypeMods.GetItem(colIndex);
	DB::DBUtil::ColType colType = DBType2ColType(dbType, typeMod);
	
	if (colSize.IsNotNull())
	{
		colSize.Set(65535);
		if ((dbType == 1042 || dbType == 1043) && typeMod >= 4)
		{
			colSize.Set((UInt32)typeMod - 4);
		}
		else if (dbType == 1700 && typeMod >= 4)
		{
			colSize.Set(((UInt32)typeMod - 4) >> 16);
		}
		else if (dbType == 1114 || dbType == 1184)
		{
			colSize.Set(typeMod >= 0 ? (UIntOS)typeMod : 6);
		}
		else if (colType == DB::DBUtil::CT_Vector)
		{
			colSize.Set(0);
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
	UInt32 dbType = columnTypes.GetItem(colIndex);
	Int32 typeMod = columnTypeMods.GetItem(colIndex);
	DB::DBUtil::ColType colType = DBType2ColType(dbType, typeMod);
	colDef->SetColType(colType);
	colDef->SetColDP(0);

	if (dbType == 1560 && (typeMod == 1 || typeMod == 5))
	{
		colDef->SetColSize(1);
	}
	else if ((dbType == 1042 || dbType == 1043) && typeMod >= 4)
	{
		colDef->SetColSize((UInt32)typeMod - 4);
	}
	else if (dbType == 1700 && typeMod >= 4)
	{
		UInt32 numericMod = (UInt32)typeMod - 4;
		Int32 scale = (Int32)(numericMod & 0x7ff);
		if ((scale & 0x400) != 0)
		{
			scale -= 0x800;
		}
		colDef->SetColSize((numericMod >> 16) & 0xffff);
		colDef->SetColDP(scale < 0 ? 0 : (UIntOS)scale);
	}
	else if ((dbType == 1114 || dbType == 1184) && typeMod >= 0)
	{
		colDef->SetColSize((UIntOS)typeMod);
	}
	else if (dbType == 1114 || dbType == 1184)
	{
		colDef->SetColSize(6);
	}
	else if (colType == DB::DBUtil::CT_Vector)
	{
		colDef->SetColSize(0);
		colDef->SetColDP(0);
	}
	else
	{
		colDef->SetColSize(65535);
	}
	
	return true;
}

DB::DBUtil::ColType DB::PostgreSQLTCPReader::DBType2ColType(UInt32 dbType, Int32 typeMod)
{
	if (dbType == this->conn->geometryOid || dbType == this->conn->geographyOid)
	{
		return DB::DBUtil::CT_Vector;
	}

	switch (dbType)
	{
	case 16:
		return DB::DBUtil::CT_Bool;
	case 1560:
		return typeMod == 1 || typeMod == 5 ? DB::DBUtil::CT_Bool : DB::DBUtil::CT_VarUTF32Char;
	case 17:
		return DB::DBUtil::CT_Binary;
	case 18:
		return DB::DBUtil::CT_UTF32Char;
	case 19:
	case 22:
	case 24:
	case 25:
	case 27:
	case 30:
	case 114:
	case 142:
	case 194:
	case 601:
	case 602:
	case 603:
	case 604:
	case 628:
	case 650:
	case 829:
	case 869:
	case 1043:
	case 1083:
	case 1115:
	case 1182:
	case 1183:
	case 1185:
	case 1186:
	case 1187:
	case 1231:
	case 1263:
	case 1266:
	case 1270:
	case 1562:
	case 199:
	case 2951:
	case 3220:
	case 3361:
	case 3402:
	case 3802:
	case 3807:
	case 5017:
	case 12028:
	case 16468:
		return DB::DBUtil::CT_VarUTF32Char;
	case 20:
		return DB::DBUtil::CT_Int64;
	case 21:
		return DB::DBUtil::CT_Int16;
	case 23:
		return DB::DBUtil::CT_Int32;
	case 26:
	case 28:
	case 29:
		return DB::DBUtil::CT_UInt32;
	case 1042:
		return DB::DBUtil::CT_UTF32Char;
	case 600:
		return DB::DBUtil::CT_Vector;
	case 700:
		return DB::DBUtil::CT_Float;
	case 701:
		return DB::DBUtil::CT_Double;
	case 790:
	case 1700:
		return DB::DBUtil::CT_Decimal;
	case 1082:
		return DB::DBUtil::CT_Date;
	case 1114:
		return DB::DBUtil::CT_DateTime;
	case 1184:
		return DB::DBUtil::CT_DateTimeTZ;
	case 2950:
		return DB::DBUtil::CT_UUID;
	default:
		return DB::DBUtil::CT_VarUTF32Char;
	}
}

Bool DB::PostgreSQLTCPReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
{
	if (!this->hasCurrentRow || colIndex >= colCount)
	{
		return false;
	}
	
	UIntOS idx = colIndex;
	UnsafeArray<UInt8> val;
	if (!rowValues.GetItem(idx).SetTo(val))
	{
		item->SetNull();
		return true;
	}
	UInt32 len = valueLengths.GetItem(idx);
	UInt32 dbType = columnTypes.GetItem(colIndex);
	if (dbType == this->conn->geometryOid || dbType == this->conn->geographyOid)
	{
		UIntOS ofst = (len >= 2 && val[0] == '\\' && val[1] == 'x') ? 2 : 0;
		UnsafeArray<UInt8> wkb = MemAllocArr(UInt8, (len - ofst) >> 1);
		UIntOS wkbLen = Text::StrHex2Bytes(UnsafeArray<const UTF8Char>(val + ofst), wkb);
		Math::WKBReader reader(0);
		NN<Math::Geometry::Vector2D> vec;
		if (reader.ParseWKB(wkb, wkbLen, 0).SetTo(vec))
		{
			MemFreeArr(wkb);
			item->SetVectorDirect(vec);
			return true;
		}
		MemFreeArr(wkb);
	}
	
	switch (dbType)
	{
	case 16:
		item->SetBool(len > 0 && (val[0] == 't' || val[0] == '1'));
		break;
	case 1560:
		if (DBType2ColType(dbType, columnTypeMods.GetItem(colIndex)) == DB::DBUtil::CT_Bool)
		{
			item->SetBool(len > 0 && val[0] == '1');
		}
		else
		{
			item->SetStrCopy(UnsafeArray<const UInt8>(val), len);
		}
		break;
	case 17:
		if (len >= 2 && val[0] == '\\' && val[1] == 'x')
		{
			UnsafeArray<UInt8> byteArr = MemAllocArr(UInt8, len / 2);
			UIntOS byteLen = Text::StrHex2Bytes(UnsafeArray<const UTF8Char>(val + 2), byteArr);
			item->SetByteArr(byteArr, byteLen);
			MemFreeArr(byteArr);
		}
		else
		{
			item->SetByteArr(UnsafeArray<const UInt8>(val), len);
		}
		break;
	case 18:
		item->SetU8(val[0]);
		break;
	case 20:
		item->SetI64(Text::StrToInt64(val.Ptr()));
		break;
	case 21:
		item->SetI16((Int16)Text::StrToInt32(val.Ptr()));
		break;
	case 23:
		item->SetI32(Text::StrToInt32(val.Ptr()));
		break;
	case 26:
	case 28:
	case 29:
		item->SetU32(Text::StrToUInt32(val.Ptr()));
		break;
	case 700:
		item->SetF32((Single)Text::StrToDoubleOrNAN(val.Ptr()));
		break;
	case 701:
	case 790:
	case 1700:
		item->SetF64(Text::StrToDoubleOrNAN(val.Ptr()));
		break;
	case 600:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(val, len);
			if (len < 5 || sb.ToString()[0] != '(' || !sb.EndsWith(')'))
			{
				item->SetStrCopy(UnsafeArray<const UInt8>(val), len);
				break;
			}
			sb.RemoveChars(1);
			Text::PString parts[3];
			if (Text::StrSplitP(parts, 3, sb.Substring(1), ',') != 2)
			{
				item->SetStrCopy(UnsafeArray<const UInt8>(val), len);
				break;
			}
			NN<Math::Geometry::Point> point;
			NEW_CLASSNN(point, Math::Geometry::Point(0, parts[0].ToDoubleOrNAN(), parts[1].ToDoubleOrNAN()));
			item->SetVectorDirect(point);
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
