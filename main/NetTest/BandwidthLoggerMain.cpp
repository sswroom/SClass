#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/BandwidthLogger.h"
#include "Net/OSSocketFactory.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(false);
	Net::BandwidthLogger logger(sockf);
	IO::ConsoleWriter console;
	if (!logger.IsError())
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrInt64(sbuff, Data::DateTimeUtil::GetCurrTimeMillis());
		sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
		if (logger.BeginLogFile(CSTRP(sbuff, sptr)))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Logging to "));
			sb.AppendP(sbuff, sptr);
			console.WriteLine(sb.ToCString());
			progCtrl->WaitForExit(progCtrl);
		}
		else
		{
			console.WriteLine(CSTR("Error in creating log file"));
		}
	}
	else
	{
		console.WriteLine(CSTR("Error in listening to socket"));
	}
	return 0;
}
