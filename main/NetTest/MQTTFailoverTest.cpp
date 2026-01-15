#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/MQTTFailoverClient.h"
#include "Net/OSSocketFactory.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Net::MQTTFailoverClient *cli;
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	NEW_CLASS(cli, Net::MQTTFailoverClient(Net::FT_MASTER_SLAVE, clif, nullptr, 30));
	cli->AddClient(CSTR("127.0.0.1"), 1883, nullptr, nullptr, false);
	cli->AddClient(CSTR("127.0.0.1"), 1884, nullptr, nullptr, false);
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = 30;
	while (i-- > 0)
	{
		dt.SetCurrTimeUTC();
		sptr = Text::StrInt64(sbuff, dt.ToTicks());
		cli->Publish(CSTR("test"), CSTRP(sbuff, sptr), false, 0, false);
		Sync::SimpleThread::Sleep(1000);
	}
	DEL_CLASS(cli);
	return 0;
}
