#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/Timestamp.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/TCPClient.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(true);
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	NN<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(sockf, CSTR("sswroom.no-ip.org"), 80, 10000));
	cli.Delete();
	Double t = Data::Timestamp::UtcNow().DiffSecDbl(ts);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Time used: "));
	sb.AppendDouble(t);
	IO::ConsoleWriter console;
	console.WriteLine(sb.ToCString());
	return 0;
}