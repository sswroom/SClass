#ifndef _SM_NET_LDAPCLIENT
#define _SM_NET_LDAPCLIENT
#include "AnyType.h"
#include "Data/FastMapNN.hpp"
#include "Net/ASN1PDUBuilder.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class LDAPClient
	{
	public:
		typedef enum
		{
			ST_BASE_OBJECT,
			ST_SINGLE_LEVEL,
			ST_WHOLE_SUBTREE
		} ScopeType;

		typedef enum
		{
			DT_NEVER_DEREF_ALIASES,
			DT_DEREF_IN_SEARCHING,
			DT_DEREF_FINDING_BASE_OBJECT,
			DT_DEREF_ALWAYS
		} DerefType;

		typedef struct
		{
			NN<Text::String> type;
			Text::String *value;
		} SearchResItem;

		typedef struct
		{
			NN<Text::String> name;
			Bool isRef;
			Optional<Data::ArrayListNN<SearchResItem>> items;
		} SearchResObject;

	private:
		typedef struct
		{
			UInt32 msgId;
			Bool isFin;
			UInt32 resultCode;
			Optional<Data::ArrayListNN<SearchResObject>> searchObjs;
		} ReqStatus;

	private:
		NN<Net::SocketFactory> sockf;
		Net::TCPClient *cli;
		
		Sync::Mutex reqMut;
		Data::FastMapNN<UInt32, ReqStatus> reqMap;

		Sync::Mutex msgIdMut;
		UInt32 lastMsgId;

		Sync::Event respEvt;
		Bool recvRunning;
		Bool recvToStop;

		static UInt32 __stdcall RecvThread(AnyType userObj);
		void ParseLDAPMessage(UnsafeArray<const UInt8> msgBuff, UOSInt msgLen);
		UnsafeArrayOpt<const UTF8Char> ParseFilter(Net::ASN1PDUBuilder *pdu, UnsafeArray<const UTF8Char> filter, Bool complex);
	public:
		LDAPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout);
		~LDAPClient();

		Bool IsError();

		Bool Bind(Text::CString userDN, Text::CString password);
		Bool Unbind();

		Bool Search(Text::CStringNN baseObject, ScopeType scope, DerefType derefAliases, UInt32 sizeLimit, UInt32 timeLimit, Bool typesOnly, UnsafeArrayOpt<const UTF8Char> filter, NN<Data::ArrayListNN<SearchResObject>> results);

		static void __stdcall SearchResultsFree(NN<Data::ArrayListNN<SearchResObject>> results);
		static void __stdcall SearchResObjectFree(NN<SearchResObject> obj);
		static void SearchResDisplay(Text::CStringNN type, Text::CStringNN value, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
