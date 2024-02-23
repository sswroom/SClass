#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"
#include "Net/LDAPClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/CharUtil.h"

#define RECVBUFFSIZE 65536

#define VERBOSE
#if defined(VERBOSE)
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>
#endif

UInt32 __stdcall Net::LDAPClient::RecvThread(void *userObj)
{
	Net::LDAPClient *me = (Net::LDAPClient*)userObj;
	UOSInt buffSize;
	UOSInt recvSize;
	UOSInt i;
	UOSInt j;
	Double t;
	me->recvRunning = true;
	{
		buffSize = 0;
		Data::ByteBuffer recvBuff(RECVBUFFSIZE);
		Manage::HiResClock clk;
		#if defined(VERBOSE)
		Text::StringBuilderUTF8 sb;
		#endif
		while (!me->recvToStop)
		{
			clk.Start();
			recvSize = me->cli->Read(recvBuff.SubArray(buffSize));
			if (recvSize == 0)
			{
				break;
			}
			t = clk.GetTimeDiff();
			#if defined(VERBOSE)
			sb.ClearStr();
			sb.AppendHexBuff(&recvBuff[buffSize], recvSize, ' ', Text::LineBreakType::CRLF);
			printf("%s\r\n", sb.ToString());
			#endif

			if (t > 2)
			{
				i = buffSize;
			}
			else
			{
				i = 0;
			}
			buffSize += recvSize;

			while (i < buffSize)
			{
				if (recvBuff[i] != 0x30)
				{
					i = buffSize;
					break;
				}
				if (i + 1 >= buffSize)
				{
					break;
				}
				UInt32 len = recvBuff[i + 1];
				if (len <= 128)
				{
					j = i + 2;
				}
				else if (len == 0x81)
				{
					if (i + 2 >= buffSize)
						break;
					else
					{
						len = recvBuff[i + 2];
						j = i + 3;
					}
				}
				else if (len == 0x82)
				{
					if (i + 3 >= buffSize)
						break;
					else
					{
						len = ReadMUInt16(&recvBuff[i + 2]);
						j = i + 4;
					}
				}
				else if (len == 0x83)
				{
					if (i + 4 >= buffSize)
						break;
					else
					{
						len = ReadMUInt24(&recvBuff[i + 2]);
						j = i + 5;
					}
				}
				else if (len == 0x84)
				{
					if (i + 5 >= buffSize)
						break;
					else
					{
						len = ReadMUInt32(&recvBuff[i + 2]);
						j = i + 6;
					}
				}
				else
				{
					i = buffSize;
					break;
				}

				if (j > RECVBUFFSIZE - 6)
				{
					i = buffSize;
					break;
				}
				else if (j + len > buffSize)
				{
					break;
				}
				else
				{
					me->ParseLDAPMessage(&recvBuff[j], len);

					i = j + len;
				}
			}
			if (i >= buffSize)
			{
				buffSize = 0;
			}
			else if (i > 0)
			{
				recvBuff.CopyInner(0, i, buffSize - i);
				buffSize -= i;
			}
		}
	}
	me->recvRunning = false;
	return 0;
}

