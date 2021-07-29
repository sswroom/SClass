#include "Stdafx.h"
#include "Net/SSLServer.h"

Net::SSLServer::SSLServer(Net::SocketFactory *sockf, Method method, const UTF8Char *certFile, const UTF8Char *keyFile)
{

}

Net::SSLServer::~SSLServer()
{

}

Bool Net::SSLServer::IsError()
{
	return true;
}

Net::TCPClient *Net::SSLServer::CreateClient(UInt32 *s)
{
	return 0;
}
