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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
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

		Text::CStringNN url = {argv[1], Text::StrCharCnt(argv[1])};
		if (url.StartsWith(UTF8STRC("http://")))
		{
			Text::StringBuilderUTF8 sb;
			Int32 httpStatus;
			Net::SSLEngine *ssl;
			NotNullPtr<Net::HTTPClient> cli;
			Net::OSSocketFactory sockf(true);
			ssl = Net::SSLEngineFactory::Create(sockf, true);
			cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR("Test/1.0"), false, url.StartsWith(UTF8STRC("https://")));
			if (!cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, &respTimeDNS, &respTimeConn, false))
			{
				console->WriteLineC(UTF8STRC("Error in requesting to server"));
			}
			else
			{
				cli->AddHeaderC(CSTR("User-Agent"), CSTR("Test/1.0"));
				cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
				cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
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
					while ((readSize = cli->Read(BYTEARR(buff))) > 0)
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
					sb.AppendDouble(respTimeDNS);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Conn Time = "));
					sb.AppendDouble(respTimeConn - respTimeDNS);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Request Time = "));
					sb.AppendDouble(respTimeReq - respTimeConn);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Response Time = "));
					sb.AppendDouble(respTimeResp - respTimeReq);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Download Time = "));
					sb.AppendDouble(respTimeTotal - respTimeResp);
					console->WriteLineC(sb.ToString(), sb.GetLength());

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Total Time = "));
					sb.AppendDouble(respTimeTotal);
					console->WriteLineC(sb.ToString(), sb.GetLength());
				}
			}
			cli.Delete();
			SDEL_CLASS(ssl);
		}
		else
		{
			console->WriteLineC(UTF8STRC("Only support http url"));
		}
	}

	DEL_CLASS(console);
	return 0;
}
