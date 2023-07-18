#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/HTTPOSClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include <stdio.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CString url = CSTR("https://resource.data.one.gov.hk/td/speedmap.xml");
	UInt8 buff[2048];
	UOSInt readSize;
	Net::SSLEngine *ssl;
	NotNullPtr<Net::HTTPClient> cli;
	Net::OSSocketFactory sockf(false);
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR_NULL, true, url.StartsWith(UTF8STRC("https://")));
	printf("Connecting to %s\r\n", url.v);
	cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	while ((readSize = cli->Read(BYTEARR(buff))) > 0)
	{
		printf("Read %d bytes\r\n", (Int32)readSize);
	}
	printf("Status code = %d\r\n", cli->GetRespStatus());

	cli.Delete();
	SDEL_CLASS(ssl);
	return 0;
}
