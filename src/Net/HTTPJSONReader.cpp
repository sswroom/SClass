#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPJSONReader.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Text::JSONBase *Net::HTTPJSONReader::Read(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString url)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(sockf, ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
#if defined(VERBOSE)
		printf("HTTPJSONReader: Error in connecting to server\r\n");
#endif
		cli.Delete();
		return 0;
	}
	if (cli->GetRespStatus() == Net::WebStatus::SC_MOVED_PERMANENTLY)
	{
		Text::CString newUrl = cli->GetRespHeader(CSTR("Location"));
		if (newUrl.leng > 0)
		{
#if defined(VERBOSE)
			printf("HTTPJSONReader: Redirect location found: %s\r\n", newUrl.v);
#endif
			NotNullPtr<Net::HTTPClient> cli2 = Net::HTTPClient::CreateConnect(sockf, ssl, newUrl, Net::WebUtil::RequestMethod::HTTP_GET, false);
			if (cli2->IsError())
			{
#if defined(VERBOSE)
				printf("HTTPJSONReader: Error in connecting to server2\r\n");
#endif
				cli2.Delete();
				cli.Delete();
				return 0;
			}
			cli.Delete();
			cli = cli2;
		}
	}
	Text::StringBuilderUTF8 sb;
	if (!cli->ReadAllContent(sb, 16384, 1048576))
	{
#if defined(VERBOSE)
		printf("HTTPJSONReader: Error in reading content from server\r\n");
#endif
		cli.Delete();
	}
	cli.Delete();
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(sb.ToCString());
#if defined(VERBOSE)
	if (json == 0)
	{
		printf("HTTPJSONReader: Error in Parsing JSON data\r\n");
		printf("%s\r\n-------------- End of data -----------------\r\n", sb.ToString());
	}
#endif
	return json;
}
