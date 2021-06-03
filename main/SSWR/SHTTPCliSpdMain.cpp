#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter *console;
	OSInt argc;
	UTF8Char **argv;
	UInt8 buff[2048];
	NEW_CLASS(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc <= 1)
	{
		console->WriteLine((const UTF8Char*)"Usage: SHTTPCliSpd [URL]");
	}
	else
	{
		Double respTimeDNS = -1;
		Double respTimeConn = -1;
		Double respTimeReq = -1;
		Double respTimeResp = -1;
		Double respTimeTotal = -1;
		UInt64 totalReadSize;

		const UTF8Char *url = argv[1];
		if (Text::StrStartsWith(url, (const UTF8Char*)"http://"))
		{
			Text::StringBuilderUTF8 sb;
			Int32 httpStatus;
			Net::SocketFactory *sockf;
			Net::HTTPClient *cli;
			NEW_CLASS(sockf, Net::OSSocketFactory(true));
			cli = Net::HTTPClient::CreateClient(sockf, (const UTF8Char*)"Test/1.0", false, Text::StrStartsWith(url, (const UTF8Char*)"https://"));
			if (!cli->Connect(url, "GET", &respTimeDNS, &respTimeConn, false))
			{
				console->WriteLine((const UTF8Char*)"Error in requesting to server");
			}
			else
			{
				cli->AddHeader((const UTF8Char*)"User-Agent", (const UTF8Char*)"Test/1.0");
				cli->AddHeader((const UTF8Char*)"Accept", (const UTF8Char*)"*/*");
				cli->AddHeader((const UTF8Char*)"Accept-Charset", (const UTF8Char*)"*");
				cli->EndRequest(&respTimeReq, &respTimeResp);

				httpStatus = cli->GetRespStatus();
				if (httpStatus == 0)
				{
					console->WriteLine((const UTF8Char*)"Error in requesting to server");
				}
				else
				{
					UOSInt readSize;
					totalReadSize = 0;
					while ((readSize = cli->Read(buff, 2048)) > 0)
					{
						totalReadSize += readSize;
					}
					respTimeTotal = cli->GetTotalTime();

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Server response ");
					sb.AppendI32(httpStatus);
					console->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Received ");
					sb.AppendU64(totalReadSize);
					sb.Append((const UTF8Char*)" bytes from server");
					console->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"DNS Time = ");
					Text::SBAppendF64(&sb, respTimeDNS);
					console->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Conn Time = ");
					Text::SBAppendF64(&sb, respTimeConn - respTimeDNS);
					console->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Request Time = ");
					Text::SBAppendF64(&sb, respTimeReq - respTimeConn);
					console->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Response Time = ");
					Text::SBAppendF64(&sb, respTimeResp - respTimeReq);
					console->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Download Time = ");
					Text::SBAppendF64(&sb, respTimeTotal - respTimeResp);
					console->WriteLine(sb.ToString());

					sb.ClearStr();
					sb.Append((const UTF8Char*)"Total Time = ");
					Text::SBAppendF64(&sb, respTimeTotal);
					console->WriteLine(sb.ToString());
				}
			}
			DEL_CLASS(cli);	
			DEL_CLASS(sockf);
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Only support http url");
		}
	}

	DEL_CLASS(console);
	return 0;
}
