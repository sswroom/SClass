#ifndef _SM_NET_MQTTSTATICCLIENT
#define _SM_NET_MQTTSTATICCLIENT
#include "Net/MQTTConn.h"
#include "Sync/Event.h"

namespace Net
{
	class MQTTStaticClient
	{
	private:
		Net::MQTTConn *conn;
		UInt32 kaSeconds;
		Bool kaRunning;
		Bool kaToStop;
		Sync::Event *kaEvt;

		static UInt32 __stdcall KAThread(void *userObj);
	public:
		MQTTStaticClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UTF8Char *username, const UTF8Char *password, Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, UInt16 kaSeconds);
		~MQTTStaticClient();

		Bool IsError();

		Bool Subscribe(const UTF8Char *topic);
		Bool Publish(const UTF8Char *topic, const UTF8Char *message);
	};
}
#endif
