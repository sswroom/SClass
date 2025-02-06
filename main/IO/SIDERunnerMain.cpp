#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MTStream.h"
#include "IO/Path.h"
#include "IO/SeleniumIDERunner.h"
#include "Net/OSSocketFactory.h"
#include "Parser/FileParser/JSONParser.h"

UOSInt testIndex;
void PrintHelp(NN<IO::ConsoleWriter> console)
{
	console->WriteLine(CSTR("Usage: SIDERunner [options] your-project-file.side"));
	console->WriteLine(CSTR("Options:"));
	console->WriteLine(CSTR("  --port [port]         WebDriver local server port number, default 4444"));
	console->WriteLine(CSTR("  --server [url]        WebDriver remote server, default using local server"));
	console->WriteLine(CSTR("  --mobile [deviceName] Mobile emulation device name"));
	console->WriteLine(CSTR("  --browser [browser]   Browser Type of the WebDriver: Chrome(Default), MSEdge, Firefox, HtmlUnit, InternetExplorer,"));
	console->WriteLine(CSTR("                          iPad, iPhone, Opera, Safari, WebKitGTK, Mock, or PhantomJS"));
	console->WriteLine(CSTR("  --log-path [logDir]   Log path for log file, default is ./log"));
}

void __stdcall OnTestStep(AnyType userObj, UOSInt cmdIndex, Data::Duration dur)
{
	NN<IO::Stream> stm = userObj.GetNN<IO::Stream>();
	Text::StringBuilderUTF8 sb;
	sb.AppendTSNoZone(Data::Timestamp::Now());
	sb.AppendUTF8Char(',');
	sb.AppendUOSInt(testIndex);
	sb.AppendUTF8Char(',');
	sb.AppendUOSInt(cmdIndex);
	sb.AppendUTF8Char(',');
	sb.AppendDouble(dur.GetTotalSec());
	sb.Append(CSTR("\r\n"));
	stm->Write(sb.ToByteArray());
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt cmdCnt;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	Text::CStringNN cmd;
	Text::CStringNN param;
	UInt16 port = 4444;
	Text::CString url = 0;
	Text::CString mobile = 0;
	IO::SeleniumIDERunner::BrowserType browser = IO::SeleniumIDERunner::BrowserType::Chrome;
	Text::CStringNN logPath = CSTR("log");
	Text::CString sideFile = 0;
	Text::CStringNN s;
	UOSInt i;
	Bool hasError = false;
	i = 1;
	while (i < cmdCnt)
	{
		cmd = Text::CStringNN::FromPtr(cmdLines[i]);
		if (cmd.StartsWith(CSTR("--")))
		{
			if (i + 1 >= cmdCnt)
			{
				hasError = true;
				console.Write(CSTR("Missing parameter for switch "));
				console.WriteLine(cmd);
				break;
			}
			i++;
			param = Text::CStringNN::FromPtr(cmdLines[i]);
			if (cmd.Equals(CSTR("--port")))
			{
				if (!param.ToUInt16(port))
				{
					hasError = true;
					console.Write(CSTR("Port is not valid: "));
					console.WriteLine(param);
					break;
				}
			}
			else if (cmd.Equals(CSTR("--server")))
			{
				if (!param.StartsWith(CSTR("http://")) && !param.StartsWith(CSTR("https://")))
				{
					hasError = true;
					console.Write(CSTR("Remote server URL is not valid: "));
					console.WriteLine(param);
					break;
				}
				url = param;
			}
			else if (cmd.Equals(CSTR("--mobile")))
			{
				mobile = param;
			}
			else if (cmd.Equals(CSTR("--browser")))
			{
				if (param.Equals(CSTR("Chrome")))
					browser = IO::SeleniumIDERunner::BrowserType::Chrome;
				else if (param.Equals(CSTR("MSEdge")))
					browser = IO::SeleniumIDERunner::BrowserType::MSEdge;
				else if (param.Equals(CSTR("Firefox")))
					browser = IO::SeleniumIDERunner::BrowserType::Firefox;
				else if (param.Equals(CSTR("HtmlUnit")))
					browser = IO::SeleniumIDERunner::BrowserType::HtmlUnit;
				else if (param.Equals(CSTR("InternetExplorer")))
					browser = IO::SeleniumIDERunner::BrowserType::InternetExplorer;
				else if (param.Equals(CSTR("IPad")))
					browser = IO::SeleniumIDERunner::BrowserType::IPad;
				else if (param.Equals(CSTR("IPhone")))
					browser = IO::SeleniumIDERunner::BrowserType::IPhone;
				else if (param.Equals(CSTR("Opera")))
					browser = IO::SeleniumIDERunner::BrowserType::Opera;
				else if (param.Equals(CSTR("Safari")))
					browser = IO::SeleniumIDERunner::BrowserType::Safari;
				else if (param.Equals(CSTR("WebKitGTK")))
					browser = IO::SeleniumIDERunner::BrowserType::WebKitGTK;
				else if (param.Equals(CSTR("Mock")))
					browser = IO::SeleniumIDERunner::BrowserType::Mock;
				else if (param.Equals(CSTR("PhantomJS")))
					browser = IO::SeleniumIDERunner::BrowserType::PhantomJS;
				else
				{
					hasError = true;
					console.Write(CSTR("Browser Type not supported: "));
					console.WriteLine(param);
					break;
				}
			}
			else if (cmd.Equals(CSTR("--log-path")))
			{
				logPath = param;
			}
			else
			{
				hasError = true;
				console.Write(CSTR("Switch not supported: "));
				console.WriteLine(cmd);
				break;
			}
		}
		else if (sideFile.IsNull())
		{
			sideFile = cmd;
		}
		else
		{
			hasError = true;
			console.Write(CSTR("Only allow 1 side file, this parameter is not allowed: "));
			console.WriteLine(cmd);
			break;
		}
		i++;
	}

	if (hasError || !sideFile.SetTo(s))
	{
		PrintHelp(console);
	}
	else
	{
		IO::FileStream fs(s, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		UInt64 fileLen = fs.GetLength();
		if (fs.IsError())
		{
			console.Write(CSTR("Error in reading side file: "));
			console.WriteLine(s);
		}
		else if (fileLen < 10 || fileLen > 1048576)
		{
			console.Write(CSTR("File size is out of range (2 - 1048576): "));
			console.WriteLine(s);
		}
		else
		{
			NN<Text::JSONBase> json;
			UnsafeArray<UInt8> fileBuff = MemAllocArr(UInt8, (UOSInt)fileLen + 1);
			fileBuff[(UOSInt)fileLen] = 0;
			if (fs.Read(Data::ByteArray(fileBuff, (UOSInt)fileLen)) != fileLen)
			{
				console.Write(CSTR("Error in reading side file: "));
				console.WriteLine(s);
			}
			else if (!Text::JSONBase::ParseJSONStr(Text::CStringNN(fileBuff, (UOSInt)fileLen)).SetTo(json))
			{
				console.WriteLine(CSTR("Error in parsing side file, not json file"));
			}
			else if (json->GetType() != Text::JSONType::Object)
			{
				console.WriteLine(CSTR("Error in parsing side file, not json object"));
			}
			else
			{
				NN<Text::String> sourceName = Text::String::New(s);
				IO::SeleniumIDE side(sourceName, NN<Text::JSONObject>::ConvertFrom(json));
				sourceName->Release();
				json->EndUse();

				Net::OSSocketFactory sockf(false);
				Net::TCPClientFactory clif(sockf);
				IO::SeleniumIDERunner runner(clif, port);
				if (url.SetTo(s))
				{
					runner.SetURL(s);
				}
				NN<IO::SeleniumTest> test;
				if (side.GetTest(0).IsNull())
				{
					console.WriteLine(CSTR("Error in parsing side file, no tests found"));
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.Append(logPath);
					IO::Path::CreateDirectory(sb.ToCString());
					if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
					{
						sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
					}
					s = sideFile.OrEmpty();
					i = s.LastIndexOf(IO::Path::PATH_SEPERATOR);
					sb.Append(s.Substring(i + 1));
					sb.AppendUTF8Char('.');
					sb.AppendI64(Data::DateTimeUtil::GetCurrTimeMillis());
					sb.Append(CSTR(".csv"));
					console.Write(CSTR("Running "));
					console.WriteLine(s.Substring(i + 1));
					IO::FileStream logFS(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal);
					IO::MTStream logStm(logFS, 1048576);
					logStm.Write(CSTR("Time,TestId,Step,Duration\r\n").ToByteArray());
					testIndex = 0;
					while (side.GetTest(testIndex).SetTo(test))
					{
						if (!runner.Run(test, browser, mobile, 0, OnTestStep, &logStm))
						{
							sb.ClearStr();
							sb.Append(CSTR("Error occurs while running the test in step "));
							sb.AppendOSInt((OSInt)runner.GetLastErrorIndex());
							console.WriteLine(sb.ToCString());
							console.Write(CSTR("Error Message: "));
							console.WriteLine(Text::String::OrEmpty(runner.GetLastErrorMsg())->ToCString());
						}
						testIndex++;
					}
				}
			}
			MemFreeArr(fileBuff);
		}
	}
	return 0;
}