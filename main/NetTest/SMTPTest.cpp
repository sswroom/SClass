#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/EmailMessage.h"
#include "Net/Email/SMTPClient.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Parser::FileParser::X509Parser parser;
	Data::DateTime dt;
	IO::ConsoleWriter writer;
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, true);
	{
		Net::Email::EmailMessage message;
		message.SetSubject(CSTR("Email Testing"));
		message.SetContent(CSTR("This is a test email"), CSTR("text/html; charset=utf-8"));
		dt.SetCurrTime();
		message.SetSentDate(dt);
		message.SetFrom(CSTR("Test"), CSTR("sswroom@simontest.local"));
		message.AddTo(CSTR("Simon"), CSTR("sswroom@yahoo.com"));
//		message.AddAttachment(CSTR("/home/sswroom/Progs/Temp/OCR2.jpg"));
		message.AddSignature(ssl, Optional<Crypto::Cert::X509Cert>::ConvertFrom(parser.ParseFilePath(CSTR("/home/sswroom/Progs/VCClass/keystore/Simon_SMIME.crt"))).OrNull(),
			Optional<Crypto::Cert::X509Key>::ConvertFrom(parser.ParseFilePath(CSTR("/home/sswroom/Progs/VCClass/keystore/Simon_SMIME.key"))).OrNull());
		Net::Email::SMTPClient client(clif, ssl, CSTR("127.0.0.1"), 25, Net::Email::SMTPConn::ConnType::Plain, &writer, 30000);
		client.Send(message);
	}
	ssl.Delete();
	return 0;
}
