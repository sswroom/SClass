#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Net/ACMEClient.h"
#include "Net/OSSocketFactory.h"
#include "Sync/SimpleThread.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN domain = CSTR("sswroom.no-ip.org");
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter *console;
	Net::ACMEClient *acme;

	NEW_CLASS(console, IO::ConsoleWriter());
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);

	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("ACMEKey.pem"));
	NEW_CLASS(acme, Net::ACMEClient(clif, CSTR("acme-staging-v02.api.letsencrypt.org"), 0, CSTRP(sbuff, sptr)));
	NN<Net::ACMEConn> conn = acme->GetConn();
	NN<Net::ACMEConn::Order> order;
	if (conn->OrderNew(domain.v, domain.leng).SetTo(order))
	{
		NN<Data::ArrayListStringNN> authURLs;
		if (order->authURLs.SetTo(authURLs))
		{
			UIntOS i = 0;
			UIntOS j = authURLs->GetCount();
			while (i < j)
			{
				NN<Net::ACMEConn::Challenge> chall;
				if (conn->OrderAuthorize(Text::String::OrEmpty(authURLs->GetItem(i)), Net::ACMEConn::AuthorizeType::TLS_ALPN_01).SetTo(chall))
				{
					Optional<Net::ACMEConn::Challenge> challStatus = conn->ChallengeBegin(chall->url);
					NN<Net::ACMEConn::Challenge> nnchallStatus;
					while (challStatus.SetTo(nnchallStatus) && (nnchallStatus->status == Net::ACMEConn::ACMEStatus::Pending || nnchallStatus->status == Net::ACMEConn::ACMEStatus::Processing))
					{
						conn->ChallengeFree(nnchallStatus);
						Sync::SimpleThread::Sleep(5000);

						challStatus = conn->ChallengeGetStatus(chall->url);
					}
					if (challStatus.SetTo(nnchallStatus))
					{
						conn->ChallengeFree(nnchallStatus);
					}
					conn->ChallengeFree(chall);
				}
				i++;
			}
		}

		conn->OrderFree(order);
	}
	DEL_CLASS(acme);

	DEL_CLASS(console);
	return 0;
}
