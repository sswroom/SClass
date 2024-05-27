#ifndef _SM_NET_SSHCONN
#define _SM_NET_SSHCONN
#include "Data/ArrayListStringNN.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"

namespace Net
{
	struct SSHChannelHandle;
	class SSHTCPChannel;

	enum class SSHMethodType
	{
		KeyExchange,
		HostKey,
		CryptoClientToServer,
		CryptoServerToClient,
		MACClientToServer,
		MACServerToClient,
		CompressClientToServer,
		CompressServerToClient,
		LangClientToServer,
		LangServerToClient,
		SignatureAlgorithm
	};

	class SSHConn
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;
		Optional<Net::TCPClient> cli;
		NN<Net::SocketFactory> sockf;
		Sync::Mutex mut;
		Int32 lastError;
	public:
		SSHConn(NN<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Data::Duration timeout);
		~SSHConn();

		Bool IsError() const;
		Int32 GetLastError() const;
		NN<Net::SocketFactory> GetSocketFactory() const;
		Optional<Net::TCPClient> GetTCPClient() const;

		Bool GetHostKeySHA1(UInt8 *buff); //20 bytes
		const UTF8Char *GetBanner();
		const UTF8Char *GetActiveAlgorithm(SSHMethodType method);
		Bool GetAuthMethods(Text::CStringNN userName, NN<Data::ArrayListStringNN> authMeth);
		Bool AuthPassword(Text::CStringNN userName, Text::CStringNN password);
		Optional<SSHTCPChannel> RemoteConnect(Optional<Socket> sourceSoc, Text::CStringNN remoteHost, UInt16 remotePort);
		Bool ChannelTryRead(SSHChannelHandle *channel, UnsafeArray<UInt8> buff, UOSInt maxSize, OutParam<UOSInt> size);
		UOSInt ChannelWrite(SSHChannelHandle *channel, const UInt8 *buff, UOSInt size);
		void ChannelClose(SSHChannelHandle *channel);
		void Close();
	};
}
#endif
