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
		const UTF8Char *host;
		UInt16 port;
		const UTF8Char *path;
		Int32 codePage;
		Net::FTPConn *conn;
		Net::TCPClient *cli2;

	public:
		FTPClient(const UTF8Char *url, Net::SocketFactory *sockf, Bool passiveMode, Int32 codePage);
		~FTPClient();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
	};
}
#endif
