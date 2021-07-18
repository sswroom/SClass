#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/HTTPClient.h"
#include "Net/MIME.h"
#include "Net/OSSocketFactory.h"
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
		console->WriteLine((const UTF8Char*)"Usage: SHTTPPost [URL] [File]");
	}
	else
	{
		const UTF8Char *url = argv[1];
		const UTF8Char *file = argv[2];

		OSInt i;
		OSInt j;
		UInt8 *fileBuff = 0;
		UOSInt fileSize = 0;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(file, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		fileSize = fs->GetLength();
		if (fileSize > 0)
		{
			fileBuff = MemAlloc(UInt8, fileSize);
			if (fs->Read(fileBuff, fileSize) != fileSize)
			{
				console->WriteLine((const UTF8Char*)"Error in reading file");
				fileSize = 0;
				MemFree(fileBuff);
				fileBuff = 0;
			}
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Error in opening file");
		}
		DEL_CLASS(fs);

		if (Text::StrStartsWith(url, (const UTF8Char*)"http://") && fileBuff && fileSize > 0)
		{
			const UTF8Char *mime = 0;
			i = Text::StrLastIndexOf(file, '.');
			if (i >= 0)
			{
				mime = Net::MIME::GetMIMEFromExt(&file[i + 1]);
			}
			Text::StringBuilderUTF8 sb;
			Int32 httpStatus;
			UOSInt totalSize = 0;
			UOSInt writeSize;
			Net::SocketFactory *sockf;
			Net::HTTPClient *cli;
			NEW_CLASS(sockf, Net::OSSocketFactory(true));
			cli = Net::HTTPClient::CreateConnect(sockf, url, "POST", false);
			if (mime)
			{
				cli->AddHeader((const UTF8Char*)"Content-Type", mime);
			}
			Text::StrUOSInt((UTF8Char*)buff, fileSize);
			cli->AddHeader((const UTF8Char*)"Content-Length", (const UTF8Char*)buff); 

			while (totalSize < fileSize)
			{
				writeSize = cli->Write(&fileBuff[totalSize], fileSize - totalSize);
				if (writeSize <= 0)
				{
					console->WriteLine((const UTF8Char*)"Error in uploading to server");
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
				console->WriteLine((const UTF8Char*)"Error in requesting to server");
			}
			else
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Server response ");
				sb.AppendI32(httpStatus);
				console->WriteLine(sb.ToString());

				IO::MemoryStream *mstm;
				NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SHTTPGet.mstm"));
				UOSInt readSize;

				while ((readSize = cli->Read(buff, 2048)) > 0)
				{
					mstm->Write(buff, readSize);
				}
				if (mstm->GetLength() == 0)
				{
					console->WriteLine((const UTF8Char*)"Received 0 bytes from server");
				}
				else
				{
					const UTF8Char *fileName = 0;
					sb.ClearStr();
					sb.Append(url);
					j = sb.IndexOf('?');
					if (j >= 0)
					{
						sb.TrimToLength((UOSInt)j);
					}
					if (sb.EndsWith('/'))
					{
						sb.TrimToLength(sb.GetLength() - 1);
					}
					j = Text::StrLastIndexOf(sb.ToString(), '/');
					if (j >= 8)
					{
						fileName = Text::StrCopyNew(sb.ToString() + j + 1);
					}
					else
					{
						fileName = Text::StrCopyNew((const UTF8Char*)"index.html");
					}
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Received ");
					sb.AppendU64(mstm->GetLength());
					sb.Append((const UTF8Char*)" bytes from server");
					console->WriteLine(sb.ToString());

/*					IO::FileStream *fs;
					UInt8 *fileBuff;
					NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
					fileBuff = mstm->GetBuff(&argc);
					argc = fs->Write(fileBuff, argc);
					DEL_CLASS(fs);
					if (argc != mstm->GetLength())
					{
						console->WriteLine(L"Error in writing to file");
					}*/
					Text::StrDelNew(fileName);
				}
				DEL_CLASS(mstm);
			}
			DEL_CLASS(cli);	
			DEL_CLASS(sockf);
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Only support http url");
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
