#ifndef _SM_NET_MQTTCONN
#define _SM_NET_MQTTCONN
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ByteArray.h"
#include "Data/CallbackStorage.h"
#include "IO/ProtoHdlr/ProtoMQTTHandler.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace Net
{
	class MQTTConn : public IO::IProtocolHandler::DataListener
	{
	public:
		typedef enum
		{
			CS_TIMEDOUT = -1,
			CS_ACCEPTED = 0,
			CS_VERERR = 1,
			CS_CLIENTID_ERR = 2,
			CS_SVR_UNAVAIL = 3,
			CS_BAD_LOGIN = 4,
			CS_NOT_AUTH = 5
		} ConnectStatus;

		typedef struct
		{
			UInt8 packetType;
			UOSInt size;
			UInt8 content[1];
		} PacketInfo;

		typedef void (__stdcall *PublishMessageHdlr)(AnyType userObj, Text::CString topic, const Data::ByteArrayR &buff);
		typedef void (__stdcall *DisconnectHdlr)(AnyType userObj);
	private:
		IO::ProtoHdlr::ProtoMQTTHandler protoHdlr;
		IO::Stream *stm;
		AnyType cliData;
		Bool recvRunning;
		Bool recvStarted;

		Data::ArrayList<Data::CallbackStorage<PublishMessageHdlr>> hdlrList;
		Data::CallbackStorage<DisconnectHdlr> discHdlr;

		Data::ArrayList<PacketInfo *> packetList;
		Sync::Mutex packetMut;
		Sync::Event packetEvt;
		Sync::Mutex cliMut;
		UInt64 totalUpload;
		UInt64 totalDownload;

		virtual void DataParsed(NotNullPtr<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(NotNullPtr<IO::Stream> stm, AnyType stmObj, const UInt8 *buff, UOSInt buffSize);
		static UInt32 __stdcall RecvThread(AnyType userObj);

		void OnPublishMessage(Text::CString topic, const UInt8 *message, UOSInt msgSize);
		PacketInfo *GetNextPacket(UInt8 packetType, Data::Duration timeout);
		Bool SendPacket(const UInt8 *packet, UOSInt packetSize);

		void InitStream(NotNullPtr<IO::Stream> stm);
	public:
		MQTTConn(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, DisconnectHdlr discHdlr, AnyType discHdlrObj, Data::Duration timeout);
		MQTTConn(NotNullPtr<IO::Stream> stm, DisconnectHdlr discHdlr, AnyType discHdlrObj);
		virtual ~MQTTConn();

		void HandlePublishMessage(PublishMessageHdlr hdlr, AnyType userObj);
		Bool IsError();

		Bool SendConnect(UInt8 protoVer, UInt16 keepAliveS, Text::CString clientId, Text::CString userName, Text::CString password);
		Bool SendPublish(Text::CString topic, Text::CString message);
		Bool SendPubAck(UInt16 packetId);
		Bool SendPubRec(UInt16 packetId);
		Bool SendSubscribe(UInt16 packetId, Text::CString topic);
		Bool SendPing();
		Bool SendDisconnect();

		ConnectStatus WaitConnAck(Data::Duration timeout);
		UInt8 WaitSubAck(UInt16 packetId, Data::Duration timeout); //0x80 = failure
		void ClearPackets();

		UInt64 GetTotalUpload();
		UInt64 GetTotalDownload();

		static Bool PublishMessage(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Text::CString username, Text::CString password, Text::CString topic, Text::CString message, Data::Duration timeout);
	};
}
#endif
