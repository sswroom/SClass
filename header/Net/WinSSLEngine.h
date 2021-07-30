#ifndef _SM_NET_WINSSLENGINE
#define _SM_NET_WINSSLENGINE
#include "Net/SSLEngine.h"

namespace Net
{
	class WinSSLEngine : public Net::SSLEngine
	{
	private:
		struct ClassData;
		struct ClassData *clsData;
		Bool skipCertCheck;

		Bool InitClient(Method method);
	public:
		WinSSLEngine(Net::SocketFactory *sockf, Method method);
		virtual ~WinSSLEngine();
		
		virtual void SetSkipCertCheck(Bool skipCertCheck);
		virtual Bool IsError();
		virtual Bool SetServerCerts(const UTF8Char *certFile, const UTF8Char *keyFile);
		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff);
		virtual Net::TCPClient *CreateServerConn(UInt32 *s);
		virtual Net::TCPClient *Connect(const UTF8Char *hostName, UInt16 port, ErrorType *err);
	};
}
#endif
