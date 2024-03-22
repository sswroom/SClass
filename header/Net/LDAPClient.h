#ifndef _SM_NET_LDAPCLIENT
#define _SM_NET_LDAPCLIENT
#include "AnyType.h"
#include "Data/FastMap.h"
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
			NotNullPtr<Text::String> type;
			Text::String *value;
		} SearchResItem;

		typedef struct
		{
			NotNullPtr<Text::String> name;
			Bool isRef;
			Data::ArrayList<SearchResItem*> *items;
		} SearchResObject;

	private:
		typedef struct
		{
			UInt32 msgId;
			Bool isFin;
			UInt32 resultCode;
			Data::ArrayList<SearchResObject *> *searchObjs;
		} ReqStatus;

	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::TCPClient *cli;
		
		Sync::Mutex reqMut;
		Data::FastMap<UInt32, ReqStatus*> reqMap;

		Sync::Mutex msgIdMut;
		UInt32 lastMsgId;

		Sync::Event respEvt;
		Bool recvRunning;
		Bool recvToStop;

		static UInt32 __stdcall RecvThread(AnyType userObj);
		void ParseLDAPMessage(const UInt8 *msgBuff, UOSInt msgLen);
		const UTF8Char *ParseFilter(Net::ASN1PDUBuilder *pdu, const UTF8Char *filter, Bool complex);
	public:
		LDAPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout);
		~LDAPClient();

		Bool IsError();

		Bool Bind(Text::CString userDN, Text::CString password);
		Bool Unbind();

		Bool Search(Text::CStringNN baseObject, ScopeType scope, DerefType derefAliases, UInt32 sizeLimit, UInt32 timeLimit, Bool typesOnly, const UTF8Char *filter, Data::ArrayList<SearchResObject*> *results);

		static void SearchResultsFree(Data::ArrayList<SearchResObject*> *results);
		static void SearchResObjectFree(SearchResObject *obj);
		static void SearchResDisplay(Text::CString type, Text::CString value, NotNullPtr<Text::StringBuilderUTF8> sb);
	};
}
#endif
