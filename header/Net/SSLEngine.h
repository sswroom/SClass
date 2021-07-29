#ifndef _SM_NET_SSLENGINE
#define _SM_NET_SSLENGINE
#include "Net/TCPClient.h"

namespace Net
{
	class SSLEngine
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

		typedef enum
		{
			ET_NONE,
			ET_HOSTNAME_NOT_RESOLVED,
			ET_OUT_OF_MEMORY,
			ET_CANNOT_CONNECT,
			ET_INIT_SESSION,
			ET_CERT_NOT_FOUND,
			ET_INVALID_NAME,
			ET_SELF_SIGN,
			ET_INVALID_PERIOD
		} ErrorType;
	protected:
		Net::SocketFactory *sockf;

		SSLEngine(Net::SocketFactory *sockf);
	public:
		virtual ~SSLEngine();
		
		virtual Bool IsError() = 0;
		virtual Bool SetServerCerts(const UTF8Char *certFile, const UTF8Char *keyFile) = 0;
		virtual void SetSkipCertCheck(Bool skipCertCheck) = 0;
		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff) = 0;
		virtual Net::TCPClient *CreateServerConn(UInt32 *s) = 0;
		virtual Net::TCPClient *Connect(const UTF8Char *hostName, UInt16 port, ErrorType *err) = 0;

		static const UTF8Char *ErrorTypeGetName(ErrorType err);
	};
}
#endif
