#ifndef _SM_NET_FTPCLIENT
#define _SM_NET_FTPCLIENT
#include "IO/Stream.h"
#include "Net/FTPConn.h"
#include "Net/TCPClient.h"
#include "Net/TCPClientFactory.h"

namespace Net
{
	class FTPClient : public IO::Stream
	{
	private:
		const UTF8Char *userName;
		const UTF8Char *password;
		Text::String *host;
		UInt16 port;
		const UTF8Char *path;
		UInt32 codePage;
		Net::FTPConn *conn;
		Optional<Net::TCPClient> cli2;

	public:
		FTPClient(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Bool passiveMode, UInt32 codePage, Data::Duration timeout);
		~FTPClient();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
