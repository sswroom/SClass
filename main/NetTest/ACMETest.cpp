#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Net/ACMEClient.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Thread.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	const UTF8Char *domain = (const UTF8Char*)"sswroom.no-ip.org";
	UTF8Char sbuff[512];
	IO::ConsoleWriter *console;
	Net::SocketFactory *sockf;
	Net::ACMEClient *acme;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"ACMEKey.pem");
	NEW_CLASS(acme, Net::ACMEClient(sockf, (const UTF8Char*)"acme-staging-v02.api.letsencrypt.org", 0, sbuff));
	Net::ACMEConn *conn = acme->GetConn();
	Net::ACMEConn::Order *order = conn->OrderNew(domain);
	if (order)
	{
		if (order->authURLs)
		{
			UOSInt i = 0;
			UOSInt j = order->authURLs->GetCount();
			while (i < j)
			{
				Net::ACMEConn::Challenge *chall = conn->OrderAuthorize(order->authURLs->GetItem(i), Net::ACMEConn::AuthorizeType::TLS_ALPN_01);
				if (chall)
				{
					Net::ACMEConn::Challenge *challStatus = conn->ChallengeBegin(chall->url);
					while (challStatus && (challStatus->status == Net::ACMEConn::ACMEStatus::Pending || challStatus->status == Net::ACMEConn::ACMEStatus::Processing))
					{
						conn->ChallengeFree(challStatus);
						Sync::Thread::Sleep(5000);

						challStatus = conn->ChallengeGetStatus(chall->url);
					}
					if (challStatus)
					{
						conn->ChallengeFree(challStatus);
					}
					conn->ChallengeFree(chall);
				}
				i++;
			}
		}

		conn->OrderFree(order);
	}
	DEL_CLASS(acme);

	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}