#ifndef _SM_NET_FTPCLIENT
#define _SM_NET_FTPCLIENT
#include "IO/Stream.h"
#include "Net/FTPConn.h"
#include "Net/TCPClient.h"
#include "Net/SocketFactory.h"

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
		Net::TCPClient *cli2;

	public:
		FTPClient(Text::CString url, Net::SocketFactory *sockf, Bool passiveMode, UInt32 codePage, Data::Duration timeout);
		~FTPClient();

		virtual Bool IsDown() const;
		virtual UOSInt Read(Data::ByteArray buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
