#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/HTTPOSClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include <stdio.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Text::CStringNN url = CSTR("https://resource.data.one.gov.hk/td/speedmap.xml");
	UInt8 buff[2048];
	UOSInt readSize;
	Optional<Net::SSLEngine> ssl;
	NN<Net::HTTPClient> cli;
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	ssl = Net::SSLEngineFactory::Create(clif, true);
	cli = Net::HTTPClient::CreateClient(clif, ssl, CSTR_NULL, true, url.StartsWith(UTF8STRC("https://")));
	printf("Connecting to %s\r\n", url.v.Ptr());
	cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	while ((readSize = cli->Read(BYTEARR(buff))) > 0)
	{
		printf("Read %d bytes\r\n", (Int32)readSize);
	}
	printf("Status code = %d\r\n", cli->GetRespStatus());

	cli.Delete();
	ssl.Delete();
	return 0;
}
