#include "Stdafx.h"
#include "Net/SSLClient.h"

Net::SSLClient::SSLClient(Net::SocketFactory *sockf, UInt32 *s) : Net::TCPClient(sockf, s)
{

}

Net::SSLClient::~SSLClient()
{

}

Bool Net::SSLClient::IsSSL()
{
	return true;
}
