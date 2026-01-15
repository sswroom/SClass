#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
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

UInt32 __stdcall Net::LDAPClient::RecvThread(AnyType userObj)
{
	NN<Net::LDAPClient> me = userObj.GetNN<Net::LDAPClient>();
	UIntOS buffSize;
	UIntOS recvSize;
	UIntOS i;
	UIntOS j;
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
			printf("%s\r\n", sb.ToPtr());
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

void Net::LDAPClient::ParseLDAPMessage(UnsafeArray<const UInt8> msgBuff, UIntOS msgLen)
{
	UInt32 msgId;
	UnsafeArray<const UInt8> msgEnd = msgBuff + msgLen;
	UInt8 seqType;
	UnsafeArray<const UInt8> seqEnd;
	NN<Net::LDAPClient::ReqStatus> req;
	NN<Data::ArrayListNN<SearchResObject>> searchObjs;

	if (!Net::ASN1Util::PDUParseUInt32(msgBuff, msgEnd, msgId).SetTo(msgBuff))
		return;
	if (!Net::ASN1Util::PDUParseSeq(msgBuff, msgEnd, seqType, seqEnd).SetTo(msgBuff))
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
			if (!Net::ASN1Util::PDUParseChoice(msgBuff, seqEnd, resultCode).SetTo(msgBuff))
				return;
			if (!Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb).SetTo(msgBuff))
				return;
			if (!Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb2).SetTo(msgBuff))
				return;
			#if defined(VERBOSE)
			printf("LDAPMessage: BindResponse, resultCode = %d, matchedDN = %s, errorMessage = %s\r\n", resultCode, sb.ToPtr(), sb2.ToPtr());
			#endif
			Sync::MutexUsage mutUsage(this->reqMut);
			if (this->reqMap.Get(msgId).SetTo(req))
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
			NN<Net::LDAPClient::SearchResObject> obj;
			NN<Data::ArrayListNN<Net::LDAPClient::SearchResItem>> items;
			UnsafeArray<const UInt8> attrEnd;
			UnsafeArray<const UInt8> itemEnd;
			UnsafeArray<const UInt8> valEnd;
			UInt8 type;
			if (!Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb).SetTo(msgBuff))
				return;
			#if defined(VERBOSE)
			Text::StringBuilderUTF8 sb3;
			printf("LDAPMessage: searchResEntry, objectName = %s\r\n", sb.ToPtr());
			#endif
			obj = MemAllocNN(Net::LDAPClient::SearchResObject);
			obj->name = Text::String::New(sb.ToString(), sb.GetLength());
			obj->isRef = false;
			NEW_CLASSNN(items, Data::ArrayListNN<Net::LDAPClient::SearchResItem>());
			obj->items = items;
			if (!Net::ASN1Util::PDUParseSeq(msgBuff, seqEnd, type, attrEnd).SetTo(msgBuff) || type != 0x30)
			{
				printf("LDAPMessage: searchResEntry, end 1\r\n");
				SearchResObjectFree(obj);
				return;
			}
			while (msgBuff < attrEnd && msgBuff[0] == 0x30)
			{
				if (!Net::ASN1Util::PDUParseSeq(msgBuff, attrEnd, type, itemEnd).SetTo(msgBuff))
				{
					printf("LDAPMessage: searchResEntry, end 2\r\n");
					SearchResObjectFree(obj);
					return;
				}
				sb.ClearStr();
				if (!Net::ASN1Util::PDUParseString(msgBuff, itemEnd, sb).SetTo(msgBuff))
				{
					printf("LDAPMessage: searchResEntry, end 3\r\n");
					SearchResObjectFree(obj);
					return;
				}
				if (msgBuff[0] == 0x31)
				{
					if (!Net::ASN1Util::PDUParseSeq(msgBuff, itemEnd, type, valEnd).SetTo(msgBuff))
					{
						break;
					}
					while (msgBuff < valEnd)
					{
						sb2.ClearStr();
						if (!Net::ASN1Util::PDUParseString(msgBuff, valEnd, sb2).SetTo(msgBuff))
						{
							SearchResObjectFree(obj);
							return;
						}
						#if defined(VERBOSE)
						sb3.ClearStr();
						SearchResDisplay(sb.ToCString(), sb2.ToCString(), sb3);
						printf("LDAPMessage: searchResEntry: -%s = %s\r\n", sb.ToPtr(), sb3.ToPtr());
						#endif
						NN<Net::LDAPClient::SearchResItem> item;
						item = MemAllocNN(Net::LDAPClient::SearchResItem);
						item->type = Text::String::New(sb.ToCString());
						item->value = Text::String::New(sb2.ToCString()).Ptr();
						items->Add(item);
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
			if (this->reqMap.Get(msgId).SetTo(req) && req->searchObjs.SetTo(searchObjs))
			{
				searchObjs->Add(obj);
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
			if (!Net::ASN1Util::PDUParseChoice(msgBuff, seqEnd, resultCode).SetTo(msgBuff))
				return;
			if (!Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb).SetTo(msgBuff))
				return;
			if (!Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb2).SetTo(msgBuff))
				return;
			#if defined(VERBOSE)
			printf("LDAPMessage: searchResDone, resultCode = %d, matchedDN = %s, errorMessage = %s\r\n", resultCode, sb.ToPtr(), sb2.ToPtr());
			#endif
			Sync::MutexUsage mutUsage(this->reqMut);
			if (this->reqMap.Get(msgId).SetTo(req))
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
			if (Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb).SetTo(msgBuff))
			{
				#if defined(VERBOSE)
				printf("LDAPMessage: searchResRef, LDAPURL = %s\r\n", sb.ToPtr());
				#endif
				Sync::MutexUsage mutUsage(this->reqMut);
				if (this->reqMap.Get(msgId).SetTo(req) && req->searchObjs.SetTo(searchObjs))
				{
					NN<Net::LDAPClient::SearchResObject> obj;
					obj = MemAllocNN(Net::LDAPClient::SearchResObject);
					obj->isRef = true;
					obj->name = Text::String::New(sb.ToString(), sb.GetLength());
					obj->items = nullptr;
					searchObjs->Add(obj);
				}
			}
		}
		break;
	case 0x78: //extendedResp
		{
			UInt32 resultCode;
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			if (!Net::ASN1Util::PDUParseChoice(msgBuff, seqEnd, resultCode).SetTo(msgBuff))
				return;
			if (!Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb).SetTo(msgBuff))
				return;
			if (!Net::ASN1Util::PDUParseString(msgBuff, seqEnd, sb2).SetTo(msgBuff))
				return;
			#if defined(VERBOSE)
			printf("LDAPMessage: extendedResp, resultCode = %d, matchedDN = %s, errorMessage = %s\r\n", resultCode, sb.ToPtr(), sb2.ToPtr());
			#endif
		}
		break;
	}
}

