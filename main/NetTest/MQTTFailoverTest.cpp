#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/MQTTFailoverClient.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::SocketFactory *sockf;
	Net::MQTTFailoverClient *cli;
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(cli, Net::MQTTFailoverClient(Net::FT_MASTER_SLAVE, sockf, 0, 30));
	cli->AddClient(CSTR("127.0.0.1"), 1883, CSTR_NULL, CSTR_NULL, false);
	cli->AddClient(CSTR("127.0.0.1"), 1884, CSTR_NULL, CSTR_NULL, false);
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i = 30;
	while (i-- > 0)
	{
		dt.SetCurrTimeUTC();
		sptr = Text::StrInt64(sbuff, dt.ToTicks());
		cli->Publish(CSTR("test"), CSTRP(sbuff, sptr));
		Sync::Thread::Sleep(1000);
	}
	DEL_CLASS(cli);
	DEL_CLASS(sockf);
	return 0;
}
