#include "Stdafx.h"
#include "Net/IAMSmartClient.h"

Net::IAMSmartClient::IAMSmartClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN domain, Text::CStringNN clientID, Text::CStringNN clientSecret) : api(sockf, ssl, domain, clientID, clientSecret)
{
}

Net::IAMSmartClient::~IAMSmartClient()
{
}
