#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/Timestamp.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/TCPClient.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(true);
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(&sockf, CSTR("sswroom.no-ip.org"), 80, 10000));
	DEL_CLASS(cli);
	Double t = Data::Timestamp::UtcNow().DiffSecDbl(ts);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Time used: "));
	sb.AppendDouble(t);
	IO::ConsoleWriter console;
	console.WriteLineCStr(sb.ToCString());
	return 0;
}