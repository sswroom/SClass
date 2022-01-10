#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SSLEngineFactory.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter *console;
	UOSInt argc;
	UTF8Char **argv;
	UInt8 buff[2048];
	NEW_CLASS(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc <= 1)
	{
		console->WriteLineC(UTF8STRC("Usage: SHTTPCliSpd [URL]"));
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
			Net::SSLEngine *ssl;
			Net::HTTPClient *cli;
			NEW_CLASS(sockf, Net::OSSocketFactory(true));
			ssl = Net::SSLEngineFactory::Create(sockf, true);
			cli = Net::HTTPClient::CreateClient(sockf, ssl, UTF8STRC("Test/1.0"), false, Text::StrStartsWith(url, (const UTF8Char*)"https://"));
			if (!cli->Connect(url, "GET", &respTimeDNS, &respTimeConn, false))
			{
				console->WriteLineC(UTF8STRC("Error in requesting to server"));
			}
			else
			{
				cli->AddHeaderC(UTF8STRC("User-Agent"), UTF8STRC("Test/1.0"));
				cli->AddHeaderC(UTF8STRC("Accept"), UTF8STRC("*/*"));
				cli->AddHeaderC(UTF8STRC("Accept-Charset"), UTF8STRC("*"));
				cli->EndRequest(&respTimeReq, &respTimeResp);

				httpStatus = cli->GetRespStatus();
				if (httpStatus == 0)
				{
					console->WriteLineC(UTF8STRC("Error in requesting to server"));
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
					sb.AppendC(UTF8STRC("Server response "));
					sb.AppendI32(httpStatus);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Received "));
					sb.AppendU64(totalReadSize);
					sb.AppendC(UTF8STRC(" bytes from server"));
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("DNS Time = "));
					Text::SBAppendF64(&sb, respTimeDNS);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Conn Time = "));
					Text::SBAppendF64(&sb, respTimeConn - respTimeDNS);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Request Time = "));
					Text::SBAppendF64(&sb, respTimeReq - respTimeConn);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Response Time = "));
					Text::SBAppendF64(&sb, respTimeResp - respTimeReq);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Download Time = "));
					Text::SBAppendF64(&sb, respTimeTotal - respTimeResp);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Total Time = "));
					Text::SBAppendF64(&sb, respTimeTotal);
					console->WriteLineC(sb.ToString(), sb.GetLength());
				}
			}
			DEL_CLASS(cli);	
			SDEL_CLASS(ssl);
			DEL_CLASS(sockf);
		}
		else
		{
			console->WriteLineC(UTF8STRC("Only support http url"));
		}
	}

	DEL_CLASS(console);
	return 0;
}
