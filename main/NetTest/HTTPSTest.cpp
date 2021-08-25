#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/HTTPOSClient.h"
#include "Net/OSSocketFactory.h"
#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	const UTF8Char *url = (const UTF8Char*)"https://resource.data.one.gov.hk/td/speedmap.xml";
	UInt8 buff[2048];
	UOSInt readSize;
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl;
	Net::HTTPClient *cli;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	ssl = Net::DefaultSSLEngine::Create(sockf, true);
	cli = Net::HTTPClient::CreateClient(sockf, ssl, 0, true, Text::StrStartsWith(url, (const UTF8Char*)"https://"));
	printf("Connecting to %s\r\n", url);
	cli->Connect(url, "GET", 0, 0, true);
	while ((readSize = cli->Read(buff, 2048)) > 0)
	{
		printf("Read %d bytes\r\n", (Int32)readSize);
	}
	printf("Status code = %d\r\n", cli->GetRespStatus());

	DEL_CLASS(cli);
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	return 0;
}
