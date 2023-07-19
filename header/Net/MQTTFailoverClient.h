#ifndef _SM_NET_MQTTFAILOVERCLIENT
#define _SM_NET_MQTTFAILOVERCLIENT
#include "Data/ArrayList.h"
#include "Net/FailoverHandler.h"
#include "Net/MQTTClient.h"
#include "Net/MQTTConn.h"
#include "Net/MQTTStaticClient.h"

namespace Net
{
	class MQTTFailoverClient : public Net::MQTTClient
	{
	private:
		struct ClientInfo
		{
			Net::MQTTStaticClient *client;
			MQTTFailoverClient *me;
		};
	private:
		Net::FailoverHandler<MQTTStaticClient> foHdlr;
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		UInt16 kaSeconds;
		Sync::Mutex hdlrMut;
		Data::ArrayList<Net::MQTTConn::PublishMessageHdlr> hdlrList;
		Data::ArrayList<void *> hdlrObjList;
		Data::ArrayList<ClientInfo*> cliList;
		
		void FreeClient(ClientInfo *cliInfo);
		static void __stdcall OnMessage(void *userObj, Text::CString topic, const Data::ByteArrayR &buff);
	public:
		MQTTFailoverClient(Net::FailoverType foType, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, UInt16 kaSeconds);
		virtual ~MQTTFailoverClient();

		void AddClient(Text::CString host, UInt16 port, Text::CString username, Text::CString password, Bool webSocket);

		virtual void HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj);
		virtual Bool Subscribe(Text::CString topic);
		virtual Bool Publish(Text::CString topic, Text::CString message);
	};
}
#endif
