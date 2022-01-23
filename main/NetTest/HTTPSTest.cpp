#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/HTTPOSClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString url = {UTF8STRC("https://resource.data.one.gov.hk/td/speedmap.xml")};
	UInt8 buff[2048];
	UOSInt readSize;
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl;
	Net::HTTPClient *cli;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	cli = Net::HTTPClient::CreateClient(sockf, ssl, 0, 0, true, Text::StrStartsWithC(url.v, url.len, UTF8STRC("https://")));
	printf("Connecting to %s\r\n", url.v);
	cli->Connect(url.v, url.len, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
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
