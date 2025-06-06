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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter *console;
	UOSInt argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	UInt8 buff[2048];
	NEW_CLASS(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc <= 2)
	{
		console->WriteLine(CSTR("Usage: SHTTPPost [URL] [File]"));
	}
	else
	{
		UnsafeArray<const UTF8Char> url = argv[1];
		UOSInt urlLen = Text::StrCharCnt(url);
		UnsafeArray<const UTF8Char> file = argv[2];

		UOSInt i;
		UOSInt j;
		UInt8 *fileBuff = 0;
		UOSInt fileSize = 0;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream({file, Text::StrCharCnt(file)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fileSize = (UOSInt)fs->GetLength();
		if (fileSize > 0)
		{
			fileBuff = MemAlloc(UInt8, fileSize);
			if (fs->Read(Data::ByteArray(fileBuff, fileSize)) != fileSize)
			{
				console->WriteLine(CSTR("Error in reading file"));
				fileSize = 0;
				MemFree(fileBuff);
				fileBuff = 0;
			}
		}
		else
		{
			console->WriteLine(CSTR("Error in opening file"));
		}
		DEL_CLASS(fs);

		if (Text::StrStartsWithC(url, urlLen, UTF8STRC("http://")) && fileBuff && fileSize > 0)
		{
			Text::CString mime = CSTR_NULL;
			Text::CStringNN nnmime;
			UOSInt fileLen = Text::StrCharCnt(file);
			i = Text::StrLastIndexOfCharC(file, fileLen, '.');
			if (i != INVALID_INDEX)
			{
				mime = Net::MIME::GetMIMEFromExt(Text::CStringNN(&file[i + 1], fileLen - i - 1));
			}
			Text::StringBuilderUTF8 sb;
			Int32 httpStatus;
			UOSInt totalSize = 0;
			UOSInt writeSize;
			Optional<Net::SSLEngine> ssl;
			NN<Net::HTTPClient> cli;
			Net::OSSocketFactory sockf(true);
			Net::TCPClientFactory clif(sockf);
			ssl = Net::SSLEngineFactory::Create(clif, true);
			cli = Net::HTTPClient::CreateConnect(clif, ssl, {url, urlLen}, Net::WebUtil::RequestMethod::HTTP_POST, false);
			if (mime.SetTo(nnmime))
			{
				cli->AddContentType(nnmime);
			}
			cli->AddContentLength(fileSize); 

			while (totalSize < fileSize)
			{
				writeSize = cli->Write(Data::ByteArrayR(&fileBuff[totalSize], fileSize - totalSize));
				if (writeSize <= 0)
				{
					console->WriteLine(CSTR("Error in uploading to server"));
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
				console->WriteLine(CSTR("Error in requesting to server"));
			}
			else
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Server response "));
				sb.AppendI32(httpStatus);
				console->WriteLine(sb.ToCString());

				IO::MemoryStream mstm;
				UOSInt readSize;

				while ((readSize = cli->Read(BYTEARR(buff))) > 0)
				{
					mstm.Write(Data::ByteArrayR(buff, readSize));
				}
				if (mstm.GetLength() == 0)
				{
					console->WriteLine(CSTR("Received 0 bytes from server"));
				}
				else
				{
					NN<Text::String> fileName;
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
					sb.AppendU64(mstm.GetLength());
					sb.AppendC(UTF8STRC(" bytes from server"));
					console->WriteLine(sb.ToCString());

/*					IO::FileStream *fs;
					UInt8 *fileBuff;
					NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					fileBuff = mstm->GetBuff(&argc);
					argc = fs->Write(fileBuff, argc);
					DEL_CLASS(fs);
					if (argc != mstm->GetLength())
					{
						console->WriteLine(CSTR("Error in writing to file"));
					}*/
					fileName->Release();
				}
			}
			cli.Delete();	
			ssl.Delete();
		}
		else
		{
			console->WriteLine(CSTR("Only support http url"));
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
