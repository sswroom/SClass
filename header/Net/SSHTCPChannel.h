#ifndef _SM_NET_SSHTCPCHANNEL
#define _SM_NET_SSHTCPCHANNEL
#include "Net/SSHConn.h"

namespace Net
{
	class SSHTCPChannel : public IO::Stream
	{
	private:
		NN<Net::SSHConn> conn;
		Optional<SSHChannelHandle> channel;
	public:
		SSHTCPChannel(NN<Net::SSHConn> conn, NN<SSHChannelHandle> channel, Text::CStringNN channelName);
		virtual ~SSHTCPChannel();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
		Bool TryRead(UInt8 *buff, UOSInt maxSize, OutParam<UOSInt> size);
	};
}
#endif
