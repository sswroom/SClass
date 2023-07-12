#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/POP3Client.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter writer;

	Net::OSSocketFactory sockf(false);
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(&sockf, true);
	{
		Net::Email::POP3Client client(&sockf, ssl, CSTR("127.0.0.1"), 110, Net::Email::POP3Conn::CT_PLAIN, &writer, CSTR("sswroom@yahoo.com"), CSTR("sswroom@yahoo.com"), 30000);
		if (!client.IsError())
		{
			Text::StringBuilderUTF8 sb;
			UOSInt i = 0;
			UOSInt j = client.GetMessageCount();
			while (i < j)
			{
				sb.ClearStr();
				client.ReadMessageAsString(i, &sb);
				i++;
			}
		}
	}
	SDEL_CLASS(ssl);
	return 0;
}
