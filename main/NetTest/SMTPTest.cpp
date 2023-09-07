#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/EmailMessage.h"
#include "Net/Email/SMTPClient.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Parser::FileParser::X509Parser parser;
	Data::DateTime dt;
	IO::ConsoleWriter writer;
	Net::OSSocketFactory sockf(false);
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(sockf, true);
	{
		Net::Email::EmailMessage message;
		message.SetSubject(CSTR("Email Testing"));
		message.SetContent(CSTR("This is a test email"), CSTR("text/html; charset=utf-8"));
		dt.SetCurrTime();
		message.SetSentDate(dt);
		message.SetFrom(CSTR("Test"), CSTR("sswroom@simontest.local"));
		message.AddTo(CSTR("Simon"), CSTR("sswroom@yahoo.com"));
//		message.AddAttachment(CSTR("/home/sswroom/Progs/Temp/OCR2.jpg"));
		message.AddSignature(ssl, (Crypto::Cert::X509Cert*)parser.ParseFilePath(CSTR("/home/sswroom/Progs/VCClass/keystore/Simon_SMIME.crt")),
			(Crypto::Cert::X509Key*)parser.ParseFilePath(CSTR("/home/sswroom/Progs/VCClass/keystore/Simon_SMIME.key")));
		Net::Email::SMTPClient client(sockf, ssl, CSTR("127.0.0.1"), 25, Net::Email::SMTPConn::ConnType::Plain, &writer, 30000);
		client.Send(message);
	}
	SDEL_CLASS(ssl);
	return 0;
}
