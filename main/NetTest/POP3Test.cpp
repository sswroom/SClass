#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/POP3Client.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter writer;

	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, true);
	{
		Net::Email::POP3Client client(clif, ssl, CSTR("127.0.0.1"), 110, Net::Email::POP3Conn::CT_PLAIN, &writer, CSTR("sswroom@yahoo.com"), CSTR("sswroom@yahoo.com"), 30000);
		if (!client.IsError())
		{
			Text::StringBuilderUTF8 sb;
			UOSInt i = 0;
			UOSInt j = client.GetMessageCount();
			while (i < j)
			{
				sb.ClearStr();
				client.ReadMessageAsString(i, sb);
				i++;
			}
		}
	}
	ssl.Delete();
	return 0;
}
