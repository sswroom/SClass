#ifndef _SM_NET_SSLSERVER
#define _SM_NET_SSLSERVER
#include "Net/TCPClient.h"

namespace Net
{
	class SSLServer
	{
	public:
		typedef enum
		{
			M_SSLV3,
			M_SSLV23,
			M_TLS,
			M_TLSV1,
			M_TLSV1_1,
			M_TLSV1_2,
			M_DTLS,
			M_DTLSV1,
			M_DTLSV1_2
		} Method;
	private:
		struct ClassData;
		struct ClassData *clsData;
		Net::SocketFactory *sockf;

	public:
		SSLServer(Net::SocketFactory *sockf, Method method, const UTF8Char *certFile, const UTF8Char *keyFile);
		~SSLServer();
		
		Bool IsError();
		UTF8Char *GetErrorDetail(UTF8Char *sbuff);
		Net::TCPClient *CreateClient(UInt32 *s);
	};
}
#endif
