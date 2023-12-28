#ifndef _SM_NET_SSHCONN
#define _SM_NET_SSHCONN
#include "Data/ArrayListStringNN.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"

namespace Net
{
	struct SSHChannelHandle;
	class SSHTCPChannel;
	class SSHConn
	{
	private:
		struct ClassData;
		NotNullPtr<ClassData> clsData;
		Optional<Net::TCPClient> cli;
		NotNullPtr<Net::SocketFactory> sockf;
	public:
		SSHConn(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Data::Duration timeout);
		~SSHConn();

		Bool IsError() const;
		NotNullPtr<Net::SocketFactory> GetSocketFactory() const;
		Optional<Net::TCPClient> GetTCPClient() const;

		Bool GetHostKeySHA1(UInt8 *buff); //20 bytes
		Bool GetAuthMethods(Text::CStringNN userName, NotNullPtr<Data::ArrayListStringNN> authMeth);
		Bool AuthPassword(Text::CStringNN userName, Text::CStringNN password);
		Optional<SSHTCPChannel> RemoteConnect(Socket *sourceSoc, Text::CStringNN remoteHost, UInt16 remotePort);
		Bool ChannelTryRead(SSHChannelHandle *channel, UInt8 *buff, UOSInt maxSize, OutParam<UOSInt> size);
		UOSInt ChannelWrite(SSHChannelHandle *channel, const UInt8 *buff, UOSInt size);
		void ChannelClose(SSHChannelHandle *channel);
		void Close();
	};
}
#endif
