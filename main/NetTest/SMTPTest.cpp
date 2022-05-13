#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
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
	message->SetSubject(CSTR("Email Testing"));
	message->SetContent(CSTR("This is a test email"), CSTR("text/html; charset=utf-8"));
	message->SetSentDate(&dt);
	message->SetFrom(CSTR("Test"), CSTR("sswroom@yahoo.com"));
	message->AddTo(CSTR("Simon"), CSTR("sswroom@yahoo.com"));
	message->AddAttachment(CSTR("/home/sswroom/Progs/Temp/OCR2.jpg"));

	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(sockf, true);
	NEW_CLASS(client, Net::Email::SMTPClient(sockf, ssl, CSTR("127.0.0.1"), 25, Net::Email::SMTPConn::CT_PLAIN, &writer));
	client->Send(message);
	DEL_CLASS(message)
	DEL_CLASS(client);
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	return 0;
}
