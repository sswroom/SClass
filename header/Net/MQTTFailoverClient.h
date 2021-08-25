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
		Net::FailoverHandler<MQTTStaticClient> *foHdlr;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		UInt16 kaSeconds;
		Sync::Mutex *hdlrMut;
		Data::ArrayList<Net::MQTTConn::PublishMessageHdlr> *hdlrList;
		Data::ArrayList<void *> *hdlrObjList;
		Data::ArrayList<ClientInfo*> *cliList;
	public:
		MQTTFailoverClient(Net::FailoverHandler::FailoverType foType, Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 kaSeconds);
		virtual ~MQTTFailoverClient();

		void AddClient(const UTF8Char *host, UInt16 port, const UTF8Char *username, const UTF8Char *password);

		virtual void HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj);
		virtual Bool Subscribe(const UTF8Char *topic);
		virtual Bool Publish(const UTF8Char *topic, const UTF8Char *message);
	};
}
#endif
