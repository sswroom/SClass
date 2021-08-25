#ifndef _SM_NET_MQTTSTATICCLIENT
#define _SM_NET_MQTTSTATICCLIENT
#include "Net/MQTTConn.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class MQTTStaticClient
	{
	private:
		Sync::Mutex *connMut;
		Net::MQTTConn *conn;
		UInt16 kaSeconds;
		Bool kaRunning;
		Bool kaToStop;
		Sync::Event *kaEvt;

		const UTF8Char *clientId;
		Net::MQTTConn::PublishMessageHdlr hdlr;
		void *hdlrObj;

		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		const UTF8Char *host;
		UInt16 port;
		const UTF8Char *username;
		const UTF8Char *password;

		static UInt32 __stdcall KAThread(void *userObj);
		static void __stdcall OnDisconnect(void *user);
		void Connect();
	public:
		MQTTStaticClient(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj);
		MQTTStaticClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, const UTF8Char *username, const UTF8Char *password, Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, UInt16 kaSeconds);
		~MQTTStaticClient();

		Bool IsStarted();
		Bool ChannelFailure();

		Bool Subscribe(const UTF8Char *topic);
		Bool Publish(const UTF8Char *topic, const UTF8Char *message);
	};
}
#endif
