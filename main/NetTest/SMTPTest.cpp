#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/Email/EmailMessage.h"
#include "Net/Email/SMTPClient.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::Email::EmailMessage *message;
	Net::SocketFactory *sockf;
	Net::Email::SMTPClient *client;
	Data::DateTime dt;
	IO::ConsoleWriter writer;
	dt.SetCurrTime();
	NEW_CLASS(message, Net::Email::EmailMessage());
	message->SetSubject((const UTF8Char*)"Email Testing");
	message->SetContent((const UTF8Char*)"This is a test email", "text/html; charset=utf-8");
	message->SetSentDate(&dt);
	message->SetFrom((const UTF8Char*)"Test", (const UTF8Char*)"sswroom@yahoo.com");
	message->AddTo((const UTF8Char*)"Simon", (const UTF8Char*)"sswroom@yahoo.com");

	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	NEW_CLASS(client, Net::Email::SMTPClient(sockf, 0, (const UTF8Char*)"127.0.0.1", 25, Net::Email::SMTPConn::CT_PLAIN, &writer));
	client->Send(message);
	DEL_CLASS(message)
	DEL_CLASS(client);
	DEL_CLASS(sockf);
	return 0;
}
