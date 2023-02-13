#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPJSONReader.h"

Text::JSONBase *Net::HTTPJSONReader::Read(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString url)
{
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(sockf, ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	if (!cli->ReadAllContent(&sb, 16384, 1048576))
	{
		DEL_CLASS(cli);
	}
	DEL_CLASS(cli);
	return Text::JSONBase::ParseJSONStr(sb.ToCString());
}
