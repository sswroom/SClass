#ifndef _SM_NET_LOGGEDSOCKETFACTORY
#define _SM_NET_LOGGEDSOCKETFACTORY
#include "IO/LogTool.h"
#include "Net/SocketFactory.h"
#include "Text/String.h"

namespace Net
{
	class LoggedSocketFactory : public Net::SocketFactory
	{
	private:
		NN<Net::SocketFactory> sockf;
		NN<IO::LogTool> log;
		Optional<Text::String> logPrefix;
	public:
		LoggedSocketFactory(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, Text::CString logPrefix);
		virtual ~LoggedSocketFactory();

		virtual Optional<Socket> CreateTCPSocketv4();
		virtual Optional<Socket> CreateTCPSocketv6();
		virtual Optional<Socket> CreateUDPSocketv4();
		virtual void DestroySocket(NN<Socket> socket);
		virtual Bool SocketIsInvalid(NN<Socket> socket);
		virtual Bool SocketBindv4(NN<Socket> socket, UInt32 ip, UInt16 port);
		virtual Bool SocketListen(NN<Socket> socket);
		virtual Optional<Socket> SocketAccept(NN<Socket> socket);

		virtual UIntOS SendData(NN<Socket> socket, UnsafeArray<const UInt8> buff, UIntOS buffSize, OptOut<ErrorType> et);
		virtual UIntOS ReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UIntOS buffSize, OptOut<ErrorType> et);
		virtual Optional<SocketRecvSess> BeginReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Sync::Event> evt, OptOut<ErrorType> et);
		virtual UIntOS EndReceiveData(NN<SocketRecvSess> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelReceiveData(NN<SocketRecvSess> reqData);

		virtual UIntOS UDPReceive(NN<Socket> socket, UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et);
		virtual UIntOS SendTo(NN<Socket> socket, UnsafeArray<const UInt8> buff, UIntOS buffSize, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port);

		virtual Bool Connect(NN<Socket> socket, UInt32 ip, UInt16 port, Data::Duration timeout);
		virtual Bool BindSocketIPv4(NN<Socket> socket, const Char *ip, UInt16 port);
		virtual void ShutdownSend(NN<Socket> socket);
		virtual void ShutdownSocket(NN<Socket> socket);
	};
}
#endif