void Net::LDAPClient::ParseLDAPMessage(const UInt8 *msgBuff, UOSInt msgLen)
{
	UInt32 msgId;
	const UInt8 *msgEnd = msgBuff + msgLen;
	UInt8 seqType;
	const UInt8 *seqEnd;
	Net::LDAPClient::ReqStatus *req;

	msgBuff = Net::ASN1Util::PDUParseUInt32(msgBuff, msgEnd, msgId);
	if (msgBuff == 0)
		return;
	msgBuff = Net::ASN1Util::PDUParseSeq(msgBuff, msgEnd, &seqType, &seqEnd);
	if (msgBuff == 0)
		return;
	#if defined(VERBOSE)
	printf("LDAPMessage: Type = %d, Id = %d\r\n", seqType, msgId);
	#endif
	switch (seqType)
	{
	case 0x61: //BindResponse
		{
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			UInt32 resultCode;
			msgBuff = Net::ASN1Util::PDUParseChoice(msgBuff, seqEnd, resultCode);
			if (msgBuff == 0)
				return;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb);
			if (msgBuff == 0)
				return;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb2);
			if (msgBuff == 0)
				return;
			#if defined(VERBOSE)
			printf("LDAPMessage: BindResponse, resultCode = %d, matchedDN = %s, errorMessage = %s\r\n", resultCode, sb.ToString(), sb2.ToString());
			#endif
			Sync::MutexUsage mutUsage(this->reqMut);
			req = this->reqMap.Get(msgId);
			if (req)
			{
				req->resultCode = resultCode;
				req->isFin = true;
				this->respEvt.Set();
			}
		}
		break;
	case 0x64: //searchResEntry
		{
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			Net::LDAPClient::SearchResObject *obj;
			const UInt8 *attrEnd;
			const UInt8 *itemEnd;
			const UInt8 *valEnd;
			UInt8 type;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb);
			if (msgBuff == 0)
				return;
			#if defined(VERBOSE)
			Text::StringBuilderUTF8 sb3;
			printf("LDAPMessage: searchResEntry, objectName = %s\r\n", sb.ToString());
			#endif
			obj = MemAlloc(Net::LDAPClient::SearchResObject, 1);
			obj->name = Text::String::New(sb.ToString(), sb.GetLength());
			obj->isRef = false;
			NEW_CLASS(obj->items, Data::ArrayList<Net::LDAPClient::SearchResItem*>());
			msgBuff = Net::ASN1Util::PDUParseSeq(msgBuff, seqEnd, &type, &attrEnd);
			if (msgBuff == 0 || type != 0x30)
			{
				printf("LDAPMessage: searchResEntry, end 1\r\n");
				SearchResObjectFree(obj);
				return;
			}
			while (msgBuff < attrEnd && msgBuff[0] == 0x30)
			{
				msgBuff = Net::ASN1Util::PDUParseSeq(msgBuff, attrEnd, &type, &itemEnd);
				if (msgBuff == 0)
				{
					printf("LDAPMessage: searchResEntry, end 2\r\n");
					SearchResObjectFree(obj);
					return;
				}
				sb.ClearStr();
				msgBuff = Net::ASN1Util::PDUParseString(msgBuff, itemEnd, sb);
				if (msgBuff == 0)
				{
					printf("LDAPMessage: searchResEntry, end 3\r\n");
					SearchResObjectFree(obj);
					return;
				}
				if (msgBuff[0] == 0x31)
				{
					msgBuff = Net::ASN1Util::PDUParseSeq(msgBuff, itemEnd, &type, &valEnd);
					if (msgBuff == 0)
					{
						break;
					}
					while (msgBuff < valEnd)
					{
						sb2.ClearStr();
						msgBuff = Net::ASN1Util::PDUParseString(msgBuff, valEnd, sb2);
						if (msgBuff == 0)
						{
							SearchResObjectFree(obj);
							return;
						}
						#if defined(VERBOSE)
						sb3.ClearStr();
						SearchResDisplay(sb.ToCString(), sb2.ToCString(), sb3);
						printf("LDAPMessage: searchResEntry: -%s = %s\r\n", sb.ToString(), sb3.ToString());
						#endif
						Net::LDAPClient::SearchResItem *item;
						item = MemAlloc(Net::LDAPClient::SearchResItem, 1);
						item->type = Text::String::New(sb.ToCString());
						item->value = Text::String::New(sb2.ToCString()).Ptr();
						obj->items->Add(item);
					}
				}
				else
				{
					printf("LDAPMessage: searchResEntry, end 4\r\n");
					SearchResObjectFree(obj);
					return;
				}
			}
			printf("LDAPMessage: searchResEntry, end 5\r\n");
			Sync::MutexUsage mutUsage(this->reqMut);
			req = this->reqMap.Get(msgId);
			if (req && req->searchObjs)
			{
				req->searchObjs->Add(obj);
			}
			else
			{
				SearchResObjectFree(obj);
			}
		}
		break;
	case 0x65: //searchResDone
		{
			UInt32 resultCode;
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			msgBuff = Net::ASN1Util::PDUParseChoice(msgBuff, seqEnd, resultCode);
			if (msgBuff == 0)
				return;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb);
			if (msgBuff == 0)
				return;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb2);
			if (msgBuff == 0)
				return;
			#if defined(VERBOSE)
			printf("LDAPMessage: searchResDone, resultCode = %d, matchedDN = %s, errorMessage = %s\r\n", resultCode, sb.ToString(), sb2.ToString());
			#endif
			Sync::MutexUsage mutUsage(this->reqMut);
			req = this->reqMap.Get(msgId);
			if (req)
			{
				req->resultCode = resultCode;
				req->isFin = true;
				this->respEvt.Set();
			}
		}
		break;
	case 0x73: //searchResRef
		{
			Text::StringBuilderUTF8 sb;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb);
			if (msgBuff)
			{
				#if defined(VERBOSE)
				printf("LDAPMessage: searchResRef, LDAPURL = %s\r\n", sb.ToString());
				#endif
				Sync::MutexUsage mutUsage(this->reqMut);
				req = this->reqMap.Get(msgId);
				if (req && req->searchObjs)
				{
					Net::LDAPClient::SearchResObject *obj;
					obj = MemAlloc(Net::LDAPClient::SearchResObject, 1);
					obj->isRef = true;
					obj->name = Text::String::New(sb.ToString(), sb.GetLength());
					obj->items = 0;
					req->searchObjs->Add(obj);
				}
			}
		}
		break;
	case 0x78: //extendedResp
		{
			UInt32 resultCode;
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			msgBuff = Net::ASN1Util::PDUParseChoice(msgBuff, seqEnd, resultCode);
			if (msgBuff == 0)
				return;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb);
			if (msgBuff == 0)
				return;
			msgBuff = Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb2);
			if (msgBuff == 0)
				return;
			#if defined(VERBOSE)
			printf("LDAPMessage: extendedResp, resultCode = %d, matchedDN = %s, errorMessage = %s\r\n", resultCode, sb.ToString(), sb2.ToString());
			#endif
		}
		break;
	}
}

