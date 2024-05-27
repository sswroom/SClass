#include "Stdafx.h"
#include "Net/SSLClient.h"

Net::SSLClient::SSLClient(NN<Net::SocketFactory> sockf, NN<Socket> s) : Net::TCPClient(sockf, s)
{

}

Net::SSLClient::~SSLClient()
{

}

Bool Net::SSLClient::IsSSL() const
{
	return true;
}

IO::StreamType Net::SSLClient::GetStreamType() const
{
	return IO::StreamType::SSLClient;
}
