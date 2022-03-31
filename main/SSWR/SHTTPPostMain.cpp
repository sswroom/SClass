#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/HTTPClient.h"
#include "Net/MIME.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter *console;
	UOSInt argc;
	UTF8Char **argv;
	UInt8 buff[2048];
	NEW_CLASS(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc <= 2)
	{
		console->WriteLineC(UTF8STRC("Usage: SHTTPPost [URL] [File]"));
	}
	else
	{
		const UTF8Char *url = argv[1];
		UOSInt urlLen = Text::StrCharCnt(url);
		const UTF8Char *file = argv[2];

		UOSInt i;
		UOSInt j;
		UInt8 *fileBuff = 0;
		UOSInt fileSize = 0;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream({file, Text::StrCharCnt(file)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fileSize = fs->GetLength();
		if (fileSize > 0)
		{
			fileBuff = MemAlloc(UInt8, fileSize);
			if (fs->Read(fileBuff, fileSize) != fileSize)
			{
				console->WriteLineC(UTF8STRC("Error in reading file"));
				fileSize = 0;
				MemFree(fileBuff);
				fileBuff = 0;
			}
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in opening file"));
		}
		DEL_CLASS(fs);

		if (Text::StrStartsWithC(url, urlLen, UTF8STRC("http://")) && fileBuff && fileSize > 0)
		{
			Text::CString mime = CSTR_NULL;
			UOSInt fileLen = Text::StrCharCnt(file);
			i = Text::StrLastIndexOfCharC(file, fileLen, '.');
			if (i != INVALID_INDEX)
			{
				mime = Net::MIME::GetMIMEFromExt(Text::CString(&file[i + 1], fileLen - i - 1));
			}
			Text::StringBuilderUTF8 sb;
			Int32 httpStatus;
			UOSInt totalSize = 0;
			UOSInt writeSize;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::HTTPClient *cli;
			NEW_CLASS(sockf, Net::OSSocketFactory(true));
			ssl = Net::SSLEngineFactory::Create(sockf, true);
			cli = Net::HTTPClient::CreateConnect(sockf, ssl, {url, urlLen}, Net::WebUtil::RequestMethod::HTTP_POST, false);
			if (mime.v)
			{
				cli->AddContentType(mime);
			}
			cli->AddContentLength(fileSize); 

			while (totalSize < fileSize)
			{
				writeSize = cli->Write(&fileBuff[totalSize], fileSize - totalSize);
				if (writeSize <= 0)
				{
					console->WriteLineC(UTF8STRC("Error in uploading to server"));
					break;
				}
				else
				{
					totalSize += writeSize;
				}

			}
			httpStatus = cli->GetRespStatus();
			if (httpStatus == 0)
			{
				console->WriteLineC(UTF8STRC("Error in requesting to server"));
			}
			else
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Server response "));
				sb.AppendI32(httpStatus);
				console->WriteLineC(sb.ToString(), sb.GetLength());

				IO::MemoryStream *mstm;
				NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SHTTPGet.mstm")));
				UOSInt readSize;

				while ((readSize = cli->Read(buff, 2048)) > 0)
				{
					mstm->Write(buff, readSize);
				}
				if (mstm->GetLength() == 0)
				{
					console->WriteLineC(UTF8STRC("Received 0 bytes from server"));
				}
				else
				{
					Text::String *fileName = 0;
					sb.ClearStr();
					sb.AppendC(url, urlLen);
					j = sb.IndexOf('?');
					if (j != INVALID_INDEX)
					{
						sb.TrimToLength((UOSInt)j);
					}
					if (sb.EndsWith('/'))
					{
						sb.TrimToLength(sb.GetLength() - 1);
					}
					j = Text::StrLastIndexOfCharC(sb.ToString(), sb.GetLength(), '/');
					if (j != INVALID_INDEX && j >= 8)
					{
						fileName = Text::String::New(sb.ToString() + j + 1, sb.GetLength() - j - 1);
					}
					else
					{
						fileName = Text::String::New(UTF8STRC("index.html"));
					}
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Received "));
					sb.AppendU64(mstm->GetLength());
					sb.AppendC(UTF8STRC(" bytes from server"));
					console->WriteLineC(sb.ToString(), sb.GetLength());

/*					IO::FileStream *fs;
					UInt8 *fileBuff;
					NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					fileBuff = mstm->GetBuff(&argc);
					argc = fs->Write(fileBuff, argc);
					DEL_CLASS(fs);
					if (argc != mstm->GetLength())
					{
						console->WriteLineC(UTF8STRC("Error in writing to file"));
					}*/
					fileName->Release();
				}
				DEL_CLASS(mstm);
			}
			DEL_CLASS(cli);	
			SDEL_CLASS(ssl);
			DEL_CLASS(sockf);
		}
		else
		{
			console->WriteLineC(UTF8STRC("Only support http url"));
		}
		if (fileBuff)
		{
			MemFree(fileBuff);
			fileBuff = 0;
		}
	}

	DEL_CLASS(console);
	return 0;
}
