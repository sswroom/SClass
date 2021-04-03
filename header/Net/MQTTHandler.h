#ifndef _SM_NET_MQTTHANDLER
#define _SM_NET_MQTTHANDLER
#include "Net/MQTTClient.h"
#include "Sync/Event.h"

namespace Net
{
	class MQTTHandler
	{
	private:
		Net::MQTTClient *client;
		UInt32 kaSeconds;
		Bool kaRunning;
		Bool kaToStop;
		Sync::Event *kaEvt;

		static UInt32 __stdcall KAThread(void *userObj);
	public:
		MQTTHandler(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UTF8Char *username, const UTF8Char *password, Net::MQTTClient::PublishMessageHdlr hdlr, void *userObj, UInt32 kaSeconds);
		~MQTTHandler();

		Bool IsError();

		Bool Subscribe(const UTF8Char *topic);
		Bool Publish(const UTF8Char *topic, const UTF8Char *message);
	};
}
#endif