UnsafeArrayOpt<const UTF8Char> Net::LDAPClient::ParseFilter(Net::ASN1PDUBuilder *pdu, UnsafeArray<const UTF8Char> filter, Bool complex)
{
	UnsafeArray<const UTF8Char> filterStart;
	while (Text::CharUtil::PtrIsWS(filter));
	if (filter[0] != '(')
	{
		return nullptr;
	}
	filter++;
	while (Text::CharUtil::PtrIsWS(filter));
	if (filter[0] == '&')
	{
		pdu->BeginOther(0xA0);
		filter++;

		while (true)
		{
			while (Text::CharUtil::PtrIsWS(filter));
			if (filter[0] == '(')
			{
				if (!ParseFilter(pdu, filter, true).SetTo(filter))
				{
					return nullptr;
				}
			}
			else if (filter[0] == ')')
			{
				pdu->EndLevel();
				return filter + 1;
			}
			else
			{
				return nullptr;
			}
		}
	}
	else if (filter[0] == '|')
	{
		pdu->BeginOther(0xA1);
		while (true)
		{
			while (Text::CharUtil::PtrIsWS(filter));
			if (filter[0] == '(')
			{
				if (!ParseFilter(pdu, filter, true).SetTo(filter))
				{
					pdu->EndLevel();
					return nullptr;
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
				return nullptr;
			}
		}
	}
	else if (filter[0] == '!')
	{
		while (Text::CharUtil::PtrIsWS(filter));
		if (filter[0] == ')' || filter[0] == '=' || filter[0] == 0)
		{
			return nullptr;
		}
		pdu->BeginOther(complex?0xA2:0x82);
		filterStart = filter;
		while (true)
		{
			if (filter[0] == ')' || filter[0] == 0)
			{
				pdu->EndLevel();
				return nullptr;
			}
			else if (filter[0] == '=')
			{
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
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
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
				pdu->EndLevel();
				return filter + 1;
			}
			else if (filter[0] == '=' || filter[0] == 0)
			{
				pdu->EndLevel();
				return nullptr;
			}
			filter++;
		}
	}
	else if (filter[0] == 0 || filter[0] == ')')
	{
		return nullptr;
	}
	else
	{
		while (Text::CharUtil::PtrIsWS(filter));
		if (filter[0] == ')' || filter[0] == '=' || filter[0] == 0)
		{
			return nullptr;
		}
		filterStart = filter;
		while (true)
		{
			if (filter[0] == ')' || filter[0] == 0)
			{
				pdu->EndLevel();
				return nullptr;
			}
			else if (filter[0] == '=' && filter[1] == '*' && filter[2] == ')')
			{
				pdu->AppendOther(complex?0xA7:0x87, filterStart, (UIntOS)(filter - filterStart));
				return filter + 3;
			}
			else if (filter[0] == '=' && filter[1] == '*')
			{
				pdu->BeginOther(complex?0xA4:0x84);
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
				filter += 2;
				break;
			}
			else if (filter[0] == '=')
			{
				pdu->BeginOther(complex?0xA3:0x83);
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
				filter += 1;
				break;
			}
			else if (filter[0] == '>' && filter[1] == '=')
			{
				pdu->BeginOther(complex?0xA5:0x85);
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
				filter += 2;
				break;
			}
			else if (filter[0] == '<' && filter[1] == '=')
			{
				pdu->BeginOther(complex?0xA6:0x86);
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
				filter += 2;
				break;
			}
			else if (filter[0] == '~' && filter[1] == '=')
			{
				pdu->BeginOther(complex?0xA8:0x88);
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
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
				pdu->AppendOctetString(filterStart, (UIntOS)(filter - filterStart));
				pdu->EndLevel();
				return filter + 1;
			}
			else if (filter[0] == '=' || filter[0] == 0)
			{
				pdu->EndLevel();
				return nullptr;
			}
			filter++;
		}
	}
}

Net::LDAPClient::LDAPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout)
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
	UIntOS buffSize;
	UnsafeArray<const UInt8> buff;
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
	Text::CStringNN nnuserDN;
	Text::CStringNN nnpassword;
	if (!userDN.SetTo(nnuserDN) || !password.SetTo(nnpassword))
	{
		pdu->AppendOctetString(U8STR(""), 0); //name
		pdu->AppendOther(0x80, U8STR(""), 0); //authentication
	}
	else
	{
		pdu->AppendOctetStringC(nnuserDN); //name
		pdu->AppendOther(0x80, nnpassword.v, nnpassword.leng); //authentication
	}
	pdu->EndLevel();

	pdu->EndLevel();
	buff = pdu->GetBuff(buffSize);

	status.isFin = false;
	status.resultCode = 0;
	status.searchObjs = nullptr;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqMap.Put(status.msgId, status);
	mutUsage.EndUse();

	valid = (this->cli->Write(Data::ByteArrayR(buff, buffSize)) == buffSize);
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
	UIntOS buffSize;
	UnsafeArray<const UInt8> buff;
	Bool valid;
	NEW_CLASS(pdu, ASN1PDUBuilder())
	pdu->BeginSequence();
	Sync::MutexUsage msgIdMutUsage(this->msgIdMut);
	pdu->AppendUInt32(++(this->lastMsgId));
	msgIdMutUsage.EndUse();
	pdu->AppendOther(0x42, U8STR(""), 0); //UnbindRequest

	pdu->BeginOther(0xA0); //control
	pdu->BeginSequence(); //Control
	pdu->AppendOctetStringC(CSTR("2.16.840.1.113730.3.4.2")); //controlType
	pdu->EndLevel();
	pdu->EndLevel();

	pdu->EndLevel();

	buff = pdu->GetBuff(buffSize);
	valid = (this->cli->Write(Data::ByteArrayR(buff, buffSize)) == buffSize);
	DEL_CLASS(pdu);
	return valid;
}