const UTF8Char *Net::LDAPClient::ParseFilter(Net::ASN1PDUBuilder *pdu, const UTF8Char *filter, Bool complex)
{
	const UTF8Char *filterStart;
	while (Text::CharUtil::PtrIsWS(&filter));
	if (filter[0] != '(')
	{
		return 0;
	}
	filter++;
	while (Text::CharUtil::PtrIsWS(&filter));
	if (filter[0] == '&')
	{
		pdu->BeginOther(0xA0);
		filter++;

		while (true)
		{
			while (Text::CharUtil::PtrIsWS(&filter));
			if (filter[0] == '(')
			{
				filter = ParseFilter(pdu, filter, true);
				if (filter == 0)
				{
					return 0;
				}
			}
			else if (filter[0] == ')')
			{
				pdu->EndLevel();
				return filter + 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else if (filter[0] == '|')
	{
		pdu->BeginOther(0xA1);
		while (true)
		{
			while (Text::CharUtil::PtrIsWS(&filter));
			if (filter[0] == '(')
			{
				filter = ParseFilter(pdu, filter, true);
				if (filter == 0)
				{
					pdu->EndLevel();
					return 0;
				}
			}
			else if (filter[0] == ')')
			{
				pdu->EndLevel();
				return filter + 1;
			}
			else
			{
				pdu->EndLevel();
				return 0;
			}
		}
	}
	else if (filter[0] == '!')
	{
		while (Text::CharUtil::PtrIsWS(&filter));
		if (filter[0] == ')' || filter[0] == '=' || filter[0] == 0)
		{
			return 0;
		}
		pdu->BeginOther(complex?0xA2:0x82);
		filterStart = filter;
		while (true)
		{
			if (filter[0] == ')' || filter[0] == 0)
			{
				pdu->EndLevel();
				return 0;
			}
			else if (filter[0] == '=')
			{
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				filter++;
				break;
			}
			filter++;
		}
		filterStart = filter;
		while (true)
		{
			if (filter[0] == ')')
			{
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				pdu->EndLevel();
				return filter + 1;
			}
			else if (filter[0] == '=' || filter[0] == 0)
			{
				pdu->EndLevel();
				return 0;
			}
			filter++;
		}
	}
	else if (filter[0] == 0 || filter[0] == ')')
	{
		return 0;
	}
	else
	{
		while (Text::CharUtil::PtrIsWS(&filter));
		if (filter[0] == ')' || filter[0] == '=' || filter[0] == 0)
		{
			return 0;
		}
		filterStart = filter;
		while (true)
		{
			if (filter[0] == ')' || filter[0] == 0)
			{
				pdu->EndLevel();
				return 0;
			}
			else if (filter[0] == '=' && filter[1] == '*' && filter[2] == ')')
			{
				pdu->AppendOther(complex?0xA7:0x87, filterStart, (UOSInt)(filter - filterStart));
				return filter + 3;
			}
			else if (filter[0] == '=' && filter[1] == '*')
			{
				pdu->BeginOther(complex?0xA4:0x84);
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				filter += 2;
				break;
			}
			else if (filter[0] == '=')
			{
				pdu->BeginOther(complex?0xA3:0x83);
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				filter += 1;
				break;
			}
			else if (filter[0] == '>' && filter[1] == '=')
			{
				pdu->BeginOther(complex?0xA5:0x85);
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				filter += 2;
				break;
			}
			else if (filter[0] == '<' && filter[1] == '=')
			{
				pdu->BeginOther(complex?0xA6:0x86);
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				filter += 2;
				break;
			}
			else if (filter[0] == '~' && filter[1] == '=')
			{
				pdu->BeginOther(complex?0xA8:0x88);
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				filter += 2;
				break;
			}
			filter++;
		}
		filterStart = filter;
		while (true)
		{
			if (filter[0] == ')')
			{
				pdu->AppendOctetString(filterStart, (UOSInt)(filter - filterStart));
				pdu->EndLevel();
				return filter + 1;
			}
			else if (filter[0] == '=' || filter[0] == 0)
			{
				pdu->EndLevel();
				return 0;
			}
			filter++;
		}
	}
}

Net::LDAPClient::LDAPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout)
{
	this->sockf = sockf;
	this->recvRunning = false;
	this->recvToStop = false;
	this->lastMsgId = 0;
	NEW_CLASS(this->cli, Net::TCPClient(sockf, addr, port, timeout));
	if (!this->cli->IsConnectError() && !this->cli->IsClosed())
	{
		Sync::ThreadUtil::Create(RecvThread, this);
		while (!this->recvRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
}

Net::LDAPClient::~LDAPClient()
{
	if (this->recvRunning)
	{
		this->recvToStop = true;
		this->cli->Close();
		while (this->recvRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
	DEL_CLASS(this->cli);
}

Bool Net::LDAPClient::IsError()
{
	return this->cli->IsConnectError() || this->cli->IsClosed();
}

Bool Net::LDAPClient::Bind(Text::CString userDN, Text::CString password)
{
	Net::ASN1PDUBuilder *pdu;
	UOSInt buffSize;
	const UInt8 *buff;
	Net::LDAPClient::ReqStatus status;
	Bool valid;
	NEW_CLASS(pdu, ASN1PDUBuilder())
	pdu->BeginSequence();
	Sync::MutexUsage msgIdMutUsage(this->msgIdMut);
	status.msgId = ++(this->lastMsgId);
	pdu->AppendUInt32(status.msgId);
	msgIdMutUsage.EndUse();
	
	pdu->BeginOther(0x60); //BindRequest
	pdu->AppendUInt32(3); //version
	if (userDN.v == 0 || password.v == 0)
	{
		pdu->AppendOctetString(0, 0); //name
		pdu->AppendOther(0x80, 0, 0); //authentication
	}
	else
	{
		pdu->AppendOctetStringC(userDN.OrEmpty()); //name
		pdu->AppendOther(0x80, password.v, password.leng); //authentication
	}
	pdu->EndLevel();

	pdu->EndLevel();
	buff = pdu->GetBuff(buffSize);

	status.isFin = false;
	status.resultCode = 0;
	status.searchObjs = 0;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqMap.Put(status.msgId, &status);
	mutUsage.EndUse();

	valid = (this->cli->Write(buff, buffSize) == buffSize);
	DEL_CLASS(pdu);
	if (valid)
	{
		Manage::HiResClock clk;
		while (!status.isFin)
		{
			this->respEvt.Wait(1000);
			if (clk.GetTimeDiff() >= 1)
			{
				break;
			}
		}
		valid = status.isFin && status.resultCode == 0;
	}
	mutUsage.BeginUse();
	this->reqMap.Remove(status.msgId);
	mutUsage.EndUse();
	return valid;
}

Bool Net::LDAPClient::Unbind()
{
	Net::ASN1PDUBuilder *pdu;
	UOSInt buffSize;
	const UInt8 *buff;
	Bool valid;
	NEW_CLASS(pdu, ASN1PDUBuilder())
	pdu->BeginSequence();
	Sync::MutexUsage msgIdMutUsage(this->msgIdMut);
	pdu->AppendUInt32(++(this->lastMsgId));
	msgIdMutUsage.EndUse();
	pdu->AppendOther(0x42, 0, 0); //UnbindRequest

	pdu->BeginOther(0xA0); //control
	pdu->BeginSequence(); //Control
	pdu->AppendOctetStringC(CSTR("2.16.840.1.113730.3.4.2")); //controlType
	pdu->EndLevel();
	pdu->EndLevel();

	pdu->EndLevel();

	buff = pdu->GetBuff(buffSize);
	valid = (this->cli->Write(buff, buffSize) == buffSize);
	DEL_CLASS(pdu);
	return valid;
}

Bool Net::LDAPClient::Search(Text::CStringNN baseObject, ScopeType scope, DerefType derefAliases, UInt32 sizeLimit, UInt32 timeLimit, Bool typesOnly, const UTF8Char *filter, Data::ArrayList<Net::LDAPClient::SearchResObject*> *results)
{
	Net::ASN1PDUBuilder *pdu;
	UOSInt buffSize;
	const UInt8 *buff;
	Net::LDAPClient::ReqStatus status;
	Data::ArrayList<Net::LDAPClient::SearchResObject*> resObjs;
	Bool valid;
	NEW_CLASS(pdu, ASN1PDUBuilder())
	pdu->BeginSequence();
	Sync::MutexUsage msgIdMutUsage(this->msgIdMut);
	status.msgId = ++(this->lastMsgId);
	pdu->AppendUInt32(status.msgId);
	msgIdMutUsage.EndUse();
	status.searchObjs = &resObjs;

	pdu->BeginOther(0x63); //SearchRequest
	pdu->AppendOctetStringC(baseObject);
	pdu->AppendChoice((UInt32)scope);
	pdu->AppendChoice((UInt32)derefAliases);
	pdu->AppendUInt32(sizeLimit);
	pdu->AppendUInt32(timeLimit);
	pdu->AppendBool(typesOnly);
	if (filter == 0 || filter[0] == 0)
	{
		filter = (const UTF8Char*)"(objectClass=*)";
	}
	filter = ParseFilter(pdu, filter, false);
	if (filter == 0 || filter[0] != 0)
	{
		DEL_CLASS(pdu);
		return false;
	}

	pdu->BeginSequence(); //attributes
	pdu->EndLevel();
	pdu->EndLevel();

	pdu->BeginOther(0xA0); //control
	pdu->BeginSequence(); //Control
	pdu->AppendOctetStringC(CSTR("2.16.840.1.113730.3.4.2")); //controlType
	pdu->EndLevel();
	pdu->EndLevel();

	pdu->EndLevel();

	buff = pdu->GetBuff(buffSize);

	status.isFin = false;
	status.resultCode = 0;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqMap.Put(status.msgId, &status);
	mutUsage.EndUse();

	valid = (this->cli->Write(buff, buffSize) == buffSize);
	DEL_CLASS(pdu);
	if (valid)
	{
		Manage::HiResClock clk;
		while (!status.isFin)
		{
			this->respEvt.Wait(1000);
			if (clk.GetTimeDiff() >= 1)
			{
				break;
			}
		}
		valid = status.isFin && status.resultCode == 0;
	}

	mutUsage.BeginUse();
	this->reqMap.Remove(status.msgId);
	mutUsage.EndUse();
	if (valid)
	{
		results->AddAll(resObjs);
	}
	else
	{
		this->SearchResultsFree(&resObjs);
	}
	return valid;
}

void Net::LDAPClient::SearchResultsFree(Data::ArrayList<Net::LDAPClient::SearchResObject*> *results)
{
	Net::LDAPClient::SearchResObject *obj;
	UOSInt i = results->GetCount();
	while (i-- > 0)
	{
		obj = results->GetItem(i);
		SearchResObjectFree(obj);
	}
	results->Clear();
}

void Net::LDAPClient::SearchResObjectFree(Net::LDAPClient::SearchResObject *obj)
{
	obj->name->Release();
	if (obj->items)
	{
		Net::LDAPClient::SearchResItem *item;
		UOSInt i = obj->items->GetCount();
		while (i-- > 0)
		{
			item = obj->items->GetItem(i);
			item->type->Release();
			item->value->Release();
			MemFree(item);
		}
		DEL_CLASS(obj->items);
	}
	MemFree(obj);
}

void Net::LDAPClient::SearchResDisplay(Text::CString type, Text::CString value, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (type.Equals(UTF8STRC("objectGUID")) || type.EndsWith(UTF8STRC("Guid")))
	{
		sb->AppendHex32(ReadUInt32(&value.v[0]));
		sb->AppendUTF8Char('-');
		sb->AppendHex16(ReadUInt16(&value.v[4]));
		sb->AppendUTF8Char('-');
		sb->AppendHex16(ReadUInt16(&value.v[6]));
		sb->AppendUTF8Char('-');
		sb->AppendHex16(ReadMUInt16(&value.v[8]));
		sb->AppendUTF8Char('-');
		sb->AppendHexBuff(&value.v[10], 6, 0, Text::LineBreakType::None);
	}
	else if (type.Equals(UTF8STRC("dSASignature")))
	{
		sb->AppendHexBuff(value.v, 40, 0, Text::LineBreakType::None);
	}
	else if (type.Equals(UTF8STRC("objectSid")))
	{
		sb->AppendC(UTF8STRC("S-"));
		sb->AppendU16(value.v[0]);
		sb->AppendUTF8Char('-');
		sb->AppendU16(value.v[7]);
		sb->AppendUTF8Char('-');
		sb->AppendU32(ReadUInt32(&value.v[8]));
		sb->AppendUTF8Char('-');
		sb->AppendU32(ReadUInt32(&value.v[12]));
		sb->AppendUTF8Char('-');
		sb->AppendU32(ReadUInt32(&value.v[16]));
		sb->AppendUTF8Char('-');
		sb->AppendU32(ReadUInt32(&value.v[20]));
	}
	else
	{
		sb->Append(value);
	}
}
