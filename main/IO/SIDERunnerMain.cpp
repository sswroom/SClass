#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MTStream.h"
#include "IO/Path.h"
#include "IO/SeleniumIDERunner.h"
#include "Manage/ExceptionRecorder.h"
#include "Manage/Process.h"
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
	console->WriteLine(CSTR("  --no-pause [bool]     Whether skip pause commands, default is false"));
	console->WriteLine(CSTR("  --headless [bool]     Whether running at headless mode if supported, default is false"));
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
	Manage::ExceptionRecorder exHdlr(CSTR("Error.txt"), Manage::ExceptionRecorder::EA_CLOSE);
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
	IO::SeleniumIDERunner::RunOptions options;
	Bool noPause = false;
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
			else if (cmd.Equals(CSTR("--no-pause")))
			{
				if (param.Equals(CSTR("false")))
				{
					noPause = false;
				}
				else if (param.Equals(CSTR("true")))
				{
					noPause = true;
				}
				else
				{
					noPause = (param.ToInt32() != 0);
				}
			}
			else if (cmd.Equals(CSTR("--headless")))
			{
				if (param.Equals(CSTR("false")))
				{
					options.headless = false;
				}
				else if (param.Equals(CSTR("true")))
				{
					options.headless = true;
				}
				else
				{
					options.headless = (param.ToInt32() != 0);
				}
			}
			else if (cmd.Equals(CSTR("--no-sandbox")))
			{
				if (param.Equals(CSTR("false")))
				{
					options.noSandbox = false;
				}
				else if (param.Equals(CSTR("true")))
				{
					options.noSandbox = true;
				}
				else
				{
					options.noSandbox = (param.ToInt32() != 0);
				}
			}
			else if (cmd.Equals(CSTR("--disable-gpu")))
			{
				if (param.Equals(CSTR("false")))
				{
					options.disableGPU = false;
				}
				else if (param.Equals(CSTR("true")))
				{
					options.disableGPU = true;
				}
				else
				{
					options.disableGPU = (param.ToInt32() != 0);
				}
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
				if (noPause)
				{
					runner.SetNoPause(noPause);
				}
				NN<IO::SeleniumTest> test;
				if (side.GetTest(0).IsNull())
				{
					console.WriteLine(CSTR("Error in parsing side file, no tests found"));
				}
				else
				{
					Text::CStringNN url = Text::String::OrEmpty(side.GetURL())->ToCString();
					NN<Net::WebDriverSession> sess;
					NN<IO::MemoryStream> screenMstm;
					Text::StringBuilderUTF8 sb;
					Int64 startTime = Data::DateTimeUtil::GetCurrTimeMillis();
					UOSInt procId = Manage::Process::GetCurrProcId();
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
					sb.AppendI64(startTime);
					sb.AppendUTF8Char('.');
					sb.AppendUOSInt(procId);
					sb.Append(CSTR(".csv"));
					IO::FileStream logFS(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal);
					IO::MTStream logStm(logFS, 1048576);
					logStm.Write(CSTR("Time,TestId,Step,Duration\r\n").ToByteArray());

					sb.ClearStr();
					sb.Append(CSTR("Running "));
					sb.Append(s.Substring(i + 1));
					console.WriteLine(sb.ToCString());
					testIndex = 0;
					while (side.GetTest(testIndex).SetTo(test))
					{
						if (runner.BeginTest(browser, mobile, 0, url, options).SetTo(sess))
						{
							if (!runner.RunTest(sess, test, url, OnTestStep, &logStm))
							{
								sb.ClearStr();
								sb.Append(logPath);
								if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
								{
									sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
								}
								sb.Append(s.Substring(i + 1));
								sb.AppendUTF8Char('.');
								sb.AppendI64(startTime);
								sb.AppendUTF8Char('.');
								sb.AppendUOSInt(procId);
								sb.Append(CSTR(".err"));
								IO::FileStream errFS(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);

								sb.ClearStr();
								sb.Append(CSTR("Error occurs while running the test ("));
								sb.AppendOpt(test->GetName());
								sb.Append(CSTR(") in step "));
								sb.AppendOSInt((OSInt)runner.GetLastErrorIndex());
								sb.Append(CSTR("\r\n"));
								console.Write(sb.ToCString());
								errFS.Write(sb.ToByteArray());
								sb.ClearStr();
								sb.Append(CSTR("Error Message: "));
								sb.AppendOpt(runner.GetLastErrorMsg());
								sb.Append(CSTR("\r\n"));
								console.Write(sb.ToCString());
								errFS.Write(sb.ToByteArray());

								if (sess->TakeScreenshot().SetTo(screenMstm))
								{
									sb.ClearStr();
									sb.Append(logPath);
									if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
									{
										sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
									}
									sb.Append(s.Substring(i + 1));
									sb.AppendUTF8Char('.');
									sb.AppendI64(startTime);
									sb.AppendUTF8Char('.');
									sb.AppendUOSInt(procId);
									sb.Append(CSTR(".png"));
									IO::FileStream screenFS(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
									screenFS.Write(screenMstm->GetArray());
									screenMstm.Delete();
								}
							}
							sess.Delete();
						}
						else
						{
							sb.ClearStr();
							sb.Append(logPath);
							if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
							{
								sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
							}
							sb.Append(s.Substring(i + 1));
							sb.AppendUTF8Char('.');
							sb.AppendI64(startTime);
							sb.AppendUTF8Char('.');
							sb.AppendUOSInt(procId);
							sb.Append(CSTR(".err"));
							IO::FileStream errFS(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);

							sb.ClearStr();
							sb.Append(CSTR("Error occurs while running the test ("));
							sb.AppendOpt(test->GetName());
							sb.Append(CSTR(") in step "));
							sb.AppendOSInt((OSInt)runner.GetLastErrorIndex());
							sb.Append(CSTR("\r\n"));
							console.Write(sb.ToCString());
							errFS.Write(sb.ToByteArray());
							sb.ClearStr();
							sb.Append(CSTR("Error Message: "));
							sb.AppendOpt(runner.GetLastErrorMsg());
							sb.Append(CSTR("\r\n"));
							console.Write(sb.ToCString());
							errFS.Write(sb.ToByteArray());
						}
						testIndex++;
					}
					sb.ClearStr();
					sb.Append(CSTR("End Running "));
					sb.Append(s.Substring(i + 1));
					console.WriteLine(sb.ToCString());
				}
			}
			MemFreeArr(fileBuff);
		}
	}
	return 0;
}