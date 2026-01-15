#ifndef _SM_NET_MQTTFAILOVERCLIENT
#define _SM_NET_MQTTFAILOVERCLIENT
#include "Data/ArrayListNN.hpp"
#include "Data/CallbackStorage.h"
#include "Net/FailoverHandler.hpp"
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
			NN<Net::MQTTStaticClient> client;
			MQTTFailoverClient *me;
		};
	private:
		Net::FailoverHandler<MQTTStaticClient> foHdlr;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		UInt16 kaSeconds;
		Sync::Mutex hdlrMut;
		Data::ArrayListObj<Data::CallbackStorage<Net::MQTTConn::PublishMessageHdlr>> hdlrList;
		Data::ArrayListNN<ClientInfo> cliList;
		
		static void __stdcall FreeClient(NN<ClientInfo> cliInfo);
		static void __stdcall OnMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &buff);
	public:
		MQTTFailoverClient(Net::FailoverType foType, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 kaSeconds);
		virtual ~MQTTFailoverClient();

		void AddClient(Text::CStringNN host, UInt16 port, Text::CString username, Text::CString password, Bool webSocket);

		virtual void HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, AnyType hdlrObj);
		virtual Bool Subscribe(Text::CStringNN topic);
		virtual Bool Publish(Text::CStringNN topic, Text::CStringNN message, Bool dup, UInt8 qos, Bool retain);
	};
}
#endif
