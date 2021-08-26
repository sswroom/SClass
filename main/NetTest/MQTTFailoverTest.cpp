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
	cli->AddClient((const UTF8Char*)"127.0.0.1", 1883, 0, 0);
	cli->AddClient((const UTF8Char*)"127.0.0.1", 1884, 0, 0);
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UOSInt i = 30;
	while (i-- > 0)
	{
		dt.SetCurrTimeUTC();
		Text::StrInt64(sbuff, dt.ToTicks());
		cli->Publish((const UTF8Char*)"test", sbuff);
		Sync::Thread::Sleep(1000);
	}
	DEL_CLASS(cli);
	DEL_CLASS(sockf);
	return 0;
}