Bool Net::LDAPClient::Search(Text::CStringNN baseObject, ScopeType scope, DerefType derefAliases, UInt32 sizeLimit, UInt32 timeLimit, Bool typesOnly, UnsafeArrayOpt<const UTF8Char> filter, NN<Data::ArrayListNN<Net::LDAPClient::SearchResObject>> results)
{
	Net::ASN1PDUBuilder *pdu;
	UIntOS buffSize;
	UnsafeArray<const UInt8> buff;
	Net::LDAPClient::ReqStatus status;
	Data::ArrayListNN<Net::LDAPClient::SearchResObject> resObjs;
	Bool valid;
	NEW_CLASS(pdu, ASN1PDUBuilder())
	pdu->BeginSequence();
	Sync::MutexUsage msgIdMutUsage(this->msgIdMut);
	status.msgId = ++(this->lastMsgId);
	pdu->AppendUInt32(status.msgId);
	msgIdMutUsage.EndUse();
	status.searchObjs = resObjs;

	pdu->BeginOther(0x63); //SearchRequest
	pdu->AppendOctetStringC(baseObject);
	pdu->AppendChoice((UInt32)scope);
	pdu->AppendChoice((UInt32)derefAliases);
	pdu->AppendUInt32(sizeLimit);
	pdu->AppendUInt32(timeLimit);
	pdu->AppendBool(typesOnly);
	UnsafeArray<const UTF8Char> nnfilter;
	if (!filter.SetTo(nnfilter) || nnfilter[0] == 0)
	{
		nnfilter = U8STR("(objectClass=*)");
	}
	if (!ParseFilter(pdu, nnfilter, false).SetTo(nnfilter) || nnfilter[0] != 0)
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
	this->reqMap.Put(status.msgId, status);
	mutUsage.EndUse();

	valid = (this->cli->Write(Data::ByteArrayR(buff, buffSize)) == buffSize);
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
		this->SearchResultsFree(resObjs);
	}
	return valid;
}

void __stdcall Net::LDAPClient::SearchResultsFree(NN<Data::ArrayListNN<Net::LDAPClient::SearchResObject>> results)
{
	results->FreeAll(SearchResObjectFree);
}

void __stdcall Net::LDAPClient::SearchResObjectFree(NN<Net::LDAPClient::SearchResObject> obj)
{
	obj->name->Release();
	NN<Data::ArrayListNN<SearchResItem>> items;
	if (obj->items.SetTo(items))
	{
		NN<Net::LDAPClient::SearchResItem> item;
		UIntOS i = items->GetCount();
		while (i-- > 0)
		{
			item = items->GetItemNoCheck(i);
			item->type->Release();
			item->value->Release();
			MemFreeNN(item);
		}
		obj->items.Delete();
	}
	MemFreeNN(obj);
}

void Net::LDAPClient::SearchResDisplay(Text::CStringNN type, Text::CStringNN value, NN<Text::StringBuilderUTF8> sb)
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
